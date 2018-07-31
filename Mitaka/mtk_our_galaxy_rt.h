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
#ifndef __MTK_OUR_GALAXY_RT_H__
#define __MTK_OUR_GALAXY_RT_H__
#include  "../common/win_file.h"
#include  "../common/astro/astrometry.h"
#include  "mtk_draw_struct.h"
#include  "mtk_our_galaxy.h"



//
void	go_to_galactic_coord();


// 読み込み
bool	load_our_galaxy_model(const directory& dir);


// 減光の強度比
const float kpG_kpR = 1.33;
const float kpB_kpR = 1.78;


//---  銀河系描画用のメモリ  ---
void	free_memory_for_galaxy();


//---  銀河系の描画  ---
void	draw_our_galaxy_model_rt(const mtk_draw_struct& ds, const vector3d& rpos);


//---  r1 と r2 の間の optical depth を求める（r1g, r2g は銀河座標）  ---
float	calc_galactic_optical_depth_g(const vector3f& r1g, const vector3f& r2g, const float tau_max = 5);


//---  r1 と r2 の間の optical depth を求める（r1, r2 は黄道座標系）
inline float
calc_galactic_optical_depth(const vector3f& r1, const vector3f& r2, const float tau_max = 5)
{
	//--  銀河座標系での位置  --
	const vector3f rg(R_gal, 0, 0);
	vector3f r1g = M_E2G * r1 - rg;
	vector3f r2g = M_E2G * r2 - rg;

	return calc_galactic_optical_depth_g(r1g, r2g, tau_max);
}



#ifndef XP_VERSION
void	init_our_galaxy_threadpool();
void	cleanup_our_galaxy_threadpool();
#endif


#endif
