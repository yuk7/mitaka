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
//-------------------------
//  Vector3
//    written by T.N.Kato
//    2003/1/2
//-------------------------
#ifndef __VECTOR3_H__
#define __VECTOR3_H__
#include  <math.h>
const double D2R(M_PI/180.0);
const double R2D(180.0/M_PI);


///==================
///  class  vector3
///==================
template<class T>
class  vector3
{
public:
	union {
		T elem[3];
		struct { T  x,y,z; };
		struct { T  r,g,b; };
	};
public:
		///   constructor / destructor
	vector3()	{}
	vector3(const T& _x, const T& _y, const T& _z)	: x(_x), y(_y), z(_z)	{}
	template <class T2>
	vector3(const vector3<T2>& v) { x=T(v.x); y=T(v.y); z=T(v.z); }
	void		set(const T& _x, const T& _y, const T& _z)							{ x=_x; y=_y; z=_z; } 
		///   operators
	template <class T2>
	vector3<T>&		operator= (const vector3<T2>& v)
		{ x=T(v.x); y=T(v.y); z=T(v.z); return *this; }
	bool			operator==(const vector3<T>& v) const			{ return  (x==v.x) && (y==v.y) && (z==v.z); }
	vector3<T>&		operator+=(const vector3<T>& v)					{ x+=v.x; y+=v.y; z+=v.z; return *this; }
	vector3<T>&		operator-=(const vector3<T>& v)					{ x-=v.x; y-=v.y; z-=v.z; return *this; }
	vector3<T>		operator+ (const vector3<T>& v) const			{ return  vector3<T>(x+v.x, y+v.y, z+v.z); }
	vector3<T>		operator- (const vector3<T>& v) const			{ return  vector3<T>(x-v.x, y-v.y, z-v.z); }
	vector3<T>		operator- () const								{ return  vector3<T>(-x, -y, -z); }
		//
	vector3<T>&		operator*=(const T& c)							{ x*=c; y*=c; z*=c; return *this; }
	vector3<T>&		operator/=(const T& c)							{ const T ic(T(1)/c); x*=ic; y*=ic; z*=ic; return *this; }
	vector3<T>		operator* (const T& c) const					{ return  vector3<T>(x*c, y*c, z*c); }
	vector3<T>		operator/ (const T& c) const					{ const T ic(T(1)/c); return  vector3<T>(ic*x, ic*y, ic*z); }
		///
	T				norm2() const									{ return  (x*x + y*y + z*z); }
	T				norm()  const									{ return  T(sqrt( norm2() )); }
	vector3<T>		normalized() const								{ const T il(T(1)/norm()); return vector3<T>(il*x, il*y, il*z); } 
		///
	T				dot_product  (const vector3<T>& v) const		{ return  (x*v.x + y*v.y + z*v.z); }
	vector3<T>		cross_product(const vector3<T>& v) const		{ return  vector3<T>(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }
	vector3<T>		mul_each     (const vector3<T>& v) const		{ return  vector3<T>(x*v.x, y*v.y, z*v.z); }
	vector3<T>		div_each     (const vector3<T>& v) const		{ return  vector3<T>(x/v.x, y/v.y, z/v.z); }
		///
	T*				get_pointer()									{ return (T*)this; }
	const T* const	get_pointer() const								{ return (T*)this; }
};


//-- special version
typedef		vector3<double>		vector3d;
typedef		vector3<float>		vector3f;


//-- functions
template<class T>		vector3<T>		operator*(const T& c, const vector3<T>& v)		{ return v*c; }
template<class T>		vector3<T>		normalize(const vector3<T>& v)					{ return v.normalized(); }
template<class T>		T				norm2(const vector3<T>& v)						{ return v.norm2();   }
template<class T>		T				norm(const vector3<T>& v)						{ return v.norm();   }
template<class T>	T				dot_product  (const vector3<T>& v1, const vector3<T>& v2)	{ return v1.dot_product(v2);   }
template<class T>	vector3<T>		cross_product(const vector3<T>& v1, const vector3<T>& v2)	{ return v1.cross_product(v2); }
template<class T>	vector3<T>		mul_each(const vector3<T>& v1, const vector3<T>& v2)		{ return v1.mul_each(v2); }
template<class T>	vector3<T>		div_each(const vector3<T>& v1, const vector3<T>& v2)		{ return v1.div_each(v2); }

//-- 垂直な１つの単位ベクトルを得る
template<class T>  vector3<T>
get_a_normal_unit_vector(const vector3<T>& v)
{
	vector3<T> nx(T(1), T(0), T(0));
	vector3<T> ny(T(0), T(1), T(0));
	vector3<T> nz(T(0), T(0), T(1));
	T dx, dy, dz;
	dx = fabs( dot_product(nx, v) );
	dy = fabs( dot_product(ny, v) );
	dz = fabs( dot_product(nz, v) );
	if (dx <= dy && dx <= dz)	return normalize( cross_product(v, nx) );
	if (dy <= dz)				return normalize( cross_product(v, ny) );
	return normalize( cross_product(v, nz) );
}



///==================
///  class  vector2
///==================
template<class T>
class  vector2
{
public:
	T	x, y;

public:
		///   constructor / destructor
	vector2()	{}
	vector2(const T& _x, const T& _y)	: x(_x), y(_y)	{}
	template <class T2>
	vector2(const vector2<T2>& v) { x=T(v.x); y=T(v.y); }

	void		set(const T& _x, const T& _y)					{ x=_x; y=_y; } 

		///   operators
	template <class T2>
	vector2<T>&		operator= (const vector2<T2>& v)			{ x=T(v.x); y=T(v.y); return *this; }
	bool			operator==(const vector2<T>& v) const		{ return  (x==v.x) && (y==v.y); }
	vector2<T>&		operator+=(const vector2<T>& v)				{ x+=v.x; y+=v.y; return *this; }
	vector2<T>&		operator-=(const vector2<T>& v)				{ x-=v.x; y-=v.y; return *this; }
	vector2<T>		operator+ (const vector2<T>& v) const		{ return  vector2<T>(x+v.x, y+v.y); }
	vector2<T>		operator- (const vector2<T>& v) const		{ return  vector2<T>(x-v.x, y-v.y); }
	vector2<T>		operator- () const							{ return  vector2<T>(-x, -y); }
		//
	vector2<T>&		operator*=(const T& c)						{ x*=c; y*=c; return *this; }
	vector2<T>&		operator/=(const T& c)						{ x/=c; y/=c; return *this; }
	vector2<T>		operator* (const T& c) const				{ return  vector2<T>(x*c, y*c); }
	vector2<T>		operator/ (const T& c) const				{ return  vector2<T>(x/c, y/c); }
		///
	T				norm2() const									{ return  (x*x + y*y); }
	T				norm()  const									{ return  T(sqrt( norm2() )); }
	vector2<T>		normalized() const								{ T l=norm(); return vector2<T>(x/l, y/l); } 
		///
	T				dot_product  (const vector2<T>& v) const		{ return  (x*v.x + y*v.y); }
	vector2<T>		mul_each     (const vector2<T>& v) const		{ return  vector2<T>(x*v.x, y*v.y); }
	vector2<T>		div_each     (const vector2<T>& v) const		{ return  vector2<T>(x/v.x, y/v.y); }
		///

	T*				get_pointer()									{ return (T*)(this); }
	const T* const	get_pointer() const								{ return (T*)(this); }
};


//-- special version
typedef		vector2<double>		vector2d;
typedef		vector2<float>		vector2f;


//-- functions
template<class T>		vector2<T>		operator*(const T& c, const vector2<T>& v)		{ return v*c; }
template<class T>		vector2<T>		normalize(const vector2<T>& v)					{ return v.normalized(); }
template<class T>		T				norm2(const vector2<T>& v)						{ return v.norm2();   }
template<class T>		T				norm(const vector2<T>& v)						{ return v.norm();   }
template<class T>	T				dot_product  (const vector2<T>& v1, const vector2<T>& v2)	{ return v1.dot_product(v2);   }
template<class T>	vector2<T>		mul_each(const vector2<T>& v1, const vector2<T>& v2)		{ return v1.mul_each(v2); }
template<class T>	vector2<T>		div_each(const vector2<T>& v1, const vector2<T>& v2)		{ return v1.div_each(v2); }



//===========
//  matrix3
//===========
template<class T>
class  matrix3
{
public:
	union {
		T	m[9];
		struct {
			T m11,m12,m13, m21,m22,m23, m31,m32,m33;
		};
	};

public:
	///   constructor / destructor
	matrix3()	{}
	matrix3(const T& a11, const T& a12, const T& a13,
			const T& a21, const T& a22, const T& a23,
			const T& a31, const T& a32, const T& a33)
			{
				set(a11,a12,a13, a21,a22,a23, a31,a32,a33);
			}
	template <class T2>		matrix3(const T2 m0[9])			{ set(m0); }
	template <class T2>		matrix3(const matrix3<T2>& M)	{ set(M); }
	template <class T2>		matrix3(const vector3<T2>& v1, const vector3<T2>& v2, const vector3<T2>& v3)	{ set(v1, v2, v3); }

	/// set elements
	void	set(const T& a11, const T& a12, const T& a13,
				const T& a21, const T& a22, const T& a23,
				const T& a31, const T& a32, const T& a33)
			{
				m11=a11;  m12=a12;  m13=a13;
				m21=a21;  m22=a22;  m23=a23;
				m31=a31;  m32=a32;  m33=a33;
			}
	template <class T2>		void	set(const T2 m0[9])			{ for (int i=0; i<9; ++i)  m[i] = T(m0[i]); }
	template <class T2>		void	set(const matrix3<T2>& M)	{ for (int i=0; i<9; ++i)  m[i] = T(M.m[i]); }
	template <class T2>		void	set(const vector3<T2>& v1, const vector3<T2>& v2, const vector3<T2>& v3)
			{
				m11=v1.x;	m12=v2.x;	m13=v3.x;
				m21=v1.y;	m22=v2.y;	m23=v3.y;
				m31=v1.z;	m32=v2.z;	m33=v3.z;
			}

	///   index
	T&				operator()(int i, int j)				{ return m[i*3 + j]; }
	const T&		operator()(int i, int j) const			{ return m[i*3 + j]; }

	///   operators
	template <class T2>
	matrix3<T>&		operator= (const matrix3<T2>& M)		{ set(M); return *this; }

	bool			operator==(const matrix3<T>& M) const	{ for (int i=0; i<9; ++i)  { if (m[i]!=M.m[i]) return false; }  return true; }
	matrix3<T>&		operator+=(const matrix3<T>& M)			{ for (int i=0; i<9; ++i)  { m[i] += M.m[i]; } return *this; }
	matrix3<T>&		operator-=(const matrix3<T>& M)			{ for (int i=0; i<9; ++i)  { m[i] -= M.m[i]; } return *this; }
	matrix3<T>		operator+ (const matrix3<T>& M) const	{ matrix3<T> M2(*this); M2+=M; return M2; }
	matrix3<T>		operator- (const matrix3<T>& M) const	{ matrix3<T> M2(*this); M2-=M; return M2; }
	matrix3<T>		operator- () const						{ return  matrix3<T>(-m11,-m12,-m13,-m21,-m22,-m23,-m31,-m32,-m33); }
	//
	matrix3<T>&		operator*=(const T& c)					{ for (int i=0; i<9; ++i) m[i]*=c; return *this; }
	matrix3<T>&		operator/=(const T& c)					{ const T ic(T(1)/c); for (int i=0; i<9; ++i) m[i]*=ic; return *this; }
	matrix3<T>		operator* (const T& c) const			{ return  matrix3<T>(c*m11, c*m12, c*m13,
																				 c*m21, c*m22, c*m23,
																				 c*m31, c*m32, c*m33);
															}
	matrix3<T>		operator/ (const T& c) const			{ const T ic(T(1)/c);
															  return  matrix3<T>(ic*m11, ic*m12, ic*m13,
																				 ic*m21, ic*m22, ic*m23,
																				 ic*m31, ic*m32, ic*m33);
															}

	matrix3<T>		operator* (const matrix3<T>& M) const
					{
						return matrix3<T>(
							m11*M.m11 + m12*M.m21 + m13*M.m31,
							m11*M.m12 + m12*M.m22 + m13*M.m32,
							m11*M.m13 + m12*M.m23 + m13*M.m33,
							m21*M.m11 + m22*M.m21 + m23*M.m31,
							m21*M.m12 + m22*M.m22 + m23*M.m32,
							m21*M.m13 + m22*M.m23 + m23*M.m33,
							m31*M.m11 + m32*M.m21 + m33*M.m31,
							m31*M.m12 + m32*M.m22 + m33*M.m32,
							m31*M.m13 + m32*M.m23 + m33*M.m33
							);
					}
	vector3<T>		operator* (const vector3<T>& v) const
	{
		return vector3<T>(
			m11*v.x + m12*v.y + m13*v.z,
			m21*v.x + m22*v.y + m23*v.z,
			m31*v.x + m32*v.y + m33*v.z
			);
	}

	///
	void		transpose()			{ swap(m12,m21);	swap(m13,m31);	swap(m23,m32); }
	matrix3<T>	transposed() const	{ return matrix3<T>(m11,m21,m31, m12,m22,m32, m13,m23,m33); }

	/// pointer
	T*				get_pointer()									{ return (T*)this; }
	const T* const	get_pointer() const								{ return (T*)this; }

	/// base
	void	get_bases(vector3<T>& ex, vector3<T>& ey, vector3<T>& ez) const {
		ex.set(m11, m21, m31);
		ey.set(m12, m22, m32);
		ez.set(m13, m23, m33);
	}

	void	get_bases_tr(vector3<T>& ex, vector3<T>& ey, vector3<T>& ez) const {
		ex.set(m11, m12, m13);
		ey.set(m21, m22, m23);
		ez.set(m31, m32, m33);
	}
};






//-- special version
typedef		matrix3<double>		matrix3d;
typedef		matrix3<float>		matrix3f;


//-- functions
template<class T>		matrix3<T>		operator*(const T& c, const matrix3<T>& M)		{ return M*c; }
template<class T>		matrix3<T>		transpose(const matrix3<T>& M)					{ return M.transposed(); }



//-----------
//  回転行列
//-----------

// x軸周り
template<class T>
matrix3<T>
Rx3(const T& angle)
{
	double cs, sn;
	cs = cos(angle);
	sn = sin(angle);
	return  matrix3<T>( T(1.0),	T(0.0),	T(0.0),
						T(0.0),	cs,		-sn,
						T(0.0),	sn,		cs);
}

// y軸周り
template<class T>
matrix3<T>
Ry3(const T& angle)
{
	double cs, sn;
	cs = cos(angle);
	sn = sin(angle);
	return  matrix3<T>( cs,		T(0.0),	sn,
						T(0.0),	T(1.0),	T(0.0),
						-sn,	T(0.0),	cs);
}

// z軸周り
template<class T>
matrix3<T>
Rz3(const T& angle)
{
	double cs, sn;
	cs = cos(angle);
	sn = sin(angle);
	return  matrix3<T>( cs,		-sn,	T(0.0),
						sn,		cs,		T(0.0),
						T(0.0),	T(0.0),	T(1.0));
}

// z->x->z 軸の周りのオイラー角による回転
template<class T>
matrix3<T>
Rzxz3(const T& angle1, const T& angle2, const T& angle3)
{
	const double c1 = cos(angle1);
	const double s1 = sin(angle1);
	const double c2 = cos(angle2);
	const double s2 = sin(angle2);
	const double c3 = cos(angle3);
	const double s3 = sin(angle3);
	const double c2s3 = c2*s3;
	const double c2c3 = c2*c3;

	return  matrix3<T>( T( c1*c3 - s1*c2s3),
						T(-c1*s3 - s1*c2c3),
						T( s1*s2),
						T( s1*c3 + c1*c2s3),
						T(-s1*s3 + c1*c2c3),
						T(-c1*s2),
						T( s2*s3),
						T( s2*c3),
						T( c2) );
}

// degree バージョン
template<class T>
matrix3<T>
Rx3deg(const T& angle)
{
	return  Rx3(T(D2R*angle));
}

template<class T>
matrix3<T>
Ry3deg(const T& angle)
{
	return  Ry3(T(D2R*angle));
}

template<class T>
matrix3<T>
Rz3deg(const T& angle)
{
	return  Rz3(T(D2R*angle));
}

template<class T>
matrix3<T>
Rzxz3deg(const T& angle1, const T& angle2, const T& angle3)
{
	return  Rzxz3(T(D2R*angle1), T(D2R*angle2), T(D2R*angle3));
}


// 任意の軸周り
template<class T>
matrix3<T>
rotation_matrix3(const T& angle, const vector3<T>& n)
{
	double cs, sn;
	cs = cos(angle);
	sn = sin(angle);
	double cc, nxyc, nxzc, nyzc, nxs, nys, nzs;
	cc  = T(1.0) - cs;
	nxyc = cc*n.x*n.y;
	nxzc = cc*n.x*n.z;
	nyzc = cc*n.y*n.z;
	nxs = n.x*sn;
	nys = n.y*sn;
	nzs = n.z*sn;

	return  matrix3<T>( cs+n.x*n.x*cc,	nxyc-nzs,		nxzc+nys,
						nxyc+nzs,		cs+n.y*n.y*cc,	nyzc-nxs,
						nxzc-nys,		nyzc+nxs,		cs+n.z*n.z*cc );
}


#endif
