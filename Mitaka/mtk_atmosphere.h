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
#ifndef __MTK_ATMOSPHERE_H__
#define __MTK_ATMOSPHERE_H__
#include  "../common/graphics/color.h"
#include  "mtk_draw_struct.h"



struct  atmosphere
{
	double R;		// Radius (in meter)
	double H;		// Scale height (in meter)
	double n0;		// Number density of scatterer at surface
	double dp;		// 描画する大気の厚さ(in H)
	double I0;		// Intensity
};




// Object
void	add_atmosphere(int plnt_idx, const atmosphere& atm);
bool	has_atmosphere(int plnt_idx);
const atmosphere&	get_atmosphere(int plnt_idx);





// Draw
void	draw_atmosphere(const atmosphere& atm, const double& zoom_rate, bool bMultipleScattering);
void	draw_planet_highlight(const mtk_draw_struct& ds, int pl_idx, const RGBf col);


//
RGBf	calc_scattered_light_intensity(float R1, float R2, float n0, float H, float I0,
								const vector3f& r0, const vector3f& ev, const vector3f& lv,
								const int imax);


void	init_multiple_scattering(const char* filename);
void	free_multiple_scattering_work();
RGBf	calc_multiple_scattered_light_intensity(const vector3d& r0, const vector3d& ev, const vector3d& lv);



RGBf	calc_attenuated_light_intensity(float R1, float R2, float n0, float H, float I0,
								const vector3f& r0, const vector3f& lv, const int imax);


vector3d	calc_highlight_pos(const vector3d& re, const vector3d& rp, const double& R, const vector3d& rs);

RGBf		calc_intensity_of_highlight(const vector3d& re, const vector3d& rp, const vector3d& rs, 
										const double& R, const double& R2_H, const double& H, const double& n0, const double& I0, int imax);


#ifndef XP_VERSION
void	init_atmosphere_threadpool();
void	cleanup_atmosphere_threadpool();
#endif


#endif
