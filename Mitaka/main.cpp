/**************************************************************************************************

Copyright (c) 2012        Tsunehiko Kato
Copyright (c) 2007-2008,2014-2016   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
//---------------------------------------------------
// Mitaka   (Hipparcos 3D Planetarium)
//  written by Tsunehiko N. Kato   2003/2/18
//  2003/2/19, 2/27, 3/7, 3/14, 3/19, 3/31, 4/1, 4/20
//      4/28, 5/4, 5/9, 5/23, 5/31, 6/5, 6/7, 6/8
//      6/25, 7/23, 7/25, 7/28-8/1, 8/3, 9/9, 9/18-19
//      9/22,24-25, 10/6-7, -10/26, 11/27
//   beta1: 05/2/1
//   beta2: 05/2/16
//   beta3: 05/2/22
//          05/6/24
//   beta4: 05/10/13
//   beta5: 06/7/20
//   beta6: 06/11/25
//   version 1.0: 07/05/11
//   version 1.0.1: 07/07/18
//   -> Mitaka++ 1.0:  07/09/02
//   -> Mitaka version 1.2: 08/05/26
//   -> Mitaka version 1.2.1: 12/06/24
//   -> Mitaka version 1.2.1a: 12/06/30
//   -> Mitaka version 1.2.2 beta:  12/9/26
//   -> Mitaka version 1.2.2 beta2: 12/11/10
//   -> Mitaka version 1.2.2 beta3: 13/10/13
//   Mitaka version 1.2.2 RC1: 14/12/10
//   Mitaka version 1.2.2: 14/12/15
//   Mitaka version 1.2.2a: 15/1/26
//   Mitaka version 1.2.2b: 15/2/12
//   Mitaka version 1.2.3: 15/3/10
//   Mitaka version 1.2.3a: 15/3/16
//   Mitaka version 1.2.4: 15/6/12
//   Mitaka version 1.2.4a: 15/6/24
//   Mitaka version 1.2.5: 15/7/29
//   Mitaka version 1.3.0: 15/11/24
//   Mitaka version 1.3.0a: 15/12/16
//   Mitaka version 1.3.1: 16/3/4
//---------------------------------------------------
#include  <windows.h>
#include  <commdlg.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <tchar.h>
#include  <process.h>
#include  <string>

#define GLEW_STATIC
#include  <gl/glew.h>


#include  "../common/win_file.h"
#include  "../common/astro/constellation.h"
#include  "../common/math/quaternion.h"
#include  "../common/math/spline.h"
#include  "../common/graphics/gl_common.h"
#include  "../common/graphics/gl_win.h"
#include  "../common/graphics/gl_Model3d.h"
#include  "../common/graphics/3ds_loader.h"
#include  "../common/graphics/obj_loader.h"
#include  "../common/graphics/gl_triangle_mesh.h"
#include  "../common/graphics/gl_image.h"
#include  "../common/graphics/color.h"
#include  "../common/win_dlg.h"
#include  "../common/win_ctrl.h"
#include  "../common/interface/trackball.h"
#include  "../common/graphics/dib.h"
#include  "../common/graphics/avi.h"
#include  "Mitaka.h"
#include  "mtk_mode.h"
#include  "mtk_OpenGL.h"
#include  "mtk_tcp.h"
#include  "mtk_movie.h"
#include  "mtk_joystick.h"
#include  "mtk_menu.h"
#include  "mtk_movement.h"
#include  "mtk_switches.h"
#include  "mtk_ini_file.h"
#include  "mtk_colors.h"
#include  "mtk_command.h"
#include  "mtk_menu_bar.h"
#include  "mtk_dialog_box.h"
#include  "mtk_target.h"
#include  "mtk_preset_target.h"
#include  "mtk_camera.h"
#include  "mtk_camera_setting.h"
#include  "mtk_screen.h"
#include  "mtk_draw_struct.h"
#include  "mtk_cpu_info.h"
#include  "mtk_window_mode.h"
#include  "mtk_network_mode.h"
#include  "mtk_screen_button.h"
#include  "mtk_title_page.h"
#include  "mtk_end_roll.h"
#include  "mtk_image.h"
#include  "mtk_on_screen.h"
#include  "mtk_time.h"
#include  "mtk_gl_common.h"
#include  "mtk_3D_model.h"
#include  "mtk_scale_line.h"
#include  "mtk_planetarium.h"
#include  "../common/JulianDate.h"
#include  "mtk_stars.h"
#include  "mtk_stars_2.h"
#include  "mtk_galactic_center.h"
#include  "mtk_blackhole.h"
#include  "mtk_constellation.h"
#include  "mtk_objects.h"
#include  "mtk_object_position.h"
#include  "mtk_planets.h"
#include  "mtk_planet_position.h"
#include  "mtk_planet_orientation.h"
#include  "mtk_solar_system.h"
#include  "mtk_floating_object.h"
#include  "mtk_sun.h"
#include  "mtk_satellites.h"
#include  "mtk_satellite_orientation.h"
#include  "mtk_minor_planets.h"
#include  "mtk_oort_cloud.h"
#include  "mtk_globular_clusters.h"
#include  "mtk_VERA.h"
#include  "mtk_our_galaxy.h"
#include  "mtk_nearby_galaxies.h"
#include  "mtk_spacecraft.h"
#include  "mtk_space_curve.h"
#include  "mtk_galaxies.h"
#include  "mtk_our_galaxy_rt.h"
#include  "mtk_elliptical_galaxy.h"
#include  "mtk_cmb.h"
#include  "mtk_gc_model.h"
#include  "mtk_atmosphere.h"
#include  "mtk_3D_chart.h"
#include  "mtk_font.h"
#include  "mtk_text.h"
#include  "mtk_places.h"
#include  "mtk_draw_places.h"
#include  "../common/frame_rate.h"
#include  "mtk_stereo.h"
#include  "mtk_pbuffer.h"
#include  "mtk_save_image.h"
#include  "mtk_directory.h"
#include  "mtk_draw_object_info.h"
#include  "mtk_screen_menu.h"
#include  "mtk_preset_scene.h"
#include  "mtk_dome_master.h"
#include  "mtk_speed.h"
#include  "mtk_languages.h"
#include  "mtk_topography.h"
#include  "mtk_selection.h"
#include  "mtk_date.h"
#include  "mtk_strings.h"
#include  "../common/text/unicode_string.h"
#include  "mtk_thread.h"
#include  "mtk_location.h"
#include  "mtk_target_planetarium.h"
#include  "resource.h"
using namespace std;




// for Visual Studio
#define snprintf _snprintf




//-----------
//  Externs
//-----------

//*****
//-- window mode
extern mtk_screen_button_minus sbTimeMinus, sbZoomMinus;
extern mtk_screen_button_plus  sbTimePlus,  sbZoomPlus;
//-- network mode
extern bool	bNetwork;
extern bool	bControler;
extern bool bDraw;
extern bool bDrawing;
//-- menu
extern menu				menu_root;
extern menu_selection	mnuSelect;




//--- Version ---
char szAppName[] = TEXT("Mitaka");
const char szVersion[] = TEXT("1.3.1");
char szTitle[256];


//--- Windows Version
OSVERSIONINFO osvi;
static bool bVistaOrLater;


//--- OpenGL Version
const GLubyte *szGLVersion;




//--- Critical Section
static CS cs_draw;



//--- Window ---
HINSTANCE	g_hInst;
HWND		g_hWnd = NULL;
HCURSOR		hCursorArrow = NULL;


int disp_w, disp_h;	// スクリーンのサイズ

int win_x0, win_y0;
int win_x1, win_y1, win_x2, win_y2;
int win_w, win_h, win_w0, win_h0;
int win_mgn_w1, win_mgn_h1;
int win_mgn_w2, win_mgn_h2;


int  gl_x0, gl_y0, gl_w, gl_h;
bool	bActive;


//--- Wnidow size ---
int _win_x1, _win_y1, _win_w, _win_h, _mgn_w, _mgn_h;





//--- OpenGL ---
COpenGL		ogl;
bool	bFBO_Available = false;


//--- Frame Rate ---
fps_counter  FrmCounter(1);
fps_counter  FrmCounter2(1);




//-------
//  状態
//-------
mtk_camera	g_camera;	// 現在のカメラの状態




//-------
//  状態
//-------
//--  Draw
bool bMotion			= false;
bool bPrevMotion		= false;
bool bDrawLowRes		= false;
bool bEnableDrawLowRes  = false;
//--  Movie
bool  bMovieMode		= false;	// ムービーを開いているか
bool  bMoviePlaying		= false;	// ムービーの再生中か


//-- Setting mode
enum {
	SETTING_MODE_NONE,
	SETTING_MODE_DATE,
	SETTING_MODE_EYE_DISTANCE
};
char chSettingModeDisp = SETTING_MODE_NONE;


//--  Movie
int	MovieRendererType = RENDERER_WINDOWED_VMR7;




//--------------
//  描画用構造体
//--------------
static mtk_draw_struct g_latest_draw_struct[2];

void set_latest_draw_struct(const mtk_draw_struct& ds, int idx) { g_latest_draw_struct[idx] = ds; }

mtk_draw_struct
get_latest_draw_struct(int idx)
{
	return  g_latest_draw_struct[idx];
}



//---------
//  Color
//---------
float DateAlpha, gDateSize;



//--------
//  Font
//--------
float FontSize;
float vFontSize[5] = {
	3.0, 2.0, 1.4, 1.0, 0.6
};



//-------
//  字幕
//-------
//vector<gltext>  vSubtitles;
int subtitles_idx = 0;











//------------------------------------
//  着陸地点（初期設定は国立天文台の位置）
//------------------------------------
float  landing_lon, landing_lat;



//------------
//  TimeZone
//------------
float DT;



//--------------
//  銀河系モデル
//--------------
vector3f  GC;


bool bData;								// データが正常に読み込まれたか






//------------
//  Textures
//------------
GLuint   texMilkyWay;




//-----------------
//  Display lists
//-----------------
GLuint	dlSquare;
GLuint  dlClstSphere;








//-----------
//  Program
//-----------
char  tyProgram = 0;

// プログラム用フラグ
bool	bFlg3;



//-------------
//  time step
//-------------
char  tyTimeStep			= TIMESTEP_10MIN;	// タイムステップ


char  tyMilkyWay			= MILKYWAY_VISIBLE;		// 天の川の種類

bool  bTest					= false;		// テスト用




//-----------------
//  星のマークモード
//-----------------
char tyStarMark = STARMARK_NONE;



//-------------
//  3Dチャート
//-------------
static char  no_3DChart = 0;




//----------------
//  アニメーション
//----------------
int animation_counter0 = 0;
int animation_counter  = 0;
int animation_interval = 1;
int animation_type = 0;



//---------
//  Movie
//---------
MediaFiles MVF("MOVIE");
mtk_movie  dsmv2;



//-- Audio
MediaFiles AUDF("AUDIO");




//---------
//  Image
//---------
MediaFiles IMGF("IMAGE");
mtk_image	ImageStill;
static bool	bMilkyWayImageLoaded = false;

// 表示する画像のインデックス
static char	current_image_index = 0;





//---------
//  Popup
//---------
enum popup_type {
	POPUP_TEXT,
	POPUP_IMAGE
};

struct popup_info {
	bool	bFlg;		// 描画したら false
	double  scale;		// 起動するスケール
	char	type;		// ポップアップのタイプ
	char	idx;		// インデックス
};

const int popup_num = 2;
popup_info
PopUp[popup_num] = {
	{ true,  10*_1AU_pc, POPUP_IMAGE, 0  },
	{ true,  3,         POPUP_IMAGE, 1  }
};



//--------
//  Date
//--------
float g_date_pos_y = 0.0;


//--------------
//  Prototypes
//--------------
void	Input();
float	get_animation_frac();	// 現在のアニメーションの進行の割合を返す
void	init_popup(const char nProgram);




//-----------
//  Pointer
//-----------
float pointer_angle_x = 0.0;
float pointer_angle_y = 0.0;
float pointer_z;








//---------
//  Color
//---------
const RGBAf    col_clst_grd(0.5,0.4,0.4,0.8);		// celestial grid
const RGBAf    col_glx_grd(0.0,0.7,0.6,0.9);		// galactic grid
const RGBAf    col_RA_DEC(0.6,0.4,0.4,0.8);			// RA, DEC






mtk_draw_struct calc_draw_struct(const mtk_camera& camera_base, float aspc_rt, int _vp_x1, int _vp_x2, int _vp_y1, int _vp_y2, vector3f dr, bool bRotBB, float line_w);




bool
isVistaOrLater()
{
	return bVistaOrLater;
}





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Init


bool
load_data()
{
	FILE *fp;
	directory  dir        = get_directory(DIR_DATA);
	directory  dir_locale = get_directory(DIR_LOCALE);


	

	//-----------
	//  Strings
	//-----------
	//SetSplashText("文字列データを読み込み中...", "Loading string data...");
	cs_draw.Enter();
	select_default_language(dir_locale);
	cs_draw.Leave();


	//-------------
	//  Selection
	//-------------
	load_selection(dir);


	//-------------------
	//  Floating object
	//-------------------
	register_floating_object();



	//-------
	//  Sun
	//-------
	register_object_sun();


	//-----------
	//  Planets
	//-----------
	SetSplashText("LOADING_PLANETS");
	load_Sun_position(dir.get_path("Sun.ppi"));
	load_Venus(dir.get_path("Venus.ppi"));
	load_Pluto_BC(dir.get_path("Pluto_BC.ppi"));
	load_CF96( dir.get_path("cf96.dat") );
	register_object_planets();
	init_planets_orbital_elements();
	register_planets_orbit_curves();



	//--------------
	//  Satellites
	//--------------
	load_satellites(dir);
	register_satellites_orbit_curves();


	//---------------------------
	//  Load Small Objects data
	//---------------------------
	SetSplashText("LOADING_MINOR_PLANETS");
	if ((fp = dir.fopen("small_objects.dat", "rt")) == NULL)  return false;
	load_small_objects(fp);
	fclose(fp);

	register_small_objects_orbit_curves();



	//--------------
	//  Oort Cloud
	//--------------
	SetSplashText("INIT_OORT_CLOUD");
	init_Oort_cloud();
	register_object_Oort_cloud();


	//---------
	//  Stars
	//---------
	SetSplashText("LOADING_STARS");
	bData = false;

	//***
	bool res;
	if ((fp=dir.fopen("hip.dat", "rb")) != NULL) {		// open the Hipparcos catalogue
		res = load_Hipparcos_data_2(fp);
		fclose(fp);
		bData = res;
	}

	
	//-----------------------------
	//  Load stars with exoplanet
	//-----------------------------
	if ((fp = dir.fopen("stars_exoplanet.dat", "rb")) != NULL) {
		res = load_stars_with_exoplanet(fp);
		fclose(fp);
	}
	register_object_stars();
	
	//  make star names
	make_star_name_text_2(swStar, swConstellation);


	//------------------------
	//  Load Exoplanets List
	//------------------------
	SetSplashText("LOADING_EXOPLANETS");
	load_exoplanet_list( dir.get_path("exoplanets_HIP_list.dat") );


	//------------------------------
	//  S-stars at Galactic center
	//------------------------------
	load_S_stars(dir);
	SetSplashText("INIT_S_STARS");


	//-------------
	//  BlackHole
	//-------------
	init_blackhole(dir.get_path("ref_angle.dat") );


	//-----------------
	//  Init star pos
	//-----------------
	init_pow_for_stars();
	


	//--------------------
	//  Globular Cluster
	//--------------------
	SetSplashText("LOADING_GLOBULAR_CLUSTERS");
	res = load_globular_cluster_data(dir);
	init_globular_cluster_points();
	
	SetSplashText("LOADING_GLOBULAR_CLUSTER_MODEL");
	load_GC_model(dir);
	

	//---------------
	//  VERA object
	//---------------
	SetSplashText("LOADING_VERA_OBJECTS");
	res = load_VERA_objects(dir);


	//--------------------
	//  Our Galaxy Model
	//--------------------
	register_object_our_galaxy();
	SetSplashText("LOADING_OUR_GALAXY_MODEL");
	load_our_galaxy_model(dir);


	//-------------------
	//  Nearby Galaxies
	//-------------------
	SetSplashText("LOADING_NEARBY_GALAXIES");
	res = load_nearby_galaxies(dir);


	//---------------
	//  Init M87 GC
	//---------------
	init_M87_GC();


	//-----------------
	//  SDSS Galaxies
	//-----------------
	SetSplashText("LOADING_DISTANT_GALAXIES");
	res = load_distant_galaxies(dir);


	//-------------
	//  SDSS QSOs
	//-------------
	SetSplashText("LOADING_QSOS");
	res = load_QSOs(dir);
	

	//-------------------------------
	//  Load Constellation Data 2
	//-------------------------------
	SetSplashText("LOADING_CONSTELLATIONS");
	load_constellations(dir);


	//-----------------------------
	//  Load Constellation illust
	//-----------------------------
	SetSplashText("LOADING_CONSTELLATION_ILLUSTS");
	load_constellation_illusts(dir);


	//-----------------
	//  Load NGC data
	//-----------------
	SetSplashText("LOADING_NGC_DATA");

	if ((fp=dir.fopen("ngc.dat", "rb")) != NULL) {		// open the NGC catalogue
		res = read_NGC_data(fp);
		fclose(fp);
	}

	if ((fp=dir.fopen("names.dat", "rb")) != NULL) {	// open the NGC common name catalogue
		set_NGC_name(fp);
		fclose(fp);
	}


	//--------------
	//  Atmosphere
	//--------------
	init_atmospehre();


	//----------------------
	//  Earth's atmosphere
	//----------------------
	SetSplashText("LOADING_EARTH_ATMOSPHERE");
	init_multiple_scattering( dir.get_path("emmap.dat") );



	//----------
	//  Places
	//----------
	SetSplashText("LOADING_PLACES");
	load_places(dir.get_path("places.dat"));



	//--------------
	//  Topography
	//--------------
	SetSplashText("LOADING_TOPOGRAPHY");
	load_topography_all(dir);

	init_planet_places_and_topography();
	init_satellite_places_and_topography();




	//--------------
	//  Spacecraft
	//--------------
	SetSplashText("LOADING_SPACECRAFT");
	init_spacecraft();
	register_spacecraft();


	//---------------
	//  Scale Lines
	//---------------
	register_scale_circle_curves();


	return res;
}










//
//
//
void
init_window_size(int x, int y, int w, int h, int mgn_w, int mgn_h)
{
	// スクリーンサイズ
	disp_w = GetSystemMetrics( SM_CXSCREEN );
	disp_h = GetSystemMetrics( SM_CYSCREEN );


	// ウィンドウサイズの補正
	if (!bNetwork) {
		RECT rc;
		rc.left = 0;
		rc.top = 0;
		rc.right = w;
		rc.bottom = h;
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, TRUE);
		w = rc.right - rc.left;
		h = rc.bottom - rc.top;
	}

	// 表示領域の設定
	int x1,y1, x2,y2;
	if (w<=0)  w=1;
	if (h<=0)  h=1;
	x1 = x;
	y1 = y;
	x2 = x + w - 1;
	y2 = y + h - 1;

	if (x1<0)  x1=0;
	if (y1<0)  y1=0;
	if (x2>=disp_w)  x2 = disp_w - 1;
	if (y2>=disp_h)  y2 = disp_h - 1;
	w = x2 - x1 + 1;
	h = y2 - y1 + 1;

	win_x1 = x1;
	win_y1 = y1;
	win_x2 = x2;
	win_y2 = y2;
	win_w  = w;
	win_h  = h;




	// マージンの設定
	if (mgn_w < 0)  mgn_w = 0;
	if (mgn_h < 0)  mgn_h = 0;
	x1 = win_x1 - mgn_w;
	y1 = win_y1 - mgn_h;
	x2 = win_x2 + mgn_w;
	y2 = win_y2 + mgn_h;
	if (x1<0)  x1=0;
	if (y1<0)  y1=0;
	if (x2>=disp_w)  x2 = disp_w - 1;
	if (y2>=disp_h)  y2 = disp_h - 1;

	win_x0 = x1;
	win_y0 = y1;
	win_w0 = x2 - x1 + 1;
	win_h0 = y2 - y1 + 1;



	win_mgn_w1 = win_x1 - win_x0;
	win_mgn_h1 = win_y1 - win_y0;
	win_mgn_w2 = x2 - win_x2;
	win_mgn_h2 = y2 - win_y2;


	// OpenGL 用の表示領域の設定
	gl_x0 = win_x1 - win_x0;
	gl_y0 = win_y1 - win_y0;
	gl_w  = win_w;
	gl_h  = win_h;
}




void
init()
{
	//-----------------------
	//  ウィンドウサイズの設定
	//-----------------------
	init_window_size(_win_x1, _win_y1, _win_w, _win_h, _mgn_w, _mgn_h);


	//-- Disp switches
	init_disp_switches();


	//-- Load data
	load_data();


	//--- joystick
	init_joystick();

	//--- Target ---
	init_target();

	//--- Preset targets ---
	init_preset_target();

	
	//--- Date ---
	//-- 当日の夜８時に設定する--
	UTC lt = get_present_LocalTime(DT);
	double t = get_JD(lt.year, lt.month, lt.day, 20, 0, 0.0) - DT/24.0; 

	t += 1e-8;	//***** to avoid intel fast error

	init_time_JD(t);
	

	//-- popup
	init_popup(tyProgram);
}




//----------
//  OpenGL
//----------
void	draw_spherical_grid();
void	draw_grid_digit(const float r, const float sz);



void
load_milkyway_texture()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	if (texMilkyWay != 0) {
		DeleteTextureName(texMilkyWay);
	}

	switch (tyMilkyWay) {
		case MILKYWAY_VISIBLE:
			texMilkyWay = LoadTexture("milkyway.jpg", TexShrinkRate, false);
			break;
		case MILKYWAY_VISIBLE2:
            texMilkyWay = LoadTexture("milkyway2.jpg", TexShrinkRate, false);
			break;
		case MILKYWAY_IR:
			texMilkyWay = LoadTexture("milkyway_IR.jpg", TexShrinkRate, false);
			break;
		case MILKYWAY_CMB:
			texMilkyWay = LoadTexture("cmb.jpg", TexShrinkRate, false);
			break;
		default:
			return;
	}
}





void
initGL()
{
	//---  glew
	GLenum	glew_error;
	glew_error = glewInit();
	if (glew_error != GLEW_OK) {
		//printf("error: %s\n", glewGetErrorString(glew_error));
	}



	szGLVersion = glGetString(GL_VERSION);

	glEnable( GL_DEPTH_TEST );

	glClearColor( 0.0, 0.0, 0.0, 0.0 );
	glClearDepth( get_z_mid() );
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);


	glHint(GL_POINT_SMOOTH_HINT, GL_DONT_CARE);
	glHint(GL_LINE_SMOOTH_HINT,  GL_NICEST);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glReadBuffer(GL_BACK);
	glDrawBuffer(GL_BACK);


	// extentions
	gl_init_extensions();

	bFBO_Available = glewIsSupported("GL_ARB_framebuffer_object");
	if (!bFBO_Available) {
		if (get_display_mode() == DISPMODE_DOME_MASTER) {
			set_display_mode(DISPMODE_NORMAL);
		}
	}




	// 垂直同期
	gl_set_interval( get_VSYNC() );




	//----------
	//  Lights
	//----------
	glEnable(GL_LIGHT0);
	GLfloat ambient[]  = {0.1, 0.1, 0.1, 1.0 };
	GLfloat diffuse[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0 };

	glLightfv( GL_LIGHT0, GL_AMBIENT, ambient );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );
	glLightfv( GL_LIGHT0, GL_SPECULAR, specular );
	glShadeModel(GL_SMOOTH);

	GLfloat ambient1[]  = {0.5, 0.5, 0.5, 1.0 };
	GLfloat diffuse1[]  = {0.5, 0.5, 0.5, 1.0 };
	GLfloat position1[] = {0.0, 0.0, 1.0, 0.0 };
	glLightfv( GL_LIGHT1, GL_AMBIENT,  ambient1 );
	glLightfv( GL_LIGHT1, GL_DIFFUSE,  diffuse1 );
	glLightfv( GL_LIGHT1, GL_POSITION, position1 );
	glDisable(GL_LIGHT1);
	GLfloat ambient2[]  = {0.55, 0.55, 0.55, 1.0 };
	GLfloat diffuse2[]  = {0.99, 0.99, 0.99, 1.0 };
	GLfloat position2[] = {0.0, 0.0, 0.0, 1.0 };
	glLightfv( GL_LIGHT2, GL_AMBIENT,  ambient2 );
	glLightfv( GL_LIGHT2, GL_DIFFUSE,  diffuse2 );
	glLightfv( GL_LIGHT2, GL_POSITION, position2 );
	glDisable(GL_LIGHT2);

	//-----------------
	//  Display lists
	//-----------------
	SetSplashText("INIT_DISPLAY_LIST");

	//-- 天球グリッドのディスプレイリストの作成
	dlClstSphere = glGenLists(1);
	glNewList(dlClstSphere, GL_COMPILE);
		draw_spherical_grid();
	glEndList();

	// 板
	// 半径でスケーリングして使用する
	dlSquare = glGenLists(1);
	glNewList(dlSquare, GL_COMPILE);
		glBegin(GL_QUADS);
			glTexCoord2f(1,1);
			glVertex3f( 1, 1,0);
			glTexCoord2f(0,1);
			glVertex3f(-1, 1,0);
			glTexCoord2f(0,0);
			glVertex3f(-1,-1,0);
			glTexCoord2f(1,0);
			glVertex3f( 1,-1,0);
		glEnd();
	glEndList();



	// 銀河のディスプレイリスト
	init_galaxy_display_list();

	


	//-----------
	//  Texture
	//-----------
	directory  dir = get_directory(DIR_TEXTURES);
	SetTextureDir( dir );

	glDisable(GL_TEXTURE_2D);
	image  img;

	//--  Star
	SetSplashText("INIT_STAR_TEXTURE");
	init_star_textures();

	//--  Milky way
	SetSplashText("LOADING_MILKYWAY_TEXTURE");
	texMilkyWay = 0;
	load_milkyway_texture();

	//--  galaxy
	SetSplashText("LOADING_GALAXY_TEXTURE");
	load_galaxies_textures();


	//--  Solar system
	load_solar_system_textures();

	//--  Galactic center
	load_galactic_center_texture();

	//-- CMB
	load_CMB_texture();

	//--  bind textures for 3D models
	bind_3D_model_textures();


	//--
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPointSize(1);


	SetSplashText("LOADING_COMPLETED");
}


//
//
//
void
goto_center_screen2()
{
	float rot_ang = 0.0;

	if (get_display_mode() == DISPMODE_DOME_MASTER && get_mode() == MODE_PLANETARIUM) {
		rot_ang = get_dome_master_elev_angle() - get_dome_master_elev_angle_planetarium();
	}

	goto_center_screen(rot_ang);
}



//----------------
//  グリッドを描く
//----------------
void
draw_spherical_grid()
{
	const float a = 3.1415926535f / 180.0f;
	const float b = 3.1415926535f / 12.0f;

	glBegin(GL_LINES);

	//-- 経線 --
	for (int i=0; i<24; i+=2) {
		float  phi, cs,sn;
		phi = b * float(i);
		cs = cos(phi);
		sn = sin(phi);

		for (int j=0; j<180; j+=2) {
			float th, rho, x,y,z;

			th  = a * float(j);
			z   = cos(th);
			rho = sin(th);
			x   = rho * cs;
			y   = rho * sn;
			glVertex3f(x,y,z);

			th  = a * float(j+2);
			z   = cos(th);
			rho = sin(th);
			x   = rho * cs;
			y   = rho * sn;
			glVertex3f(x,y,z);
		}
	}


	//-- 緯線 --
	for (int i=0; i<180; i+=15) {
		float th, rho, z;
		th  = a * float(i);
		z   = cos(th);
		rho = sin(th);

		for (int j=0; j<360; j+=2) {
			float phi,x,y;

			phi = a * float(j);
			x   = rho * cos(phi);
			y   = rho * sin(phi);
			glVertex3f(x,y,z);

			phi = a * float(j+2);
			x   = rho * cos(phi);
			y   = rho * sin(phi);
			glVertex3f(x,y,z);
		}
	}

	glEnd();
}


//---------------------
//  グリッドの数値を描く
//---------------------
void
draw_grid_digit(const float r, const float sz)
{
	const float  a = pi / 180.0f;
	const float  b = pi / 12.0f;


	// 経度
	for (int i=0; i<24; i+=2) {
		float th_d,phi_d;
		th_d  = 0 + 0.3;
		phi_d = float(i)/12.0f*180 -0.3;
		
		glPushMatrix();
			glRotatef(phi_d-90,0,0,1);
			glRotatef(th_d,1,0,0);
			glTranslatef(0,r,0);
			glRotatef(90, 1,0,0);

			glScalef(sz,sz,sz);
			draw_textf("%d\\Sh", i);
		glPopMatrix();
	}

	// 緯度
	for (int i=-90; i<=90; i+=15) {
		float th_d,phi_d;
		th_d  = float(i) + 0.3;
		phi_d = 0 -0.3;
		
		glPushMatrix();
			glRotatef(phi_d-90,0,0,1);
			glRotatef(th_d,1,0,0);
			glTranslatef(0,r,0);
			glRotatef(90, 1,0,0);

			glScalef(sz,sz,sz);
			draw_textf("%d\xc2\xb0", i);
		glPopMatrix();
	}

}


void
draw_textured_square(const float& sz)
{
	glPushMatrix();
		glScalef(sz,sz,sz);
		glCallList(dlSquare);
	glPopMatrix();
}





//==========
//  Target
//==========
void	OnCommandDraw();


void
change_target(int tgt_ID)
{
	set_target(tgt_ID);
	double  ini_scale = get_target().get_scale();
	
	if (ini_scale > get_current_scale() ) {
        OnSetZoom( ini_scale );
	}
}






///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Draw


//---------------------
//  赤経・赤緯の線を描く
//---------------------
void
draw_celestial_grid(const mtk_draw_struct& ds, bool bDigit)
{
	const float Rmax = (ds.tyMode == MODE_PLANETARIUM)  ?  1.00005  :  get_Rmax();

	const float lw = 2.0 * ds.line_width_fac / (ds.rad_per_pixel / 0.001);
	glLineWidth(lw);

	glColor4fv(col_clst_grd.get_pointer());
	glPushMatrix();
		go_to_equ_coord();	// 赤道座標系へ
		draw_grid_digit(Rmax, Rmax/40.0f * 0.5 * ds.ltf);
		glScalef(Rmax,Rmax,Rmax);
		glCallList(dlClstSphere);
	glPopMatrix();
}




//---------------------
//  銀経・銀緯の線を描く
//---------------------
void
draw_galactic_grid(const mtk_draw_struct& ds, bool bDigit)
{
	const float Rmax = (ds.tyMode == MODE_PLANETARIUM)  ?  1.00005 :  get_Rmax();

	const float lw = 2.0 * ds.line_width_fac / (ds.rad_per_pixel / 0.001);
	glLineWidth(lw);
	glColor4fv(col_glx_grd.get_pointer());
	glPushMatrix();
		go_to_galactic_coord();	// 銀河座標系へ
		draw_grid_digit(Rmax, Rmax / 40.0f * 0.5 * ds.ltf);
		glScalef(Rmax,Rmax,Rmax);
		glCallList(dlClstSphere);
	glPopMatrix();
}


//------------
//  黄道を描く
//------------
void
draw_ecliptic(const mtk_draw_struct& ds)
{
	float x,y,th;


	const float lw = 2.0 * ds.line_width_fac / (ds.rad_per_pixel / 0.001);

	glEnable(GL_DEPTH_TEST);
	if (ds.tyMode==MODE_PLANETARIUM) {
		glColor3f(1,1,0);
		glLineWidth(lw);
		glBegin(GL_LINE_LOOP);
			for (int i=0; i<360; i+=5) {
				th = i*pi/180;
				x = cos(th);
				y = sin(th);
				glVertex3f(x,y,0);
			}
		glEnd();
	} else {
		const vector3d camera_pos = ds.camera.get_pos();

		float r = 100.0 * _1AU_pc / ds.scale;
		if (r < 0.1) return;

		float af = 1.0f;
		if (r < 0.5) af = (r-0.1)/0.4;
		

		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);

		glPushMatrix();
			glTranslatef(-camera_pos.x, -camera_pos.y, -camera_pos.z);


			float sz = 100.0 * _1AU_pc;

			glColor4f(0.8,0.8,0,af);
			glLineWidth(2*lw);
			glBegin(GL_LINES);
				glVertex3f(0,0,0);
				glVertex3f(sz,0,0);
			glEnd();
			glLineWidth(lw);
			glBegin(GL_LINES);
				glVertex3f(0,0,0);
				glVertex3f(-sz,0,0);
				glVertex3f(0,sz,0);
				glVertex3f(0,-sz,0);
			glEnd();

			glColor4f(1,1,0,0.1*af);
			glBegin(GL_QUADS);
				glVertex3f(sz,sz,0);
				glVertex3f(-sz,sz,0);
				glVertex3f(-sz,-sz,0);
				glVertex3f(sz,-sz,0);
			glEnd();
		glPopMatrix();
		glPopAttrib();
	}
}






//--------------
//  天の川を描く
//--------------
void
draw_milkyway(const mtk_draw_struct& ds)
{
	if (tyMilkyWay == MILKYWAY_NONE)  return;


	float af, Rmax;
	af = 1.0f;
	if (ds.tyMode == MODE_3D) {
		const float dist = norm(get_sun_position() - ds.camera_base.get_pos());
		const float dist1 = 2000;
		const float dist2 = 3000;
		if (dist > dist1) {
			float t = (dist - dist1)/(dist2 - dist1);
			af = 1-t;
		}

		if (af <= 0.0) return;
		Rmax = get_Rmax();
	} else {
		Rmax  = 1.0;
	}

	//-- 読み込んでいない時は読み込む
	if (!bMilkyWayImageLoaded) {
		load_milkyway_texture();
		bMilkyWayImageLoaded = true;
	}

	if (tyMilkyWay == MILKYWAY_CMB) {
		af *= 0.25f;
	}


	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, texMilkyWay);

	
	glColor4f(1,1,1,af);
	glPushMatrix();
		go_to_galactic_coord();
		glScalef(Rmax, Rmax, Rmax);
		glRotatef(180, 0,0,1);
		glFrontFace(GL_CW);
		gl_draw_sphere(10, true);
		glFrontFace(GL_CCW);
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}






//----------------
//  字幕を表示する
//----------------
void
draw_subtitles()
{
	/*
	const int subtitles_lines = vSubtitles.size();
	if (subtitles_idx < 0 || subtitles_idx >= subtitles_lines)  return;


	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);

	// 透明度
	float af = 1.0f;


	glPushMatrix();
		goto_center_screen();
		const float scl0 = get_screen_w() / 180.0;
		glScalef(scl0,scl0,scl0);

		float wh = 110;

		glTranslatef(0,0,-20);
		glTranslatef(-wh,0,0);
		glTranslatef(0, -90, 0);

		float w0 = 2*wh;
		

		glColor4f(0.7,0.7,0.7,af);
		const float sz = 8;
		glScalef(sz,sz,sz);

		//gltext&  txt = vSubtitles[subtitles_idx];
		//txt.draw_center(w0/sz, get_font());
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
	*/
}




//----------------
//  時刻を表示する
//----------------
void
draw_date(const mtk_draw_struct& ds)
{
	const bool bRealtime = GetRealtimeMode();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);

	// 透明度
	float af = DateAlpha;

	if (ds.tyMode == MODE_3D) {
		const float R1 = 2.0;
		const float R2 = 20.0;
		float r = ds.scale / _1ly_pc;
		if (r > R2) return;
		if (r > R1)  {
			af *= 1-(r-R1)/(R2-R1);
		}
	}


	glPushMatrix();
		goto_center_screen2();
		const float scl = get_screen_w() / 180.0;
		glScalef(scl,scl,scl);

		glTranslatef(0,0,-20);

		



		const float sz = 10;
		glScalef(sz,sz,sz);

		glTranslatef(0, g_date_pos_y, 0);
		if (bSubtitles || subtitles_idx != 0) {
			glTranslatef(0,+9,0);	// 字幕モードのときは上側に表示
		} else {
			glTranslatef(0,-8,0);
		}


		const float f2 = 0.66 * gDateSize * (ds.ltf / 1.2f);
		glScalef(f2, f2, f2);


		// Color
		int col_idx = (bRealtime) ? MTK_COLOR_DATE_REALTIME_MODE : MTK_COLOR_DATE;
		const RGBf& col = get_color(col_idx);
		glColor4f(col.r, col.g, col.b, af);


		UTC utc = JD_to_UTC(ds.JD + DT/24.0);
		
		const float  spc = 1.0f;
		const float  shft = -0.5f;
		std::string date;
		char tm[100];

		//  Adjust position
		date = make_date_string(0000, utc.month, 00);
		if (bRealtime) {
			sprintf(tm, "%02d:%02d:%02d", 00, 00, 00);
		}
		else {
			sprintf(tm, "%02d:%02d", 00, 00);
		}
		float wtot, w, h;
		get_text_size(date, wtot, h);
		get_text_size(tm, w, h);
		wtot += w;
		wtot += spc;
		glTranslatef(-0.5f*wtot + shft, 0, 0);


		//  Date and time
		date = make_date_string(utc.year, utc.month, utc.day);
		if (bRealtime) {
			int isec = int(utc.sec);
			sprintf(tm, "%02d:%02d:%02d", utc.hour, utc.min, isec);
		}
		else {
			sprintf(tm, "%02d:%02d", utc.hour, utc.min);
		}


		//  Draw
		draw_text(date);
		glTranslatef(spc, 0, 0);
		draw_text(tm);

	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
}




//----------------
//  Setting Mode
//----------------
void
draw_setting_mode()
{
	if (chSettingModeDisp == SETTING_MODE_NONE)  return;

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);

	// 透明度
	float af = 0.6f;


	glPushMatrix();
		goto_center_screen2();

		const float scl = get_screen_w() / 180.0;
		glScalef(scl,scl,scl);
		glTranslatef(0,0,-20);

		glColor4f(0.9,0.8,0.6,af);
		const float sz = 10;
		glScalef(sz,sz,sz);

		if (bSubtitles || subtitles_idx != 0) {
			glTranslatef(0,+8,0);	// 字幕モードのときは上側に表示
		} else {
			glTranslatef(0,-6.5,0);
		}


		const char szTimeStepKey[][20] = {
			"TIMESTEP_10SEC",
			"TIMESTEP_1MIN",
			"TIMESTEP_10MIN",
			"TIMESTEP_1HOUR",
			"TIMESTEP_4HOURS",
			"TIMESTEP_1DAY",
			"TIMESTEP_1WEEK",
			"TIMESTEP_1MONTH",
			"TIMESTEP_1YEAR",
			"TIMESTEP_10YEARS",
			"TIMESTEP_100YEARS"
		};

		char szEyeDist[100];
		std::string str;

		switch (chSettingModeDisp) {
			case SETTING_MODE_DATE:
				str = get_string(szTimeStepKey[tyTimeStep]);
				break;
			case SETTING_MODE_EYE_DISTANCE:
				sprintf(szEyeDist, "%2.1f cm", get_eye_distance_2());
				str = szEyeDist;
				break;
			default:
				str = "";
				break;
		}
		draw_text_line(str, TEXT_CENTER, false);

	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
}




//---------------
//  FPSを表示する
//---------------
void
draw_fps()
{
	static char str[50];
	
	float fps;
	fps = 1000.0 / (FrmCounter.get_elapsed_time() + FrmCounter2.get_elapsed_time());
	sprintf(str, "fps = %2.1f", fps);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glColor3f(1,1,1);

	int mode = get_display_mode();
	if (mode == DISPMODE_DOME_MASTER) {
		glPushMatrix();
		goto_center_screen2();
		const float scl = get_screen_w() / 180.0;
		glScalef(scl, scl, scl);
		glTranslatef(0, 0, -20);

		glColor4f(0.7, 0.8, 1, 0.8);
		const float sz = 12;
		glScalef(sz, sz, sz);
		glTranslatef(0, -7, 0);

		draw_text_centering(str);
		glPopMatrix();
	}
	else {
		gl_enter_2D_mode();
		const float sz = 20;
		glScalef(sz, -sz, sz);
		glTranslatef(0.1, -1, 0);
		draw_text(str);
		gl_leave_2D_mode();

	}

	glEnable(GL_DEPTH_TEST);
}


//-----------------------------------
//  アプリケーション・タイトルを表示する
//-----------------------------------
void
draw_app_title()
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);

	const float alpha = 0.6f;
	glPushMatrix();
		goto_center_screen2();
		const float scl = get_screen_w() / 180.0;
		glScalef(scl,scl,scl);
		glTranslatef(0,0,-20);

		glColor4f(0.7,0.8,1,alpha);
		const float sz = 50;
		glScalef(sz,sz,sz);
		glTranslatef(0,1,0);

		draw_text_centering(szAppName);
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
}

//-------------------------
//  バージョン情報を表示する
//-------------------------
void
draw_about()
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);

	const float alpha = 0.6f;
	glPushMatrix();
		goto_center_screen2();
		const float scl = get_screen_w() / 180.0;
		glScalef(scl,scl,scl);

		glColor4f(0.4,0.6,0.8,alpha);
		float sz = 20;
		glScalef(sz,sz,sz);

		const int N = 100;
		char str[N];

		snprintf(str, N, "Version %s", szVersion);
		draw_text_line(str, TEXT_CENTER, true);

		const float sc = 0.6;
		glScalef(sc, sc, sc);

		snprintf(str, N, "%d Minor Planets", get_small_objects_num());
		draw_text_line(str, TEXT_CENTER, true);
		
		snprintf(str, N, "%d Stars", get_stars_num());
		draw_text_line(str, TEXT_CENTER, true);


		snprintf(str, N, "%d Nearby Galaxies", get_number_of_nearby_galaxies());
		draw_text_line(str, TEXT_CENTER, true);

		snprintf(str, N, "%d Distant Galaxies", get_number_of_distant_galaxies());
		draw_text_line(str, TEXT_CENTER, false);
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
}


//-----------------------
//  デバッグ情報を表示する
//-----------------------
void
draw_debug_info()
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);

	const float alpha = 0.6f;
	glPushMatrix();
		goto_center_screen2();
		const float scl = get_screen_w() / 180.0;
		glScalef(scl,scl,scl);

		glColor4f(0.4,0.6,0.8,alpha);
		float sz = 6;
		glScalef(sz,sz,sz);

		const int N = 100;
		static char str[N];

		glTranslatef(0,-4,0);

		snprintf(str, N, "L_ref = %.2e", get_L_ref());
		draw_text_line(str, TEXT_CENTER, true);

		snprintf(str, N, "Julian Date = %lf", get_time_JD());
		draw_text_line(str, TEXT_CENTER, true);
		
		const float scale = get_current_scale();
		snprintf(str, N, "Scale = %.3le pc", scale);
		draw_text_line(str, TEXT_CENTER, true);

		snprintf(str, N, "Log Scale = %.3le  Log L_r_e_f = %.3le", log10(scale), get_log_L_ref());
		draw_text_line(str, TEXT_CENTER, true);

		snprintf(str, N, "Window size = (%d,%d)", win_w, win_h);
		draw_text_line(str, TEXT_CENTER, false);
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
}



//---------------------
//  Display View Info
//---------------------
void
draw_view_info(const mtk_draw_struct& ds)
{
	const vector3d  camera_pos = ds.camera.get_pos();
	const vector3d  target_r = ds.tgt_pos;

	double dist = norm(target_r - camera_pos);
	const float pc2m = _1pc_meter;

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);


	const float alpha = 0.6f;
	glPushMatrix();
		goto_center_screen2();
		const float scl = get_screen_w() / 180.0;
		glScalef(scl, scl, scl);

		glColor4f(0.4, 0.6, 0.8, alpha);
		float sz = 6;
		glScalef(sz, sz, sz);
		glTranslatef(0, -6, 0);

		const int N = 256;
		static char str[N];


		snprintf(str, N, "%s : (%.2le %.2le %.2le)", get_string("VIEWINFO_POS_VIEW").c_str(), pc2m*camera_pos.x, pc2m*camera_pos.y, pc2m*camera_pos.z);
		draw_text_line(str, TEXT_CENTER, true);

		snprintf(str,N, "%s : (%.2le %.2le %.2le)", get_string("VIEWINFO_POS_TARGET").c_str(), pc2m*target_r.x, pc2m*target_r.y, pc2m*target_r.z);
		draw_text_line(str, TEXT_CENTER, true);

		if (ds.tyMode == MODE_3D) {
			if (dist*_1pc_ly > 1) {
				snprintf(str, N, "%s : %.1le m   (%.1le %s)", get_string("VIEWINFO_DISTANCE").c_str(), pc2m*dist, dist*_1pc_ly, get_string("VIEWINFO_LY").c_str());
			}
			else {
				snprintf(str, N, "%s : %.1le m   (%.1le %s)", get_string("VIEWINFO_DISTANCE").c_str(), pc2m*dist, dist*_1pc_AU, get_string("VIEWINFO_AU").c_str());
			}
		}
		else {
			snprintf(str, N, "%s : %.2lf %s", get_string("VIEWINFO_VIEW_ANGLE").c_str(), get_plnt_angle(), get_string("VIEWINFO_DEG").c_str());
		}
		draw_text_line(str, TEXT_CENTER, true);

		if (ds.tyMode == MODE_3D) {
			const quaterniond& ori = g_camera.get_ori();
			snprintf(str, N, "%s : (%.2le %.2le %.2le %.2le)", get_string("VIEWINFO_ORIENTATION").c_str(), ori.w, ori.v.x, ori.v.y, ori.v.z);
		}
		else {
			double azimuth = 180 - get_plnt_long();
			if (azimuth < 0.0) azimuth += 360;
			snprintf(str, N, get_string("VIEWINFO_VIEW_DIRECTION_FMT").c_str(), azimuth, get_plnt_lati());
		}
		draw_text_line(str, TEXT_CENTER, true);
		
		snprintf(str, N, "Speed : %.2f", get_move_speed());
		draw_text_line(str, TEXT_CENTER, true);

		glPopMatrix();

	glEnable(GL_DEPTH_TEST);
}





//---------------------------
//  Display Connection Info
//---------------------------
void
draw_connection_wait()
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);

	glPushMatrix();
		goto_each_screen();
		const float scl = get_screen_w() / 180.0;
		glScalef(scl,scl,scl);

		float sz = 12;
		glScalef(sz,sz,sz);

		std::string str = std::string(szHostName) + "  " + get_string("WAITING_CONNECTION");

		char eye_CLR = get_eye_CLR();
		if (eye_CLR == 'L')		{ glTranslatef(0, 1,0); glColor3f(1,0,0); }
		if (eye_CLR == 'R')		{ glTranslatef(0,-1,0); glColor3f(0,1,0); }
		
		draw_text_centering(str);
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
}






//---------
//  ヘルプ
//---------
const int help_item_num = 16;
struct help_item {
	char key[20];
	char expl[100];
};

help_item HelpItem[help_item_num] = {
	{ "HELP_X", "HELP_X_CMT" },
	{ "HELP_Z", "HELP_Z_CMT" },
	{ "HELP_ARROW", "HELP_ARROW_CMT" },
	{ "HELP_1_ARROW", "HELP_1_ARROW_CMT" },
	{ "HELP_PAGEUP", "HELP_PAGEUP_CMT" },
	{ "HELP_PAGEDOWN", "HELP_PAGEDOWN_CMT" },
	{ "HELP_4", "HELP_4_CMT" },
	{ "HELP_3", "HELP_3_CMT" },
	{ "HELP_Z_4", "HELP_Z_4_CMT" },
	{ "HELP_Z_3", "HELP_Z_3_CMT" },
	{ "HELP_A", "HELP_A_CMT" },
	{ "HELP_S", "HELP_S_CMT" },
	{ "HELP_T", "HELP_T_CMT" },
	{ "HELP_C", "HELP_C_CMT" },
	{ "HELP_F", "HELP_F_CMT" },
	{ "HELP_L", "HELP_L_CMT" }
};



void
draw_help()
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);

	const float alpha = 0.6f;
	glPushMatrix();
		goto_center_screen2();
		const float scl = get_screen_w() / 180.0;
		glScalef(scl,scl,scl);
		glTranslatef(0,0,-20);

		glColor4f(0,0.8, 1,alpha);
		const float sz = 5;
		glScalef(sz,sz,sz);
		glTranslatef(-12,7,0);

		for (int i=0; i<help_item_num; i++) {
			glPushMatrix();
				draw_text(get_string(HelpItem[i].key));
			glPopMatrix();
			glPushMatrix();
			glTranslatef(10,0,0);
				draw_text(get_string(HelpItem[i].expl));
			glPopMatrix();
			glTranslatef(0,-1,0);
		}

	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
}



//
void
draw_3D_near(mtk_draw_struct ds)
{
	ds.set_near_region();
	gl_set_frustum(ds.frs_near);
	glMatrixMode(GL_MODELVIEW);


	if (swGalaxy & GLXY_QSO)			{ draw_QSOs(ds); }


	draw_CMB_map(ds, true);
	draw_galaxies(ds);
	draw_CMB_map(ds, false);


	if (swConstellation & CNST_LINE)	{ draw_constellation_lines(ds); }

	
	draw_spacecraft_trajectory(ds);

	draw_objects(ds, true);
	draw_space_curves(ds);

	if (bEcliptic)						{ draw_ecliptic(ds); }
	if (swScaleLine & SCALE_SQUARE)		{ draw_scale_square(ds); }
}


void
draw_3D_far(mtk_draw_struct ds)
{
	ds.set_far_region();
	gl_set_frustum(ds.frs_far);
	glMatrixMode(GL_MODELVIEW);


	draw_milkyway(ds);
	draw_galactic_center_background(ds);


	if (bCelestialGrid)					{ draw_celestial_grid(ds, true); }
	if (bGalacticGrid)					{ draw_galactic_grid(ds, true); }


	if (swConstellation & CNST_LINE)	{ draw_constellation_lines(ds); }
	if (swConstellation & CNST_BND)		{ draw_constellation_boundary(ds); }
	if (swConstellation & CNST_ILLUST)	{ draw_constellation_illust(ds); }
	if (swConstellation & CNST_NAME)	{ draw_constellation_name(ds); }
	

	draw_spacecraft_trajectory(ds);



	if (swGalaxy & GLXY_QSO)			{ draw_QSOs(ds); }


	draw_CMB_map(ds, true);
	draw_galaxies(ds);
	draw_CMB_map(ds, false);

	draw_space_curves(ds);

	draw_objects(ds, false);
}


void
draw_3D(const mtk_draw_struct& ds)
{
	glPushMatrix();


	// 光源のセット
	float lp[4];
	lp[0] = lp[1] = lp[2] = 0;	lp[3] = 1.0;
	glLightfv(GL_LIGHT0, GL_POSITION, lp);

	// スケールと原点のセット
	double fac = ds.scale_factor;
	glScaled(fac, fac, fac);

	//--  draw far region   --
	draw_3D_far(ds);

	


	//--  draw near region  --
	glClear( GL_DEPTH_BUFFER_BIT );
	draw_3D_near(ds);


	glPopMatrix();
}




void
set_pointer_pos()
{
	const double* Mbc;
	Mbc = get_local_camera_matrix_GL();

	glLoadIdentity();
	glTranslatef(-eye_offset_x, 0, 0);
	glMultMatrixd(Mbc);
	glRotatef(-pointer_angle_x,0,1,0);
	glRotatef(pointer_angle_y,1,0,0);
	glTranslatef(0,0,-pointer_z);
}



void
draw_pointer0()
{
	glPushMatrix();
		glScalef(5,5,5);
		glColor3f(1,1,1);
		glLineWidth(1.0);
		glBegin(GL_LINE_LOOP);
			glVertex3f(-1,-1,0);
			glVertex3f(1,-1,0);
			glVertex3f(1,1,0);
			glVertex3f(-1,1,0);
		glEnd();
	glPopMatrix();
}


void
draw_pointer()
{
	glPushMatrix();
		set_pointer_pos();
		draw_pointer0();
	glPopMatrix();
}



//---------
//  Image
//---------

void
draw_image()
{
	media_file_info&  imgf = IMGF[current_image_index];
	const char type = imgf.type;
	const float scl = get_screen_w()/180.0 * get_chart_zoom_factor();
	float sz = 0.5 * get_screen_w();



	glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);


		// 画像を表示
		glColor4f(1,1,1,1);
		switch (type) {
			case MEDIATYPE_POPUP_IMAGE:
				{
					goto_center_screen2();
					glTranslatef(0,get_screen_w()/2,0);

					glScalef(scl, scl, scl);
					float d = sz;
					glTranslatef(0,-d,0);
					glScalef(0.85,0.85,0.85);
					glScalef(sz,sz,sz);
					ImageStill.draw(gl_w, gl_h);
				}
				break;
			case MEDIATYPE_IMAGE:
			default:
				set_chart_pos();
				glPushMatrix();
					glScalef(scl, scl, scl);
					glTranslatef(0,-13,0);
					glScalef(sz,sz,sz);
					ImageStill.draw(gl_w*get_title_width_ratio(), gl_h);
				glPopMatrix();
				break;
		}
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);

		// タイトルを表示
		if (type == MEDIATYPE_IMAGE) {
			glScalef(scl, scl, scl);
			glTranslatef(0, 90, 0.1);
			glScalef(17,17,17);
			glColor3f(0.7,0.7,0.7);

			if (is_defined_key(imgf.disp_title_key)) {
				draw_text_centering(get_string(imgf.disp_title_key));
			}
			else {
				draw_text_centering(get_string(imgf.menu_title_key));
			}

		}
	glPopMatrix();
}





//--------------
//  3D チャート
//--------------
void
draw_3DChart()
{
	const float scl = get_screen_w() / 180.0 * get_chart_zoom_factor();

	glPushMatrix();
		set_chart_pos();
		glScalef(scl, scl, scl);
		draw_3D_chart(no_3DChart);
	glPopMatrix();
}




//--------------
//  Draw Scene
//--------------
void
draw_scene(mtk_draw_struct ds)
{
	glDepthFunc(GL_LEQUAL);
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();



	//-- Object frame
	double M[16];
	const double* Mbc;
	Mbc = get_local_camera_matrix_GL();
	glMultMatrixd(Mbc);

	quaternion_to_matrix_GL_tr( get_base_camera_ori(), M);
	glMultMatrixd(M);

	quaternion_to_matrix_GL_tr(ds.camera_base.get_ori(), M);
	glMultMatrixd(M);



	//-- Draw
	ds.set_near_region();

	gl_set_frustum(ds.frs_near);
	glMatrixMode(GL_MODELVIEW);



	
	switch (ds.tyMode) {
		case MODE_3D:			draw_3D(ds);				break;
		case MODE_PLANETARIUM:	draw_planetarium(ds);		break;
		default:											break;
	}
	


	ds.set_near_region();
	gl_set_frustum(ds.frs_near);
	glMatrixMode(GL_MODELVIEW);


	
	//---  object info  ---
	draw_selected_object_info(ds);



	if (bFPS)			{ draw_fps(); }
	if (bDate)			{ draw_date(ds); }
	if (bAppTitle)		{ draw_app_title(); }
	if (bAbout)			{ draw_about(); }
	if (bDebug)			{ draw_debug_info(); }
	if (bViewInfo)		{ draw_view_info(ds); }
	if (!bControler && bNetwork && !bConnect)	{ draw_connection_wait(); }
	if (bKeyHelp)		{ draw_help(); }
}



void
draw_scene_all_0(const mtk_draw_struct& ds, bool bClear)
{
	//-- Clear
	if (bClear) {
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}



	//-- Single-eye mode
	if (bSingleEye && get_eye_CLR() == 'L')  return;


	//--
	if (!isFrustumLoaded()) {
		set_local_camera_pos(ds.eye_offset);
	}


	//-- Draw scene
	glViewport(ds.vp_x1, ds.vp_y1, ds.vp_w, ds.vp_h);


	draw_scene(ds);



	//
	switch (ds.tySubMode) {
		case SUBMODE_SETTING:	draw_setting_mode();				break;
		case SUBMODE_TITLE:		draw_title_page();					break;
		case SUBMODE_POINTER:	draw_pointer();						break;
		case SUBMODE_IMAGE:		draw_image();						break;
		case SUBMODE_3DCHART:	draw_3DChart();						break;
		default:													break;
	}


	//--  subtitle
	if (bSubtitles)		{ draw_subtitles(); }


	if (ds.tySubMode == SUBMODE_MENU && is_menu_visible() ) {
		draw_menu(mnuSelect, 0.5,0.5, 0);
	}


	// 
	const float alp = GetScreenAlpha();
	if ( alp < 1 ) {
		draw_black_layer( ds.frs_near, alp );
	}
}


void
draw_scene_all(const mtk_draw_struct& ds, bool bClear)
{
	cs_draw.Enter();
	draw_scene_all_0(ds, bClear);
	cs_draw.Leave();
}


//void		RecordMovie();


//---------------
//  Draw Normal
//---------------
void
draw_normal(const mtk_camera camera_base, int w, int h)
{
	vector3f dr = get_eye_offset_x()*vector3f(1, 0, 0);

	float aspc_rt = float(h) / float(w);
	mtk_draw_struct  ds = calc_draw_struct(camera_base, aspc_rt, 0, w, 0, h, dr, true, 1.0);
	set_latest_draw_struct(ds, 0);

	if (ds.tyMode == MODE_3D) {
		prepare_draw_objects(&ds, 1);
	}
	else {
		prepare_draw_planetarium(&ds, 1);
	}
	
	draw_scene_all(ds, true);
}






//--------
//  Draw
//--------
vector2d	get_landing_pos();


mtk_camera
update_time_and_global_camera()
{
	//--- 時刻のアップデート
	update_time();


	//--- g_camera の更新 ---
	const mtk_object& tgt = get_target();
	vector3d  tpos = tgt.get_pos();
	int mode;
	mode = get_mode();

	if (mode == MODE_3D) {
		g_camera.set_scale_factor(get_scale_factor());

		double dist = get_obj_z() / g_camera.get_scale_factor();


		dist += get_target().get_landing_radius();

		g_camera.set_pos_by_target_pos(tpos, dist);
	}
	else {
		double sclfac = get_clst_R() / (1 * _1km_pc);
		g_camera.set_scale_factor(sclfac);



		//**** とりあえず惑星のみ
		if (tgt.get_type() == OBJ_TYPE_PLANET) {
			int idx = tgt.get_index();
			planet& pnt = get_planet(idx);

			double R = pnt.get_Re() * _1km_pc;	// プラネタリウムモードでの視点はリアル半径で計算する

			//---  地形ありの場合
			if (pnt.has_topography()) {
				TopoSphere *tp = pnt.get_topography();
				vector2d  lon_lat = get_landing_pos();
				R += tp->get_elv_fac() * tp->get_elevation(landing_lon, landing_lat) * _1km_pc / 1000.0;
			}

			vector3d nv = get_planet_nv_ec(idx, landing_lon, landing_lat);
			tpos += R*nv;
		}


		g_camera.set_pos(tpos);


		// プラネタリウムモードでのターゲット追尾（地球にいるときのみ）
		if (tgt.get_type() == OBJ_TYPE_PLANET && tgt.get_index() == PLNT_EARTH) {
			follow_planetarium_target(tpos);
		}


		g_camera.set_ori(get_planetarium_ori(tgt.get_index(), landing_lon, landing_lat, get_plnt_long(), get_plnt_lati()));
	}


	return g_camera;
}



mtk_draw_struct
calc_draw_struct(const mtk_camera& camera_base, float aspc_rt, int _vp_x1, int _vp_x2, int _vp_y1, int _vp_y2, vector3f dr, bool bRotBB, float line_w)
{
	//-------------------
	//  描画用構造体の生成
	//-------------------
	const mtk_object& tgt = get_target();
	vector3d  tpos = tgt.get_pos();

	int mode, sub_mode;
	mode = get_mode();
	sub_mode = get_sub_mode();

	mtk_draw_struct  ds;
	ds.set(mode, sub_mode, camera_base, get_time_JD(), aspc_rt, _vp_x1, _vp_x2, _vp_y1, _vp_y2, FontSize, get_target_ID(), tpos, dr, bRotBB, line_w);

	return  ds;
}



void
draw(const mtk_camera camera_base)
{
	//***
	if (bMovieMode) {
		return;
	}
	

	//--------------
	//  シーンを描画
	//--------------
	int w, h;
	w = gl_w;
	h = gl_h;

	switch (get_display_mode()) {
		case DISPMODE_ANAGLYPH:			draw_stereo_anaglyph(camera_base, w, h);			break;
		case DISPMODE_COLOR_ANAGLYPH:	draw_stereo_color_anaglyph(camera_base, w, h);	break;
		case DISPMODE_SIDE_BY_SIDE:		draw_stereo_side_by_side(camera_base, w, h);		break;
		case DISPMODE_TOP_AND_BOTTOM:	draw_stereo_above_below(camera_base, w, h);		break;
		case DISPMODE_DOME_MASTER:		draw_dome_master(camera_base, w, h);			break;
		case DISPMODE_NORMAL:
		default:						draw_normal(camera_base, w, h);					break;
	}
}







////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Window


//=================
//  Event handler
//=================
static bool bTimer = false;
static volatile bool bKillTimer = false;


void OnTimer();
void OnKillTimer();



static MMRESULT timer_ID = NULL;
void CALLBACK
TimeProc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	OnTimer();
};

void CALLBACK
KillTimerProc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	OnKillTimer();
};




void
OnCreate(HWND hWnd)
{
	g_hWnd = hWnd;

	//-- init OpenGL
	HDC  hDC = GetDC(hWnd);

	bool res;
	res = initGL_Window(ogl, hDC);
	if (!res) {
		MessageBoxf(hWnd, "OpenGL", MB_OK, "Failed to initialize OpenGL");
	}


	ogl.MakeCurrent(hDC);
		initGL();
	ogl.MakeCurrent(NULL);
	ReleaseDC(hWnd, hDC);


	//-- init DirectInput
	joystick_SetCooperativeLevel(hWnd);

	//-- set timer for input
	if (bControler || !bNetwork) {
		timeBeginPeriod(1);
		
		timer_ID = timeSetEvent( get_timer_interval(), 0, TimeProc, 0, TIME_PERIODIC | TIME_CALLBACK_FUNCTION);

		bTimer = true;
	}
}


void
OnDestroy()
{
	if (bTimer) {
		timeKillEvent( timer_ID );

		bKillTimer = false;
		MMRESULT res = timeSetEvent( 100, 0, KillTimerProc, 0, TIME_ONESHOT | TIME_CALLBACK_FUNCTION);

		timeEndPeriod(1);
	}
}







void
draw_movie_margen(HDC hDC)
{
	// 背景をクリア
	HBRUSH  hBrush, hBrushOld;
	HPEN  hPenOld;
	
	hBrush = (HBRUSH)GetStockObject( BLACK_BRUSH );
	hBrushOld = (HBRUSH)SelectObject(hDC, hBrush);
	hPenOld = (HPEN)SelectObject(hDC, GetStockObject(NULL_PEN));

		//
		const int x0 = gl_x0;
		const int y0 = gl_y0;

		// ムービーサイズを取得
		int mvx, mvy, mvw, mvh;
		
		dsmv2.GetWindowPosition(mvx, mvy, mvw, mvh);

		int x1,y1,x2,y2, X1,Y1,X2,Y2;
		x1 = mvx;
		y1 = mvy;
		x2 = x1 + mvw;
		y2 = y1 + mvh;
		X1 = 0;
		Y1 = 0;
		if (bNetwork) {
			X2 = win_w0;
			Y2 = win_h0;
		} else {
			X2 = win_w;
			Y2 = win_h;
		}

		
		// Left
		int w, h;
		w = x1-X1;
		h = Y2-Y1;
		if (w>0)	Rectangle(hDC, X1,Y1, x1+1, Y2+1);

		// Right
		w = X2-x2;
		h = Y2-Y1;
		if (w>0)	Rectangle(hDC, x2,Y1, X2+1, Y2+1);
		
		// Top
		w = x2-x1;
		h = y1-Y1;
		if (h>0)	Rectangle(hDC, x1,Y1, x2+1, y1+1);

		// Bottom
		w = x2-x1;
		h = Y2-y2;
		if (h>0)	Rectangle(hDC, x1,y2, x2+1, Y2+1);
		
		

	SelectObject(hDC, hBrushOld);
	SelectObject(hDC, hPenOld );
	DeleteObject(hBrush);
}




void
OnActivate(WPARAM wParam) 
{
	if (LOWORD(wParam) != WA_INACTIVE)	{ bActive = true;  }
	else								{ bActive = false; }
	joystick_Acquire(bActive);	
}







//-------------
//  Animation
//-------------
void
OnAnimType(char type)
{
	animation_type = type;
	if (bControler)  { send_data_char(DATA_CHAR_ANIM_TYPE, type); }
}

void
OnAnimationCounter0(int counter0)
{
	animation_counter0 = counter0;
	if (bControler)  { send_data_int(DATA_INT_ANIM_COUNTER0, animation_counter0); }
}


void
set_animation(char anim_type, int counter, int interval)
{
	OnAnimType(anim_type);
	OnAnimationCounter0(counter);
	animation_counter	= counter;
	animation_interval	= interval;
}


float
get_animation_frac()
{
	if (animation_counter0 == 0)  return 0;
	return  1.0 - float(animation_counter)/float(animation_counter0);
}




void
OnAnimation(int counter)
{
	animation_counter = counter;
	animation_type = ANIM_TY_NONE;

	if (bControler)  { send_data_int(DATA_INT_ANIM_COUNTER, animation_counter); }

	OnCommandDraw();
}



void
process_animation()
{
	// アニメーションの処理
	static int count=0;
	if (bControler || !bNetwork) {
		if (animation_counter > 0) {
			count++;
			if (count >= animation_interval) {
				OnAnimation(animation_counter-1);
				count = 0;
			}
		}
	}
}


void
OnTimer()
{
	static bool first = true;
	
	if (first) {
		first = false;
		//HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
		HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	}

	// 入力
	Input();

	// アニメーション処理
	process_animation();
}



void
OnKillTimer()
{
	CoUninitialize();
	bKillTimer = true;
}







//============
//  Commands
//============
void
OnOpen()
{
	OnCommandDraw();
}

void
OnSaveImage()
{
	static wchar_t filename[MAX_PATH];
	int no;
	filename[0] = '\0';


	std::wstring title, filter;

	title = get_wstring("SAVEIMAGE_TITLE");
	filter = utf8_to_wstring(apply_null_character(get_string("SAVEIMAGE_FILTER")));

	no = GetFileNameW(NULL, filename, filter.c_str(), title.c_str(), true);
	if (no <= 0)  return;

	HDC  hDC = GetDC(g_hWnd);
	ogl.MakeCurrent(hDC);
		image  img_tmp( 0,0, 3 );
		gl_capture_viewport(img_tmp, GL_FRONT);
	ogl.MakeCurrent(NULL);
	ReleaseDC(g_hWnd, hDC);

	switch (no) {
		case 1:	img_tmp.saveJPEG(filename);	break;
		case 2:	img_tmp.savePNG(filename);	break;
		case 3:	img_tmp.saveBMP(filename);	break;
		default:							break;
	}
}


void
OnSaveImageHighRes()
{
	if ( !isPBufferEnable() )   return;

	DialogBox(g_hInst, (LPCTSTR)IDD_DIALOG_SAVE_IMAGE, g_hWnd, (DLGPROC)SaveImageHighResDialogBox);

	if (get_save_image_high_res_filename()[0] != '\0') {
		SetCursor( LoadCursor(NULL, IDC_WAIT) );
		HDC  hDC = GetDC(g_hWnd);
		ogl.MakeCurrent(hDC);
			save_image_high_resolution(g_camera);
		ogl.MakeCurrent(NULL);
		ReleaseDC(g_hWnd, hDC);
		SetCursor( LoadCursor(NULL, IDC_ARROW) );
	}
}



//****************
void
OnUpdateViewCtrl()
{
	//***************
	//adjust_target_position();

	if (bControler) {
		view_info  vwif;
		vwif.qm  = g_camera.get_ori();

		send_frame(vwif);
	}
}

//--- 地平座標の場合
void
OnUpdateViewCtrl2()
{
	if (bControler) {
		view_info2  vwif2;
		vwif2.lon = get_plnt_long();
		vwif2.lat = get_plnt_lati();
		send_frame2(vwif2);
	}
}



//-----------
// 離陸・着陸
//-----------
vector2d
get_landing_pos()
{
	const mtk_object&  tgt = get_target();
	vector3d  nv = g_camera.get_pos() - tgt.get_pos();
	int type = tgt.get_type();


	vector2d lon_lat;

	switch (type) {
	case OBJ_TYPE_PLANET:
		lon_lat = get_planet_lon_lat_ec(tgt.get_index(), nv);
		break;
	case OBJ_TYPE_SATELLITE:
		lon_lat = get_satellite_lon_lat_ec(tgt.get_index(), nv);
		break;
	default:
		lon_lat.set(0, 0);
		break;
	}



	return  lon_lat;
}


void
landing(const vector2d& landing_pos)
{
	const mtk_object&  tgt = get_target();

	// 着陸地点の計算
	landing_lon = landing_pos.x;
	landing_lat = landing_pos.y;

	set_mode( MODE_PLANETARIUM );


	//***********
	//adjust_target_position();



	if (tgt.get_type() == OBJ_TYPE_PLANET) {
		set_plnt_lati(5);
		
		set_plnt_long( get_dirc_from_3D_ori(tgt.get_index(), g_camera.get_ori()) );	// 今の上方向を正面に
		set_planetarium_angle(plnt_angle_ini);
	} else {
	}



	//****
	set_satellites_zoom_rate(MODE_PLANETARIUM);
	double t = get_time_JD();
	update_objects_position_and_orientation(t, true);
	update_space_curves(t);
	//****


	if (bControler) {
		OnUpdateViewCtrl();
		OnUpdateViewCtrl2();
	}
}



void
OnLandingMode(char mode)
{
	set_landing_mode(mode);
	if (bControler)  { send_data_char(DATA_CHAR_LANDING_MODE, mode); }
}



//--- 着陸
void
OnLanding()
{
	if (!bNetwork || bControler) {
		const mtk_object&  tgt = get_target();


		// 着陸できるのは、惑星のみ
		if (tgt.get_type() != OBJ_TYPE_PLANET)  return;


		// 着陸できるほど接近しているかの判定（惑星のみ）
		if (tgt.get_type() == OBJ_TYPE_PLANET) {
			const double Re   = get_planet_Re(MODE_3D, tgt.get_index()) * 1000;			// ターゲットの半径
			const double L    = _1pc_meter * norm( tgt.get_pos() - g_camera.get_pos() );	// ターゲットまでの距離
			const float thR = tan(20.0f /180.0 *M_PI);												// 着陸できる時の球の視直径
			if (Re/L < thR)		return;
		}
	}

	if (bControler)	{ 
		send_command(COMMAND_LANDING);
	}
	landing( get_landing_pos() );
}


void
OnSetPlanetariumMode()
{
	if (bControler) { send_command(COMMAND_SET_PLANETARIUM_MODE); }
	set_mode(MODE_PLANETARIUM);
}

void
OnSetLandingLon(float val)
{
	landing_lon = val;
	if (bControler) { send_data_float(DATA_FLOAT_SET_LANDING_LON, val); }
}

void
OnSetLandingLat(float val)
{
	landing_lat = val;
	if (bControler) { send_data_float(DATA_FLOAT_SET_LANDING_LAT, val); }
}




//------------
//  Location
//------------
void
OnSetLocationInfo(const location& loc)
{
	set_initial_location(loc);
}


void	OnInitTime0();

void
OnLandingInit()
{
	//OnInitTime0();	//

	if (bControler)	{ send_command(COMMAND_LANDING_INIT); }
		
	const location& loc = get_initial_location();
	change_target( get_object_ID("PLNT_EARTH") );
	landing( vector2d(loc.lon, loc.lat) );
}



//--- 離陸
void
OnTakeoff()
{
	if (bControler)	{ send_command(COMMAND_TAKEOFF); }

	set_mode( MODE_3D );


	const mtk_object&  tgt = get_target();
	if (tgt.get_type() == OBJ_TYPE_PLANET) {
		int idx = tgt.get_index();

		// 今向いている方向を正面にする
		quaterniond  ori = get_ori_from_planetarium_dirc(idx, landing_lon, landing_lat, get_plnt_long());

		g_camera.set_ori( ori );

		const float height_rate = 0.005;
		set_scale( height_rate * get_planet_Re(MODE_3D, idx) * _1km_pc );
	} else {
	}


	set_planetarium_target(TARGET_PLNT_NONE);	// 離陸したときは地上モードのターゲットを外す


	//****
	set_satellites_zoom_rate(MODE_3D);
	double t = get_time_JD();
	update_objects_position_and_orientation(t, true);
	update_space_curves(t);
	//****



	swConstellation = 0;
	bEcliptic = false;

	if (bControler) {
		OnUpdateViewCtrl();
		OnUpdateViewCtrl2();
	}
}





//-----------
//  Program
//-----------
void	OnInitModes();

void
OnProgram(char program_no)
{
	tyProgram = program_no;
	init_root_menu();
	menu_root.set_current_pos(menu_root.size()-1);

	if (bControler) {
		send_data_char(DATA_CHAR_PROGRAM, tyProgram);
	}
	if (bControler || !bNetwork) {
		OnInitModes();
	}
}



//-----------
// Switches
//-----------

void
OnTarget(int tgt_ID)
{
	if (bControler)  { send_data_int(DATA_INT_TARGET, tgt_ID); }
	set_target(tgt_ID);
}


void
OnChangeTargetControler(int tgt_ID)
{
	set_changing_target( get_target_ID(), tgt_ID );
}


// 移動のアニメーションをしないバージョン
void
OnTargetByKey0(const char* target_key)
{
	int ID = get_object_ID(target_key);

	if (ID != -1) {
		OnTarget(ID);
	}
}


void
OnTargetByKey(const char* target_key)
{
	int ID = get_object_ID(target_key);

	if (ID != -1) {
		OnChangeTargetControler(ID);
	}
}


void
OnPlanetariumTarget(int tgt_ID)
{
	if (bControler) { send_data_int(DATA_INT_PLANETARIUM_TARGET, tgt_ID); }
	set_planetarium_target(tgt_ID);
}




void
OnChangeFloatingObjectPos(const vector3f& pos)
{
	//********** Floating object の位置更新に用いる ********//
	if (bControler)  { send_data_vector3f( DATA_VECTOR3F_FLOATING_OBJECT_POS, get_floating_object_position() ); }
	set_floating_object_position(pos);
}


void	OnChangeScaleControler(double final_scale);

void
OnTargetMoveControler()
{
	double  ini_scale = get_target().get_scale();
	OnChangeScaleControler( ini_scale );
}


void
OnPresentTime()
{
	if (bControler || !bNetwork) {
        OnChangeTime(get_present_JD());	// 現在の時刻
	}
}


void
OnRealTimeMode(bool bFlg)
{
	if (bControler)  { send_data_char(DATA_CHAR_REALTIME_MODE, bFlg); }
	SetRealtimeMode(bFlg);
}



void
OnResetViewangle()
{
	if (get_mode() != MODE_PLANETARIUM) return;
	OnPlanetariumAngle(45.0);
}



void
OnDispMode(char ty)
{
	if (get_display_mode() == DISPMODE_DOME_MASTER && ty != DISPMODE_DOME_MASTER) {
		reset_local_camera_angle(get_eye_offset_x());
	}

	set_display_mode(ty);
	if (bControler)  { send_data_char(DATA_CHAR_DISPMODE, ty); }
}


void
OnStar(char sw)
{
	char mask = STAR_NAME | STAR_NAME_SELECT | STAR_BAYER | STAR_FRAMSTEED | STAR_HIP;
	bool bMakeName = (swStar & mask) ^ (sw & mask);

	swStar = sw;
	if (bControler)  { send_data_char(DATA_CHAR_STAR, swStar); }

	if (bMakeName) {
		make_star_name_text_2(swStar, swConstellation);
	}
}

void
OnStarSet(char mask, bool bFlg)
{
	char sw;
	if (bFlg)	{ sw = swStar |  mask; }
	else		{ sw = swStar & ~mask; }
	OnStar(sw);
}


void
OnStarMark(char idx)
{
	tyStarMark = idx;
	if (bControler)  { send_data_char(DATA_CHAR_STAR_MARK, idx); }
}



void
OnConstellation(char sw)
{
	bool bMakeName = (swConstellation & CNST_BND) ^ (sw & CNST_BND);

	swConstellation = sw;
	if (bControler)  { send_data_char(DATA_CHAR_CONSTELLATION, swConstellation); }
	
	if (bMakeName) {
		make_star_name_text_2(swStar, swConstellation);
	}
}

void
OnCnstSet(char mask, bool bFlg)
{
	char sw;
	if (bFlg)	{ sw = swConstellation |  mask; }
	else		{ sw = swConstellation & ~mask; }
	OnConstellation(sw);
}




void
OnDispSwitch(char idx, char sw)
{
	if (bControler)  { send_data_switch(DATA_SWITCH_DISP, idx, sw); }
	set_disp_switch(idx, sw);
}


void
OnDispSwitchSetBit(char idx, char mask, bool bFlg)
{
	char sw = get_disp_switch(idx);
	if (bFlg)	{ sw |= mask; }
	else		{ sw &= ~mask; }

	OnDispSwitch(idx, sw);
}

void
OnDispSwitchToggle(char idx, char mask)
{
	char sw = get_disp_switch(idx) ^ mask;

	OnDispSwitch(idx, sw);
}

void
OnEclipse(char sw)
{
	swEclipse = sw;
	if (bControler)  { send_data_char(DATA_CHAR_ECLIPSE, swEclipse); }
}

void
OnEclipseSet(char mask, bool bFlg)
{
	char sw;
	if (bFlg)	{ sw = swEclipse |  mask; }
	else		{ sw = swEclipse & ~mask; }
	OnEclipse(sw);
}


void
OnPlanetAtmosphere(char val)
{
	AtmosphereMode = val;
	if (bControler)  { send_data_char(DATA_CHAR_ATMOSPHERE_MODE, val); }
}




void
OnOortCloud(bool bFlg)
{
	bOortCloud = bFlg;
	if (bControler)  { send_data_char(DATA_CHAR_OORT_CLOUD, bFlg); }
}



void
OnSpacecraft(bool bFlg)
{
	bSpacecraft = bFlg;
	if (bControler)  { send_data_char(DATA_CHAR_SPACECRAFT, bSpacecraft); }
}


void
OnSpacecraftPath(char idx, bool bFlg)
{
	if (bControler) { 
		char buf[2];
		buf[0] = idx;
		buf[1] = char( bFlg );
		send_data_custom(DATA_CUSTOM_SPACECRAFT_PATH, buf, 2*sizeof(char));
	}

	set_spacecraft_trajectory_mode(idx, bFlg);
}



void
OnBaseCameraOri(quaterniond _ori)
{
	static quaterniond ori = _ori;
	set_base_camera(ori);
	if (bControler) {
		send_data_custom(DATA_CUSTOM_BASE_ORI, (const char*)&ori, sizeof(quaterniond));
	}
}



void
OnGalaxy(char sw)
{
	swGalaxy = sw;
	if (bControler)  { send_data_char(DATA_CHAR_GALAXY, swGalaxy); }
}

void
OnGalaxySet(char mask, bool bFlg)
{
	char sw;
	if (bFlg)	{ sw = swGalaxy |  mask; }
	else		{ sw = swGalaxy & ~mask; }
	OnGalaxy(sw);
}



void
OnMarkVirgoCluster(bool bFlg)
{
	bVirgoCluster = bFlg;
	if (bControler)  { send_data_char(DATA_CHAR_VIRGO_CLUSTER, bFlg); }
}



void
OnMilkyWay(char _tyMilkyWay)
{
	if (tyMilkyWay==_tyMilkyWay)  return;

	tyMilkyWay = _tyMilkyWay;
	if (bControler)  { send_data_char(DATA_CHAR_MILKYWAY, tyMilkyWay); }

	bMilkyWayImageLoaded = false;
}


void
OnDate(bool bFlg)
{
	bDate = bFlg;
	if (bControler)  { send_data_char(DATA_CHAR_DATE, bFlg); }
}

void
OnTimeSetting()
{
	CreateSetTimeDialog();
}


void
OnTimeStep(char ty)
{
	tyTimeStep = ty;
	if (bControler)  { send_data_char(DATA_CHAR_TIMESTEP, tyTimeStep); }
}

void
OnChangeEyeDistance(float dist)
{
	set_eye_distance_2(dist, get_eye_CLR());
	
	if (bControler)	{ send_data_float(DATA_FLOAT_EYE_DISTANCE, dist); };
}





void
OnPresetScale(char ty)
{
	const double preset_scale[] = {
		1e7   * _1km_pc,
		        _1AU_pc,
		10    * _1AU_pc,
		100   * _1AU_pc,
		1000  * _1AU_pc,
		10000 * _1AU_pc,
		1     * _1ly_pc,
		10    * _1ly_pc,
		100   * _1ly_pc,
		1000  * _1ly_pc,  // 1kly
		10000 * _1ly_pc,
		1e5   * _1ly_pc,
		1e6   * _1ly_pc,  // 1Mly
		1e7   * _1ly_pc,
		1e8   * _1ly_pc,
		1e9   * _1ly_pc,  // 1Gly
		1e10  * _1ly_pc
	};

	const double fac = 10.0;

	OnChangeScaleControler( fac * preset_scale[ty] );
}


void
OnInitRootMenu()
{
	init_root_menu();
}


void	open_media_info_files(const std::string& lang_id);

void
OnChangeLanguage(char idx)
{
	if (get_current_language() == idx) { return; }

	if (bControler)  {
		send_data_char(DATA_CHAR_LANG, idx);

		//send_all_strings();
	}




	cs_draw.Enter();	//*****
	select_language(get_directory(DIR_LOCALE), idx);
	make_star_name_text_2(swStar, swConstellation);

	if (!bNetwork || bControler) {
		const std::string& lang_id = get_current_language_id();

		open_media_info_files(lang_id);
		load_initial_location("location.dat", lang_id);

		if (bNetwork && bControler) {
			send_media_info(MVF, MEDIATYPE_MOVIE);
			send_media_info(IMGF, MEDIATYPE_IMAGE);
			send_media_info(AUDF, MEDIATYPE_AUDIO);
			send_location_info(get_initial_location());
			send_command(COMMAND_INIT_ROOT_MENU);
		}

		init_root_menu();
	}


	if (!bNetwork) {
		destroy_menu_bar();
		create_menu_bar();
	}

	cs_draw.Leave();	//*****


	if (!bNetwork) {
		set_menu_bar(g_hWnd);
	}


	OnCommandDraw();
}


void
OnZoomMode(char val)
{
	if (bControler)  { send_data_char(DATA_CHAR_ZOOM_MODE, val); }
	set_zoom_mode(val);

	double  ini_scale = get_target().get_scale();
	if (ini_scale > get_current_scale() ) {
		OnSetZoom( ini_scale );
	}
}

void
OnTopoZoomMode(char val)
{
	if (bControler)  { send_data_char(DATA_CHAR_TOPO_ZOOM_MODE, val); }
	set_topo_zoom_mode(val);
}


void
OnCelestialGrid(bool bFlg)
{
	bCelestialGrid = bFlg;
	if (bControler)  { send_data_char(DATA_CHAR_CLST_GLD, bFlg); }
}

void
OnEcliptic(bool bFlg)
{
	bEcliptic = bFlg;
	if (bControler)  { send_data_char(DATA_CHAR_ECLIPTIC, bFlg); }
}

void
OnHorizonCoord(bool bFlg)
{
	bHorizonCoord = bFlg;
	if (bControler)  { send_data_char(DATA_CHAR_HORIZON_COORD, bFlg); }
}

void
OnGalacticGrid(bool bFlg)
{
	bGalacticGrid = bFlg;
	if (bControler)  { send_data_char(DATA_CHAR_GLX_GLD, bFlg); }
}

void
OnDistScale(char sw)
{
	swScaleLine = sw;
	if (bControler)  { send_data_char(DATA_CHAR_DIST_SCALE, sw); }
}


void
OnDistScaleSet(char mask, bool bFlg)
{
	char sw;
	if (bFlg)	{ sw = swScaleLine |  mask; }
	else		{ sw = swScaleLine & ~mask; }
	OnDistScale(sw);
}


void
OnCMB(bool bFlg)
{
	bCMB = bFlg;
	if (bControler)  { send_data_char(DATA_CHAR_CMB, bFlg); }
}

void
OnGalCenterBG(bool bFlg)
{
	bGalCenterBG = bFlg;
	if (bControler)  { send_data_char(DATA_CHAR_GAL_CENTER_BG, bFlg); }
}


void
OnNGC(bool bFlg)
{
	bNGC = bFlg;
	if (bControler)  { send_data_char(DATA_CHAR_NGC, bFlg); }
}

void
OnSideLight(bool bFlg)
{
	bSideLight = bFlg;
	if (bControler)  { send_data_char(DATA_CHAR_SIDE_LIGHT, bFlg); }
}

void
OnSurfacePointer(bool bFlg)
{
	bSurfacePointer = bFlg;
	if (bControler)  { send_data_char(DATA_CHAR_SURFACE_POINTER, bFlg); }
}

void
OnTest(bool bFlg)
{
	bTest = bFlg;

	char type = bFlg ? ANIM_TY_ZOOMIN_IMAGE : ANIM_TY_ZOOMOUT_IMAGE;
	set_animation(type, 50, 1);
	if (bControler)  { send_data_char(DATA_CHAR_TEST, bFlg); }
}



//----------------
//  Pointer Mode
//----------------
void
OnPointerPos(const float* pos)
{
	pointer_angle_x = pos[0];
	pointer_angle_y = pos[1];
	pointer_z		= pos[2];
	if (bControler) { send_data_custom(DATA_CUSTOM_POINTER_POS, (const char*)pos, 3*sizeof(float)); }
}

void
OnPointerMode(char bFlg)
{
	int sub_mode = (bFlg) ? SUBMODE_POINTER : SUBMODE_NORMAL;
	set_sub_mode( sub_mode );
	if (bFlg) {
		float pos[3];
		pos[0] = 0;
		pos[1] = 0;
		pos[2] = get_screen_z();
		OnPointerPos(pos);
	}

	if (bControler) { send_data_char(DATA_CHAR_SUBMODE_POINTER, bFlg); }
}


//------------
//  3D chart
//------------
void
OnOpen3DChart(char val)
{
	set_sub_mode( SUBMODE_3DCHART );
	no_3DChart = val;
	if (bControler) { send_data_char(DATA_CHAR_3DCHART, val); }
}

void
OnClose3DChart()
{
	set_sub_mode( SUBMODE_MENU );
	if (bControler) { send_command(COMMAND_3DCHART_CLOSE); }
}



//===============
//  Media Files
//===============
char media_file_path[20][MAX_PATH];


void
open_media_info_files(const std::string& lang_id)
{
	directory  dir = get_directory(DIR_MEDIA);

	MVF.open_info_file(dir, "movies_list.dat", lang_id);
	IMGF.open_info_file(dir, "images_list.dat", lang_id);
	AUDF.open_info_file(dir, "audio_list.dat", lang_id);
}


void
OnClearMedia(const char media_list_type)
{
	switch (media_list_type) {
	case MEDIATYPE_IMAGE:	IMGF.clear();	break;
	case MEDIATYPE_MOVIE:	MVF.clear();	break;
	case MEDIATYPE_AUDIO:	AUDF.clear();	break;
	}
}


void
OnAddMediaFile(int media_list_type, const media_file_info& mfi)
{
	switch (media_list_type) {
	case MEDIATYPE_IMAGE:		IMGF.add_media_file(mfi);	break;
	case MEDIATYPE_MOVIE:		MVF.add_media_file(mfi);	break;
	case MEDIATYPE_AUDIO:		AUDF.add_media_file(mfi);	break;
	default:	break;
	}
}





//---------
//  Image
//---------
static char tyLastSubMode;



void
OnImageOpenControler(int idx)
{
	bool bSubtitleMode2 = (bSubtitles || (subtitles_idx != 0));
	OnImageOpen(idx);

	if (bControler) {
		send_data_int(DATA_INT_IMAGE_OPEN, idx);
	}
}

void
OnImageOpen(int idx)
{
	char sub_mode = get_sub_mode();
	if (sub_mode != SUBMODE_IMAGE) {
		tyLastSubMode = sub_mode;
	}

	set_sub_mode(SUBMODE_IMAGE);
	current_image_index = idx;

	ImageStill.set_image(IMGF[idx].file_path.c_str());
}

void
OnImageClose()
{
	if (bControler)	{ send_command(COMMAND_IMAGE_CLOSE); }
	set_sub_mode( tyLastSubMode );
}



//---------
//  PopUp
//---------
void
OnPopup(int pidx)
{
	popup_info&  pi = PopUp[pidx];

	switch (pi.type) {
		case POPUP_IMAGE:
				OnImageOpenControler(pi.idx);
				break;
		default:
			break;
	}
	PopUp[pidx].bFlg = false;	// 起動は１回のみ
}


//--------------
//  ScreenFade
//--------------
void
OnScreenAlpha(float fac)
{
	if (bControler) { send_data_float(DATA_FLOAT_SCREEN_ALPHA, fac); }
	SetScreenAlpha(fac);
}





//---------
//  Movie
//---------



void
OnMovieOpenControler(int idx)
{
	bool bSubtitleMode2 = (bSubtitles || (subtitles_idx != 0));
	OnMovieOpen(idx);

	if (bControler) {
		send_data_int(DATA_INT_MOVIE_OPEN, idx);
	}
}

void
OnAudioOpenControler(int idx)
{
	bool bSubtitleMode2 = (bSubtitles || (subtitles_idx != 0));
	OnAudioOpen(idx);

	if (bControler) {
		send_data_int(DATA_INT_AUDIO_OPEN, idx);
	}
}

void
OnMovieOpenSub(const char* filename)
{
	bMovieMode = true;
	bMoviePlaying = false;

	SetMovieStart(true);
	SetMovieFadeAlpha(0.0);


	if (bControler || !bNetwork) {
		dsmv2.Open(filename, g_hWnd, win_mgn_w1, win_mgn_h1, win_w, win_h, WM_GRAPHNOTIFY, MovieRendererType);
	}
	else {
		dsmv2.Open(filename, g_hWnd, win_mgn_w1, win_mgn_h1, win_w, win_h, NULL, MovieRendererType);
	}

	while (bNetwork && bDrawing) { Sleep(1); }



	// 背景をクリア

	//***
	if (dsmv2.GetState() != STATE_NO_MOVIE) {
		HDC hDC = GetDC(g_hWnd);

		draw_movie_margen(hDC);
		ReleaseDC(g_hWnd, hDC);
	}

	dsmv2.OnPaint(g_hWnd, GetDC(g_hWnd));
}


void
OnMovieOpen(int idx)
{
	OnMovieOpenSub(MVF[idx].file_path.c_str());
}


void
OnAudioOpen(int idx)
{
	OnMovieOpenSub(AUDF[idx].file_path.c_str());
}


void
OnMovieFade(float fac)
{
	if (bControler) { send_data_float(DATA_FLOAT_MOVIE_FADE_ALPHA, fac); }
	SetMovieFadeAlpha(fac);
}


void
OnMovieSetPosition(LONGLONG pos)
{
	if (bControler)	{ send_data_LONGLONG(DATA_LONGLONG_MOVIE_POS, pos); }
	dsmv2.SetPosition(pos);
	bMoviePlaying = false;
}


void
OnMoviePlay()
{
	if (bControler)	{ send_command(COMMAND_MOVIE_PLAY); }
	SetMovieStart(true);
	dsmv2.Play();
	bMoviePlaying = true;
}

void
OnMoviePauseControler()
{
	dsmv2.Pause();
	if (bControler) {
		LONGLONG  pos = dsmv2.GetPosition();
		send_data_LONGLONG(DATA_LONGLONG_MOVIE_POS, pos);
	}
	
	bMoviePlaying = false;
}

void
OnMovieHome()
{
	if (bControler)	{ send_command(COMMAND_MOVIE_HOME); }
	dsmv2.Home();
	bMoviePlaying = false;
}

void
OnMovieEnd()
{
	if (bControler)	{ send_command(COMMAND_MOVIE_END); }
	dsmv2.End();
	bMoviePlaying = false;
}



//**********************************************
void
OnMovieClose()
{
	if (bControler)	{
		if (!send_command(COMMAND_MOVIE_CLOSE)) {
		}
	}
	SetMovieFadeAlpha(1.0);

	dsmv2.Close();
	bMoviePlaying = false;
	bMovieMode = false;

	OnScreenAlpha(0);
}
//**********************************************


void
OnDirectShowEvent()
{
	long  evCode, prm1, prm2;


	while (dsmv2.GetEvent(evCode, prm1, prm2))  {
		if (evCode == EC_COMPLETE) {
			bMoviePlaying = false;
		}
	}
}





void
OnTitle(bool bFlg)
{
	bAppTitle = bFlg;
	if (bControler)  { send_data_char(DATA_CHAR_TITLE, bFlg); }
}

void
OnCheckAbout(bool bFlg)
{
	bAbout = bFlg;
	if (bControler)  { send_data_char(DATA_CHAR_ABOUT, bFlg); }
}

void
OnDebug(bool bFlg)
{
	bDebug = bFlg;
	if (bControler)  { send_data_char(DATA_CHAR_DEBUG, bFlg); }
}

void
OnObjectInfo(bool bFlg)
{
	enable_object_info(bFlg);
	if (bControler)  { send_data_char(DATA_CHAR_OBJECT_INFO, bFlg); }
}

void
OnViewInfo(bool bFlg)
{
	bViewInfo = bFlg;
	if (bControler)  { send_data_char(DATA_CHAR_VIEW_INFO, bFlg); }
}

void
OnSingleEye(bool bFlg)
{
	bSingleEye = bFlg;
	if (bControler)  { send_data_char(DATA_CHAR_SINGLE_EYE, bFlg); }
}

void
OnKeyHelp(bool bFlg)
{
	bKeyHelp = bFlg;
	if (bControler)  { send_data_char(DATA_CHAR_KEY_HELP, bFlg); }
}


void
OnSubtitles(bool bFlg)
{
	bSubtitles = bFlg;
	if (bControler)  { send_data_char(DATA_CHAR_SUBTITLES, bFlg); }
}


void
OnPathInfo(bool bFlg)
{
	bPathInfo = bFlg;
	if (bControler)  { send_data_char(DATA_CHAR_PATH_INFO, bFlg); }
}





//
void
OnSetLRef(float val)
{
	set_L_ref(val);
	if (bControler)  { send_data_float(DATA_FLOAT_MAG, get_L_ref()); }

}

void
OnLRef(float dL)
{
	OnSetLRef(get_L_ref()*dL);
}


void
OnSetSpeedFactor(float val)
{
	set_move_speed_factor(val);
	if (bControler)  { send_data_float(DATA_FLOAT_SPEED_FACTOR, val); }
}


void
OnSetFontSize(float val)
{
	FontSize = val;
	if (bControler)  { send_data_float(DATA_FLOAT_FONT_SIZE, FontSize); }
}


void
OnFontSize(const char idx)
{
	if (idx>=0 && idx<=4) {
		OnSetFontSize( vFontSize[idx] );
	}
}


void
OnChangeScaleControler(double final_scale)
{
	set_changing_scale(final_scale);
}



void
OnSetZoom(double val)
{
	double scl0 = get_current_scale();

	if (val/scl0 > 1.001 || val/scl0 < 0.999) {
		reset_selected_object();
	}

	set_scale(val);

	double scl = get_current_scale();

	//******
	//adjust_target_position();


	if (bControler)  { send_data_double(DATA_DOUBLE_ZOOM_FACTOR, scl); }
}

void
OnZoom(double fac)
{
	OnSetZoom(get_current_scale() * fac);
}

void
OnPlanetariumAngle(float val)
{
	set_planetarium_angle(val);
	if (bControler)  { send_data_float(DATA_FLOAT_PLNT_ANGLE, val); }
}

void
OnChangeTime(const double& t)
{
	if (bControler) { send_data_double(DATA_DOUBLE_JD, t ); }
	change_time_JD(t);
}


void
OnPreset(int idx)
{

	switch (idx) {

		case PRESET_SOLARSYSTEM_COMPLETE_VIEW:
			if (get_mode() != MODE_3D) {
				OnTakeoff();
			}
			g_camera.set_ori( quaterniond(-3.49e-1, -9.35e-1, -5.34e-2, 2.31e-2) );
			OnUpdateViewCtrl();
			OnTargetByKey0("PLNT_EARTH");
			OnSetZoom(2.04e-3);
			break;

		case PRESET_VOYAGER_PATH:
			if (get_mode() != MODE_3D) {
				OnTakeoff();
			}
			OnChangeTime(get_JD(1972, 3, 3, 0, 0, 0.0));
			OnTimeStep( TIMESTEP_MONTH );
			g_camera.set_ori( quaterniond(-4.23e-1, -8.24e-1, 3.34e-1, -1.73e-1) ); 
			OnUpdateViewCtrl();
			OnTargetByKey0("SUN");
			OnSetZoom(1.90e-4);
			OnZoomMode(1);
			for (int i=0; i<get_spacecraft_num(); i++) {
				OnSpacecraftPath(i, true);
			}
			break;

		case PRESET_NEW_HORIZONS:
			if (get_mode() != MODE_3D) {
				OnTakeoff();
			}
			OnChangeTime(get_JD(2015, 7, 14, 11, 0, 10.0));
			OnTimeStep(TIMESTEP_10SEC);
			g_camera.set_ori(quaterniond(3.84e-2, 4.15e-2, 8.02e-1, 5.95e-1));
			OnUpdateViewCtrl();
			OnTargetByKey0("SCF_NEW_HORIZONS");
			OnSetZoom(9.0e-13);
			OnZoomMode(0);
			break;

		case PRESET_HAYABUSA2:
			if (get_mode() != MODE_3D) {
				OnTakeoff();
			}
			OnChangeTime(get_JD(2015, 12, 3, 7, 30, 10.0));
			OnTimeStep(TIMESTEP_MIN);
			g_camera.set_ori(quaterniond(9.01e-1, 5.66e-2, 6.03e-2, 4.26e-1));
			OnUpdateViewCtrl();
			OnTargetByKey0("SCF_HAYABUSA_2");
			OnSetZoom(9.07e-13);
			OnZoomMode(0);
			break;

		case PRESET_AKATSUKI:
			if (get_mode() != MODE_3D) {
				OnTakeoff();
			}
			OnChangeTime(get_JD(2015, 12, 7, 0, 30, 10.0));
			OnTimeStep(TIMESTEP_10SEC);
			g_camera.set_ori(quaterniond(-2.97e-1, 5.38e-1, -4.88e-1, 6.21e-1));
			OnUpdateViewCtrl();
			OnTargetByKey0("SCF_AKATSUKI");
			OnSetZoom(6.22e-13);
			OnZoomMode(0);
			break;

		case PRESET_BLACKHOLE_GC:
			if (get_mode() != MODE_3D) {
				OnTakeoff();
			}
			OnChangeTime( get_present_JD() );
			OnTimeStep(TIMESTEP_MONTH);
			g_camera.set_ori(quaterniond(-3.99e-1, -3.08e-1, -6.87e-1, 5.24e-1));
			OnUpdateViewCtrl();
			OnTargetByKey0("SGR_A_STAR");
			OnSetZoom(5.117e-4);
			OnZoomMode(0);
			break;

		case PRESET_ECLIPSE_2009_7_22:
			if (get_mode() != MODE_3D) {
				OnTakeoff();
			}
			if (get_target().get_type() != OBJ_TYPE_PLANET || get_target().get_index() != PLNT_EARTH) {
				OnTargetByKey0("PLNT_EARTH");
			}
			OnChangeTime(get_JD(2009, 7, 22, 2, 0, 10.0));
			OnTimeStep(TIMESTEP_MIN);
			g_camera.set_ori(quaterniond(9.41e-2, 2.83e-2, -6.70e-1, -7.36e-1));
			OnUpdateViewCtrl();
			OnZoomMode(0);
			OnSetZoom(1.3e-9);
			break;

		case PRESET_ECLIPSE_2016_3_9_TOKYO:
			if (get_target().get_type() != OBJ_TYPE_PLANET || get_target().get_index() != PLNT_EARTH) {
				OnTargetByKey0("PLNT_EARTH");
			}
			if (get_mode() != MODE_PLANETARIUM) {
				OnSetPlanetariumMode();
			}
			OnPlanetariumAngle( 5 );
			// Position of Mitaka, Tokyo
			OnSetLandingLon(139.54125);
			OnSetLandingLat(35.6725);
			OnChangeTime(get_JD(2016, 3, 9, 1, 12, 10.0));
			OnTimeStep(TIMESTEP_10SEC);
			OnPlanetariumTarget(TARGET_PLNT_SUN);
			OnZoomMode(0);
			OnUpdateViewCtrl2();
			break;

		case PRESET_ECLIPSE_2016_3_9_TERNATE:
			if (get_target().get_type() != OBJ_TYPE_PLANET || get_target().get_index() != PLNT_EARTH) {
				OnTargetByKey0("PLNT_EARTH");
			}
			if (get_mode() != MODE_PLANETARIUM) {
				OnSetPlanetariumMode();
			}
			OnPlanetariumAngle(5);
			// Position of Ternate
			OnSetLandingLon(127.36667);
			OnSetLandingLat(0.78333);
			OnChangeTime(get_JD(2016, 3, 8, 23, 36, 10.0));
			OnTimeStep(TIMESTEP_10SEC);
			OnPlanetariumTarget(TARGET_PLNT_SUN);
			OnZoomMode(0);
			OnUpdateViewCtrl2();
			break;

		default:
			break;
	}
}





void
OnInitOrientation()
{
	// プラネタリウムモードで南の空を見るようにする
	float lon = plnt_long_ini;
	
	set_plnt_long(lon);
	set_plnt_lati(plnt_lati_ini);
}

void
OnInitTime0()
{
	//-- 当日の夜８時に設定する--
	UTC lt = get_present_LocalTime(DT);
	OnChangeTime( get_JD(lt.year, lt.month, lt.day, 20, 0, 0.0) - DT/24.0 + 1e-8  );	//***
}



void
OnInitTime()
{
	OnInitTime0();	//

	if (bControler)	{ send_command(COMMAND_INIT_TIME); }
}




void
OnInitModes()
{
	OnLandingInit();
	OnInitOrientation();
	OnUpdateViewCtrl();
	OnUpdateViewCtrl2();
	//OnTargetMoveControler();
	{
		double  ini_scale = get_target().get_scale();
		OnSetZoom( ini_scale );
	}
	
	OnPlanetariumAngle(45.0);
	OnPlanetariumTarget(TARGET_PLNT_NONE);

	OnSetLRef( L_ref_ini );
	OnDate(false);
	OnEcliptic(false);
	OnCelestialGrid(false);
	OnInitTime0();
	OnTimeStep( TIMESTEP_10MIN );


	OnDispSwitch( DISPSW_PLANET,		DISPM_DISP | DISPM_ORBIT | DISPM_CLOUD);
	OnDispSwitch( DISPSW_SATELLITE,		DISPM_DISP | DISPM_NAME  | DISPM_ORBIT | DISPM_SELECT );
	OnDispSwitch( DISPSW_MINOR_PLANET,	DISPM_DISP | DISPM_NAME  | DISPM_ORBIT | DISPM_SELECT );
	OnDispSwitch( DISPSW_TNO,			DISPM_DISP | DISPM_NAME  | DISPM_ORBIT | DISPM_SELECT );

	OnPlanetAtmosphere( ATMOSPHERE_MULTIPLE );
	OnStar(STAR_DISP | STAR_NAME_SELECT | STAR_ORBIT );
	OnConstellation(0);
	OnGalaxy(GLXY_OUR_GALAXY | GLXY_SGR_A_ST | GLXY_GLOB_CLSTR | GLXY_NEARBY_GALAXY | GLXY_DISTANT_GALAXY | GLXY_QSO);
	OnStarMark(STARMARK_NONE);
	OnMarkVirgoCluster(false);

	OnZoomMode(0);
	OnOortCloud(true);
	OnMilkyWay(MILKYWAY_VISIBLE);
	OnDistScale(SCALE_CIRCLE);
	for (int i=0; i<get_spacecraft_num(); i++) {
		OnSpacecraftPath(i, false);
	}

	OnChangeSubtitlesIndex(0);
	OnSubtitles(false);

	bFlg3 = false;	//***
	switch (tyProgram) {
		case 0:		//--- 宇宙の大規模構造（ダイジェスト版）
			break;
		case 1:		//--- 宇宙の大規模構造（ジュニア版）
			OnOortCloud(false);
			OnGalaxySet(GLXY_DISTANT_GALAXY, false);
			OnGalaxySet(GLXY_QSO, false);
			break;
		case 2:		//--- 天の川の秘密
			OnOortCloud(false);
			break;
		case 3:		//--- 太陽系紀行
			bFlg3 = true;
			break;
		case 4:		//--- 太陽系を越えて
			OnTimeStep( TIMESTEP_DAY );
			break;
		default:
			break;
	}
	init_popup(tyProgram);

	OnCommandDraw();
}



void
OnNameToggle()
{
	if (get_mode() == MODE_PLANETARIUM) {
		
		bool flg = (!bDate || !(swStar & STAR_NAME) || !test_disp_switch(DISPSW_PLANET, DISPM_NAME)
			|| !(swConstellation & (CNST_NAME | CNST_LINE)))  ?  true  :  false;

		OnEcliptic(flg);
		OnDate(flg);

		OnDispSwitchSetBit(DISPSW_PLANET,		DISPM_NAME, flg);
		OnDispSwitchSetBit(DISPSW_SATELLITE,	DISPM_NAME, flg);
		OnDispSwitchSetBit(DISPSW_MINOR_PLANET,	DISPM_NAME, flg);
		OnDispSwitchSetBit(DISPSW_TNO,			DISPM_NAME, flg);

		OnStarSet(STAR_NAME, flg);
		OnCnstSet(CNST_NAME | CNST_LINE, flg);
	} else {
		bool flg = (!bDate || !(swStar & STAR_NAME) || !test_disp_switch(DISPSW_PLANET, DISPM_NAME))  ?  true  :  false;
		OnDate(flg);

		OnDispSwitchSetBit(DISPSW_PLANET,		DISPM_NAME, flg);
		OnDispSwitchSetBit(DISPSW_SATELLITE,	DISPM_NAME, flg);
		OnDispSwitchSetBit(DISPSW_MINOR_PLANET,	DISPM_NAME, flg);
		OnDispSwitchSetBit(DISPSW_TNO,			DISPM_NAME, flg);


		OnStarSet(STAR_NAME, flg);
		
		OnDistScaleSet(SCALE_CIRCLE, flg);
	}

	//
	OnCommandDraw();
}



//----------
//  Signal
//----------
volatile static bool g_Signal;


void
ClearSignal() {
	g_Signal = false;
}

bool
GetSignal()
{
	return g_Signal;
}

void
WaitForSignal()
{
	while( !g_Signal ) { Sleep(10); };		// *** to avoid Intel 2015 optimization
	//while( !GetSignal() ) { Sleep(100); };
}

void
OnCommandSignal()
{
	g_Signal = true;
	if (bControler) { send_command(COMMAND_SIGNAL); }
}



//----------------
// Menu commands
//----------------
void
OnExitAll()
{
	if (bControler)	{ send_command(COMMAND_EXIT_ALL); }

	SendMessage(g_hWnd, WM_CLOSE, 0, 0);
}


void
OnCaptureImage()
{
	bool flg = false;

	if (bControler)	{ send_command(COMMAND_CAPTURE_IMAGE); }

	
	image  img_tmp(0,0,3);

	HDC  hDC = GetDC(g_hWnd);
	ogl.MakeCurrent(hDC);
	if (get_display_mode() != DISPMODE_DOME_MASTER) {
		gl_capture_viewport(img_tmp, GL_FRONT);
	}
	else {
		capture_dome_master(g_camera, img_tmp);
	}
	flg = true;
	ogl.MakeCurrent(NULL);

	if (flg) {
		img_tmp.savePNG("capture.png");
	}

	ReleaseDC(g_hWnd, hDC);
}

void
OnMakeStereogram()
{
	float eye_offset_x_org = eye_offset_x;
	image  img_tmp(0,0,3);

	HDC  hDC = GetDC(g_hWnd);

	mtk_camera cam = g_camera;

	ogl.MakeCurrent(hDC);
		eye_offset_x = -get_eye_distance()/2;
		draw(cam);
		gl_capture_viewport(img_tmp, GL_BACK);
		img_tmp.savePNG("stereo_L.png");

		eye_offset_x =  get_eye_distance()/2;
		draw(cam);
		gl_capture_viewport(img_tmp, GL_BACK);
		img_tmp.savePNG("stereo_R.png");
	ogl.MakeCurrent(NULL);

	ReleaseDC(g_hWnd, hDC);

	eye_offset_x = eye_offset_x_org;
}





//-------------------
//  字幕(Subtitles)
//-------------------
void
OnChangeSubtitlesIndex(int idx)
{
	subtitles_idx = idx;

	if (bControler)  { send_data_int(DATA_INT_SUBTITLES_INDEX, subtitles_idx); }
}



//--------------
//  Title Mode
//--------------
void
OnTitlePageMode(bool bFlg)
{
	char sub_mode = (bFlg) ? SUBMODE_TITLE : SUBMODE_NORMAL;
	set_sub_mode( sub_mode );
	if (bControler)  { send_data_char(DATA_CHAR_TITLE_MODE, bFlg); }
}


void
OnTitlePage(char val)
{
	set_title_page_no(val);
	if (bControler)  { send_data_char(DATA_CHAR_TITLE_PAGE, val); }
}

void
set_title_page_ctrl(int idx)
{
	if (get_sub_mode() != SUBMODE_TITLE)  return;
	if (bSubtitles)	{ OnSubtitles(false); }

	switch (idx) {
		case TITLEPAGE_CREDITS:
			OnDate(false);
			OnDispSwitchSetBit( DISPSW_PLANET, DISPM_NAME, false );
			OnStarSet(STAR_NAME | STAR_BAYER | STAR_FRAMSTEED | STAR_HIP, false);
			OnCnstSet(CNST_NAME, false);
			set_animation(ANIM_TY_CREDIT_SCROLL, (1000.0/get_timer_interval()) * get_credit_roll_duration(), 1);
			break;
		case TITLEPAGE_ATTENTION:
			OnInitModes();
			break;
		case TITLEPAGE_TITLE:
			set_animation(ANIM_TY_TITLE, (1000.0/get_timer_interval()) * get_title_duration(), 1);
			break;
		case TITLEPAGE_THEME:
			set_animation(ANIM_TY_THEME, 30, 1);
			break;
		default:
			OnTitlePageMode(false);
			return;
	}
	OnTitlePage(idx);
	OnCommandDraw();
}




void
OnEnterTitleMode()
{
	OnTitlePageMode(true);
	set_title_page_ctrl(0);
}

void
OnNextTitlePage()
{
	set_title_page_ctrl( get_title_page_no()+1 );
}



//----------------
//  Setting mode
//----------------
void
OnEnterSettingMode()
{
	if (bControler)	{ send_command(COMMAND_SETTING_ENTER); }
	set_sub_mode( SUBMODE_SETTING );
	chSettingModeDisp = SETTING_MODE_NONE;
}

void
OnLeaveSettingMode()
{
	if (bControler)	{ send_command(COMMAND_SETTING_LEAVE); }
	set_sub_mode( SUBMODE_NORMAL );
	chSettingModeDisp = SETTING_MODE_NONE;
}

void
OnSettingModeDisp(char val)
{
	if (bControler)  { send_data_char(DATA_CHAR_SETTING_DISP, val); }
	chSettingModeDisp = val;
}







void
OnAbout()
{
	DialogBox(g_hInst, (LPCTSTR)IDD_DIALOG_ABOUT, g_hWnd, (DLGPROC)About);
}









//---------
//  Popup
//---------
void
init_popup(const char nProgram)
{
	bool bFlg;
	
	switch (nProgram) {
		case 0:
		case 2:
		default:
			bFlg = false;
			break;
		case 1:
			bFlg = true;
			break;
	}

	for (int i=0; i<popup_num; i++) {
		PopUp[i].bFlg = bFlg;		// ポップアップの有効化
	}
}

int
check_popup()
{
	const double scale = get_current_scale();
	for (int i=0; i<popup_num; i++) {
		popup_info&  pi = PopUp[i];
		if (!pi.bFlg)  continue;
		bool bFlg1 = (get_scale_last() < pi.scale);
		bool bFlg2 = (scale < pi.scale);
		if (bFlg1 != bFlg2) {	// ポップアップの起動
			return i;
		}
	}

	return -1;
}



//-- 惑星に近づいた時の回転角のスケール
float
get_rotation_factor()
{
	const mtk_object&  tgt = get_target();


	//*****  とりあえず惑星と衛星のみ  ******//
	int tgt_ty = tgt.get_type();
	if (tgt_ty != OBJ_TYPE_PLANET && tgt_ty != OBJ_TYPE_SATELLITE)  return 1.0f;

	float Re;
	if (tgt_ty == OBJ_TYPE_SATELLITE) {
		double Rx, Ry, Rz;
		get_satellite_size(MODE_3D, tgt.get_index(), Rx, Ry, Rz);
		Re = Rx;
	}
	else {
		Re = get_planet_Re(get_mode(), tgt.get_index() );
	}

	const float Rt = Re * _1km_pc * get_scale_factor();

	const float Rc = get_obj_z();
	if (Rt > Rc) {
		return Rc/Rt;
	}
	return 1.0f;
}


//--- ズームイン・ズームアウトの処理
void
process_zoom(const float& zm)
{
	bMotion = true;

	if (get_mode() == MODE_3D) {
		float zma = 1.0 + fabs(zm);
		if (zm > 0)		{ OnZoom(1.0/zma);     }
		else			{ OnZoom(zma); }
		int pidx = check_popup();
		if (pidx >= 0) {
			OnPopup(pidx);
		}
	} else {
		float a = 1.0 + fabs(zm);
		float ang;

		if (zm > 0)		{ ang = get_plnt_angle()/a; }
		else			{ ang = get_plnt_angle()*a; }

		float angle_max = plnt_angle_max;
		if (get_display_mode() == DISPMODE_DOME_MASTER) {
			angle_max = 55;
		}
		if (ang >= plnt_angle_min && ang <= angle_max) {
			OnPlanetariumAngle(ang);
		}
	}
}



//=========
//  Input
//=========
bool
InputNormal(button_state* btn, axis_state* axi)
{
	bool flg = false;


	//--  Play ボタン: メニューモード
	if (btn[BTN_PLAY].value && btn[BTN_PLAY].bToggle)		{ OnEnterTitleMode();		return true; }

	//--  Selectボタン: ポインタモード
	if (btn[BTN_SELECT].value && btn[BTN_SELECT].bToggle)	{ OnPointerMode(true); return true; }

	//--  ○ボタン: メニュー
	if (btn[BTN_CIR].value && btn[BTN_CIR].bToggle)	{
		OnEnterMenu();		return true;							// メニューを開く
	}

	//--  ×ボタン
	if (btn[BTN_CRS].value)	{
		OnEnterSettingMode();
		return  true;
	}
	
	//--  △ボタン:（離陸、着陸）
	if (btn[BTN_TRI].value && btn[BTN_TRI].bToggle)	{
		switch (get_mode()) {
			case MODE_PLANETARIUM:	OnTakeoff();	break;
			case MODE_3D:			OnLanding();	break;
			default:								break;
		}

		return true;
	}

	//--  □ボタン:  名前、星座線のオン・オフ
	if (btn[BTN_SQR].value && btn[BTN_SQR].bToggle)			{ OnNameToggle();			return true; }

	/*
	//--  R3ボタン:  ターゲット付近に移動
	if (btn[BTN_R3].value && btn[BTN_R3].bToggle) {
		OnTargetMoveControler();
		return  true;
	}
	*/


	const float Rate = 0.7 * get_move_speed();	// 移動量にかけるファクタ


	//-- Axes
	float x, y, z, rt, zm, zz;
	x = y = z = rt = zm = zz = 0;
	if (btn[BTN_L2].value)	{
		rt  = axi[0].get_normalized_value();
		z   = axi[2].get_normalized_value();
	} else if (btn[BTN_R2].value) {
		zz  = -axi[3].get_normalized_value();
	
		x   = axi[0].get_normalized_value();
		y   = axi[1].get_normalized_value();
	} else {
		x   = axi[0].get_normalized_value();
		y   = axi[1].get_normalized_value();
		zm  = -axi[2].get_normalized_value();		// 上が正になるようにする
	}

	if      (sbZoomMinus.GetState() == SCRBTN_PUSH) { zm= 1; }
	else if (sbZoomPlus.GetState()  == SCRBTN_PUSH) { zm=-1; }

	if (get_mode() == MODE_PLANETARIUM) { z=-z; }



	const mtk_object&  tgt = get_target();


	float lvr_Flon = 0;
	float lvr_Flat = 0;


	if (get_mode() == MODE_3D) {
		float Fx, Fy, Fs, Fzm;

		//--  Rotation  --
		float fxy = Rate * 5;

		float fac = get_rotation_factor();
		fxy *= fac; // 惑星に近づいたときは回転角を小さくする


		// 地表探索モード
		if (fac < 2.0e-1) {
			y = -y;
			rt = -x;
			x = 0;
		}


		Fx  = -0.5*fxy*x;
		Fy  =  0.5*fxy*y;
		Fs  =  0.5 * Rate * 5 * rt;
		Fzm =  10 * Rate * zm;
		SetInputForce3D(Fx, Fy, Fs, Fzm);
	} else {
		const float fac = 100 * 2 * Rate * get_z_far() / get_plnt_R() / 500;
		float  Flon, Flat, Fpzm;

		Flon = -fac * x;
		Flat = -fac * y;
		Fpzm = 10 * Rate * zm;
		SetInputForcePlanetarium(Flon, Flat, Fpzm);
	}


	//-- 時刻を変える
	int idJD = 0;
	if (btn[BTN_L1].value || sbTimeMinus.GetState()==SCRBTN_PUSH)	{ idJD = -1; }
	if (btn[BTN_R1].value || sbTimePlus.GetState() ==SCRBTN_PUSH)	{ idJD =  1; }
	
	double dJD = double(idJD);
	if (idJD!=0) {
		flg = true;
		bMotion = true;	//*****************
		
		UTC utc = JD_to_UTC( get_time_JD() );
		switch (tyTimeStep) {
			case TIMESTEP_10SEC:	utc.sec		+= 10*dJD;		break;
			case TIMESTEP_MIN:		utc.min		+= idJD;		break;
			case TIMESTEP_10MIN:	utc.min		+= 10*idJD;		break;
			case TIMESTEP_HOUR:		utc.hour	+= idJD;		break;
			case TIMESTEP_4HOUR:	utc.hour	+= 4*idJD;		break;
			case TIMESTEP_DAY:		utc.day		+= idJD;		break;
			case TIMESTEP_WEEK:		utc.day		+= 7*idJD;		break;
			case TIMESTEP_MONTH:	utc.month	+= idJD;		break;
			case TIMESTEP_YEAR:		utc.year	+= idJD;		break;
			case TIMESTEP_10YEAR:	utc.year	+= 10*idJD;		break;
			case TIMESTEP_100YEAR:	utc.year	+= 100*idJD;	break;
			default:											break;
		}
		OnRealTimeMode(false);
		OnChangeTime( UTC_to_JD(utc) );
	}

	return flg;
}



bool
InputSetting(button_state* btn, axis_state* axi)
{
	// ×ボタンを離すと設定モード終了
	if (!btn[BTN_CRS].value)	{
		OnLeaveSettingMode();
		return  true;
	}

	//-- 時刻を変える
	char dTS = 0;
	if (btn[BTN_L1].value && btn[BTN_L1].bToggle)	{ dTS = -1; }
	if (btn[BTN_R1].value && btn[BTN_R1].bToggle)	{ dTS =  1; }
	if (dTS != 0) {
		if (chSettingModeDisp != SETTING_MODE_DATE) {
			OnSettingModeDisp(SETTING_MODE_DATE);
		}

		char TS = tyTimeStep + dTS;
		if (TS < 0)					{ TS = 0; }
		if (TS > TIMESTEP_100YEAR)	{ TS = TIMESTEP_100YEAR; }
		if (TS != tyTimeStep) {
			OnTimeStep(TS);
		}

		return  true;
	}


	//-- 視差を変える
	if (is_eye_dist_change_enabled()) {
		const float dd = 0.1;
		const float d_default = 6.4f;
		if (bNetwork) {
			const float d_old = get_eye_distance_2();
			float d = d_old;
			if (btn[BTN_L2].value && btn[BTN_L2].bToggle)	{ d -= dd; }
			if (btn[BTN_R2].value && btn[BTN_R2].bToggle)	{ d += dd; }
			if (btn[BTN_L2].value && btn[BTN_R2].value)		{ d = d_default; }
			if (fabs(d - d_old) > 1e-5) {
				if (chSettingModeDisp != SETTING_MODE_EYE_DISTANCE) {
					OnSettingModeDisp(SETTING_MODE_EYE_DISTANCE);
				}

				OnChangeEyeDistance(d);
				return  true;
			}
		}
		else {
			// stand-alone
			const float d_old = get_stereo_eye_distance();
			float d = d_old;
			if (btn[BTN_L2].value && btn[BTN_L2].bToggle)	{ d -= dd; }
			if (btn[BTN_R2].value && btn[BTN_R2].bToggle)	{ d += dd; }
			if (btn[BTN_L2].value && btn[BTN_R2].value)		{ d = d_default; }
			if (fabs(d - d_old) > 1e-5) {
				set_stereo_eye_distance(d);
				return  true;
			}
		}
	}



	//-- 夜景のトグル
	if (btn[BTN_SQR].value && btn[BTN_SQR].bToggle) {
		OnDispSwitchToggle(DISPSW_PLANET, DISPM_CITY_LIGHTS);
		return  true;
	}


	//-- 拡大率を変える
	int  x   = axi[0].value;
	if (x != 0) {
		char mode = get_zoom_mode();
		if (x > 0 && axi[0].bToggle) { 
			mode--;
			if (mode >= 0)	{ OnZoomMode(mode); return true; }
		}
		if (x < 0 && axi[0].bToggle) { 
			mode++;
			if (mode <= 4)	{ OnZoomMode(mode); return true; }
		}
	}


	//-- 当日の午後8時に設定する
	if (btn[BTN_TRI].value && btn[BTN_TRI].bToggle)	{
		OnInitTime();
		return true;
	}


	return false;
}




bool
InputMenu(button_state* btn, axis_state* axi)
{
	menu&  mnu = *mnuSelect.back();


	//-- Process input
	if (btn[BTN_CIR].value && btn[BTN_CIR].bToggle)							{ OnExecMenu();		return true; }
	if (btn[BTN_CRS].value && btn[BTN_CRS].bToggle)							{ OnCloseMenu();	return true; }
	

	//-- Axes
	bool flg = false;
	int pos = mnu.get_current_pos();

	// 一つ上げる
	if ((axi[1].value<0 && axi[1].bToggle) || (btn[BTN_UP].value && btn[BTN_UP].bToggle)) {
		pos--;
		if (pos < 0) pos=mnu.size()-1;
		OnMenuPos(pos);
		flg=true; 
	}

	// 一つ下げる
	if ((axi[1].value>0 && axi[1].bToggle) || (btn[BTN_DOWN].value && btn[BTN_DOWN].bToggle)) {
		pos++;
		if (pos>=mnu.size())  pos=0;
		OnMenuPos(pos);
		flg=true;
	}


	//-- Axes
	float x, y, fac;
	
	fac = get_timer_interval() / 15.0;
	x = axi[3].get_normalized_value();
	y = axi[2].get_normalized_value();
	x *= fac;
	y *= fac;

	if (x != 0) {
		OnMenuLonAngle(get_menu_lon_angle() + 0.5*x);
		flg = true;
	}

	if (y != 0) {
		OnMenuLatAngle(get_menu_lat_angle() + 0.5*y);
		flg = true;
	}


	return flg;
}


bool
InputPointer(button_state* btn, axis_state* axi)
{
	// Selectボタン: ポインタモードの終了
	if (btn[BTN_SELECT].value && btn[BTN_SELECT].bToggle)		{ OnPointerMode(false); return true; }
	if (btn[BTN_CRS].value && btn[BTN_CRS].bToggle)				{ OnPointerMode(false);	return true; }




	//-- Axes
	float x, y, z;
	x   = axi[0].get_normalized_value();
	y   = axi[1].get_normalized_value();
	z   = axi[2].get_normalized_value();
	bool flg = false;

	float fac = get_timer_interval() / 15.0f;

	if (x != 0) {
		pointer_angle_x +=  fac*0.5*x;
		flg = true;
	}
	if (y != 0)  {
		pointer_angle_y += -fac*0.5*y;
		flg = true;
	}
	if (z != 0)  {
		pointer_z -= fac*5*z;
		if (pointer_z < 1.5*get_z_near()) pointer_z = 1.5*get_z_near();
		flg = true;
	}

	static	float pos[3];
	if (flg) {
		pos[0] = pointer_angle_x;
		pos[1] = pointer_angle_y;
		pos[2] = pointer_z;
		OnPointerPos(pos);
	}


	return flg;
}



bool
InputTitle(button_state* btn, axis_state* axi)
{
	// ○ボタン
	if (btn[BTN_CIR].value && btn[BTN_CIR].bToggle)	{
		OnNextTitlePage();	return true;		// タイトルページを進める
	}

	// ×ボタン（タイトルページのキャンセル）
	if (btn[BTN_CRS].value && btn[BTN_CRS].bToggle)	{
		OnTitlePageMode(false);	return true;
	}

	return false;
}




bool
InputImage(button_state* btn, axis_state* axi)
{
	if (btn[BTN_CIR].value && btn[BTN_CIR].bToggle)		{ OnImageClose();	return true; }
	if (btn[BTN_CRS].value && btn[BTN_CRS].bToggle)		{ OnImageClose();	return true; }
	

	bool flg = false;


	//-- Axes
	float x, y, z, fac;

	fac = get_timer_interval() / 15.0;
	x = axi[0].get_normalized_value();
	y = axi[1].get_normalized_value();
	z = -axi[2].get_normalized_value();
	x *= fac;
	y *= fac;
	z *= fac;

	if (IMGF[current_image_index].type != MEDIATYPE_POPUP_IMAGE) {
		if (x != 0) {
			OnChartLonAngle(get_chart_lon_angle() + 0.5*x);
			flg = true;
		}

		if (y != 0) {
			OnChartLatAngle(get_chart_lat_angle() + 0.5*y);
			flg = true;
		}
	}


	//-- Zoom
	if (z != 0) {
		OnChartZoomFactor(get_chart_zoom_factor() + 0.02*z);
		flg = true;
	}

	//-- Change image index
	char d = 0;
	if (btn[BTN_L1].value && btn[BTN_L1].bToggle)	{ d = -1; }
	if (btn[BTN_R1].value && btn[BTN_R1].bToggle)	{ d = 1; }
	if (d != 0) {
		int idx;
		idx = current_image_index + d;
		if (idx < 0) idx = IMGF.size() - 1;
		if (idx > IMGF.size() - 1)  idx = 0;

		OnImageOpenControler(idx);
		flg = true;
	}


	return flg;
}



bool
Input3DChart(button_state* btn, axis_state* axi)
{
	if (btn[BTN_CIR].value && btn[BTN_CIR].bToggle)		{ OnClose3DChart();	return true; }
	if (btn[BTN_CRS].value && btn[BTN_CRS].bToggle)		{ OnClose3DChart();	return true; }
	

	bool flg = false;

	

	//-- Axes
	float x, y, z, fac;

	fac = get_timer_interval() / 15.0;
	x = axi[0].get_normalized_value();
	y = axi[1].get_normalized_value();
	z = -axi[2].get_normalized_value();
	x *= fac;
	y *= fac;
	z *= fac;


	if (x != 0) {
		OnChartLonAngle(get_chart_lon_angle() + 0.5*x);
		flg = true;
	}

	if (y != 0) {
		OnChartLatAngle(get_chart_lat_angle() + 0.5*y);
		flg = true;
	}


	//-- Zoom
	if (z != 0) {
		OnChartZoomFactor(get_chart_zoom_factor() + 0.02*z);
		flg = true;
	}


	return flg;
}


void
InputMovie(button_state* btn, axis_state* axi)
{
	// Cross button
	if (btn[BTN_CRS].value && btn[BTN_CRS].bToggle)	{
		OnMovieClose();
		OnCommandDraw();
		return;
	}


	// Circle button
	if (btn[BTN_CIR].value && btn[BTN_CIR].bToggle)	{
		if (bMoviePlaying)		{ OnMoviePauseControler(); }
		else					{ OnMoviePlay(); }
	}




	if (!bMoviePlaying) {
		// L2/R2 button
		if (btn[BTN_L2].value)	{ OnMovieHome(); }
		if (btn[BTN_R2].value)	{ OnMovieEnd(); }


		// Movie position
		LONGLONG  pos, end_pos, dpos;
		pos		= dsmv2.GetPosition();
		end_pos = dsmv2.GetEndPosition();
		dpos = 0;


		// L1/R1 button
		bool bL1, bR1;
		bL1 = btn[BTN_L1].value && btn[BTN_L1].bToggle;
		bR1 = btn[BTN_R1].value && btn[BTN_R1].bToggle;
		if (bL1 || bR1) {
			LONGLONG	dpos0 = end_pos / 10;
			if (bL1)	dpos = -dpos0;
			if (bR1)	dpos =  dpos0;
		}

		// axis
		if (axi[3].value != 0 && axi[3].bToggle) {
			dpos	= end_pos / 50;
			if (axi[3].value < 0)  dpos=-dpos;
		}

		if (dpos!=0) {
			pos+=dpos;
			if (pos<0)			{ pos=0; }
			if (pos>end_pos)	{ pos=end_pos; }
			OnMovieSetPosition(pos);
		}
	}
}






void
Input()
{
	//
	bPrevMotion = bMotion;
	bMotion = false;
	ResetInputForces();

	if (!bActive)  return;

	// ジョイスティックの状態を取得
	update_joystick_state();
	button_state*	btn = get_joystick_button_state();
	axis_state*		axi = get_joystick_axis_state();


	if      (bMovieMode)	{
		InputMovie(btn, axi);
		bool flg = ProcessMovement();
		if (flg) {
			OnCommandDraw();
		}

		return;
	}

	bool flg = false;
	switch ( get_sub_mode() ) {
		case SUBMODE_MENU:		flg = InputMenu(btn, axi);		break;
		case SUBMODE_TITLE:		flg = InputTitle(btn, axi);		break;
		case SUBMODE_POINTER:	flg = InputPointer(btn, axi);	break;
		case SUBMODE_IMAGE:		flg = InputImage(btn, axi);		break;
		case SUBMODE_3DCHART:	flg = Input3DChart(btn, axi);	break;
		case SUBMODE_SETTING:	flg = InputSetting(btn, axi);	break;
		case SUBMODE_NORMAL:
		default:				flg = InputNormal(btn, axi);	break;
	}


	bool  flg2;
	flg2 = ProcessMovement();


	//-- Update Window
	if (flg || flg2 || (bPrevMotion && !bMotion)) { 
		OnCommandDraw();
	}
}





//----------------
//  Update state
//----------------
void
OnCommandNone()
{
}



void
OnUpdateView(view_info* pVwif)
{
	vector3d& r = pVwif->frm.r;
	g_camera.set_ori( pVwif->qm );

	//adjust_target_position();
}

void
OnUpdateView2(view_info2* pVwif2)
{
	set_plnt_long( pVwif2->lon );
	set_plnt_lati( pVwif2->lat );
}



void
OnTCPCommand(char cmd)
{
	switch (cmd) {
		case COMMAND_SIGNAL:		OnCommandSignal();		break;
		case COMMAND_DRAW_LOWRES:	OnCommandDraw_LowRes();	break;
		case COMMAND_DRAW_HIGHRES:	OnCommandDraw_HighRes();break;
		case COMMAND_SETTING_ENTER:	OnEnterSettingMode();	break;
		case COMMAND_SETTING_LEAVE:	OnLeaveSettingMode();	break;
		case COMMAND_MENU_ENTER:	OnEnterMenu();			break;
		case COMMAND_MENU_CLOSE:	OnCloseMenu();			break;
		case COMMAND_MENU_EXEC:		OnExecMenu();			break;
		case COMMAND_EXIT_ALL:		OnExitAll();			break;
		case COMMAND_MOVIE_CLOSE:	OnMovieClose();			break;
		case COMMAND_MOVIE_PLAY:	OnMoviePlay();			break;
		case COMMAND_MOVIE_HOME:	OnMovieHome();			break;
		case COMMAND_MOVIE_END:		OnMovieEnd();			break;
		case COMMAND_IMAGE_CLOSE:	OnImageClose();			break;
		case COMMAND_3DCHART_CLOSE:	OnClose3DChart();		break;
		case COMMAND_CAPTURE_IMAGE:	OnCaptureImage();		break;
		case COMMAND_TAKEOFF:		OnTakeoff();			break;
		case COMMAND_LANDING:		OnLanding();			break;
		case COMMAND_LANDING_INIT:	OnLandingInit();		break;
		case COMMAND_SET_PLANETARIUM_MODE:	OnSetPlanetariumMode();	break;
		case COMMAND_INIT_TIME:		OnInitTime();			break;
		case COMMAND_INIT_ROOT_MENU:	OnInitRootMenu();	break;
		default:											break;
	}
}



void
OnDataChar(char type, char val)
{
	switch (type) {
		case DATA_CHAR_DISPMODE:		OnDispMode(val);				break;
		case DATA_CHAR_SETTING_DISP:OnSettingModeDisp(val);				break;
		case DATA_CHAR_PROGRAM:		OnProgram(val);						break;

		case DATA_CHAR_ECLIPSE:			OnEclipse(val);					break;
		case DATA_CHAR_STAR:			OnStar(val);					break;
		case DATA_CHAR_STAR_MARK:		OnStarMark(val);				break;	//****
		case DATA_CHAR_CONSTELLATION:	OnConstellation(val);			break;	
		case DATA_CHAR_GALAXY:			OnGalaxy(val);					break;
		
		case DATA_CHAR_ATMOSPHERE_MODE:	OnPlanetAtmosphere(val);		break;
		case DATA_CHAR_ZOOM_MODE:		OnZoomMode(val);				break;
		case DATA_CHAR_TOPO_ZOOM_MODE:	OnTopoZoomMode(val);			break;
		case DATA_CHAR_LANDING_MODE:	OnLandingMode(val);				break;

		case DATA_CHAR_SPACECRAFT:		OnSpacecraft(val);				break;

		case DATA_CHAR_OORT_CLOUD:		OnOortCloud(val);				break;
		
		case DATA_CHAR_VIRGO_CLUSTER:		OnMarkVirgoCluster(val);	break;

		case DATA_CHAR_MILKYWAY:	OnMilkyWay(val);					break;

		case DATA_CHAR_CLST_GLD:	OnCelestialGrid(val);				break;
		case DATA_CHAR_ECLIPTIC:	OnEcliptic(val);					break;
		case DATA_CHAR_HORIZON_COORD:	OnHorizonCoord(val);			break;
		case DATA_CHAR_GLX_GLD:		OnGalacticGrid(val);				break;
		case DATA_CHAR_DIST_SCALE:	OnDistScale(val);					break;

		case DATA_CHAR_CMB:			OnCMB(val);							break;

		case DATA_CHAR_GAL_CENTER_BG:	OnGalCenterBG(val);				break;

		case DATA_CHAR_NGC:			OnNGC(val);							break;

		case DATA_CHAR_SIDE_LIGHT:	OnSideLight(val);					break;
		case DATA_CHAR_SURFACE_POINTER:	OnSurfacePointer(val);			break;

		case DATA_CHAR_DATE:			OnDate(val);					break;
		case DATA_CHAR_TIMESTEP:		OnTimeStep(val);				break;
		case DATA_CHAR_REALTIME_MODE:	OnRealTimeMode(val);			break;

		case DATA_CHAR_LANG:		OnChangeLanguage(val);				break;

		case DATA_CHAR_TEST:		OnTest(val);						break;

		case DATA_CHAR_SUBMODE_POINTER:	OnPointerMode(val);				break;
		case DATA_CHAR_3DCHART:		OnOpen3DChart(val);					break;

		case DATA_CHAR_SUBTITLES:	OnSubtitles(val);					break;
		case DATA_CHAR_TITLE:		OnTitle(val);						break;
		case DATA_CHAR_ABOUT:		OnCheckAbout(val);					break;
		case DATA_CHAR_DEBUG:		OnDebug(val);						break;
		case DATA_CHAR_KEY_HELP:	OnKeyHelp(val);						break;
		case DATA_CHAR_SINGLE_EYE:	OnSingleEye(val);					break;
		case DATA_CHAR_OBJECT_INFO:	OnObjectInfo(val);					break;

		case DATA_CHAR_PATH_INFO:	OnPathInfo(val);					break;

		case DATA_CHAR_ANIM_TYPE:	OnAnimType(val);					break;
		
		case DATA_CHAR_TITLE_MODE:	OnTitlePageMode(val);				break;
		case DATA_CHAR_TITLE_PAGE:	OnTitlePage(val);					break;

		case DATA_CHAR_MENU_POS:	OnMenuPos(val);						break;

		case DATA_CHAR_CLEAR_MEDIA_FILES:	OnClearMedia(val);			break;

		default:														break;
	}
}


void
OnDataSwitch(char type, char idx, char val)
{
	switch (type) {
		case DATA_SWITCH_DISP:		OnDispSwitch(idx, val);		break;

		default:												break;
	}
}


void
OnDataInt(int_packet *pintp)
{
	switch (pintp->type) {
		case DATA_INT_TARGET:				OnTarget(pintp->val);				break;
		case DATA_INT_PLANETARIUM_TARGET:	OnPlanetariumTarget(pintp->val);	break;
		case DATA_INT_ANIM_COUNTER0:		OnAnimationCounter0(pintp->val);	break;
		case DATA_INT_ANIM_COUNTER:			OnAnimation(pintp->val);			break;
		case DATA_INT_SUBTITLES_INDEX:		OnChangeSubtitlesIndex(pintp->val);	break;
		case DATA_INT_IMAGE_OPEN:			OnImageOpen(pintp->val);			break;
		case DATA_INT_MOVIE_OPEN:			OnMovieOpen(pintp->val);			break;
		case DATA_INT_AUDIO_OPEN:			OnAudioOpen(pintp->val);			break;
		default:																break;
	}
}


void
OnDataFloat(float_packet *pflp)
{
	switch (pflp->type) {
		case DATA_FLOAT_MAG:				OnSetLRef(pflp->val);			break;
		case DATA_FLOAT_PLNT_ANGLE:			OnPlanetariumAngle(pflp->val);	break;
		case DATA_FLOAT_MENU_LON_ANGLE:		OnMenuLonAngle(pflp->val);		break;
		case DATA_FLOAT_MENU_LAT_ANGLE:		OnMenuLatAngle(pflp->val);		break;
		case DATA_FLOAT_MENU_FADE_FACTOR:	OnMenuFade(pflp->val);			break;
		case DATA_FLOAT_CHART_LON_ANGLE:	OnChartLonAngle(pflp->val);		break;
		case DATA_FLOAT_CHART_LAT_ANGLE:	OnChartLatAngle(pflp->val);		break;
		case DATA_FLOAT_CHART_ZOOM_FACTOR:	OnChartZoomFactor(pflp->val);	break;
		case DATA_FLOAT_MOVIE_FADE_ALPHA:	OnMovieFade(pflp->val);			break;
		case DATA_FLOAT_SCREEN_ALPHA:		OnScreenAlpha(pflp->val);		break;
		case DATA_FLOAT_FONT_SIZE:			OnSetFontSize(pflp->val);		break;
		case DATA_FLOAT_EYE_DISTANCE:		OnChangeEyeDistance(pflp->val);	break;
		case DATA_FLOAT_SPEED_FACTOR:		OnSetSpeedFactor(pflp->val);	break;
		case DATA_FLOAT_SET_LANDING_LON:	OnSetLandingLon(pflp->val);		break;
		case DATA_FLOAT_SET_LANDING_LAT:	OnSetLandingLat(pflp->val);		break;
		default:															break;
	}
}


void
OnDataDouble(double_packet *pdblp)
{
	switch (pdblp->type) {
		case DATA_DOUBLE_ZOOM_FACTOR:	OnSetZoom(pdblp->val);		break;
		case DATA_DOUBLE_JD:			OnChangeTime(pdblp->val);	break;
		default:													break;
	}
}


void
OnDataVector3f(vector3f_packet *pv3fp)
{
	switch (pv3fp->type) {
		case DATA_VECTOR3F_FLOATING_OBJECT_POS:		OnChangeFloatingObjectPos(pv3fp->val);	break;
		default:																			break;
	}
}


void
OnDataLONGLONG(LONGLONG_packet *pllp)
{
	switch (pllp->type) {
		case DATA_LONGLONG_MOVIE_POS:	OnMovieSetPosition(pllp->val);		break;
		default:															break;
	}
}

void
OnDataCustom(char* pBuf)
{
	const char  idx   = pBuf[0];
	const char* pData = pBuf+1;

	switch (idx) {
		case DATA_CUSTOM_POINTER_POS:		OnPointerPos((const float*)(pData));			break;
		case DATA_CUSTOM_SPACECRAFT_PATH:	OnSpacecraftPath(pData[0], bool(pData[1]));		break;
		case DATA_CUSTOM_BASE_ORI:			OnBaseCameraOri(*(quaterniond*)(pData));		break;
		default:																			break;
	}
}














//----------------
//  キー入力の処理
//----------------
int
OnKeyDown(HWND hWnd, WPARAM wParam)
{
	switch (wParam) {
		case 'C':	OnCaptureImage();									return 0;

		case 'L':
			{
				OnDispSwitchToggle( DISPSW_PLANET, DISPM_CITY_LIGHTS );
				OnCommandDraw();
			}
			return  0;

		case 'T':
			OnTargetMoveControler();
			return 0;


		case 'F':
			bFPS = !bFPS;
			OnCommandDraw();
			return 0;


		//--- Cosmiv View ---//
		#ifdef COSMIC_VIEW
			case 'I':			make_images_auto("CV_image_view.dat");	return 0;
		#endif


		case VK_F1:
			OnKeyHelp(!bKeyHelp);
			OnCommandDraw();
			return 0;

		case VK_ESCAPE:
			if ( is_fullscreen_mode() ) {
				OnFullScreen(false);
			} else {
				SetMouseCursorCounter(0);
				if (MessageBoxW(hWnd, get_wstring("DLG_MB_EXIT_TITLE").c_str(), get_wstring("DLG_MB_EXIT_QUERY").c_str(), MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES) {
					SendMessage(hWnd, WM_CLOSE, 0, 0);
				}
			}
			return 0;
		default:														break;
	}

	return 1;
}





//----------------
//  字幕の読み込み
//----------------
void
load_subtitles()
{
	//vSubtitles.clear();

	FILE* fp;
	if ((fp=fopen_utf8("subtitles.txt", "rt")) == NULL)  return;

	while (true) {
		// 1つを読む
		string str;
		char ch;

		while (true) {
			char ch2;
			
			ch = getc(fp);
			if (ch==EOF)  break;

			if (_ismbblead(ch)) {
				ch2 = getc(fp);
				if (ch2 == EOF)  {
					ch = EOF;
					break;
				}
				str.push_back(ch);
				str.push_back(ch2);
				continue;
			}

			if (ch=='\\') {
				ch2 = getc(fp);
				if (ch2 == '@')  break;
				str.push_back(ch);
				str.push_back(ch2);
				continue;
			}

			str.push_back(ch);
		}

		if (str.size() > 0) {
			//vSubtitles.push_back( gltext(str.c_str()) );
			str.clear();
		}

		if (ch == EOF)  break;
	}

	fclose(fp);
}




//-------------------
//  ２重起動のチェック
//-------------------
bool
check_multiple_launch()
{
	// ミューテックスを作成する
	HANDLE hMutex;
	hMutex = CreateMutex(NULL, TRUE, "MitakaMutexObject");

	// 作成に失敗した場合
	if (!hMutex)	return false;

	// すでに起動している場合
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		// ウィンドウを手前に持ってくる
		HWND  hWnd0;
		if((hWnd0 = FindWindow(szAppName,NULL)) != NULL) {
		    ShowWindow(hWnd0,SW_RESTORE);  //ウインドウをアクティブにする
		    SetForegroundWindow(hWnd0);    //ウインドウにフォーカスを移す
		}

		return false;
	}

	return true;
}





//===========
//  WinMain
//===========
int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int iCmdShow )
{
	init_FT();


	FrmCounter2.begin_frame();	//****


	init_cpu_info();


	// Windows version
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);

	bVistaOrLater = (osvi.dwMajorVersion >= 6);


	//---------------------
	//  コンピュータ名の取得
	//---------------------
	char computer_name[MAX_COMPUTERNAME_LENGTH +1];
	DWORD len =	MAX_COMPUTERNAME_LENGTH +1;

	GetComputerName(computer_name, &len);


	//-----------------------
	//  設定ファイルの読み込み
	//-----------------------
	init_directory();
	load_ini_file(computer_name);



	//------------
	//  Langages
	//------------
	load_language_files(get_directory(DIR_LOCALE));


	//--------------
	//  二重起動防止
	//--------------
	if (!bMultipleLaunch) {
		if ( !check_multiple_launch() )  return FALSE;
	}



	//----------------
	//  その他の初期化
	//----------------
	//  タイトル作成
	snprintf(szTitle, 256, "%s  Ver.%s", szAppName, szVersion);
	//  カーソル設定
	hCursorArrow = LoadCursor(NULL , IDC_ARROW);



	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

	// スレッドプール
#ifndef XP_VERSION
	init_atmosphere_threadpool();
	init_our_galaxy_threadpool();
#endif

	//========
	//  Main
	//========
	if (bNetwork)	{ Main_NetworkMode(hInstance, iCmdShow); }
	else			{ Main_WindowMode( hInstance, iCmdShow); }



	//------------
	//  Clean-up
	//------------

	//-- スレッドプール
#ifndef XP_VERSION
	cleanup_atmosphere_threadpool();
	cleanup_our_galaxy_threadpool();
#endif


	//-- メモリーの解放
	free_memory_for_galaxy();
	free_multiple_scattering_work();


	//-- メニューの破棄
	destroy_menu_bar();


	free_FT();


	if (bControler || !bNetwork) {
		// DirectShow
		dsmv2.Close();
		CoUninitialize();
	}



	// スクリーンセーバーをオン（ネットワークモードの場合）
	if (bNetwork)	SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE,  NULL, SPIF_SENDCHANGE);	// オン


	//--  CoUninitialize が終了するまで待つ  --
	if (bTimer) {
		while (!bKillTimer) {
			Sleep(5);
		}
	}



	return 0;
}
