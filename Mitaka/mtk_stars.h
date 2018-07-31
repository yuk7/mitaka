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
#ifndef __MTK_STARS_H__
#define __MTK_STARS_H__
#include <vector>
#include <set>
#include <map>
#include  "mitaka.h"
#include  "../common/math/vector3.h"
#include  "../common/graphics/3D_common.h"
#include  "../common/win_file.h"
#include  "mtk_draw_struct.h"
#include  "stars.h"
#include  "../common/math/fast_pow.h"




const star_picture&	get_star_picture();



//----------
//  Define
//----------
enum {
	STAR_DRAW_NONE,
	STAR_DRAW_NEAR,
	STAR_DRAW_FAR
};


class mtk_star
{
	string key;		// キー/固有名文字列のキー

	int			obj_ID;
	vector3d	pos;		// 黄道座標 ***
	float		L, sqrL;	// ルミノシティー（一等星が１）
	int			HIP;		// HIP 番号
	bool		bDouble;	// 連星かどうか ***
	float		lv;			// 明るさ
	float		size;		// 表示サイズ
	float		Tc;
	char		SpcType, SpcIdx;	// スペクトル型
	//-- 名前 --
	bool		bNameLarge;
	bool		bNameSmall;
	char		cnstID;				// 星座の番号
	char		BayerID, BayerSub;	// バイエル名
	short		Framsteed;			// フラムスチード番号
	char		ADScomp;			// 連星番号
	string		name_L;
	string		name_S;
	float		label_width;
	//-- 描画用のテンポラリ --
	vector3d	disp_pos;	// 表示用の座標
	float		dist_sqr;	// 距離の２乗
	char		tyDraw;
public:
	bool		bPlanet;	// 惑星を持つか
	int			TempIdx;
	float		u1, u2;				// テクスチャ座標
	vector3f	eR, eU;				// ビルボードの辺の方向ベクトル
	float		Rb;					// 表示サイズ

public:
	mtk_star() {};

	void  set_key(const string& _key)	{ key  = _key; }
	const string& get_key() const  { return key; }

	void	set_obj_ID(int id) { obj_ID = id; }
	int		get_obj_ID() const { return obj_ID; }

	void			set_pos(const vector3d& _pos)	{ pos = _pos; }
	const vector3d&	get_pos() const					{ return pos; }

	void	set_disp_pos(const vector3d& pos)	{ disp_pos = pos; }
	const vector3d&	get_disp_pos() const	{ return disp_pos; }

	float	get_dist_sqr() const			{ return dist_sqr; }
	char	get_draw_type() const			{ return tyDraw; }

	void	set_spectrum_type(char type, char idx)	{ SpcType = type; SpcIdx = idx; }
	char	get_spectrum_type() const	{ return SpcType; }
	char	get_spectrum_index() const	{ return SpcIdx; }

	void	set_L(float _L)		{ L = _L; sqrL = sqrt(L); }
	float	get_L() const		{ return L; }
	float	get_sqrL() const	{ return sqrL; }

	void	set_lv(float _lv)	{ lv = _lv; }
	float	get_lv() const		{ return lv; }

	void	set_disp_size(float sz)		{ size = sz; }
	float	get_disp_size() const	{ return size; } 

	void	set_HIP(int _HIP)	{ HIP = _HIP; }
	int		get_HIP() const		{ return HIP; }

	void	set_double(bool _bDouble)	{ bDouble = _bDouble; }
	bool	is_double() const			{ return bDouble; }

	void	set_Tc(const float _Tc)				{
		Tc = _Tc;

		const star_picture& sp = get_star_picture();
		const int TempN   = sp.get_TempN();
		TempIdx = sp.get_TempIdx(Tc);
		
		float du = 1.0f / TempN;
		u1 = du*TempIdx;
		u2 = du*(TempIdx + 1);
	}
	float	get_Tc() const						{ return Tc; }
	void	set_B_V(const float B_V)			{ set_Tc( B_V_to_Tc(B_V) ); }

	void	set_name_info(char cnst_ID, char Bayer_ID, char Bayer_sub, short FramsteedNo, char _ADScomp) {
		cnstID			= cnst_ID;
		BayerID			= Bayer_ID;
		BayerSub		= Bayer_sub;
		Framsteed		= FramsteedNo;
		ADScomp			= _ADScomp;
	}
	char	get_cnst_ID() const				{ return cnstID; }
	char	get_BayerID() const				{ return BayerID; }
	char	get_BayerSub() const			{ return BayerSub; }
	short	get_FramsteedNo() const			{ return Framsteed; }
	char	get_ADScomp() const				{ return ADScomp; }

	void	make_name(char _swStar, char _swCnst);
	bool	get_bNameLarge() const			{ return bNameLarge;  }
	bool	get_bNameSmall() const			{ return bNameSmall; }
	const string&	get_NameLarge() const	{ return name_L; }
	const string&	get_NameSmall() const	{ return name_S; }
	float	get_label_width() const			{ return label_width; }
};


//-------------
//  Constants
//-------------

//-- Luminosity
const float L_ref_ini = 1.0;
const float Scl_star2 = 1000 * _1AU_pc;	// ３Dモード時の、背景の星を表示する最大スケール
const float Scl_inner = 0.4 * Scl_star2;
const RGBAf		col_star_name(0.9,0.9,1.0,0.8);		// star name


//-------------
//  Functions
//-------------
bool	load_Hipparcos_data_2(FILE* fp);
bool	load_exoplanet_list(const char* filename);
bool	load_stars_with_exoplanet(FILE* fp);

string	make_Bayer_name(int BayerID, int BayerSub, int FramsteedNo, char ADScomp, const string& szCnst);


mtk_star&	get_star(int idx);
bool	is_HIP_present(int HIP);

//--  size
int				get_stars_num();
const mtk_star&	get_star_by_HIP(int HIP);

//--  position
vector3d	get_star_position(int idx);
vector3d	get_star_position_by_HIP(int HIP);



void	init_pow_for_stars();
void	init_star_textures();

void	make_star_name_text_2(char _swStar, char _swCnst);



void	draw_a_star(const mtk_draw_struct& ds, const vector3f& rpos, float Rb, const RGBf& RGBcr, const int idx);
void	draw_a_star_TempIdx(const mtk_draw_struct& ds, const vector3f& rpos, float sz, const RGBf& RGBcr, const int TempIdx, const float af0);
void	draw_a_star_TempIdx_2(const mtk_draw_struct& ds, const vector3f& rpos, float sz, const int TempIdx, const float af0);


void	draw_double_star_mark(const mtk_draw_struct& ds, const vector3f& xyz, const mtk_star& st, const float sz, const float alpha);
void	mark_stars_with_planets(const mtk_draw_struct& ds, const vector3f& xyz, const mtk_star& st, const float sz0, const float alpha);


//--  Luminosity Reference --
float	get_L_ref();
void	set_L_ref(float L_ref);
float	get_log_L_ref();







#endif
