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
#include  "mtk_switches.h"


//
extern bool EarthCityLightsIni;


//-----------------------------------------
//  タイマー:  joystick, アニメーションで使用
//-----------------------------------------
static int	TIMER_INTERVAL = 1;		// タイマ割り込みの間隔 (in msec)

void set_timer_interval(int itvl) {
	if (itvl <= 0)		{ itvl = 1; }
	if (itvl > 10000)	{ itvl = 10000; }
	TIMER_INTERVAL = itvl;
}

int get_timer_interval()  { return TIMER_INTERVAL; }



//-------
//  設定
//-------
int TexShrinkRate = 1;	// 読み込み時の縮小率

static bool bVSYNC = true;		// 垂直同期
void	set_VSYNC(bool bSync) { bVSYNC = bSync; }
bool	get_VSYNC()  { return bVSYNC;  }


//---------
//  モード
//---------
static char tyLandingMode = LANDING_MODE_HORIZONTAL;
char	get_landing_mode()			{ return tyLandingMode; }
void	set_landing_mode(char mode)	{ tyLandingMode = mode; }


//----------------
//  表示用スイッチ
//----------------
static  char DispSwitch[DISPSW_NUM];

//---  Init
void
init_disp_switches()
{
	char sw = DISPM_DISP | DISPM_NAME | DISPM_ORBIT | DISPM_CLOUD;
	if (EarthCityLightsIni)  sw |= DISPM_CITY_LIGHTS;
	DispSwitch[ DISPSW_PLANET ]			= sw;
	DispSwitch[ DISPSW_SATELLITE ]		= DISPM_DISP | DISPM_NAME | DISPM_ORBIT | DISPM_SELECT;
	DispSwitch[ DISPSW_MINOR_PLANET ]	= DISPM_DISP | DISPM_NAME | DISPM_ORBIT | DISPM_SELECT;
	DispSwitch[ DISPSW_TNO ]			= DISPM_DISP | DISPM_NAME | DISPM_ORBIT | DISPM_SELECT;
}

//---  Set / Get
void
set_disp_switch(char idx, char val)
{
	DispSwitch[idx] = val;
}

char
get_disp_switch(char idx)
{
	return DispSwitch[idx];
}

bool
test_disp_switch(char idx, char mask)
{
	char sw = DispSwitch[idx];
	return  (sw & mask) != 0;
}

//--  日食
char  swEclipse				= ECLIPSE_DISP;
//--  恒星
char  swStar				= STAR_DISP | STAR_NAME | STAR_NAME_SELECT | STAR_ORBIT;
//--  星座
char  swConstellation		= 0;
//--  銀河
char  swGalaxy				= GLXY_OUR_GALAXY | GLXY_SGR_A_ST | GLXY_GLOB_CLSTR | GLXY_NEARBY_GALAXY | GLXY_DISTANT_GALAXY | GLXY_QSO;
//-- 大気
char AtmosphereMode = ATMOSPHERE_MULTIPLE;
//-- スケール線
char swScaleLine			= SCALE_CIRCLE;

bool  bSpacecraft			= true;			// 探査機
bool  bSurfacePointer		= true;			// 地表のポインタ
bool  bOortCloud			= true;			// オールトの雲
bool  bCMB					= true;			// CMB
bool  bGalCenterBG			= true;			// 銀河系中心の背景
bool  bVirgoCluster			= false;		// おとめ座銀河団のマーク	
bool  bNGC					= false;		// NGC天体
bool  bCelestialGrid		= false;		// 赤経・赤緯の線
bool  bEcliptic				= false;		// 黄道
bool  bHorizonCoord			= false;		// 地平座標
bool  bGalacticGrid			= false;		// 銀経・銀緯の線
bool  bDate					= true;			// 時刻を表示
bool  bSideLight			= false;		// 横からの照明
bool  bSubtitles			= false;		// 字幕モード
bool  bAppTitle				= false;		// アプリケーションのタイトル
bool  bAbout				= false;		// 情報
bool  bDebug				= false;		// デバッグ情報の表示
bool  bViewInfo				= false;		// 視点情報
bool  bSingleEye			= false;		// 片目（右目）用映像のみ
bool  bPathInfo				= false;		// パス情報の表示
bool  bKeyHelp				= false;		// キーボードヘルプを表示
bool  bFPS					= false;		// fps の表示


//----------------
//  点や線の強調率
//----------------
static float mag_factor;

void
set_mag_factor(float fac)
{
	mag_factor = fac;
}

float
get_mag_factor()
{
	return mag_factor;
}


//--------------
//  ムービー作成
//--------------
char make_movie_type = MV_AVI;
char mv_filename[256];
char mv_basefilename[256];
