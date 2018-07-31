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
#ifndef  __MTK_TCP_H__
#define  __MTK_TCP_H__
#include  <windows.h>
#include  "../common/math/vector3.h"
#include  "../common/graphics/frame.h"
#include  "mtk_movie.h"
#include  "mtk_location.h"




enum {
	CMD_NONE,
	CMD_COMMAND,
	CMD_DATA_CHAR,
	CMD_DATA_SWITCH,
	CMD_DATA_INT,
	CMD_DATA_FLOAT,
	CMD_DATA_DOUBLE,
	CMD_DATA_VECTOR3F,
	CMD_DATA_LONGLONG,
	CMD_DATA_CUSTOM,
	CMD_UPDATE_VIEW,
	CMD_UPDATE_VIEW_2,
	CMD_ADD_MEDIA_FILE,
	CMD_LOCATION_INFO
};



enum {
	COMMAND_NONE,
	COMMAND_SIGNAL,
	COMMAND_EXIT_ALL,
	COMMAND_DRAW_LOWRES,
	COMMAND_DRAW_HIGHRES,
	COMMAND_SETTING_ENTER,
	COMMAND_SETTING_LEAVE,
	COMMAND_MENU_ENTER,
	COMMAND_MENU_CLOSE,
	COMMAND_MENU_EXEC,
	COMMAND_MOVIE_CLOSE,
	COMMAND_MOVIE_PLAY,
	COMMAND_MOVIE_HOME,
	COMMAND_MOVIE_END,
	COMMAND_IMAGE_CLOSE,
	COMMAND_3DCHART_CLOSE,
	COMMAND_CAPTURE_IMAGE,
	COMMAND_TAKEOFF,
	COMMAND_LANDING,
	COMMAND_LANDING_INIT,
	COMMAND_SET_PLANETARIUM_MODE,
	COMMAND_INIT_TIME,
	COMMAND_INIT_ROOT_MENU
};

enum {
	DATA_CHAR_DISPMODE,
	DATA_CHAR_SETTING_DISP,
	DATA_CHAR_PROGRAM,
	DATA_CHAR_ECLIPSE,
	DATA_CHAR_STAR,
	DATA_CHAR_CONSTELLATION,
	DATA_CHAR_GALAXY,
	DATA_CHAR_SPACECRAFT,
	DATA_CHAR_STAR_MARK,
	DATA_CHAR_ATMOSPHERE_MODE,
	DATA_CHAR_ZOOM_MODE,
	DATA_CHAR_TOPO_ZOOM_MODE,
	DATA_CHAR_LANDING_MODE,
	DATA_CHAR_OORT_CLOUD,
	DATA_CHAR_VIRGO_CLUSTER,
	DATA_CHAR_MILKYWAY,
	DATA_CHAR_CLST_GLD,
	DATA_CHAR_ECLIPTIC,
	DATA_CHAR_HORIZON_COORD,
	DATA_CHAR_GLX_GLD,
	DATA_CHAR_DIST_SCALE,
	DATA_CHAR_CMB,
	DATA_CHAR_GAL_CENTER_BG,
	DATA_CHAR_NGC,
	DATA_CHAR_SIDE_LIGHT,
	DATA_CHAR_SURFACE_POINTER,
	DATA_CHAR_LANG,
	DATA_CHAR_DATE,
	DATA_CHAR_TIMESTEP,
	DATA_CHAR_REALTIME_MODE,
	DATA_CHAR_TEST,
	DATA_CHAR_3DCHART,
	DATA_CHAR_SUBMODE_POINTER,
	DATA_CHAR_PLAY,
	DATA_CHAR_SUBTITLES,
	DATA_CHAR_TITLE,
	DATA_CHAR_ABOUT,
	DATA_CHAR_DEBUG,
	DATA_CHAR_OBJECT_INFO,
	DATA_CHAR_VIEW_INFO,
	DATA_CHAR_KEY_HELP,
	DATA_CHAR_SINGLE_EYE,
	DATA_CHAR_PATH_INFO,
	DATA_CHAR_ANIM_TYPE,
	DATA_CHAR_TITLE_MODE,
	DATA_CHAR_TITLE_PAGE,
	DATA_CHAR_MENU_POS,
	DATA_CHAR_CLEAR_MEDIA_FILES
};

enum {
	DATA_SWITCH_DISP
};

enum {
	DATA_INT_NULL,
	DATA_INT_TARGET,
	DATA_INT_PLANETARIUM_TARGET,
	DATA_INT_ANIM_COUNTER0,
	DATA_INT_ANIM_COUNTER,
	DATA_INT_SUBTITLES_INDEX,
	DATA_INT_IMAGE_OPEN,
	DATA_INT_MOVIE_OPEN,
	DATA_INT_AUDIO_OPEN
};

enum {
	DATA_FLOAT_MAG,
	DATA_FLOAT_PLNT_ANGLE,
	DATA_FLOAT_MENU_LON_ANGLE,
	DATA_FLOAT_MENU_LAT_ANGLE,
	DATA_FLOAT_MENU_FADE_FACTOR,
	DATA_FLOAT_CHART_LON_ANGLE,
	DATA_FLOAT_CHART_LAT_ANGLE,
	DATA_FLOAT_CHART_ZOOM_FACTOR,
	DATA_FLOAT_MOVIE_FADE_ALPHA,
	DATA_FLOAT_SCREEN_ALPHA,
	DATA_FLOAT_FONT_SIZE,
	DATA_FLOAT_EYE_DISTANCE,
	DATA_FLOAT_SPEED_FACTOR,
	DATA_FLOAT_SET_LANDING_LON,
	DATA_FLOAT_SET_LANDING_LAT
};

enum {
	DATA_DOUBLE_ZOOM_FACTOR,
	DATA_DOUBLE_JD
};

enum {
	DATA_VECTOR3F_FLOATING_OBJECT_POS
};

enum {
	DATA_LONGLONG_MOVIE_POS
};

enum {
	DATA_CUSTOM_POINTER_POS,
	DATA_CUSTOM_SPACECRAFT_PATH,
	DATA_CUSTOM_BASE_ORI
};


//----------------
//  data packets
//----------------
struct char_packet {
	char type;
	char val;
};

struct switch_packet {
	char type;
	char idx;
	char val;
};

struct int_packet {
	char type;
	int  val;
};

struct float_packet {
	char  type;
	float val;
};

struct double_packet {
	char type;
	double val;
};

struct vector3f_packet {
	char type;
	vector3f val;
};

struct LONGLONG_packet {
	char type;
	LONGLONG val;
};


//-------------
//  View info
//-------------
struct  view_info {
	framed		 frm;  // view
	quaterniond  qm;   // model
};

struct  view_info2 {
	float  lon, lat;
};


// const
const int svr_num_max = 12;


// extern
extern char szHostName[MAX_PATH];
extern int  svr_num;
extern char szSvrName[svr_num_max][MAX_PATH];
extern int  svr_port[svr_num_max];
extern int  TCP_PORT;
extern const int  TCP_PORT_DEFAULT;
extern int	mvp_no_ctrl, mvp_no[svr_num_max];	// 各サーバの .mvp ファイルの記述行
extern bool bConnect;


// Functions
bool	init_TCP(bool bControler);
bool	send_command(char cmd);
bool	send_data_char(char type, char val);
bool	send_data_switch(char type, char idx, char val);
bool	send_data_int(char type, int val);
bool	send_data_float(char type, float val);
bool	send_data_double(char type, const double& val);
bool	send_data_vector3f(char type, const vector3f& val);
bool	send_data_LONGLONG(char type, const LONGLONG& val);
bool	send_data_custom(char type, const char* buf0, int size);
bool	send_frame(const view_info& vwif);
bool	send_frame2(const view_info2& vwif2);
void	send_media_info(const MediaFiles& mdf, char media_list_type);
void	send_location_info(const location& loc);





#endif
