/**************************************************************************************************

Copyright (c) 2007   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#include  "mtk_screen.h"

#include  "mtk_camera_setting.h"







static  quaterniond  base_ori(1,0,0,0);		// 基準座標系

static  quaterniond  camera_ori;	// 基準座標系でのカメラの姿勢
static  vector3d     camera_pos;	// 基準系で表したカメラの位置

static float g_local_camera_angle = 0;	// カメラの回転角（3面シアター）

// GL 用変換行列
static double Mbc_GL[16];		// 基準系からカメラ座標系への変換行列 
static double Mcb_GL[16];		// カメラ座標系から基準系への変換行列


// frustum
static frustum  frs_near, frs_far;
static bool bLoadFrustum = false;


//  視差を変えるときの（右目の）移動方向(デフォルトは x 軸上）
static vector3d  eye_shift_dirc(1,0,0);



void
set_local_camera(const quaterniond& ori, const vector3d& r0)
{
	double M[16];

	camera_ori = ori;
	camera_pos = r0;


	//--- カメラ座標系から基準座標系への変換行列 ---
	quaternion_to_matrix_GL(camera_ori, M);
	for (int i=0; i<3; i++) {
		int idx = 4*i;
		Mcb_GL[idx  ] = M[idx  ];
		Mcb_GL[idx+1] = M[idx+1];
		Mcb_GL[idx+2] = M[idx+2];
		Mcb_GL[idx+3] = 0.0;
	}
	Mcb_GL[12] = r0.x;
	Mcb_GL[13] = r0.y;
	Mcb_GL[14] = r0.z;
	Mcb_GL[15] = 1.0;


	//--- 基準座標系からカメラ座標系への変換行列 ---
	quaternion_to_matrix_GL(camera_ori.inv(), M);
	for (int i=0; i<3; i++) {
		int idx = 4*i;
		Mbc_GL[idx  ] = M[idx  ];
		Mbc_GL[idx+1] = M[idx+1];
		Mbc_GL[idx+2] = M[idx+2];
		Mbc_GL[idx+3] = 0.0;
	}
	// カメラ座標系での基準座標系の原点の位置
	for (int i=0; i<3; i++) {
		double val = 0.0;
		for (int j=0; j<3; j++) {
			val += -M[4*j+i] * Mcb_GL[12+j];
		}
		Mbc_GL[12+i] = val;
	}
	Mbc_GL[15] = 1.0;
}



// ３面シアターの場合
void
set_local_camera_sub(const float eye_offset)
{
	quaterniond  ori;

	ori = make_rotation_quaternion(double(-g_local_camera_angle*D2R), vector3d(0,1,0));
	
	vector3d eF, eU, eR;
	quaternion_to_axes(ori, eF, eU, eR);

	eye_shift_dirc = eR;	// 目のずれの方向

	set_local_camera(ori, double(eye_offset) * eye_shift_dirc );
}

void
init_local_camera_angle(const float angle, const float eye_offset)
{
	g_local_camera_angle = angle;

	set_local_camera_sub(eye_offset);
}

void
reset_local_camera_angle(const float eye_offset)
{
	set_local_camera_sub(eye_offset);
}


void
set_local_camera_pos(const vector3d& r0)
{
	camera_pos = r0;

	//--- カメラ座標系から基準座標系への変換行列 （位置のみ更新）---
	Mcb_GL[12] = r0.x;
	Mcb_GL[13] = r0.y;
	Mcb_GL[14] = r0.z;
	Mcb_GL[15] = 1.0;

	//--- 基準座標系からカメラ座標系への変換行列 （位置のみ更新）---
	// カメラ座標系での基準座標系の原点の位置
	for (int i=0; i<3; i++) {
		double val = 0.0;
		for (int j=0; j<3; j++) {
			val += -Mbc_GL[4*j+i] * Mcb_GL[12+j];
		}
		Mbc_GL[12+i] = val;
	}
	Mbc_GL[15] = 1.0;
}


void
set_local_camera_pos(const float eye_offset)
{
	set_local_camera_pos( double(eye_offset) * eye_shift_dirc );
}




quaterniond
get_local_camera_orientation()
{
	return  camera_ori;
}

vector3d
get_local_camera_pos()
{
	return  camera_pos;
}



const double*
get_local_camera_matrix_GL()
{
	return  Mbc_GL;
}

const double*
get_local_camera_matrix_GL_inv()
{
	return  Mcb_GL;
}

vector3d
get_eU_of_base_on_local_camera()
{
	return  vector3d(Mbc_GL[4], Mbc_GL[5], Mbc_GL[6]);
}

vector3d
get_base_origin_on_local_camera()
{
	return  vector3d(-Mbc_GL[12], -Mbc_GL[13], -Mbc_GL[14]);
}




bool
load_local_camera_setting(FILE* fp)
{
	int count;
	float a, b, c, d;
	float x, y, z;

	//  位置と姿勢は、基準座標系での値
	count = fscanf(fp, "%f %f %f %f", &a, &b, &c, &d);		// 姿勢
	if (count != 4)  return false;
	count = fscanf(fp, "%f %f %f", &x, &y, &z);				// 位置
	if (count != 3)  return false;

	quaterniond ori(a, b, c, d);
	ori.normalize();
	vector3d r0(x,y,z);

	set_local_camera(ori, r0);

	return  true;
}



void
set_base_camera(const quaterniond& ori)
{
	base_ori = ori;
}

quaterniond
get_base_camera_ori()
{
	return  base_ori;
}


bool
load_base_camera_setting(FILE* fp)
{
	int count;
	float a, b, c, d;

	//  基準座標系の姿勢を設定
	count = fscanf(fp, "%f %f %f %f", &a, &b, &c, &d);
	if (count != 4)  return false;

	quaterniond ori(a, b, c, d);
	ori.normalize();

	set_base_camera(ori);

	return  true;
}




void
set_frustum_sub(frustum& frs, float l, float r, float b, float t, float n, float f, float z1, float z2)
{
	float x1, x2, y1, y2, a;
	a  = z1 / n;
	x1 = a * l;
	x2 = a * r;
	y1 = a * b;
	y2 = a * t;
	frs.set(x1, x2, y1, y2, z1, z2);
}


bool
load_frustum(FILE* fp, float z_near, float z_mid, float z_far)
{
	int count;
	float l, r, b, t, n, f;

	// 
	count = fscanf(fp, "%f %f %f %f %f %f", &l, &r, &b, &t, &n, &f);
	if (count != 6)  return false;

	//--- near
	set_frustum_sub(frs_near, l, r, b, t, n, f, z_near, z_mid);

	//--- far
	set_frustum_sub(frs_far, l, r, b, t, n, f, z_mid, z_far);


	bLoadFrustum = true;
	return  true;
}


bool
load_glrc_cfg(FILE* fp, float z_near, float z_mid, float z_far)
{
	int count;
    char token[1024];

	quaterniond  ori(1,0,0,0);
	vector3d  r(0,0,0);

    while (fscanf(fp, "%s", token)==1)
    {
        if (strcmp(token, "Offset")==0) {
        }
        else if (strcmp(token, "Translate")==0) {
            float x, y, z;
            count = fscanf(fp, "%f %f %f", &x, &y, &z);
			if (count != 3)  continue;
			r.set(100*x, 100*y, 100*z);
        }
        else if (strcmp(token, "Rotate")==0) {
            float angle, x, y, z;
            count = fscanf(fp, "%f %f %f %f", &angle, &x, &y, &z);
			if (count != 4)  continue;
			ori = make_rotation_quaternion(double(angle)*3.14159265/180.0, vector3d(x, y, z)) * ori;
        }
        else if (strcmp(token, "Scale")==0) {
        }
        else if (strcmp(token, "Lens")==0) {
        }
        else if (strcmp(token, "Frustum")==0) {
            float l, r, b, t, n, f;
            count = fscanf(fp, "%f %f %f %f %f %f", &l, &r, &b, &t, &n, &f);
			if (count != 6)  continue;

			set_frustum_sub(frs_near, l, r, b, t, n, f, z_near, z_mid);
			set_frustum_sub(frs_far,  l, r, b, t, n, f, z_mid,  z_far);
			bLoadFrustum = true;
        }
    }


	ori.normalize();
	set_local_camera(ori, r);




    return true;
}




bool
isFrustumLoaded()
{
	return bLoadFrustum;
}

frustum
get_loaded_frustum_near()
{
	return  frs_near;
}


frustum
get_loaded_frustum_far()
{
	return  frs_far;
}






//--- 視差 ---
static float eye_distance_2 = 6.4;

float
get_eye_distance_2()
{
	return eye_distance_2;
}


void
set_eye_distance_2(float d, char CLR)
{
	eye_distance_2 = d;
	
	float offset;
	switch ( CLR ) {
		case 'L':	offset = -0.5*d;	break;
		case 'R':	offset =  0.5*d;	break;
		default:
		case 'C':	offset =  0.0;		break;
	}

	set_local_camera_pos( offset );
	
	set_eye_offset_x( offset );
}
