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
#include  "trackball.h"


//-------------
//  trackball
//-------------
void
trackball::set_point1(float x1, float y1)
{
	float x, y;
	x =  (x1-xc) / (0.5f*w); 
	y = -(y1-yc) / (0.5f*h); 
	v1 = direction(x,y);
}

quaternionf
trackball::set_point2(float x2, float y2)
{
	float x, y;
	x =  (x2-xc) / (0.5f*w); 
	y = -(y2-yc) / (0.5f*h); 
	vector3f v2 = direction(x,y);

	vector3f  axis = cross_product(v1, v2);
	float  norm = axis.norm();
	if (norm < 1e-10)  { return quaternionf(1,0,0,0); }
	axis /= norm;

	float phi = (v2-v1).norm() / (2.0 * R);
	return  make_rotation_quaternion(phi*sc, axis);
}

vector3f
trackball::direction(float x, float y)  const
{
	const float a = 1.0 / sqrt(2.0);
	float  d, t, z;

	d = sqrt(x*x + y*y);
	if (d < R * a) {
		z = float(sqrt(R*R - d*d));
	} else {
		t = R * a;
		z = t*t / d;
	}
	return  vector3d(x,y,z);
}



//---------
//  lever
//---------
void
lever::set_point1(float _y1)
{
	y1 = _y1/h;
}

float
lever::set_point2(float _y2)
{
	float y2 = _y2/h;
	float a = (y2-y1);
	return sc*a;
}
