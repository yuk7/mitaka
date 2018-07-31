/**************************************************************************************************

Copyright (c) 2007-2012, 2015   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#include  "mtk_menu_bar.h"
#include  <stdio.h>
#include  "mitaka.h"
#include  "mtk_mode.h"
#include  "mtk_switches.h"
#include  "mtk_command.h"
#include  "mtk_text.h"
#include  "mtk_target.h"
#include  "mtk_solar_system.h"
#include  "mtk_planets.h"
#include  "mtk_pbuffer.h"
#include  "mtk_spacecraft.h"
#include  "mtk_movement.h"
#include  "mtk_stereo.h"
#include  "mtk_window_mode.h"
#include  "mtk_strings.h"
#include  "mtk_languages.h"
#include  "mtk_preset_scene.h"
#include  "mtk_draw_object_info.h"
#include  "mtk_target_planetarium.h"
#include  "resource.h"


static HMENU	hMenu      = NULL;
static HMENU	hMenuTime  = NULL;
static HMENU	hMenuScale = NULL;

extern float FontSize, vFontSize[5];
extern bool bFBO_Available;			// for Domemaster mode



enum {
	ITEM_COMMAND,
	ITEM_SEPARATOR,
	ITEM_SUBMENU
};

enum {
	SUBMENU_FILE,
	
	SUBMENU_LANDING,
	SUBMENU_LANDING_MODE,
	
	SUBMENU_TARGET,
	SUBMENU_TARGET_SOLARSYSTEM,
	SUBMENU_TARGET_SATELLITES,
	SUBMENU_TARGET_MINOR_PLANETS, 
	SUBMENU_TARGET_TNOS,
	SUBMENU_TARGET_STARS, 
	SUBMENU_TARGET_GALACTIC_OBJECTS, 
	SUBMENU_TARGET_EXTRA_GALACTIC_OBJECTS, 
	SUBMENU_TARGET_SPACECRAFT,
	SUBMENU_PLANETARIUM_TARGET,

	SUBMENU_PRESET,

	SUBMENU_SCALE,
	
	SUBMENU_VIEW,
	SUBMENU_VIEW_PLANETS,
	SUBMENU_VIEW_PLANET_ATMOSPHERE,
	SUBMENU_VIEW_PLANET_ECLIPSE,
	SUBMENU_VIEW_PLANET_MAG_RATE,
	SUBMENU_VIEW_PLANET_AMP_TOPO,
	SUBMENU_VIEW_SATELLITES,
	SUBMENU_VIEW_MINOR_PLANETS,
	SUBMENU_VIEW_TNOS,
	SUBMENU_VIEW_STARS,
	SUBMENU_VIEW_STAR_MARK,
	SUBMENU_VIEW_GALACTIC_OBJECTS,
	SUBMENU_VIEW_EXTRA_GALACTIC_OBJECTS,
	SUBMENU_VIEW_SPACECRAFT,
	SUBMENU_VIEW_SPACECRAFT_TRAJECTORY,
	SUBMENU_VIEW_CONSTELLATIONS,
	SUBMENU_VIEW_MILKY_WAY,
	SUBMENU_VIEW_GRID,
	SUBMENU_VIEW_SCALE_LINES,
	SUBMENU_VIEW_TEXT_SIZE,

	SUBMENU_TIME,

	SUBMENU_SETTING,
	SUBMENU_SETTING_DISP_MODE,

	SUBMENU_HELP,

	SUBMENU_NUM
};






struct submenu_item_info {
	char type;
	UINT ID;
	const char* key;
};



//===  File  ===
submenu_item_info siiFile[] = {
	{ITEM_COMMAND, ID_FILE_SAVEIMAGE,			"MENUB_FILE_SAVE_IMAGE_AS" },
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_COMMAND, ID_FILE_SAVEIMAGE_HIGHRES,	"MENUB_FILE_SAVE_IMAGE_ARB"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_COMMAND, ID_FILE_EXIT,				"MENUB_FILE_EXIT"}
};
const int nFile = sizeof(siiFile) / sizeof(siiFile[0]);

//-- Landing
submenu_item_info siiLanding[] = {
	{ITEM_COMMAND, ID_LANDING_LANDING,				"MENUB_LANDING_LANDING"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_COMMAND, ID_LANDING_MITAKA,		"MENUB_LANDING_POS_NAME"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_SUBMENU, SUBMENU_LANDING_MODE,	"MENUB_LANDING_MODE"}
};
const int nLanding = sizeof(siiLanding) / sizeof(siiLanding[0]);

//-- Landing Mode
submenu_item_info siiLandingMode[] = {
	{ITEM_COMMAND, ID_LANDING_MODE_HORIZONTAL,		"MENUB_LANDING_MODE_HOR"},
	{ITEM_COMMAND, ID_LANDING_MODE_SURFACE_VIEW,	"MENUB_LANDING_MODE_SUR"}
};
const int nLandingMode = sizeof(siiLandingMode) / sizeof(siiLandingMode[0]);




//===  Target  ===
submenu_item_info siiTarget[] = {
	{ITEM_SUBMENU, SUBMENU_TARGET_SOLARSYSTEM,		"MENUB_TARGET_SUN_PLANETS"},
	{ITEM_SUBMENU, SUBMENU_TARGET_SATELLITES,		"MENUB_TARGET_SATELLITS"},
	{ITEM_SUBMENU, SUBMENU_TARGET_MINOR_PLANETS,	"MENUB_TARGET_MINOR_PLANETS"},
	{ITEM_SUBMENU, SUBMENU_TARGET_TNOS,				"MENUB_TARGET_TNOS"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_SUBMENU, SUBMENU_TARGET_STARS,			"MENUB_TARGET_STARS"},
	{ITEM_SUBMENU, SUBMENU_TARGET_GALACTIC_OBJECTS,	"MENUB_TARGET_GALACTIC_OBJECTS"},
	{ITEM_SUBMENU, SUBMENU_TARGET_EXTRA_GALACTIC_OBJECTS, "MENUB_TARGET_EXTRA_GALACTIC_OBJECTS"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_SUBMENU, SUBMENU_TARGET_SPACECRAFT,		"MENUB_TARGET_SPACECRAFT"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_COMMAND, ID_TARGET_GO_TO_TARGET,			"MENUB_TARGET_MOVE_TARGET"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_SUBMENU, SUBMENU_PLANETARIUM_TARGET,		"MENUB_PLANETARIUM_TARGET" }
};
const int nTarget = sizeof(siiTarget) / sizeof(siiTarget[0]);

//--  Target: Sun and Planets
submenu_item_info siiTargetSunPlanet[] = {
	{ITEM_COMMAND, ID_TARGET_SUN,		"MENUB_TARGET_SUN"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_COMMAND, ID_TARGET_MERCURY,	"MENUB_TARGET_MERCURY"},
	{ITEM_COMMAND, ID_TARGET_VENUS,		"MENUB_TARGET_VENUS"},
	{ITEM_COMMAND, ID_TARGET_EARTH,		"MENUB_TARGET_EARTH"},
	{ITEM_COMMAND, ID_TARGET_MARS,		"MENUB_TARGET_MARS"},
	{ITEM_COMMAND, ID_TARGET_JUPITER,	"MENUB_TARGET_JUPITER"},
	{ITEM_COMMAND, ID_TARGET_SATURN,	"MENUB_TARGET_SATURN"},
	{ITEM_COMMAND, ID_TARGET_URANUS,	"MENUB_TARGET_URANUS"},
	{ITEM_COMMAND, ID_TARGET_NEPTUNE,	"MENUB_TARGET_NEPTUNE"}
};
const int nTargetSunPlanet = sizeof(siiTargetSunPlanet) / sizeof(siiTargetSunPlanet[0]);

//--  Target: Satellites
submenu_item_info siiTargetSatellites[] = {
	{ITEM_COMMAND, ID_TARGET_MOON,		"MENUB_TARGET_MOON"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_COMMAND, ID_TARGET_PHOBOS,	"MENUB_TARGET_PHOBOS"},
	{ITEM_COMMAND, ID_TARGET_DEIMOS,	"MENUB_TARGET_DEIMOS"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_COMMAND, ID_TARGET_IO,		"MENUB_TARGET_IO"},
	{ITEM_COMMAND, ID_TARGET_EUROPA,	"MENUB_TARGET_EUROPA"},
	{ITEM_COMMAND, ID_TARGET_GANYMEDE,	"MENUB_TARGET_GANYMEDE"},
	{ITEM_COMMAND, ID_TARGET_CALLISTO,	"MENUB_TARGET_CALLISTO"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_COMMAND, ID_TARGET_MIMAS,		"MENUB_TARGET_MIMAS"},
	{ITEM_COMMAND, ID_TARGET_ENCELADUS,	"MENUB_TARGET_ENCELADUS"},
	{ITEM_COMMAND, ID_TARGET_TITAN,		"MENUB_TARGET_TITAN"},
	{ITEM_COMMAND, ID_TARGET_IAPETUS,	"MENUB_TARGET_IAPETUS"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_COMMAND, ID_TARGET_TRITON,	"MENUB_TARGET_TRITON"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_COMMAND, ID_TARGET_CHARON,	"MENUB_TARGET_CHARON"}
};
const int nTargetSatellites = sizeof(siiTargetSatellites) / sizeof(siiTargetSatellites[0]);

//--  Target: Minor Planets
submenu_item_info siiTargetMinorPlanets[] = {
	{ITEM_COMMAND, ID_TARGET_CERES,		"MENUB_TARGET_CERES"},
	{ITEM_COMMAND, ID_TARGET_ITOKAWA,	"MENUB_TARGET_ITOKAWA"},
	{ITEM_COMMAND, ID_TARGET_RYUGU,		"MENUB_TARGET_RYUGU"},
	{ITEM_COMMAND, ID_TARGET_CHARIKLO,	"MENUB_TARGET_CHARIKLO"}
};
const int nTargetMinorPlanets = sizeof(siiTargetMinorPlanets) / sizeof(siiTargetMinorPlanets[0]);

//--  Target: TNOs
submenu_item_info siiTargetTNOs[] = {
	{ITEM_COMMAND, ID_TARGET_PLUTO,		"MENUB_TARGET_PLUTO"},
	{ITEM_COMMAND, ID_TARGET_ERIS,		"MENUB_TARGET_ERIS"},
	{ITEM_COMMAND, ID_TARGET_HAUMEA,	"MENUB_TARGET_HAUMEA"},
	{ITEM_COMMAND, ID_TARGET_MAKEMAKE,	"MENUB_TARGET_MAKEMAKE"},
	{ITEM_COMMAND, ID_TARGET_SEDNA,		"MENUB_TARGET_SEDNA"}
};
const int nTargetTNOs = sizeof(siiTargetTNOs) / sizeof(siiTargetTNOs[0]);

//--  Target: Stars
submenu_item_info siiTargetStars[] = {
	{ITEM_COMMAND, ID_TARGET_ALP_CEN,	"MENUB_TARGET_ALPHA_CEN"},
	{ITEM_COMMAND, ID_TARGET_SIRIUS,	"MENUB_TARGET_SIRIUS"},
	{ITEM_COMMAND, ID_TARGET_PLEIADES,	"MENUB_TARGET_PLEIADES"},
	{ITEM_COMMAND, ID_TARGET_CASTOR,	"MENUB_TARGET_CASTOR"}
};
const int nTargetStars = sizeof(siiTargetStars) / sizeof(siiTargetStars[0]);

//--  Target: Galactic Objects
submenu_item_info siiTargetGalacticObjects[] = {
	{ITEM_COMMAND, ID_TARGET_SGR_A_STAR,			"MENUB_TARGET_SGR_A_STAR"},
	{ITEM_COMMAND, ID_TARGET_M13,				"MENUB_TARGET_M13"},
	{ITEM_COMMAND, ID_TARGET_OUR_GALAXY_CENTER,	"MENUB_TARGET_OUR_GALAXY_CENTER"}
};
const int nTargetGalacticObjects = sizeof(siiTargetGalacticObjects) / sizeof(siiTargetGalacticObjects[0]);

//--  Target: Extragalactic Objects
submenu_item_info siiTargetExtragalacticObjects[] = {
	{ITEM_COMMAND, ID_TARGET_M31,			"MENUB_TARGET_ANDROMEDA_GAL"},
	{ITEM_COMMAND, ID_TARGET_VIRGO_CLUSTER,	"MENUB_TARGET_VIRGO_CLUSTER"}
};
const int nTargetExtragalacticObjects = sizeof(siiTargetExtragalacticObjects) / sizeof(siiTargetExtragalacticObjects[0]);

//--  Target: Spacecraft
submenu_item_info siiTargetSpacecraft[] = {
	{ITEM_COMMAND, ID_TARGET_PIONEER10,		"MENUB_TARGET_PIONEER10"},
	{ITEM_COMMAND, ID_TARGET_PIONEER11,		"MENUB_TARGET_PIONEER11"},
	{ITEM_COMMAND, ID_TARGET_VOYAGER1,		"MENUB_TARGET_VOYAGER1"},
	{ITEM_COMMAND, ID_TARGET_VOYAGER2,		"MENUB_TARGET_VOYAGER2"},
	{ITEM_COMMAND, ID_TARGET_CASSINI,		"MENUB_TARGET_CASSINI"},
	{ITEM_COMMAND, ID_TARGET_NEW_HORIZONS,	"MENUB_TARGET_NEW_HORIZONS"},
	{ITEM_COMMAND, ID_TARGET_GALILEO,		"MENUB_TARGET_GALILEO"},
	{ITEM_COMMAND, ID_TARGET_HAYABUSA2,		"MENUB_TARGET_HAYABUSA2"},
	{ITEM_COMMAND, ID_TARGET_AKATSUKI,		"MENUB_TARGET_AKATSUKI"}
};
const int nTargetSpacecraft = sizeof(siiTargetSpacecraft) / sizeof(siiTargetSpacecraft[0]);


//--  Planetarium Target
submenu_item_info siiPlanetariumTargets[] = {
	{ ITEM_COMMAND, ID_PLANETARIUM_TARGET_NONE,			"MENUB_PLANETARIUM_TARGET_NONE" },
	{ ITEM_COMMAND, ID_PLANETARIUM_TARGET_SUN,			"MENUB_PLANETARIUM_TARGET_SUN" },
	{ ITEM_COMMAND, ID_PLANETARIUM_TARGET_MOON,			"MENUB_PLANETARIUM_TARGET_MOON" },
	{ ITEM_COMMAND, ID_PLANETARIUM_TARGET_JUPITER,		"MENUB_PLANETARIUM_TARGET_JUPITER" }
};
const int nPlanetariumTargets = sizeof(siiPlanetariumTargets) / sizeof(siiPlanetariumTargets[0]);



//===  Preset  ===
submenu_item_info siiPreset[] = {
	{ITEM_COMMAND, ID_PRESET_NEW_HORIZONS, "MENUB_PRESET_NEW_HORIZONS"},
	{ITEM_COMMAND, ID_PRESET_HAYABUSA_2, "MENUB_PRESET_HAYABUSA_2"},
	{ITEM_COMMAND, ID_PRESET_AKATSUKI, "MENUB_PRESET_AKATSUKI"},
	{ITEM_COMMAND, ID_PRESET_BLACKHOLE_GC, "MENUB_PRESET_BLACKHOLE_GC"},
	{ITEM_COMMAND, ID_PRESET_ECLIPSE_2009, "MENUB_PRESET_ECLIPSE_2009"},
	{ITEM_COMMAND, ID_PRESET_ECLIPSE_2016, "MENUB_PRESET_ECLIPSE_2016"},
	{ITEM_COMMAND, ID_PRESET_ECLIPSE_2016_2, "MENUB_PRESET_ECLIPSE_2016_2"}
};
const int nPreset = sizeof(siiPreset) / sizeof(siiPreset[0]);



//===  Scale  ===
submenu_item_info siiScale[] = {
	{ITEM_COMMAND, ID_SCALE_0, "MENUB_SCALE_10MKM"},
	{ITEM_COMMAND, ID_SCALE_1, "MENUB_SCALE_1AU"},
	{ITEM_COMMAND, ID_SCALE_2, "MENUB_SCALE_10AU"},
	{ITEM_COMMAND, ID_SCALE_3, "MENUB_SCALE_100AU"},
	{ITEM_COMMAND, ID_SCALE_4, "MENUB_SCALE_1KAU"},
	{ITEM_COMMAND, ID_SCALE_5, "MENUB_SCALE_10KAU"},
	{ITEM_COMMAND, ID_SCALE_6, "MENUB_SCALE_1LY"},
	{ITEM_COMMAND, ID_SCALE_7, "MENUB_SCALE_10LY"},
	{ITEM_COMMAND, ID_SCALE_8, "MENUB_SCALE_100LY"},
	{ITEM_COMMAND, ID_SCALE_9, "MENUB_SCALE_1KLY"},
	{ITEM_COMMAND, ID_SCALE_A, "MENUB_SCALE_10KLY"},
	{ITEM_COMMAND, ID_SCALE_B, "MENUB_SCALE_100KLY"},
	{ITEM_COMMAND, ID_SCALE_C, "MENUB_SCALE_1MLY"},
	{ITEM_COMMAND, ID_SCALE_D, "MENUB_SCALE_10MLY"},
	{ITEM_COMMAND, ID_SCALE_E, "MENUB_SCALE_100MLY"},
	{ITEM_COMMAND, ID_SCALE_F, "MENUB_SCALE_1GLY"},
	{ITEM_COMMAND, ID_SCALE_G, "MENUB_SCALE_10GLY"}
};
const int nScale = sizeof(siiScale) / sizeof(siiScale[0]);



//===  View  ===
submenu_item_info siiView[] = {
	{ITEM_SUBMENU, SUBMENU_VIEW_PLANETS,				"MENUB_VIEW_PLANETS"},
	{ITEM_SUBMENU, SUBMENU_VIEW_SATELLITES,				"MENUB_VIEW_SATELLITES"},
	{ITEM_SUBMENU, SUBMENU_VIEW_MINOR_PLANETS,			"MENUB_VIEW_MINOR_PLANETS"},
	{ITEM_SUBMENU, SUBMENU_VIEW_TNOS,					"MENUB_VIEW_TNOS"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_SUBMENU, SUBMENU_VIEW_STARS,					"MENUB_VIEW_STARS"},
	{ITEM_SUBMENU, SUBMENU_VIEW_GALACTIC_OBJECTS,		"MENUB_VIEW_GALACTIC_OBJECTS"},
	{ITEM_SUBMENU, SUBMENU_VIEW_EXTRA_GALACTIC_OBJECTS,	"MENUB_VIEW_EXTRA_GALACTIC_OBJECTS"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_SUBMENU, SUBMENU_VIEW_SPACECRAFT,				"MENUB_VIEW_SPACECRAFT"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_SUBMENU, SUBMENU_VIEW_CONSTELLATIONS,			"MENUB_VIEW_CONSTELLATIONS"},
	{ITEM_SUBMENU, SUBMENU_VIEW_MILKY_WAY,				"MENUB_VIEW_MILKYWAY"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_SUBMENU, SUBMENU_VIEW_GRID,					"MENUB_VIEW_GRID"},
	{ITEM_SUBMENU, SUBMENU_VIEW_SCALE_LINES,				"MENUB_VIEW_SCALELINE"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_COMMAND, ID_VIEW_HEADLIGHT,					"MENUB_VIEW_HEADLIGHT"},
	{ITEM_COMMAND, ID_VIEW_PLANET_POINTER,				"MENUB_VIEW_PLANET_POINTER"},
	{ITEM_COMMAND, ID_VIEW_OBJECT_INFO,					"MENUB_VIEW_OBJECT_INFO"},
	{ITEM_COMMAND, ID_VIEW_VIEW_INFO, "MENUB_VIEW_VIEW_INFO"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_COMMAND, ID_VIEW_RESET_VIEW,					"MENUB_VIEW_RESET_VIEW"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_SUBMENU, SUBMENU_VIEW_TEXT_SIZE,				"MENUB_VIEW_TEXT_SIZE"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_COMMAND, ID_VIEW_FULLSCREEN,					"MENUB_VIEW_FULLSCREEN"}
};
const int nView = sizeof(siiView) / sizeof(siiView[0]);

//--  View: Planets
submenu_item_info siiViewPlanets[] = {
	{ITEM_COMMAND, ID_VIEW_PLANET_DISP,				"MENUB_VIEW_PLANET_DISPLAY"},
	{ITEM_COMMAND, ID_VIEW_PLANET_NAME,				"MENUB_VIEW_PLANET_NAME"},
	{ITEM_COMMAND, ID_VIEW_PLANET_ORBIT,			"MENUB_VIEW_PLANET_ORBIT"},
	{ITEM_COMMAND, ID_VIEW_PLANET_AXIS,				"MENUB_VIEW_PLANET_AXIS_ROTATION"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_SUBMENU, SUBMENU_VIEW_PLANET_ATMOSPHERE,	"MENUB_VIEW_PLANET_ATMOSPHERE"},
	{ITEM_COMMAND, ID_VIEW_PLANET_CLOUD,			"MENUB_VIEW_PLANET_CLOUD"},
	{ITEM_COMMAND, ID_VIEW_PLANET_CITY_LIGHT,		"MENUB_VIEW_PLANET_CITY_LIGHT"},
	{ITEM_SUBMENU, SUBMENU_VIEW_PLANET_ECLIPSE,		"MENUB_VIEW_PLANET_ECLIPSE"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_SUBMENU, SUBMENU_VIEW_PLANET_MAG_RATE,	"MENUB_VIEW_PLANET_MAG_RATE"},
	{ITEM_SUBMENU, SUBMENU_VIEW_PLANET_AMP_TOPO,	"MENUB_VIEW_PLANET_AMP_TOPO"}
};
const int nViewPlanets = sizeof(siiViewPlanets) / sizeof(siiViewPlanets[0]);

//--  View: Planets: Atmosphere
submenu_item_info siiViewPlanetAtmosphere[] = {
	{ITEM_COMMAND, ID_VIEW_PLANET_ATMOSPHERE_NONE,		"MENUB_VIEW_PLANET_ATMOSPHERE_NONE"},
	{ITEM_COMMAND, ID_VIEW_PLANET_ATMOSPHERE_SINGLE,	"MENUB_VIEW_PLANET_ATMOSPHERE_SINGLE"},
	{ITEM_COMMAND, ID_VIEW_PLANET_ATMOSPHERE_MULTIPLE,	"MENUB_VIEW_PLANET_ATMOSPHERE_MULTIPLE"}
};
const int nViewPlanetAtmosphere = sizeof(siiViewPlanetAtmosphere) / sizeof(siiViewPlanetAtmosphere[0]);

//--  View: Planets: Eclipse
submenu_item_info siiViewPlanetEclipse[] = {
	{ITEM_COMMAND, ID_VIEW_PLANET_ECLIPSE_DISP,	"MENUB_VIEW_PLANET_ECLIPSE_DISPLAY"},
	{ITEM_COMMAND, ID_VIEW_PLANET_ECLIPSE_BORDER,	"MENUB_VIEW_PLANET_ECLIPSE_BORDER"}
};
const int nViewPlanetEclipse = sizeof(siiViewPlanetEclipse) / sizeof(siiViewPlanetEclipse[0]);

//--  View: Planets: MagRate
submenu_item_info siiViewPlanetMagRate[] = {
	{ITEM_COMMAND, ID_VIEW_PLANET_MAG_RATE_0, "MENUB_VIEW_PLANET_MAG_RATE_0"},
	{ITEM_COMMAND, ID_VIEW_PLANET_MAG_RATE_1, "MENUB_VIEW_PLANET_MAG_RATE_1"},
	{ITEM_COMMAND, ID_VIEW_PLANET_MAG_RATE_2, "MENUB_VIEW_PLANET_MAG_RATE_2"},
	{ITEM_COMMAND, ID_VIEW_PLANET_MAG_RATE_3, "MENUB_VIEW_PLANET_MAG_RATE_3"},
	{ITEM_COMMAND, ID_VIEW_PLANET_MAG_RATE_4, "MENUB_VIEW_PLANET_MAG_RATE_4"}
};
const int nViewPlanetMagRate = sizeof(siiViewPlanetMagRate) / sizeof(siiViewPlanetMagRate[0]);

//--  View: Planets: AmpTopo
submenu_item_info siiViewPlanetAmpTopo[] = {
	{ITEM_COMMAND, ID_VIEW_PLANET_AMP_TOPO_0, "MENUB_VIEW_PLANET_AMP_TOPO_0"},
	{ITEM_COMMAND, ID_VIEW_PLANET_AMP_TOPO_1, "MENUB_VIEW_PLANET_AMP_TOPO_1"},
	{ITEM_COMMAND, ID_VIEW_PLANET_AMP_TOPO_2, "MENUB_VIEW_PLANET_AMP_TOPO_2"},
	{ITEM_COMMAND, ID_VIEW_PLANET_AMP_TOPO_3, "MENUB_VIEW_PLANET_AMP_TOPO_3"},
	{ITEM_COMMAND, ID_VIEW_PLANET_AMP_TOPO_4, "MENUB_VIEW_PLANET_AMP_TOPO_4"},
};
const int nViewPlanetAmpTopo = sizeof(siiViewPlanetAmpTopo) / sizeof(siiViewPlanetAmpTopo[0]);

//--  View: Satellites
submenu_item_info siiViewSatellites[] = {
	{ITEM_COMMAND, ID_VIEW_SATELLITE_DISP,		"MENUB_VIEW_SATELLITE_DISPLAY"},
	{ITEM_COMMAND, ID_VIEW_SATELLITE_NAME,		"MENUB_VIEW_SATELLITE_NAME"},
	{ITEM_COMMAND, ID_VIEW_SATELLITE_ORBIT,		"MENUB_VIEW_SATELLITE_ORBIT"},
	{ITEM_COMMAND, ID_VIEW_SATELLITE_SELECT,	"MENUB_VIEW_SATELLITE_SELECT"},
};
const int nViewSatellites = sizeof(siiViewSatellites) / sizeof(siiViewSatellites[0]);

//--  View: Minor Planets
submenu_item_info siiViewMinorPlanets[] = {
	{ITEM_COMMAND, ID_VIEW_MINOR_PLANET_DISP,	"MENUB_VIEW_MINOR_PLANET_DISPLAY"},
	{ITEM_COMMAND, ID_VIEW_MINOR_PLANET_NAME,	"MENUB_VIEW_MINOR_PLANET_NAME"},
	{ITEM_COMMAND, ID_VIEW_MINOR_PLANET_ORBIT,	"MENUB_VIEW_MINOR_PLANET_ORBIT"},
	{ITEM_COMMAND, ID_VIEW_MINOR_PLANET_SELECT,	"MENUB_VIEW_MINOR_PLANET_SELECT"},
};
const int nViewMinorPlanets = sizeof(siiViewMinorPlanets) / sizeof(siiViewMinorPlanets[0]);

//--  View: TNOs
submenu_item_info siiViewTNOs[] = {
	{ITEM_COMMAND, ID_VIEW_TNO_DISP,		"MENUB_VIEW_TNO_DISPLAY"},
	{ITEM_COMMAND, ID_VIEW_TNO_NAME,		"MENUB_VIEW_TNO_NAME"},
	{ITEM_COMMAND, ID_VIEW_TNO_ORBIT,		"MENUB_VIEW_TNO_ORBIT"},
	{ITEM_COMMAND, ID_VIEW_TNO_SELECT,		"MENUB_VIEW_TNO_SELECT"},
	{ITEM_COMMAND, ID_VIEW_OORT_CLOUD_DISP,	"MENUB_VIEW_OORT_CLOUD_DISPLAY"}
};
const int nViewTNOs = sizeof(siiViewTNOs) / sizeof(siiViewTNOs[0]);

//--  View: Stars
submenu_item_info siiViewStars[] = {
	{ITEM_COMMAND, ID_VIEW_STAR_DISP,			"MENUB_VIEW_STAR_DISPLAY"},
	{ITEM_COMMAND, ID_VIEW_STAR_NAME,			"MENUB_VIEW_STAR_PROPER_NAME"},
	{ITEM_COMMAND, ID_VIEW_STAR_NAME_SELECT,	"MENUB_VIEW_STAR_SELECT"},
	{ITEM_COMMAND, ID_VIEW_STAR_BAYER,			"MENUB_VIEW_STAR_BAYER"},
	{ITEM_COMMAND, ID_VIEW_STAR_FRAMSTEED,		"MENUB_VIEW_STAR_FRAMSTEED"},
	{ITEM_COMMAND, ID_VIEW_STAR_HIP,			"MENUB_VIEW_STAR_HIP"},
	{ITEM_COMMAND, ID_VIEW_STAR_ORBIT,			"MENUB_VIEW_STAR_ORBIT"},
	{ITEM_SUBMENU, SUBMENU_VIEW_STAR_MARK,		"MENUB_VIEW_STAR_MARK"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_COMMAND, ID_VIEW_STAR_BRIGHTEN,		"MENUB_VIEW_STAR_BRIGHTEN"},
	{ITEM_COMMAND, ID_VIEW_STAR_DARKEN,			"MENUB_VIEW_STAR_DARKEN"},
	{ITEM_COMMAND, ID_VIEW_STAR_STANDARD,		"MENUB_VIEW_STAR_DEFAULT"}
};
const int nViewStars = sizeof(siiViewStars) / sizeof(siiViewStars[0]);

//--  View: Stars: Mark
submenu_item_info siiViewStarMark[] = {
	{ITEM_COMMAND, ID_VIEW_STAR_MARK_NONE,		"MENUB_VIEW_STAR_MARK_NONE"},
	{ITEM_COMMAND, ID_VIEW_STAR_MARK_BINARY,	"MENUB_VIEW_STAR_MARK_BINARY"},
	{ITEM_COMMAND, ID_VIEW_STAR_MARK_EXOPLANET,	"MENUB_VIEW_STAR_MARK_EXOPLANET"}
};
const int nViewStarMark = sizeof(siiViewStarMark) / sizeof(siiViewStarMark[0]);

//--  View: Galactic Objects
submenu_item_info siiViewGalacticObjects[] = {
	{ITEM_COMMAND, ID_VIEW_OUR_GALAXY_DISP,			"MENUB_VIEW_OUR_GALAXY"},
	{ITEM_COMMAND, ID_VIEW_SGR_A_STAR_DISP,			"MENUB_VIEW_SGR_A_STAR"},
	{ITEM_COMMAND, ID_VIEW_VERA_OBJECT_DISP,		"MENUB_VIEW_VERA"},
	{ITEM_COMMAND, ID_VIEW_GLOBULAR_CLUSTER_DISP,	"MENUB_VIEW_GC"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_COMMAND, ID_VIEW_GALACTIC_CENTER_BG_DISP,	"MENUB_VIEW_GAL_CENTER_BG"}
};
const int nViewGalacticObjects = sizeof(siiViewGalacticObjects) / sizeof(siiViewGalacticObjects[0]);

//--  View: Extraglactic Objects
submenu_item_info siiViewExtragalacticObjects[] = {
	{ITEM_COMMAND, ID_VIEW_NEARBY_GALAXY_DISP,	"MENUB_VIEW_NBG"},
	{ITEM_COMMAND, ID_VIEW_SDSS_GALAXY_DISP,	"MENUB_VIEW_SDSS"},
	{ITEM_COMMAND, ID_VIEW_QSO_DISP,			"MENUB_VIEW_QSO"},
	{ITEM_COMMAND, ID_VIEW_CMB_DISP,			"MENUB_VIEW_CMB"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_COMMAND, ID_VIEW_MARK_VIRGO_CLUSTER,	"MENUB_VIEW_GALAXY_MARK_VIRGO"}
};
const int nViewExtragcticObjects = sizeof(siiViewExtragalacticObjects) / sizeof(siiViewExtragalacticObjects[0]);

//--  View: Spacecraft
submenu_item_info siiViewSpacecraft[] = {
	{ITEM_COMMAND, ID_VIEW_SPACECRAFT_DISP,				"MENUB_VIEW_SPACECRAFT_DISPLAY"},
	{ITEM_SUBMENU, SUBMENU_VIEW_SPACECRAFT_TRAJECTORY,	"MENUB_VIEW_SPACECRAFT_TRAJECTORY"}
};
const int nViewSpacecraft = sizeof(siiViewSpacecraft) / sizeof(siiViewSpacecraft[0]);

//--  View: Spacecraft: Trajectory
submenu_item_info siiViewSpacecraftTrajectory[] = {
	{ITEM_COMMAND, ID_VIEW_SPACECRAFT_TRAJECTORY_PIONEER10, "MENUB_VIEW_SPACECRAFT_PIONEER10"},
	{ITEM_COMMAND, ID_VIEW_SPACECRAFT_TRAJECTORY_PIONEER11, "MENUB_VIEW_SPACECRAFT_PIONEER11"},
	{ITEM_COMMAND, ID_VIEW_SPACECRAFT_TRAJECTORY_VOYAGER1, "MENUB_VIEW_SPACECRAFT_VOYAGER1"},
	{ITEM_COMMAND, ID_VIEW_SPACECRAFT_TRAJECTORY_VOYAGER2, "MENUB_VIEW_SPACECRAFT_VOYAGER2"},
	{ITEM_COMMAND, ID_VIEW_SPACECRAFT_TRAJECTORY_CASSINI, "MENUB_VIEW_SPACECRAFT_CASSINI"},
	{ITEM_COMMAND, ID_VIEW_SPACECRAFT_TRAJECTORY_NEW_HORIZONS, "MENUB_VIEW_SPACECRAFT_NEW_HORIZONS"},
	{ITEM_COMMAND, ID_VIEW_SPACECRAFT_TRAJECTORY_GALILEO, "MENUB_VIEW_SPACECRAFT_GALILEO"},
	{ITEM_COMMAND, ID_VIEW_SPACECRAFT_TRAJECTORY_HAYABUSA2, "MENUB_VIEW_SPACECRAFT_HAYABUSA2"},
	{ITEM_COMMAND, ID_VIEW_SPACECRAFT_TRAJECTORY_AKATSUKI, "MENUB_VIEW_SPACECRAFT_AKATSUKI"}
};
const int nViewSpacecraftTrajectory = sizeof(siiViewSpacecraftTrajectory) / sizeof(siiViewSpacecraftTrajectory[0]);

//--  View: Constellations
submenu_item_info siiViewConstellations[] = {
	{ITEM_COMMAND, ID_VIEW_CNST_NAME,		"MENUB_VIEW_CONSTELLATION_NAME"},
	{ITEM_COMMAND, ID_VIEW_CNST_LINE,		"MENUB_VIEW_CONSTELLATION_LINE"},
	{ITEM_COMMAND, ID_VIEW_CNST_ILLUST,		"MENUB_VIEW_CONSTELLATION_ILLUST"},
	{ITEM_COMMAND, ID_VIEW_CNST_BOUNDARY,	"MENUB_VIEW_CONSTELLATION_BOUNDARY"}
};
const int nViewConstellations = sizeof(siiViewConstellations) / sizeof(siiViewConstellations[0]);

//--  View: Milky Way
submenu_item_info siiViewMilkyWay[] = {
	{ITEM_COMMAND, ID_VIEW_CLST_NONE,				"MENUB_VIEW_MILKYWAY_NONE"},
	{ITEM_COMMAND, ID_VIEW_CLST_MILKYWAY,			"MENUB_VIEW_MILKYWAY_NORMAL"},
	{ITEM_COMMAND, ID_VIEW_CLST_MILKYWAY_BRIGHT,	"MENUB_VIEW_MILKYWAY_BRIGHTER"},
	{ITEM_COMMAND, ID_VIEW_CLST_MILKYWAY_IR,		"MENUB_VIEW_MILKYWAY_IR"},
	{ITEM_COMMAND, ID_VIEW_CLST_CMB,				"MENUB_VIEW_MILKYWAY_CMB"}
};
const int nViewMilkyWay = sizeof(siiViewMilkyWay) / sizeof(siiViewMilkyWay[0]);

//--  View: Grid
submenu_item_info siiViewGrid[] = {
	{ITEM_COMMAND, ID_VIEW_GRID_EQ,			"MENUB_VIEW_GRID_EQ"},
	{ITEM_COMMAND, ID_VIEW_GRID_EC,			"MENUB_VIEW_GRID_EC"},
	{ITEM_COMMAND, ID_VIEW_GRID_GAL,		"MENUB_VIEW_GRID_GALACTIC"},
	{ITEM_COMMAND, ID_VIEW_GRID_HORIZON,	"MENUB_VIEW_GRID_HORIZONTAL"}
};
const int nViewGrid = sizeof(siiViewGrid) / sizeof(siiViewGrid[0]);

//--  View: Scale Lines
submenu_item_info siiViewScaleLines[] = {
	{ITEM_COMMAND, ID_VIEW_SCALELINE_CIRCLE,	"MENUB_VIEW_SCALELINE_CIRCLE"},
	{ITEM_COMMAND, ID_VIEW_SCALELINE_SQUARE,	"MENUB_VIEW_SCALELINE_SQUARE"}
};
const int nViewScaleLines = sizeof(siiViewScaleLines) / sizeof(siiViewScaleLines[0]);

//--  View: Text Size
submenu_item_info siiViewTextSize[] = {
	{ITEM_COMMAND, ID_VIEW_TEXTSIZE_LARGEST,	"MENUB_VIEW_TEXTSIZE_LARGEST"},
	{ITEM_COMMAND, ID_VIEW_TEXTSIZE_LARGER,		"MENUB_VIEW_TEXTSIZE_LARGER"},
	{ITEM_COMMAND, ID_VIEW_TEXTSIZE_MEDIUM,		"MENUB_VIEW_TEXTSIZE_MEDIUM"},
	{ITEM_COMMAND, ID_VIEW_TEXTSIZE_SMALLER,	"MENUB_VIEW_TEXTSIZE_SMALLER"},
	{ITEM_COMMAND, ID_VIEW_TEXTSIZE_SMALLEST,	"MENUB_VIEW_TEXTSIZE_SMALLEST"}
};
const int nViewTextSize = sizeof(siiViewTextSize) / sizeof(siiViewTextSize[0]);



//===  Time  ===
submenu_item_info siiTime[] = {
	{ITEM_COMMAND, ID_TIME_DISP, "MENUB_TIME_DISP"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_COMMAND, ID_TIME_10SEC,			"MENUB_TIME_10SEC"},
	{ITEM_COMMAND, ID_TIME_1MIN,			"MENUB_TIME_1MIN"},
	{ITEM_COMMAND, ID_TIME_10MIN,			"MENUB_TIME_10MIN"},
	{ITEM_COMMAND, ID_TIME_1HOUR,			"MENUB_TIME_1HOUR"},
	{ITEM_COMMAND, ID_TIME_4HOUR,			"MENUB_TIME_4HOURS"},
	{ITEM_COMMAND, ID_TIME_1DAY,			"MENUB_TIME_1DAY"},
	{ITEM_COMMAND, ID_TIME_1WEEK,			"MENUB_TIME_1WEEK"},
	{ITEM_COMMAND, ID_TIME_1MONTH,			"MENUB_TIME_1MONTH"},
	{ITEM_COMMAND, ID_TIME_1YEAR,			"MENUB_TIME_1YEAR"},
	{ITEM_COMMAND, ID_TIME_10YEAR,			"MENUB_TIME_10YEARS"},
	{ITEM_COMMAND, ID_TIME_100YEAR,			"MENUB_TIME_100YEARS"},
	{ITEM_COMMAND, ID_TIME_SETTING,			"MENUB_TIME_SETTING"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_COMMAND, ID_TIME_REALTIME_MODE,	"MENUB_TIME_REALTIME_MODE"}
};
const int nTime = sizeof(siiTime) / sizeof(siiTime[0]);



//===  Settings  ===
submenu_item_info siiSetting[] = {
	{ITEM_SUBMENU, SUBMENU_SETTING_DISP_MODE, "MENUB_SETTING_DISP_MODE"}
};
const int nSetting = sizeof(siiSetting) / sizeof(siiSetting[0]);

//--  Setting: Display Mode
submenu_item_info siiSettingDispMode[] = {
	{ITEM_COMMAND, ID_SETTING_DISPMODE_NORMAL,			"MENUB_SETTING_DISPMODE_NORMAL"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_COMMAND, ID_SETTING_DISPMODE_ANAGLYPH,		"MENUB_SETTING_DISPMODE_ANAGLYPH"},
	{ITEM_COMMAND, ID_SETTING_DISPMODE_COLOR_ANAGLYPH,	"MENUB_SETTING_DISPMODE_COLOR_ANAGLYPH"},
	{ITEM_COMMAND, ID_SETTING_DISPMODE_SIDE_BY_SIDE,	"MENUB_SETTING_DISPMODE_SIDE_BY_SIDE"},
	{ITEM_COMMAND, ID_SETTING_DISPMODE_TOP_AND_BOTTOM,	"MENUB_SETTING_DISPMODE_TOP_AND_BOTTOM"},
	{ITEM_SEPARATOR, 0, ""},
	{ITEM_COMMAND, ID_SETTING_DISPMODE_DOMEMASTER,		"MENUB_SETTING_DISPMODE_DOMEMASTER"}
};
const int nSettingDispMode = sizeof(siiSettingDispMode) / sizeof(siiSettingDispMode[0]);


//===  Helps  ===
submenu_item_info siiHelp[] = {
	{ITEM_COMMAND, ID_HELP_ABOUT, "MENUB_HELP_ABOUT"},
};
const int nHelp = sizeof(siiHelp) / sizeof(siiHelp[0]);





struct submenu_info {
	const char* key;
	const submenu_item_info *sii;
	int N;
};

submenu_info vSubMenu[SUBMENU_NUM] = {
	{"MENUB_FILE",						siiFile,					nFile},
	{"MENUB_LANDING",					siiLanding,					nLanding},
	{"MENUB_LANDING_MODE",				siiLandingMode,				nLandingMode},

	{"MENUB_TARGET",					siiTarget,					nTarget},
	{"MENUB_TARGET_SUN_PLANETS",		siiTargetSunPlanet,			nTargetSunPlanet},
	{"MENUB_TARGET_SATELLITES",			siiTargetSatellites,		nTargetSatellites},
	{"MENUB_TARGET_MINOR_PLANETS",		siiTargetMinorPlanets,		nTargetMinorPlanets},
	{"MENUB_TARGET_TNOS",				siiTargetTNOs,				nTargetTNOs},
	{"MENUB_TARGET_STARS",				siiTargetStars,				nTargetStars},
	{"MENUB_TARGET_GALACTIC_OBJECTS",	siiTargetGalacticObjects,	nTargetGalacticObjects},
	{"MENUB_TARGET_EXTRAGALACTIC_OBJECTS", siiTargetExtragalacticObjects, nTargetExtragalacticObjects},
	{"MENUB_TARGET_SPACECRAFT",			siiTargetSpacecraft,		nTargetSpacecraft},
	{"MENUB_PLANETARIUM_TARGET",		siiPlanetariumTargets,		nPlanetariumTargets },

	{"MENUB_PRESET",					siiPreset,					nPreset},

	{"MENUB_SCALE",						siiScale,					nScale},

	{"MENUB_VIEW",						siiView,					nView},
	{"MENUB_VIEW_PLANET",				siiViewPlanets,				nViewPlanets},
	{"MENUB_VIEW_PLANET_ATMOSPHERE",	siiViewPlanetAtmosphere,	nViewPlanetAtmosphere},
	{"MENUB_VIEW_PLANET_ECLIPSE",		siiViewPlanetEclipse,		nViewPlanetEclipse},
	{"MENUB_VIEW_PLANET_MAG_RATE",		siiViewPlanetMagRate,		nViewPlanetMagRate},
	{"MENUB_VIEW_PLANET_AMP_TOPO",		siiViewPlanetAmpTopo,		nViewPlanetAmpTopo},
	{"MENUB_VIEW_SATELLITE",			siiViewSatellites,			nViewSatellites},
	{"MENUB_VIEW_MINOR_PLANET",			siiViewMinorPlanets,		nViewMinorPlanets},
	{"MENUB_VIEW_TNO",					siiViewTNOs,				nViewTNOs},

	{"MENUB_VIEW_STAR",					siiViewStars,				nViewStars},
	{"MENUB_VIEW_STAR_MARK",			siiViewStarMark,			nViewStarMark},
	{"MENUB_VIEW_GALACTIC_OBJECT",		siiViewGalacticObjects,		nViewGalacticObjects},
	{"MENUB_VIEW_EXTRAGALACTIC_OBJECT",	siiViewExtragalacticObjects, nViewExtragcticObjects},
	{"MENUB_VIEW_SPACECRAFT",			siiViewSpacecraft,			nViewSpacecraft},
	{"MENUB_VIEW_SPACECRAFT_TRAJECTORY",siiViewSpacecraftTrajectory, nViewSpacecraftTrajectory},
	{"MENUB_VIEW_CONSTELLATION",		siiViewConstellations,		nViewConstellations},
	{"MENUB_VIEW_MILKYWAY",				siiViewMilkyWay,			nViewMilkyWay},
	{"MENUB_VIEW_GRID",					siiViewGrid,				nViewGrid},
	{"MENUB_VIEW_SCALELINE",			siiViewScaleLines,			nViewScaleLines},
	{"MENUB_VIEW_TEXTSIZE",				siiViewTextSize,			nViewTextSize},

	{"MENUB_TIME",						siiTime,					nTime},

	{"MENUB_SETTING",					siiSetting,					nSetting},
	{"MENUB_SETTING_DISP_MODE",			siiSettingDispMode, nSettingDispMode},

	{"MENUB_HELP",						siiHelp,					nHelp}
};







void
add_menu_item(HMENU _hMenu, int no, UINT ID, const std::string& key)
{
	MENUITEMINFOW mii;
	ZeroMemory(&mii, sizeof(mii));

	std::wstring wstr = get_wstring(key);

	mii.cbSize = sizeof(mii);
	mii.fMask = MIIM_ID | MIIM_STRING;
	mii.wID = ID;
	mii.dwTypeData = (wchar_t*)wstr.c_str();

	InsertMenuItemW(_hMenu, no, TRUE, &mii);
}

void
add_separator(HMENU _hMenu, int no)
{
	MENUITEMINFOW mii;
	ZeroMemory(&mii, sizeof(mii));

	mii.cbSize = sizeof(mii);
	mii.fType = MFT_SEPARATOR;
	mii.fMask = MIIM_FTYPE;
	InsertMenuItemW(_hMenu, no, TRUE, &mii);
}

void
add_sub_menu(HMENU _hMenu, int no, int submenu_ID)
{
	HMENU hSubMenu;
	hSubMenu = CreateMenu();

	const submenu_info& sif = vSubMenu[submenu_ID];
	const submenu_item_info* sii = sif.sii;
	const int N = sif.N;

	for (int i = 0; i < N; i++) {
		const submenu_item_info& si = sii[i];

		switch (si.type) {
		case ITEM_COMMAND:
			add_menu_item(hSubMenu, i, si.ID, si.key);
			break;
		case ITEM_SEPARATOR:
			add_separator(hSubMenu, i);
			break;
		case ITEM_SUBMENU:
			add_sub_menu(hSubMenu, i, si.ID);
		default:
			break;
		}
	}


	MENUITEMINFOW mii;
	ZeroMemory(&mii, sizeof(mii));

	std::wstring wstr = get_wstring(sif.key);

	mii.cbSize = sizeof(mii);
	mii.fMask = MIIM_STRING | MIIM_SUBMENU;
	mii.dwTypeData = (wchar_t*)wstr.c_str();
	mii.hSubMenu = hSubMenu;
	InsertMenuItemW(_hMenu, no, TRUE, &mii);	//
}


void
add_language_sub_menu(HMENU _hMenu, int no, const std::string& key)
{
	HMENU hSubMenu;
	hSubMenu = CreateMenu();


	int lang_num = get_num_languages();
	for (int i = 0; i<lang_num; i++) {
		add_menu_item(hSubMenu, i, ID_LANG_0 + i, get_language_name_key(i));
	}


	MENUITEMINFOW mii;
	ZeroMemory(&mii, sizeof(mii));

	std::wstring wstr = get_wstring(key);
	mii.cbSize = sizeof(mii);
	mii.fMask = MIIM_STRING | MIIM_SUBMENU | MIIM_FTYPE;
	mii.fType = MFT_RIGHTJUSTIFY;
	mii.dwTypeData = (wchar_t*)wstr.c_str();
	mii.hSubMenu = hSubMenu;
	InsertMenuItemW(_hMenu, no, TRUE, &mii);	//
}




void
create_menu_bar()
{
	//===  Menu Bar  ===
	hMenu = CreateMenu();
	// File
	add_sub_menu(hMenu, 0, SUBMENU_FILE);
	//  Landing/Takeoff
	add_sub_menu(hMenu, 1, SUBMENU_LANDING);
	//  Target
	add_sub_menu(hMenu, 2, SUBMENU_TARGET);
	//  Preset
	add_sub_menu(hMenu, 3, SUBMENU_PRESET);
	//  Scale
	add_sub_menu(hMenu, 4, SUBMENU_SCALE);
	//  View
	add_sub_menu(hMenu, 5, SUBMENU_VIEW);
	//  Time
	add_sub_menu(hMenu, 6, SUBMENU_TIME);
	//  Settings
	add_sub_menu(hMenu, 7, SUBMENU_SETTING);
	//  Help
	add_sub_menu(hMenu, 8, SUBMENU_HELP);

	//  Language
	add_language_sub_menu(hMenu, 9, "MENUB_LANGUAGE");



	//===  Time Popup  ===
	hMenuTime = CreateMenu();
	add_sub_menu(hMenuTime, 0, SUBMENU_TIME);
	hMenuTime = GetSubMenu(hMenuTime, 0);


	//===  Scale Popup  ===
	hMenuScale = CreateMenu();
	add_sub_menu(hMenuScale, 0, SUBMENU_SCALE);
	hMenuScale = GetSubMenu(hMenuScale, 0);
}


void
set_menu_bar(HWND hWnd)
{
	if ( !is_fullscreen_mode() ) {
		SetMenu(hWnd, hMenu);
	}
}




void
destroy_menu_bar()
{
	if (hMenu)		DestroyMenu(hMenu);
	if (hMenuTime)	DestroyMenu(hMenuTime);
	if (hMenuScale)	DestroyMenu(hMenuScale);
	hMenu = hMenuTime = hMenuScale = NULL;
}


void
EnableMenuBar(HWND hWnd, bool flg)
{
	if (flg) {
		SetMenu(hWnd, hMenu);
	} else {
		SetMenu(hWnd, NULL);
	}
}




void
OnTargetByKey2(const char* key)
{
	if (get_mode() != MODE_3D) return;

	OnTargetByKey(key);
}



bool
process_menu_command(HWND hWnd, int id)
{
	//--  Languages
	if (id >= ID_LANG_0 && id <= ID_LANG_0 + get_num_languages() -1) {
		OnChangeLanguage(id - ID_LANG_0);
		return  true;
	}


	switch (id) {

		//===  Menu Bar  ===

		//-- FILE
		case ID_FILE_SAVEIMAGE:			OnSaveImage();						return true;
		case ID_FILE_SAVEIMAGE_HIGHRES:	OnSaveImageHighRes();				return true;
		case ID_FILE_EXIT:				SendMessage(hWnd, WM_CLOSE, 0, 0);	return true;


		//-- LANDING
		case ID_LANDING_LANDING:
		{
			switch (get_mode()) {
			case MODE_PLANETARIUM:	OnTakeoff();	break;
			case MODE_3D:			OnLanding();	break;
			default:								break;
			}
			return true;
		}
		case ID_LANDING_MITAKA:	OnLandingInit();										return true;
		case ID_LANDING_MODE_HORIZONTAL:	OnLandingMode(LANDING_MODE_HORIZONTAL);		return true;
		case ID_LANDING_MODE_SURFACE_VIEW:	OnLandingMode(LANDING_MODE_SURFACE_VIEW);	return true;

			
		//-- TARGET
		// Sun and Planets
		case ID_TARGET_SUN:					OnTargetByKey2("SUN");				return true;
		case ID_TARGET_MERCURY:				OnTargetByKey2("PLNT_MERCURY");		return true;
		case ID_TARGET_VENUS:				OnTargetByKey2("PLNT_VENUS");		return true;
		case ID_TARGET_EARTH:				OnTargetByKey2("PLNT_EARTH");		return true;
		case ID_TARGET_MARS:				OnTargetByKey2("PLNT_MARS");		return true;
		case ID_TARGET_JUPITER:				OnTargetByKey2("PLNT_JUPITER");		return true;
		case ID_TARGET_SATURN:				OnTargetByKey2("PLNT_SATURN");		return true;
		case ID_TARGET_URANUS:				OnTargetByKey2("PLNT_URANUS");		return true;
		case ID_TARGET_NEPTUNE:				OnTargetByKey2("PLNT_NEPTUNE");		return true;
		// Satellites
		case ID_TARGET_MOON:				OnTargetByKey2("SAT_E_MOON");		return true;
		case ID_TARGET_PHOBOS:				OnTargetByKey2("SAT_M_PHOBOS");		return true;
		case ID_TARGET_DEIMOS:				OnTargetByKey2("SAT_M_DEIMOS");		return true;
		case ID_TARGET_IO:					OnTargetByKey2("SAT_J_IO");			return true;
		case ID_TARGET_EUROPA:				OnTargetByKey2("SAT_J_EUROPA");		return true;
		case ID_TARGET_GANYMEDE:			OnTargetByKey2("SAT_J_GANYMEDE");	return true;
		case ID_TARGET_CALLISTO:			OnTargetByKey2("SAT_J_CALLISTO");	return true;
		case ID_TARGET_MIMAS:				OnTargetByKey2("SAT_S_MIMAS");		return true;
		case ID_TARGET_ENCELADUS:			OnTargetByKey2("SAT_S_ENCELADUS");	return true;
		case ID_TARGET_TITAN:				OnTargetByKey2("SAT_S_TITAN");		return true;
		case ID_TARGET_IAPETUS:				OnTargetByKey2("SAT_S_IAPETUS");	return true;
		case ID_TARGET_TRITON:				OnTargetByKey2("SAT_N_TRITON");		return true;
		case ID_TARGET_CHARON:				OnTargetByKey2("SAT_P_CHARON");		return true;
		// Minor Planets
		case ID_TARGET_CERES:				OnTargetByKey2("MP_1");				return true;
		case ID_TARGET_ITOKAWA:				OnTargetByKey2("MP_25143");			return true;
		case ID_TARGET_RYUGU:				OnTargetByKey2("MP_162173");		return true;
		case ID_TARGET_CHARIKLO:			OnTargetByKey2("MP_10199");			return true;
		// Transneptunian Objects
		case ID_TARGET_PLUTO:				OnTargetByKey2("PLNT_PLUTO");		return true;
		case ID_TARGET_ERIS:				OnTargetByKey2("MP_136199");		return true;
		case ID_TARGET_HAUMEA:				OnTargetByKey2("MP_136108");		return true;
		case ID_TARGET_MAKEMAKE:			OnTargetByKey2("MP_136472");		return true;
		case ID_TARGET_SEDNA:				OnTargetByKey2("MP_90377");			return true;
		// Stars
		case ID_TARGET_ALP_CEN:				OnTargetByKey2("HIP_71683");		return true;
		case ID_TARGET_SIRIUS:				OnTargetByKey2("HIP_32349");		return true;
		case ID_TARGET_PLEIADES:			OnTargetByKey2("HIP_17702");		return true;
		case ID_TARGET_CASTOR:				OnTargetByKey2("HIP_36850");		return true;
		// Galactic Objects
		case ID_TARGET_SGR_A_STAR:			OnTargetByKey2("SGR_A_STAR");		return true;
		case ID_TARGET_M13:					OnTargetByKey2("GC_NGC_6205");		return true;
		case ID_TARGET_OUR_GALAXY_CENTER:	OnTargetByKey2("OUR_GALAXY");	return true;
		// Extragalactic Objects
		case ID_TARGET_M31:					OnTargetByKey2("NBG_NGC_224");	return true;
		case ID_TARGET_VIRGO_CLUSTER:		OnTargetByKey2("NBG_NGC_4486");	return true;
		// Spacecraft
		case ID_TARGET_PIONEER10:			OnTargetByKey2("SCF_PIONEER_10");	return true;
		case ID_TARGET_PIONEER11:			OnTargetByKey2("SCF_PIONEER_11");	return true;
		case ID_TARGET_VOYAGER1:			OnTargetByKey2("SCF_VOYAGER_I");	return true;
		case ID_TARGET_VOYAGER2:			OnTargetByKey2("SCF_VOYAGER_II");	return true;
		case ID_TARGET_CASSINI:				OnTargetByKey2("SCF_CASSINI");		return true;
		case ID_TARGET_NEW_HORIZONS:		OnTargetByKey2("SCF_NEW_HORIZONS");	return true;
		case ID_TARGET_GALILEO:				OnTargetByKey2("SCF_GALILEO");		return true;
		case ID_TARGET_HAYABUSA2:			OnTargetByKey2("SCF_HAYABUSA_2");	return true;
		case ID_TARGET_AKATSUKI:			OnTargetByKey2("SCF_AKATSUKI");		return true;
		// Other Target Commands
		case ID_TARGET_GO_TO_TARGET:		OnTargetMoveControler();			return true;


		// Planetarium Targets
		case ID_PLANETARIUM_TARGET_NONE:	OnPlanetariumTarget(TARGET_PLNT_NONE);		return true;
		case ID_PLANETARIUM_TARGET_SUN:		OnPlanetariumTarget(TARGET_PLNT_SUN);		return true;
		case ID_PLANETARIUM_TARGET_MOON:	OnPlanetariumTarget(TARGET_PLNT_MOON);		return true;
		case ID_PLANETARIUM_TARGET_JUPITER:	OnPlanetariumTarget(TARGET_PLNT_JUPITER);	return true;


		//-- Preset
		case ID_PRESET_NEW_HORIZONS:	OnPreset(PRESET_NEW_HORIZONS);				return true;
		case ID_PRESET_HAYABUSA_2:		OnPreset(PRESET_HAYABUSA2);					return true;
		case ID_PRESET_AKATSUKI:		OnPreset(PRESET_AKATSUKI);					return true;
		case ID_PRESET_BLACKHOLE_GC:	OnPreset(PRESET_BLACKHOLE_GC);				return true;
		case ID_PRESET_ECLIPSE_2009:	OnPreset(PRESET_ECLIPSE_2009_7_22);			return true;
		case ID_PRESET_ECLIPSE_2016:	OnPreset(PRESET_ECLIPSE_2016_3_9_TOKYO);	return true;
		case ID_PRESET_ECLIPSE_2016_2:	OnPreset(PRESET_ECLIPSE_2016_3_9_TERNATE);	return true;

			
		//-- Scale
		case ID_SCALE_0:				OnPresetScale(SCALE_10MKM);		return true;
		case ID_SCALE_1:				OnPresetScale(SCALE_1AU);		return true;
		case ID_SCALE_2:				OnPresetScale(SCALE_10AU);		return true;
		case ID_SCALE_3:				OnPresetScale(SCALE_100AU);		return true;
		case ID_SCALE_4:				OnPresetScale(SCALE_1000AU);	return true;
		case ID_SCALE_5:				OnPresetScale(SCALE_10000AU);	return true;
		case ID_SCALE_6:				OnPresetScale(SCALE_1LY);		return true;
		case ID_SCALE_7:				OnPresetScale(SCALE_10LY);		return true;
		case ID_SCALE_8:				OnPresetScale(SCALE_100LY);		return true;
		case ID_SCALE_9:				OnPresetScale(SCALE_1KLY);		return true;
		case ID_SCALE_A:				OnPresetScale(SCALE_10KLY);		return true;
		case ID_SCALE_B:				OnPresetScale(SCALE_100KLY);	return true;
		case ID_SCALE_C:				OnPresetScale(SCALE_1MLY);		return true;
		case ID_SCALE_D:				OnPresetScale(SCALE_10MLY);		return true;
		case ID_SCALE_E:				OnPresetScale(SCALE_100MLY);	return true;
		case ID_SCALE_F:				OnPresetScale(SCALE_1GLY);		return true;
		case ID_SCALE_G:				OnPresetScale(SCALE_10GLY);		return true;


		//-- VIEW
		// Planets
		case ID_VIEW_PLANET_DISP:					OnDispSwitchToggle(DISPSW_PLANET, DISPM_DISP);			return true;
		case ID_VIEW_PLANET_NAME:					OnDispSwitchToggle(DISPSW_PLANET, DISPM_NAME);			return true;
		case ID_VIEW_PLANET_ORBIT:					OnDispSwitchToggle(DISPSW_PLANET, DISPM_ORBIT);			return true;
		case ID_VIEW_PLANET_AXIS:					OnDispSwitchToggle(DISPSW_PLANET, DISPM_AXIS);			return true;
		case ID_VIEW_PLANET_ATMOSPHERE_NONE:		OnPlanetAtmosphere(ATMOSPHERE_NONE);					return true;
		case ID_VIEW_PLANET_ATMOSPHERE_SINGLE:		OnPlanetAtmosphere(ATMOSPHERE_SINGLE);					return true;
		case ID_VIEW_PLANET_ATMOSPHERE_MULTIPLE:	OnPlanetAtmosphere(ATMOSPHERE_MULTIPLE);				return true;
		case ID_VIEW_PLANET_CLOUD:					OnDispSwitchToggle(DISPSW_PLANET, DISPM_CLOUD);			return true;
		case ID_VIEW_PLANET_CITY_LIGHT:				OnDispSwitchToggle(DISPSW_PLANET, DISPM_CITY_LIGHTS);	return true;
		case ID_VIEW_PLANET_ECLIPSE_DISP:			OnEclipse(swEclipse ^ ECLIPSE_DISP);					return true;
		case ID_VIEW_PLANET_ECLIPSE_BORDER:			OnEclipse(swEclipse ^ ECLIPSE_BORDER);					return true;
		case ID_VIEW_PLANET_MAG_RATE_0:				OnZoomMode(0);											return true;
		case ID_VIEW_PLANET_MAG_RATE_1:				OnZoomMode(1);											return true;
		case ID_VIEW_PLANET_MAG_RATE_2:				OnZoomMode(2);											return true;
		case ID_VIEW_PLANET_MAG_RATE_3:				OnZoomMode(3);											return true;
		case ID_VIEW_PLANET_MAG_RATE_4:				OnZoomMode(4);											return true;
		case ID_VIEW_PLANET_AMP_TOPO_0:				OnTopoZoomMode(0);										return true;
		case ID_VIEW_PLANET_AMP_TOPO_1:				OnTopoZoomMode(1);										return true;
		case ID_VIEW_PLANET_AMP_TOPO_2:				OnTopoZoomMode(2);										return true;
		case ID_VIEW_PLANET_AMP_TOPO_3:				OnTopoZoomMode(3);										return true;
		case ID_VIEW_PLANET_AMP_TOPO_4:				OnTopoZoomMode(4);										return true;
		// Satellites
		case ID_VIEW_SATELLITE_DISP:				OnDispSwitchToggle(DISPSW_SATELLITE, DISPM_DISP);		return true;
		case ID_VIEW_SATELLITE_NAME:				OnDispSwitchToggle(DISPSW_SATELLITE, DISPM_NAME);		return true;
		case ID_VIEW_SATELLITE_ORBIT:				OnDispSwitchToggle(DISPSW_SATELLITE, DISPM_ORBIT);		return true;
		case ID_VIEW_SATELLITE_SELECT:				OnDispSwitchToggle(DISPSW_SATELLITE, DISPM_SELECT);		return true;
		// Minor Planets
		case ID_VIEW_MINOR_PLANET_DISP:				OnDispSwitchToggle(DISPSW_MINOR_PLANET, DISPM_DISP);	return true;
		case ID_VIEW_MINOR_PLANET_NAME:				OnDispSwitchToggle(DISPSW_MINOR_PLANET, DISPM_NAME);	return true;
		case ID_VIEW_MINOR_PLANET_ORBIT:			OnDispSwitchToggle(DISPSW_MINOR_PLANET, DISPM_ORBIT);	return true;
		case ID_VIEW_MINOR_PLANET_SELECT:			OnDispSwitchToggle(DISPSW_MINOR_PLANET, DISPM_SELECT);	return true;
		// TNOs
		case ID_VIEW_TNO_DISP:						OnDispSwitchToggle(DISPSW_TNO, DISPM_DISP);				return true;
		case ID_VIEW_TNO_NAME:						OnDispSwitchToggle(DISPSW_TNO, DISPM_NAME);				return true;
		case ID_VIEW_TNO_ORBIT:						OnDispSwitchToggle(DISPSW_TNO, DISPM_ORBIT);			return true;
		case ID_VIEW_TNO_SELECT:					OnDispSwitchToggle(DISPSW_TNO, DISPM_SELECT);			return true;
		case ID_VIEW_OORT_CLOUD_DISP:				OnOortCloud(!bOortCloud);								return true;
		// Stars
		case ID_VIEW_STAR_DISP:				OnStar(swStar ^ STAR_DISP);				return true;
		case ID_VIEW_STAR_NAME:				OnStar(swStar ^ STAR_NAME);				return true;
		case ID_VIEW_STAR_NAME_SELECT:		OnStar(swStar ^ STAR_NAME_SELECT);		return true;
		case ID_VIEW_STAR_BAYER:			OnStar(swStar ^ STAR_BAYER);			return true;
		case ID_VIEW_STAR_FRAMSTEED:		OnStar(swStar ^ STAR_FRAMSTEED);		return true;
		case ID_VIEW_STAR_HIP:				OnStar(swStar ^ STAR_HIP);				return true;
		case ID_VIEW_STAR_ORBIT:			OnStar(swStar ^ STAR_ORBIT);			return true;
		case ID_VIEW_STAR_MARK_NONE:		OnStarMark(0);							return true;
		case ID_VIEW_STAR_MARK_BINARY:		OnStarMark(1);							return true;
		case ID_VIEW_STAR_MARK_EXOPLANET:	OnStarMark(2);							return true;
		case ID_VIEW_STAR_BRIGHTEN:			OnLRef(pow(10, -0.4*0.5));				return true;
		case ID_VIEW_STAR_DARKEN:			OnLRef(pow(10, +0.4*0.5));				return true;
		case ID_VIEW_STAR_STANDARD:			OnSetLRef(1.0f);						return true;
		// Galactic Objects
		case ID_VIEW_OUR_GALAXY_DISP:			OnGalaxy(swGalaxy ^ GLXY_OUR_GALAXY);		return true;
		case ID_VIEW_SGR_A_STAR_DISP:			OnGalaxy(swGalaxy ^ GLXY_SGR_A_ST);			return true;
		case ID_VIEW_VERA_OBJECT_DISP:			OnGalaxy(swGalaxy ^ GLXY_VERA);				return true;
		case ID_VIEW_GLOBULAR_CLUSTER_DISP:		OnGalaxy(swGalaxy ^ GLXY_GLOB_CLSTR);		return true;
		case ID_VIEW_GALACTIC_CENTER_BG_DISP:	OnGalCenterBG(!bGalCenterBG);				return true;
		// Extragalactic Objects
		case ID_VIEW_NEARBY_GALAXY_DISP:		OnGalaxy(swGalaxy ^ GLXY_NEARBY_GALAXY);	return true;
		case ID_VIEW_SDSS_GALAXY_DISP:			OnGalaxy(swGalaxy ^ GLXY_DISTANT_GALAXY);	return true;
		case ID_VIEW_QSO_DISP:					OnGalaxy(swGalaxy ^ GLXY_QSO);				return true;
		case ID_VIEW_CMB_DISP:					OnCMB(!bCMB);								return true;
		case ID_VIEW_MARK_VIRGO_CLUSTER:		OnMarkVirgoCluster(!bVirgoCluster);			return true;
		// Spacecraft
		case ID_VIEW_SPACECRAFT_DISP:						OnSpacecraft(!bSpacecraft);									return true;
		case ID_VIEW_SPACECRAFT_TRAJECTORY_PIONEER10:		OnSpacecraftPath(0, !get_spacecraft_trajectory_mode(0));	return true;
		case ID_VIEW_SPACECRAFT_TRAJECTORY_PIONEER11:		OnSpacecraftPath(1, !get_spacecraft_trajectory_mode(1));	return true;
		case ID_VIEW_SPACECRAFT_TRAJECTORY_VOYAGER1:		OnSpacecraftPath(2, !get_spacecraft_trajectory_mode(2));	return true;
		case ID_VIEW_SPACECRAFT_TRAJECTORY_VOYAGER2:		OnSpacecraftPath(3, !get_spacecraft_trajectory_mode(3));	return true;
		case ID_VIEW_SPACECRAFT_TRAJECTORY_CASSINI:			OnSpacecraftPath(4, !get_spacecraft_trajectory_mode(4));	return true;
		case ID_VIEW_SPACECRAFT_TRAJECTORY_NEW_HORIZONS:	OnSpacecraftPath(5, !get_spacecraft_trajectory_mode(5));	return true;
		case ID_VIEW_SPACECRAFT_TRAJECTORY_GALILEO:			OnSpacecraftPath(6, !get_spacecraft_trajectory_mode(6));	return true;
		case ID_VIEW_SPACECRAFT_TRAJECTORY_HAYABUSA2:		OnSpacecraftPath(7, !get_spacecraft_trajectory_mode(7));	return true;
		case ID_VIEW_SPACECRAFT_TRAJECTORY_AKATSUKI:		OnSpacecraftPath(8, !get_spacecraft_trajectory_mode(8));	return true;
		// Constellation
		case ID_VIEW_CNST_NAME:		OnConstellation(swConstellation ^ CNST_NAME);		return true;
		case ID_VIEW_CNST_LINE:		OnConstellation(swConstellation ^ CNST_LINE);		return true;
		case ID_VIEW_CNST_ILLUST:	OnConstellation(swConstellation ^ CNST_ILLUST);		return true;
		case ID_VIEW_CNST_BOUNDARY:	OnConstellation(swConstellation ^ CNST_BND);		return true;
		// Milkyway
		case ID_VIEW_CLST_NONE:				OnMilkyWay(0);		return true;
		case ID_VIEW_CLST_MILKYWAY:			OnMilkyWay(1);		return true;
		case ID_VIEW_CLST_MILKYWAY_BRIGHT:	OnMilkyWay(2);		return true;
		case ID_VIEW_CLST_MILKYWAY_IR:		OnMilkyWay(3);		return true;
		case ID_VIEW_CLST_CMB:				OnMilkyWay(4);		return true;
		// Grid
		case ID_VIEW_GRID_EQ:				OnCelestialGrid(!bCelestialGrid);			return true;
		case ID_VIEW_GRID_EC:				OnEcliptic(!bEcliptic);						return true;
		case ID_VIEW_GRID_GAL:				OnGalacticGrid(!bGalacticGrid);				return true;
		case ID_VIEW_GRID_HORIZON:			OnHorizonCoord(!bHorizonCoord);				return true;
		// Scale Lines
		case ID_VIEW_SCALELINE_CIRCLE:		OnDistScale(swScaleLine ^ SCALE_CIRCLE);	return true;
		case ID_VIEW_SCALELINE_SQUARE:		OnDistScale(swScaleLine ^ SCALE_SQUARE);	return true;
		// Text Size 
		case ID_VIEW_TEXTSIZE_LARGEST:		OnFontSize(0);								return true;
		case ID_VIEW_TEXTSIZE_LARGER:		OnFontSize(1);								return true;
		case ID_VIEW_TEXTSIZE_MEDIUM:		OnFontSize(2);								return true;
		case ID_VIEW_TEXTSIZE_SMALLER:		OnFontSize(3);								return true;
		case ID_VIEW_TEXTSIZE_SMALLEST:		OnFontSize(4);								return true;
		// Other View Commands 
		case ID_VIEW_HEADLIGHT:				OnSideLight(!bSideLight);					return true;
		case ID_VIEW_PLANET_POINTER:		OnSurfacePointer(!bSurfacePointer);			return true;
		case ID_VIEW_OBJECT_INFO:			OnObjectInfo(!is_object_info_enable());						return true;
		case ID_VIEW_VIEW_INFO:				OnViewInfo(!bViewInfo);						return true;
		case ID_VIEW_RESET_VIEW:			OnResetViewangle();							return true;
		case ID_VIEW_FULLSCREEN:			OnFullScreen(true);							return true;

	

		//-- Time
		case ID_TIME_DISP:				OnDate(!bDate);						return true;
		case ID_TIME_10SEC:				OnTimeStep(TIMESTEP_10SEC);			return true;
		case ID_TIME_1MIN:				OnTimeStep(TIMESTEP_MIN);			return true;
		case ID_TIME_10MIN:				OnTimeStep(TIMESTEP_10MIN);			return true;
		case ID_TIME_1HOUR:				OnTimeStep(TIMESTEP_HOUR);			return true;
		case ID_TIME_4HOUR:				OnTimeStep(TIMESTEP_4HOUR);			return true;
		case ID_TIME_1DAY:				OnTimeStep(TIMESTEP_DAY);			return true;
		case ID_TIME_1WEEK:				OnTimeStep(TIMESTEP_WEEK);			return true;
		case ID_TIME_1MONTH:			OnTimeStep(TIMESTEP_MONTH);			return true;
		case ID_TIME_1YEAR:				OnTimeStep(TIMESTEP_YEAR);			return true;
		case ID_TIME_10YEAR:			OnTimeStep(TIMESTEP_10YEAR);		return true;
		case ID_TIME_100YEAR:			OnTimeStep(TIMESTEP_100YEAR);		return true;
		case ID_TIME_SETTING:			OnTimeSetting();					return true;
		case ID_TIME_REALTIME_MODE:		OnRealTimeMode(!GetRealtimeMode());	return true;


			
		//-- Settings
		case ID_SETTING_DISPMODE_NORMAL:			OnDispMode(DISPMODE_NORMAL);			return true;
		case ID_SETTING_DISPMODE_ANAGLYPH:			OnDispMode(DISPMODE_ANAGLYPH);			return true;
		case ID_SETTING_DISPMODE_COLOR_ANAGLYPH:	OnDispMode(DISPMODE_COLOR_ANAGLYPH);	return true;
		case ID_SETTING_DISPMODE_SIDE_BY_SIDE:		OnDispMode(DISPMODE_SIDE_BY_SIDE);		return true;
		case ID_SETTING_DISPMODE_TOP_AND_BOTTOM:	OnDispMode(DISPMODE_TOP_AND_BOTTOM);	return true;
		case ID_SETTING_DISPMODE_DOMEMASTER:		OnDispMode(DISPMODE_DOME_MASTER);		return true;

		//-- HELP
		case ID_HELP_ABOUT:							OnAbout();							return true;


		default:											break;
	}
	return false;
}


void
process_menu_checks(WPARAM wParam)
{
	HMENU hmnu = (HMENU)wParam;

	//-- File
	EnableMenuItem( hmnu, ID_FILE_SAVEIMAGE_HIGHRES,	(isPBufferEnable())    ?  MF_ENABLED : MF_GRAYED );

	//-- Landing
	const char ldmd = get_landing_mode();
	CheckMenuItem( hmnu, ID_LANDING_MODE_HORIZONTAL,	(ldmd==LANDING_MODE_HORIZONTAL)    ?  MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_LANDING_MODE_SURFACE_VIEW,	(ldmd==LANDING_MODE_SURFACE_VIEW)  ?  MF_CHECKED : MF_UNCHECKED );

	//-- View
	// Planets
	CheckMenuItem( hmnu, ID_VIEW_PLANET_DISP,		test_disp_switch(DISPSW_PLANET, DISPM_DISP)			? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_PLANET_NAME,		test_disp_switch(DISPSW_PLANET, DISPM_NAME)			? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_PLANET_ORBIT,		test_disp_switch(DISPSW_PLANET, DISPM_ORBIT)		? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_PLANET_AXIS,		test_disp_switch(DISPSW_PLANET, DISPM_AXIS)			? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_PLANET_CLOUD,		test_disp_switch(DISPSW_PLANET, DISPM_CLOUD)		? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_PLANET_CITY_LIGHT,	test_disp_switch(DISPSW_PLANET, DISPM_CITY_LIGHTS)	? MF_CHECKED : MF_UNCHECKED );

	const char atmd = AtmosphereMode;
	CheckMenuItem( hmnu, ID_VIEW_PLANET_ATMOSPHERE_NONE,		(atmd==ATMOSPHERE_NONE)		? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_PLANET_ATMOSPHERE_SINGLE,		(atmd==ATMOSPHERE_SINGLE)	? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_PLANET_ATMOSPHERE_MULTIPLE,	(atmd==ATMOSPHERE_MULTIPLE)	? MF_CHECKED : MF_UNCHECKED );

	CheckMenuItem(hmnu, ID_VIEW_PLANET_ECLIPSE_DISP,	swEclipse & ECLIPSE_DISP ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_PLANET_ECLIPSE_BORDER,	swEclipse & ECLIPSE_BORDER ? MF_CHECKED : MF_UNCHECKED);

	const char zmmd = get_zoom_mode();
	CheckMenuItem( hmnu, ID_VIEW_PLANET_MAG_RATE_0,		(zmmd==0)	? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_PLANET_MAG_RATE_1,		(zmmd==1)	? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_PLANET_MAG_RATE_2,		(zmmd==2)	? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_PLANET_MAG_RATE_3,		(zmmd==3)	? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_PLANET_MAG_RATE_4,		(zmmd==4)	? MF_CHECKED : MF_UNCHECKED );

	const char tzmd = get_topo_zoom_mode();
	CheckMenuItem( hmnu, ID_VIEW_PLANET_AMP_TOPO_0,		(tzmd==0)	? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_PLANET_AMP_TOPO_1,		(tzmd==1)	? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_PLANET_AMP_TOPO_2,		(tzmd==2)	? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_PLANET_AMP_TOPO_3,		(tzmd==3)	? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_PLANET_AMP_TOPO_4,		(tzmd==4)	? MF_CHECKED : MF_UNCHECKED );

	// Satellites
	CheckMenuItem(hmnu, ID_VIEW_SATELLITE_DISP, test_disp_switch(DISPSW_SATELLITE, DISPM_DISP) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_SATELLITE_NAME, test_disp_switch(DISPSW_SATELLITE, DISPM_NAME) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_SATELLITE_ORBIT, test_disp_switch(DISPSW_SATELLITE, DISPM_ORBIT) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_SATELLITE_SELECT, test_disp_switch(DISPSW_SATELLITE, DISPM_SELECT) ? MF_CHECKED : MF_UNCHECKED);

	// Minor Planets
	CheckMenuItem(hmnu, ID_VIEW_MINOR_PLANET_DISP, test_disp_switch(DISPSW_MINOR_PLANET, DISPM_DISP) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_MINOR_PLANET_NAME, test_disp_switch(DISPSW_MINOR_PLANET, DISPM_NAME) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_MINOR_PLANET_ORBIT, test_disp_switch(DISPSW_MINOR_PLANET, DISPM_ORBIT) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_MINOR_PLANET_SELECT, test_disp_switch(DISPSW_MINOR_PLANET, DISPM_SELECT) ? MF_CHECKED : MF_UNCHECKED);

	// TNOs
	CheckMenuItem(hmnu, ID_VIEW_TNO_DISP, test_disp_switch(DISPSW_TNO, DISPM_DISP) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_TNO_NAME, test_disp_switch(DISPSW_TNO, DISPM_NAME) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_TNO_ORBIT, test_disp_switch(DISPSW_TNO, DISPM_ORBIT) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_TNO_SELECT, test_disp_switch(DISPSW_TNO, DISPM_SELECT) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_OORT_CLOUD_DISP, bOortCloud ? MF_CHECKED : MF_UNCHECKED);

	// Stars
	CheckMenuItem( hmnu, ID_VIEW_STAR_DISP,				swStar & STAR_DISP			? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_STAR_NAME,				swStar & STAR_NAME			? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_STAR_NAME_SELECT,		swStar & STAR_NAME_SELECT	? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_STAR_BAYER,			swStar & STAR_BAYER			? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_STAR_FRAMSTEED,		swStar & STAR_FRAMSTEED		? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_STAR_HIP,				swStar & STAR_HIP			? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_STAR_ORBIT,			swStar & STAR_ORBIT			? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_STAR_MARK_NONE,		(tyStarMark==0)				? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_STAR_MARK_BINARY,		(tyStarMark==1)				? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_STAR_MARK_EXOPLANET,	(tyStarMark==2)				? MF_CHECKED : MF_UNCHECKED );

	// Galactic Objects
	CheckMenuItem(hmnu, ID_VIEW_OUR_GALAXY_DISP,			swGalaxy & GLXY_OUR_GALAXY ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_SGR_A_STAR_DISP,			swGalaxy & GLXY_SGR_A_ST ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_VERA_OBJECT_DISP,			swGalaxy & GLXY_VERA ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_GLOBULAR_CLUSTER_DISP,		swGalaxy & GLXY_GLOB_CLSTR ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_GALACTIC_CENTER_BG_DISP,	bGalCenterBG ? MF_CHECKED : MF_UNCHECKED);

	// Extragalactic Objects
	CheckMenuItem(hmnu, ID_VIEW_NEARBY_GALAXY_DISP,	swGalaxy & GLXY_NEARBY_GALAXY ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_SDSS_GALAXY_DISP,	swGalaxy & GLXY_DISTANT_GALAXY ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_QSO_DISP,			swGalaxy & GLXY_QSO ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_CMB_DISP,			bCMB ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_MARK_VIRGO_CLUSTER,	bVirgoCluster ? MF_CHECKED : MF_UNCHECKED);

	// Spacecraft
	CheckMenuItem(hmnu, ID_VIEW_SPACECRAFT_DISP, bSpacecraft ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_SPACECRAFT_TRAJECTORY_PIONEER10, get_spacecraft_trajectory_mode(0) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_SPACECRAFT_TRAJECTORY_PIONEER11, get_spacecraft_trajectory_mode(1) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_SPACECRAFT_TRAJECTORY_VOYAGER1, get_spacecraft_trajectory_mode(2) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_SPACECRAFT_TRAJECTORY_VOYAGER2, get_spacecraft_trajectory_mode(3) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_SPACECRAFT_TRAJECTORY_CASSINI, get_spacecraft_trajectory_mode(4) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_SPACECRAFT_TRAJECTORY_NEW_HORIZONS, get_spacecraft_trajectory_mode(5) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_SPACECRAFT_TRAJECTORY_GALILEO, get_spacecraft_trajectory_mode(6) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_SPACECRAFT_TRAJECTORY_HAYABUSA2, get_spacecraft_trajectory_mode(7) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_SPACECRAFT_TRAJECTORY_AKATSUKI, get_spacecraft_trajectory_mode(8) ? MF_CHECKED : MF_UNCHECKED);

	// Constellations
	CheckMenuItem( hmnu, ID_VIEW_CNST_NAME,		swConstellation & CNST_NAME		? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_CNST_LINE,		swConstellation & CNST_LINE		? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_CNST_ILLUST,	swConstellation & CNST_ILLUST	? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_CNST_BOUNDARY,	swConstellation & CNST_BND		? MF_CHECKED : MF_UNCHECKED );
	
	// Milky Way
	CheckMenuItem( hmnu, ID_VIEW_CLST_NONE,				tyMilkyWay==0	? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_CLST_MILKYWAY,			tyMilkyWay==1	? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_CLST_MILKYWAY_BRIGHT,	tyMilkyWay==2	? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_CLST_MILKYWAY_IR,		tyMilkyWay==3	? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_CLST_CMB,				tyMilkyWay==4	? MF_CHECKED : MF_UNCHECKED );
	
	// Grid
	CheckMenuItem( hmnu, ID_VIEW_GRID_EQ,			bCelestialGrid	? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_GRID_EC,			bEcliptic		? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_GRID_GAL,			bGalacticGrid	? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_GRID_HORIZON,		bHorizonCoord	? MF_CHECKED : MF_UNCHECKED );

	// Scale Lines
	CheckMenuItem( hmnu, ID_VIEW_SCALELINE_CIRCLE,		swScaleLine & SCALE_CIRCLE	? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_SCALELINE_SQUARE,		swScaleLine & SCALE_SQUARE	? MF_CHECKED : MF_UNCHECKED );

	// Text Size
	CheckMenuItem(hmnu, ID_VIEW_TEXTSIZE_LARGEST,	(FontSize == vFontSize[0]) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_TEXTSIZE_LARGER,	(FontSize == vFontSize[1]) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_TEXTSIZE_MEDIUM,	(FontSize == vFontSize[2]) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_TEXTSIZE_SMALLER,	(FontSize == vFontSize[3]) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_TEXTSIZE_SMALLEST,	(FontSize == vFontSize[4]) ? MF_CHECKED : MF_UNCHECKED);

	// Other View Items
	CheckMenuItem( hmnu, ID_VIEW_HEADLIGHT,			bSideLight		? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_VIEW_PLANET_POINTER,	bSurfacePointer	? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem(hmnu, ID_VIEW_OBJECT_INFO, is_object_info_enable() ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_VIEW_VIEW_INFO, bViewInfo ? MF_CHECKED : MF_UNCHECKED);


	//-- Time
	CheckMenuItem( hmnu, ID_TIME_DISP,		bDate								? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_TIME_10SEC,		(tyTimeStep == TIMESTEP_10SEC)		? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_TIME_1MIN,		(tyTimeStep == TIMESTEP_MIN)		? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_TIME_10MIN,		(tyTimeStep == TIMESTEP_10MIN)		? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_TIME_1HOUR,		(tyTimeStep == TIMESTEP_HOUR)		? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_TIME_4HOUR,		(tyTimeStep == TIMESTEP_4HOUR)		? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_TIME_1DAY,		(tyTimeStep == TIMESTEP_DAY)		? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_TIME_1WEEK,		(tyTimeStep == TIMESTEP_WEEK)		? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_TIME_1MONTH,	(tyTimeStep == TIMESTEP_MONTH)		? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_TIME_1YEAR,		(tyTimeStep == TIMESTEP_YEAR)		? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_TIME_10YEAR,	(tyTimeStep == TIMESTEP_10YEAR)		? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_TIME_100YEAR,	(tyTimeStep == TIMESTEP_100YEAR)	? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_TIME_REALTIME_MODE,	GetRealtimeMode()				? MF_CHECKED : MF_UNCHECKED );


	//-- Planetarium Targets
	const int plnt_tgt = get_planetarium_target();
	CheckMenuItem(hmnu, ID_PLANETARIUM_TARGET_NONE, (plnt_tgt == TARGET_PLNT_NONE) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_PLANETARIUM_TARGET_SUN,  (plnt_tgt == TARGET_PLNT_SUN) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_PLANETARIUM_TARGET_MOON, (plnt_tgt == TARGET_PLNT_MOON) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hmnu, ID_PLANETARIUM_TARGET_JUPITER, (plnt_tgt == TARGET_PLNT_JUPITER) ? MF_CHECKED : MF_UNCHECKED);



	//-- Settings
	// Display Modes
	char ty = get_display_mode();
	CheckMenuItem( hmnu, ID_SETTING_DISPMODE_NORMAL,			(ty == DISPMODE_NORMAL)			? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( hmnu, ID_SETTING_DISPMODE_ANAGLYPH,			(ty == DISPMODE_ANAGLYPH) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem( hmnu, ID_SETTING_DISPMODE_COLOR_ANAGLYPH,	(ty == DISPMODE_COLOR_ANAGLYPH) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem( hmnu, ID_SETTING_DISPMODE_SIDE_BY_SIDE,		(ty == DISPMODE_SIDE_BY_SIDE) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem( hmnu, ID_SETTING_DISPMODE_TOP_AND_BOTTOM,	(ty == DISPMODE_TOP_AND_BOTTOM) ? MF_CHECKED : MF_UNCHECKED);
	EnableMenuItem(hmnu, ID_SETTING_DISPMODE_DOMEMASTER,		(bFBO_Available) ? MF_ENABLED : MF_GRAYED);
	if (bFBO_Available) {
		CheckMenuItem(hmnu, ID_SETTING_DISPMODE_DOMEMASTER, (ty == DISPMODE_DOME_MASTER) ? MF_CHECKED : MF_UNCHECKED);
	}


	//-- Languages
	int lang_idx = get_current_language();
	for (int i = 0; i<get_num_languages(); i++) {
		CheckMenuItem(hmnu, ID_LANG_0 + i, (lang_idx == i) ? MF_CHECKED : MF_UNCHECKED);
	}
}




//---------------------------------
//  時間設定用のポップアップ・メニュー
//---------------------------------
void
popup_time_menu(HWND hWnd, short x, short y)
{
	POINT pt;
	pt.x = x;
	pt.y = y;
	ClientToScreen(hWnd, &pt);

	TrackPopupMenu(hMenuTime, TPM_RIGHTBUTTON , pt.x, pt.y, 0, hWnd, NULL);
}




//-------------------------------------
//  スケール設定用のポップアップ・メニュー
//-------------------------------------
void
popup_scale_menu(HWND hWnd, short x, short y)
{
	POINT pt;
	pt.x = x;
	pt.y = y;
	ClientToScreen(hWnd, &pt);

	TrackPopupMenu(hMenuScale, TPM_RIGHTBUTTON , pt.x, pt.y, 0, hWnd, NULL);
}
