/**************************************************************************************************

Copyright (c) 2007-2015  Tsunehiko Kato

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
#include  "mtk_screen_menu.h"
#include  "mtk_dome_master.h"
#include  "mtk_object.h"
#include  "mtk_switches.h"
#include  "mtk_3D_chart.h"
#include  "mtk_colors.h"
#include  "mtk_ini_file.h"
#include  "mtk_title_page.h"
#include  "mtk_solar_system.h"
#include  "mtk_planets.h"
#include  "mtk_spacecraft.h"
#include  "mtk_target.h"
#include  "mtk_movement.h"
#include  "mtk_window_mode.h"
#include  "mtk_preset_scene.h"
#include  "mtk_font.h"
#include  "mtk_camera_setting.h"
#include  "mtk_command.h"
#include  "mtk_tcp.h"
#include  "mtk_mode.h"
#include  "mtk_preset_target.h"
#include  "mtk_objects.h"
#include  "mtk_movie.h"
#include  "mtk_speed.h"
#include  "mtk_strings.h"
#include  "mtk_languages.h"
#include  "mtk_target_planetarium.h"




//************
extern MediaFiles MVF, AUDF, IMGF;
extern char  tyProgram;
extern float FontSize;
extern float vFontSize[5];
extern bool	bControler;
//************




//--------
//  Menu
//--------
static float menu_lon_angle = 0.0;
static float menu_lat_angle = 0.0;
static float MenuTextSize;
float MenuBdWidth;
float MenuTriangleDy;



//--  Menu
static bool  bMenuVisible		= true;		// メニューを表示するかどうか
const char* szMenuTextColorDef		= "#A59FA5";
const char* szMenuSelectColorDef	= "#DEDED3";
const char* szMenuTitleColorDef		= "#EBBE47";
const char* szMenuBgColor1Def		= "#464E64";
const char* szMenuBgColor2Def		= "#000000";
const char* szMenuBdColorDef		= "#3E4557";
const char* szMenuHlColor1Def		= "#599AE5";
const char* szMenuHlColor2Def		= "#2764D2";
RGBf colMenuText, colMenuSelect, colMenuTitle;
RGBf colMenuBgColor1, colMenuBgColor2, colMenuBdColor;
RGBf colMenuHlColor1, colMenuHlColor2;
float MenuCornerR, MenuBgAlpha, MenuBehindAlpha, MenuFadingRate;







struct menu_color {
	RGBAf  txt_color, select_color, title_color;
	RGBAf  bg_color, bd_color;
};



//****
menu			menu_root;
//****
static menu			menu_disp, menu_target, menu_time, menu_system, menu_path, menu_preset;
static menu			menu_movie, menu_image, menu_audio, menu_3Dchart;
static menu_option  menu_scale;
static menu_option	menu_target_solarsystem, menu_target_solarsystem2, menu_target_solarsystem3, menu_target_solarsystem4;
static menu_option	menu_target_spacecraft, menu_target_star, menu_target_galactic, menu_target_extragalactic;
static menu			menu_planet, menu_satellite, menu_minor_planet, menu_TNO;
static menu			menu_cnst, menu_star, menu_eclipse, menu_spacecraft, menu_galactic, menu_extragalactic, menu_grid;
static menu_option	menu_program, menu_clst_sphere, menu_lang, menu_timestep, menu_star_mark;
static menu_option	menu_atmosphere, menu_zoom_mode, menu_topo_zoom, menu_font_size, menu_landing_mode;
static menu			menu_spacecraft_trajectory, menu_speed;
static menu_option	menu_planetarium_target;




//*****
menu_selection	mnuSelect;
//*****

static menu_color		mnu_col, mnu_col_bk;






struct menu_item_def {
	const char* key;
	int	idx;
};


struct menu_item_object_def {
	const char* key;
};







//-----------
//  Get/Set
//-----------
bool
is_menu_visible()
{
	return  bMenuVisible;
}

void
set_menu_visible(bool bFlg)
{
	bMenuVisible = bFlg;
}

float
get_menu_text_size()
{
	return  MenuTextSize;
}

void
set_menu_text_size(float sz)
{
	MenuTextSize = sz;
}

float
get_menu_lon_angle()
{
	return  menu_lon_angle;
}

float
get_menu_lat_angle()
{
	return  menu_lat_angle;
}




//--------
//  Init
//--------
void
init_option_menu(menu_option& mn, int num, int MENU_ID, const char* title_key, const menu_item_def* mndf, int ini_check_no)
{
	mn.set_title(title_key);
	for (int i = 0; i<num; ++i) {
		bool bChk = (i == ini_check_no);
		mn.add_check(MENU_ID, mndf[i].key, mndf[i].idx, bChk);
	}
}



void
init_option_menu_object(menu_option& mn, int num, int MENU_ID, const char* title_key,
						const menu_item_object_def* mndf, int ini_check_no)
{
	mn.set_title(title_key);
	for (int i = 0; i<num; ++i) {
		bool bChk = (i == ini_check_no);
		mn.add_check(MENU_ID, mndf[i].key, get_object_ID(mndf[i].key), bChk);
	}
}





int virgo_menu_idx;


//-- ルートメニューの設定（言語変更時にも呼び出す）
void
init_root_menu()
{
	menu_root.clear();
	menu_root.set_title("MENU_ROOT_TITLE");

	


	//---  Media
	if (MVF.size() > 0)		menu_root.add_child_menu(&menu_movie, "MENU_CHILD_MOVIES");
	if (IMGF.size() > 0)	menu_root.add_child_menu(&menu_image, "MENU_CHILD_IMAGES");
	if (AUDF.size() > 0)	menu_root.add_child_menu(&menu_audio, "MENU_CHILD_AUDIO");
	menu_root.add_child_menu(&menu_3Dchart, "MENU_CHILD_3DCHART");


	//===  Movies  ===
	menu_movie.clear();
	menu_movie.set_title("MENU_CHILD_MOVIES");
	for (int i = 0; i<MVF.size(); i++) {
		menu_movie.add_command(MENU_COMMAND_MOVIE_OPEN, i, MVF[i].menu_title_key);
	}

	//===  Images  ===
	menu_image.clear();
	menu_image.set_title("MENU_CHILD_IMAGES");
	for (int i = 0; i<IMGF.size(); i++) {
		menu_image.add_command(MENU_COMMAND_IMAGE_OPEN, i, IMGF[i].menu_title_key);
	}

	//===  Audio  ===
	menu_audio.clear();
	menu_audio.set_title("MENU_CHILD_AUDIO");
	for (int i = 0; i<AUDF.size(); i++) {
		menu_audio.add_command(MENU_COMMAND_AUDIO_OPEN, i, AUDF[i].menu_title_key);
	}



	//---  View
	menu_root.add_child_menu(&menu_disp, "MENU_CHILD_DISP_SETTING");
	if (tyProgram == 2) {
		menu_root.add_command(MENU_COMMAND_DRAW_MILKYWAY2, 0, "MENU_CMD_BRIGHT_MW");
		menu_root.add_command(MENU_COMMAND_DRAW_MILKYWAY, 0, "MENU_CMD_MW");
	}
	if (tyProgram == 2 || tyProgram == 4) {
		menu_root.add_child_menu(&menu_star_mark, "MENU_CHK_MARK_STARS");
	}
	if (tyProgram == 5) {
		menu_root.add_check(MENU_CHECK_GALAXY_VIRGO, "MENU_CHK_MARK_VIRGO_CLUSTER", 0, false);
		virgo_menu_idx = menu_root.size() - 1;
	}



	//---  Targets
	menu_root.add_child_menu(&menu_target, "MENU_CHILD_TARGET");
	menu_root.add_command(MENU_COMMAND_TARGET_MOVE, 0, "MENU_CMD_GOTO_TARGET");
	if (tyProgram == 3) {
		menu_root.add_child_menu(&menu_target_solarsystem, "MENU_CHILD_SOLAR_SYSTEM");
	}
	menu_root.add_command(MENU_COMMAND_GOTO, 0, "MENU_CMD_GOTO_EARTH");
	if (tyProgram >= 0 && tyProgram <= 4) {
		menu_root.add_command(MENU_COMMAND_PRESET, PRESET_SOLARSYSTEM_COMPLETE_VIEW, "MENU_CMD_PRESET_SS");
	}
	if (tyProgram == 0 || tyProgram == 1 || tyProgram == 2) {
		menu_root.add_command(MENU_COMMAND_GOTO, 2, "MENU_CMD_GOTO_PLEIADES");
	}
	if (tyProgram == 2 || tyProgram == 4) {
		menu_root.add_command(MENU_COMMAND_GOTO, 3, "MENU_CMD_GOTO_M13");
	}
	if (tyProgram == 2) {
		menu_root.add_command(MENU_COMMAND_GOTO, 4, "MENU_CMD_GOTO_GC");
	}
	if (tyProgram == 3) {
		menu_root.add_command(MENU_COMMAND_GOTO, 1, "MENU_CMD_GOTO_CASSINI");
	}
	if (tyProgram == 4) {
		menu_root.add_child_menu(&menu_target_spacecraft, "MENU_CHILD_GOTO_SCF");
	}
	if (tyProgram == 4) {
		//menu_root.add_command(MENU_COMMAND_PRESET, PRESET_SPACECRAFT_PATH, "MENU_CMD_PRESET_SCF_PATH");
		menu_root.add_command(MENU_COMMAND_PRESET, PRESET_VOYAGER_PATH, "MENU_CMD_PRESET_VOYAGER_PATH");
	}
	if (tyProgram == 5) {
		menu_root.add_command(MENU_COMMAND_GOTO, 5, "MENU_CMD_GOTO_VIRGO");
	}



	//---  Scale
	menu_root.add_child_menu(&menu_scale, "MENU_CHILD_SCALE");
	


	//---  Time
	menu_root.add_child_menu(&menu_time, "MENU_CHILD_TIME");
	


	//---  Landing Position
	menu_root.add_command(MENU_COMMAND_LANDING_INIT, 0, "MENU_LANDING_POS_NAME");
	

	//---  Preset
	menu_root.add_child_menu(&menu_preset, "MENU_CHILD_PRESET");

	

	//---  System Menu
	if (bEnableSystemMenu) {
		menu_root.add_child_menu(&menu_system, "MENU_CHILD_SYSTEM");
	}
}





//-- メニューの初期化
void
init_menu()
{
	//===  Root  ===
	init_root_menu();




	//===  3D charts  ===
	menu_3Dchart.set_title("MENU_CHILD_3DCHART");
	menu_3Dchart.add_command(MENU_COMMAND_3DCHART, _3DCHART_PLANETS, "MENU_3DCHART_PLANETS");
	menu_3Dchart.add_command(MENU_COMMAND_3DCHART, _3DCHART_DWARF_PLANETS, "MENU_3DCHART_DWARF_PLANETS");
	menu_3Dchart.add_command(MENU_COMMAND_3DCHART, _3DCHART_STAR_COLOR, "MENU_3DCHART_STAR_COLOR");





	//===  View  ===
	menu_disp.set_title("MENU_DISP_TITLE");
	menu_disp.add_child_menu(&menu_planet, "MENU_DISP_PLANETS");
	menu_disp.add_child_menu(&menu_satellite, "MENU_DISP_SATELLITES");
	menu_disp.add_child_menu(&menu_minor_planet, "MENU_DISP_MINOR_PLANETS");
	menu_disp.add_child_menu(&menu_TNO, "MENU_DISP_TNOS");

	menu_disp.add_child_menu(&menu_star, "MENU_DISP_STARS");
	menu_disp.add_child_menu(&menu_galactic, "MENU_DISP_GALACTIC_OBJECTS");
	menu_disp.add_child_menu(&menu_extragalactic, "MENU_DISP_EXTRAGALACTIC_OBJECTS");

	menu_disp.add_child_menu(&menu_spacecraft, "MENU_DISP_SCF");

	menu_disp.add_child_menu(&menu_cnst, "MENU_DISP_CNST");
	menu_disp.add_child_menu(&menu_clst_sphere, "MENU_DISP_MW");
	menu_disp.add_check(MENU_CHECK_NGC, "MENU_DISP_NGC", 0, false);
	menu_disp.add_child_menu(&menu_grid, "MENU_DISP_GRID");

	menu_disp.add_command(MENU_COMMAND_RESET_VIEWANGLE, 0, "MENU_CMD_RESET_VIEWANGLE");
	menu_disp.add_check(MENU_CHECK_SIDE_LIGHT, "MENU_CHK_HEADLIGHT", 0, false);
	menu_disp.add_check(MENU_CHECK_SURFACE_POINTER, "MENU_CHK_SURFACE_POINTER", 0, false);
	menu_disp.add_check(MENU_CHECK_VIEW_INFO, "MENU_CHK_VIEW_INFO", 0, true);



	//-- Planets
	menu_planet.set_title("MENU_PLANET_TITLE");
	menu_planet.add_check(MENU_CHECK_PLANETS, "MENU_CHK_PLANET_DISP", 0, false);
	menu_planet.add_check(MENU_CHECK_PLANET_NAME, "MENU_CHK_PLANET_NAME", 0, false);
	menu_planet.add_check(MENU_CHECK_PLANET_ORBIT, "MENU_CHK_PLANET_ORBIT", 0, false);
	menu_planet.add_check(MENU_CHECK_PLANET_AXIS, "MENU_CHK_PLANET_AXIS", 0, false);
	menu_planet.add_child_menu(&menu_atmosphere, "MENU_CHILD_PLANET_ATMOSPHERE");
	menu_planet.add_check(MENU_CHECK_PLANET_CLOUD, "MENU_CHK_PLANET_CLOUD", 0, false);
	menu_planet.add_check(MENU_CHECK_PLANET_CITY_LIGHTS, "MENU_CHK_PLANET_CITY_LIGHT", 0, false);
	menu_planet.add_child_menu(&menu_eclipse, "MENU_CHILD_ECLIPSE");
	menu_planet.add_child_menu(&menu_zoom_mode, "MENU_CHILD_ZOOM_MODE");
	menu_planet.add_child_menu(&menu_topo_zoom, "MENU_CHILD_AMP_TOPO");
	menu_planet.add_child_menu(&menu_landing_mode, "MENU_CHILD_LANDING_MODE");

	// Atmosphere
	const int menu_atmosphere_num = 3;
	const menu_item_def  mdf_atmosphere[] = {
		{"MENU_ATMOSPHERE_NONE", ATMOSPHERE_NONE},
		{"MENU_ATMOSPHERE_SINGLE_SCAT", ATMOSPHERE_SINGLE},
		{"MENU_ATMOSPHERE_MULTI_SCAT", ATMOSPHERE_MULTIPLE}
	};
	init_option_menu(menu_atmosphere, menu_atmosphere_num, MENU_CHECK_PLANET_ATMOSPHERE, "MENU_CHK_ATMOSPHERE", mdf_atmosphere, AtmosphereMode);

	// Shadow of moon
	menu_eclipse.set_title("MENU_ECLIPSE_TITLE");
	menu_eclipse.add_check(MENU_CHECK_ECLIPSE_DISP, "MENU_CHK_ECLIPSE_DISP", 0, false);
	menu_eclipse.add_check(MENU_CHECK_ECLIPSE_BORDER, "MENU_CHK_ECLIPSE_BORDER", 0, false);

	// Zoom rate
	const int menu_zoom_mode_num = 5;
	const menu_item_def  mdf_zoom[] = {
		{"MENU_ZOOM_REAL_SCALE", 0},
		{"MENU_ZOOM_1", 1},
		{"MENU_ZOOM_2", 2},
		{"MENU_ZOOM_3", 3},
		{"MENU_ZOOM_4", 4}
	};
	init_option_menu(menu_zoom_mode, menu_zoom_mode_num, MENU_CHECK_PLANET_ZOOM, "MENU_CHK_PLANET_ZOOM", mdf_zoom, get_zoom_mode());

	// Amplification of Topography
	const int menu_topo_zoom_num = 5;
	const menu_item_def  mdf_topo_zoom[] = {
		{"MENU_TOPO_ZOOM_REAL_SCALE", 0},
		{"MENU_TOPO_ZOOM_1", 1},
		{"MENU_TOPO_ZOOM_2", 2},
		{"MENU_TOPO_ZOOM_3", 3},
		{"MENU_TOPO_ZOOM_4", 4}
	};
	init_option_menu(menu_topo_zoom, menu_topo_zoom_num, MENU_CHECK_TOPO_ZOOM, "MENU_CHK_TOPO_ZOOM", mdf_topo_zoom, get_topo_zoom_mode());

	// Landing mode
	const menu_item_def  mdf_landing_mode[] = {
		{"MENU_LANDING_MODE_HORIZONTAL", LANDING_MODE_HORIZONTAL},
		{"MENU_LANDING_MODE_SURFACE_VIEW", LANDING_MODE_SURFACE_VIEW}
	};
	init_option_menu(menu_landing_mode, 2, MENU_CHECK_LANDING_MODE, "MENU_CHK_LANDING_MODE", mdf_landing_mode, 0);



	//-- Satellites
	menu_satellite.set_title("MENU_SATELLITE_TITLE");
	menu_satellite.add_check(MENU_CHECK_SATELLITE_DISP, "MENU_CHK_SATELLITE_DISP", 0, false);
	menu_satellite.add_check(MENU_CHECK_SATELLITE_NAME, "MENU_CHK_SATELLITE_NAME", 0, false);
	menu_satellite.add_check(MENU_CHECK_SATELLITE_ORBIT, "MENU_CHK_SATELLITE_ORBIT", 0, false);
	menu_satellite.add_check(MENU_CHECK_SATELLITE_SELECT, "MENU_CHK_SATELLITE_SELECT", 0, false);



	//-- Minor planets
	menu_minor_planet.set_title("MENU_MP_TITLE");
	menu_minor_planet.add_check(MENU_CHECK_MINOR_PLANET_DISP, "MENU_CHK_MP_DISP", 0, false);
	menu_minor_planet.add_check(MENU_CHECK_MINOR_PLANET_NAME, "MENU_CHK_MP_NAME", 0, false);
	menu_minor_planet.add_check(MENU_CHECK_MINOR_PLANET_ORBIT, "MENU_CHK_MP_ORBIT", 0, false);
	menu_minor_planet.add_check(MENU_CHECK_MINOR_PLANET_SELECT, "MENU_CHK_MP_SELECT", 0, false);



	//-- TNOs
	menu_TNO.set_title("MENU_TNO_TITLE");
	menu_TNO.add_check(MENU_CHECK_TNO_DISP, "MENU_CHK_TNO_DISP", 0, false);
	menu_TNO.add_check(MENU_CHECK_TNO_NAME, "MENU_CHK_TNO_NAME", 0, false);
	menu_TNO.add_check(MENU_CHECK_TNO_ORBIT, "MENU_CHK_TNO_ORBIT", 0, false);
	menu_TNO.add_check(MENU_CHECK_TNO_SELECT, "MENU_CHK_TNO_SELECT", 0, false);
	menu_TNO.add_check(MENU_CHECK_TNO_OORT_CLOUD, "MENU_CHK_OORT_CLOUD", 0, false);



	//-- Stars
	menu_star.set_title("MENU_STAR_TITLE");
	menu_star.add_check(MENU_CHECK_STAR, "MENU_CHK_STAR", 0, false);
	menu_star.add_check(MENU_CHECK_STAR_NAME, "MENU_CHK_STAR_NAME", 0, false);
	menu_star.add_check(MENU_CHECK_STAR_NAME_SELECT, "MENU_CHK_STAR_NAME_SELECT", 0, false);
	menu_star.add_check(MENU_CHECK_STAR_BAYER, "MENU_CHK_STAR_BAYER", 0, false);
	menu_star.add_check(MENU_CHECK_STAR_FRAMSTEED, "MENU_CHK_STAR_FRAMSTEED", 0, false);
	menu_star.add_check(MENU_CHECK_STAR_HIP, "MENU_CHK_STAR_HIP", 0, false);
	menu_star.add_check(MENU_CHECK_STAR_ORBIT, "MENU_CHK_STAR_ORBIT", 0, false);
	menu_star.add_command(MENU_COMMAND_STAR_BRIGHTEN, 0, "MENU_CMD_STAR_BRIGHTEN");
	menu_star.add_command(MENU_COMMAND_STAR_DARKEN, 0, "MENU_CMD_STAR_DARKEN");
	menu_star.add_command(MENU_COMMAND_STAR_DEFAULT_BRIGHTNESS, 0, "MENU_CMD_STAR_DEFAULT_BRIGHTNESS");
	menu_star.add_child_menu(&menu_star_mark, "MENU_CHILD_STAR_MARK");

	// Mark stars
	const int menu_star_mark_num = 3;
	const menu_item_def  mdf_star_mark[] = {
		{"MENU_MARK_NONE", 0},
		{"MENU_MARK_BINARIES", 1},
		{"MENU_MARK_EXOPLANETS", 2}
	};
	init_option_menu(menu_star_mark, menu_star_mark_num, MENU_CHECK_STAR_MARK, "MENU_MARK_TITLE", mdf_star_mark, tyStarMark);



	//-- Galactic objects
	menu_galactic.set_title("MENU_GALACTIC_OBJECT_TITLE");
	menu_galactic.add_check(MENU_CHECK_OUR_GALAXY, "MENU_CHK_OUR_GALAXY", 0, false);
	menu_galactic.add_check(MENU_CHECK_SGR_A, "MENU_CHK_SGR_A_STAR", 0, false);
	menu_galactic.add_check(MENU_CHECK_VERA_OBJECTS, "MENU_CHK_VERA_OBJECT", 0, false);
	menu_galactic.add_check(MENU_CHECK_GLOBULAR_CLUSTER, "MENU_CHK_GC", 0, false);
	menu_galactic.add_check(MENU_CHECK_GAL_CENTER_BG, "MENU_CHK_GAL_CENTER_BG", 0, false);



	//-- Extragalactic objects
	menu_extragalactic.set_title("MENU_EXTRAGALACTIC_OBJECT_TITLE");
	menu_extragalactic.add_check(MENU_CHECK_GALAXY_NEAR, "MENU_CHK_GALAXY_NEAR", 0, false);
	menu_extragalactic.add_check(MENU_CHECK_GALAXY_SDSS, "MENU_CHK_GALAXY_SDSS", 0, false);
	menu_extragalactic.add_check(MENU_CHECK_GALAXY_QSO, "MENU_CHK_GALAXY_QSO", 0, false);
	menu_extragalactic.add_check(MENU_CHECK_CMB, "MENU_CHK_CMB", 0, true);
	menu_extragalactic.add_check(MENU_CHECK_GALAXY_VIRGO, "MENU_CHK_MARK_VIRGO_CLUSTER", 0, false);



	//-- Spacecraft
	menu_spacecraft.set_title("MENU_SPACECRAFT_TITLE");
	menu_spacecraft.add_check(MENU_CHECK_SPACECRAFT, "MENU_CHK_SPACECRAFT_DISP", 0, false);
	menu_spacecraft.add_child_menu(&menu_spacecraft_trajectory, "MENU_CHILD_SPACECRAFT_TRAJECTORY");
	
	// Spacecraft trajectories
	menu_spacecraft_trajectory.set_title("MENU_SPACECRAFT_TRAJECTORIES_TITLE");
	for (int i = 0; i<get_spacecraft_num(); i++) {
		const char* key = get_spacecraft_key(i).c_str();
		menu_spacecraft_trajectory.add_check(MENU_CHECK_SPACECRAFT_TRAJECTORY, key, i, false);
	}



	//-- Constellations
	menu_cnst.set_title("MENU_CNST_TITLE");
	menu_cnst.add_check(MENU_CHECK_CNST_NAME, "MENU_CHK_CNST_NAME", 0, false);
	menu_cnst.add_check(MENU_CHECK_CNST_LINE, "MENU_CHK_CNST_LINE", 0, false);
	menu_cnst.add_check(MENU_CHECK_CNST_ILLUST, "MENU_CHK_CNST_ILLUST", 0, false);
	menu_cnst.add_check(MENU_CHECK_CNST_BND, "MENU_CHK_CNST_BND", 0, false);



	//-- Milky way
	const int menu_clst_sphere_num = 5;
	const menu_item_def  mdf_milkyway[] = {
		{"MENU_MILKYWAY_NONE", 0},
		{"MENU_MILKYWAY_DEFAULT", 1},
		{"MENU_MILKYWAY_BRIGHT", 2},
		{"MENU_MILKYWAY_IR", 3},
		{"MENU_MILKYWAY_CMB", 4},
	};
	init_option_menu(menu_clst_sphere, menu_clst_sphere_num, MENU_CHECK_MILKYWAY, "MENU_CHK_MILKYWAY", mdf_milkyway, tyMilkyWay);



	//-- Grid/Scale
	menu_grid.set_title("MENU_GRID_TITLE");
	menu_grid.add_check(MENU_CHECK_CLST_GRD, "MENU_CHK_CLST_GRID", 0, false);
	menu_grid.add_check(MENU_CHECK_ECLIPTIC, "MENU_CHK_ECLIPTIC", 0, false);
	menu_grid.add_check(MENU_CHECK_HORIZON_COORD, "MENU_CHK_HORIZON_COORD", 0, false);
	menu_grid.add_check(MENU_CHECK_GLX_GRD, "MENU_CHK_GALACTIC_GRID", 0, false);
	menu_grid.add_check(MENU_CHECK_DIST_SCALE, "MENU_CHK_SCALE_CIRCLE", 0, false);
	menu_grid.add_check(MENU_CHECK_SCALE_SQUARE, "MENU_CHK_SCALE_SQUARE", 0, false);





	//===  Targets  ===
	menu_target.set_title("MENU_TARGET_TITLE");
	menu_target.add_child_menu(&menu_target_solarsystem, "MENU_TARGET_SS");
	menu_target.add_child_menu(&menu_target_solarsystem2, "MENU_TARGET_SS2");
	menu_target.add_child_menu(&menu_target_solarsystem3, "MENU_TARGET_SS3");
	menu_target.add_child_menu(&menu_target_solarsystem4, "MENU_TARGET_SS4");
	menu_target.add_child_menu(&menu_target_star, "MENU_TARGET_STAR");
	menu_target.add_child_menu(&menu_target_galactic, "MENU_TARGET_GALACTIC_OBJECT");
	menu_target.add_child_menu(&menu_target_extragalactic, "MENU_TARGET_EXTRAGALACTIC_OBJECT");
	menu_target.add_child_menu(&menu_target_spacecraft, "MENU_TARGET_SCF");
	menu_target.add_command(MENU_COMMAND_TARGET_MOVE, 0, "MENU_CMD_GOTO_TARGET");
	menu_target.add_child_menu(&menu_planetarium_target, "MENU_PLANETARIUM_TARGET");


	//-- Sun and planets
	const menu_item_object_def  mdf_ss[] = {
		{"SUN"},
		{"PLNT_MERCURY"},
		{"PLNT_VENUS"},
		{"PLNT_EARTH"},
		{"PLNT_MARS"},
		{"PLNT_JUPITER"},
		{"PLNT_SATURN"},
		{"PLNT_URANUS"},
		{"PLNT_NEPTUNE"},
	};
	init_option_menu_object(menu_target_solarsystem, 9, MENU_CHECK_TARGET, "MENU_TARGET_SUN_AND_PLANETS_TITLE", mdf_ss, 0);



	//-- Satellites
	const menu_item_object_def  mdf_ss2[] = {
		{"SAT_E_MOON"},
		{"SAT_M_PHOBOS"},
		{"SAT_M_DEIMOS"},
		{"SAT_J_IO"},
		{"SAT_J_EUROPA"},
		{"SAT_J_GANYMEDE"},
		{"SAT_J_CALLISTO"},
		{"SAT_S_MIMAS"},
		{"SAT_S_ENCELADUS"},
		{"SAT_S_TITAN"},
		{"SAT_S_IAPETUS"},
		{"SAT_N_TRITON"},
		{"SAT_P_CHARON"}
	};
	init_option_menu_object(menu_target_solarsystem2, 13, MENU_CHECK_TARGET, "MENU_TARGET_SATELLITES_TITLE", mdf_ss2, 0);



	//-- Minor planets
	const menu_item_object_def  mdf_ss3[] = {
		{"MP_1"},			// Ceres
		{"MP_25143"},		// Itokawa
		{"MP_162173"},		// Ryugu
		{"MP_10199" }		// Chariklo
	};
	init_option_menu_object(menu_target_solarsystem3, 4, MENU_CHECK_TARGET, "MENU_TARGET_MINOR_PLANETS_TITLE", mdf_ss3, 0);



	//-- TNOs
	const menu_item_object_def  mdf_ss4[] = {
		{"PLNT_PLUTO"},
		{"MP_136199"},	// Eris
		{"MP_136108"},	// Haumea
		{"MP_136472"},	// Makemake
		{"MP_90377"}	// Sedna
	};
	init_option_menu_object(menu_target_solarsystem4, 5, MENU_CHECK_TARGET, "MENU_TARGET_TNOS_TITLE", mdf_ss4, 0);



	//-- Stars
	const menu_item_object_def  mdf_star[] = {
		{"HIP_71683"},	// Alpha Centauri
		{"HIP_32349"},	// Sirius
		{"HIP_17702"},	// Pleiades
		{"HIP_36850"}	// Castor
	};
	init_option_menu_object(menu_target_star, 4, MENU_CHECK_TARGET, "MENU_TARGET_STARS_TITLE", mdf_star, 0);



	//-- Galactic objects
	const menu_item_object_def  mdf_galactic[] = {
		{"SGR_A_STAR"},		// SGR A*
		{"GC_NGC_6205"},	// M13
		{"OUR_GALAXY"}
	};
	init_option_menu_object(menu_target_galactic, 3, MENU_CHECK_TARGET, "MENU_TARGET_GALACTIC_OBJECTS_TITLE", mdf_galactic, 0);



	//-- Extragalactic objects
	const menu_item_object_def  mdf_extragalactic[] = {
		{"NBG_NGC_224"},	// Andromeda Galaxy
		{"NBG_NGC_4486"}	// M87/Virgo Cluster
	};
	init_option_menu_object(menu_target_extragalactic, 2, MENU_CHECK_TARGET, "MENU_TARGET_EXTRAGALACTIC_OBJECTS_TITLE", mdf_extragalactic, 0);



	//-- Spacecraft
	{
		int Nscf = get_spacecraft_num();
		menu_item_object_def  *mdf_spccrf;
		mdf_spccrf = new menu_item_object_def[Nscf];
		for (int i = 0; i < Nscf; i++) {
			menu_item_object_def& def = mdf_spccrf[i];
			def.key = get_spacecraft_key(i).c_str();
		}

		init_option_menu_object(menu_target_spacecraft, get_spacecraft_num(), MENU_CHECK_TARGET, "MENU_TARGET_SCF_TITLE", mdf_spccrf, 0);

		delete[] mdf_spccrf;
	}



	//-- Planetarium target
	const menu_item_def  mdf_plnt_target[] = {
		{ "MENU_PLANETARIUM_TARGET_NONE", TARGET_PLNT_NONE },
		{ "MENU_PLANETARIUM_TARGET_SUN", TARGET_PLNT_SUN },
		{ "MENU_PLANETARIUM_TARGET_MOON", TARGET_PLNT_MOON },
		{ "MENU_PLANETARIUM_TARGET_JUPITER", TARGET_PLNT_JUPITER }
	};
	init_option_menu(menu_planetarium_target, 4, MENU_CHECK_PLANETARIUM_TARGET, "MENU_PLANETARIUM_TARGET_TITLE", mdf_plnt_target, get_planetarium_target());



	//===  Time  ===
	menu_time.set_title("MENU_TIME_TITLE");
	menu_time.add_check(MENU_CHECK_DATE, "MENU_CHK_DATE", 0, false);
	menu_time.add_command(MENU_COMMAND_PRESENT_TIME, 0, "MENU_CMD_SET_PRESENT_TIME");
	menu_time.add_child_menu(&menu_timestep, "MENU_CHILD_TIMESTEP");
	menu_time.add_check(MENU_CHECK_REALTIME_MODE, "MENU_CHK_REALTIME_MODE", 0, false);



	//-- Time step
	const int menu_timestep_num = 11;
	const menu_item_def  mdf_timestep[] = {
		{"MENU_TIMESTEP_10SEC", 0},
		{"MENU_TIMESTEP_1MIN", 1},
		{"MENU_TIMESTEP_10MIN", 2},
		{"MENU_TIMESTEP_1HOUR", 3},
		{"MENU_TIMESTEP_4HOURS", 4},
		{"MENU_TIMESTEP_1DAY", 5},
		{"MENU_TIMESTEP_1WEEK", 6},
		{"MENU_TIMESTEP_1MONTH", 7},
		{"MENU_TIMESTEP_1YEAR", 8},
		{"MENU_TIMESTEP_10YEARS", 9},
		{"MENU_TIMESTEP_100YEARS", 10}
	};
	init_option_menu(menu_timestep, menu_timestep_num, MENU_CHECK_TIME, "MENU_CHK_TIMESTEP", mdf_timestep, tyTimeStep);





	//===  Scale  ===
	const int menu_scale_num = 17;
	const menu_item_def  mdf_scale[menu_scale_num] = {
		{"SCL_10MKM", SCALE_10MKM},
		{"SCL_1AU", SCALE_1AU},
		{"SCL_10AU", SCALE_10AU},
		{"SCL_100AU", SCALE_100AU},
		{"SCL_1KAU", SCALE_1000AU},
		{"SCL_10KAU", SCALE_10000AU},
		{"SCL_1LY", SCALE_1LY},
		{"SCL_10LY", SCALE_10LY},
		{"SCL_100LY", SCALE_100LY},
		{"SCL_1KLY", SCALE_1KLY},
		{"SCL_10KLY", SCALE_10KLY},
		{"SCL_100KLY", SCALE_100KLY},
		{"SCL_1MLY", SCALE_1MLY},
		{"SCL_10MLY", SCALE_10MLY},
		{"SCL_100MLY", SCALE_100MLY},
		{"SCL_1GLY", SCALE_1GLY},
		{"SCL_10GLY", SCALE_10GLY}
	};
	menu_scale.set_title("MENU_SCALE_TITLE");
	for (int i = 0; i<menu_scale_num; i++) {
		const menu_item_def&  mdf = mdf_scale[i];
		menu_scale.add_command(MENU_COMMAND_SCALE, mdf.idx, mdf.key);
	}





	//===  System menu  ===
	menu_system.set_title("MENU_SYSTEM_TITLE");
	menu_system.add_child_menu(&menu_program, "MENU_CHILD_SELECT_PROGRAM");
	menu_system.add_child_menu(&menu_lang, "MENU_CHILD_LANGUAGE");
	menu_system.add_child_menu(&menu_font_size, "MENU_CHILD_FONT_SIZE");
	menu_system.add_child_menu(&menu_speed, "MENU_CHILD_SPEED");
	menu_system.add_check(MENU_CHECK_FULLSCREEN, "MENU_CHK_FULL_SCREEN", 0, true);
	menu_system.add_check(MENU_CHECK_SUBTITLES, "MENU_CHK_SUBTITLES", 0, true);
	menu_system.add_check(MENU_CHECK_TITLE, "MENU_CHK_TITLE", 0, true);
	menu_system.add_check(MENU_CHECK_ABOUT, "MENU_CHK_ABOUT", 0, true);
	menu_system.add_check(MENU_CHECK_DEBUG, "MENU_CHK_DEBUG", 0, true);
	menu_system.add_check(MENU_CHECK_SINGLE_EYE, "MENU_CHK_SINGLE_EYE", 0, true);
	menu_system.add_command(MENU_COMMAND_EXIT_ALL, 0, "MENU_CMD_EXIT_ALL");



	//-- Select program
	const int menu_program_num = ThemeNum;
	const menu_item_def  mdf_program[ThemeNum] = {
		{"MENU_PROGRAM_1", 0},
		{"MENU_PROGRAM_2", 1},
		{"MENU_PROGRAM_3", 2},
		{"MENU_PROGRAM_4", 3},
		{"MENU_PROGRAM_5", 4},
		{"MENU_PROGRAM_6", 5},
		{"MENU_PROGRAM_7", 6}
	};
	init_option_menu(menu_program, menu_program_num, MENU_CHECK_PROGRAM, "MENU_PROGRAM_TITLE", mdf_program, 0);



	//-- Language
	const int menu_lang_num = get_num_languages();
	menu_lang.set_title("MENU_LANGUAGE_TITLE");
	for (int i = 0; i<menu_lang_num; ++i) {
		bool bChk = (i == get_current_language());
		menu_lang.add_check(MENU_CHECK_LANG, get_language_name_key(i).c_str(), i, bChk);
	}

	
	//-- Text size
	const int menu_font_size_num = 5;
	const menu_item_def  mdf_font_size[] = {
		{"MENU_FONT_SIZE_LARGEST", 0},
		{"MENU_FONT_SIZE_LARGER", 1},
		{"MENU_FONT_SIZE_MEDIUM", 2},
		{"MENU_FONT_SIZE_SMALLER", 3},
		{"MENU_FONT_SIZE_SMALLEST", 4}
	};
	init_option_menu(menu_font_size, menu_font_size_num, MENU_CHECK_FONT_SIZE, "MENU_FONT_SIZE_TITLE", mdf_font_size, 0);



	//-- Speed
	menu_speed.set_title("MENU_SPEED_TITLE");
	menu_speed.add_command(MENU_COMMAND_SPEED_FASTER, 0, "MENU_SPEED_FASTER");
	menu_speed.add_command(MENU_COMMAND_SPEED_SLOWER, 0, "MENU_SPEED_SLOWER");
	menu_speed.add_command(MENU_COMMAND_SPEED_DEFAULT, 0, "MENU_SPEED_DEFAULT");






    


	//-- プリセット
	menu_preset.set_title("MENU_PRESET_TITLE");
	//menu_preset.add_command(MENU_COMMAND_PRESET, PRESET_MARS_2003, "火星大接近", "Mars 2003");
	//menu_preset.add_command(MENU_COMMAND_PRESET, PRESET_VOYAGER_PATH, "MENU_CMD_PRESET_VOYAGER_PATH");
	menu_preset.add_command(MENU_COMMAND_PRESET, PRESET_NEW_HORIZONS, "MENU_CMD_PRESET_NEW_HORIZONS");
	menu_preset.add_command(MENU_COMMAND_PRESET, PRESET_HAYABUSA2, "MENU_CMD_PRESET_HAYABUSA_2");
	menu_preset.add_command(MENU_COMMAND_PRESET, PRESET_AKATSUKI, "MENU_CMD_PRESET_AKATSUKI");
	menu_preset.add_command(MENU_COMMAND_PRESET, PRESET_BLACKHOLE_GC, "MENU_CMD_PRESET_BLACKHOLE_GC");
	menu_preset.add_command(MENU_COMMAND_PRESET, PRESET_ECLIPSE_2009_7_22, "MENU_CMD_PRESET_ECLIPSE_2009");
	menu_preset.add_command(MENU_COMMAND_PRESET, PRESET_ECLIPSE_2016_3_9_TOKYO, "MENU_CMD_PRESET_ECLIPSE_2016");
	menu_preset.add_command(MENU_COMMAND_PRESET, PRESET_ECLIPSE_2016_3_9_TERNATE, "MENU_CMD_PRESET_ECLIPSE_2016_2");

	



	//-- color
	const float f=1.3;
	const float alpha = 0.7;
	mnu_col.txt_color.set(0.647,0.623,0.647,1);
	mnu_col.select_color.set(0.87,0.87,0.827,1);
	mnu_col.bg_color.set(0.274,0.305,0.392,0.7);
	mnu_col.bd_color.set(0.36,0.4,0.52,1);
	mnu_col.title_color.set(0.8,0.8,0.6,1);


	const float a = 0.4;
	RGBf c = a * mnu_col.txt_color.rgb;
	mnu_col_bk.txt_color.set(c.x, c.y, c.z, 1);
	c = a * mnu_col.select_color.rgb;
	mnu_col_bk.select_color.set(c.x, c.y, c.z, 1);
	c = a * mnu_col.bg_color.rgb;
	mnu_col_bk.bg_color.set(c.x, c.y, c.z, 0.7);
	c = a * mnu_col.bd_color.rgb;
	mnu_col_bk.bd_color.set(c.x, c.y, c.z, 1);
	c = a * mnu_col.title_color.rgb;
	mnu_col_bk.title_color.set(c.x, c.y, c.z, 1);
}




//-------------
// Check Menu
//-------------
void
set_root_menu_checks()
{
	if (tyProgram==5) {
		menu_root.set_check(virgo_menu_idx, bVirgoCluster);
	}
}




void
set_menu_checks()
{
	//-- ルート
	set_root_menu_checks();



	//-- 番組
	menu_program.set_check(tyProgram);


	//-- 移動・ターゲット
	const int tgt_ID = get_target_ID();
	menu_target_solarsystem.set_check_by_index(tgt_ID);
	menu_target_solarsystem2.set_check_by_index(tgt_ID);
	menu_target_solarsystem3.set_check_by_index(tgt_ID);
	menu_target_solarsystem4.set_check_by_index(tgt_ID);
	menu_target_spacecraft.set_check_by_index(tgt_ID);
	menu_target_star.set_check_by_index(tgt_ID);
	menu_target_galactic.set_check_by_index(tgt_ID);
	menu_target_extragalactic.set_check_by_index(tgt_ID);

	menu_planetarium_target.set_check(get_planetarium_target());


	//-- 時刻
	menu_time.set_check(0, bDate);
	menu_time.set_check(3, GetRealtimeMode());
	
	//-- タイムステップ
	menu_timestep.set_check(tyTimeStep);

	//-- システム
	menu_system.set_check(4, is_fullscreen_mode());
	menu_system.set_check(5, bSubtitles);
	menu_system.set_check(6, bAppTitle);
	menu_system.set_check(7, bAbout);
	menu_system.set_check(8, bDebug);
	menu_system.set_check(9, bSingleEye);

	//--文字サイズ
	for (int i=0; i<5; i++) {
		if (FontSize == vFontSize[i]) {
			menu_font_size.set_check(i);
		}
	}


	//-- 表示設定
	menu_disp.set_check(10, bNGC);
	menu_disp.set_check(13, bSideLight);
	menu_disp.set_check(14, bSurfacePointer);
	menu_disp.set_check(15, bViewInfo);

	//-- 星座
	menu_cnst.set_check(0, swConstellation & CNST_NAME);
	menu_cnst.set_check(1, swConstellation & CNST_LINE);
	menu_cnst.set_check(2, swConstellation & CNST_ILLUST);
	menu_cnst.set_check(3, swConstellation & CNST_BND);
	//-- 恒星
	menu_star.set_check(0,  swStar & STAR_DISP);
	menu_star.set_check(1,  swStar & STAR_NAME);
	menu_star.set_check(2,  swStar & STAR_NAME_SELECT);
	menu_star.set_check(3,  swStar & STAR_BAYER);
	menu_star.set_check(4,  swStar & STAR_FRAMSTEED);
	menu_star.set_check(5,  swStar & STAR_HIP);
	menu_star.set_check(6,  swStar & STAR_ORBIT);
	//-- 星のマーク
	menu_star_mark.set_check(tyStarMark);

	//--  惑星
	menu_planet.set_check(0,	test_disp_switch(DISPSW_PLANET,			DISPM_DISP));
	menu_planet.set_check(1,	test_disp_switch(DISPSW_PLANET,			DISPM_NAME));
	menu_planet.set_check(2,	test_disp_switch(DISPSW_PLANET,			DISPM_ORBIT));
	menu_planet.set_check(3,	test_disp_switch(DISPSW_PLANET,			DISPM_AXIS));
	menu_planet.set_check(5,	test_disp_switch(DISPSW_PLANET,			DISPM_CLOUD));
	menu_planet.set_check(6,	test_disp_switch(DISPSW_PLANET,			DISPM_CITY_LIGHTS));

	//--  衛星
	menu_satellite.set_check(0,	test_disp_switch(DISPSW_SATELLITE,		DISPM_DISP));
	menu_satellite.set_check(1,	test_disp_switch(DISPSW_SATELLITE,		DISPM_NAME));
	menu_satellite.set_check(2,	test_disp_switch(DISPSW_SATELLITE,		DISPM_ORBIT));
	menu_satellite.set_check(3,	test_disp_switch(DISPSW_SATELLITE,		DISPM_SELECT));

	//--  小惑星
	menu_minor_planet.set_check(0,	test_disp_switch(DISPSW_MINOR_PLANET,		DISPM_DISP));
	menu_minor_planet.set_check(1,	test_disp_switch(DISPSW_MINOR_PLANET,		DISPM_NAME));
	menu_minor_planet.set_check(2,	test_disp_switch(DISPSW_MINOR_PLANET,		DISPM_ORBIT));
	menu_minor_planet.set_check(3,	test_disp_switch(DISPSW_MINOR_PLANET,		DISPM_SELECT));

	//--  TNO
	menu_TNO.set_check(0,	test_disp_switch(DISPSW_TNO,		DISPM_DISP));
	menu_TNO.set_check(1,	test_disp_switch(DISPSW_TNO,		DISPM_NAME));
	menu_TNO.set_check(2,	test_disp_switch(DISPSW_TNO,		DISPM_ORBIT));
	menu_TNO.set_check(3,	test_disp_switch(DISPSW_TNO,		DISPM_SELECT));
	menu_TNO.set_check(4,	bOortCloud);


	//-- 大気
	menu_atmosphere.set_check(AtmosphereMode);

	//--  月の影
	menu_eclipse.set_check(0, swEclipse & ECLIPSE_DISP);
	menu_eclipse.set_check(1, swEclipse & ECLIPSE_BORDER);


	//-- 拡大率
	menu_zoom_mode.set_check(get_zoom_mode());
	menu_topo_zoom.set_check(get_topo_zoom_mode());

	//-- 離着陸モード
	menu_landing_mode.set_check(get_landing_mode());


	//-- 探査機
	menu_spacecraft.set_check(0, bSpacecraft);

	//-- 探査機の軌道
	for (int i=0; i<get_spacecraft_num(); i++) {
		menu_spacecraft_trajectory.set_check(i, get_spacecraft_trajectory_mode(i) );
	}


	//-- 銀河系内天体
	menu_galactic.set_check(0, swGalaxy & GLXY_OUR_GALAXY);
	menu_galactic.set_check(1, swGalaxy & GLXY_SGR_A_ST);
	menu_galactic.set_check(2, swGalaxy & GLXY_VERA);
	menu_galactic.set_check(3, swGalaxy & GLXY_GLOB_CLSTR);
	menu_galactic.set_check(4, bGalCenterBG);

	//-- 銀河系外天体
	menu_extragalactic.set_check(0, swGalaxy & GLXY_NEARBY_GALAXY);
	menu_extragalactic.set_check(1, swGalaxy & GLXY_DISTANT_GALAXY);
	menu_extragalactic.set_check(2, swGalaxy & GLXY_QSO);
	menu_extragalactic.set_check(3, bCMB);
	menu_extragalactic.set_check(4, bVirgoCluster);
	

	//-- 天の川
	menu_clst_sphere.set_check(tyMilkyWay);

	//-- ガイド線
	menu_grid.set_check(0, bCelestialGrid);
	menu_grid.set_check(1, bEcliptic);
	menu_grid.set_check(2, bHorizonCoord);
	menu_grid.set_check(3, bGalacticGrid);
	menu_grid.set_check(4, swScaleLine & SCALE_CIRCLE);
	menu_grid.set_check(5, swScaleLine & SCALE_SQUARE);

	//-- Languages
	menu_lang.set_check(get_current_language());
}



//--------------------
//  Update menu text
//--------------------
void
update_menu_text()
{
	char title[256];

	const std::string& fmt = get_string("MENU_SPEED_TITLE_FMT");
	_snprintf(title, 256, fmt.c_str(), get_move_speed_factor());
	set_string("MENU_SPEED_TITLE", title);
}




//-----------
//  Execute
//-----------
void
OnEnterMenu()
{
	if (bControler)	{ send_command(COMMAND_MENU_ENTER); }

	//
	set_sub_mode( SUBMODE_MENU );
	mnuSelect.clear();
	mnuSelect.push_back(&menu_root);
	menu_root.set_current_pos(0);

	OnMenuFade(0);
	float v = 1 / GetMenuFadeDuration();
	SetMenuFadeVelocity(v);
}

void
OnLeaveMenu()
{
	set_sub_mode( SUBMODE_NORMAL );
	mnuSelect.clear();
}

void
OnCloseMenu()
{
	if (bControler)	{ send_command(COMMAND_MENU_CLOSE); }

	if (mnuSelect.size() == 1)	{ OnLeaveMenu();  }
	else						{ mnuSelect.pop_back(); }
}


void
OnMenuPos(int pos)
{
	if (bControler)	{ send_data_char(DATA_CHAR_MENU_POS, pos); }

	menu&  mnu = *mnuSelect.back();
	mnu.set_current_pos(pos);
}

void
OnMenuLonAngle(float angle)
{
	menu_lon_angle = angle;
	if (menu_lon_angle < 0)   menu_lon_angle += 360;
	if (menu_lon_angle > 360) menu_lon_angle -= 360;

	if (bControler)	{ send_data_float(DATA_FLOAT_MENU_LON_ANGLE, menu_lon_angle); }
}

void
OnMenuLatAngle(float angle)
{
	menu_lat_angle = angle;
	if (menu_lat_angle < -90)  menu_lat_angle = -90;
	if (menu_lat_angle > 90)   menu_lat_angle = 90;

	if (bControler)	{ send_data_float(DATA_FLOAT_MENU_LAT_ANGLE, menu_lat_angle); }
}


void
OnMenuFade(float fac)
{
	if (bControler) { send_data_float(DATA_FLOAT_MENU_FADE_FACTOR, fac); }
	SetMenuFadeFactor(fac);
}


void
OnExecMenu()
{
	menu&  mnu = *mnuSelect.back();
	int pos = mnu.get_current_pos();
	switch (mnu[pos]->get_type()) {
		case MENU_TY_CHILD:
			{
				menu*  pMnuChild = ((menu_item_child*)mnu[pos])->get_child_menu();
				mnuSelect.push_back( pMnuChild );
				pMnuChild->set_current_pos_auto();

				OnMenuFade(0);
				float v = 1 / GetMenuFadeDuration();
				SetMenuFadeVelocity(v);

				if (bControler)	{ send_command(COMMAND_MENU_EXEC); }
			}
			break;
		case MENU_TY_COMMAND:
			{
				menu_item_command&  mtc = *(menu_item_command*)(mnu[pos]);
				int ID = mtc.get_ID();
				const int idx = mtc.get_idx();

				switch (ID) {
					case MENU_COMMAND_CLOSE:				OnCloseMenu();						return;
					case MENU_COMMAND_EXIT_ALL:				OnExitAll();						return;

					case MENU_COMMAND_SCALE:				OnPresetScale(idx);					return;

					case MENU_COMMAND_MOVIE_OPEN:			OnMovieOpenControler(idx);			return;
					case MENU_COMMAND_IMAGE_OPEN:			OnImageOpenControler(idx);			return;
					case MENU_COMMAND_AUDIO_OPEN:			OnAudioOpenControler(idx);			return;
					case MENU_COMMAND_3DCHART:				OnOpen3DChart(idx);					return;

					case MENU_COMMAND_LANDING_INIT:			OnLandingInit();					return;

					case MENU_COMMAND_STAR_BRIGHTEN:		OnLRef( pow(10, -0.4*0.5) );		return;
					case MENU_COMMAND_STAR_DARKEN:			OnLRef( pow(10, +0.4*0.5) );		return;
					case MENU_COMMAND_STAR_DEFAULT_BRIGHTNESS:	OnSetLRef( 1.0f );				return;
					case MENU_COMMAND_PRESENT_TIME:			OnPresentTime();					return;
					case MENU_COMMAND_TARGET_MOVE:			OnTargetMoveControler();			return;
					case MENU_COMMAND_RESET_VIEWANGLE:		OnResetViewangle();					return;
					case MENU_COMMAND_PRESET:				OnPreset(idx);						return;

					case MENU_COMMAND_SPEED_FASTER:			OnSetSpeedFactor( 1.2 * get_move_speed_factor());		return;
					case MENU_COMMAND_SPEED_SLOWER:			OnSetSpeedFactor( (1.0/1.2) * get_move_speed_factor());		return;
					case MENU_COMMAND_SPEED_DEFAULT:		OnSetSpeedFactor(1.0f);				return;


					case MENU_COMMAND_GOTO:
						{
							if (get_mode() != MODE_3D)  return;
							int tgt_ID = get_preset_target_ID( idx );
							
							if (tgt_ID < 0) return;
							
							OnChangeTargetControler( tgt_ID );

							if (idx == TGT_CASSINI) {
								OnTargetMoveControler();
							}

						}
						return;
					
					case MENU_COMMAND_DRAW_MILKYWAY:		OnMilkyWay(MILKYWAY_VISIBLE);				return;
					case MENU_COMMAND_DRAW_MILKYWAY2:		OnMilkyWay(MILKYWAY_VISIBLE2);				return;
					default:	return;
				}
				break;
			}
		case MENU_TY_CHECK:
			{
				menu_item_check&  mtck = *(menu_item_check*)(mnu[pos]);
				bool bChk = !mtck.get_check();
				mtck.set_check( bChk );

				const int ID = mtck.get_ID();
				const int idx = mtck.get_idx();

				switch (ID) {
					//--  Target
					case MENU_CHECK_TARGET:				if (get_mode() == MODE_3D && idx>=0)	OnChangeTargetControler(idx);	return;
					case MENU_CHECK_PLANETARIUM_TARGET:	if (get_mode() == MODE_PLANETARIUM)		OnPlanetariumTarget(idx);		return;

					case MENU_CHECK_CNST_NAME:			OnCnstSet(CNST_NAME,	bChk);		return;
					case MENU_CHECK_CNST_LINE:			OnCnstSet(CNST_LINE,	bChk);		return;
					case MENU_CHECK_CNST_ILLUST:		OnCnstSet(CNST_ILLUST,	bChk);		return;
					case MENU_CHECK_CNST_BND:			OnCnstSet(CNST_BND,		bChk);		return;

					case MENU_CHECK_STAR:				OnStarSet(STAR_DISP,		bChk);	return;
					case MENU_CHECK_STAR_NAME:			OnStarSet(STAR_NAME,		bChk);	return;
					case MENU_CHECK_STAR_NAME_SELECT:	OnStarSet(STAR_NAME_SELECT,	bChk);	return;
					case MENU_CHECK_STAR_BAYER:			OnStarSet(STAR_BAYER,		bChk);	return;
					case MENU_CHECK_STAR_FRAMSTEED:		OnStarSet(STAR_FRAMSTEED,	bChk);	return;
					case MENU_CHECK_STAR_HIP:			OnStarSet(STAR_HIP,			bChk);	return;
					case MENU_CHECK_STAR_ORBIT:			OnStarSet(STAR_ORBIT,		bChk);	return;

					case MENU_CHECK_STAR_MARK:			OnStarMark( idx );					return;

				
					//--  Planet
					case MENU_CHECK_PLANETS:			OnDispSwitchSetBit(DISPSW_PLANET,	DISPM_DISP,			bChk);	return;
					case MENU_CHECK_PLANET_NAME:		OnDispSwitchSetBit(DISPSW_PLANET,	DISPM_NAME, 		bChk);	return;
					case MENU_CHECK_PLANET_ORBIT:		OnDispSwitchSetBit(DISPSW_PLANET, 	DISPM_ORBIT, 		bChk);	return;
					case MENU_CHECK_PLANET_AXIS:		OnDispSwitchSetBit(DISPSW_PLANET,	DISPM_AXIS,			bChk);	return;
					case MENU_CHECK_PLANET_CLOUD:		OnDispSwitchSetBit(DISPSW_PLANET,	DISPM_CLOUD,		bChk);	return;
					case MENU_CHECK_PLANET_CITY_LIGHTS:	OnDispSwitchSetBit(DISPSW_PLANET,	DISPM_CITY_LIGHTS,	bChk);	return;

					//--  Satellite
					case MENU_CHECK_SATELLITE_DISP:		OnDispSwitchSetBit(DISPSW_SATELLITE,	DISPM_DISP,			bChk);	return;
					case MENU_CHECK_SATELLITE_NAME:		OnDispSwitchSetBit(DISPSW_SATELLITE,	DISPM_NAME,			bChk);	return;
					case MENU_CHECK_SATELLITE_ORBIT:	OnDispSwitchSetBit(DISPSW_SATELLITE,	DISPM_ORBIT,		bChk);	return;
					case MENU_CHECK_SATELLITE_SELECT:	OnDispSwitchSetBit(DISPSW_SATELLITE,	DISPM_SELECT,		bChk);	return;

					//--  Minor Planet
					case MENU_CHECK_MINOR_PLANET_DISP:		OnDispSwitchSetBit(DISPSW_MINOR_PLANET,	DISPM_DISP,		bChk);	return;
					case MENU_CHECK_MINOR_PLANET_NAME:		OnDispSwitchSetBit(DISPSW_MINOR_PLANET,	DISPM_NAME,		bChk);	return;
					case MENU_CHECK_MINOR_PLANET_ORBIT:		OnDispSwitchSetBit(DISPSW_MINOR_PLANET,	DISPM_ORBIT,	bChk);	return;
					case MENU_CHECK_MINOR_PLANET_SELECT:	OnDispSwitchSetBit(DISPSW_MINOR_PLANET,	DISPM_SELECT,	bChk);	return;

					//--  TNOs
					case MENU_CHECK_TNO_DISP:		OnDispSwitchSetBit(DISPSW_TNO,	DISPM_DISP,		bChk);	return;
					case MENU_CHECK_TNO_NAME:		OnDispSwitchSetBit(DISPSW_TNO,	DISPM_NAME,		bChk);	return;
					case MENU_CHECK_TNO_ORBIT:		OnDispSwitchSetBit(DISPSW_TNO,	DISPM_ORBIT,	bChk);	return;
					case MENU_CHECK_TNO_SELECT:		OnDispSwitchSetBit(DISPSW_TNO,	DISPM_SELECT,	bChk);	return;
					case MENU_CHECK_TNO_OORT_CLOUD:	OnOortCloud(bChk);										return;
						
					case MENU_CHECK_PLANET_ATMOSPHERE:	OnPlanetAtmosphere(idx);		return;
					case MENU_CHECK_PLANET_ZOOM:		OnZoomMode(idx);				return;
					case MENU_CHECK_TOPO_ZOOM:			OnTopoZoomMode(idx);			return;
					case MENU_CHECK_LANDING_MODE:		OnLandingMode(idx);				return;

					case MENU_CHECK_ECLIPSE_DISP:		OnEclipseSet(ECLIPSE_DISP,		bChk);	return;
					case MENU_CHECK_ECLIPSE_BORDER:		OnEclipseSet(ECLIPSE_BORDER,	bChk);	return;

					case MENU_CHECK_SPACECRAFT:				OnSpacecraft(bChk);				return;
					case MENU_CHECK_SPACECRAFT_TRAJECTORY:	OnSpacecraftPath(idx, bChk);	return;

					case MENU_CHECK_OUR_GALAXY:			OnGalaxySet(GLXY_OUR_GALAXY, bChk);		return;
					case MENU_CHECK_SGR_A:				OnGalaxySet(GLXY_SGR_A_ST, bChk);		return;
					case MENU_CHECK_VERA_OBJECTS:		OnGalaxySet(GLXY_VERA, bChk);			return;
					case MENU_CHECK_GLOBULAR_CLUSTER:	OnGalaxySet(GLXY_GLOB_CLSTR, bChk);		return;
					case MENU_CHECK_GALAXY_NEAR:		OnGalaxySet(GLXY_NEARBY_GALAXY, bChk);	return;
					case MENU_CHECK_GALAXY_SDSS:		OnGalaxySet(GLXY_DISTANT_GALAXY, bChk);		return;
					case MENU_CHECK_GALAXY_QSO:			OnGalaxySet(GLXY_QSO, bChk);			return;
					case MENU_CHECK_GALAXY_VIRGO:		OnMarkVirgoCluster(bChk);				return;

					case MENU_CHECK_MILKYWAY:		OnMilkyWay(idx);					return;

					case MENU_CHECK_CMB:			OnCMB(bChk);						return;

					case MENU_CHECK_GAL_CENTER_BG:	OnGalCenterBG(bChk);				return;

					case MENU_CHECK_NGC:			OnNGC(bChk);						return;

					case MENU_CHECK_CLST_GRD:		OnCelestialGrid(bChk);				return;
					case MENU_CHECK_ECLIPTIC:		OnEcliptic(bChk);					return;
					case MENU_CHECK_HORIZON_COORD:	OnHorizonCoord(bChk);				return;
					case MENU_CHECK_GLX_GRD:		OnGalacticGrid(bChk);				return;

					case MENU_CHECK_DIST_SCALE:		OnDistScaleSet(SCALE_CIRCLE, bChk);	return;
					case MENU_CHECK_SCALE_SQUARE:	OnDistScaleSet(SCALE_SQUARE, bChk);	return;

					case MENU_CHECK_SIDE_LIGHT:		OnSideLight(bChk);					return;
					case MENU_CHECK_SURFACE_POINTER:OnSurfacePointer(bChk);				return;

					case MENU_CHECK_DATE:			OnDate(bChk);						return;
					case MENU_CHECK_TIME:			OnTimeStep(idx);					return;
					case MENU_CHECK_REALTIME_MODE:	OnRealTimeMode(bChk);				return;


					case MENU_CHECK_LANG:			OnChangeLanguage(idx);				return;

					case MENU_CHECK_TEST:			OnTest(bChk);						return;

					case MENU_CHECK_PROGRAM: 		OnProgram(idx);						return;

					case MENU_CHECK_FONT_SIZE: 		OnFontSize(idx);					return;

					case MENU_CHECK_FULLSCREEN:		OnFullScreen(bChk);					return;
					case MENU_CHECK_SUBTITLES:		OnSubtitles(bChk);					return;
					case MENU_CHECK_TITLE:			OnTitle(bChk);						return;
					case MENU_CHECK_ABOUT:			OnCheckAbout(bChk);					return;
					case MENU_CHECK_DEBUG:			OnDebug(bChk);						return;
					case MENU_CHECK_VIEW_INFO:		OnViewInfo(bChk);					return;
					case MENU_CHECK_SINGLE_EYE:		OnSingleEye(bChk);					return;
					
					case MENU_CHECK_PATH_INFO:		OnPathInfo(bChk);					return;

					default:															return;
				}
				break;
			}
			break;
		default:
			return;
	}
}





//-------------
//  Draw menu
//-------------
inline RGBf
calc_menu_bg_color(RGBf& col1, RGBf& col2, float h, float y)
{
	float f2 = fabs(y)/h;
	float f1 = 1 - f2;
	
	return f1 * col1 + f2 * col2;
}

inline void
set_menu_bg_color(RGBf& col1, RGBf& col2, float h, float y, float alp)
{
	RGBf col = calc_menu_bg_color(col1, col2, h, y);
	glColor4f(col.r, col.g, col.b, alp);
}

void
draw_menu_background(float w, float h, float R, float alp, int sel_idx, float mgn_y)
{
	const int csN = 10;
	static bool bFirst = true;
	static float cs[csN], sn[csN];
	if (bFirst) {
		bFirst = false;

		for (int i=0; i<csN; i++) {
			float th = 0.5*M_PI * float(i)/(csN-1);
			cs[i] = cos(th);
			sn[i] = sin(th);
		}
	}



	const float x1 = R;
	const float y1 = -R;
	const float x2 = w-R;
	const float y2 = -h+R;
	const float ys1 = -sel_idx + mgn_y;
	const float ys2 = ys1 - 1;



	//--------------
	//  Background
	//--------------


	//-- Quads --
	RGBf col1  = colMenuBgColor1;
	RGBf col2  = colMenuBgColor2;
	RGBf col11 = calc_menu_bg_color(col1, col2, h, y1);
	RGBf col21 = calc_menu_bg_color(col1, col2, h, y2);
	RGBf col1s = calc_menu_bg_color(col1, col2, h, ys1);
	RGBf col2s = calc_menu_bg_color(col1, col2, h, ys2);
	RGBf colhl1 = colMenuHlColor1;
	RGBf colhl2 = colMenuHlColor2;

	glBegin(GL_QUADS);
		// Top
		glColor4f(col1.r, col1.g, col1.b, alp);
		glVertex3f(x2, 0, 0);
		glVertex3f(x1, 0, 0);
		glColor4f(col11.r, col11.g, col11.b, alp);
		glVertex3f(x1,y1, 0);
		glVertex3f(x2,y1, 0);
		// Top2
		glColor4f(col11.r, col11.g, col11.b, alp);
		glVertex3f(w, y1, 0);
		glVertex3f(0, y1, 0);
		glColor4f(col1s.r, col1s.g, col1s.b, alp);
		glVertex3f(0, ys1, 0);
		glVertex3f(w, ys1, 0);

		// Highlight
		glColor4f(colhl1.r, colhl1.g, colhl1.b, alp);
		glVertex3f(w, ys1, 0);
		glVertex3f(0, ys1, 0);
		glColor4f(colhl2.r, colhl2.g, colhl2.b, alp);
		glVertex3f(0, ys2, 0);
		glVertex3f(w, ys2, 0);


		// Bottom2
		glColor4f(col2s.r, col2s.g, col2s.b, alp);
		glVertex3f(w, ys2, 0);
		glVertex3f(0, ys2, 0);
		glColor4f(col21.r, col21.g, col21.b, alp);
		glVertex3f(0, y2, 0);
		glVertex3f(w, y2, 0);
		// Bottom
		glColor4f(col21.r, col21.g, col21.b, alp);
		glVertex3f(x2, y2, 0);
		glVertex3f(x1, y2, 0);
		glColor4f(col21.r, col21.g, col21.b, alp);
		glVertex3f(x1, -h, 0);
		glVertex3f(x2, -h, 0);

	glEnd();
	

	//-- Corner --
	float xc, yc;
	
	xc = x1;	yc = y1;
	glBegin(GL_TRIANGLE_FAN);
		set_menu_bg_color(col1, col2, h, yc, alp);
		glVertex3f(xc, yc, 0);
		for (int i=0; i<csN; i++) {
			float y = R*cs[i]+yc;
			set_menu_bg_color(col1, col2, h, y, alp);
			glVertex3f(-R*sn[i]+xc, y, 0);
		}
	glEnd();

	xc = x1;	yc = y2;
	glBegin(GL_TRIANGLE_FAN);
		set_menu_bg_color(col1, col2, h, yc, alp);
		glVertex3f(xc, yc, 0);
		for (int i=0; i<csN; i++) {
			float y = -R*sn[i]+yc;
			set_menu_bg_color(col1, col2, h, y, alp);
			glVertex3f(-R*cs[i]+xc, y, 0);
		}
	glEnd();

	xc = x2;	yc = y2;
	glBegin(GL_TRIANGLE_FAN);
		set_menu_bg_color(col1, col2, h, yc, alp);
		glVertex3f(xc, yc, 0);
		for (int i=0; i<csN; i++) {
			float y = -R*cs[i]+yc;
			set_menu_bg_color(col1, col2, h, y, alp);
			glVertex3f(R*sn[i]+xc, y, 0);
		}
	glEnd();

	xc = x2;	yc = y1;
	glBegin(GL_TRIANGLE_FAN);
		set_menu_bg_color(col1, col2, h, yc, alp);
		glVertex3f(xc, yc, 0);
		for (int i=0; i<csN; i++) {
			float y = R*sn[i]+yc;
			set_menu_bg_color(col1, col2, h, y, alp);
			glVertex3f(R*cs[i]+xc, y, 0);
		}
	glEnd();




	
	//----------
	//  Border
	//----------
	const RGBf cbd = colMenuBdColor;
	glColor4f(cbd.r, cbd.g, cbd.b, alp); 
	glLineWidth(MenuBdWidth);
	
	glBegin(GL_LINE_STRIP);
		// LT
		xc = x1;
		yc = y1;
		for (int i=0; i<csN; i++) {
			glVertex3f(-R*sn[i]+xc, R*cs[i]+yc, 0);
		}
		// LB
		xc = x1;
		yc = y2;
		for (int i=0; i<csN; i++) {
			glVertex3f(-R*cs[i]+xc, -R*sn[i]+yc, 0);
		}
		// RB
		xc = x2;
		yc = y2;
		for (int i=0; i<csN; i++) {
			glVertex3f(R*sn[i]+xc, -R*cs[i]+yc, 0);
		}
		// RT
		xc = x2;
		yc = y1;
		for (int i=0; i<csN; i++) {
			glVertex3f(R*cs[i]+xc, R*sn[i]+yc, 0);
		}
		glVertex3f(x1,0,0);
	glEnd();

}


void
draw_menu_triangle(float sz)
{
	float l = sz;

	float y0 = 0.5;
	float y1 = y0 - 0.5*l;
	float y2 = y0 + 0.5*l;
	float x1 = 0.78*l;
	float d  = 0.1*l;


	glBegin(GL_TRIANGLE_FAN);
		glVertex3f(x1-2*d, y0,   0);
		glVertex3f(  -d, y1+d, 0);
		glVertex3f(   0, y1,   0);
		glVertex3f(  x1, y0,   0);
		glVertex3f(   0, y2,   0);
		glVertex3f(  -d, y2-d,   0);
	glEnd();
}


void
draw_menu_diamond(float sz)
{
	float l = sz;

	float x0 = 0.5;
	float y0 = 0.5;
	float x1 = x0 - 0.5*l;
	float x2 = x0 + 0.5*l;
	float y1 = y0 - 0.5*l;
	float y2 = y0 + 0.5*l;


	glBegin(GL_TRIANGLE_FAN);
		glVertex3f(x0, y2, 0);
		glVertex3f(x1, y0, 0);
		glVertex3f(x0, y1, 0);
		glVertex3f(x2, y0, 0);
	glEnd();
}



void
draw_each_menu(menu* pMenu, float x0, float y0, float z0, menu_color& mnc, const float alp1, const float alp2, const float scl)
{
	const float szT = 1.1f;

	const float dz = 0.1;
	menu&  mnu = *pMenu;
	glTranslatef(x0,-y0,z0);

	//--- 領域サイズの計算
	bool bCheck = false;
	float w, h;
	
	const std::string& title = get_string(mnu.get_title_key());
	get_text_size(title, w, h);	// タイトル
	
	h *= szT;
	float hT = h;

	h += 0.5;
	for (int i=0; i<pMenu->size(); ++i) {
		char ty = mnu[i]->get_type();
		if (ty == MENU_TY_CHECK)  bCheck=true;

		const std::string& name = get_string(mnu[i]->get_name_key());

		float dw, dh;
		if (ty == MENU_TY_CHILD) {
			get_text_size(name, dw, dh);
			dw += 0.5 + 0.6;
		} else {
			get_text_size(name, dw, dh);
		}
		
		if (w < dw)  w = dw;
		h += dh;
	}

	float xx=0;
	if (bCheck) xx=1;

	//--- 背景の描画
	const float mgn_x = 0.4;
	const float mgn_y = 0.4;
	float x2,y2;
	x2 = w + xx + 2*mgn_x;
	y2 = h + 2*mgn_y;


	//-- Backgroud
	const float _mgny = -mgn_y-hT-0.5;
	draw_menu_background(x2, y2, MenuCornerR, alp2, mnu.get_current_pos(), _mgny);


	glLineWidth(1);

	glTranslatef(mgn_x, -mgn_y-hT, 0);


	//--- メニュータイトルの表示
	glColor4f(colMenuTitle.r, colMenuTitle.g, colMenuTitle.b, alp1);
	glPushMatrix();
		glScalef(szT, szT, szT);
		draw_text(title, false);
	glPopMatrix();
	glTranslatef(0,-1.5,0);
	glTranslatef(0, 0.15,0);	//*** 微調整



	//--- チェックの表示
	glColor4f(colMenuSelect.r, colMenuSelect.g, colMenuSelect.b, alp1);
	if (bCheck) {
		glPushMatrix();
		for (int i=0; i<pMenu->size(); ++i) {
			if (mnu[i]->get_type() == MENU_TY_CHECK) {
				if (((menu_item_check*)mnu[i])->get_check()) {
					glPushMatrix();
						glTranslatef(0.0, -0.15 + MenuTriangleDy, 0);
						draw_menu_diamond(0.6);
					glPopMatrix();
				}
			}
			glTranslatef(0,-1,0);
		}
		glPopMatrix();
		glTranslatef(xx,0,0);
	}


	//--- メニュー文字の描画
	for (int i=0; i<pMenu->size(); ++i) {
		glPushMatrix();
		if (mnu.get_current_pos() == i)	{
			glColor4f(colMenuSelect.r, colMenuSelect.g, colMenuSelect.b, alp1);
			glTranslatef(0,0,dz);
		} else {
			glColor4f(colMenuText.r, colMenuText.g, colMenuText.b, alp1);
		}

		char ty = (*pMenu)[i]->get_type();
		const std::string& text = get_string((*pMenu)[i]->get_name_key());
		if (ty == MENU_TY_CHILD)	{
			draw_text(text, false);
			glTranslatef(0.5, -0.15 + MenuTriangleDy, 0);

			if (mnu.get_current_pos() == i)	{
				glColor4f(0.7*colMenuSelect.r, 0.7*colMenuSelect.g, 0.7*colMenuSelect.b, alp1);
			} else {
				glColor4f(0.7*colMenuText.r, 0.7*colMenuText.g, 0.7*colMenuText.b, alp1);
			}
			draw_menu_triangle(0.6);
		} else {
			draw_text(text, false);
		}

		glPopMatrix();
		glTranslatef(0,-1,0);
	}
}



void
set_menu_pos0(const float angle)
{
	const double* Mbc;
	Mbc = get_local_camera_matrix_GL();

	glLoadIdentity();
	glMultMatrixd(Mbc);

	
	double M[16];
	quaternion_to_matrix_GL_tr( get_base_camera_ori(), M );
	glMultMatrixd(M);


	
	if (get_display_mode() == DISPMODE_DOME_MASTER && get_mode() == MODE_PLANETARIUM) {
		glRotatef(get_dome_master_elev_angle() - get_dome_master_elev_angle_planetarium(), 1,0,0);
	}
	

	glRotatef(-menu_lon_angle + angle  ,0,1,0);
	glRotatef(-menu_lat_angle, 1, 0, 0);
	glTranslatef(0, 0, -1.2*get_screen_z());
}






void
set_menu_pos(const float angle)
{
	set_menu_pos0(angle);
	glTranslatef(-0.5*get_screen_w(),0.5*get_screen_w(),0);
}


void
draw_menu(menu_selection& ms, float x0, float y0, float z0)
{
	set_menu_checks();
	update_menu_text();

	float scl = 0.055 * get_screen_w();
	float step = 0.8;
	float step_z = 0.5;

	scl *= MenuTextSize;//****
	if (get_display_mode() == DISPMODE_DOME_MASTER) {
		scl *= get_dome_master_label_mag();
	}


	glPushMatrix();
		set_menu_pos( get_menu_pos_angle() );

		glRotatef( get_menu_rotate_angle(), 0,1,0);

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);

		glScalef(scl,scl,scl);
		for (int i=0; i<ms.size(); ++i) {
			float alp1, alp2, dz;
			const float fac = GetMenuFadeFactor();
			float scl;

			if (i==ms.size()-1) {
				// top window の場合
				alp1 = 1.0f * fac;
				alp2 = MenuBgAlpha * fac;
				dz = -(1-fac);
				scl = fac;
			} else {
				float alp1a, alp1b;
				if (i==ms.size()-2) {
					alp1a = 1.0f;
				} else {
					alp1a = MenuBehindAlpha * (1 - float(ms.size()-3 -i)/MenuFadingRate);
				}
				alp1b = MenuBehindAlpha * (1 - float(ms.size()-2 -i)/MenuFadingRate);
				alp1 = alp1a * (1-fac) + alp1b * fac;
				alp2 = MenuBgAlpha * alp1;
				dz = 0;
				scl = 1;
			}

			glPushMatrix();
				menu_color& mnc = (i==ms.size()-1) ? mnu_col : mnu_col_bk;
				draw_each_menu(ms[i], x0 + (i)*step, y0 + (i)*step, z0 + (i+dz)*step_z, mnc, alp1, alp2, scl);
			glPopMatrix();
		}
	
		glEnable(GL_DEPTH_TEST);
	glPopMatrix();
}
