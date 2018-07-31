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
#ifndef __MTK_SCREEN_H__
#define __MTK_SCREEN_H__
#include  "../common/graphics/gl_common.h"



//-------------
//  Functions
//-------------
//--  screen --
float	get_screen_z();
float	get_screen_w();
float	get_z_near();
float	get_z_mid();
float	get_z_far();
float	get_obj_z();

void	set_screen_z(float z);
void	set_screen_w(float w);
void	set_z_near(float z);
void	set_z_mid(float z);
void	set_z_far(float z);
void	set_obj_z(float z);




//--  frustum  --
frustum	get_frustum_near(float aspc_rt);
frustum	get_frustum_far(float aspc_rt);
//--  Eye  --
float	get_eye_offset_x();
void	set_eye_offset_x(float x);
float	get_screen_offset_x();
void	set_screen_offset_x(float x);
char	get_eye_CLR();
void	set_eye_CLR(char CLR);
float	get_eye_distance();
void	set_eye_distance(float dist);
bool	is_eye_dist_change_enabled();
void	set_eye_dist_change(bool bFlg);



//--  scale  --
double	get_current_scale();
double	get_scale_last();
void	set_scale(const double& scl);

double	get_clst_R();
double	calc_scale_factor(const double& scl);
double	get_scale_factor();
float	get_Rmax();

//--  Fade angle
void	set_fade_angle_factor(float fac);
float	get_fade_angle_factor();


#endif
