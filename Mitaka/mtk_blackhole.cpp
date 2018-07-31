/**************************************************************************************************

Copyright (c) 2015   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#include  "../common/graphics/gl_framebuffer.h"
#include  "mtk_blackhole.h"
#include  "mtk_objects.h"
#include  "mtk_label.h"
#include  "mtk_colors.h"
#include  "mtk_switches.h"
#include  "mtk_calc_alpha.h"
#include  "mtk_our_galaxy.h"
#include  "../common/astro/astro_const.h"
#include  "../common/graphics//gl_image.h"
#include  "../common/graphics//gl_win.h"
#include  "mtk_solar_system.h"
#include  "mtk_target.h"
#include  "mtk_camera_setting.h"
#include  "mtk_strings.h"
#include  "mtk_make_info.h"




//--  Externs
void  draw_scene_all(const mtk_draw_struct& ds, bool bClear);

extern int win_w, win_h;



// ブラックホールのシュヴァルツシルト半径
const double Rg = 0.08 * _1AU_pc;

// 見かけの半径
const double Ra = 6 * Rg;

static int tex_w = 0, tex_h = 0;
static double b_min, b_max;




struct ref_angle {
	double b, ang;
};

std::vector<ref_angle>  vRefAngle;





//------------
//  Register
//------------
void
register_object_bloackhole()
{
	mtk_object  obj;

	//  register object
	float dz = 0.1;
	obj.init(OBJ_TYPE_BLACKHOLE, 0, dz, true);
	int obj_idx = register_object(obj, "SGR_A_STAR");

	// register label
	float thc = 5 * D2R;	//**
	add_label( "SGR_A_STAR", obj_idx, MTK_COLOR_NAME_SPECIAL_OBJECT, 1.0f, get_object_name_size(), Ra, thc, true);
}


void
init_blackhole(const char *fn)
{
	// register
	register_object_bloackhole();


	// load table
	char buf[100];
	FILE *fp;
	b_min = b_max = 0;

	fp = fopen(fn, "rt");
	if (fp == NULL) return;


	vRefAngle.clear();
	while (fgets(buf, 100, fp) != NULL) {
		ref_angle ra;

		sscanf(buf, "%le %le\n", &ra.b, &ra.ang);
		vRefAngle.push_back(ra);

		if (b_min == 0 || b_min > ra.b) {
			b_min = ra.b;
		}
		if (b_max == 0 || b_max < ra.b) {
			b_max = ra.b;
		}
	}

	fclose(fp);
}



//--------------------
//  Refraction angle
//--------------------
double
get_ref_angle(const double &b)
{
	if (vRefAngle.size() == 0) {
		return 2.0 / b;	// approximation if no table
	}

	if (b < b_min)  return 0;	// out of range

	if (b > b_max) {
		return 2.0 / b;	// approximation
	}


	// calculate with table
	int il, ir;
	il = 0;
	ir = vRefAngle.size() - 1;

	while (il+1 < ir) {
		int ih;
		ih = (il + ir) / 2;

		double bh = vRefAngle[ih].b;
		if (b < bh) {
			ir = ih;
		}
		else if (b > bh) {
			il = ih;
		}
	}

	double b1, b2, ang1, ang2, t, ang;
	b1   = vRefAngle[il].b;
	ang1 = vRefAngle[il].ang;
	b2   = vRefAngle[ir].b;
	ang2 = vRefAngle[ir].ang;
	t = (b - b1) / (b2 - b1);

	ang = (1.0 - t) * ang1 + t * ang2;

	return ang;
}



//------------
//  Position
//------------
vector3d
get_blackhole_position()
{
	return  get_our_galaxy_position();
}



//---------
//  Scale
//---------
double
get_scale_blackhole()
{
	return  50 * _1AU_pc;
}



//--------------
//  Visibility
//--------------
const double star_lc1 = 0.01 * _1AU_pc;
const double star_lc2 = 0.03 * _1AU_pc;


bool
is_blackhole_label_visible(const mtk_draw_struct& ds)
{
	if (!(swGalaxy & GLXY_SGR_A_ST))  return false;
	if (!(swStar & STAR_NAME))  return false;

	return  true;
}


float
get_blackhole_label_alpha(const mtk_draw_struct& ds)
{
	const vector3d  cpos = ds.camera.get_pos();
	float  Dth = 500.0f * _1AU_pc / norm(get_our_galaxy_position() - cpos);

	return  calc_alpha_by_apparent_diameter(Dth);
}



//---------
//  Alpha
//---------
float
get_blackhole_point_alpha(const mtk_draw_struct& ds)
{
	const vector3d  cpos = ds.camera.get_pos();

	const float R3 = 1;
	const float R4 = 4;
	float R = norm(get_our_galaxy_position() - cpos);
	if (R > R4)  return  0.0;

	float af = 1.0;
	if (R > R3)	{ af *= 1 - (R - R3) / (R4 - R3); }

	if (af < 0.05)  return 0.0;
	return  af;
}


//------------------
//  Landing radius
//------------------
double
get_landing_radius_blackhole()
{
	return  0.1 * _1AU_pc;
}




//----------------
//  Prepare draw
//----------------
double
get_blackhole_radius(const mtk_draw_struct& ds, const vector3d& rpos)
{
	return Rg;
}


bool
prepare_draw_blackhole(const mtk_draw_struct& ds, double* pRb, double* pRh)
{
	//---  Visible
	bool bFlg;
	bFlg = swGalaxy & GLXY_SGR_A_ST;

	if (!bFlg)  return false;


	//---  Radius
	vector3d rpos;

	rpos = get_blackhole_position() - ds.camera_base.get_pos();
	*pRh = Ra;
	*pRb = 100 * Ra;


	return  true;
}





//--------
//  Draw
//--------
void
draw_blackhole_sphere()
{
	glDisable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_NORMALIZE);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	glColor3f(0, 0, 0);

	gl_draw_LOD_sphere();
}


void
draw_blackhole_mesh(const mtk_draw_struct& ds)
{
	const int N = 250;
	static float u[N][N], v[N][N];
	int vp[4];



	int w, h;
	glGetIntegerv(GL_VIEWPORT, vp);
	w = vp[2];
	h = vp[3];

	int x0, y0;
	x0 = vp[0];
	y0 = vp[1];



	//--- テクスチャ座標の計算 ---

	// 視点座標系
	quaterniond ori = ds.camera_base.get_ori() * get_base_camera_ori() * get_local_camera_orientation();

	// 視点座標系から黄道座標系への変換行列
	const matrix3d  M = quaternion_to_matrix3(ori);
	const matrix3d  tM = transpose(M);


	// base camera 座標系から黄道座標系への変換行列
	quaterniond ori2 = ds.camera_base.get_ori() * get_base_camera_ori();
	const matrix3d  M2 = quaternion_to_matrix3(ori2);

	// 黄道座標系での視点の位置
	const vector3d eye_pos = ds.camera_base.get_pos() + M2 * ds.eye_offset / ds.scale_factor;

	const vector3d dr = get_blackhole_position() - eye_pos;


	const frustum& frs = ds.get_frustum();
	const float sclf = 1.002 * get_screen_z() / frs.get_z1();
	float x1, y1, x2, y2, z1;
	x1 = sclf * frs.get_x1();// - eye_offset_x;
	x2 = sclf * frs.get_x2();// - eye_offset_x;
	y1 = sclf * frs.get_y1();
	y2 = sclf * frs.get_y2();
	z1 = get_screen_z();


	for (int iy = 0; iy < N; iy++) {
		float t, y;
		t = float(iy) / (N - 1);
		y = (1 - t)*y1 + t*y2;


		for (int ix = 0; ix < N; ix++) {
			float x;

			t = float(ix) / (N - 1);
			x = (1 - t)*x1 + t*x2;

			// 黄道座標系での視線方向ベクトル
			vector3d n = M * normalize(vector3d(x, y, -z1));
			
			// 視線とブラックホールを含む面の法線とインパクト・パラメータ
			vector3d vp, np;
			double b;

			vp = cross_product(n, dr);
			b = norm(vp);
			np = vp/b;

			b /= Rg;


			// 屈折角(rad)
			double ang;
			ang = get_ref_angle(b);


			// 曲げられる方向
			vector3d nperp;
			nperp = cross_product(np, n);

			// 曲げられた視線ベクトル
			vector3d _n;
			_n = n*cos(ang) + nperp*sin(ang);

			// 視点座標系での視線ベクトル
			vector3d nn = tM * _n;

			// スクリーン上の座標
			vector3d _r;
			float _x, _y;
			t = -z1 / nn.z;
			_x = t * nn.x;
			_y = t * nn.y;

			// テクスチャ座標
			float _u, _v;
			_u = (_x - x1) / (x2 - x1) * w / tex_w;
			_v = (_y - y1) / (y2 - y1) * h / tex_h;

			u[iy][ix] = _u;
			v[iy][ix] = _v;
		}
	}



	//---  描画  ---
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_NORMALIZE);
	glDisable(GL_BLEND);
	glColor3f(1, 1, 1);



	// テスト用
	//glEnable(GL_BLEND);
	//glColor4f(1, 0, 0, 0.5);

	gl_enter_2D_mode();

	for (int iy = 0; iy < N-1; iy++) {
		float t, y1, y2;


		t = float(iy) / (N-1);
		y1 = t * h - y0;

		t = float(iy+1) / (N-1);
		y2 = t * h - y0;


		glBegin(GL_TRIANGLE_STRIP);
		for (int ix = 0; ix < N; ix++) {
			float x;

			t = float(ix) / (N-1);
			x = t * w + x0;


			glTexCoord2f(u[iy][ix], v[iy][ix]);
			glVertex2f(x, h-y1);
			glTexCoord2f(u[iy+1][ix], v[iy+1][ix]);
			glVertex2f(x, h-y2);
		}
		glEnd();
	}

	gl_leave_2D_mode();
}


void
draw_blackhole(const mtk_draw_struct& ds, const vector3d& rpos, const double& scl, const double& Rb)
{
	static image img(0,0,3);
	static GLuint  texname = 0;


	double R2;
	R2 = norm2(rpos);
	if (R2 < 1e-100)  return;
	const float sz = 2.0 * scl * Rb;




	gl_capture_viewport(img, GL_BACK);


	GLint max_size;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_size);

	bool bFlg = false;
	int ww, hh;

	ww = win_w;
	hh = win_h;
	if (ww > max_size) { ww = max_size; }
	if (hh > max_size) { hh = max_size; }



	glEnable(GL_TEXTURE_2D);
	if (tex_w < ww || tex_h < hh) {


		int  width, height, chs;
		GLenum fmt;
		width = img.get_width();
		height = img.get_height();
		chs = img.get_channels();
		fmt = get_GLformat(chs);

		if (is2n(width) && is2n(height)) {
			tex_w = width;
			tex_h = height;
		}
		else {
			tex_w = to2n(width);
			tex_h = to2n(height);
		}

		if (tex_w > max_size) tex_w = max_size;
		if (tex_h > max_size) tex_h = max_size;

		if (texname != 0) {
			glDeleteTextures(1, &texname);
		}

		glGenTextures(1, &texname);
		glBindTexture(GL_TEXTURE_2D, texname);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, fmt, tex_w, tex_h, 0, fmt, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img.get_width(), img.get_height(), GL_RGB, GL_UNSIGNED_BYTE, img.get_pointer());
	}
	else {
		glBindTexture(GL_TEXTURE_2D, texname);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img.get_width(), img.get_height(), GL_RGB, GL_UNSIGNED_BYTE, img.get_pointer());
	}



	// draw mesh
	draw_blackhole_mesh(ds);

	// draw BH
	glPushMatrix();
	glTranslatef(rpos.x, rpos.y, rpos.z);
	glScalef(Ra, Ra, Ra);

	draw_blackhole_sphere();

	glPopMatrix();
}



//--------
//  Info
//--------
void
make_object_info_blackhole(const mtk_draw_struct& ds, std::vector<std::string>& text)
{
	//---  Name  ---
	text.push_back(get_string("SGR_A_STAR"));

	//---  Type  ---
	add_info_line(text, "INFO_TYPE", "INFO_TYPE_SMBH");
}
