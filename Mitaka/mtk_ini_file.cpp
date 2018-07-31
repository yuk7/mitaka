/**************************************************************************************************

Copyright (c) 2012   Tsunehiko Kato
Copyright (c) 2007,2014-2015   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#include  <stdio.h>
#include  "mtk_ini_file.h"
#include  "mtk_dome_master.h"
#include  "mtk_params.h"
#include  "mtk_switches.h"
#include  "mtk_tcp.h"
#include  "mtk_font.h"
#include  "mtk_text.h"
#include  "mtk_screen.h"
#include  "mtk_planetarium.h"
#include  "mtk_joystick.h"
#include  "mtk_title_page.h"
#include  "mtk_end_roll.h"
#include  "mtk_camera_setting.h"
#include  "mtk_directory.h"
#include  "mtk_colors.h"
#include  "mtk_movement.h"
#include  "mtk_solar_system.h"
#include  "mtk_sun.h"
#include  "mtk_satellites.h"
#include  "mtk_minor_planets.h"
#include  "mtk_galaxies.h"
#include  "mtk_screen_menu.h"
#include  "mtk_stereo.h"
#include  "mtk_draw_object_info.h"
#include  "mtk_places.h"
#include  "mtk_draw_places.h"
#include  "mtk_constellation.h"
#include  "mtk_scale_line.h"
#include  "mtk_video.h"
#include  "mtk_mode.h"
#include  "mtk_planet_eclipse.h"
#include  "mtk_speed.h"
#include  "mtk_languages.h"
#include  "mtk_cmb.h"




//----------
//  Extern
//----------
//--- System ---
extern bool bIniEnablePBuffer;
//--- Window ---
extern int _win_x1, _win_y1, _win_w, _win_h, _mgn_w, _mgn_h;
extern const int win_x1_def, win_y1_def, win_w_def, win_h_def;
//--- Network ---
extern bool	bNetwork;			// 通信をするかどうか
extern bool	bControler;			// コントローラーか
//--- Mouse ---
extern bool	bMouseZoomReverse;		// ズーム操作を反転する
extern float   mouse_hide_timeout;	// マウスカーソルを消すまでの時間（0以下で消さない）
//--- Joystick ---
extern int  JoystickAxisIni[4];
extern int	JoystickButtonIni[16];
extern char szIniAxisText[4][20];
extern char szIniButtonText[16][20];
//--- Screen ---
extern const float  screen_z_def;
extern const float  screen_w_def;
extern const float  z_near_def;
extern const float  z_mid_def;
extern const float  obj_z_def;
//--- MovieMode ---
extern int	MovieRendererType;
//--- Date ---
extern float g_date_pos_y;
//--- Performance ---
extern int GalaxyRayNum;		// 銀河系のレイの数
extern float GalaxyRayStepLen;	// 銀河系のレイの積分の最小ステップ長
extern int GalaxyStarNum;		// 銀河系の星の数
extern int TexShrinkRate;		// 読み込み時の縮小率
extern bool bEnableDrawLowRes;	// 動きがあるときに低解像度で描画するか
//---  Lighting  ---
extern float LightingAmbientDef;
//---  Earth  ---
extern float EarthSpecular;
extern float EarthSpecularDef;
extern float EarthShininess;
extern float EarthShininessDef;
extern float AtmosphereIntensity;
extern float AtmosphereIntensityDef;
extern float AtmosphereIntensityPlanetarium;
extern float AtmosphereIntensityPlanetariumDef;
extern int  EarthCityLightsIniDef;
extern bool EarthCityLightsIni;
//---  Sun  ---
extern const char* szSunGlareColorDef;
//---  Star  ---
extern float StarImagePowIndex;
extern float StarImagePowIndexDef;
//--- Time zone ---
extern const float DT_def;
extern float DT;
//--- Font ---
extern float FontSize;
//--- Color ---
extern const char* szMenuTextColorDef;
extern const char* szMenuSelectColorDef;
extern const char* szMenuTitleColorDef;
extern const char* szMenuBgColor1Def;
extern const char* szMenuBgColor2Def;
extern const char* szMenuBdColorDef;
extern const char* szMenuHlColor1Def;
extern const char* szMenuHlColor2Def;
extern RGBf colMenuText, colMenuSelect, colMenuTitle;
extern RGBf colMenuBgColor1, colMenuBgColor2, colMenuBdColor;
extern RGBf colMenuHlColor1, colMenuHlColor2;
extern float LocationNameSize, ConstellationNameSize;
extern float ScaleTextSize, ScaleWidth, DateAlpha, gDateSize;
extern const char* szOrbitWidthDef;
//--- Menu ---
extern float MenuCornerR;
extern float MenuBgAlpha;
extern float MenuBdWidth;
extern float MenuBehindAlpha;
extern float MenuFadingRate;
extern float MenuTriangleDy;


//-------
//  起動
//-------
bool bMultipleLaunch	= false;	// 二重起動
bool bFullScreenInit	= false;	// フルスクリーンで初期化する
bool bEnableSystemMenu	= true;		// システムメニューを表示するか
bool bEnableMovieMake	= false;	// ムービー作成メニューを表示するか

//--------
//  Menu
//--------
float g_MenuPosAngle = 0.0;
float g_MenuRotateAngle = 0.0;


bool	load_servers_dat_file(const char* filename);


//-- Date
const float DateSizeDef		= 0.9f;



//-------
//  sub
//-------
bool
read_ini_bool(const char *section, const char* key, bool def, LPCTSTR file_path)
{
	int val, val0;

	val0 = def ? 1 : 0;
	val = GetPrivateProfileInt(section, key, val0, file_path);
	return (val != 0);
}


float
read_ini_float(const char *section, const char* key, float def, LPCSTR file_path)
{
	const int N = 512;
	char strdef[N], str[N];
	float val;
		
	sprintf(strdef, "%f", def);

	GetPrivateProfileString(section, key, strdef, str, N, file_path);
	if (sscanf(str, "%f", &val)==1)	{ return val; }

	return def;
}




//========================
//  ini ファイルの読み込み
//========================
void
load_ini_file(const char* computer_name)
{
	char ini_file_name[MAX_PATH];
	sprintf(ini_file_name, "mitaka_%s.ini", computer_name);
	{
		FILE *fp = fopen(ini_file_name, "rt");
		if (fp != NULL) {
			fclose(fp);
		} else {
			strcpy(ini_file_name, "mitaka.ini");
		}
	}

	//-- ini ファイル名を取得
    char path[MAX_PATH], file_path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH , path);
	wsprintf(file_path , TEXT("%s\\%s"), path, ini_file_name);


	//-- 設定の読み込み
	char str[256];
	float val;
	

	//--------------------
	//  Float parameters
	//--------------------
	load_float_params_from_ini_file(file_path);


	//------------
	//  [System]
	//------------
	//-- MultipleLaunch
	bMultipleLaunch = read_ini_bool("System", "MultipleLaunch", false, file_path);
	//-- FullScreenInit
	bFullScreenInit = read_ini_bool("System", "FullScreenInit", false, file_path);
	//-- Language
	char lang;
	GetPrivateProfileString("System", "Language", "Default", str, 256, file_path);
	if (stricmp(str, "Default") == 0) {
		char szISO639[100];

		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SISO639LANGNAME, szISO639, 100);
		
		set_default_language_ios639(szISO639);
	}
	else {
		set_default_language_ios639(str);
	}
	


	//-- VSync
	bool bVSYNC = read_ini_bool("System", "VSYNC", true, file_path);
	set_VSYNC(bVSYNC);
	//-- PBuffer
	bIniEnablePBuffer = read_ini_bool("System", "PBuffer", false, file_path);

	

	//-------------
	//  [Network]
	//-------------
	//-- NetworkMode
	bNetwork = read_ini_bool("Network", "NetworkMode", false, file_path);
	//-- Controler 
	bControler = (bNetwork && read_ini_bool("Network", "Controler", false, file_path));
	//-- TCP Port (コントローラは不要）
	TCP_PORT = GetPrivateProfileInt("Network", "Port", TCP_PORT_DEFAULT, file_path);



	//-------------------
	//  [Configuration]
	//-------------------
	set_screen_z( read_ini_float("Configuration", "ScreenZ", screen_z_def,     file_path));
	set_screen_w( read_ini_float("Configuration", "ScreenW", screen_w_def,     file_path));
	set_z_near(   read_ini_float("Configuration", "NearZ",   z_near_def,       file_path));
	set_z_mid(    read_ini_float("Configuration", "MidZ",    z_mid_def,        file_path));
	set_z_far(    read_ini_float("Configuration", "FarZ",    z_mid_def * 8000, file_path));
	set_obj_z(    read_ini_float("Configuration", "ObjZ",    obj_z_def,        file_path));
	set_obj_z( get_obj_z() + get_screen_z() );	//***
	
	float screen_angle = read_ini_float("Configuration", "Angle", 0.0, file_path);
	//-- EyeOffset
	val = read_ini_float("Configuration", "EyeOffset", 0.0, file_path);
	set_eye_offset_x(val);///
	init_local_camera_angle(screen_angle, val);
	//-- EyeCLR
	char CLR = 'C';
	char ch;
	GetPrivateProfileString("Configuration", "EyeCLR", "C", str, 255, file_path);
	if (sscanf(str, "%c", &ch) == 1) {
		if (ch=='c' || ch=='C')	CLR = 'C';
		if (ch=='l' || ch=='L')	CLR = 'L';
		if (ch=='r' || ch=='R')	CLR = 'R';
	}
	set_eye_CLR(CLR);

	//-- EnableChangeEyeDist
	bool bFlg;
	bFlg = read_ini_bool("Configuration", "EnableChangeEyeDist", false, file_path);
	set_eye_dist_change(bFlg);

	set_screen_offset_x(   read_ini_float("Configuration", "ScreenOffsetX",   0.0, file_path) );
	set_title_width_ratio( read_ini_float("Configuration", "TitleWidthRatio", 1.0, file_path) );

	set_plnt_R(0.001*get_z_far());
	//set_plnt_R(0.1*get_z_far());

	set_fade_angle_factor( read_ini_float("Configuration", "FadeAngleFactor", 1.0, file_path) );
	set_stereo_eye_distance( read_ini_float("Configuration", "StereoEyeDist", 6.4, file_path) );


	//------------
	//  [Window]
	//------------
	_win_x1 = GetPrivateProfileInt("Window", "WinX", win_x1_def, file_path);
	_win_y1 = GetPrivateProfileInt("Window", "WinY", win_y1_def, file_path);
	_win_w  = GetPrivateProfileInt("Window", "WinW", win_w_def, file_path);
	_win_h  = GetPrivateProfileInt("Window", "WinH", win_h_def, file_path);
	_mgn_w  = GetPrivateProfileInt("Window", "MgnW", 0, file_path);
	_mgn_h  = GetPrivateProfileInt("Window", "MgnH", 0, file_path);
	// 規格化
	if (_win_w <= 0)	_win_w = 1;
	if (_win_h <= 0)	_win_h = 1;
	if (_mgn_w <  0)	_mgn_w = 0;
	if (_mgn_h <  0)	_mgn_h = 0;



	//---------------
	//  [Directory]
	//---------------
	char szDir[MAX_PATH+1];
	//-- data
	GetPrivateProfileString("Directory", "Data", "\0", szDir, MAX_PATH, file_path);
	if (szDir[0] != '\0')  { set_directory(DIR_DATA, szDir); }
	//-- strings
	GetPrivateProfileString("Directory", "Locale", "\0", szDir, MAX_PATH, file_path);
	if (szDir[0] != '\0')  { set_directory(DIR_LOCALE, szDir); }
	//-- textures
	GetPrivateProfileString("Directory", "Textures", "\0", szDir, MAX_PATH, file_path);
	if (szDir[0] != '\0')  { set_directory(DIR_TEXTURES, szDir); }
	//-- media
	GetPrivateProfileString("Directory", "Media", "\0", szDir, MAX_PATH, file_path);
	if (szDir[0] != '\0')  { set_directory(DIR_MEDIA, szDir); }
	//-- spacecraft
	GetPrivateProfileString("Directory", "Spacecraft", "\0", szDir, MAX_PATH, file_path);
	if (szDir[0] != '\0')  { set_directory(DIR_SPACECRAFT, szDir); }



	//----------
	//  [Mode]
	//----------
	//-- Realtime mode
	SetRealtimeMode( read_ini_bool("Mode", "RealTimeMode", false, file_path) );
	
	//-- Display mode
	int display_mode = DISPMODE_NORMAL;
	GetPrivateProfileString("Mode", "DisplayMode", "Normal", str, 256, file_path);
	if		(_stricmp(str, "Anaglyph")==0)		{ display_mode = DISPMODE_ANAGLYPH;  }
	else if (_stricmp(str, "ColorAnaglyph") == 0)	{ display_mode = DISPMODE_COLOR_ANAGLYPH; }
	else if (_stricmp(str, "SideBySide") == 0)		{ display_mode = DISPMODE_SIDE_BY_SIDE; }
	else if (_stricmp(str, "TopAndBottom") == 0)	{ display_mode = DISPMODE_TOP_AND_BOTTOM; }
	else if (_stricmp(str, "DomeMaster") == 0)	{ display_mode = DISPMODE_DOME_MASTER; }
	set_display_mode(display_mode);


	//---------------------
	//  [PlanetariumMode]
	//---------------------
	float init_lon, init_lat;
	init_lon = read_ini_float("PlanetariumMode", "InitLon", 0.0, file_path);
	init_lat = read_ini_float("PlanetariumMode", "InitLat", 0.0, file_path);
	set_plnt_long(init_lon+180);
	set_plnt_lati(init_lat);



	//----------------
	//  [DomeMaster]
	//----------------
	int dm_buffer_size, dm_capture_size, dm_mesh_N;
	float dm_elev_angle, dm_elev_angle_plnt, dm_label_mag, dm_line_w;
	float dm_R, dm_proj_offset_x, dm_proj_offset_y, dm_proj_offset_z;
	float dm_proj_elev_angle, dm_proj_azimuth, dm_proj_slue, dm_proj_view_angle;
	bool bUseDomeIntersection;
	float dm_intersection_elev_angle, dm_intersection_azimuth;
	float dm_camera_rot_angle;
	dm_buffer_size  = GetPrivateProfileInt("DomeMaster", "BufferSize",  1024, file_path);
	dm_capture_size = GetPrivateProfileInt("DomeMaster", "CaptureSize", 2048, file_path);
	dm_mesh_N       = GetPrivateProfileInt("DomeMaster", "MeshNum",     30, file_path);
	dm_elev_angle	   = read_ini_float("DomeMaster", "ElevationAngle", 30.0, file_path);
	dm_elev_angle_plnt = read_ini_float("DomeMaster", "ElevationAnglePlanetarium", dm_elev_angle, file_path);
	dm_R = read_ini_float("DomeMaster", "DomeR", 500.0, file_path);
	dm_proj_offset_x = read_ini_float("DomeMaster", "ProjOffsetX", 0.0, file_path);
	dm_proj_offset_y = read_ini_float("DomeMaster", "ProjOffsetY", 0.0, file_path);
	dm_proj_offset_z = read_ini_float("DomeMaster", "ProjOffsetZ", 0.0, file_path);
	dm_proj_elev_angle = read_ini_float("DomeMaster", "ProjElevAngle", 90.0, file_path);
	dm_proj_azimuth = read_ini_float("DomeMaster", "ProjAzimuth", 0.0, file_path);
	dm_proj_slue = read_ini_float("DomeMaster", "ProjSlue", 0.0, file_path);
	dm_proj_view_angle = read_ini_float("DomeMaster", "ProjViewAngle", 180.0, file_path);

	bUseDomeIntersection = read_ini_bool("DomeMaster", "UseDomeIntersection", false, file_path);
	dm_intersection_elev_angle = read_ini_float("DomeMaster", "IntersectionElevAngle", 90.0, file_path);
	dm_intersection_azimuth    = read_ini_float("DomeMaster", "IntersectionAzimuth", 0.0, file_path);

	dm_camera_rot_angle = read_ini_float("DomeMaster", "CameraRotationAngle", 0.0, file_path);

	dm_label_mag	= read_ini_float("DomeMaster", "LabelMagnification", 2.0, file_path);
	dm_line_w		= read_ini_float("DomeMaster", "LineWidthFactor", 2.5, file_path);

	if (dm_R <= 0.0) dm_R = 1.0;

	set_dome_master_buffer_size(dm_buffer_size);
	set_dome_master_capture_size(dm_capture_size);
	set_dome_master_mesh_N(dm_mesh_N);
	set_dome_master_dome_R(dm_R);
	set_dome_master_elev_angle(dm_elev_angle);
	set_dome_master_elev_angle_planetarium(dm_elev_angle_plnt);
	set_dome_master_proj_offset( vector3f(dm_proj_offset_x, dm_proj_offset_y, dm_proj_offset_z) );
	set_dome_master_proj_elev_angle(dm_proj_elev_angle);
	set_dome_master_proj_azimuth(dm_proj_azimuth);
	set_dome_master_proj_slue(dm_proj_slue);
	set_dome_master_proj_view_angle(dm_proj_view_angle);
	set_dome_master_intersection_mode(bUseDomeIntersection);
	set_dome_master_intersection_elev_angle(dm_intersection_elev_angle);
	set_dome_master_intersection_azimuth(dm_intersection_azimuth);
	set_dome_master_stereo_rot_angle(dm_camera_rot_angle);
	set_dome_master_label_mag(dm_label_mag);
	set_dome_master_line_width_fac(dm_line_w);



	//---------
	//  Movie
	//---------
	MovieRendererType = RENDERER_WINDOWED_VMR7;
	GetPrivateProfileString("Movie", "Renderer", "Windowed", str, 256, file_path);
	if (_stricmp(str, "Windowed") == 0)				{ MovieRendererType = RENDERER_WINDOWED_VMR7; }
	else if (_stricmp(str, "WindowlessVMR") == 0)	{ MovieRendererType = RENDERER_WINDOWLESS_VMR7; }
	else if (_stricmp(str, "WindowlessVMR9") == 0)	{ MovieRendererType = RENDERER_WINDOWLESS_VMR9; }
	else if (_stricmp(str, "EVR") == 0)				{ MovieRendererType = RENDERER_EVR; }
	SetMovieFadeDuration(read_ini_float("Movie", "FadingDuration", 1.0, file_path));
	


	//-----------------
	//  [Performance]
	//-----------------
	//-- Speed
	float speed;
	speed = read_ini_float("Performance", "Speed", 1.0, file_path);
	set_default_move_speed(speed);
	
	//-- GalaxyRayNum
	GalaxyRayNum  = GetPrivateProfileInt("Performance", "GalaxyRayNum", 6000, file_path);
	if (GalaxyRayNum < 500)		GalaxyRayNum = 500;
	if (GalaxyRayNum > 100000)	GalaxyRayNum = 100000;
	//-- GalaxyRayStepLength
	GalaxyRayStepLen = read_ini_float("Performance", "GalaxyRayStepLength", 40.0f, file_path);
	if (GalaxyRayStepLen < 1)		GalaxyRayStepLen = 1;
	if (GalaxyRayStepLen > 1000)	GalaxyRayStepLen = 1000;
	//-- GalaxyStarNum
	GalaxyStarNum = GetPrivateProfileInt("Performance", "GalaxyStarNum", 2000, file_path);
	if (GalaxyStarNum < 0)		GalaxyStarNum = 0;
	if (GalaxyStarNum > 100000)	GalaxyStarNum = 100000;
	//-- TextureReduction
	TexShrinkRate = GetPrivateProfileInt("Performance", "TextureReduction", 1, file_path);
	if (TexShrinkRate < 1) TexShrinkRate = 1;
	if (TexShrinkRate >16) TexShrinkRate = 16;
	//-- DistantGalaxyThinning
	int thinning;
	thinning = GetPrivateProfileInt("Performance", "DistantGalaxyThinning", 0, file_path);
	SetDistantGalaxyThinning(thinning);
	//-- EnableDrawLowRes
	bEnableDrawLowRes = read_ini_bool("Performance", "EnableDrawLowRes", false, file_path);



	//-------------
	//  [Display]
	//-------------
	float mag_factor;
	mag_factor = read_ini_float("Display", "MagFactor", 1.0, file_path);
	set_mag_factor(mag_factor);

	

	//----------
	//  [Date]
	//----------
	g_date_pos_y = read_ini_float("Date", "PosY", 0.0, file_path);
	


	//-----------
	//  [Mouse]
	//-----------
	int mzr;
	mzr = GetPrivateProfileInt("Mouse", "ZoomReverse", 0, file_path);
	bMouseZoomReverse = (mzr != 0);

	mouse_hide_timeout = read_ini_float("Mouse", "HideTimeout", 3.0, file_path);


	//--------------
	//  [Joystick]
	//--------------
	//---  タイマーインターバル
	int itvl = GetPrivateProfileInt("Joystick", "TimerInterval", 5, file_path);
	set_timer_interval(itvl);
	//---  任意のキーバインド  ---
	for (int i=0; i<4; i++) {
		JoystickAxisIni[i] = GetPrivateProfileInt("Joystick", szIniAxisText[i], i, file_path);
	}
	for (int i=0; i<16; i++) {
		JoystickButtonIni[i] = GetPrivateProfileInt("Joystick", szIniButtonText[i], i, file_path);
	}
	//---  Power Index
	set_joystick_power_index( read_ini_float("Joystick", "PowerIndex", 1.0f, file_path) );
	


	//-------------
	//  [Inertia]
	//-------------
	SetInputTrackballMass(                read_ini_float("Inertia", "TrackballMass",         2.0f, file_path) );
	SetInputTrackballFriction(            read_ini_float("Inertia", "TrackballFriction",     1.5f, file_path) );
	SetInputTrackballMassPlanetarium(     read_ini_float("Inertia", "PlntTrackballMass",     2.0f, file_path) );
	SetInputTrackballFrictionPlanetarium( read_ini_float("Inertia", "PlntTrackballFriction", 8.0f, file_path) );
	SetInputZoomLeverMass(                read_ini_float("Inertia", "ZoomLeverMass",         2.0f, file_path) );
	SetInputZoomLeverFriction(            read_ini_float("Inertia", "ZoomLeverFriction",    10.0f, file_path) );



	//--------------
	//  [Lighting]
	//--------------
	set_lighting_ambient( read_ini_float("Lighting", "Ambient", LightingAmbientDef, file_path) );



	//----------------
	//  [Earth]
	//----------------
	EarthSpecular                  = read_ini_float("Earth", "Specular",                       EarthSpecularDef,                  file_path);
	EarthShininess                 = read_ini_float("Earth", "Shininess",                      EarthShininessDef,                 file_path);
	AtmosphereIntensity            = read_ini_float("Earth", "AtmosphereIntensity",            AtmosphereIntensityDef,            file_path);
	AtmosphereIntensityPlanetarium = read_ini_float("Earth", "AtmosphereIntensityPlanetarium", AtmosphereIntensityPlanetariumDef, file_path);
	//-- CityLights
	int cl;
	cl = GetPrivateProfileInt("Earth", "CityLights", EarthCityLightsIniDef, file_path);
	EarthCityLightsIni = (cl == 1);
	if (EarthCityLightsIni) {
		char sw;
		sw = get_disp_switch( DISPSW_PLANET );
		sw |= DISPM_CITY_LIGHTS;
		set_disp_switch( DISPSW_PLANET, sw );
	}



	//----------------
	//  [Sun]
	//----------------
	GetPrivateProfileString("Sun", "GlareColor", szSunGlareColorDef, str, 256, file_path);
	if (is_colorref(str))	{ set_sun_glare_color( get_color_from_colorref(str) ); }
	else					{ set_sun_glare_color( get_color_from_colorref(szSunGlareColorDef) ); }



	//----------------
	//  [Moon]
	//----------------
	float shdR, shdG;
	shdR = read_ini_float("Moon", "EclipseShadowR", 0.3, file_path);
	shdG = read_ini_float("Moon", "EclipseShadowG", 0.0, file_path);
	set_moon_eclipse_shadow_color(shdR, shdG);



	//----------------
	//  [Star]
	//----------------
	StarImagePowIndex = read_ini_float("Star", "ImagePowIndex", StarImagePowIndexDef, file_path);


	//----------------
	//  [CMB]
	//----------------
	bCMB = read_ini_bool("CMB", "Display", true, file_path);

	float CMB_Alpha, CMB_R1, CMB_R2;
	CMB_Alpha = read_ini_float("CMB", "Brightness", 0.4, file_path);
	CMB_R1 = read_ini_float("CMB", "FadeR1", 0.85, file_path);
	CMB_R2 = read_ini_float("CMB", "FadeR2", 1.15, file_path);
	set_CMB_Alpha(CMB_Alpha);
	set_CMB_fade_scales(CMB_R1, CMB_R2);



	//-------------
	//  [Landing]
	//-------------
	GetPrivateProfileString("Landing", "Mode", "Horizontal", str, 256, file_path);
	if (_stricmp(str, "SurfaceView") == 0)	set_landing_mode(LANDING_MODE_SURFACE_VIEW);
	else									set_landing_mode(LANDING_MODE_HORIZONTAL);



	//--------------
	//  [TimeZone]
	//--------------
	DT = read_ini_float("TimeZone", "DT", DT_def, file_path);



	//-----------
	//  [Title]
	//-----------
	set_title_duration( read_ini_float("Title", "TitleDuration", 0.6f, file_path) );
	set_title_zpos_ini( read_ini_float("Title", "TitleZPosIni",  0.1f, file_path) );



	//----------------
	//  [CreditRoll]
	//----------------
	set_credit_roll_width(    read_ini_float("CreditRoll", "Width",    get_screen_w(), file_path) );
	set_credit_roll_dy(       read_ini_float("CreditRoll", "DY",       0.0f,           file_path) );
	set_credit_roll_duration( read_ini_float("CreditRoll", "Duration", 10.0f,          file_path) );
	set_credit_roll_angle(    read_ini_float("CreditRoll", "Angle",    0.0f,           file_path) );



	//---------------
	//  [ObjectInfo]
	//---------------
	enable_object_info(   read_ini_bool( "ObjectInfo", "Enable", true, file_path) );
	set_object_info_size( read_ini_float("ObjectInfo", "Size",   1.0,  file_path) );



	//----------
	//  [Font]
	//----------
	float ltf_fac = read_ini_float("Font", "Size", 1.4f, file_path);
	if (ltf_fac <= 0.1) ltf_fac = 0.1;
	FontSize = ltf_fac;

	int char_pixel_size, bmp_size;
	char_pixel_size = GetPrivateProfileInt("Font", "CharPixelSize", 64, file_path);
	bmp_size = GetPrivateProfileInt("Font", "BmpSize", 1024, file_path);
	set_font_rendering_sizes(bmp_size, char_pixel_size);



	//----------
	//  [Menu]
	//----------
	set_menu_visible(    read_ini_bool( "Menu", "Visible",     true, file_path) );
	bEnableSystemMenu  = read_ini_bool( "Menu", "SystemMenu",  true, file_path);
	g_MenuPosAngle     = read_ini_float("Menu", "PosAngle",    0.0f, file_path);
	g_MenuRotateAngle  = read_ini_float("Menu", "RotateAngle", 0.0f, file_path);
	set_menu_text_size(  read_ini_float("Menu", "TextSize",    MenuTextSizeDef,    file_path) );
	MenuCornerR        = read_ini_float("Menu", "CornerR",     MenuCornerRDef,     file_path);
	MenuBgAlpha        = read_ini_float("Menu", "BgAlpha",     MenuBgAlphaDef,     file_path);
	MenuBdWidth        = read_ini_float("Menu", "BdWidth",     MenuBdWidthDef,     file_path);
	MenuBehindAlpha    = read_ini_float("Menu", "BehindAlpha", MenuBehindAlphaDef, file_path);
	MenuFadingRate     = read_ini_float("Menu", "FadingRate",  MenuFadingRateDef,  file_path);
	if (MenuFadingRate < 0.1) MenuFadingRate = 0.1;
	MenuTriangleDy     = read_ini_float("Menu", "TriangleDy",  0.0f,               file_path);
	SetMenuFadeDuration( read_ini_float("Menu", "FadingDuration", 0.2f,            file_path) );

	//-- TextColor
	GetPrivateProfileString("Menu", "TextColor", szMenuTextColorDef, str, 256, file_path);
	if (is_colorref(str))	{ colMenuText = get_color_from_colorref(str); }
	else					{ colMenuText = get_color_from_colorref(szMenuTextColorDef); }
	//-- SelectColor
	GetPrivateProfileString("Menu", "SelectColor", szMenuSelectColorDef, str, 256, file_path);
	if (is_colorref(str))	{ colMenuSelect = get_color_from_colorref(str); }
	else					{ colMenuSelect = get_color_from_colorref(szMenuSelectColorDef); }
	//-- TitleColor
	GetPrivateProfileString("Menu", "TitleColor", szMenuTitleColorDef, str, 256, file_path);
	if (is_colorref(str))	{ colMenuTitle = get_color_from_colorref(str); }
	else					{ colMenuTitle = get_color_from_colorref(szMenuTitleColorDef); }
	//-- BgColor1
	GetPrivateProfileString("Menu", "BgColor1", szMenuBgColor1Def, str, 256, file_path);
	if (is_colorref(str))	{ colMenuBgColor1 = get_color_from_colorref(str); }
	else					{ colMenuBgColor1 = get_color_from_colorref(szMenuBgColor1Def); }
	//-- BgColor2
	GetPrivateProfileString("Menu", "BgColor2", szMenuBgColor2Def, str, 256, file_path);
	if (is_colorref(str))	{ colMenuBgColor2 = get_color_from_colorref(str); }
	else					{ colMenuBgColor2 = get_color_from_colorref(szMenuBgColor2Def); }
	//-- BdColor
	GetPrivateProfileString("Menu", "BdColor", szMenuBdColorDef, str, 256, file_path);
	if (is_colorref(str))	{ colMenuBdColor = get_color_from_colorref(str); }
	else					{ colMenuBdColor = get_color_from_colorref(szMenuBdColorDef); }
	//-- HlColor1
	GetPrivateProfileString("Menu", "HlColor1", szMenuHlColor1Def, str, 256, file_path);
	if (is_colorref(str))	{ colMenuHlColor1 = get_color_from_colorref(str); }
	else					{ colMenuHlColor1 = get_color_from_colorref(szMenuHlColor1Def); }
	//-- HlColor2
	GetPrivateProfileString("Menu", "HlColor2", szMenuHlColor2Def, str, 256, file_path);
	if (is_colorref(str))	{ colMenuHlColor2 = get_color_from_colorref(str); }
	else					{ colMenuHlColor2 = get_color_from_colorref(szMenuHlColor2Def); }



	//---------
	//  Color
	//---------
	load_colors_from_ini_file(file_path);
	set_object_name_size(         read_ini_float("Color", "ObjectNameSize",        ObjectNameSizeDef,        file_path) );
	set_special_object_name_size( read_ini_float("Color", "SpecialObjectNameSize", SpecialObjectNameSizeDef, file_path) );
	set_satellites_name_size(     read_ini_float("Color", "SatelliteNameSize",     SatelliteNameSizeDef,     file_path) );
	set_dwarf_planet_name_size(   read_ini_float("Color", "DwarfPlanetNameSize",   DwarfPlanetNameSizeDef,   file_path) );
	set_small_object_name_size(   read_ini_float("Color", "SmallObjectNameSize",   SmallObjectNameSizeDef,   file_path) );
	LocationNameSize =            read_ini_float("Color", "LocationNameSize",      LocationNameSizeDef,      file_path);
	ConstellationNameSize =       read_ini_float("Color", "ConstellationNameSize", ConstellationNameSizeDef, file_path);
	ScaleTextSize =               read_ini_float("Color", "ScaleTextSize",         ScaleTextSizeDef,         file_path);
	ScaleWidth =                  read_ini_float("Color", "ScaleWidth",            ScaleWidthDef,            file_path);
	set_orbit_width(              read_ini_float("Color", "OrbitWidth",            OrbitWidthDef,            file_path) );
	DateAlpha =                   read_ini_float("Color", "DateAlpha",             0.5f,                     file_path);
	gDateSize =                   read_ini_float("Color", "DateSize",              DateSizeDef,              file_path);



	//---------------
	//  [MakeMovie]
	//---------------
	bEnableMovieMake = read_ini_bool("MakeMovie", "Enable", false, file_path);
	GetPrivateProfileString("MakeMovie", "Type", "AVI", str, 256, file_path);
	make_movie_type = MV_AVI;
	if (_stricmp(str, "TARGA") == 0)		{ make_movie_type = MV_TARGA; }
	if (_stricmp(str, "TARGA6") == 0)	{ make_movie_type = MV_TARGA6; }
	if (_stricmp(str, "PNG") == 0)		{ make_movie_type = MV_PNG; }
	if (_stricmp(str, "PNG6") == 0)		{ make_movie_type = MV_PNG6; }
	if (_stricmp(str, "CUBIC") == 0)		{ make_movie_type = MV_CUBIC; }
	char filename_def[256];
	sprintf(filename_def, "movie.avi");
	GetPrivateProfileString("MakeMovie", "FileName", filename_def, mv_filename, 256, file_path);
	sprintf(filename_def, "mv_%c_", get_eye_CLR() );
	GetPrivateProfileString("MakeMovie", "BaseFileName", filename_def, mv_basefilename, 256, file_path);


	//-------------------
	//  DomeMaster Mesh
	//-------------------
	load_dome_master_distortion_map();


	//------------------
	//  glrc のファイル
	//------------------
	{
		FILE* fp;
		char fn[MAX_PATH];
		sprintf(fn, "camera_%s.cfg", computer_name);
		fp = fopen(fn, "rt");
		if (fp == NULL) {
			fp = fopen("camera.cfg", "rt");
		}
		if (fp != NULL) {
			load_glrc_cfg(fp, get_z_near(), get_z_mid(), get_z_far());
			fclose(fp);
		}
	}



	//==================
	//  PC情報の読み込み
	//==================
	if (bControler) {
		bool res = load_servers_dat_file("servers.dat");
		if (!res)  {
			bControler = false;
		}
	}
}


//------------------------
//  servers.dat の読み込み
//------------------------
bool
load_servers_dat_file(const char* filename)
{
	FILE *fp;
	fp = fopen(filename, "rt");
	if (fp == NULL)  return  false;

	svr_num = 0;
	int i=0;
	while (true) {
		int count;
		count =	fscanf(fp, "%s %d", szSvrName[i], &svr_port[i]);

		if (count != 2)  break;
		i++;
		if (i > svr_num_max) break;
	}
	svr_num = i;
	fclose(fp);

	return true;
}


//-------
//  取得
//-------
float
get_menu_pos_angle()
{
	return g_MenuPosAngle;
}

float
get_menu_rotate_angle()
{
	return g_MenuRotateAngle;
}
