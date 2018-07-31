/******************************************************************************

Copyright (c) 2015   Tsunehiko Kato

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
#include  "fast_pow.h"
#include  <math.h>
#include  <float.h>
#include  <stdint.h>


const int FLT_BITS = 32;
const int FLT_MNT_BITS = FLT_MANT_DIG - 1;
const int FLT_IDX_BITS = FLT_BITS - FLT_MNT_BITS - 1;
const int FLT_IDX_OFFSET = FLT_MAX_EXP - 1;
const int32_t FLT_MNT_MASK = (1 << FLT_MNT_BITS) - 1;

const int TBL_MNT_BITS = 10;

const int tblIdxN = 1 << FLT_IDX_BITS;
const int tblMntN = 1 << TBL_MNT_BITS;


fast_pow::fast_pow(float a)
{
	init(a);
}


void
fast_pow::init(float a)
{
	if (tblIdx == 0) {
		tblIdx = new float[tblIdxN];
	}
	if (tblMnt == 0) {
		tblMnt = new float[tblMntN];
	}

	// table of 2^s
	for (int i = 0; i < tblIdxN; i++) {
		tblIdx[i] = powf(2.0f, float(i - FLT_IDX_OFFSET)*a);
	}

	// table of mantissa
	for (int i = 0; i < tblMntN; i++) {
		tblMnt[i] = powf(1.0f + i / float(tblMntN), a);
	}
}


float
fast_pow::eval(float x) const
{
	if (tblIdx == 0 || tblMnt == 0 || x < 0.0f)  return 0.0f;

	union {
		float f;
		int32_t i;
	} xx;
	xx.f = x;

	int32_t idx, mnt;
	idx = xx.i >> FLT_MNT_BITS;
	mnt = (xx.i & FLT_MNT_MASK) >> (FLT_MNT_BITS - TBL_MNT_BITS);

	return tblIdx[idx] * tblMnt[mnt];
}



// Refs:
// Original algorthm from Shigeo Mitsunari
// http://homepage1.nifty.com/herumi/adv/adv43.html
