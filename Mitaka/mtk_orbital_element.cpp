/**************************************************************************************************

Copyright (c) 2007, 2014   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan
Copyright (c) 2013   Tsunehiko Kato

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
#include  "mtk_orbital_element.h"
#include  "../common/astro/cele_mech.h"
#include  "../common/astro/astro_const.h"






orbital_elements::orbital_elements(const double& epoch, const double& a, const double& e, const double& M0, const double& n,
					 const double& node,  const double& i, const double& omg)
{
	set_parameters(epoch, a, e, M0, n, node, i, omg);
}


void
orbital_elements::set_parameters(const double& epoch, const double& a, const double& e, const double& _M0, const double& n,
								 const double& node,  const double& i, const double& omg)
{
	double M0;
	
	M0 = _M0;
	if (fabs(e) < 1 && M0 > 180.0) { M0 -= 360.0; }

	m_epoch	= epoch;
	m_a		= a;
	m_e		= e;
	m_M0	= M0;
	m_n		= n;
	
	// 変換行列
	m_MT = Rzxz3(D2R*node, D2R*i, D2R*omg);
}


// 指定した時刻での位置
vector3d
orbital_elements::calc_pos(const double& t)
{
	return  m_MT * calc_orbital_pos(m_a, m_e, m_M0, m_n, m_epoch, t);
}


// 指定した時刻での位置と速度
void
orbital_elements::calc_pos_and_velocity(const double& t, vector3d &pos, vector3d &vel)
{
	vector3d x, v;

	x = calc_orbital_pos(m_a, m_e, m_M0, m_n, m_epoch, t);
	v = calc_orbital_velocity(x, m_a, m_e, m_n);

	pos = m_MT * x;
	vel = m_MT * v;
}


// 指定した時刻での速度
vector3d
orbital_elements::calc_velocity(const double& t)
{
	vector3d x;

	x = calc_orbital_pos(m_a, m_e, m_M0, m_n, m_epoch, t);

	return  m_MT * calc_orbital_velocity(x, m_a, m_e, m_n);
}


void
orbital_elements::update_pos_and_velocity(const double& t)
{
	calc_pos_and_velocity(t, m_pos, m_vel);
}



// 軌道上の位置
vector3d
orbital_elements::calc_orbit_pos(const double& phi) const
{
	return  m_MT * ::calc_orbital_position(m_a, m_e, phi);
}



// 位置と速度から軌道要素を計算する
void
set_orbital_elements(vector3d x, vector3d v, double t0, double mu, orbital_elements &oe)
{
	vector3d  er, eth, vperp;
	double    r, vr, vth, lmd, alp, bet, e, phi, a, n, u, M0;
	double GM;
	matrix3d A;

	GM = 2.95912208e-4 * mu;

	er = normalize(x);
	vr = dot_product(v, er);
	vperp = v - vr*er;
	vth = norm(vperp);
	eth = normalize(vperp);

	r = norm(x) / _1AU_pc;	// in AU
	vr /= _1AU_pc;
	vth /= _1AU_pc;

	lmd = r*r*vth*vth / fabs(GM);
	alp = lmd / r;
	bet = vr / vth;


	if (GM >= 0) {
		e = sqrt(alp*alp*bet*bet + (alp - 1)*(alp - 1));
		phi = atan2(alp*bet, alp - 1);
	}
	else {
		e = sqrt(alp*alp*bet*bet + (alp + 1)*(alp + 1));
		phi = atan2(alp*bet, alp + 1);
	}


	if (e > 1.0) {
		double z;
		a = lmd / (e*e - 1.0);
		z = r*sin(phi) / (a*sqrt(e*e - 1));
		u = log(z + sqrt(z*z + 1));
		if (GM >= 0) {
			M0 = e*sinh(u) - u;
		}
		else {
			M0 = e*sinh(u) + u;
		}
	}
	else {
		a = lmd / (1.0 - e*e);
		u = atan2(r / (a*sqrt(1 - e*e)) * sin(phi), r*cos(phi) / a + e);
		M0 = u - e*sin(u);
	}
	n = sqrt(fabs(GM) / (a*a*a)) * 180 / M_PI;

	if (GM<0) { e = -e; }
	oe.set_parameters(t0, a, e, M0*R2D, n, 0, 0, -phi*R2D);

	A.set(er.x, eth.x, 0, er.y, eth.y, 0, er.z, eth.z, 0);
	oe.mult_MT(A);
}
