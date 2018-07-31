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
#ifndef __MTK_SWITCHES_H__
#define __MTK_SWITCHES_H__



// タイマー
void set_timer_interval(int itvl);
int get_timer_interval();



//-------
//  設定
//-------
extern int TexShrinkRate;

void	set_VSYNC(bool bSync);
bool	get_VSYNC();


//---------
//  モード
//---------
enum {
	LANDING_MODE_HORIZONTAL,
	LANDING_MODE_SURFACE_VIEW
};
char	get_landing_mode();
void	set_landing_mode(char mode);

//--------------
//  表示スイッチ
//--------------
//--- 日食の表示
const char ECLIPSE_DISP		= 0x01;		// 日食の影の表示
const char ECLIPSE_BORDER	= 0x02;		// 本影・半影の境界
//--- 恒星
const char STAR_DISP		= 0x01;		// 星を表示するか
const char STAR_NAME		= 0x02;		// 星の固有名
const char STAR_NAME_SELECT	= 0x04;		// 選択した星の固有名のみを表示
const char STAR_BAYER		= 0x08;		// 星のバイエル名
const char STAR_FRAMSTEED	= 0x10;		// 星のフラムスチード番号
const char STAR_HIP			= 0x20;		// 星のHIP番号
const char STAR_ORBIT		= 0x40;		// 星の軌道
//--- 星座
const char CNST_NAME		= 0x01;		// 星座名
const char CNST_LINE		= 0x02;		// 星座線
const char CNST_ILLUST		= 0x04;		// 星座絵
const char CNST_BND			= 0x08;		// 星座の境界線
//--- 銀河
const char GLXY_OUR_GALAXY		= 0x01;	// 銀河系
const char GLXY_SGR_A_ST		= 0x02;	// いて座A*
const char GLXY_GLOB_CLSTR		= 0x04;	// 球状星団
const char GLXY_NEARBY_GALAXY	= 0x08;	// 近傍銀河
const char GLXY_DISTANT_GALAXY	= 0x10;	// 遠方銀河
const char GLXY_QSO				= 0x20;	// QSO
const char GLXY_VERA			= 0x40;	// VERA天体
//--- スケール線
const char SCALE_CIRCLE		= 0x01;		// スケール円
const char SCALE_SQUARE		= 0x02;		// スケール枠



//-------------
//  Time Step
//-------------
enum {
	TIMESTEP_10SEC,
	TIMESTEP_MIN,
	TIMESTEP_10MIN,
	TIMESTEP_HOUR,
	TIMESTEP_4HOUR,
	TIMESTEP_DAY,
	TIMESTEP_WEEK,
	TIMESTEP_MONTH,
	TIMESTEP_YEAR,
	TIMESTEP_10YEAR,
	TIMESTEP_100YEAR
};
extern	char  tyTimeStep;



//---------
//  Scale
//---------
enum {
	SCALE_10MKM,
	SCALE_1AU,
	SCALE_10AU,
	SCALE_100AU,
	SCALE_1000AU,
	SCALE_10000AU,
	SCALE_1LY,
	SCALE_10LY,
	SCALE_100LY,
	SCALE_1KLY,
	SCALE_10KLY,
	SCALE_100KLY,
	SCALE_1MLY,
	SCALE_10MLY,
	SCALE_100MLY,
	SCALE_1GLY,
	SCALE_10GLY
};



//-----------------
//  星のマークモード
//-----------------
enum {
	STARMARK_NONE,
	STARMARK_DOUBLE,
	STARMARK_EXOPLANETS
};
extern	char tyStarMark;



//----------------
//  天の川・天球面
//----------------
enum {
	MILKYWAY_NONE,
	MILKYWAY_VISIBLE,
	MILKYWAY_VISIBLE2,
	MILKYWAY_IR,
	MILKYWAY_CMB
};
extern	char  tyMilkyWay;		// 天の川の種類



//--------------
//  大気の表示法
//--------------
enum {
	ATMOSPHERE_NONE,
	ATMOSPHERE_SINGLE,
	ATMOSPHERE_MULTIPLE
};
extern char AtmosphereMode;



//================
//  表示用スイッチ
//================
const int DISPSW_NUM = 4;

//--  Index
enum {
	DISPSW_PLANET,			// 惑星
	DISPSW_SATELLITE,		// 衛星
	DISPSW_MINOR_PLANET,	// 小惑星
	DISPSW_TNO				// 太陽系外縁天体
};

//--  Mask
const char DISPM_DISP			= 0x01;		// 表示
const char DISPM_NAME			= 0x02;		// 名前
const char DISPM_ORBIT			= 0x04;		// 軌道
const char DISPM_SELECT			= 0x08;		// 選択
const char DISPM_AXIS			= 0x10;		// 自転軸
const char DISPM_CLOUD			= 0x20;		// 雲
const char DISPM_CITY_LIGHTS	= 0x40;		// 夜景

void	init_disp_switches();
void	set_disp_switch(char idx, char val);
char	get_disp_switch(char idx);
bool	test_disp_switch(char idx, char mask);


extern	char  swEclipse;			// 日食
extern	char  swStar;				// 恒星
extern	char  swConstellation;		// 星座
extern	char  swGalaxy;				// 銀河
extern	char  swScaleLine;			// スケール線

extern  bool  bSpacecraft;			// 探査機
extern	bool  bSurfacePointer;		// 地表のポインタ
extern	bool  bOortCloud;			// オールトの雲
extern	bool  bCMB;					// CMB		
extern  bool  bGalCenterBG;			// 銀河系中心の背景
extern	bool  bVirgoCluster;		// おとめ座銀河団のマーク	
extern	bool  bNGC;					// NGC天体
extern	bool  bCelestialGrid;		// 赤経・赤緯の線
extern	bool  bEcliptic;			// 黄道
extern	bool  bHorizonCoord;		// 地平座標
extern	bool  bGalacticGrid;		// 銀経・銀緯の線
extern	bool  bDate;				// 時刻を表示
extern	bool  bSideLight;			// 横からの照明
extern	bool  bSubtitles;			// 字幕モード
extern	bool  bAppTitle;			// アプリケーションのタイトル
extern	bool  bAbout;				// 情報
extern	bool  bDebug;				// デバッグ情報の表示
extern	bool  bViewInfo;			// 視点情報
extern	bool  bSingleEye;			// 片目（右目）用映像のみ
extern	bool  bPathInfo;			// パス情報の表示
extern  bool  bKeyHelp;				// キーボードヘルプを表示
extern	bool  bFPS;					// fps の表示

//------------------------
//  作成するムービーのタイプ
//------------------------
enum {
	MV_AVI,
	MV_TARGA,
	MV_PNG,
	MV_TARGA6,
	MV_PNG6,
	MV_CUBIC
};

extern char make_movie_type;
extern char mv_filename[256];
extern char mv_basefilename[256];

//-------
//  状態
//-------
extern	bool  bMovieMode;			// ムービーを開いているか
extern	bool  bMoviePlaying;		// ムービーの再生中か
extern	bool  bSettingModeDisp;		// 設定モードの表示


//  点や線の強調率
void	set_mag_factor(float fac);
float	get_mag_factor();


#endif
