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
#include  "random.h"
#include  <math.h>
#include  <time.h>

// Ramdom number generator
// original: Numerical Recipes in C

// Uniform distribution random number
static int  initFrg = 0;
static long idum = -1;
static long idum2 = 1;
const long IM1 = 2147483563;
const long IM2 = 2147483399;
const double AM = 1.0/IM1;
const long IMM1 = IM1-1;
const long IA1 = 40014;
const long IA2 = 40692;
const long IQ1 = 53668;
const long IQ2 = 52774;
const long IR1 = 12211;
const long IR2 = 3791;
const int  NTAB = 32;
const long NDIV = 1+IMM1/NTAB;
const double EPS = 1.2e-7;
const double RNMX = 1.0 - EPS;
static long iv[NTAB];
static long iy = 0;


//  Initialize
void
rndInit(long seed)
{
	int j;
	long k;

	if (seed < 0)  seed = -seed;
	if (seed == 0) seed = 1;
	
	idum2 = idum = seed;
		
	for (j=NTAB+7; j>=0; j--) {
		k = idum / IQ1;
		idum = IA1*(idum - k*IQ1) - k*IR1;
		if (idum < 0) idum += IM1;
		if (j < NTAB) iv[j] = idum;
	}
	iy = iv[0];

	initFrg = 1;
}

// Initialize the random seed by time
void
rndInit( void )
{
		rndInit( long(time(NULL)) );  
}

// Uniform random number
double
rnd(void)
{
	int j;
	long k;
	double temp;

	if ( !initFrg ) {
		rndInit();
	}
	k = idum / IQ1;
	idum = IA1 * (idum - k*IQ1) - k*IR1;
	if (idum < 0) idum += IM1;
	k = idum2 / IQ2;
	idum2 = IA2 * (idum2 - k*IQ2) -k*IR2;
	if (idum2 < 0) idum2 += IM2;
	j = iy / NDIV;
	iy = iv[j] - idum2;
	iv[j] = idum;
	if (iy < 1) iy += IMM1;

	if ((temp = AM*iy) > RNMX) return RNMX;
	else return temp;
}

// random number uniform distributed between x1 and x2
double
rndUni(double x1, double x2) 
{
	if (x1 == x2) return x1;
	if (x2 < x1) {
		double tmp = x2;
		x2 = x1;
		x1 = tmp;
	}
	return rnd()*(x2-x1) + x1;
}

//  Gaussian random number
double
rndStdGauss() {
	static int sw = 0;
	static double gset;
	double fac, rsq, v1, v2;

	if (sw == 0) {
		sw = 1;
		do {
			v1 = 2.0 * rnd() - 1.0;
			v2 = 2.0 * rnd() - 1.0;
			rsq = v1*v1 + v2*v2;
		} while (rsq >= 1.0 || rsq == 0.0);
		fac = sqrt(-2.0*log(rsq)/rsq);
		gset = v1*fac;
		return v2*fac;
	} else {
		sw = 0;
		return gset;
	}
}

double
rndGauss( double mean, double dev )
{
	return dev * rndStdGauss() + mean;
}


//  Exponential ranbom number
double
rndStdExp( void )
{
	double dum;
	
	do {
		dum = rnd();
	} while (dum == 0.0);
	return -log(dum);
}

double
rndExp( double mean )
{
	return mean * rndStdExp();
}

double
rndExpLtd( double mean, double max )
{
	if (mean==0.0 || max == 0.0) return 0.0;
	if (mean * max < 0.0) return 0.0;
	return - mean * log(1.0 - rnd() * (1.0-exp(-max/mean)) );
}

//--- ガンマ分布 ---
// 整数次の場合
double
rndGammaInt(int ia)
{
	double x;

	if (ia < 1) return 0;	// error
	if (ia < 6) {
		x = 1.0;
		for (int j=0; j<ia; j++)  x *= rnd();
		x = -log(x);
	} else {
		double e;

		do {
			double y, s, am;
			do {
				double v1, v2;
				do {
					v1 = 2.0*rnd() - 1.0;
					v2 = 2.0*rnd() - 1.0;
				} while (v1*v1 + v2*v2 > 1.0);
				y = v2/v1;
				am = ia-1;
				s = sqrt(2.0*am + 1.0);
				x = s*y + am;
			} while (x <= 0.0);

			e = (1.0 + y*y) * exp(am*log(x/am) - s*y);
		} while (rnd() > e);
	}

	return x;
}

// 一般の場合
double
rndGamma(double a)
{
	const double E = 2.718281828459045235;
	double t, u, x, y;

	if (a <= 0.0)  return 0.0;	// error

	if (a > 1.0) {
		t = sqrt(2.0*a - 1.0);
		do {
			do {
				do {
					x = 1.0 - rnd();
					y = 2.0*rnd() - 1.0;
				} while (x*x + y*y > 1.0);
				y /= x;
				x = t*y + a - 1.0;
			} while (x <= 0.0);
			u = (a - 1.0) * log(x / (a-1.0)) - t*y;
		} while (u < -50.0 || rnd() > (1.0 + y*y)*exp(u));
	} else {
		t = E / (a + E);
		do {
			if (rnd() < t) {
				x = pow(rnd(), 1.0/a);
				y = exp(-x);
			} else {
				x = 1.0 - log(rnd());
				y = pow(x, a-1.0);
			}
		} while (rnd() >= y);
	}

	return x;
}
