/**************************************************************************************************

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
#ifndef  __STARS_H__
#define  __STARS_H__
#include  "../common/graphics/color.h"
#include  "../common/graphics/image.h"
#include  "../common/astro/astrometry.h"


struct RGB2 {
	RGBf  ncr, cr;
};


//----------------------
//  class star_picture
//----------------------
class star_picture
{
	int		T_N, Lum_N;
	float	T_min, T_max;
	vector<float>	Temp, TempBin;
	vector<vector<RGB2> >	vvCol;
public:
	star_picture(float TempMin, float TempMax, int TempN, int LumN);
	int		get_TempN() const				{ return T_N; }
	int		get_TempIdx(float Tc) const;
	void	create_texture_image(image& img, int img_size, const float& SizeIdx, const float& ShapeIdx);
	const RGBf&		get_correct_RGB(int T_idx, float lv) const;
	int				get_Lum_rank(float lv) const;
};


//-------------------
//  star draw cache
//-------------------
struct star_draw_cache {
	int			TempIdx;		// 温度のインデックス
	int			LumRank;		// 明るさのランク
	RGBf		RGBcr;			// ガンマ補正した色
	float		sz;				// 表示サイズ
	bool		bPoint;			// 点で表示するかどうか
};


//--------------
//  class star
//--------------
class star
{
	float		Tc;
	star_draw_cache		sdc;
	float		highest_comp, lowest_comp;		// 輝度が最大の成分と最小の成分 (0-1)
public:
	void	set_Tc(const float _Tc);
	float	get_Tc() const						{ return Tc; }
	void	set_B_V(const float B_V)			{ set_Tc( B_V_to_Tc(B_V) ); }
	void	make_picture_cache(float lv, const star_picture& sp);
	const star_draw_cache&	get_picture_cache() const	{ return sdc; }
};



#endif