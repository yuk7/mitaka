/**************************************************************************************************

Copyright (c) 2007-2015   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#include  <gl/gl.h>
#include  <math.h>
#include  <assert.h>
#include  <process.h>
#include  <xmmintrin.h>
#include  "../common/math/vector3.h"
#include  "../common/astro/astro_const.h"
#include  "mtk_atmosphere.h"
#include  "../common/graphics/3D_common.h"
#include  "../common/cpu.h"
#include  "mtk_cpu_info.h"
#include  "mtk_approx_func.h"
#include  "mtk_triangle_mesh.h"
#include  "mtk_planet_position.h"
#include  "mtk_planet_orientation.h"
#include  "mtk_planets.h"
#include  "mtk_sun.h"
#include  "../common/math/Triangulation.h"
#include  "../common/graphics/image.h"
#include  "mtk_screen.h"
#include  "mtk_camera_setting.h"
#include  "mtk_thread.h"


#include  "../common/win_dlg.h"

#include  <vector>
#include  <map>
using namespace std;


bool	isVistaOrLater();





extern float EarthSpecular;
extern float EarthShininess;






//----------
//  Object
//----------
static  std::map<int, atmosphere>	vAtmosphere;


void
add_atmosphere(int plnt_idx, const atmosphere& atm)
{
	vAtmosphere[plnt_idx] = atm;
}


bool
has_atmosphere(int plnt_idx)
{
	std::map<int, atmosphere>::const_iterator  it;

	it = vAtmosphere.find(plnt_idx);
	return  (it != vAtmosphere.end());
}


const atmosphere&
get_atmosphere(int plnt_idx)
{
	return  vAtmosphere[plnt_idx];
}







const int ATMOSPHERE_IMAX  = 16;
const int ATMOSPHERE_JMAX  = 64;



//-- Wavelength in meter (CIE) R=700nm, G=546.1nm, B=435.8nm
const float lmd_R = 700.0 * 1e-9;
const float lmd_G = 546.1 * 1e-9;
const float lmd_B = 435.8 * 1e-9;

const double Ns = 2.5469e25;
const double ns_1 = 295e-6;
const double dlt = 0.031;
const double sgm0 = 32.0*M_PI*M_PI*M_PI/3.0*(ns_1*ns_1)/(Ns*Ns)*(6+3*dlt)/(6-7*dlt);
const double sgm_R = sgm0 / pow(lmd_R, 4);
const double sgm_G = sgm0 / pow(lmd_G, 4);
const double sgm_B = sgm0 / pow(lmd_B, 4);


const int lmdN = 29;
const float lambda[lmdN] = {
	410.0 * 1e-9,
	420.0 * 1e-9,
	430.0 * 1e-9,
	440.0 * 1e-9,
	450.0 * 1e-9,
	460.0 * 1e-9,
	470.0 * 1e-9,
	480.0 * 1e-9,
	490.0 * 1e-9,
	500.0 * 1e-9,
	510.0 * 1e-9,
	520.0 * 1e-9,
	530.0 * 1e-9,
	540.0 * 1e-9,
	550.0 * 1e-9,
	560.0 * 1e-9,
	570.0 * 1e-9,
	580.0 * 1e-9,
	590.0 * 1e-9,
	600.0 * 1e-9,
	610.0 * 1e-9,
	620.0 * 1e-9,
	630.0 * 1e-9,
	640.0 * 1e-9,
	650.0 * 1e-9,
	660.0 * 1e-9,
	670.0 * 1e-9,
	680.0 * 1e-9,
	690.0 * 1e-9
};




//----------------
//  SSE バージョン
//----------------
inline  float
calc_ray_sub_SSE(vector3d& rr, const vector3d& _lv, const float _R1, const int N)
{
	__declspec(align(16)) const float a0 = 1.0f;
	__declspec(align(16)) const float a1 = 0.2507213f;
	__declspec(align(16)) const float a2 = 0.0292732f;
	__declspec(align(16)) const float a3 = 0.0038278f;
	__declspec(align(16)) const float aR1 = _R1;

	const __m128 mR1 = _mm_set_ps1(aR1);
	const __m128 m0  = _mm_set_ps1(a0);
	const __m128 m1  = _mm_set_ps1(a1);
	const __m128 m2  = _mm_set_ps1(a2);
	const __m128 m3  = _mm_set_ps1(a3);


	__m128  mx, mexp;
	mexp = _mm_setzero_ps();

	for (int i=0; i<N; i+=4) {
	
		__declspec(align(16)) float rr4[4];
		rr4[0]	= rr.norm2();	rr += _lv;
		rr4[1]	= rr.norm2();	rr += _lv;
		rr4[2]	= rr.norm2();	rr += _lv;
		rr4[3]	= rr.norm2();	rr += _lv;

		mx = _mm_load_ps(rr4);

		__asm {
			; xmm1 = h/H = sqrt(rr) - _R1
			movaps xmm0, mx
			; rsqrtps xmm1, xmm0
			; rcpps   xmm1, xmm1
			sqrtps xmm1, xmm0
			movaps xmm0, mR1
			subps  xmm1, xmm0

			; xmm0 = a0
			movaps xmm0, m0

			; xmm0 += a1*x
			movaps xmm2, xmm1	; xmm2 = x
			movaps xmm3, m1
			mulps  xmm3, xmm2
			addps  xmm0, xmm3

			; xmm0 += a2*x^2
			mulps  xmm2, xmm1	; xmm2 = x^2
			movaps xmm3, m2
			mulps  xmm3, xmm2
			addps  xmm0, xmm3

			; xmm0 += a3*x^3
			mulps  xmm2, xmm1	; xmm2 = x^3
			movaps xmm3, m3
			mulps  xmm3, xmm2
			addps  xmm0, xmm3

			; xmm0 = xmm0^4
			mulps  xmm0, xmm0
			mulps  xmm0, xmm0

			; xmm0 = 1.0f/xmm0
			rcpps  xmm4, xmm0

			movaps  xmm0, mexp
			addps   xmm0, xmm4
			movaps  mexp, xmm0
		}
	}


	__declspec(align(16)) float dTs[4];
	_mm_store_ps(dTs, mexp);
	return  dTs[0] + dTs[1] + dTs[2] + dTs[3];
}







//--------------------------------
//  光線１本の計算（３波長バージョン）
//--------------------------------
inline RGBf
calc_ray(const double R1, const double R2, const double H,
			 const vector3d& r0, const vector3d& ev, const vector3d &lv,
			 const double kap_R, const double kap_G, const double kap_B,
			 const double t1, const double t2, const int imax)
{
	float FR, FG, FB;
	FR = FG = FB = 0.0;


	const double _R1 = R1 / H;
	const double _R2 = R2 / H;
	const double _R2_SQR = _R2*_R2; 
	const vector3d  d_rs = (t2 - t1)/(imax - 1)/H * ev;
	vector3d rs = (t1/H)*ev + r0/H;	// 散乱点


	double Tt = 0.0;	// opt. depth on t
	for (int i=0; i<imax; i++) {
		const double rs_SQR = rs.norm2();
        
		// 散乱点の高さと数密度を得る
		double _h = sqrt(rs_SQR) - _R1;
		const double nh = exp_d(_h);
		Tt += nh;

		// 大気の外層(R2) までの距離 smax を求める
		const double bs = dot_product(rs, lv);
		double Ds = bs*bs - rs_SQR + _R2_SQR;

		double smax;
		if (Ds <= 0.0)	{ smax = -bs; }				// 数値誤差で Ds <= 0 となる場合に対処
		else			{ smax = -bs + sqrt(Ds); }



		//-----------------------
		//  Trace scattered ray
		//-----------------------
		const int jmax = imax - i + 1;
		const vector3d& _lv = (smax/(jmax-1)) * lv;
		vector3d rr = rs;
		double Ts = 0.0;	// opt. depth on s

		if (!is_SSE_Enable()) {
			for (int j=0; j<jmax; j++) {
				_h  = rr.norm() - _R1;
				Ts += exp_d(_h);
				rr += _lv;
			}
		} else {
			const int _jmax = 4*(jmax/4);
			Ts += calc_ray_sub_SSE(rr, _lv, _R1, _jmax);
			
			for (int j=_jmax; j<jmax; j++) {
				_h  = rr.norm() - _R1;
				Ts += exp_d(_h);
				rr += _lv;
			}
		}
	

		const double dt = (t2-t1)/imax;
		const double ds = (smax*H)/jmax;
		const double T = Tt*dt + Ts*ds;

		FR += nh * exp_d(kap_R*T);
		FG += nh * exp_d(kap_G*T);
		FB += nh * exp_d(kap_B*T);


		rs += d_rs;
	}


	const float dt = (t2 - t1)/(imax-1);
	FR *= kap_R * dt;
	FG *= kap_G * dt;
	FB *= kap_B * dt;

	return  RGBf(FR,FG,FB);
}





inline RGBf
inline_calc_scattered_light_intensity(double R1, double R2, double n0, double H, double I0,
								const vector3d& r0, const vector3d& ev, const vector3d& lv,
								const int imax)
// R1, R2, H  ... in meter
{
	bool  bSecondRegion = false;
	double t1, t2, _t1, _t2;



	//-------------------
	//  大気層との交差判定
	//-------------------
	const double b  = dot_product(ev, r0);
	const double D0 = b*b - r0.norm2();
	const double DA = D0 + R2*R2;

	// 視線が大気層と交差しない場合
	if (DA <= 0.0) {
		return  RGBf(0,0,0);
	}


	const double sqrt_DA = sqrt(DA);
	const double tA1 = -b - sqrt_DA;
	const double tA2 = -b + sqrt_DA;

	if (tA2 <= 0.0) {
		//  後ろで交差する場合
		return  RGBf(0,0,0);
	} else if (tA1 < 0.0) {
		// 大気層の内部から出発
		t1 = 0.0;
		t2 = tA2;
	} else {
		// 大気層外部から
		t1 = tA1;
		t2 = tA2;
	}



	//------------------
	//  地表との交差判定
	//------------------
	//*****
	R1 *= 0.9999;
	//*****
	const double DG = D0 + R1*R1;

	// DG <= 0.0 の場合は、交差しない
	if (DG >= 0.0f) {
		const double sqrt_DG = sqrt(DG);
		const double tG1 = -b - sqrt_DG;
		const double tG2 = -b + sqrt_DG;

		if (t1 < tG2) {
			if (t1 > tG1) {
				// 地表内部の場合
			} else {
				// 前方で交差する場合
				if (t2 > tG1) {
					t2 = tG1;
				}
			}
		}
	}



	//-------------------
	//  影領域との交差判定
	//-------------------
	const vector3d ev_perp = ev - dot_product(ev, lv)*lv;
	const vector3d r0_perp = r0 - dot_product(r0, lv)*lv;
	const double as = ev_perp.norm2();
	const double bs = dot_product(ev_perp, r0_perp);
	const double DS = bs*bs - as * (r0_perp.norm2() - R1*R1);

	// 影領域と交差する可能性がある場合
	if (DS > 0.0f) {
		const double sqrt_DS = sqrt(DS);
		const double tS1 = (-bs - sqrt_DS)/as;
		const double tS2 = (-bs + sqrt_DS)/as;

		// 前方で交差する場合
		if (t1 < tS2) {
			const double el  = dot_product(ev, lv);
			const double r0l = dot_product(r0, lv);
			const double zS1 = tS1 * el + r0l;
			const double zS2 = tS2 * el + r0l;
			const double z1  = t1  * el + r0l;

			if (t1 >= tS1) {
				// 影領域内部から外部へ
				if (z1 < 0) {
					if (t2 <= tS2) {
						// すべて影領域内
						return  RGBf(0,0,0);
					}
					t1 = tS2;
				}
			} else {
				// 影領域外部から内部へ
				if (zS1 < 0.0) {
					if (t2 > tS1) {
						if (t2 <= tS2) {
							t2 = tS1;
						} else {
							bSecondRegion = true;
							_t1 = tS2;
							_t2 = t2;
							t2 = tS1;
						}
					}
				}
			}
		}
	}


	//-------
	//  計算
	//-------
	
	//-- Absorption coeff.
	const double kap_R = n0 * sgm_R;
	const double kap_G = n0 * sgm_G;
	const double kap_B = n0 * sgm_B;
	
	
	RGBf rgb(0,0,0);
	rgb = calc_ray(R1, R2, H, r0, ev, lv, kap_R, kap_G, kap_B, t1, t2, imax);
	if (bSecondRegion) {
		rgb += calc_ray(R1, R2, H, r0, ev, lv, kap_R, kap_G, kap_B, _t1, _t2, imax);
	}


	const double pi = 3.141592654;
	const double costh = dot_product(ev, -lv);			// 散乱角
	const double f = 3.0/(16.0*pi) * (1+costh*costh);	// 位相関数
	const double fac = f * I0;

	rgb.r *= fac;
	rgb.g *= fac;
	rgb.b *= fac;

	return  rgb;
}






RGBf
calc_scattered_light_intensity(float R1, float R2, float n0, float H, float I0,
								const vector3f& r0, const vector3f& ev, const vector3f& lv, const int imax)
{
	return  inline_calc_scattered_light_intensity(R1, R2, n0, H, I0, r0, ev, lv, imax);
}










//==========
//  多重散乱
//==========
int msRN;
int msThN;
double msThMax;
double  msR1, msR2, msH, msN0;
double* msR;
RGBf *vEmMap;
bool bInitMultipleScattering = false;


inline RGBf
emMap(int iR, int iTh) {
	return vEmMap[iR*msThN + iTh];
}


void
init_multiple_scattering(const char* filename)
{
	FILE* fp;
	fp = fopen(filename, "rb");
	if (fp == NULL)  return;

	fread(&msRN,  sizeof(int), 1, fp);
	fread(&msThN, sizeof(int), 1, fp);
	fread(&msR1,  sizeof(double), 1, fp);
	fread(&msR2,  sizeof(double), 1, fp);
	fread(&msH,   sizeof(double), 1, fp);
	fread(&msN0,  sizeof(double), 1, fp);
	fread(&msThMax, sizeof(double), 1, fp);

	msR = new double[msRN];
	for (int iR=0; iR<msRN; iR++) {
		msR[iR] = msR1 + (msR2 - msR1) * double(iR)/(msRN - 1);
	}


	vEmMap = new RGBf[msRN * msThN];
	for (int iR=0; iR<msRN; iR++) {
		fread(vEmMap+iR*msThN, sizeof(RGBf), msThN, fp);
	}

	fclose(fp);

	bInitMultipleScattering = true;
}


void
free_multiple_scattering_work()
{
	bInitMultipleScattering = false;
	delete [] msR;
	delete [] vEmMap;
}




//------------------
//  多重散乱光の計算
//------------------
RGBf
calc_multiple_scattered_light_intensity(const vector3d& r0, const vector3d& ev, const vector3d& lv)
{
	if (!bInitMultipleScattering)  return RGBf(0,0,0);


	const int acosN = 500;
	static double acos_tbl[acosN];

	static bool bFirst = true;
	if (bFirst) {
		bFirst = false;

		for (int i=0; i<acosN; i++) {
			double x = double(i)/(acosN - 1);
			acos_tbl[i] = (x >= 1.0)  ?  0.0  :  acos(x);
		}
	}

	const double   dTh = msThMax / (msThN - 1);
	const double   R  = norm(r0);
	const vector3d nv = r0/R;
	const double   R_sqr = R * R;

	const double kap_R = msN0 * sgm_R;
	const double kap_G = msN0 * sgm_G;
	const double kap_B = msN0 * sgm_B;




	int iR=msRN-1;
	for (int i=1; i<msRN; i++) {
		if (R < msR[i]) {
			iR = i-1;
			break;
		}
	}



	const double b  = dot_product(r0, ev);
	const double D0 = b*b - R_sqr;


	double t = 0;
	if (iR == msRN-1) {
		const double Ri = msR[iR];
		const double D = D0 + Ri*Ri;
		if (D < 0)  { return RGBf(0,0,0); }
	
		t = -b - sqrt(D);
		iR = msRN - 2;
	}

	
	RGBf I(0,0,0);
	double tauR, tauG, tauB;
	tauR = tauG = tauB = 0;


	const bool bInward = (dot_product(nv, ev) < 0);
	
	if (bInward) {
		for (int i=iR; i>=0; i--) {
			const double Ri = msR[i];
			const double D = D0 + Ri*Ri;
			if (D < 0)  { iR=i; break; }
			double tn = -b - sqrt(D);


			// シェルとの交点
			const vector3d rp = tn*ev + r0;
			const double h = Ri - msR1;
			const double dt = tn - t;
			const double exp_dt_h = 0.5 * exp_d(h/msH) * dt;

			double dTauRh, dTauGh, dTauBh;
			dTauRh = kap_R * exp_dt_h;
			dTauGh = kap_G * exp_dt_h;
			dTauBh = kap_B * exp_dt_h;
			tauR += dTauRh;
			tauG += dTauGh;
			tauB += dTauBh;


			const vector3d np = rp / Ri;
			
			double dp = dot_product(np, lv);
			if (dp > 1) dp=1;
			if (dp < -1) dp=-1;
			const double th = (dp >= 0)  ?  acos_tbl[int(dp*(acosN-1))]  :  M_PI - acos_tbl[int(-dp*(acosN-1))];
			
			const int iTh = th / dTh;

			if (iTh < msThN-1) {
				const float f2 = (th/dTh - iTh) * dt;
				const float f1 = dt - f2;
				const RGBf emRGB = f1 * emMap(i, iTh) +  f2 * emMap(i, iTh+1);

				I.r += exp_d(tauR)*emRGB.r;
				I.g += exp_d(tauG)*emRGB.g;
				I.b += exp_d(tauB)*emRGB.b;
			}

			tauR += dTauRh;
			tauG += dTauGh;
			tauB += dTauBh;
			t = tn;

			if (i==0)  return I;
		}
	}

	

	for (int i=iR+1; i<msRN; i++) {
		const double Ri = msR[i];
		const double D = D0 + Ri*Ri;
		double tn = -b + sqrt(D);

		// シェルとの交点
		const vector3d rp = tn*ev + r0;
		const double h = Ri - msR1;
		const double dt = tn - t;
		const double exp_dt_h = 0.5 * exp_d(h/msH) * dt;

		double dTauRh, dTauGh, dTauBh;
		dTauRh = kap_R * exp_dt_h;
		dTauGh = kap_G * exp_dt_h;
		dTauBh = kap_B * exp_dt_h;
		tauR += dTauRh;
		tauG += dTauGh;
		tauB += dTauBh;

		const vector3d np = rp / Ri;
		
		double dp = dot_product(np, lv);
		if (dp > 1) dp=1;
		if (dp < -1) dp=-1;
		const double th = (dp >= 0)  ?  acos_tbl[int(dp*(acosN-1))]  :  M_PI - acos_tbl[int(-dp*(acosN-1))];
		
		const int iTh = th / dTh;

		if (iTh < msThN-1) {
			const float f2 = (th/dTh - iTh) * dt;
			const float f1 = dt - f2;
			const RGBf emRGB = f1 * emMap(i, iTh)  +  f2 * emMap(i, iTh+1);

			I.r += exp_d(tauR)*emRGB.r;
			I.g += exp_d(tauG)*emRGB.g;
			I.b += exp_d(tauB)*emRGB.b;
		}

		tauR += dTauRh;
		tauG += dTauGh;
		tauB += dTauBh;
		t = tn;
	}

	return  I;
}



//--------------------------------------
//  r1, r2 間の大気による減光を計算する
//--------------------------------------
//   R1, R2, H, r1, r2: in meter
//--------------------------------------
inline RGBf
calc_atmosphere_attenuation(const double R1, const double R2, const double H,
			 const vector3d& r1, const vector3d& r2,
			 const double kap_R, const double kap_G, const double kap_B,
			 const int imax)
{
	//----------------
	//  計算領域の設定
	//----------------
	const vector3d r12 = r2 - r1;
	const double   l   = norm(r12);
	const vector3d nv  = r12 / l;
	const double b = dot_product(nv, r1);
	const double R_sqr = r1.norm2();
	double D = b*b + R2*R2 - R_sqr;

	//-- 大気層と交差しない
	if (D <= 0.0) {
		return  vector3d(1,1,1);
	}



	double t1, t2;
	t1 = -b - sqrt(D);
	t2 = -b + sqrt(D);


	//-- 大気層まで到達しないか後ろで交わる
	if (t1 >= l || t2 <= 0.0) {
		return  vector3d(1,1,1);
	}

	//-- 大気層内にいる場合
	if (t1 <= 0.0) {
		t1 = 0.0;
	}

	if (t2 >= l) {
		t2 = l;
	}



	//--------------
	//  減光率の計算
	//--------------
	const double _R1 = R1 / H;
	const double _R2 = R2 / H;

	const double dt = t2 - t1;
	const vector3d  d_rs = dt/(imax - 1)/H * nv;
	vector3d rs = (nv*t1 + r1)/H;


	double Tt = 0.0;	// opt. depth on t
	for (int i=0; i<imax; i++) {
		const double rs_SQR = rs.norm2();
        
		// 散乱点の高さと数密度を得る
		double _h = sqrt(rs_SQR) - _R1;
		const double nh = exp_d(_h);
		Tt += nh;

		rs += d_rs;
	}

	const float ds = dt/(imax-1);
	const double tauR = kap_R * Tt * ds;
	const double tauG = kap_G * Tt * ds;
	const double tauB = kap_B * Tt * ds;


	float AR, AG, AB;
	AR = exp(-tauR);
	AG = exp(-tauG);
	AB = exp(-tauB);

	return  RGBf(AR,AG,AB);
}



//--------------
//  吸収された光
//--------------
inline RGBf
calc_ray_attenuation(const double R1, const double R2, const double H,
			 const vector3d& r0, const vector3d &lv,
			 const double kap_R, const double kap_G, const double kap_B,
			 const int imax)
{


	const double _R1 = R1 / H;
	const double _R2 = R2 / H;

	const double b = dot_product(lv, r0);
	double D = b*b + R2*R2 - R1*R1;
	if (D < 0) D=0;
	const double tmax = -b + sqrt(D);

	const vector3d  d_rs = tmax/(imax - 1)/H * lv;
	vector3d rs = r0/H;


	double Tt = 0.0;	// opt. depth on t
	for (int i=0; i<imax; i++) {
		const double rs_SQR = rs.norm2();
        
		// 散乱点の高さと数密度を得る
		double _h = sqrt(rs_SQR) - _R1;
		const double nh = exp_d(_h);
		Tt += nh;

		rs += d_rs;
	}

	const float dt = tmax/(imax-1);
	const double tauR = kap_R * Tt * dt;
	const double tauG = kap_G * Tt * dt;
	const double tauB = kap_B * Tt * dt;


	float FR, FG, FB;
	FR = exp(-tauR);
	FG = exp(-tauG);
	FB = exp(-tauB);

	return  RGBf(FR,FG,FB);
}

RGBf
calc_attenuated_light_intensity(float R1, float R2, float n0, float H, float I0,
								const vector3f& r0, const vector3f& lv, const int imax)
{
	//-- Absorption coeff.
	const double kap_R = n0 * sgm_R;
	const double kap_G = n0 * sgm_G;
	const double kap_B = n0 * sgm_B;


	return I0 * calc_ray_attenuation(R1, R2, H, r0, lv, kap_R, kap_G, kap_B, imax);
}






//-------------------------
//  大気の Intensity の計算
//-------------------------
const int atmosphere_thread_num_max = 128;

static float cosphi[ATMOSPHERE_JMAX+1], sinphi[ATMOSPHERE_JMAX+1];
// 表示用計算領域（外側用）
static	RGBAf F_RGB[ATMOSPHERE_IMAX+1][ATMOSPHERE_JMAX+1];
static	vector3f r_in[ATMOSPHERE_IMAX+1][ATMOSPHERE_JMAX+1];
// （内側用）
static	RGBAf F_RGB2[ATMOSPHERE_IMAX+1][ATMOSPHERE_JMAX+1];
static	vector3f r_in2[ATMOSPHERE_IMAX+1][ATMOSPHERE_JMAX+1];

struct atms_info
{
	bool bInside, bMultiplueScattering;
	int skip;
	float R1, R2, H, L;
	float _R1, _R2;
	float n0, I0;
	vector3f lv;
	float th1, th2, thc;
};

static atms_info g_atmsif;


// スレッド用変数
struct atms_thread_info {
	int i0, iskip;
};


static CS cs;	// Grobal only


void
calc_atmosphere_intensity_map_sub2
							  (bool bInner,
							  const int imax,
							  const int i0, const int iskip,
							  vector3f r_out[][ATMOSPHERE_JMAX+1], RGBAf F_out[][ATMOSPHERE_JMAX+1])
{
	const bool bInside = g_atmsif.bInside;
	const bool bMultipleScattering = g_atmsif.bMultiplueScattering;
	const int  jskip = g_atmsif.skip;

	const float  R1 = g_atmsif.R1;
	const float  R2 = g_atmsif.R2;
	const float  H  = g_atmsif.H;
	const float  L  = g_atmsif.L;
	const float _R1 = g_atmsif._R1;
	const float _R2 = g_atmsif._R2;
	const float th1 = g_atmsif.th1;
	const float th2 = g_atmsif.th2;
	const float thc = g_atmsif.thc;

	const float n0  = g_atmsif.n0;
	const float I0  = g_atmsif.I0;
	const vector3f lv  = g_atmsif.lv;




	const float  RR = (bInner && bInside)  ?   R1  :   R2;
	const float _RR = (bInner && bInside)  ?  _R1  :  _R2;
	const vector3f r0 = vector3f(0,0,L);

	
	for (int i=i0; i<=imax; i+=iskip) {
		float f, th, z, rho;
		f   = float(i)/imax;
		if (bInner)	{ if (bInside) { th = f * th1; } else { th = f * thc; } }
		else		{ th = f * (th2 - thc) + thc; }
		z   = cos(th);
		rho = sin(th);


		for (int j=0; j<=ATMOSPHERE_JMAX; j+=jskip) {
			float x,y;
			x   = rho * cosphi[j];
			y   = rho * sinphi[j];
			const vector3f n(x,y,z);
			const vector3f r = RR * n;
			const vector3f ev = normalize( r - r0 );


			RGBf col;
			col = inline_calc_scattered_light_intensity(R1, R2, n0, H, I0, r0, ev, lv, 14); //22);

			if (bMultipleScattering) {
				col += I0 * calc_multiple_scattered_light_intensity(r0, ev, lv);
			}

			r_out[i][j] = _RR * n;

			RGBf rgb = col;
			F_out[i][j].rgb = rgb;
			float aa = 4.0f * max(rgb.r, max(rgb.g, rgb.b));
			if (aa > 1.0f) aa = 1.0f;
			F_out[i][j].a   = aa;
		}
	}
}



void
calc_atmosphere_intensity_sub(int i0, int iskip)
{
	//----------------------
	//  輝度分布の計算（外側）
	//----------------------
	calc_atmosphere_intensity_map_sub2(false, ATMOSPHERE_IMAX, i0, iskip, r_in, F_RGB);

	//----------------------
	//  輝度分布の計算（内側）
	//----------------------
	calc_atmosphere_intensity_map_sub2(true, ATMOSPHERE_IMAX, i0, iskip, r_in2, F_RGB2);
}


unsigned  __stdcall
calc_atmosphere_intensity_thread(void* p)
{
	atms_thread_info*  pif = (atms_thread_info*)p;
	calc_atmosphere_intensity_sub(pif->i0, pif->iskip);

	return 1;
}



void
calc_atmosphere_intensity_map_xp(atms_info& atmsif, int thread_num)
{
	HANDLE hThreads[atmosphere_thread_num_max];


	// 各スレッドの計算範囲を設定
	atms_thread_info  atif[atmosphere_thread_num_max];
	int skip = atmsif.skip;
	for (int i = 0; i<thread_num; i++) {
		atif[i].i0 = i * skip;
		atif[i].iskip = skip * thread_num;
	}

	// スレッドを起動
	//g_atmsif = atmsif;
	
	for (int i = 0; i<thread_num - 1; i++) {
		hThreads[i] = (HANDLE)_beginthreadex(NULL, 0, calc_atmosphere_intensity_thread, &atif[i + 1], 0, NULL);
	}
	calc_atmosphere_intensity_sub(atif[0].i0, atif[0].iskip);


	// すべてのスレッドが終了するのを待つ
	WaitForMultipleObjects(thread_num -1, hThreads, TRUE, INFINITE);


	for (int i = 0; i < thread_num-1; i++) {
		CloseHandle(hThreads[i]);
	}
}



//***** スレッドプールバージョン（VISTA 以降） *****
#ifndef XP_VERSION
struct atms_thread_info2 {
	volatile long id;
	atms_thread_info  atif_work[atmosphere_thread_num_max];
};

static atms_thread_info2 gAtif2;
static PTP_WORK pAtmosphereWork;



VOID CALLBACK
calc_atmosphere_intensity_work(PTP_CALLBACK_INSTANCE pInstance, PVOID pv, PTP_WORK work)
{
	atms_thread_info2*  atif = (atms_thread_info2*)pv;

	long id = InterlockedIncrement(&(atif->id));

	const atms_thread_info& pif = atif->atif_work[id-1];
	calc_atmosphere_intensity_sub(pif.i0, pif.iskip);
}

void
calc_atmosphere_intensity_map_vista(atms_info& atmsif, int cpu_num)
{
	//g_atmsif = atmsif;

	gAtif2.id = 0;

	int skip = atmsif.skip;
	for (int i = 0; i<cpu_num; i++) {
		gAtif2.atif_work[i].i0 = i * skip;
		gAtif2.atif_work[i].iskip = skip * cpu_num;
	}
	
	for (int i = 0; i < cpu_num; i++) {
		SubmitThreadpoolWork(pAtmosphereWork);
	}

	WaitForThreadpoolWorkCallbacks(pAtmosphereWork, FALSE);
}


void
init_atmosphere_threadpool()
{
	if (!isVistaOrLater())  return;

	pAtmosphereWork = CreateThreadpoolWork(calc_atmosphere_intensity_work, (PVOID)&gAtif2, NULL);
}

void
cleanup_atmosphere_threadpool()
{
	if (!isVistaOrLater())  return;

	CloseThreadpoolWork(pAtmosphereWork);
}
#endif


void
calc_atmosphere_intensity_map(atms_info& atmsif, int thread_num)
{
	if (thread_num <= 0)						thread_num = 1;
	if (thread_num > atmosphere_thread_num_max)	thread_num = atmosphere_thread_num_max;

	g_atmsif = atmsif;

#ifndef XP_VERSION
	calc_atmosphere_intensity_map_vista(atmsif, thread_num);
#else
	calc_atmosphere_intensity_map_xp(atmsif, thread_num);
#endif
}





///============================================
///   Draw Atmosphere
///--------------------------------------------
///   R: radius (in meter)
///   H: scale height (in meter)
///   n: number density of scatterer at surface
///  dp: 描画する大気の厚さ(in H)
///============================================
void
draw_atmosphere(const atmosphere& atm, const double& zoom_rate, bool bMultipleScattering)
{
	const int imax  = ATMOSPHERE_IMAX;
	const int imax2 = ATMOSPHERE_IMAX;
	const int jmax  = ATMOSPHERE_JMAX;

	static bool first = true;
	if (first) {
		first = false;
		for (int j=0; j<=jmax; j++) {
			float phi = float(j)/jmax * 2 * 3.1415926535;
			cosphi[j] = cos(phi);
			sinphi[j] = sin(phi);
		}
	}

	const double R  = atm.R * zoom_rate;
	const double H  = atm.H * zoom_rate;
	const double n0 = atm.n0 / zoom_rate;
	
	GLfloat M[16];
	glGetFloatv( GL_MODELVIEW_MATRIX,  M);
	const float LSQR   = M[12]*M[12] + M[13]*M[13] + M[14]*M[14];	// 中心までの距離
	const float R1SQR  = M[0] *M[0]  + M[1] *M[1]  + M[2] *M[2];		// 半径
	
	const float _L  = sqrt(LSQR);			// 視点までの距離
	const float _R1 = sqrt(R1SQR);			// 地表の半径
	const float _H  = H/R;
	float _R2 = _R1 + atm.dp*_H*_R1;		// 大気の半径


	// 視点が地球内部の時は描画しない
	if (_L < _R1)  return;

	//--- 視直径による判定 ---
	const float ad  = _R2/_L;
	const float adc = 0.005;
	if (ad < adc)	return;		// 見込む角が小さすぎる場合は描画しない



	// 間引き率の設定
	int skip = 1;
	float ad2 = ad;
	while (ad2 < 25*adc) {
		skip *= 2;
		ad2 *= 5;
	}

	bool bInside = (_L <= 1.001*_R2);
	
	if (bInside) {
		_R2 *= 1.001;
	}


	vector3f n, nx, ny;
	n = normalize( -vector3f(M[12], M[13], M[14]) );	// オブジェクトから見た視点の方向
	if (fabs(n.x) < fabs(n.y)) {
		ny = normalize(cross_product(n, vector3f(1,0,0)));
		nx = normalize(cross_product(ny, n));
	} else {
		nx = normalize(cross_product(vector3f(0,1,0), n));
		ny = normalize(cross_product(n, nx));
	}
	
	M[0] = nx.x;	M[1] = nx.y;	M[2]  = nx.z;	M[3]  = 0.0;
	M[4] = ny.x;	M[5] = ny.y;	M[6]  = ny.z;	M[7]  = 0.0;
	M[8] = n.x;		M[9] = n.y;		M[10] = n.z;	M[11] = 0.0;


	glPushMatrix();
	glLoadMatrixf(M);


	//----------------
	//  半径 in meter
	//----------------
	const double R1 = R;
	double R2 = R1 + atm.dp*H;
	const double fac = R1/_R1;
	const double L  = fac * _L;
	
	if (bInside) {
		R2 *= 1.001;
	}
	

	//-----------------
	//  光源の方向を得る
	//-----------------
	float lp[4];
	glGetLightfv(GL_LIGHT0, GL_POSITION, lp);						// 視点座標系での光線の位置
	const vector3f lve = normalize(vector3f(lp[0], lp[1], lp[2]));	// 光源方向を向いたベクトル（平行光線を仮定；視点座標）

	//-- オブジェクト方向をｚ座標とした座標系での光線ベクトルに変換
	vector3f lv;
	lv.x = dot_product(lve, nx);
	lv.y = dot_product(lve, ny);
	lv.z = dot_product(lve, n );




	double th1, th2, thc;
	th1  = acos(R1/L);
	if (!bInside) {
		//  Outside
		th2  = acos(R2/L);
		const double _th1 = asin(R1/L);
		thc  = -_th1 + asin(R1/R2);
	} else {
		// Inside
		th2 = th1 + acos(R1/R2);
		thc = 0;
	}


	//----------------
	//  輝度分布の計算
	//----------------
	atms_info atmsif;
	atmsif.bInside				= bInside;
	atmsif.bMultiplueScattering	= bMultipleScattering;
	atmsif.skip					= skip;
	atmsif.R1	= R1;
	atmsif.R2	= R2;
	atmsif.H	= H;
	atmsif.L	= L;
	atmsif._R1	= _R1;
	atmsif._R2	= _R2;
	atmsif.n0	= n0;
	atmsif.I0	= atm.I0;
	atmsif.lv	= lv;
	atmsif.th1	= th1;
	atmsif.th2	= th2;
	atmsif.thc	= thc;


	int thread_num = get_number_of_cpu();
	calc_atmosphere_intensity_map(atmsif, thread_num);




	//-------
	//  描画
	//-------
	glDepthMask(GL_FALSE);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glBlendFunc(GL_ONE, GL_ONE);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	if (bInside) {
		glDisable(GL_CULL_FACE);
	}

	//--- 外側
	for (int i=0; i<imax; i+=skip) {

		glBegin(GL_TRIANGLE_STRIP);
		for (int j=0; j<=jmax; j+=skip) {
			glColor4fv( F_RGB[i][j].get_pointer() );
			glVertex3fv( r_in[i][j].get_pointer() );
			glColor4fv( F_RGB[i+skip][j].get_pointer() );
			glVertex3fv( r_in[i+skip][j].get_pointer() );
		}
		glEnd();
	}
	
	
	//--- 内側
	if (bInside)  glDisable(GL_DEPTH_TEST);
	for (int i=0; i<imax2; i+=skip) {
		glBegin(GL_TRIANGLE_STRIP);
		for (int j=0; j<=jmax; j+=skip) {
			glColor4fv( F_RGB2[i][j].get_pointer() );
			glVertex3fv( r_in2[i][j].get_pointer() );
			glColor4fv( F_RGB2[i+skip][j].get_pointer() );
			glVertex3fv( r_in2[i+skip][j].get_pointer() );
		}
		glEnd();
	}
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);


	glPopMatrix();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_TRUE);
}





inline  double
calc_highlight_psi(const double& L, const double& R, const double& th)
{
	const double costh = cos(th);
	const double l2 = L*L + R*R - 2*L*R*costh;
	const double l  = sqrt(l2);

	const double cosalpha = L/l - R/l * costh;
	const double alpha = acos(cosalpha);

	return  2*th + alpha;
}


//------------------------------------
//  ハイライトの位置を求める
//  視点 re、惑星 rp （半径 R）、太陽 rs
//------------------------------------
vector3d
calc_highlight_pos(const vector3d& re, const vector3d& rp, const double& R, const vector3d& rs)
{
	const vector3d lv = normalize(rs - rp);	// 惑星中心から見た光源方向

	// 視点、光源、惑星中心を含む平面の基底ベクトルを作る（惑星中心が原点）
	const vector3d ex = normalize(re - rp);

	const vector3d ey0 = lv - dot_product(lv, ex) * ex;
	const double ey0l = norm(ey0);

	if (ey0l < 1e-8) {
		// ほぼ視線上
		return  rp + R*ex;
	}
	

	const vector3d ey = ey0/ey0l;

	//
	const double cosphi = dot_product(lv, ex);  //位相角
	const double phi = acos(cosphi);
	const double L = norm(re - rp);
	const double thc = acos(R/L);


	// ２分法でハイライトの位置の角度 th を求める
	double th1, th2, th;
	th1 = 0;
	th2 = thc;


	int c = 0;
	do {
		th = 0.5*(th1 + th2);
		double d = calc_highlight_psi(L, R, th) - phi;

		if (fabs(d) < 1e-4)  break;

		if (d < 0)	{ th1 = th; }
		else		{ th2 = th; }
		c++;
	} while (th2 - th1 > 1e-5);

	// ハイライトの位置の座標を求める
	return  rp + R*cos(th)*ex + R*sin(th)*ey;
}



RGBf
calc_intensity_of_highlight(const vector3d& re, const vector3d& rp, const vector3d& rs, 
							const double& R, const double& R2_H, const double& H, const double& n0, const double& I0, int imax)
{
	const vector3d  lv = normalize(rs - rp);
	const vector3d  rh = calc_highlight_pos(re, rp, R*_1km_pc/1000, rs);
	const vector3d  rh_m = (rh - rp) * _1pc_meter;
	const vector3d  rs_m = (rs - rp) * _1pc_meter;
	const vector3d  re_m = (re - rp) * _1pc_meter;


	//-- Absorption coeff.
	const double kap_R = n0 * sgm_R;
	const double kap_G = n0 * sgm_G;
	const double kap_B = n0 * sgm_B;


	//-- 入射光の減光率 --
	RGBf Atn = calc_atmosphere_attenuation(R, R+R2_H*H, H, rs_m, rh_m, kap_R, kap_G, kap_B, imax);
	
	//-- 反射光の減光率 --
	Atn.mul_each( calc_atmosphere_attenuation(R, R+R2_H*H, H, rh_m, re_m, kap_R, kap_G, kap_B, imax) );


	//-- 最終的な強度 --
	RGBf I = Atn * I0;

	
	// 反射率の計算 (Fresnel formula for reflection)
	const vector3d nv = normalize(rh_m);
	const float cosi = dot_product(nv, lv);
	const float nw = 1.33;
	const float na = 1.00028;
	const float a  = na/nw;

	// Perp
	const float c1 = sqrt(1 - a*a*(1-cosi*cosi));
	const float Eperp = (a*cosi - c1) / (a*cosi + c1);

	// Para
	const float Epara = (cosi - a*c1)/(cosi + a*c1);

	I *= 0.5*(Eperp*Eperp + Epara*Epara);

	return I;
}





static vector2f vVtx2D[TRIANGLE_VTX_MAX];
static float vU[TRIANGLE_VTX_MAX], vV[TRIANGLE_VTX_MAX];
static float hl_int[TRIANGLE_VTX_MAX];


//----------------------------
//  惑星のテクスチャ座標を求める
//----------------------------
inline  void
get_planet_texture_coord(const vector3d& n, float& u, float& v, float phi_shift)
{
	// 緯度・経度を求める
	const float rho = sqrt(n.x*n.x + n.y*n.y);
	const float thp = atan2(rho, float(n.z));
			
	float phip = atan2(n.y, n.x) + D2R*phi_shift;
	if (phip < 0.0)			{ phip += 2.0*M_PI; }
	if (phip > 2.0*M_PI)	{ phip -= 2.0*M_PI; }

	u = phip / (2.0*M_PI);
	v = 1.0f - thp  / M_PI;
}



//--------------------------
//  ハイライトの描画
//--------------------------
//   re: 視点位置
//   rp: 惑星中心の位置
//   rs: 光源の位置
//   R: 惑星半径 (in meter)
//--------------------------
void
draw_planet_highlight(const mtk_draw_struct& ds, int pl_idx, const RGBf col)
{
	vector3d eF, eU, eR;
	quaterniond ori0 = ds.camera_base.get_ori() * get_base_camera_ori();
	quaternion_to_axes(ori0, eF, eU, eR);


	vector3d r_cam = get_local_camera_pos();
	
			
	const vector3d re = ds.camera.get_pos() + ( r_cam.x*eR + r_cam.y*eU - r_cam.z*eF)/ ds.camera.get_scale_factor();	// 視点の位置
	


	
	const vector3d rp = get_planet_position(pl_idx);			// 惑星中心
	const vector3d rs = get_sun_position();									// 太陽の位置
	const vector3d lv = normalize(rs - rp);						// 光線方向ベクトル
	const double   R = get_planet_Re(ds.tyMode, pl_idx) * _1km_pc;			// 惑星半径
	const double   L = norm(re - rp);								// 視点と惑星中心の距離

	const double thc = asin(R / L);
	if (R >= L)			return;
	if (thc < 0.005)	return;



	// 視点座標系
	quaterniond ori = ds.camera_base.get_ori() * get_base_camera_ori() * get_local_camera_orientation();

	// 視点座標系から黄道座標系への変換行列
	const matrix3d  M  = quaternion_to_matrix3(ori);
	const matrix3d  tM = transpose(M);
	
	// 黄道座標系から惑星座標系への変換行列を得る
	const matrix3d T = get_planet_orientation(pl_idx, true);
	
	// 視点座標系から惑星座標系への変換行列
	const matrix3d A = T * M;
	const vector3d re_p = T * (re - rp);
	const double re_sqr = norm2(re_p);
	const double R_sqr  = R * R;

	const vector3d lv_p = T * lv;

	const double lv_nv_min = 0.0;


	//
	const double beta = 0.5 * acos(pow(0.5, 1.0/EarthShininess));	// 水面の平均的ばらつきの角度
	const double dpc   = sin(beta);									// 影の効果が現れる内積値


	//============================
	//  サンプリング・ポイントの設定
	//============================
	int vn=0;
	clear_triangle_vtx();


	//-------------------------------
	//  グリッド状のサンプリングポイント
	//-------------------------------
	const int Nx = 60;
	const int Ny = 60;


	const frustum& frs = ds.get_frustum();
	const float sclf = 1.002 * get_screen_z() / frs.get_z1();
	float x1, y1, x2, y2, z1;
	x1 = sclf * frs.get_x1();// - eye_offset_x;
	x2 = sclf * frs.get_x2();// - eye_offset_x;
	y1 = sclf * frs.get_y1();
	y2 = sclf * frs.get_y2();
	z1 = get_screen_z();


	for (int iy=0; iy<Ny; iy++) {
		float t = float(iy)/(Ny-1);
		float y = (1-t)*y1 + t*y2;
			
		for (int ix=0; ix<Nx; ix++) {
			t = float(ix)/(Nx-1);
			float x = (1-t)*x1 + t*x2;

			// 惑星座標系での視線方向ベクトル
			vector3d n = A * normalize( vector3d(x,y,-z1) );


			// 地表との交点を求める
			const double b = dot_product(n, re_p);
			const double c = re_sqr - R_sqr;
			const double D = b*b - c;

			if(D < 0) continue;
			if (b >= 0) continue;


			// 法線の計算
			const double t2 = -b + sqrt(D);
			const double t  = c/t2;
			const vector3d nv = normalize(t*n + re_p);	// 視線と地表との交点（法線ベクトル）
			const double dp = dot_product(nv, lv_p);
			if (dp  <= lv_nv_min )  continue;

			// ハイライトの計算
			float hl = 0.0;
			if ( dp > 0.0 ) {
				const vector3d ev = -n;
				const vector3d hv = normalize(lv_p + ev);
				const double s = dot_product(hv, nv);
			

				if (s <= 0.0) continue;
				hl = (s <= 0)  ?  0  :  pow(float(s), EarthShininess);
				if (dp < dpc) {
					hl *= sqrt(dp/dpc);
				}
				if (hl < 1e-3) continue;
			}
			hl_int[vn] = hl;

			add_triangle_xy(x, y);
			vVtx2D[vn].x = x;
			vVtx2D[vn].y = y;

			float u, v;
			get_planet_texture_coord(nv, u, v, 180.0);
			vU[vn] = u;
			vV[vn] = v;

			vn++;
		}
	}




	//-------------
	//  地球の外縁
	//-------------
	const vector3d nep = normalize(re - rp);
	const vector3d rc = rp + (R_sqr/L) * nep;
	const vector3d nx = get_a_normal_unit_vector(nep);
	const vector3d ny = cross_product(nep, nx);
	const double costhc = sqrt(1 - R_sqr /(L*L));
	const double Rc = R * costhc;
	
	int phiN = 60 * (thc/(M_PI/4));
	if (phiN < 20)  phiN = 20;
	if (phiN > 60)  phiN = 60;




	//-----------------
	//  ハイライト座標系
	//-----------------
	const vector3d ne = normalize(re - rp);
	const vector3d rh_0 = calc_highlight_pos(re, rp, R, rs);
	const vector3d rh   = rh_0 - rp;

	const vector3d ez_h = normalize(rh);
	const vector3d ex_h = normalize( cross_product(lv, ne) );
	const vector3d ey_h = cross_product(ez_h, ex_h);


	int ThN = 30 * (thc/(M_PI/4));
	if (ThN < 5)  ThN = 5;
	if (ThN > 30) ThN = 30;



	for (int iTh=0; iTh<ThN; iTh++) {
		const double th = 0.1 * M_PI * double(iTh+1)/(ThN - 1);
		const double _z   = R * cos(th);
		const double rho = R * sin(th);


		for (int i=0; i<phiN; i++) {
			double phi = 2*M_PI * double(i+0.5) / phiN;
			double _x, _y;
			_x = rho * cos(phi);
			_y = rho * sin(phi);

			vector3d r = _x*ex_h + _y*ey_h + _z * ez_h + rp;


			// 惑星座標
			vector3d nv = T * normalize(r - rp);
			if (dot_product(nv, lv_p) <= lv_nv_min) continue;

			vector3d ev_p = T * normalize(re - r);
			if ( dot_product(nv, ev_p) <= 0.0 ) continue;


			// 表示位置
			vector3d re2 = tM * (r - re);	// 視点座標系

			if (re2.z > 0) continue;
			double z = -re2.z;

			double x, y;
			x = re2.x/z * get_screen_z();
			y = re2.y/z * get_screen_z();

			if (x < x1 || x > x2) continue;
			if (y < y1 || y > y2) continue;


			// ハイライトの計算
			float hl = 0.0f;
			const double dp = dot_product(nv, lv_p);
			if ( dp > 0.0 ) {
				const vector3d ev = T * normalize(re - r);
				const vector3d hv = normalize(lv_p + ev);
				const double s = dot_product(hv, nv);
			
				hl = (s <= 0)  ?  0  :  pow(float(s), EarthShininess);
				if (dp < dpc) {
					hl *= sqrt(dp/dpc);
				}
				if (hl < 1e-3)  continue;
			}
			hl_int[vn] = hl;


			add_triangle_xy(x, y);
			vVtx2D[vn].x = x;
			vVtx2D[vn].y = y;

			float u, v;
			get_planet_texture_coord(nv, u, v, 180.0);
			vU[vn] = u;
			vV[vn] = v;

			vn++;
		}
	}


	// 描画点が一つも無い場合はリターン
	if (vn <= 0)  return;

	int nTri = triangulation();
	if (nTri <= 0)  return;




	//-------
	//  描画
	//-------
	Triangle*		vTri = get_triangle_list();
	vector<xlist>   vXlist = get_xlist();


	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0,0, -get_screen_z());


	glDisable(GL_LIGHTING);


	// test
	bool bWire = false;


	if (bWire) {
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);

		glColor3f(0,0.3,0);
		glPointSize(3);
		glBegin(GL_POINTS);
		for (int i=0; i<vn; i++) {
			glVertex2f(vVtx2D[i].x, vVtx2D[i].y);
		}
		glEnd();
	
		glLineWidth(1);
		for (int i=0; i<nTri; i++) {
			const Triangle& tri = vTri[i];
			
			glBegin(GL_LINE_LOOP);
			for (int j=0; j<3; j++) {
				int idx  = tri.p[j];
				glVertex2f(vVtx2D[idx].x, vVtx2D[idx].y);
			}
			glEnd();
		}
	}


	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	glBegin(GL_TRIANGLES);
	for (int i=0; i<nTri; i++) {
		const Triangle& tri = vTri[i];

		// テクスチャの境界を跨ぐかチェックする
		bool bCrossing = false;
		int iLeftSide = 0;
		int iRightSide = 0;
		for (int j=0; j<3; j++) {
			int idx = tri.p[j];
			if (vU[idx] < 0.1) iLeftSide++;
			if (vU[idx] > 0.9) iRightSide++;
		}
		if (iLeftSide > 0 && iRightSide > 0) {
			bCrossing = true;
		}

		// 三角形１つの描画
		for (int j=0; j<3; j++) {
			int idx = tri.p[j];

			RGBf cola = hl_int[idx] * col;
			glColor3fv(cola.get_pointer());

			if (!bCrossing) {
				glTexCoord2f(vU[idx], vV[idx]);
			} else {
				float u = vU[idx];
				if (u < 0.1) u += 1.0;
				glTexCoord2f(u, vV[idx]);
			}
			glVertex2f(vVtx2D[idx].x, vVtx2D[idx].y);

		}
	}
	glEnd();

	glPopMatrix();
}
