/******************************************************************************

Copyright (c) 2007   Tsunehiko Kato

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

以下に定める条件に従い、本ソフトウェアおよび関連文書のファイル（以下「ソフトウェ
ア」）の複製を取得するすべての人に対し、ソフトウェアを無制限に扱うことを無償で許
可します。これには、ソフトウェアの複製を使用、複写、変更、結合、掲載、頒布、サブ
ライセンス、および/または販売する権利、およびソフトウェアを提供する相手に同じこ
とを許可する権利も無制限に含まれます。

上記の著作権表示および本許諾表示を、ソフトウェアのすべての複製または重要な部分に
記載するものとします。

ソフトウェアは「現状のまま」で、明示であるか暗黙であるかを問わず、何らの保証もな
く提供されます。ここでいう保証とは、商品性、特定の目的への適合性、および権利非侵
害についての保証も含みますが、それに限定されるものではありません。 作者または著
作権者は、契約行為、不法行為、またはそれ以外であろうと、ソフトウェアに起因または
関連し、あるいはソフトウェアの使用またはその他の扱いによって生じる一切の請求、損
害、その他の義務について何らの責任も負わないものとします。
*******************************************************************************/
#ifndef  __ASTRO_CONST_H__
#define  __ASTRO_CONST_H__
#include <math.h>


//-----------------------------------------------
//  IAU(1976) System of Astronomical Constatnts
//-----------------------------------------------
const double speed_of_light = 299792458.0;		// Speed of light  [m/s]
const double k_Gauss		= 0.01720209895;	// Gaussian gravitational constant
const double G_grav			= 6.672e-11;		// Constant of gravitation [m^3 kg^-1 s^-2]

const double	J2000 = 2451545.0;					// (J2000.0 = JD2451545.0)
const double	epsJ2000 = 84381.448 / (60.0*60.0);	// obliquity at J2000.0

//-- IERS numerical standard
const double _1AU_sec   = 499.0047838061;
const double _1AU_meter = speed_of_light * _1AU_sec;

//
const double _1ly_meter = speed_of_light * 365.25 * 24.0 * 60.0 * 60.0;
const double _1pc_meter = _1AU_meter / sin(M_PI/180.0 /60.0/60.0);

//
const float _1AU_km = _1AU_meter * 1e-3;
const float _1km_AU = 1.0 / _1AU_km;
const float _1ly_km = _1ly_meter * 1e-3;
const float _1km_ly = 1.0 / _1ly_km;
const float _1pc_km = _1pc_meter * 1e-3;
const float _1km_pc = 1.0 / _1pc_km;

//
const float _1AU_pc = _1AU_meter/_1pc_meter;
const float _1pc_AU = _1pc_meter/_1AU_meter;
const float _1pc_ly = _1pc_meter/_1ly_meter;
const float _1ly_pc = _1ly_meter/_1pc_meter;


#endif
