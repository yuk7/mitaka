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
#include  "color.h"
#include  <assert.h>



//-----------------------------------------------
// CIE 1931 XYZ Color Matching Function (2-deg)
//  (JIS Z 8701)
//-----------------------------------------------
// total = 81
// wave length: 380.0nm - 780.0nm
const int	XYZ_CMF2deg_num = 81;
const float	XYZ_CMF2deg_dlmd = 5.0f;

//-- lambda (in nm) --
const float XYZ_CMF2deg_lmd[] =
{
	380.0f, 385.0f, 390.0f, 395.0f, 400.0f, 405.0f, 410.0f, 415.0f,
	420.0f, 425.0f, 430.0f, 435.0f, 440.0f, 445.0f, 450.0f, 455.0f,
	460.0f, 465.0f, 470.0f, 475.0f, 480.0f, 485.0f, 490.0f, 495.0f,
	500.0f, 505.0f, 510.0f, 515.0f, 520.0f, 525.0f, 530.0f, 535.0f,
	540.0f, 545.0f, 550.0f, 555.0f, 560.0f, 565.0f, 570.0f, 575.0f,
	580.0f, 585.0f, 590.0f, 595.0f, 600.0f, 605.0f, 610.0f, 615.0f,
	620.0f, 625.0f, 630.0f, 635.0f, 640.0f, 645.0f, 650.0f, 655.0f,
	660.0f, 665.0f, 670.0f, 675.0f, 680.0f, 685.0f, 690.0f, 695.0f,
	700.0f, 705.0f, 710.0f, 715.0f, 720.0f, 725.0f, 730.0f, 735.0f,
	740.0f, 745.0f, 750.0f, 755.0f, 760.0f, 765.0f, 770.0f, 775.0f,
	780.0f
};

//-- Color matching function (2-deg)
const vector3f XYZ_CMF2deg[] = 
{
	vector3f( 0.0014f, 0.0000f, 0.0065f ),
	vector3f( 0.0022f, 0.0001f, 0.0105f ),
	vector3f( 0.0042f, 0.0001f, 0.0201f ),
	vector3f( 0.0076f, 0.0002f, 0.0362f ),
	vector3f( 0.0143f, 0.0004f, 0.0679f ),
	vector3f( 0.0232f, 0.0006f, 0.1102f ),
	vector3f( 0.0435f, 0.0012f, 0.2074f ),
	vector3f( 0.0776f, 0.0022f, 0.3713f ),
	vector3f( 0.1344f, 0.0040f, 0.6456f ),
	vector3f( 0.2148f, 0.0073f, 1.0391f ),
	vector3f( 0.2839f, 0.0116f, 1.3856f ),
	vector3f( 0.3285f, 0.0168f, 1.6230f ),
	vector3f( 0.3483f, 0.0230f, 1.7471f ),
	vector3f( 0.3481f, 0.0298f, 1.7826f ),
	vector3f( 0.3362f, 0.0380f, 1.7721f ),
	vector3f( 0.3187f, 0.0480f, 1.7441f ),
	vector3f( 0.2908f, 0.0600f, 1.6692f ),
	vector3f( 0.2511f, 0.0739f, 1.5281f ),
	vector3f( 0.1954f, 0.0910f, 1.2876f ),
	vector3f( 0.1421f, 0.1126f, 1.0419f ),
	vector3f( 0.0956f, 0.1390f, 0.8130f ),
	vector3f( 0.0580f, 0.1693f, 0.6162f ),
	vector3f( 0.0320f, 0.2080f, 0.4652f ),
	vector3f( 0.0147f, 0.2586f, 0.3533f ),
	vector3f( 0.0049f, 0.3230f, 0.2720f ),
	vector3f( 0.0024f, 0.4073f, 0.2123f ),
	vector3f( 0.0093f, 0.5030f, 0.1582f ),
	vector3f( 0.0291f, 0.6082f, 0.1117f ),
	vector3f( 0.0633f, 0.7100f, 0.0782f ),
	vector3f( 0.1096f, 0.7932f, 0.0573f ),
	vector3f( 0.1655f, 0.8620f, 0.0422f ),
	vector3f( 0.2257f, 0.9149f, 0.0298f ),
	vector3f( 0.2904f, 0.9540f, 0.0203f ),
	vector3f( 0.3597f, 0.9803f, 0.0134f ),
	vector3f( 0.4334f, 0.9950f, 0.0087f ),
	vector3f( 0.5121f, 1.0000f, 0.0057f ),
	vector3f( 0.5945f, 0.9950f, 0.0039f ),
	vector3f( 0.6784f, 0.9786f, 0.0027f ),
	vector3f( 0.7621f, 0.9520f, 0.0021f ),
	vector3f( 0.8425f, 0.9154f, 0.0018f ),
	vector3f( 0.9163f, 0.8700f, 0.0017f ),
	vector3f( 0.9786f, 0.8163f, 0.0014f ),
	vector3f( 1.0263f, 0.7570f, 0.0011f ),
	vector3f( 1.0567f, 0.6949f, 0.0010f ),
	vector3f( 1.0622f, 0.6310f, 0.0008f ),
	vector3f( 1.0456f, 0.5668f, 0.0006f ),
	vector3f( 1.0026f, 0.5030f, 0.0003f ),
	vector3f( 0.9384f, 0.4412f, 0.0002f ),
	vector3f( 0.8544f, 0.3810f, 0.0002f ),
	vector3f( 0.7514f, 0.3210f, 0.0001f ),
	vector3f( 0.6424f, 0.2650f, 0.0000f ),
	vector3f( 0.5419f, 0.2170f, 0.0000f ),
	vector3f( 0.4479f, 0.1750f, 0.0000f ),
	vector3f( 0.3608f, 0.1382f, 0.0000f ),
	vector3f( 0.2835f, 0.1070f, 0.0000f ),
	vector3f( 0.2187f, 0.0816f, 0.0000f ),
	vector3f( 0.1649f, 0.0610f, 0.0000f ),
	vector3f( 0.1212f, 0.0446f, 0.0000f ),
	vector3f( 0.0874f, 0.0320f, 0.0000f ),
	vector3f( 0.0636f, 0.0232f, 0.0000f ),
	vector3f( 0.0468f, 0.0170f, 0.0000f ),
	vector3f( 0.0329f, 0.0119f, 0.0000f ),
	vector3f( 0.0227f, 0.0082f, 0.0000f ),
	vector3f( 0.0158f, 0.0057f, 0.0000f ),
	vector3f( 0.0114f, 0.0041f, 0.0000f ),
	vector3f( 0.0081f, 0.0029f, 0.0000f ),
	vector3f( 0.0058f, 0.0021f, 0.0000f ),
	vector3f( 0.0041f, 0.0015f, 0.0000f ),
	vector3f( 0.0029f, 0.0010f, 0.0000f ),
	vector3f( 0.0020f, 0.0007f, 0.0000f ),
	vector3f( 0.0014f, 0.0005f, 0.0000f ),
	vector3f( 0.0010f, 0.0004f, 0.0000f ),
	vector3f( 0.0007f, 0.0002f, 0.0000f ),
	vector3f( 0.0005f, 0.0002f, 0.0000f ),
	vector3f( 0.0003f, 0.0001f, 0.0000f ),
	vector3f( 0.0002f, 0.0001f, 0.0000f ),
	vector3f( 0.0002f, 0.0001f, 0.0000f ),
	vector3f( 0.0001f, 0.0000f, 0.0000f ),
	vector3f( 0.0001f, 0.0000f, 0.0000f ),
	vector3f( 0.0001f, 0.0000f, 0.0000f ),
	vector3f( 0.0000f, 0.0000f, 0.0000f )
};


XYZf
get_XYZ_value(const float& lmd)
{
	if (lmd < XYZ_CMF2deg_lmd[0] || lmd > XYZ_CMF2deg_lmd[XYZ_CMF2deg_num-1]) {
		return  XYZf(0,0,0);
	}

	float t,s, lmd1, lmd2;
	int idx;
	idx = int( (lmd-XYZ_CMF2deg_lmd[0]) / XYZ_CMF2deg_dlmd );
	lmd1 = XYZ_CMF2deg_lmd[idx];
	lmd2 = XYZ_CMF2deg_lmd[idx+1];
	assert(idx+1 < XYZ_CMF2deg_num);

	t = (lmd - lmd1) / XYZ_CMF2deg_dlmd;
	s = 1.0f - t;
	assert(t>=0 && t<=1);
	assert(s>=0 && s<=1);

	return	s * XYZ_CMF2deg[idx] + t * XYZ_CMF2deg[idx+1];
}


// 強度関数 f(lmd) の色を計算する
XYZf
make_XYZ_color(float f(const float& lmd))
{
	XYZf  xyz(0,0,0);
	for (int i=0; i<XYZ_CMF2deg_num; ++i) {
		float val = f( XYZ_CMF2deg_lmd[i] );

		xyz += val * XYZ_CMF2deg_dlmd * XYZ_CMF2deg[i];
	}
	return  xyz;
}


//------------
//  ガンマ補正
//------------
static const int gm_N = 5000;
static float gm_cr[gm_N+1];
static bool init_gamma = false;

inline float
correct_gamma_0(const float& val)
{
	//--- make gamma correction table ---
	if (!init_gamma) {
		for (int i=0; i<=gm_N; ++i) {
			float lv = float(i)/gm_N;

			if (lv <= 0.0031308f)	{ lv = 12.92f * lv; }
			else					{ lv = 1.055f * pow(lv, 1.0f/2.4f) - 0.055f; }
			gm_cr[i] = lv;
		}
		init_gamma = true;
	}

	int i;
	i  = int(val * gm_N);
	if (i < 0) i=0; else if (i > gm_N)  i=gm_N;

	return  gm_cr[i];
}

float
correct_gamma(const float& val)
{
	return  correct_gamma_0(val);
}


RGBf
correct_RGB0(const RGBf& rgb0)
{
	RGBf rgbcr;
	rgbcr.r = correct_gamma_0( rgb0.r );
	rgbcr.g = correct_gamma_0( rgb0.g );
	rgbcr.b = correct_gamma_0( rgb0.b );
	return  rgbcr;
}


RGBf
correct_RGB(const RGBf& rgb)
{
	return correct_RGB0(rgb);
}


//-----------
//  黒体の色
//-----------
float
PlanckLaw(const float& T, const float& lmd_nm)
{
	const float h  = 6.62606876e-27;	// Planck (erg s)
	const float c  = 2.99792458e10;		// speed of light (cm/s)
	const float kB = 1.3806503e-16;		// Boltzmann (erg/K)
	const float C  = 2.0f*h*c*c;
	const float C2 = h*c/kB;
	float lmd  = lmd_nm * 1e-7;
	float lmd2 = lmd*lmd;
	float lmd5 = lmd2*lmd2 * lmd;

	float val = C/lmd5/(exp(C2/(lmd*T)) - 1.0f);
	return val;
}


static float g_T = 5000;

float
Planck(const float& lmd_nm)
{
	return  PlanckLaw(g_T, lmd_nm);
}

XYZf
blackbody_color_XYZ(const float T)
{
	XYZf XYZ;
	g_T = T;
	XYZ = make_XYZ_color(Planck);
	XYZ.x /= XYZ.y;
	XYZ.z /= XYZ.y;
	XYZ.y = 1;
	return XYZ;
}


float
bb_max_level(const float T1, const float T2)
{
	const int N = 10000;
	const float dT = (T2-T1)/N;
	float lv = 0;

	for (int i=0; i<N; i++) {
		float T = i * dT + T1;
		RGBf rgb = blackbody_color(T);
		float mlv = max_level(rgb);
		if (mlv > lv)  lv = mlv;
	}
	return lv;
}
