/**************************************************************************************************

Copyright (c) 2007-2016   Tsunehiko Kato

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
#include  <math.h>
#include  "mtk_mode.h"
#include  "mtk_stars_2.h"
#include  "mtk_stars.h"
#include  "mtk_objects.h"
#include  "mtk_label.h"
#include  "mtk_colors.h"
#include  "mtk_switches.h"
#include  "mtk_solar_system.h"
#include  "mtk_font.h"
#include  "mtk_gl_common.h"
#include  "mtk_calc_alpha.h"
#include  "mtk_draw_common.h"
#include  "../common/astro/constellation.h"
#include  "../common/astro/star_name.h"
#include  "../common/math/fast_pow.h"
#include  "mtk_strings.h"
#include  "mtk_make_info.h"
#include  "mtk_selection.h"
#include  <vector>

#include  "../common/graphics/octree.h"
#include  "../common/graphics/quadtree.h"
#include  "mtk_point_block.h"



//**********
extern GLuint  texStar;
extern float  StarImagePowIndex;
extern fast_pow  pow_for_stars;
//**********




static point_blocks vBlks;
static vector<int> star_obj_idx_list;
static vector<int> list_tmp;
static octree<float>  octree_stars;
static quadtree<float> quadtree_stars[6];




// axes and origin for quadtree planes
static vector3f ex[6] = { vector3f(0,1,0), vector3f(0,-1,0), vector3f(-1,0,0), vector3f(1,0,0),  vector3f(1,0,0), vector3f(1,0,0)  };
static vector3f ey[6] = { vector3f(0,0,1), vector3f(0,0,1),  vector3f(0,0,1),  vector3f(0,0,1),  vector3f(0,1,0), vector3f(0,1,0)  };
static vector3f r0[6] = { vector3f(1,0,0), vector3f(-1,0,0), vector3f(0,1,0),  vector3f(0,-1,0), vector3f(0,0,1), vector3f(0,0,-1) };



static float  Lv0;	// 星の基準の明るさ



float
get_Lv0(){
	return Lv0;
}



//------------
//  Register
//------------
void
register_object_stars()
{
	//--  Calc boundary box for stars
	bbox3f bb;
	for (int i = 0; i < get_stars_num(); i++) {
		const mtk_star&  st = get_star(i);
		bb.add_point(st.get_pos());
	}


	// build octree
	const int octree_lv = 8;
	int N = 1;
	for (int i = 0; i < octree_lv; i++) {
		N *= 2;
	}
	vBlks.init(N, bb);

	const float qtscl = 3000;	// in pc
	bbox2f  bb2;
	bb2.set(-qtscl, qtscl, -qtscl, qtscl);
	for (int i = 0; i < 6; i++) {
		quadtree_stars[i].init(bb2);
		quadtree_stars[i].set_axes_and_origin(ex[i], ey[i], cross_product(ex[i], ey[i]), qtscl*r0[i]);
	}



	mtk_object  obj;
	for (int i=0; i<get_stars_num(); i++) {
		mtk_star&  st = get_star(i);


		//--  Register a star
		obj.init(OBJ_TYPE_STAR, i, 0.0f, true);
		int obj_ID = register_object(obj, st.get_key());
		st.set_obj_ID(obj_ID);


		float R = 0;
		float thc = 8 * D2R;
		add_label( st.get_key(), obj_ID, MTK_COLOR_NAME_OBJECT, 1.0f, get_object_name_size(), R, thc, true );


		//--  Point Blocks
		vector3f pos = st.get_pos();
		vBlks.add_point(obj_ID, pos);



		star_obj_idx_list.push_back(obj_ID);


		const vector3f _nv = normalize(pos);
		for (int j = 0; j < 6; j++) {
			vector3f np = cross_product(ex[j], ey[j]);
			float dnm, t;
			dnm = dot_product(_nv, np);
			if (fabs(dnm) < 1e-3) continue;
			t = dot_product(r0[j], np) / dnm;

			if (t < 0) continue;

			vector3f r = _nv * t;

			float x, y;
			x = dot_product(r - r0[j], ex[j]);
			y = dot_product(r - r0[j], ey[j]);

			if (-1 <= x && x <= 1 && -1 <= y && y <= 1) {
				vector2f pos2;
				pos2.x = qtscl * x;
				pos2.y = qtscl * y;

				quadtree_stars[j].add_object(obj_ID, pos2);

				break;
			}
		}
	}


	//--  Octree
	octree_stars.init(bb);
	for (int i = 0; i < get_stars_num(); i++) {
		const mtk_star&  st = get_star(i);

		vector3f pos = st.get_pos();
		int obj_ID = st.get_obj_ID();

		octree_stars.add_object(obj_ID, pos);
	}
}





inline double 
get_star_radius(const mtk_draw_struct& ds, int idx)
{
	const float R1 = 9.0e-4;	// １等星の 1pc でのイメージ半径


	//  Radius
	const mtk_star&  st = get_star(idx);
	const float L = st.get_L();
	double dist2 = norm2(st.get_pos() - ds.camera_base.get_pos());
	float dist = sqrt(dist2);


	//return  pow(Lv0*L/dist2, double(StarImagePowIndex)) * R1 * dist;
	return pow_for_stars.eval(float(Lv0*L / dist2)) * R1 * dist;
}





//------------------
//  Landing radius
//------------------
double
get_landing_radius_star(int idx)
{
	return  0.1 * _1AU_pc;		//*****
}


//----------------
//  Prepare draw
//----------------
bool
prepare_draw_star(const mtk_draw_struct& ds, int idx, double* pRb, double* pRh)
{
	*pRh = *pRb = get_star_radius(ds, idx);
	return  true;
}



//---------
//  Scale
//---------
double
get_scale_star(int idx)
{
	//  Radius
	const mtk_star&  st = get_star(idx);
	const double& L = st.get_L();
	const double R1 = 9.0e-4;	// １等星の 1pc でのイメージ半径

	return  sqrt(Lv0*L) * pow(100*R1, 1.0/(2.0*double(StarImagePowIndex)));
}



//---------
//  Label
//---------
const double star_lc1 = 0.6;
const double star_lc2 = 1.5;


bool
is_star_label_visible(const mtk_draw_struct& ds, int idx)
{
	if ( !(swStar & STAR_DISP) )  return false; 
	if ( !(swStar & (STAR_NAME | STAR_BAYER | STAR_FRAMSTEED | STAR_HIP)) )  return false;
	if (ds.cnorm > 1.5e5)  return false;
	if (ds.cnorm < star_lc1)  return  false;

	const mtk_star& st = get_star(idx);
	if (!st.get_bNameLarge() && !st.get_bNameSmall()) {
		return false;	// 表示する名前が無い場合
	}


	return  true;
}


float
get_star_label_alpha(const mtk_draw_struct& ds, int idx)
{
	const double lc = ds.cnorm;
	if (lc < star_lc2) {
		return  (lc - star_lc1) / (star_lc2 - star_lc1);
	}

	float scl = 0.5f;
	const vector3d  spos = get_star_position(idx);
	const vector3d  cpos = ds.camera.get_pos();
	const vector3d  rpos = spos - cpos;
	const double ls = spos.norm();
	float  Rv = scl * ls;
	float  Dth = 2.0f * Rv / rpos.norm();

	return  calc_alpha_by_apparent_diameter( Dth );
}





//-------------
//  Draw name
//-------------
void
draw_a_star_label(const mtk_draw_struct& ds, int idx)
{
	const mtk_star& st = get_star(idx);

	bool bNameLarge, bNameSmall;
	bNameLarge = st.get_bNameLarge();
	bNameSmall = st.get_bNameSmall();
	if (!bNameLarge && !bNameSmall) {
		return;
	}
	

	if (bNameLarge) {
		draw_text(st.get_NameLarge());
	}

	
	if (bNameSmall) {
		const float f = 4.0 / 5.0;
		glScalef(f,f,f);
		draw_text(st.get_NameSmall());
	}
}


float
get_star_label_size(int idx)
{
	return  get_star(idx).get_label_width();
}



//----------------
//  Prepare draw
//----------------

//***  prepare_draw_objects の前に呼ぶ  ***
void
prepare_draw_stars_2(const mtk_draw_struct &ds)
{
	//float L0 = 50000;
	float L0 = 35000;
	//float L0 = 22000;
	const float scl0 = 500.0f;


	//**
	switch (get_display_mode()) {
	case DISPMODE_SIDE_BY_SIDE:
	case DISPMODE_TOP_AND_BOTTOM:
		L0 *= 2.5;
		break;
	case DISPMODE_DOME_MASTER:
		L0 *= 4.0;
		break;
	default:
		break;
	}



	//  基準の明るさの設定(スケール依存）
	float amp = 1;
	if (ds.scale > scl0) {
		float a = 4*log10(ds.scale/scl0);
		amp *= (1 + a*a);
	}
	Lv0 = amp * L0 / get_L_ref();
}







//--------
//  Draw
//--------
void
draw_star_mark_3D(const vector3d &rpos, const vector3f &dx, const vector3f &dy, float fac)
{
	vector3f  r;
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINE_STRIP);

	r = rpos + fac * dx;
	glVertex3fv(r.get_pointer());
	r = rpos + fac * dy;
	glVertex3fv(r.get_pointer());
	r = rpos - fac * dx;
	glVertex3fv(r.get_pointer());
	r = rpos - fac * dy;
	glVertex3fv(r.get_pointer());
	r = rpos + fac * dx;
	glVertex3fv(r.get_pointer());

	glEnd();

	glEnable(GL_TEXTURE_2D);
}



void
draw_a_star(const mtk_draw_struct& ds, int idx, const vector3d& rpos, const double& scl, const double& Rb, float alp, bool bChangeObjType)
{
	if (bChangeObjType) {
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glBindTexture(GL_TEXTURE_2D, texStar);
		glColor3f(1, 1, 1);
	}



	//----------------------
	//  Textured billboard
	//----------------------
	const mtk_star& st = get_star(idx);
	const float sz = 2.0 * scl * Rb;
	vector3f  r, dx, dy;
	float u1, u2;

	if (ds.bRotateBillboard) {
		dx = sz * st.eR;
		dy = sz * st.eU;
	}
	else {
		dx = sz * ds.eRf;
		dy = sz * ds.eUf;
	}
	u1 = st.u1;
	u2 = st.u2;

	glBegin(GL_QUADS);
		r = rpos + dx;
		glTexCoord2f(u2, 1);		glVertex3fv(r.get_pointer());
		r = rpos + dy;
		glTexCoord2f(u1, 1);		glVertex3fv(r.get_pointer());
		r = rpos - dx;
		glTexCoord2f(u1, 0);		glVertex3fv(r.get_pointer());
		r = rpos - dy;
		glTexCoord2f(u2, 0);		glVertex3fv(r.get_pointer());
	glEnd();


	if (tyStarMark == STARMARK_DOUBLE && st.is_double()) {
		glColor4f(0, 1, 0, alp);
		glLineWidth(0.5 * ds.line_width_fac / (ds.rad_per_pixel / 0.001));
		draw_star_mark_3D(rpos, dx, dy, 1.2);
		glColor3f(1, 1, 1);
	}

	if (tyStarMark == STARMARK_EXOPLANETS && st.bPlanet) {
		if (st.get_HIP() == -1) {
			glColor4f(1, 0.5, 0, alp);
		}
		else {
			glColor4f(1, 0.9, 0, alp);
		}
		glLineWidth(2.0 * ds.line_width_fac / (ds.rad_per_pixel / 0.001));
		draw_star_mark_3D(rpos, dx, dy, 2);
		glColor3f(1, 1, 1);
	}
}





//--------
//  Info
//--------
void
make_object_info_star(const mtk_draw_struct& ds, int idx, std::vector<std::string>& text)
{
	const mtk_star& st = get_star(idx);
	int HIP = st.get_HIP();


	const string& key = st.get_key();
	if (is_defined_key(key)) {
		text.push_back(get_string(key));	// 固有名
	}



	bool bHIP = (HIP != -1);
	if (bHIP) {
		//-- バイエル名、フラムスチード記号の表示文字列を作る
		int BayerID = st.get_BayerID();
		int BayerSub = st.get_BayerSub();
		int FramsteedNo = st.get_FramsteedNo();
		int ADScomp = st.get_ADScomp();
		const string szCnst = get_constellation_abbr(st.get_cnst_ID());

		string name;
		name = make_Bayer_name(BayerID, BayerSub, FramsteedNo, ADScomp, szCnst);
		if (name == "") {
			name = "HIP" + to_string(HIP);
		}
		text.push_back(name);
	}


	string str;

	//---  Type  ---
	add_info_line(text, "INFO_TYPE", "INFO_TYPE_STAR");

	// Spectral type
	char spc_type, spc_idx;
	spc_type = st.get_spectrum_type();
	spc_idx = st.get_spectrum_index();
	if (spc_type != 0) {
		str = spc_type + to_string(spc_idx);
		add_info_line_str(text, "INFO_STAR_SPECTRAL_TYPE", str.c_str());
	}

	// HIP number
	if (HIP != -1) {
		add_info_line_int(text, "INFO_HIP_NUMBER", HIP);
	}

	// distance
	//float ly = _1pc_ly * norm(st.get_pos());

	// Exoplanet
	if (st.bPlanet) {
		text.push_back(get_string("INFO_STAR_WITH_PLANET"));
	}
}





//----------
//  Octree
//----------
const vector<int>&
get_stars_draw_list(const mtk_draw_struct& ds)
{
	static bool first = true;
	const vector3d* nv;
	const double *D;


	if (first) {
		first = false;
		list_tmp.reserve(get_stars_num());
	}



	ds.get_frustum_planes_on_world(nv, D);

	vector3f nvf[6];
	float Df[6];
	for (int i = 0; i < 6; i++) {
		nvf[i] = nv[i];
		Df[i] = D[i];
	}

	
	




	list_tmp.clear();

	
	if (ds.scale > 700) {
		if (!is_visible(nvf, Df, octree_stars.get_bbox(), false)) {
			return list_tmp;
		}

		return star_obj_idx_list;
	}
	

	if (ds.scale >= 0.02) {
		octree_stars.make_draw_objects_list(nvf, Df, false, list_tmp);
	}
	else {
		for (int i = 0; i < 6; i++) {
			quadtree_stars[i].make_draw_objects_list(nvf, Df, false, list_tmp);
		}
	}

	return list_tmp;
}

