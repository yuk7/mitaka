/******************************************************************************

Copyright (c) 2007, 2015-2016   Tsunehiko Kato

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
#ifndef __3D_COMMON_H__
#define __3D_COMMON_H__
#include  "../math/vector3.h"


//-----------
//  Frustum
//-----------
class frustum
{
private:
	float x1, y1, x2, y2, z1, z2;
public:
	frustum() {};
	~frustum() {};
	void	set(float _x1, float _x2, float _y1, float _y2, float _z1, float _z2);
	void	set2(float aspc_rt, float screen_z, float screen_w, float near_z, float far_z, float offset_x);
	float	get_x1() const	{ return x1; }
	float	get_x2() const	{ return x2; }
	float	get_y1() const	{ return y1; }
	float	get_y2() const	{ return y2; }
	float	get_z1() const	{ return z1; }
	float	get_z2() const	{ return z2; }

	void	get_planes(vector3f *nv, float *D) const;

	void	calc_cull_vectors(vector3d *nv);
	char	cull_test(const float& x, const float& y, const float& z) const;
	char	cull_test(const vector3f& r) const { return cull_test(r.x, r.y, r.z); }
	bool	is_outside(int face_no, const float& x, const float& y, const float& z) const;
	bool	is_outside(int face_no, const vector3f& r) const { return is_outside(face_no, r.x, r.y, r.z); }

	vector3f	get_intersection(int face_no, const vector3f& r1, const vector3f& r2) const;
};


//--
const char FRUSTUM_CULL_X1 = 0x01;
const char FRUSTUM_CULL_X2 = 0x02;
const char FRUSTUM_CULL_Y1 = 0x04;
const char FRUSTUM_CULL_Y2 = 0x08;
const char FRUSTUM_CULL_Z1 = 0x10;
const char FRUSTUM_CULL_Z2 = 0x20;
const char FRUSTUM_CULL_X  = FRUSTUM_CULL_X1 | FRUSTUM_CULL_X2;
const char FRUSTUM_CULL_Y  = FRUSTUM_CULL_Y1 | FRUSTUM_CULL_Y2;
const char FRUSTUM_CULL_XY = FRUSTUM_CULL_X  | FRUSTUM_CULL_Y;

//
enum {
	FRUSTUM_FACE_LEFT,
	FRUSTUM_FACE_RIGHT,
	FRUSTUM_FACE_TOP,
	FRUSTUM_FACE_BOTTOM,
	FRUSTUM_FACE_NEAR,
	FRUSTUM_FACE_FAR
};



// 外向き法線、z負が正面方向
inline  void
frustum::get_planes(vector3f *nv, float *D) const
{
	
	//*** Test
	/*
	float a = 0.8;
	nv[0] = normalize(vector3f(-z1, 0, -x1*a));
	nv[1] = normalize(vector3f(z1, 0, x2*a));
	nv[2] = normalize(vector3f(0, -z1, -y1*a));
	nv[3] = normalize(vector3f(0, z1, y2*a));
	*/

	
	nv[0] = normalize(vector3f(-z1,   0, -x1));
	nv[1] = normalize(vector3f( z1,   0,  x2));
	nv[2] = normalize(vector3f(  0, -z1, -y1));
	nv[3] = normalize(vector3f(  0,  z1,  y2));
	

	nv[4].set(0, 0, 1);
	nv[5].set(0, 0, -1);

	D[0] = D[1] = D[2] = D[3] = 0;
	D[4] = z1;
	D[5] = -z2;
}



//*** to be deleted
inline void
frustum::calc_cull_vectors(vector3d *nv) {
	float _z1;
	_z1 = -z1;

	vector3f v11, v12, v21, v22;

	//***  Test
	/*
	float a = 0.9;
	v11.set(x1*a, y1*a, _z1);
	v12.set(x2*a, y1*a, _z1);
	v21.set(x1*a, y2*a, _z1);
	v22.set(x2*a, y2*a, _z1);
	*/
	//***


	
	v11.set(x1, y1, _z1);
	v12.set(x2, y1, _z1);
	v21.set(x1, y2, _z1);
	v22.set(x2, y2, _z1);
	


	// 各平面の外向き法線ベクトルを求める
	nv[0] = normalize(cross_product(v21, v11));	//  Left
	nv[1] = normalize(cross_product(v12, v22));	//  Right
	nv[2] = normalize(cross_product(v22, v21));	//  Top
	nv[3] = normalize(cross_product(v11, v12));	//  Bottom
}



// 指定した点が、外部にあるか調べる
inline char
frustum::cull_test(const float& x, const float& y, const float& z) const
{
	char bf = 0;
	const float xz = x*z1;
	const float yz = y*z1;
	if (xz < x1*z)		{ bf |= FRUSTUM_CULL_X1; }
	if (xz > x2*z)		{ bf |= FRUSTUM_CULL_X2; }
	if (yz < y1*z)		{ bf |= FRUSTUM_CULL_Y1; }
	if (yz > y2*z)		{ bf |= FRUSTUM_CULL_Y2; }
	if (z < z1)			{ bf |= FRUSTUM_CULL_Z1; }
	if (z > z2)			{ bf |= FRUSTUM_CULL_Z2; }
	return  bf;
}


// 指定した点が指定した面の外部にあるか調べる
inline bool
frustum::is_outside(int face_no, const float& x, const float& y, const float& z) const
{
	switch (face_no) {
	case FRUSTUM_FACE_LEFT:		return (x*z1 < x1*z);
	case FRUSTUM_FACE_RIGHT:	return (x*z1 > x2*z);
	case FRUSTUM_FACE_TOP:		return (y*z1 < y1*z);
	case FRUSTUM_FACE_BOTTOM:	return (y*z1 > y2*z);
	case FRUSTUM_FACE_NEAR:		return (z < z1);
	case FRUSTUM_FACE_FAR:		return (z > z2);
	default:					return false;
	}
}


// 2点と指定した面との交点を求める
inline vector3f
frustum::get_intersection(int face_no, const vector3f& r1, const vector3f& r2) const
{
	vector3f dr;
	float dx, dy, dz, t;

	dr = r2 - r1;
	dx = dr.x;
	dy = dr.y;
	dz = dr.z;

	switch (face_no) {
	case FRUSTUM_FACE_LEFT:
		t = (x1*r1.z - z1*r1.x) / (dx*z1 - dz*x1);
		break;
	case FRUSTUM_FACE_RIGHT:
		t = (x2*r1.z - z1*r1.x) / (dx*z1 - dz*x2);
		break;
	case FRUSTUM_FACE_TOP:
		t = (y1*r1.z - z1*r1.y) / (dy*z1 - dz*y1);
		break;
	case FRUSTUM_FACE_BOTTOM:
		t = (y2*r1.z - z1*r1.y) / (dy*z1 - dz*y2);
		break;
	case FRUSTUM_FACE_NEAR:
		t = (z1 - r1.z) / dz;
		break;
	case FRUSTUM_FACE_FAR:
		t = (z2 - r1.z) / dz;
		break;
	default:
		t = 0;
		break;
	}

	return dr*t + r1;
}


#endif
