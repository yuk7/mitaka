/**************************************************************************************************

Copyright (c) 2007,2015   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

以下に定める条件に従い、本ソフトウェアおよび関連文書のファイル（以下「ソフトウェア」）の複製を取得す
るすべての人に対し、ソフトウェアを無制限に扱うことを無償で許可します。これには、ソフトウェアの複製を
使用、複写、変更、結合、掲載、頒布、サブライセンス、および/または販売する権利、およびソフトウェアを
提供する相手に同じことを許可する権利も無制限に含まれます。

上記の著作権表示および本許諾表示を、ソフトウェアのすべての複製または重要な部分に記載するものとしま
す。

ソフトウェアは「現状のまま」で、明示であるか暗黙であるかを問わず、何らの保証もなく提供されます。ここ
でいう保証とは、商品性、特定の目的への適合性、および権利非侵害についての保証も含みますが、それに限定
されるものではありません。 作者または著作権者は、契約行為、不法行為、またはそれ以外であろうと、ソフ
トウェアに起因または関連し、あるいはソフトウェアの使用またはその他の扱いによって生じる一切の請求、損
害、その他の義務について何らの責任も負わないものとします。
***************************************************************************************************/
#include  "mtk_object.h"
#include  "mtk_mode.h"
#include  "mtk_object_position.h"
#include  "mtk_solar_system.h"
#include  "mtk_sun.h"
#include  "mtk_planets.h"
#include  "mtk_satellites.h"
#include  "mtk_planet_position.h"
#include  "mtk_planet_orientation.h"
#include  "mtk_minor_planets.h"
#include  "mtk_spacecraft.h"
#include  "mtk_oort_cloud.h"
#include  "mtk_fixed_object.h"
#include  "mtk_stars.h"
#include  "mtk_stars_2.h"
#include  "mtk_galactic_center.h"
#include  "mtk_blackhole.h"
#include  "mtk_globular_clusters.h"
#include  "mtk_our_galaxy.h"
#include  "mtk_nearby_galaxies.h"
#include  "mtk_galaxies.h"	//**** old
#include  "mtk_spacecraft.h"
#include  "mtk_label.h"
#include  "mtk_scale_line.h"
#include  "mtk_switches.h"
#include  "mtk_calc_alpha.h"
#include  "mtk_VERA.h"
#include  "mtk_draw_common.h"
#include  "mtk_strings.h"
#include  "mtk_target.h"



const float HITTEST_MIN_RADIUS = 5.0;	// ターゲット選択時の最小ピクセル数



void
mtk_object::init(int type, int idx, float dz, bool bTargetable)
{
	m_type	= type;
	m_idx	= idx;
	m_dz	= dz;
	m_bTgt	= bTargetable; 
}


vector3d
mtk_object::get_pos() const
{
	return  get_object_positon( *this );
}


vector3d
mtk_object::get_parent_pos() const
{
	switch (m_type) {
		case OBJ_TYPE_SUN:
		case OBJ_TYPE_S_STAR:
			return  get_our_galaxy_position();

		case OBJ_TYPE_SATELLITE:
			return  get_planet_position( get_satellite_planet_index(m_idx)  ); 

		case OBJ_TYPE_PLANET:
		default:
			return  vector3d(0,0,0);
	}
}



//------------------
//  Landing radius
//------------------
double
mtk_object::get_landing_radius()  const
{
	switch (m_type) {
		case OBJ_TYPE_SUN:				return  get_landing_radius_sun();
		case OBJ_TYPE_PLANET:			return  get_landing_radius_planet(m_idx);
		case OBJ_TYPE_SATELLITE:		return  get_landing_radius_satellite(m_idx);
		case OBJ_TYPE_SMALL_OBJECT:		return  get_landing_radius_small_object(m_idx);
		case OBJ_TYPE_SPACECRAFT:		return  get_landing_radius_spacecraft(m_idx);
		case OBJ_TYPE_STAR:				return  get_landing_radius_star(m_idx);
		case OBJ_TYPE_BLACKHOLE:		return  get_landing_radius_blackhole();
		case OBJ_TYPE_GLOBULAR_CLUSTER:	return  get_landing_radius_globular_cluster(m_idx);
		case OBJ_TYPE_OUR_GALAXY:		return  get_landing_radius_our_galaxy(m_idx);
		case OBJ_TYPE_NEARBY_GALAXY:	return  get_landing_radius_our_galaxy(m_idx);
		//case OBJ_TYPE_OORT_CLOUD:
		//case OBJ_TYPE_SCALE_CIRCLE:
		//case OBJ_TYPE_LABEL:
		default:
			return  0.0;
	}
}



//----------------
//  Prepare draw
//----------------
int
mtk_object::prepare_draw(const mtk_draw_struct *v_ds, int Nds, const float z_near, const float z_mid, const float z_far)
{
	int draw_rgn;
	float alp = 1.0;

	const mtk_draw_struct& ds = v_ds[0];

	hit_test_status = OBJ_HITTEST_STAT_NONE;



	bool bFlg = true;
	switch (m_type) {
		case OBJ_TYPE_SUN:				bFlg = prepare_draw_sun(ds, &m_Rb, &m_Rh);							break;
		case OBJ_TYPE_PLANET:			bFlg = prepare_draw_planet(ds, m_idx, &m_Rb, &m_Rh);				break;
		case OBJ_TYPE_SATELLITE:		bFlg = prepare_draw_satellite(ds, m_idx, &m_Rb, &m_Rh);				break;
		case OBJ_TYPE_SMALL_OBJECT:		bFlg = prepare_draw_small_object(ds, m_idx, &m_Rb, &m_Rh);			break;
		//case OBJ_TYPE_COMET:			bFlg = prepare_draw_comets(ds, m_idx, &m_Rb, &m_Rh);				break;
		case OBJ_TYPE_SPACECRAFT:		bFlg = prepare_draw_spacecraft(ds, m_idx, &m_Rb, &m_Rh);			break;
		case OBJ_TYPE_OORT_CLOUD:		bFlg = prepare_draw_Oort_cloud(ds, m_idx, &m_Rb, &m_Rh);			break;
		case OBJ_TYPE_STAR:				bFlg = prepare_draw_star(ds, m_idx, &m_Rb, &m_Rh);					break;
		case OBJ_TYPE_S_STAR:			bFlg = prepare_draw_S_star(ds, m_idx, &m_Rb, &m_Rh);				break;
		case OBJ_TYPE_BLACKHOLE:		bFlg = prepare_draw_blackhole(ds, &m_Rb, &m_Rh);					break;
		case OBJ_TYPE_GLOBULAR_CLUSTER:	bFlg = prepare_draw_globular_cluster(ds, m_idx, &m_Rb, &m_Rh);		break;
		case OBJ_TYPE_VERA_OBJECT:		bFlg = prepare_draw_VERA_object(ds, m_idx, &m_Rb, &m_Rh);			break;
		case OBJ_TYPE_OUR_GALAXY:		bFlg = prepare_draw_our_galaxy(ds, &m_Rb, &m_Rh);					break;
		case OBJ_TYPE_NEARBY_GALAXY:	bFlg = prepare_draw_nearby_galaxy(ds, m_idx, &m_Rb, &m_Rh, &alp);	break;
		case OBJ_TYPE_LABEL:			bFlg = prepare_draw_label(ds, m_idx, &m_Rb, &m_Rh, &alp);					break;

		case OBJ_TYPE_SCALE_CIRCLE:		bFlg = false;	break;	//*****
		case OBJ_TYPE_SCALE_CIRCLE_G:	bFlg = false;	break;	//*****
		default:						bFlg = false;	break;	//****
	}

	if (!bFlg)  return OBJ_DRAW_NONE;
	

	//****
	//--  オブジェクトのカメラからの相対位置
	const vector3d  pos = get_pos();
	const vector3d  cpos = ds.camera_base.get_pos();
	rpos = pos - cpos;
	const double l2 = rpos.norm2();

	//---  Draw Size  ---
	double  Rb = 1.1 * m_Rb;	// 少し大きめに取る
	double  rpp = ds.rad_per_pixel;
	double  Dth2 = 4.0 * Rb*Rb/l2;
	double  psz2 = Dth2 / (rpp*rpp);
	double  psz0 = 1.27;
	double  psz02 = psz0*psz0;




	alpha = 1.0f;
	if (m_type == OBJ_TYPE_STAR && psz2 < psz02) {
		if ( tyStarMark == STARMARK_EXOPLANETS  && get_star(m_idx).bPlanet ) {
			if (psz2 < 0.1)  return  OBJ_DRAW_NONE;
			alpha = psz2 / psz02;
			psz2 = psz02;
		} else {
			return  OBJ_DRAW_NONE;
		}
	}

	if (m_type == OBJ_TYPE_S_STAR && psz2 < psz02) {
		return  OBJ_DRAW_NONE;
	}




	const float alpha_thr = 0.1f;
	if (psz2 < psz02) {
		switch (m_type) {
			case OBJ_TYPE_STAR:
			case OBJ_TYPE_OORT_CLOUD:
				return OBJ_DRAW_NONE;
			default:
				break;
		}

		if (m_type == OBJ_TYPE_SUN) {
			draw_status = OBJ_DRAW_STAT_REALSIZE;
		} else {
			draw_status = OBJ_DRAW_STAT_POINT;

			const vector3d  ppos = get_parent_pos();
			double Dth, R, l;
			R = norm( pos - ppos );
			l = norm( ppos - cpos );
			Dth = 2.0 * R / l;
			alpha = calc_alpha_by_apparent_diameter( Dth );

			if (m_type == OBJ_TYPE_NEARBY_GALAXY) {
				alpha *= alp;
			}
			if (m_type == OBJ_TYPE_SMALL_OBJECT) {
				alpha *= get_minor_planet_point_alpha(ds, m_idx);
			}

			if (m_type == OBJ_TYPE_S_STAR) {
				alpha *= get_S_star_alpha(ds, m_idx);
			}

			if (m_type == OBJ_TYPE_BLACKHOLE) {
				alpha *= get_blackhole_point_alpha(ds);
			}


			if (alpha < alpha_thr)  return  OBJ_DRAW_NONE;
		}
	} else {
		draw_status = OBJ_DRAW_STAT_REALSIZE;
	}


	if (m_type == OBJ_TYPE_LABEL) {
		alpha = alp;
	}





	//-----------
	//  Culling
	//-----------
	vector3d rv = ds.calc_pos_on_viewframe( pos );
	double z = -rv.z;
	
	double Rb0;
	Rb0 = Rb;
	const double sf = ds.scale_factor;
	Rb *= sf;



	if (z + Rb < z_near)  return  OBJ_DRAW_NONE;
	
	if (m_type == OBJ_TYPE_BLACKHOLE && z < 0) {
		return  OBJ_DRAW_NONE;
	}
	
	
	

	if (m_type == OBJ_TYPE_BLACKHOLE) {
		Rb0 = 0;
	}
	


	//if (m_type != OBJ_TYPE_STAR) {
	{
		bool bOutside = true;
		for (int j = 0; j < Nds; j++) {

			const vector3d* nv;
			const double *D;
			v_ds[j].get_frustum_planes_on_world(nv, D);


			bool bInside = true;
			for (int i = 0; i < 4; i++) {

				double d = dot_product(pos, nv[i]) + D[i];

				if (d > Rb0) {
					bInside = false;
					break;
				}
			}
			if (bInside) {
				bOutside = false;
				break;
			}
		}
		if (bOutside)  return  OBJ_DRAW_NONE;
	}





	//--  z_far より遠いものは置きなおす
	double l = sf * sqrt(l2);
	scl = 1.0;
	const double l0 = z_far * 0.9;
	if (l > l0) {
		scl = l0 / l;
		rpos *= scl;
		z *= scl;
		Rb *= scl;
		l = l0;
	}


	//--  Region
	bool bNear, bFar;
	bNear = bFar = false;
	if (z - Rb < z_mid)	{ bNear = true; }
	if (z + Rb > z_mid)	{ bFar  = true; }

	if (bNear) {
		if (bFar)	draw_rgn = OBJ_DRAW_BOTH;
		else		draw_rgn = OBJ_DRAW_NEAR;
	} else if (bFar) {
		draw_rgn = OBJ_DRAW_FAR;
	} else {
		return  OBJ_DRAW_NONE;
	}


	//---  for z-list
	draw_z = z;


	//---  Hit-test  ---
	float  tgtc = alpha;

	if (m_type == OBJ_TYPE_STAR) {
		tgtc = 0.2 * psz2 / 2.0;
	}


	if ( !m_bTgt || tgtc < 0.5 ) {
		hit_test_status = OBJ_HITTEST_STAT_NONE;
		return  draw_rgn;
	}


	//---  Radius
	if (m_type != OBJ_TYPE_LABEL) {
		double  Rh = sf * m_Rh;
		double  Dth, psz;
		Dth = 2.0 * Rh / l;
		psz = Dth / ds.rad_per_pixel;
		hit_test_status  =  (psz > HITTEST_MIN_RADIUS)  ?  OBJ_HITTEST_STAT_REALSIZE  :  OBJ_HITTEST_STAT_POINT;
	} else {
		hit_test_status = OBJ_HITTEST_STAT_REALSIZE;
	}

	return  draw_rgn;
}



//---------
//  Scale
//---------
double
mtk_object::get_scale()  const
{
	switch (m_type) {
		case OBJ_TYPE_SUN:				return  get_scale_sun();
		case OBJ_TYPE_PLANET:			return  get_scale_planet(m_idx);
		case OBJ_TYPE_SATELLITE:		return  get_scale_satellite(m_idx);
		case OBJ_TYPE_SMALL_OBJECT:		return  get_scale_small_object(m_idx);
		//case OBJ_TYPE_COMET:			return  get_scale_comet(m_idx);
		case OBJ_TYPE_STAR:				return  get_scale_star(m_idx);
		case OBJ_TYPE_S_STAR:			return  get_scale_S_star(m_idx);
		case OBJ_TYPE_BLACKHOLE:		return  get_scale_blackhole();
		case OBJ_TYPE_GLOBULAR_CLUSTER:	return  get_scale_globular_cluster(m_idx);
		case OBJ_TYPE_VERA_OBJECT:		return  get_scale_VERA_object(m_idx);
		case OBJ_TYPE_OUR_GALAXY:		return  get_scale_our_galaxy();
		case OBJ_TYPE_NEARBY_GALAXY:	return  get_scale_nearby_galaxy(m_idx);
		case OBJ_TYPE_SPACECRAFT:		return  get_scale_spacecraft(m_idx);
			break;
		default:
			break;
	}

	return  1000 * _1km_pc;	//****
}



//---------
//  Label
//---------
bool
is_object_label_visible(const mtk_draw_struct& ds, const mtk_object& obj)
{
	if (ds.tySubMode==SUBMODE_IMAGE || ds.tySubMode==SUBMODE_3DCHART)  return false;

	const mtk_object& tgt = get_target();
	bool bTgt = ((tgt.get_type() == obj.get_type()) && (tgt.get_index() == obj.get_index()));


	const int obj_type = obj.get_type();
	const int idx = obj.get_index();
	switch (obj_type) {
		case OBJ_TYPE_SUN:				return  is_sun_label_visible();
		case OBJ_TYPE_PLANET:			return  is_planet_label_visible(idx);
		case OBJ_TYPE_SATELLITE:		return  is_satellite_label_visible(idx, bTgt);
		
		case OBJ_TYPE_SMALL_OBJECT:		return  is_small_object_label_visible(idx, bTgt);
		
		//case OBJ_TYPE_COMET:			return  is_comet_label_visible(idx);
		case OBJ_TYPE_OORT_CLOUD:		return  is_Oort_cloud_label_visible();
		case OBJ_TYPE_STAR:				return  is_star_label_visible(ds, idx);
		case OBJ_TYPE_S_STAR:			return	is_S_star_label_visible(ds, idx, bTgt);
		case OBJ_TYPE_BLACKHOLE:		return	is_blackhole_label_visible(ds);
		case OBJ_TYPE_VERA_OBJECT:		return	is_VERA_label_visible(ds, idx, bTgt);
		case OBJ_TYPE_GLOBULAR_CLUSTER:	return  is_globular_cluster_label_visible(ds, idx, bTgt);
		case OBJ_TYPE_NEARBY_GALAXY:	return  is_nearby_galaxy_label_visible(ds, idx, bTgt);
		case OBJ_TYPE_SPACECRAFT:		return  is_spacecraft_label_visible(ds, idx);

		case OBJ_TYPE_FIXED_OBJECT:		return  is_fixed_object_label_visible(ds, idx);
		case OBJ_TYPE_SCALE_CIRCLE:		return  is_scale_circle_label_visible( idx );
		case OBJ_TYPE_SCALE_CIRCLE_G:	return  is_scale_circle_label_visible(idx);

		default:						break;
	}

	return  false;
}


float
get_label_alpha(const mtk_draw_struct& ds, const mtk_object& obj)
{
	const int idx = obj.get_index();

	switch (obj.get_type()) {
		case OBJ_TYPE_SUN:				return  get_sun_label_alpha( ds );
		case OBJ_TYPE_PLANET:			return  get_planet_label_alpha( ds, idx );
		case OBJ_TYPE_SATELLITE:		return  get_satellite_label_alpha( ds, idx );
		case OBJ_TYPE_SMALL_OBJECT:		return  get_small_object_label_alpha( ds, idx );
		//case OBJ_TYPE_COMET:			return  get_comet_label_alpha( ds, idx );
		case OBJ_TYPE_OORT_CLOUD:		return  get_Oort_cloud_label_alpha( ds );
		case OBJ_TYPE_STAR:				return  get_star_label_alpha(ds, idx);
		case OBJ_TYPE_S_STAR:			return  get_S_star_label_alpha(ds, idx);
		case OBJ_TYPE_BLACKHOLE:		return	get_blackhole_label_alpha(ds);
		case OBJ_TYPE_VERA_OBJECT:		return	get_VERA_label_alpha(ds, idx);
		case OBJ_TYPE_GLOBULAR_CLUSTER:	return  get_globular_cluster_label_alpha( ds, idx );
		case OBJ_TYPE_NEARBY_GALAXY:	return  get_nearby_galaxy_label_alpha(ds, idx);
		
		case OBJ_TYPE_SPACECRAFT:		return  get_spacecraft_label_alpha( ds, idx );
		case OBJ_TYPE_FIXED_OBJECT:		return	get_fixed_object_label_alpha( ds, idx );
		case OBJ_TYPE_SCALE_CIRCLE:		return  get_scale_circle_alpha( ds, idx );
		case OBJ_TYPE_SCALE_CIRCLE_G:	return  get_scale_circle_g_alpha(ds, idx);
		default:						break;
	}

	return  1.0f;
}




//--------
//  Draw
//--------
void
mtk_object::draw(const mtk_draw_struct& ds, bool bSelect, bool bChangeObjType)  const
{
	if (bChangeObjType && m_type != OBJ_TYPE_STAR) {
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		glDisable(GL_LIGHTING);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}


	if (draw_status == OBJ_DRAW_STAT_POINT) {
		switch (m_type) {
			//case OBJ_TYPE_SMALL_OBJECT:		return;
			case OBJ_TYPE_SMALL_OBJECT:		draw_a_small_object_point(ds, m_idx, rpos, alpha);		return;
			case OBJ_TYPE_NEARBY_GALAXY:	draw_a_nearby_galaxy_as_point(ds, m_idx, rpos, alpha);	return;
			default:	break;
		}


		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glEnable(GL_BLEND);

		if (m_type == OBJ_TYPE_GLOBULAR_CLUSTER || m_type == OBJ_TYPE_NEARBY_GALAXY || m_type == OBJ_TYPE_VERA_OBJECT) {
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		} else {
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		glColor3f(alpha, alpha, alpha);


		if (true) {
			double R2, R;
			R2 = norm2(rpos);
			if (R2 < 1e-100)  return;
			R = sqrt(R2);

			draw_point(ds, 0.0018, R, rpos, ds.eRf, ds.eUf);
		} else {
			glPointSize(2);
			glBegin(GL_POINTS);
				glVertex3dv( rpos.get_pointer() );
			glEnd();
		}
	} else {
		switch (m_type) {
			case OBJ_TYPE_SUN:				draw_sun(ds, 0, rpos, scl);										break;
			case OBJ_TYPE_PLANET:			draw_a_planet(ds, m_idx, rpos, scl);							break;
			case OBJ_TYPE_SATELLITE:		draw_a_satellite(ds, m_idx, rpos, scl);							break;
			case OBJ_TYPE_SMALL_OBJECT:		draw_a_small_object(ds, m_idx, rpos, scl);						break;
			case OBJ_TYPE_SPACECRAFT:		draw_a_spacecraft(ds, m_idx, rpos, scl);						break;
			//case OBJ_TYPE_COMET:			draw_a_comet(ds, m_idx, rpos, scl);								break;
			case OBJ_TYPE_OORT_CLOUD:		draw_Oort_cloud(ds, 0, rpos, scl);								break;
			case OBJ_TYPE_STAR:				draw_a_star(ds, m_idx, rpos, scl, m_Rb, alpha, bChangeObjType);	break;
			case OBJ_TYPE_S_STAR:			draw_a_S_star(ds, m_idx, rpos, scl, m_Rb, alpha);				break;
			case OBJ_TYPE_BLACKHOLE:		draw_blackhole(ds, rpos, m_Rb, scl);							break;
			case OBJ_TYPE_GLOBULAR_CLUSTER:	draw_a_globular_cluster(ds, m_idx, rpos, scl);					break;
			case OBJ_TYPE_VERA_OBJECT:		draw_a_VERA_object(ds, m_idx, rpos, scl, alpha);				break;
			case OBJ_TYPE_OUR_GALAXY:		draw_our_galaxy(ds, 0, rpos, scl);								break;
			case OBJ_TYPE_NEARBY_GALAXY:	draw_a_nearby_galaxy(ds, m_idx, rpos, scl);						break;

			case OBJ_TYPE_LABEL:			draw_a_label(ds, m_idx, rpos, scl, alpha, m_dz);				break;
			default:	break;
		}
	}
}




//------------
//  Hit-test
//------------
bool
mtk_object::hit_test(const mtk_draw_struct& ds, int mx, int my, const vector3d& cpos, const vector3d& nv) const
{
	//---------------
	//  ラベルの場合
	//---------------
	if (m_type == OBJ_TYPE_LABEL) {
		return  hit_test_label(ds, m_idx, get_pos(), cpos, nv);
	}


	//----------------------------
	//  ポイントサイズで判定する場合
	//----------------------------
	if (hit_test_status == OBJ_HITTEST_STAT_POINT) {
		const vector2f  pos = ds.calc_pos_on_screen( get_pos() );

		int ix, iy;
		ix = int(pos.x + 0.5);
		iy = int(pos.y + 0.5);

		int dx, dy;
		dx = ix - mx;
		dy = iy - my;
		if (dx*dx + dy*dy < HITTEST_MIN_RADIUS * HITTEST_MIN_RADIUS) {
			return  true;
		} else {
			return  false;
		}
	}


	//-----------------------
	//  実サイズで判定する場合
	//-----------------------
	const vector3d pos  = get_pos();
	const vector3d rpos = cpos - pos;

	switch (m_type) {
		case  OBJ_TYPE_PLANET:			return  hit_test_planet(ds, m_idx, rpos, nv);
		case  OBJ_TYPE_SATELLITE:		return  hit_test_satellite(ds, m_idx, rpos, nv);
		case  OBJ_TYPE_SMALL_OBJECT:	return  hit_test_small_object(ds, m_idx, rpos, nv);
		case  OBJ_TYPE_OUR_GALAXY:		return  hit_test_our_galaxy(ds, rpos, nv);
		default:						break;
	}

	//--  オブジェクトとの交点を求める
	const double b = dot_product(nv, rpos);
	const double D = b*b - rpos.norm2() + m_Rh*m_Rh;

	if (D <= 0.0)  return false;  // 交点なし
	if (-b - sqrt(D) > 0.0)  return  true;

	return  false;
}





bool
is_object_orbit_visible(const mtk_object& obj)
{
	const int obj_type = obj.get_type();
	const int idx = obj.get_index();
	char dispsw_idx;


	switch (obj_type) {
		//--  Planets
		case OBJ_TYPE_PLANET:
			if (idx == 8)	dispsw_idx = DISPSW_TNO;
			else			dispsw_idx = DISPSW_PLANET;
			break;

		case OBJ_TYPE_SATELLITE:
			dispsw_idx = DISPSW_SATELLITE;
			break;
		
		//--  Small Objects
		case OBJ_TYPE_SMALL_OBJECT:
			{
				int type = get_small_object_type( idx );
				if (type == SMALLOBJ_TYPE_TNO || type == SMALLOBJ_TYPE_SDO)		dispsw_idx = DISPSW_TNO;
				else															dispsw_idx = DISPSW_MINOR_PLANET;
			}
			break;

		case OBJ_TYPE_S_STAR:
			if (!(swStar & STAR_DISP)) {
				return false;
			}
			if ((swStar & STAR_NAME_SELECT) && !is_S_star_selected(idx)) {
				return false;
			}
			if (!(swStar & STAR_ORBIT)) {
				return false;
			}
			return true;

		case OBJ_TYPE_SCALE_CIRCLE:
		case OBJ_TYPE_SCALE_CIRCLE_G:
			if (swScaleLine & SCALE_SQUARE && idx == 20) {
				return  true;
			}
			return  swScaleLine & SCALE_CIRCLE;

		default:
			return  true;
	}


	if ( !test_disp_switch( dispsw_idx, DISPM_DISP) || !test_disp_switch( dispsw_idx, DISPM_ORBIT ) )  return  false;


	if (obj_type == OBJ_TYPE_SATELLITE) {
		if (test_disp_switch(dispsw_idx, DISPM_SELECT) && !is_satellite_selected(idx)) {
			return  false;
		}
	}

	if (obj_type == OBJ_TYPE_SMALL_OBJECT) {
		if (test_disp_switch(dispsw_idx, DISPM_SELECT) && !is_small_object_selected(idx)) {
			return  false;
		}
	}


	return  true;
}




//---------
//  Alpha
//---------

float
get_orbit_alpha(const mtk_draw_struct& ds, const mtk_object& obj)
{
	int idx = obj.get_index();

	switch (obj.get_type()) {
		case OBJ_TYPE_PLANET:		return  get_planet_orbit_alpha( ds, idx );
		case OBJ_TYPE_SATELLITE:	return  get_satellite_orbit_alpha( ds, idx );
		case OBJ_TYPE_SMALL_OBJECT:	return  get_small_object_orbit_alpha( ds, idx );
		case OBJ_TYPE_S_STAR:		return  get_S_star_object_orbit_alpha(ds, idx );
		case OBJ_TYPE_SCALE_CIRCLE:		return  get_scale_circle_alpha( ds, idx );
		case OBJ_TYPE_SCALE_CIRCLE_G:	return  get_scale_circle_g_alpha(ds, idx);
		default:				break;
	}

	return  1.0f;
}






//--------
//  Info
//--------
void
mtk_object::make_object_info(const mtk_draw_struct& ds, std::vector< std::string >& text)  const
{
	text.clear();

	switch (m_type) {
		case OBJ_TYPE_SUN:				make_object_info_sun(ds, text);							break;
		case OBJ_TYPE_PLANET:			make_object_info_planet(ds, m_idx, text);				break;
		case OBJ_TYPE_SATELLITE:		make_object_info_satellite(ds, m_idx, text);			break;
		case OBJ_TYPE_SMALL_OBJECT:		make_object_info_small_object(ds, m_idx, text);			break;
		//case OBJ_TYPE_COMET:			make_object_info_comet(ds, m_idx, text);				break;
		case OBJ_TYPE_STAR:				make_object_info_star(ds, m_idx, text);					break;
		case OBJ_TYPE_S_STAR:			make_object_info_S_star(ds, m_idx, text);				break;
		case OBJ_TYPE_BLACKHOLE:		make_object_info_blackhole(ds, text);					break;
		case OBJ_TYPE_GLOBULAR_CLUSTER:	make_object_info_globular_cluster(ds, m_idx, text);		break;
		case OBJ_TYPE_VERA_OBJECT:		make_object_info_VERA_object(ds, m_idx, text);			break;
		case OBJ_TYPE_OUR_GALAXY:		make_object_info_our_galaxy(ds, text);					break;
		case OBJ_TYPE_NEARBY_GALAXY:	make_object_info_nearby_galaxy(ds, m_idx, text);		break;
		case OBJ_TYPE_SPACECRAFT:		make_object_info_spacecraft(ds, m_idx, text);			break;

		default:
			text.push_back(get_string("INFO_TYPE_UNDEFINED"));
			text.push_back(get_string("INFO_TYPE_UNDEFINED"));
			break;
	}
}
