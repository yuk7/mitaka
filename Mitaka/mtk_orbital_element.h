/**************************************************************************************************

Copyright (c) 2007   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan
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
#ifndef __MTK_ORBITAL_ELEMENT_H__
#define __MTK_ORBITAL_ELEMENT_H__
#include  "../common/math/vector3.h"




//  軌道上の位置
inline  vector3d
calc_orbital_position(const double& a, const double& e, const double& phi)
{
	double x, y, b;
	x = cos(phi);
	y = sin(phi);
	b = a * sqrt(1.0-e*e);

	double xx, yy;
	xx = a*x - a*e;
	yy = b*y;

	return  vector3d(xx, yy, 0);
}



//--------------------
//  orbital elements
//--------------------
class orbital_elements
{
	double	m_epoch;	// epoch (JD)

    double	m_a;		// 軌道長半径(AU)
	double	m_e;		// 軌道離心率
	double	m_M0;		// Mean anomaly; 平均近点角
	double	m_n;		// mean daily motion；平均日々運動

	matrix3d	m_MT;	// 軌道座標から基準座標への変換行列
	vector3d	m_pos;
	vector3d	m_vel;

public:
	orbital_elements() {}
	orbital_elements(const double& epoch, const double& a, const double& e, const double& M0, const double& n,
					 const double& node,  const double& i, const double& omg);

	void	set_parameters(const double& epoch, const double& a, const double& e, const double& M0, const double& n,
						   const double& node,  const double& i, const double& omg);

	vector3d	calc_pos(const double& t);
	void		calc_pos_and_velocity(const double& t, vector3d &pos, vector3d &vel);
	vector3d	calc_velocity(const double& t);
	void		update_pos_and_velocity(const double& t);

	vector3d	calc_orbit_pos(const double& phi) const;

	// accessor
	double		get_epoch() const	{ return m_epoch; }
	double		get_a() const		{ return m_a; }
	double		get_e() const		{ return m_e; }
	double		get_M0() const		{ return m_M0; }
	double		get_n() const		{ return m_n; }
	matrix3d	get_MT() const		{ return m_MT; }
	vector3d	get_pos() const			{ return m_pos; }
	vector3d	get_velocity() const	{ return m_vel; }

	void		mult_MT(const matrix3d& M) { m_MT = M * m_MT; }
};



//
//
//
void	set_orbital_elements(vector3d x, vector3d v, double t0, double mu, orbital_elements &oe);


#endif

