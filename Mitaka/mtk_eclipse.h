/**************************************************************************************************

Copyright (c) 2007, 2016   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#ifndef __MTK_ECLIPSE_H__
#define __MTK_ECLIPSE_H__
#include  "../common/math/vector3.h"
#include  "../common/math/Triangulation.h"
#include  "mtk_draw_struct.h"


struct eclipse_info {
	bool		bFlg;
	vector3d	nv, n1, n2;
	vector3d	r_umb;	// 本影コーンの始点
	vector3d	r_pen;	// 半影コーンの始点
	double		sinth_umb, costh_umb;	// 本影コーンの角度
	double		sinth_pen, costh_pen;	// 半影コーンの角度
};

struct eclipse_vertex {
	vector3d	pos;
	float		f;
};

vector3d	eclipse_get_shadow_center(const vector3d& r1, const double& R1, const vector3d& r2, const vector3d& rs);
eclipse_info	calc_eclipse_info(const vector3d& r1, const double& R1, const vector3d& r2, const double& R2, const vector3d& rs, const double& Rs);
double			calc_eclipse_fraction(double R, double dlt);

int
make_eclipse_shadow_polygon(const vector3d& r1, const double& R1, const vector3d& nz, const float f, 
							const vector3d& r2, const double& R2,
							const vector3d& rs, const double& Rs,
							const int N1, const int N2, eclipse_vertex* vVtx);

void
draw_eclipse_shadow(const mtk_draw_struct& ds,
					const vector3d& r1, const double& R1, const vector3d& nz, const float f,
					const vector3d& r2, const double& R2,
					const vector3d& rs, const double& Rs,
					const matrix3d& M,
					const float thc,
					const int N1, const int N2,
					bool bShadow, bool bBorder,
					const vector3f& shd_col,
					const double& Rc_pl);


#endif
