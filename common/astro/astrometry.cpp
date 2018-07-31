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
#include  "astrometry.h"
#include  <time.h>



//-------------------------------
//  B1950.0 から J2000.0 への変換
//-------------------------------
// 「天体の位置計算」pg.238
//-------------------------------

//-- 位置
const matrix3d M_B1950_to_J2000_1(0.9999256782, -0.0111820610, -0.0048579477,
								  0.0111820609,  0.9999374784, -0.0000271765,
								  0.0048579479, -0.0000271474,  0.9999881997);
const matrix3d M_B1950_to_J2000_2(0.0000024239502, -0.0000000271066, -0.0000000117766,
								  0.0000000271066,  0.0000024239788, -0.0000000000659,
								  0.0000000117766, -0.0000000000658,  0.0000024241017);
//-- 固有速度
const matrix3d M_B1950_to_J2000_3(-0.00055, -0.23854,  0.43574,
								   0.23849, -0.00267, -0.00854,
								  -0.43562,  0.01225,  0.00212);
const matrix3d M_B1950_to_J2000_4(0.99994704, -0.01118251, -0.00485767,
								  0.01118251,  0.99995883, -0.00002718,
								  0.00485767, -0.00002714,  1.00000956);

//--- 位置の変換 ---

// 固有運動あり
//  mua0  in sec    per 100 Bessel year
//  mud0  in arcsec per 100 Bessel year
//  Ra0   in km/sec
//  Plx0  in arcsec
RA_DEC
B1950_to_J2000(const RA_DEC& posB1950, const float& mua0, const float& mud0, const float& Ra0, const float& Plx0)
{
	const double D2R = M_PI / 180.0;

	vector3d X0, V0;
	

	double alp0, dlt0, sina0, cosa0, sind0, cosd0;
	alp0 = D2R * posB1950.RAdeg;
	dlt0 = D2R * posB1950.DEdeg;
	sina0 = sin(alp0);
	cosa0 = cos(alp0);
	sind0 = sin(dlt0);
	cosd0 = cos(dlt0);

	// 方向ベクトル(double)
	X0.set(cosd0*cosa0, cosd0*sina0, sind0);

	// 固有運動速度
	V0 = double(mud0) * vector3d(-sind0*cosa0, -sind0*sina0, cosd0)
	   + 15.0 * double(mua0) * vector3d(-cosd0*sina0, cosd0*cosa0, 0.0)
	   + 21.094502 * Plx0 * Ra0 * X0;

	//
	const double  q = dot_product(X0, vector3d(-1.62557, -0.31919, -0.13843));
	const double dq = dot_product(X0, vector3d( 1.245, -1.580, -0.659));

	vector3d X1, V1;
	X1 = X0 + 1e-6 * vector3d(1.62557, 0.31919, 0.13843) + q*1e-6*X0;
	V1 = V0 + 1e-3 * vector3d(-1.245, 1.580, 0.659) + 1e-3*dq*X0;
	
	RA_DEC pos2000;
	vector3d X2000 = M_B1950_to_J2000_1 * X1 + M_B1950_to_J2000_2 * V1;
	pos2000.set_dirc_vec( X2000 );

	return  pos2000;
}

//--- 固有運動の変換 ---
RA_DEC
B1950_to_J2000_pm(const RA_DEC& pos1950, const RA_DEC& pm1950)
{
	RA_DEC pm2000;
	vector3d V2000 = M_B1950_to_J2000_3 * pos1950.get_dirc_vec()
					+M_B1950_to_J2000_4 * pm1950.get_dirc_vec();
	pm2000.set_dirc_vec( V2000 );

	return  pm2000;
}


//-----------
//  変換行列
//-----------
// 赤道座標と黄道座標
const double incE = 23.43870583;						// 地軸の傾き
const double thE  = -incE * M_PI/180;
const double csE( cos(thE) );
const double snE( sin(thE) );
const matrix3d  M_E2R(1,0,0, 0,csE, snE, 0,-snE,csE);	// 黄道座標から赤道座標への変換行列
const matrix3d	M_R2E(1,0,0, 0,csE,-snE, 0, snE,csE);	// 赤道座標から黄道座標への変換行列
// 銀河座標から日心黄道座標へ
const vector3f  vGC( GC_RA_DEC.get_dirc_vec() );
const vector3f  vGN( GN_RA_DEC.get_dirc_vec() );
const vector3f  vGO( cross_product(vGN, vGC) );
const matrix3d	M_G2R(vGC, vGO, vGN);
const matrix3d	M_R2G( M_G2R.transposed() );
const matrix3d	M_G2E( M_R2E * M_G2R );
const matrix3d	M_E2G( M_G2E.transposed() );
