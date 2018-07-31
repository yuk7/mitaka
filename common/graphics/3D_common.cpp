/******************************************************************************

Copyright (c) 2007-2015   Tsunehiko Kato

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
#include  "3D_common.h"




//-----------
//  frustum
//-----------


// frustum を設定する
void
frustum::set(float _x1, float _x2, float _y1, float _y2, float _z1, float _z2)
{
	x1 = _x1;
	x2 = _x2;
	y1 = _y1;
	y2 = _y2;
	z1 = _z1;
	z2 = _z2;
}


// frustum を計算する
void
frustum::set2(	float aspc_rt,		// ウィンドウのアスペクト比(h/w)
				float screen_z,		// スクリーンまでの距離
				float screen_w,		// スクリーンの幅
				float near_z,		// 近いクリッピング面までの距離
				float far_z,		// 遠いクリッピング面までの距離
				float offset_x		// カメラ位置の x 方向のずれ（目のずれ＋スクリーン）
			 )
{
	float  dd = screen_w / 2.0f;
	if (aspc_rt >= 1.0f) {
		x1 = -dd;
		y1 = -aspc_rt * dd;
	}
	else {
		x1 = -(1.0f/aspc_rt) * dd;
		y1 = -dd;
	}
	x2 = -x1;
	y2 = -y1;
	x1 -= offset_x;
	x2 -= offset_x;

	float scl = near_z / screen_z;
	x1 *= scl;
	x2 *= scl;
	y1 *= scl;
	y2 *= scl;
	z1 = near_z;
	z2 = far_z;
}
