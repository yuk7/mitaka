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
#include  <windows.h>
#include  "mtk_calc_alpha.h"
#include  "mtk_screen.h"


float
calc_alpha_by_disapear_scale_inside(float current_scale, float disapear_scale)
{
	const float d_in  = 1.0;
	const float d_out = 5.0;
	const float d = current_scale / disapear_scale;
	if (d < d_in)	return 0.0f;
	if (d > d_out)	return 1.0f;

	float t = (d_out - d) / (d_out - d_in);
	return  (1.0f - t);
}


float
calc_alpha_by_disapear_scale_outside(float current_scale, float disapear_scale)
{
	const float d_in  = 0.2;
	const float d_out = 1.0;
	const float d = current_scale / disapear_scale;
	if (d > d_out)	return 0.0f;
	if (d < d_in)	return 1.0f;

	float t = (d - d_in) / (d_out - d_in);
	return  (1.0f - t);
}



float
calc_alpha_by_apparent_diameter(float Dth)
{
	float thc = 7.0f * D2R * get_fade_angle_factor();

	if (Dth >= thc)  return  1.0f;

	float lv = Dth / thc;
	return  lv*lv*lv*lv;
}


//---  Old
float
calc_alpha(const float r1, const float r2, const float r, const float fac)
{
	if (r<r1 || r>r2)  return 0;

    const float dr1= fac*0.02f;
	const float dr2= fac*0.3f;
	float alp=1;
	if (r<r1+dr1) { alp = fabs(r-r1)/dr1; }
	if (r>r2-dr2) { alp = fabs(r-r2)/dr2; }
	alp *= 0.9f;

	return alp;
}
