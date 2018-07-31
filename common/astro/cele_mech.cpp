/******************************************************************************

Copyright (c) 2007-2014   Tsunehiko Kato

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
#include  "cele_mech.h"



//-----------------------------
//  楕円軌道(e<1)を解く
//   Landau1, (15.10),(15.11)
//-----------------------------
vector3d
calc_elliptical_orbit(const double& a, const double& e, const double& _M)
{
	const int iter_max = 100;
	double M, xi, dxi;

	M = _M;
	if (M > M_PI) { M -= 2.0*M_PI; }

	xi = M;	// first guess

	for (int i=0; i<iter_max; ++i) {
		double cs, sn;
		cs = cos(xi);
		sn = sin(xi);

		dxi = (xi - e*sn - M) / (1.0 - e*cs);
		xi -= dxi;
		
		if (fabs(dxi) < 1e-8) {
			// 軌道座標系での座標を求める
			double x,y;
			x = a * (cs - e);
			y = a * sqrt(1.0-e*e) * sn;
			return  vector3d(x, y, 0.0);
		}
	}

	return  vector3d(0,0,0);	// 収束しない場合
}


//-----------------------
//  双曲線軌道(e>1)を解く
//   Landau1, (15.12)
//-----------------------
vector3d
calc_hyperbolic_orbit(const double& a, const double& e, const double& _M)
{
	const int iter_max = 1000;
	double M, xi, dxi;
	double xi1, xi2, f1, f2, xim, fm;

	M = _M;
	
	xi1 = -10;
	xi2 = 10;
	f1 = e*sinh(xi1) - xi1 - M;
	f2 = e*sinh(xi2) - xi2 - M;
	for (int i=0; i<10; i++) {
		xim = 0.5 * (xi1 + xi2);
		fm = e*sinh(xim) - xim - M;
		if (f1*fm>0) {
			xi1 = xim;
			f1 = fm;
		} else {
			xi2 = xim;
			f2 = fm;
		}
	}
	xi = 0.5 * (xi1 + xi2);

	//xi = M/e;	// first guess
	for (int i=0; i<iter_max; ++i) {
		double csh, snh;
		csh = cosh(xi);
		snh = sinh(xi);

		dxi = (e*snh - xi - M) / (e*csh - 1.0);
		xi -= dxi;
		
		if (fabs(dxi) < 1e-10) {
			// 軌道座標系での座標を求める
			double x, y;
			x = a * (e - csh);
			y = a * sqrt(e*e - 1.0) * snh;
			return  vector3d(x, y, 0.0);
		}
	}

	return  vector3d(0,0,0);	// 収束しない場合
}


// 斥力の場合
vector3d
calc_hyperbolic_orbit2(const double& a, const double& e, const double& _M)
{
	const int iter_max = 1000;
	double M, xi, dxi;
	double xi1, xi2, f1, f2, xim, fm;

	M = _M;
	
	
	xi1 = -10;
	xi2 = 10;
	f1 = e*sinh(xi1) - xi1 - M;
	f2 = e*sinh(xi2) - xi2 - M;
	for (int i=0; i<20; i++) {
		xim = 0.5 * (xi1 + xi2);
		fm = e*sinh(xim) - xim - M;
		if (f1*fm>0) {
			xi1 = xim;
			f1 = fm;
		} else {
			xi2 = xim;
			f2 = fm;
		}
	}
	xi = 0.5 * (xi1 + xi2);

	//xi = M/e;	// first guess
	for (int i=0; i<iter_max; ++i) {
		double csh, snh;
		csh = cosh(xi);
		snh = sinh(xi);

		dxi = (e*snh + xi - M) / (e*csh + 1.0);
		xi -= dxi;
		
		if (fabs(dxi) < 1e-10) {
			// 軌道座標系での座標を求める
			double x, y;
			x = a * (e + csh);
			y = a * sqrt(e*e - 1.0) * snh;
			return  vector3d(x, y, 0.0);
		}
	}

	return  vector3d(0,0,0);	// 収束しない場合
}




//----------------------------------------------
//  軌道上の位置を求める（軌道座標系：焦点が原点）
//    a:  軌道長半径
//    e:  離心率
//    M0: 平均近点離角（元期、度）
//    n:  平均運動（度、単位時間あたり）
//    epoch: 元期
//    t:  位置を求める時刻
//----------------------------------------------
vector3d
calc_orbital_pos(const double& a, const double& e, const double& M0, const double& n, const JD& epoch, const JD& t)
{
	double M, M_rad;
	M = M0 + n*(t - epoch);		// 平均近点離角（現在）
	M_rad = M*D2R;

	if (0<=e && e<1){ return  calc_elliptical_orbit( a, e, M_rad);  }		// 楕円軌道
	else if (e>1)	{ return  calc_hyperbolic_orbit( a, e, M_rad);  }		// 双曲軌道
	else if (e<-1)	{ return  calc_hyperbolic_orbit2(a,-e, M_rad); }		// 双曲軌道（斥力）
	else			{ return  vector3d(0,0,0);					   }		// !!!  放物線は未対応  !!!
}





//---------------------------------
//  軌道座標系での速度ベクトルを求める
//---------------------------------
vector3d
calc_orbital_velocity(const vector3d& pos, const double& a, const double& e, const double& n)
{
	double x, y, r, lmd, rtGM, vth, vx, vy;

	x = pos.x;
	y = pos.y;
	r = sqrt(x*x + y*y);

	if (fabs(e)>1) {
		lmd = a*(e*e - 1.0);
	} else {
		lmd = a*(1.0 - e*e);
	}

	rtGM = D2R * n * sqrt(a*a*a);
	vth = rtGM * sqrt(lmd) / r;

	vx = vth*y/r*(e*x/lmd - 1.0);
	vy = vth/r*(e*y*y/lmd + x);

	return  vector3d(vx, vy, 0);
}