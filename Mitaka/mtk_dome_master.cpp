/**************************************************************************************************

Copyright (c) 2015-2016   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#include  "mtk_dome_master.h"
#include  <stdio.h>
#include  "../common/math/vector3.h"
#include  "../common/math/quaternion.h"
#include  "mtk_camera_setting.h"
#include  "mtk_draw_struct.h"
#include  "mtk_mode.h"
#include  "mtk_objects.h"
#include  "mtk_planetarium.h"

#include  "../common/graphics/gl_Model3D.h"
#include  "../common/graphics/obj_loader.h"






//--  Externs
void  draw_scene_all(const mtk_draw_struct& ds, bool bClear);
mtk_draw_struct calc_draw_struct(const mtk_camera& camera_base, float aspc_rt, int _vp_x1, int _vp_x2, int _vp_y1, int _vp_y2, vector3f dr, bool bRotBB, float line_w);



//
static gl_dome_master dm;

static int buffer_size  = 1024;			// FBOのサイズ
static int capture_size = 2048;			// キャプチャー画像のサイズ
static int mesh_N = 50;					// メッシュの分割数
static float dome_R = 1000;				// ドーム半径(in cm)
static float elev_angle = 30.0;			// 正面方向の仰角
static float elev_angle_plnt = 30.0;	// 正面方向の仰角（プラネタリウム）
static float label_mag = 2.0;			// ラベルの拡大率
static float line_width_fac = 2.5;		// 

//--- DomeMaster parameters
static vector3f g_proj_offset(0,0,0);	// プロジェクタのオフセット（ドーム半径単位、X:右、Y:正面、Z:上）
static float g_proj_elev_angle = 90;	// プロジェクタの仰角（ドームマスターのとき90度）
static float g_proj_azimuth = 0;		// プロジェクタの方位角
static float g_proj_slue = 0;			// プロジェクタの光軸回りの回転
static float g_proj_view_angle = 180;	// 画面の短辺方向の画角（ドームマスターのとき180度）

static bool bUseDomeIntersection = false;		// プロジェクタの姿勢（Slue除く）を光軸とドームとの交点の座標（仰角、方位角）から決めるかどうか
static float g_intersection_elev_angle = 90;	// プロジェクタ光軸とドーム交点の仰角
static float g_intersection_azimuth = 0;		// プロジェクタ光軸とドーム交点の方位角

//---  Distortion map mode
static glModel3D dmDistMap;
static bool bDmDistMap;
static int dm_dist_fbo_size = 0;
static 	gl_frame_buffer dm_dist_fbo;

//--- Rotation for stereoscopic effect
static float stereo_rot_angle;			// 立体視モード時のカメラの回転角



void
set_dome_master_buffer_size(int sz)
{
	if (sz < 128)	{ sz = 128; }
	if (sz > 4096)	{ sz = 4096; }

	buffer_size = sz;
}

void
set_dome_master_capture_size(int sz)
{
	if (sz < 100)	{ sz = 100; }
	if (sz > 10000)	{ sz = 10000; }

	capture_size = sz;
}

void
set_dome_master_mesh_N(int N)
{
	if (N < 4)		{ N = 4; }
	if (N > 2000)	{ N = 2000; }

	mesh_N = N;
}

void
set_dome_master_dome_R(float R)
{
	dome_R = R;
}

void
set_dome_master_elev_angle(float angle)
{
	elev_angle = angle;
}

float
get_dome_master_elev_angle()
{
	return elev_angle;
}

void
set_dome_master_elev_angle_planetarium(float angle)
{
	elev_angle_plnt = angle;
}

float
get_dome_master_elev_angle_planetarium()
{
	return elev_angle_plnt;
}

void
set_dome_master_label_mag(float fac)
{
	label_mag = fac;
}

float
get_dome_master_label_mag()
{
	return label_mag;
}

void
set_dome_master_line_width_fac(float fac)
{
	line_width_fac = fac;
}

float
get_dome_master_line_width_fac()
{
	return line_width_fac;
}


void
set_dome_master_proj_offset(const vector3f &proj_offset)
{
	g_proj_offset = proj_offset;
}

void
set_dome_master_proj_elev_angle(float angle)
{
	g_proj_elev_angle = angle;
}

void
set_dome_master_proj_azimuth(float angle)
{
	g_proj_azimuth = angle;
}

void
set_dome_master_proj_slue(float angle)
{
	g_proj_slue = angle;
}

void
set_dome_master_proj_view_angle(float angle)
{
	g_proj_view_angle = angle;
}

void
set_dome_master_intersection_mode(bool bFlg)
{
	bUseDomeIntersection = bFlg;
}

void
set_dome_master_intersection_elev_angle(float angle)
{
	g_intersection_elev_angle = angle;
}

void
set_dome_master_intersection_azimuth(float angle)
{
	g_intersection_azimuth = angle;
}

void
set_dome_master_stereo_rot_angle(float angle)
{
	stereo_rot_angle = angle;
}





void
check_extentions()
{
	char ext_name[][50] = {
		"GL_ARB_framebuffer_object",
		"GL_EXT_framebuffer_object"
	};
	const int N = sizeof(ext_name) / sizeof(ext_name[0]);

	FILE *fp;


	fp = fopen("OpenGL_ext_check.txt", "wt");
	for (int i = 0; i < N; i++) {
		char str[2][20] = {"NG", "OK"};
		bool bSupport;
		int j;

		bSupport = glewIsExtensionSupported(ext_name[i]);
		j = (bSupport) ? 1 : 0;

		fprintf(fp, "%s %s\n", ext_name[i], str[j]);
	}
	fclose(fp);
}






//---------------
//  Dome master
//---------------
const vector3f _eF[6] = {
	vector3f(1, 0, 0), vector3f(-1, 0, 0),
	vector3f(0, 1, 0), vector3f(0, -1, 0),
	vector3f(0, 0, 1), vector3f(0, 0, -1)
};
const vector3f _eU[6] = {
	vector3f(0, 0, 1), vector3f(0, 0, 1),
	vector3f(0, 0, 1), vector3f(0, 0, 1),
	vector3f(0, -1, 0), vector3f(0, -1, 0)
};




gl_dome_master::gl_dome_master() : N(0), vtxbuf(0), uvbuf(0), idxbuf(0)
{
}


void
gl_dome_master::init(int _N)
{
	N = _N;
	vtxbuf = new float[5 * N*N * 3];
	uvbuf = new float[5 * N*N * 2];
	idxbuf = new unsigned int[5 * N * N * 2];
}


void
gl_dome_master::generate_map(const vector3f& eye_offset, const vector3f& proj_offset, float proj_elev_angle, float proj_azimuth, float proj_slue, float proj_view_angle)
{
	if (proj_view_angle < 5) proj_view_angle = 5;
	if (proj_view_angle > 300) proj_view_angle = 300;


	circle_R = 180.0f / proj_view_angle;


	const float thp = proj_elev_angle *D2R;
	const float phip = -proj_azimuth * D2R;
	const float slue = proj_slue * D2R;

	matrix3f M = rotation_matrix3(phip, vector3f(0, 0, 1)) * rotation_matrix3(thp, vector3f(1, 0, 0)) * rotation_matrix3(slue, vector3f(0,1,0));
	vector3f eRp, eUp, eFp;
	M.get_bases(eRp, eFp, eUp);


	int idx0 = 0;
	int addr = 0;
	
	for (int face = 0; face < 5; face++) {

		vector3f eF, eU, eR;
		eF = _eF[face];
		eU = _eU[face];
		eR = normalize(cross_product(eU, -eF));


		int ix1, ix2, Nx;
		float u0, u1;
		ix1 = 0;
		ix2 = N;
		u0 = 0;
		u1 = 1;
		Nx = ix2 - ix1;

		int iy1, iy2, Ny;
		float v0, v1;
		iy1 = 0;
		iy2 = N;
		v0 = 0;
		v1 = 1;
		if (face != 4) { iy1 = N / 2; iy2 = N; v0 = 0.5; }	// using half area
		Ny = iy2 - iy1;

		_Nx[face] = Nx;
		_Ny[face] = Ny;

		// make vertex
		for (int iy = 0; iy < Ny; iy++) {
			float ry, v, _y;

			ry = float(iy) / (Ny - 1);
			v = (v1 - v0) * ry + v0;;
			_y = 2.0 * v - 1.0;


			for (int ix = 0; ix < Nx; ix++) {
				float rx, u, _x;

				rx = float(ix) / (Nx - 1);
				u = (u1 - u0) * rx + u0;
				_x = 2.0 * u - 1.0;

				
				vector3f nv, pos, _pos;
				float xp, yp, zp, rho, th, phi, r;
				float b, c, D, t;


				// direction at camera
				nv = normalize(_x*eR + _y*eU + 1.0f*eF);


				// intersection point ont dome
				b = dot_product(nv, eye_offset);
				c = norm2(eye_offset) - 1;
				D = b*b - c;
				if (b <= 0) {
					t = -b + sqrt(D);
				}
				else {
					t = c / (-b - sqrt(D));
				}
				//t = -b + sqrt(b*b - c);

				pos = nv*t + eye_offset;


				// position as seen from projector
				_pos = pos - proj_offset;
				xp = dot_product(eRp, _pos);
				yp = -dot_product(eUp, _pos);
				zp = dot_product(eFp, _pos);
				
				rho = sqrt(xp*xp + yp*yp);
				th = atan2(rho, zp);
				phi = atan2(yp, xp);
				r = th / (0.5 * M_PI);
				r *= circle_R;

				// position on dome master
				float xx, yy;
				xx = r * cos(phi);
				yy = r * sin(phi);

				// vertex position
				int idx = (idx0 + iy*Nx + ix) * 3;
				vtxbuf[idx] = xx;
				vtxbuf[idx + 1] = yy;
				vtxbuf[idx + 2] = 0.0;

				// tex coord
				idx = (idx0 + iy*Nx + ix) * 2;
				uvbuf[idx] = u;
				uvbuf[idx + 1] = v;
			}
		}

		// make index
		_idx0[face] = addr;
		for (int iy = 0; iy < Ny; iy++) {
			for (int ix = 0; ix < Nx; ix++) {
				int idx;

				idx = idx0 + iy*Nx + ix;
				idxbuf[addr++] = idx + Nx;	// to be counter-clock wise
				idxbuf[addr++] = idx;
			}
		}

		idx0 += Nx*Ny;
	}
}


gl_dome_master::~gl_dome_master()
{
	delete[] vtxbuf;
	delete[] uvbuf;
	delete[] idxbuf;
}


void
gl_dome_master::Draw(int face) const
{
	int Nx, Ny, idx0;
	Nx = _Nx[face];
	Ny = _Ny[face];
	idx0 = _idx0[face];

	for (int i = 0; i < Ny - 1; i++) {
		glBegin(GL_TRIANGLE_STRIP);
		for (int j = 0; j < Nx * 2; j++) {
			unsigned int idx;

			idx = idxbuf[idx0 + j + Nx*2*i];

			// tex coord
			glTexCoord2f(uvbuf[2*idx], uvbuf[2*idx + 1]);

			// vertex position
			glVertex2f(vtxbuf[3 * idx], vtxbuf[3 * idx + 1]);
		}
		glEnd();
	}
}



void
init_dome_master()
{
	vector3f re, rp;
	float proj_elev_angle, proj_azimuth, proj_slue, proj_view_angle;


	re = vector3f(get_eye_offset_x(), 0, 0);

	if (bDmDistMap) {
		rp.set(0, 0, 0);
		proj_elev_angle = 90.0;
		proj_azimuth = 0.0;
		proj_slue = 0.0;
		proj_view_angle = 180.0;
	}
	else {
		rp = g_proj_offset;
		proj_view_angle = g_proj_view_angle;
		if (!bUseDomeIntersection) {
			proj_elev_angle = g_proj_elev_angle;
			proj_azimuth = g_proj_azimuth;
			proj_slue = g_proj_slue;
		}
		else {
			float _th, _phi, _x, _y, _z, _rho;
			_th = g_intersection_elev_angle * D2R;
			_phi = g_intersection_azimuth * D2R;
			_z = dome_R * sin(_th);
			_rho = dome_R * cos(_th);
			_x = _rho * sin(_phi);
			_y = _rho * cos(_phi);

			float dx, dy, dz, rho, th, phi;
			dx = _x - g_proj_offset.x;
			dy = _y - g_proj_offset.y;
			dz = _z - g_proj_offset.z;
			rho = sqrt(dx*dx + dy*dy);
			phi = atan2(dx, dy);
			th = atan2(dz, rho);

			proj_elev_angle = th * R2D;
			proj_azimuth = phi * R2D;
			proj_slue = proj_azimuth + g_proj_slue;
		}
	}


	dm.init(mesh_N);
	dm.generate_map(re / dome_R, rp / dome_R, proj_elev_angle, proj_azimuth, proj_slue, proj_view_angle);
}



void
draw_dome_master_sub(const mtk_camera camera_base, int w, int h, gl_frame_buffer *fbo_out)
{
	static gl_frame_buffer fbo;
	static bool first = true;

	if (first) {
		init_dome_master();

		//check_extentions();

		glEnable(GL_TEXTURE_2D);
		fbo.Init(buffer_size, buffer_size, true);
		first = false;
	}

	int fbo_w, fbo_h;
	fbo_w = fbo.get_width();
	fbo_h = fbo.get_height();

	mtk_draw_struct  ds[5];
	quaternionf _ori[5];
	for (int face = 0; face < 5; face++) {
		vector3f eF, eU;
		quaternionf ori;

		eF = _eF[face];
		eU = _eU[face];
		ori = axes_to_quaternion(eF, eU);



		// Elevation angle
		ori = make_rotation_quaternion(float(M_PI), vector3f(1, 0, 0)) * ori;

		float elevation_angle;
		if (get_mode() == MODE_3D)	{ elevation_angle = elev_angle; }
		else						{ elevation_angle = elev_angle_plnt; }
		ori = make_rotation_quaternion(float(D2R * (90.0-elevation_angle)), vector3f(1, 0, 0)) * ori;


		// Rotation for stereo mode
		ori = make_rotation_quaternion(double(-stereo_rot_angle*D2R), vector3d(0, 1, 0)) * ori;	// 立体視時のカメラの回転角


		_ori[face] = ori;


		//*****
		vector3f dr = get_eye_offset_x()*vector3f(1, 0, 0);
		set_local_camera(ori, dr);
		//set_local_camera(ori, vector3d(0, 0, 0));
		//*****



		// set draw_struct and frustum
		bool bRotBB = true;

		float z1, z2;
		z1 = get_z_near();
		z2 = get_z_mid();


		if (face == 4) {
			ds[face] = calc_draw_struct(camera_base, 1.0, 0, fbo_w, 0, fbo_h, dr, bRotBB, line_width_fac);
			ds[face].frs_near.set(-z1, z1, -z1, z1, z1, z2);
			ds[face].frs_far.set(-z2, z2, -z2, z2, z2, get_z_far());
			ds[face].recalc_frustum_planes_on_world();	//*****
		}
		else {
			ds[face] = calc_draw_struct(camera_base, 1.0, 0, fbo_w, fbo_h / 2, fbo_h, dr, bRotBB, line_width_fac);
			ds[face].frs_near.set(-z1, z1, 0, z1, z1, z2);
			ds[face].frs_far.set(-z2, z2, 0, z2, z2, get_z_far());
			ds[face].recalc_frustum_planes_on_world();	//*****
		}
		ds[face].set_red_per_pixel();
	}


	float x, y;
	if (w > h) {
		x = float(w) / h;
		y = 1;
	}
	else {
		x = 1;
		y = float(h) / w;
	}


	if (fbo_out != 0) {
		fbo_out->Bind();
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (fbo_out != 0) {
		fbo_out->Unbind();
	}


	for (int face = 0; face < 5; face++) {


		//---  draw on FBO  ---
		fbo.Bind();
		glViewport(0, 0, fbo_w, fbo_h);


		// draw scene
		//***
		set_local_camera(_ori[face], ds[face].eye_offset);
		//***

		if (ds[face].tyMode == MODE_3D) {
			prepare_draw_objects(&ds[face], 1);
		}
		else {
			prepare_draw_planetarium(&ds[face], 1);
		}

		draw_scene_all(ds[face], true);
		fbo.Unbind();
		


		
		//---  draw on dome master  ---
		if (fbo_out != 0) {
			fbo_out->Bind();
		}

		glColor3f(1, 1, 1);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(-x, x, y, -y, -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glViewport(0, 0, w, h);
		glBindTexture(GL_TEXTURE_2D, fbo.get_texture());
		dm.Draw(face);

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		if (fbo_out != 0) {
			fbo_out->Unbind();
		}
	}


	//**
	// 他の表示モードに切り替えたときのために、oriを単位クォータニオンに戻す
	set_local_camera(quaternionf(1,0,0,0), vector3d(0, 0, 0));
}





//-----------------------
//  Distortion map mode
//-----------------------
void
load_dome_master_distortion_map()
{
	bDmDistMap = load_OBJ_file("dm_mesh.obj", dmDistMap);

	dmDistMap.set_r0(vector3f(-0.5, -0.5, 0));
	dmDistMap.set_scale(2.0f);
}


void
draw_dome_master_distortion_map_mode(const mtk_camera camera_base, int w, int h, gl_frame_buffer *fbo_out)
{
	if (!bDmDistMap) return;

	int sz;
	sz = (w < h) ? w : h;

	if (sz != dm_dist_fbo_size) {
		dm_dist_fbo_size = sz;

		dm_dist_fbo.Init(dm_dist_fbo_size, dm_dist_fbo_size, true);
	}

	draw_dome_master_sub(camera_base, sz, sz, &dm_dist_fbo);


	float x, y;
	if (w > h) {
		x = float(w) / h;
		y = 1;
	}
	else {
		x = 1;
		y = float(h) / w;
	}



	if (fbo_out != 0) {
		fbo_out->Bind();
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-x, x, -y, y, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glViewport(0, 0, w, h);
	glBindTexture(GL_TEXTURE_2D, dm_dist_fbo.get_texture());


	vector3f r0;
	r0 = dmDistMap.get_r0();
	float scl;
	scl = dmDistMap.get_scale();



	glScalef(scl, scl, scl);
	glTranslatef(r0.x, r0.y, r0.z);



	dmDistMap.draw_triangle_mesh(0);


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	if (fbo_out != 0) {
		fbo_out->Unbind();
	}
}



//--------
//  Draw
//--------
void
draw_dome_master_fbo(const mtk_camera camera_base, int w, int h, gl_frame_buffer *fbo_out)
{
	if (bDmDistMap) {
		draw_dome_master_distortion_map_mode(camera_base, w, h, fbo_out);
	}
	else {
		draw_dome_master_sub(camera_base, w, h, fbo_out);
	}
}

void
draw_dome_master(const mtk_camera camera_base, int w, int h)
{
	draw_dome_master_fbo(camera_base, w, h, 0);
}



//-----------
//  Capture
//-----------
void
capture_dome_master(const mtk_camera camera_base, image& img)
{
	gl_frame_buffer fbo_out;

	fbo_out.Init(capture_size, capture_size, false);

	draw_dome_master_fbo(camera_base, capture_size, capture_size, &fbo_out);

	fbo_out.Bind();
	gl_read_pixels(img, 0, 0, capture_size, capture_size);
	fbo_out.Unbind();
}


