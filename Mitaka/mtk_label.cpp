/**************************************************************************************************

Copyright (c) 2007-2015   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#include  <windows.h>
#include  "mtk_label.h"
#include  "mtk_mode.h"
#include  <vector>
#include  <string>
#include  "mtk_gl_common.h"
#include  "mtk_draw_struct.h"
#include  "mtk_font.h"
#include  "mtk_colors.h"
#include  "mtk_objects.h"
#include  "mtk_object_position.h"
#include  "mtk_planet_position.h"
#include  "mtk_solar_system.h"
#include  "mtk_sun.h"
#include  "mtk_planets.h"
#include  "mtk_minor_planets.h"
#include  "mtk_fixed_object.h"
#include  "mtk_stars_2.h"
#include  "mtk_switches.h"
#include  "mtk_planetarium.h"
#include  "mtk_strings.h"




struct  mtk_label {
	std::string		string_key;	// key for string

	int   ID;			// ラベルの ID
	int	  obj_ID;		// 親オブジェクトのインデックス
	int   color_no;
	float alp;			// 透明度
	float size;			// 文字サイズ
	float R;			// 基準天体からのラベルの表示位置の半径
	float offset_x;		// オフセット
	float offset_y;
	float thc;			// フェードの基準になる視直径(rad) 
};
static  std::vector<mtk_label>  vLabels;





//------------
//  Register
//------------
int
add_label(const std::string& str_key, int obj_idx, int color_no, float alpha, float size, float R, float thc, bool bTargetable)
{

	mtk_label  lbl;
	lbl.obj_ID = obj_idx;
	lbl.color_no = color_no;
	lbl.alp = alpha;
	lbl.size = size;
	lbl.R = R;
	lbl.offset_x = 0.707;
	lbl.offset_y = 0.707;
	lbl.thc = thc;
	lbl.string_key = str_key;

	int label_idx;
	label_idx = vLabels.size();

	mtk_object  obj(OBJ_TYPE_LABEL, label_idx, 0.0f, bTargetable);

	std::string  label_key = "LABEL_" + str_key;
	int ID = register_object(obj, label_key.c_str());
	lbl.ID = ID;
	vLabels.push_back(lbl);


	return  ID;
}






vector3d
get_label_position(int idx)
{
	const mtk_label&  lbl = vLabels[idx];
	return  get_object_positon( get_object(lbl.obj_ID) );
}

float
get_label_letter_size(int idx)
{
	return  vLabels[idx].size;
}






//*****  問題あり  unicode string *****
int
get_label_letter_count(int idx)
{
	return  strlen(get_string(vLabels[idx].string_key).c_str());
}

//*************************************





const std::string&
get_label_string_key(int idx)
{
	return  vLabels[idx].string_key;
}

const std::string&
get_label_text(int idx)
{
	return  get_string(vLabels[idx].string_key);
}


int
get_label_parent_object_index(int idx)
{
	return  vLabels[idx].obj_ID;
}


//------------
//  Hit-test
//------------
bool
hit_test_label(const mtk_draw_struct& ds, int idx, const vector3d& pos, const vector3d& cpos, const vector3d& nv)
{
	const mtk_label& label = vLabels[idx];
	const mtk_object& obj = get_object( label.obj_ID );



	//---  カメラ座標系の軸
	vector3d  eF, eU, eR;
	ds.camera.get_axes(eF, eU, eR);


	//---  ラベル座標系の軸
	vector3d  rpos, eF2, eU2, eR2;
	double R2, R;
	rpos = pos - cpos;
	R2 = norm2(rpos);
	if (R2 < 1e-100)  return  false;
	
	R = sqrt(R2);
	eF2 = rpos / R;
	eR2 = normalize( cross_product(eF2, eU) );
	eU2 = cross_product( eR2, eF2 );


	//---  ラベル座標上の交点の位置
	double dnm, t;
	dnm = dot_product(nv, eF2);
	if (fabs(dnm) < 1e-100)  return  false;
	t = dot_product(rpos, eF2) / dnm;

	if (t < 0.0)  return  false;

	vector3d  rxy;
	double fac;
	float  x, y;
	
	fac = R / get_screen_z() * get_screen_w() / 180 * label.size * ds.ltf;
	
	rxy = (nv*t - rpos) / fac;



	const int type = obj.get_type();
	/***** 拡大時の hit-test のため、以下が必要 *****/
	/*
	float fac2 = 1.0f;
	if (ds.tyMode == MODE_3D) {
		if (type == OBJ_TYPE_PLANET) {
			fac2 = get_zoom_rate().zmhPlanet;
		}
		if (type == OBJ_TYPE_SATELLITE && obj.get_index() == get_satellite_index("SAT_E1")) {
			fac2 = get_zoom_rate().zmhPlanet;
		}
		if (type == OBJ_TYPE_SUN) {
			fac2 = get_zoom_rate().zmhSun;
		}
	}
	*/



	float RR0;
	switch ( type ) {
		case OBJ_TYPE_STAR:				RR0 = 1.2*obj.get_R();	break;
		case OBJ_TYPE_SMALL_OBJECT:		RR0 = 1.4*obj.get_R();	break;
		default:						RR0 = label.R;
		//default:						RR0 = fac2*label.R;
	}
	

	float  RR = RR0 / fac;

	x = dot_product( rxy, eR2 ) - RR * label.offset_x;
	y = dot_product( rxy, eU2 ) - RR * label.offset_y;


	

	//**************
	if (type == OBJ_TYPE_SPACECRAFT) {
		y += 1.0;
	}





	//---  文字列のサイズ
	float w, h;
	if (type == OBJ_TYPE_STAR) {
		w = get_star_label_size(obj.get_index());
		h = 1;
	}
	else if (type == OBJ_TYPE_SUN) {
		w = get_sun_label_size(ds);
		h = 1;
	}
	else {
		get_text_size( get_string(label.string_key.c_str()), w, h );
	}



	//---  判定
	if ( x>0.0f  &&  x<w  && y>0.0f  &&  y<h )  return true;

	return  false;
}










//--------
//  Draw
//--------
bool
prepare_draw_label(const mtk_draw_struct& ds, int idx, double* pRb, double* pRh, float* pAlpha)
{
	const mtk_label& label = vLabels[idx];
	const mtk_object& obj = get_object( label.obj_ID );



	if ( !is_object_label_visible(ds, obj) )  return false;



	//--  透明度
	*pAlpha = label.alp * get_label_alpha( ds, obj );
	if (*pAlpha < 0.02f)  return false;



	//**********   仮    *******
	double L = norm( get_label_position(idx) - ds.camera_base.get_pos() );
	double  fac = ds.ltf * L / get_screen_z() * get_screen_w() / 180.0;


		// 文字サイズ in pc
	if (obj.get_type() == OBJ_TYPE_STAR) {
		//--- 星の場合
		float sz = get_star_label_size( obj.get_index() );
		if (sz < 0.01)  return false;
		sz *= fac * get_label_letter_size(idx);
		*pRb = *pRh = sz;
	} else if (obj.get_type() == OBJ_TYPE_SUN ) {
		float sz = get_sun_label_size(ds);
		sz *= fac * get_label_letter_size(idx);
		*pRb = *pRh = sz;
	} else {
		float sz = fac * get_label_letter_size(idx);
		int len = get_label_letter_count(idx); 
		*pRb = *pRh = sz*len;
	}



	return  true;
}



void
draw_a_label(const mtk_draw_struct& ds, int idx, const vector3d& rpos, const double& scl, float alpha, float dz)
{
	static bool first = true;

	const mtk_label& label = vLabels[idx];
	int obj_ID = label.obj_ID;
	const mtk_object& obj0 = get_object(obj_ID);

	//***
	if ( obj0.get_type() == OBJ_TYPE_FIXED_OBJECT) {
		if ( get_fixed_object_type( obj0.get_index() ) == OBJ_TYPE_OUR_GALAXY ) {
			obj_ID = get_object_ID("OUR_GALAXY");
		}
	}
	
	const mtk_object& obj = get_object(obj_ID);


	static int idx_Moon;
	if (first) {
		first = false;
		idx_Moon = get_object_ID("SAT_E_MOON");
	}



	//************************
	//  以下もすべてコールバックにする
	//***********************
	const int type = obj.get_type();
	float fac = 1.0f;
	if (ds.tyMode == MODE_3D) {
		if (type == OBJ_TYPE_PLANET) {
			fac = get_zoom_rate().zmhPlanet;
		}
		if (type == OBJ_TYPE_SATELLITE && obj.get_index()==idx_Moon) {
			fac = get_zoom_rate().zmhPlanet;
		}
		if (type == OBJ_TYPE_SUN) {
			fac = get_zoom_rate().zmhSun;
		}
	} else {
		if (type == OBJ_TYPE_PLANET) {
			fac = get_zoom_rate().zmpPlanet;
		}
		if (type == OBJ_TYPE_SATELLITE && obj.get_index() == idx_Moon) {
			fac = get_zoom_rate().zmpSunMoon;
		}
		if (type == OBJ_TYPE_SUN) {
			fac = get_zoom_rate().zmpSunMoon;
		}

		fac *= ds.plnt_R;	//***
	}



	float R;
	switch (type) {
		case OBJ_TYPE_STAR:				R = 1.2*scl*obj.get_R();	break;
		case OBJ_TYPE_S_STAR:			R = 0.6*scl*obj.get_R();	break;
		case OBJ_TYPE_SMALL_OBJECT:		R = 1.4*scl*obj.get_R();	break;
		default:
			R = fac*scl*label.R;
			break;
	}

	float dx, dy;
	dx = dy = 0;
	if (label.R == 0.0) {
		dx = dy = 0.1;	// 少しずらす
	}



	//**************
	if (type == OBJ_TYPE_SPACECRAFT) {
		dy -= 1.0;
	}



	int sel_ID = get_selected_object_ID();
	RGBf  col;
	if (sel_ID != obj_ID && sel_ID != label.ID) {
		col = get_color( label.color_no );
	} else {
		col.set(0, 1.0, 0.1);
	}




	//--  描画
	glPushMatrix();
		glTranslatef(rpos.x, rpos.y, rpos.z);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		prepare_3Dtext_1(R, 45, label.size*ds.ltf, ds.scale_factor, false);

		glTranslatef(dx, dy, dz);

		switch (type) {
			case OBJ_TYPE_SUN:
				draw_sun_label(ds, col, alpha);
				break;
			case OBJ_TYPE_STAR:
				{
					glColor4f(col.r, col.g, col.b, alpha);
					draw_a_star_label(ds, obj.get_index());
				}
				break;
			default:
				{
					glColor4f(col.r, col.g, col.b, alpha);
					draw_text( get_string(label.string_key) );
				}
				break;
		}

	glPopMatrix();
}


