/**************************************************************************************************

Copyright (c) 2012   Tsunehiko Kato
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
#include  "mitaka.h"
#include  "mtk_mode.h"
#include  "mtk_movement.h"
#include  "mtk_trackball.h"
#include  "mtk_switches.h"
#include  "mtk_planetarium.h"
#include  "mtk_camera.h"
#include  "mtk_floating_object.h"
#include  "mtk_stereo.h"




//--  Externs
extern mtk_camera	g_camera;
void	OnUpdateViewCtrl();
void	OnUpdateViewCtrl2();
void	process_zoom(const float& _zm);
void	OnMenuFade(float fac);
void	OnMovieFade(float fac);
void	OnScreenAlpha(float fac);
void	OnPresentTime();
//--  Const
//--  Trackballs / Levers
static mtk_trackball  tb;			// Rotate/Roll
static mtk_lever lvr_zm;			// Zoom
static mtk_lever lvr_lon, lvr_lat;	// Planetarium
static mtk_lever lvr_pzm;
//--  Forces
static float F_rot_x, F_rot_y, F_rol;
static float F_zoom;
static float F_lon, F_lat, F_zoom_plnt;
//--  MenuFade
static float menu_fade_factor = 0;
static float menu_fade_velocity = 5;
static float menu_fade_duration = 0.2;
//--  MovieFade
static bool  bMovieStart = false;
static float movie_fade_alpha    = 0;
static float movie_fade_duration = 2;
//--  ScreenFade
static float screen_alpha = 1;
//--  Realtime mode
static bool bRealtimeMode = false;
//--  Mouse cursor
static bool bCursorToBeHidden = false;
static bool bHideMouseCursor = false;
static float mouse_cursor_counter = 0;



//--------
//  Init
//--------
void
ResetInputForces()
{
	F_rot_x = F_rot_y = F_rol = 0.0;
	F_zoom = 0.0;
	F_lon = F_lat = 0.0;
	F_zoom_plnt = 0.0;
}

void
SetInputTrackballMass(float m)
{
	tb.set_density(m);
}

void
SetInputTrackballFriction(float mu)
{
	tb.set_friction(mu);
}

void
SetInputZoomLeverMass(float m)
{
	lvr_zm.set_mass(m);
	lvr_pzm.set_mass(m);
}

void
SetInputZoomLeverFriction(float mu)
{
	lvr_zm.set_friction(mu);
	lvr_pzm.set_friction(mu);
}

void
SetInputTrackballMassPlanetarium(float m)
{
	lvr_lon.set_mass(m);
	lvr_lat.set_mass(m);
}

void
SetInputTrackballFrictionPlanetarium(float mu)
{
	lvr_lon.set_friction(mu);
	lvr_lat.set_friction(mu);
}


//--------------
//  Space Mode
//--------------
void
SetInputForce3D(float _F_rot_x, float _F_rot_y, float _F_rol, float _F_zoom)
{
	F_rot_x = _F_rot_x;
	F_rot_y = _F_rot_y;
	F_rol   = _F_rol;
	F_zoom  = _F_zoom;
}

void
SetInputForcePlanetarium(float _F_lon, float _F_lat, float _F_zoom_plnt)
{
	F_lon = _F_lon;
	F_lat = _F_lat;
	F_zoom_plnt = _F_zoom_plnt;
}

void
SetInputAngularVelocity3D(const vector3f& Omg)
{
	tb.set_angular_velocity( Omg );
}

void
SetInputZoomSpeed3D(float v)
{
	lvr_zm.set_velocity(v);
}

float
GetInputZoomFriction3D()
{
	return  lvr_zm.get_friction();
}


//---------------
//  Planetarium
//---------------
void
SetInputLonLatSpeedPlanetarium(float vlon, float vlat)
{
	lvr_lon.set_velocity(vlon);
	lvr_lat.set_velocity(vlat);
}

void
SetInputZoomSpeedPlanetarium(float v)
{
	lvr_pzm.set_velocity(v);
}

float
GetInputZoomFrictionPlanetarium()
{
	return  lvr_pzm.get_friction();
}


//------------
//  MenuFade
//------------
void
SetMenuFadeFactor(float val)
{
	menu_fade_factor = val;
}

float
GetMenuFadeFactor()
{
	return  menu_fade_factor;
}

void
SetMenuFadeVelocity(float v)
{
	menu_fade_velocity = v;
}

void
SetMenuFadeDuration(float t)
{
	if (t < 1e-4)  t = 1e-4;
	menu_fade_duration = t;
}

float
GetMenuFadeDuration()
{
	return  menu_fade_duration;
}


//-------------
//  MovieFade
//-------------
void
SetMovieStart(bool flg)
{
	bMovieStart = flg;
}

bool
GetMovieStart()
{
	return  bMovieStart;
}

void
SetMovieFadeAlpha(float val)
{
	movie_fade_alpha = val;
}

float
GetMovieFadeAlpha()
{
	return  movie_fade_alpha;
}

void
SetMovieFadeDuration(float val)
{
	movie_fade_duration = val;
}

float
GetMovieFadeDuration()
{
	return  movie_fade_duration;
}


//--------------
//  ScreenFade
//--------------
void
SetScreenAlpha(float val)
{
	screen_alpha = val;
}

float
GetScreenAlpha()
{
	return  screen_alpha;
}


//-----------------
//  Realtime Mode
//-----------------
void
SetRealtimeMode(bool flg)
{
	bRealtimeMode = flg;
}

bool
GetRealtimeMode()
{
	return  bRealtimeMode;
}


//----------------
//  Mouse Cursor
//----------------
void
SetMouseCursorCounter(float duration)
{
	if (bHideMouseCursor) {
		while (ShowCursor(TRUE) < 0) {}
		bHideMouseCursor = false;
	}

	mouse_cursor_counter = duration;
}

void
ProcessMouseCursor()
{
	if (bCursorToBeHidden) {
		while (ShowCursor(FALSE) >= 0) {}
		bHideMouseCursor = true;
		bCursorToBeHidden = false;
	}
}





//--------------------
//  Process Movement
//--------------------
bool
ProcessMovement()
{
	const float dt = get_timer_interval() / 1000.0f;

	bool  flg = false;
	
	if (get_mode() == MODE_3D) {
	
		float ang = tb.proceed_time(dt, F_rot_x, F_rot_y, F_rol);
		//if (ang > 1e-4) {
		if (ang > 1e-8) {
			quaterniond ori = tb.get_dori();
			g_camera.set_ori( g_camera.get_ori() * ori );
			flg = true;
			OnUpdateViewCtrl();
		}

		float v = lvr_zm.proceed_time(dt, F_zoom);
		if (fabs(v) > 1e-4) {
			float zmr = lvr_zm.get_dx();
			process_zoom(zmr);
			flg = true;
		}
	} else {

		float vlon = lvr_lon.proceed_time(dt, F_lon);
		float vlat = lvr_lat.proceed_time(dt, F_lat);
		if (fabs(vlon) > 1e-4 || fabs(vlat) > 1e-4) {
			float lon, lat;
			lon = get_plnt_long();
			lat = get_plnt_lati();

			lon += lvr_lon.get_dx();
			if (lon > 360.0)	lon -= 360.0;
			if (lon <  0.0)		lon += 360.0;
			lat += lvr_lat.get_dx();
			if (lat > 90.0)		lat = 90.0;
			if (lat <  0.0)		lat = 0.0;


			set_plnt_long(lon);
			set_plnt_lati(lat);

			flg = true;
			OnUpdateViewCtrl2();
		}

		float v = lvr_pzm.proceed_time(dt, F_zoom_plnt);
		if (fabs(v) > 1e-4) {
			float zmr = lvr_pzm.get_dx();
			process_zoom(zmr);
			flg = true;
		}
	}

	//--  Menu Fade  --
	if (fabs(menu_fade_velocity) > 1e-4) {
		menu_fade_factor += menu_fade_velocity * dt;
		if (menu_fade_factor > 1) {
			menu_fade_factor = 1;
			menu_fade_velocity = 0;
		} else if (menu_fade_factor < 0) {
			menu_fade_factor = 0;
			menu_fade_velocity = 0;
		}
		OnMenuFade(menu_fade_factor);
		flg = true;
	}

	//--  Movie Fade  --
	if (bMovieStart && movie_fade_alpha < 1) {
		if (movie_fade_duration <= 1e-3) {
			movie_fade_alpha = 1;
		} else {
			movie_fade_alpha += dt / movie_fade_duration;
			if (movie_fade_alpha > 1)  movie_fade_alpha = 1;
		}
		OnMovieFade(movie_fade_alpha);
		flg = true;
	}

	//--  Screen Fade  --
	if (screen_alpha < 1) {
		screen_alpha += dt / (movie_fade_duration);
		if (screen_alpha > 1)  screen_alpha = 1;
		OnScreenAlpha(screen_alpha);
		flg = true;
	}

	//--  RealtimeMode
	static bool bRealtimeModeFirst = true;
	static DWORD RealtimeModeInterval = 100;
	static DWORD last_time;
	if (bRealtimeMode) {
		DWORD t = timeGetTime();
		if (bRealtimeModeFirst || t - last_time > RealtimeModeInterval) {
			bRealtimeModeFirst = false;
			OnPresentTime();
			last_time = t;
			flg = true;
		}
	}

	//--  Floating object
	if ( is_changing_target() ) {
		process_changing_target();
		flg = true;
	}

	//--  Change scale
	if ( is_changing_scale() ) {
		process_changing_scale();
		flg = true;
	}

	//--  Mouse cursor
	if (mouse_cursor_counter > 0) {
		mouse_cursor_counter -= dt;
		if (mouse_cursor_counter <= 0) {
			mouse_cursor_counter = 0;
			bCursorToBeHidden = true;
		}
	}


	return  flg;
}


