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
//--------------------------
//  Quaternion
//    written by T.N.Kato
//    2002/12/11, 2003/1/2
//--------------------------
#ifndef  __QUATERNION_H__
#define  __QUATERNION_H__
#include  <math.h>
#include  "vector3.h"


//=====================
//  class  quaternion
//=====================
template  <class T>
class  quaternion
{
public:
	T			w;
	vector3<T>	v;

public:
	quaternion()	{};
	quaternion(T _w, T vx, T vy, T vz) : w(_w), v(vx,vy,vz)	{};
	quaternion(T _w, const vector3<T>& _v) : w(_w), v(_v)	{};
	quaternion(const quaternion<T>& q) : w(q.w), v(q.v)		{};
	~quaternion() {};
	template <class T2>
	quaternion(const quaternion<T2>& q) { w=T(q.w); v=vector3<T>(q.v); }

	void	set(const T& _w, const T& _vx, const T& _vy, const T& _vz) { w=_w; v.set(_vx, _vy, _vz); }
	quaternion&	operator=(const quaternion<T>& q)			{ w=q.w; v=q.v; return *this; }
	quaternion&	operator=(const T& _w)						{ w=_w; v=vector3<T>(0,0,0); return *this; }

	//-- operators --
	quaternion	operator- () const							{ return  quaternion<T>(-w, -v.x, -v.y, -v.z); }
	quaternion&	operator+=(const quaternion<T>& q)			{ w+=q.w; v+=q.v; return *this; }
	quaternion&	operator-=(const quaternion<T>& q)			{ w-=q.w; v-=q.v; return *this; }
	quaternion&	operator*=(const quaternion<T>& q);
	quaternion&	operator/=(const quaternion<T>& q)			{ *this *= q.inv(); return *this; } 
	quaternion	operator+(const quaternion<T>& q) const		{ return quaternion<T>(w+q.w, v+q.v); }
	quaternion	operator-(const quaternion<T>& q) const		{ return quaternion<T>(w-q.w, v-q.v); };
	quaternion	operator*(const quaternion<T>& q) const;
	quaternion	operator/(const quaternion<T>& q) const		{ return *this * q.inv(); }
	quaternion& operator*=(const T& c)						{ w*=c; v*=c; return *this; }
	quaternion& operator/=(const T& c)						{ w/=c; v/=c; return *this; }
	quaternion  operator*(const T& c) const					{ return quaternion<T>(w*c, v*c); }
	quaternion  operator/(const T& c) const					{ return quaternion<T>(w/c, v/c); }

	quaternion	conjg() const	{ return  quaternion<T>(w, -v); }
	T			norm2() const	{ return  w*w + v.norm2(); }
	T			norm() const	{ return  T(::sqrt( double(norm2()) )); }
	quaternion	inv() const		{ return  quaternion<T>(conjg() / norm2()); }
	void		normalize()		{ T _norm = norm(); w /= _norm; v /= _norm; }

	T			dot_product  (const quaternion<T>& q) const		{ return  (w*q.w + ::dot_product(v, q.v)); }
};


//-------------
//  operators
//-------------
template  <class T>
quaternion<T>&
quaternion<T>::operator*=(const quaternion<T>& q)
{
	T W = w;
	w = W*q.w - ::dot_product(v, q.v);
	v = W*q.v + q.w*v + cross_product(v, q.v);
	return  *this;
}

template  <class T>
quaternion<T>
quaternion<T>::operator*(const quaternion<T>& q) const
{
	return  quaternion<T>( w*q.w - ::dot_product(v,q.v), w*q.v + q.w*v + cross_product(v, q.v));
}


//-----------
//  typedef
//-----------
typedef  quaternion<float>   quaternionf;
typedef  quaternion<double>  quaterniond;

//  functions
template<class T>		quaternion<T>	operator*(const T& c, const quaternion<T>& q)		{ return q*c; }
template<class T>	T	dot_product(const quaternion<T>& q1, const quaternion<T>& q2)		{ return q1.dot_product(q2);   }


//------------------------------------------
//  functions (quaternion <-> axis, angle)
//------------------------------------------
template  <class T>
quaternion<T>
make_rotation_quaternion(const T& angle, const vector3<T>& axisv)
{
	return  quaternion<T>( T(cos(0.5*angle)), T(sin(0.5*angle)) * axisv );
}

template  <class T>
T
get_rotation_angle(const quaternion<T>& q)
{
	return 2.0 * acos(q.w);
}

template  <class T>
vector3<T>
get_rotation_axis(const quaternion<T>& q)
{
	return  q.v.normalized();
}


//--------------------------------------------
//  functions (quaternion <-> axes of frame)
//--------------------------------------------
template  <class T>
quaternion<T>
axes_to_quaternion(const vector3<T>& eForward, const vector3<T>& eUp)
{
	vector3<T>  eRight(cross_product(eForward, eUp));

	T  M00, M01, M02, M10, M11, M12, M20, M21, M22, TrA;
	M00 = eRight.x;
	M10 = eRight.y;
	M20 = eRight.z;
	M01 = eUp.x;
	M11 = eUp.y;
	M21 = eUp.z;
	M02 = -eForward.x;
	M12 = -eForward.y;
	M22 = -eForward.z;
	TrA = M00 + M11 + M22;

	T  aa, a, b, c, d;
	bool bBC, bCD, bBD;
	aa  = T(0.25) * (T(1.0) + TrA);
	bBC = (M00 >= M11);
	bBD = (M00 >= M22);
	bCD = (M11 >= M22);

	if (aa >= T(0.25)) {
		//-- |a| is the largest --
		a = T( sqrt(aa) );			// choose positive root

		const T f = T(0.25/a);
		b = f * (M21-M12);
		c = f * (M02-M20);
		d = f * (M10-M01);
	} else if (bBC && bBD) {
		//-- |b| is the largest --
		b = T( 0.5 * sqrt(1.0 - TrA + 2.0*M00) );
		if (M21-M12 < T(0))  b = -b;

		const T f = T(0.25/b);
		a = f * (M21-M12);
		c = f * (M10+M01);
		d = f * (M02+M20);
	} else if (bCD) {
		//-- |c| is the largest --
		c = T( 0.5 * sqrt(1.0 - TrA + 2.0*M11) );
		if (M02-M20 < T(0))  c = -c;
		
		const T f = T(0.25/c);
		a = f * (M02-M20);
		b = f * (M10+M01);
		d = f * (M21+M12);
	} else {
		//-- |d| is the largest --
		d = T( 0.5 * sqrt(1.0 - TrA + 2.0*M22) );
		if (M10-M01 < T(0))  d = -d;

		const T f = T(0.25/d);
		a = f * (M10-M01);
		b = f * (M02+M20);
		c = f * (M21+M12);
	}

	return  quaternion<T>(a, b, c, d);
}



//-- 対応する直交座標の基底を得る
template  <class T>
void
quaternion_to_axes(const quaternion<T>& q, vector3<T>& eForward, vector3<T>& eUp, vector3<T>& eRight)
{
	quaternion<T>  qn(q);
	qn.normalize();

	T  wn, xn, yn, zn, xx, yy, zz, xy, yz, zx, wx, wy, wz;
	wn = qn.w;
	xn = qn.v.x;
	yn = qn.v.y;
	zn = qn.v.z;
	xx = xn*xn;
	yy = yn*yn;
	zz = zn*zn;
	xy = xn*yn;
	yz = yn*zn;
	zx = zn*xn;
	wx = wn*xn;
	wy = wn*yn;
	wz = wn*zn;

	eRight.set( T(1.0) - T(2.0) * (yy + zz), T(2.0) * (xy + wz), T(2.0) * (zx - wy));
	eUp.set( T(2.0) * (xy - wz), ( T(1.0) - T(2.0) * (xx + zz)), T(2.0) * (yz + wx) );
	eForward.set( -T(2.0) * (zx + wy), -T(2.0) * (yz - wx), -T(1.0) + T(2.0) * (xx + yy) );
}


//-- 正面ベクトルのみを得る
template  <class T>
vector3<T>
quaternion_to_eF(const quaternion<T>& q)
{
	quaternion<T>  qn(q);
	qn.normalize();

	T  wn, xn, yn, zn, xx, yy, yz, zx, wx, wy;
	wn = qn.w;
	xn = qn.v.x;
	yn = qn.v.y;
	zn = qn.v.z;
	xx = xn*xn;
	yy = yn*yn;
	yz = yn*zn;
	zx = zn*xn;
	wx = wn*xn;
	wy = wn*yn;

	return  vector3<T>( -T(2.0) * (zx + wy), -T(2.0) * (yz - wx), -T(1.0) + T(2.0) * (xx + yy) );
}


//------------------------------------
//  functions (quaternion <-> SO(3))
//------------------------------------
template  <class T>
matrix3<T>
quaternion_to_matrix3(const quaternion<T>& q0)
{
	matrix3<T>  M;
	quaternion<T>  q(q0);
	q.normalize();

	T  wn, xn, yn, zn, xx, yy, zz, xy, yz, zx, wx, wy, wz;
	wn = q.w;
	xn = q.v.x;
	yn = q.v.y;
	zn = q.v.z;
	xx = xn*xn;
	yy = yn*yn;
	zz = zn*zn;
	xy = xn*yn;
	yz = yn*zn;
	zx = zn*xn;
	wx = wn*xn;
	wy = wn*yn;
	wz = wn*zn;

	M.m11 = 1.0 - 2.0 * (yy + zz);
	M.m12 = 2.0 * (xy - wz);
	M.m13 = 2.0 * (zx + wy);
	M.m21 = 2.0 * (xy + wz);
	M.m22 = 1.0 - 2.0 * (xx + zz);
	M.m23 = 2.0 * (yz - wx);
	M.m31 = 2.0 * (zx - wy);
	M.m32 = 2.0 * (yz + wx);
	M.m33 = 1.0 - 2.0 * (xx + yy);

	return  M;
}


template  <class T>
void
quaternion_to_matrix_GL(const quaternion<T>& q0, T *M)
{
	quaternion<T>  q(q0);
	q.normalize();

	T  wn, xn, yn, zn, xx, yy, zz, xy, yz, zx, wx, wy, wz;
	wn = q.w;
	xn = q.v.x;
	yn = q.v.y;
	zn = q.v.z;
	xx = xn*xn;
	yy = yn*yn;
	zz = zn*zn;
	xy = xn*yn;
	yz = yn*zn;
	zx = zn*xn;
	wx = wn*xn;
	wy = wn*yn;
	wz = wn*zn;

	M[0]  =  1.0 - 2.0 * (yy + zz);
	M[1]  =  2.0 * (xy + wz);
	M[2]  =  2.0 * (zx - wy);
	M[3]  =  0.0;
	M[4]  =  2.0 * (xy - wz);
	M[5]  =  1.0 - 2.0 * (xx + zz);
	M[6]  =  2.0 * (yz + wx);
	M[7]  = 0.0;
	M[8]  =  2.0 * (zx + wy);
	M[9]  = 2.0 * (yz - wx);
	M[10] = 1.0 - 2.0 * (xx + yy);
	M[11] = 0.0;
	M[12] = 0.0;
	M[13] = 0.0;
	M[14] = 0.0;
	M[15] = 1.0;
}

template  <class T>
void
quaternion_to_matrix_GL_tr(const quaternion<T>& q0, T *M)
{
	quaternion<T>  q(q0);
	q.normalize();

	T  wn, xn, yn, zn, xx, yy, zz, xy, yz, zx, wx, wy, wz;
	wn = q.w;
	xn = q.v.x;
	yn = q.v.y;
	zn = q.v.z;
	xx = xn*xn;
	yy = yn*yn;
	zz = zn*zn;
	xy = xn*yn;
	yz = yn*zn;
	zx = zn*xn;
	wx = wn*xn;
	wy = wn*yn;
	wz = wn*zn;

	M[0]  = 1.0 - 2.0 * (yy + zz);
	M[1]  = 2.0 * (xy - wz);
	M[2]  = 2.0 * (zx + wy);
	M[3]  = 0.0;
	M[4]  = 2.0 * (xy + wz);
	M[5]  = 1.0 - 2.0 * (xx + zz);
	M[6]  = 2.0 * (yz - wx);
	M[7]  = 0.0;
	M[8]  = 2.0 * (zx - wy);
	M[9]  = 2.0 * (yz + wx);
	M[10] = 1.0 - 2.0 * (xx + yy);
	M[11] = 0.0;
	M[12] = 0.0;
	M[13] = 0.0;
	M[14] = 0.0;
	M[15] = 1.0;
}


template  <class T>
quaternion<T>
matrix3_to_quaternion(const matrix3<T>& M)
{
	return  axes_to_quaternion( vector3<T>(M.m13, M.m23, M.m33), vector3<T>(-M.m12, -M.m22, -M.m32) );
}


//--------------
//  球面線形補間
//--------------
template  <class T>
quaternion<T>
slerp(const quaternion<T>& q1, const quaternion<T>& q2, const T& t)
{
	double dp = dot_product(q1,q2);

	quaternion<T> _q2 = q2;
	if (dp < 0) { _q2*=-1; dp=-dp; }
	if (dp > 1.0)  dp = 1.0;

	double th = acos(dp);
	if (fabs(th) < 1e-7) {
		return  T(1.0-t)*q1 + T(t)*_q2;
	} else {
		return  (T(sin(th*(1.0-t)))*q1 + T(sin(th*t))*_q2) / T(sin(th));
	}
}


#endif
