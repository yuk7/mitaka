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
#include  "stars.h"


//----------------------------
//  星のテクスチャと色のテーブル
//----------------------------
star_picture::star_picture(float TempMin, float TempMax, int TempN, int LumN)
{
	T_N   = TempN;
	T_min = TempMin;
	T_max = TempMax;
	Lum_N = LumN;

	//  ベクタの確保
	Temp.resize(T_N);
	TempBin.resize(T_N-1);
	vvCol.resize(T_N);

	//  温度テーブルの作成
	const double logT_min = log10(T_min);
	const double logT_max = log10(T_max);
	const double dlogT    = (logT_max - logT_min)/(T_N-1);

	for (int i=0; i<T_N; ++i) {
		const double logT = logT_min + i*dlogT;
		Temp[i]    = pow(10.0, logT);
		if (i<T_N-1)	TempBin[i] = pow(10.0, logT + 0.5*dlogT);
	}

	//  色テーブルの作成
	for (int i=0; i<T_N; ++i) {
		RGBf rgb0 = blackbody_color( Temp[i] );
		rgb0 /= min_level(rgb0);					// 一番レベルが低い成分で規格化する
		vector<RGB2>&  vCol = vvCol[i];
		vCol.resize(Lum_N+1);
		for (int j=0; j<=Lum_N; ++j) {
			const float lv = float(j)/Lum_N;
			RGBf rgb = lv * rgb0;
			clamp(rgb);
			vCol[j].ncr	= rgb;
			vCol[j].cr	= correct_RGB( rgb );
		}
	}
}


void
star_picture::create_texture_image(image& img, int img_size, const float& SizeIdx, const float& ShapeIdx)
{
	const int sz(img_size);
	img.alloc(sz*T_N, sz, 4);

	for (int i=0; i<T_N; ++i) {
		const int ix0 = i * sz;
		RGBf rgb = blackbody_color( Temp[i] );

		//  テクスチャの作成
		double fR, fB, fG;
		rgb /= max_level(rgb);
		fR = pow(rgb.r, SizeIdx);		// 補正なしの場合 SizeIdx=0.5
		fG = pow(rgb.g, SizeIdx);
		fB = pow(rgb.b, SizeIdx);
		const float r_star2 = 0.50;
		float r0 = sz/2.0;
		float r2 = r0 * r_star2;
		for (int iy=0; iy<sz; ++iy) {
			for (int ix=0; ix<sz; ++ix) {
				float x, y, r;
				x = double(ix) - double(sz)/2.0;
				y = double(iy) - double(sz)/2.0;
				r = sqrt(x*x+y*y);

				double R,G,B,A, rr0, rr2;
				float val;
				//-- Red
				rr0 = r0*fR;
				rr2 = r2*fR;
				if (r > rr2) {
					val = 1.0 - (r-rr2)/(rr0-rr2);
					if (val<0)	{ R = 0; }
					else		{ R = correct_gamma(pow(val, ShapeIdx)); }
				} else {
					R = 1.0;
				}
				//-- Green
				rr0 = r0*fG;
				rr2 = r2*fG;
				if (r > rr2) {
					val = 1.0 - (r-rr2)/(rr0-rr2);
					if (val<0)	{ G = 0; }
					else		{ G = correct_gamma(pow(val, ShapeIdx)); }
				} else {
					G = 1.0;
				}
				//-- Blue
				rr0 = r0*fB;
				rr2 = r2*fB;
				if (r > rr2) {
					val = 1.0 - (r-rr2)/(rr0-rr2);
					if (val<0)	{ B = 0; }
					else		{ B = correct_gamma(pow(val, ShapeIdx)); }
				} else {
					B = 1.0;
				}
				//-- Alpha
				A = R+G+B;
				if (A > 1.0)  A=1.0;
				//-- Set pixel
				img.setRGBAf(ix0+ix,iy, R,G,B,A);
			}
		}
	}
}


int
star_picture::get_TempIdx(float Tc) const
{
	int idx = 0;
	for (int i=0; i<T_N; ++i) {
		if (TempBin[i] > Tc) break;
		idx++;
	}
	return  idx;
}


const RGBf&
star_picture::get_correct_RGB(int T_Idx, float lv) const
{
	if (lv > 1.0f) lv=1.0f;
	int LumIdx = int(lv*Lum_N);
	return  vvCol[T_Idx][LumIdx].cr;
}

int
star_picture::get_Lum_rank(float lv) const
{
	if (lv > 1.0f) lv=1.0f;
	if (lv < 0.0f) lv=0.0f;
	return  int(lv*100);
}


//--------------
//  class star
//--------------
void
star::set_Tc(float _Tc)
{
	Tc = _Tc;
	RGBf rgb = blackbody_color(Tc);
	highest_comp = max_level(rgb);
	lowest_comp  = min_level(rgb);
}


void
star::make_picture_cache(float lv, const star_picture& sp)
{
	// 温度のインデックス
	sdc.TempIdx = sp.get_TempIdx(Tc);
	sdc.LumRank = sp.get_Lum_rank(lv);
	sdc.RGBcr   = sp.get_correct_RGB(sdc.TempIdx, lv);
    // 星のサイズ
	const float sz_max = 50;
	float sz, hcm;
	hcm = highest_comp * lv;
	sz = (hcm > 1.0)  ?  sqrt(hcm)  :  1.0;
	if (sz > sz_max) sz = sz_max;
	sdc.sz = sz;
    sdc.bPoint = (sz <= 1.0);
}

