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
#include  "mtk_mode.h"
#include  "mtk_screen.h"
#include  "mtk_target.h"
#include  "mtk_camera_setting.h"
#include  "mtk_planets.h"
#include  "../common/astro/astro_const.h"


//----------
//  Screen
//----------
static float screen_z;
static float screen_w;
static float z_near;
static float z_mid;
static float z_far2;
static float ini_obj_z;
static float fade_angle_factor = 1.0f;

//-------
//  Eye
//-------
char   eye_CLR = 'C';	// 視点の位置
float  eye_offset_x = 0.0f;
float  screen_offset_x = 0.0f;	// スクリーンのずらし幅

// ステレオグラムの作成用
float eye_distance = 30;	// ステレオグラムでのみ使用

static bool bEnableChangeEyeDist = false;


//-----------
//  スケール
//-----------
const double scale_max	= 2e13;						// 最大のスケール

static double  g_scale			= 3.0 * _1AU_pc;	// 距離のスケール(pc)の初期値
static double g_scale_last;							// 直前の値（ポップアップ判定用　OnSetZoomでセット)


float
get_screen_z()
{
	return  screen_z;
}

float
get_screen_w()
{
	return  screen_w;
}

float
get_z_near()
{
	return  z_near;
}

float
get_z_mid()
{
	return  z_mid;
}

float
get_z_far()
{
	return  z_far2;
}

float
get_obj_z()
{
	return  ini_obj_z;
}

void
set_screen_z(float z)
{
	screen_z = z;
}

void
set_screen_w(float w)
{
	screen_w = w;
}

void
set_z_near(float z)
{
	z_near = z;
}

void
set_z_mid(float z)
{
	z_mid = z;
}

void
set_z_far(float z)
{
	z_far2 = z;
}

void
set_obj_z(float z)
{
	ini_obj_z = z;
}




// 視野を設定する（近傍）
frustum
get_frustum_near(float aspc_rt)
{
	frustum frs;
	if (isFrustumLoaded()) {
		frs = get_loaded_frustum_near();
	} else {
		const float dx = eye_offset_x - screen_offset_x;
		frs.set2(aspc_rt, screen_z, screen_w, z_near, z_mid, dx);
	}
	return  frs;
}


// 視野を設定する（遠方）
frustum
get_frustum_far(float aspc_rt)
{
	frustum frs;

	if (isFrustumLoaded()) {
		frs = get_loaded_frustum_far();
	} else {
		const float dx = eye_offset_x - screen_offset_x;
		frs.set2(aspc_rt, screen_z, screen_w, z_mid, z_far2, dx);
	}
	return frs;
}



//-------
//  Eye
//-------
float
get_eye_offset_x()
{
	return eye_offset_x;
}

void
set_eye_offset_x(float x)
{
	eye_offset_x = x;
}

float
get_screen_offset_x()
{
	return screen_offset_x;
}

void
set_screen_offset_x(float x)
{
	screen_offset_x = x;
}

char
get_eye_CLR()
{
	return eye_CLR;
}

void
set_eye_CLR(char CLR) {
	eye_CLR = CLR;
}

float
get_eye_distance()
{
	return eye_distance;
}

void
set_eye_distance(float dist)
{
	eye_distance = dist;
}

bool
is_eye_dist_change_enabled()
{
	return bEnableChangeEyeDist;
}

void
set_eye_dist_change(bool bFlg)
{
	bEnableChangeEyeDist = bFlg;
}



//---------
//  Scale
//---------
double
get_clst_R()
{
	return screen_w / 180.0 * 2000.0;
}

double
get_current_scale()
{
	return g_scale;
}

double
get_scale_last()
{
	return  g_scale_last;
}

void
set_scale(const double& scl)
{
	//****
	g_scale_last = g_scale;

	g_scale = scl;
	if (g_scale > scale_max)	{ g_scale = scale_max; }

	const mtk_object&  tgt = get_target();
	const int type = tgt.get_type();

	if (type == OBJ_TYPE_PLANET) {
		const double R = get_planet_Re(get_mode(), tgt.get_index()) * _1km_pc;
		const double scale_min = 5e-4 * R;
		if (g_scale < scale_min) g_scale = scale_min;
	}

	if (type == OBJ_TYPE_OUR_GALAXY || type == OBJ_TYPE_BLACKHOLE) {
		const double scale_min = 20 * _1AU_pc;
		if (g_scale < scale_min) g_scale = scale_min;
	}
}

double
calc_scale_factor(const double& scl)
{
	return get_clst_R() / scl;
}

double
get_scale_factor()
{
	return calc_scale_factor( get_current_scale() );
}

float
get_Rmax()
{
	return 0.5 * z_far2 / get_scale_factor(); 
}



//--------------
//  Fade angle
//--------------
void
set_fade_angle_factor(float fac)
{
	fade_angle_factor = fac;
}

float
get_fade_angle_factor()
{
	return  fade_angle_factor;
}
