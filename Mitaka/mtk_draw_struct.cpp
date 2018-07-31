/**************************************************************************************************

Copyright (c) 2007-2016   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#include  "mitaka.h"
#include  "mtk_dome_master.h"
#include  "mtk_mode.h"
#include  "mtk_draw_struct.h"
#include  "mtk_switches.h"
#include  "mtk_camera.h"
#include  "mtk_camera_setting.h"
#include  "mtk_target.h"
#include  "mtk_planets.h"
#include  "../common/astro/astro_const.h"
#include  "mtk_planetarium.h"
#include  "mtk_planet_orientation.h"
#include  "mtk_satellites.h"
#include  "mtk_stereo.h"

#include  "mtk_objects.h"
#include  "mtk_spacecraft.h"
#include  "mtk_sun.h"


void
mtk_draw_struct::set(int mode, int submode, const mtk_camera& _camera_base, const double& _JD, float scrn_aspct_ratio, int _vp_x1, int _vp_x2, int _vp_y1, int _vp_y2, const float font_size, int target_ID, const vector3d& target_pos, vector3f dr, bool bRotBB, float line_w)
{
	tyMode			= mode;
	tySubMode		= submode;

	JD				= _JD;


	camera_base0 	= _camera_base;
	camera_base 	= _camera_base;
	camera			= camera_base;

	tgt_ID			= target_ID;
	tgt_pos			= target_pos;
	
	
	scale_factor	= camera.get_scale_factor();
	scale			= get_clst_R() / scale_factor;
	plnt_R			= get_plnt_R();
	plnt_angle		= get_plnt_angle();
	plnt_d			= get_plnt_d();

	if (get_display_mode() == DISPMODE_DOME_MASTER) {
		plnt_R = get_plnt_R0();
		plnt_d = 0;
	}


	// viewport
	vp_x1			= _vp_x1;
	vp_x2			= _vp_x2;
	vp_y1			= _vp_y1;
	vp_y2			= _vp_y2;
	vp_w			= vp_x2 - vp_x1;
	vp_h			= vp_y2 - vp_y1;

	// screen aspect ration
	screen_aspect_ratio = scrn_aspct_ratio;



	//***
	if (!isFrustumLoaded()) {
		eye_offset = dr;

		set_eye_offset_x(dr.x);
		set_local_camera_pos(eye_offset);
	}
	else {
		eye_offset.set(0, 0, 0);
	}
	//***


	// frustum
	//*****
	frs_near = get_frustum_near(screen_aspect_ratio);
	frs_far = get_frustum_far(screen_aspect_ratio);
	bNear = true;


	// radian per pixel
	set_red_per_pixel();




	// 文字サイズ
	ltf				= 1.2f * font_size;
	if (get_display_mode() == DISPMODE_DOME_MASTER) {
		ltf *= get_dome_master_label_mag();
	}

	// 線の太さ
	line_width_fac = line_w;



	bRotateBillboard = bRotBB;


	//--------------
	//  カメラの設定
	//--------------
	vector3d  eF, eU, eR;
	quaterniond  qr;


	if (tyMode == MODE_3D) {

		//---  地平線モード （惑星と衛星のみ） ---
		const mtk_object&  tgt = get_object(tgt_ID);
		const int tgt_ty = tgt.get_type();
		
		if ((tgt_ty == OBJ_TYPE_PLANET || tgt_ty == OBJ_TYPE_SATELLITE) && get_landing_mode() == LANDING_MODE_HORIZONTAL) {

			double Re;
			if (tgt_ty == OBJ_TYPE_SATELLITE) {
				double Rx, Ry, Rz;
				get_satellite_size(tyMode, tgt.get_index(), Rx, Ry, Rz);
				Re = Rx;
			}
			else {
				Re = get_planet_Re(tyMode, tgt.get_index());
			}

			const double R = Re * _1km_pc;
			const double Rc = 2*R;
			const double R1 = 1.002*R;
			const double L = norm(camera_base.get_pos() - tgt_pos);
			
			//
			const double th_max = (90.0 + 5.0) * D2R;

			if (L < Rc) {
				const double fac = (L < R1)  ?  1  :  (asin(R1/L) - asin(R1/Rc)) / (asin(1.0) - asin(R1/Rc));
				double a;
				if (fac > 0.5) {
					a = (1 - fac)/0.5;
					a = 1 - 0.5*a*a;
				} else {
					a = fac/0.5;
					a = 0.5*a*a;
				}
				const double th = th_max * a;

				camera_base.get_axes(eF, eU, eR);
				qr = make_rotation_quaternion(th, eR);
				camera_base.set_ori( qr * camera_base.get_ori() );
			}
		}
	}


	camera = camera_base;
	camera.set_ori( camera.get_ori() * get_local_camera_orientation() );
	camera.get_axes(eF, eU, eR);


	//ビルボードを回転させない場合の辺の向き
	eRf = vector3f(eR);
	eUf = vector3f(eU);


	//----------------------------
	//  黄道座標から視点座標への変換
	//----------------------------
	matrix3d  Moc, Mbc, Mlc, Mtot;
	Moc = quaternion_to_matrix3( camera_base.get_ori().inv() );				// ec to obj
	Mbc = quaternion_to_matrix3( get_base_camera_ori().inv() );				// obj to base
	Mlc = quaternion_to_matrix3( get_local_camera_orientation().inv() );	// base to local
	M_view = Mlc * Mbc * Moc;
	M_view_inv = transpose(M_view);
	M_view *= scale_factor;
	M_view_inv /= scale_factor;


	// 視点座標系での黄道座標系の原点座標を求める
	vector3d  ro;
	Moc *= scale_factor;
	ro = -Moc * camera.get_pos();					// world -> object
	ro = Mbc * (ro);								// objct -> base
	ro = Mlc * (ro - get_local_camera_pos());		// base -> local
	r_view = ro;


	//  World 座標での視錐台の平面を計算する
	calc_frustum_planes_on_world(frs_near, nv_near, D_near);
	calc_frustum_planes_on_world(frs_far, nv_far, D_far);


	cnorm = norm( camera.get_pos() );
}


void
mtk_draw_struct::set_red_per_pixel()
{
	// radian per pixel
	float rpp1, rpp2;
	rpp1 = (frs_near.get_x2() - frs_near.get_x1()) / (frs_near.get_z1() * vp_w);
	rpp2 = (frs_near.get_y2() - frs_near.get_y1()) / (frs_near.get_z1() * vp_h);

	//rad_per_pixel = (rpp1 < rpp2) ? rpp1 : rpp2;
	rad_per_pixel = 0.5f * (rpp1 + rpp2);

	//***
	rad_per_pixel /= get_mag_factor();
}


vector2f
mtk_draw_struct::calc_pos_on_screen(const vector3d& pos) const
{
	const vector3d rv = calc_pos_on_viewframe(pos);

	const float x1 = frs_near.get_x1();
	const float x2 = frs_near.get_x2();
	const float y1 = frs_near.get_y1();
	const float y2 = frs_near.get_y2();
	const float fac = -frs_near.get_z1() / rv.z;

	float x, y;
	x = (fac * rv.x - x1) / (x2 - x1);
	y = (fac * rv.y - y1) / (y2 - y1);

	x = vp_w * x +vp_x1;
	y = vp_h * y +vp_y1;

	return  vector2f(x, y);
}



//-------------------------------
//  World 座標での視錐台の平面を得る
//-------------------------------
void
mtk_draw_struct::calc_frustum_planes_on_world(const frustum& frs, vector3d* nv, double* D) const
{
	vector3f _nv[6];
	float _D[6];

	frs.get_planes(_nv, _D);	// 視点座標系での視錐台平面

								// 平面の変換
	vector3d _r = camera.get_pos();
	matrix3d _M = transpose(M_view);
	for (int face = 0; face < 6; face++) {
		vector3d v;
		float vnorm;

		v = _M * _nv[face];
		vnorm = norm(v);

		nv[face] = v / vnorm;
		D[face] = _D[face] / vnorm - dot_product(_r, nv[face]);
	}
}


void
mtk_draw_struct::recalc_frustum_planes_on_world()
{
	//  World 座標での視錐台の平面を計算する
	calc_frustum_planes_on_world(frs_near, nv_near, D_near);
	calc_frustum_planes_on_world(frs_far, nv_far, D_far);
}

void
mtk_draw_struct::get_frustum_planes_on_world(const vector3d* &nv, const double* &D) const
{
	if (bNear) {
		nv = nv_near;
		D = D_near;
	}
	else {
		nv = nv_far;
		D = D_far;
	}
}

