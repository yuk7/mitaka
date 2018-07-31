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
#ifndef  __ASTROMETRY_H__
#define  __ASTROMETRY_H__
#include  <vector>
#include  "../math/vector3.h"
#include  "../astro/astro_const.h"
using namespace std;
const char  star_spectral_type[] = {'O', 'B', 'A', 'F', 'G', 'K', 'M' };


//--- 変換行列 ---
extern const matrix3d	M_E2R;		// 黄道座標から赤道座標への変換行列
extern const matrix3d	M_R2E;		// 赤道座標から黄道座標への変換行列
extern const matrix3d	M_R2G;		// 赤道座標から銀河座標への変換行列
extern const matrix3d	M_G2R;		// 銀河座標から赤道座標への変換行列
extern const matrix3d	M_E2G;		// 黄道座標から銀河座標への変換行列
extern const matrix3d	M_G2E;		// 銀河座標から黄道座標への変換行列


//------------
//  赤経・赤緯
//------------
struct  RAhms {
	short	h, m;
	float	s;
};

struct  DEdms {
	bool	b_minus;	// 符号（d,m,sには絶対値を入れる。d=-0対処用）
	short	d, m;
	float	s;
};

//----------------
//  class RA_DEC
//----------------
class RA_DEC
{
public:
	float  RAdeg, DEdeg;
public:
	RA_DEC() {};
	RA_DEC(const float& alp, const float& dlt) : RAdeg(alp), DEdeg(dlt) {};
	RA_DEC(const RAhms& RA, const DEdms& DEC) { set_RAhms(RA); set_DEdms(DEC); }
	//-- 方向余弦 --
	vector3d	get_dirc_vec() const;
	void	set_dirc_vec(const vector3d& v);
	//-- hms/dms 表記からの変換 --
	void	set_RAhms(const RAhms& RA);
	void	set_DEdms(const DEdms& DEC);
};

inline  vector3d
RA_DEC::get_dirc_vec() const
{
	const double D2R = M_PI/180.0;
	const double alp = D2R * RAdeg;
	const double dlt = D2R * DEdeg;
	const double cosd = cos(dlt);
	return  vector3d(cosd*cos(alp), cosd*sin(alp), sin(dlt));
}

inline  void
RA_DEC::set_dirc_vec(const vector3d& v)
{
	const double R2D = 180.0 / M_PI;
	double rho = sqrt(v.x*v.x + v.y*v.y);
	RAdeg = R2D * atan2(v.y, v.x);
	while (RAdeg < 0.0) { RAdeg += 360.0; }
	DEdeg = R2D * atan2(v.z, rho);
	while (DEdeg < 0.0) { DEdeg += 360.0; }
}

inline  void
RA_DEC::set_RAhms(const RAhms& RA)
{
	RAdeg = 15.0f*RA.h + 0.25f*RA.m + 0.25f/60.0f*RA.s;
}

inline  void
RA_DEC::set_DEdms(const DEdms& DEC)
{
	if (!DEC.b_minus)	{ DEdeg = DEC.d + 1.0f/60.0f * DEC.m + 1.0f/3600.0f * DEC.s; }
	else				{ DEdeg = DEC.d - 1.0f/60.0f * DEC.m - 1.0f/3600.0f * DEC.s; }
}


//--- B1950.0 から J2000.0 への変換 ---
// 位置の変換（固有運動あり）
RA_DEC	B1950_to_J2000(const RA_DEC& posB1950, const float& mua0, const float& mud0, const float& Ra0, const float& Plx0);	
// 位置の変換（固有運動なし）
inline RA_DEC  B1950_to_J2000(const RA_DEC& posB1950)  { return B1950_to_J2000(posB1950, 0, 0, 0, 0); }

// 固有運動の変換
RA_DEC	B1950_to_J2000_pm(const RA_DEC& pos1950, const RA_DEC& pm1950);


//-----------------------------------------------------
//  RA-DEC position of Galactic center/ northern pole
//  SgrA*, Yusef-Zadeh & Choate, 1999, ApJ, 518, L33
//-----------------------------------------------------
const RAhms		GC_RAhms ={ 17, 45, 40.0383 };
const DEdms		GC_DEdms={true,  -29, 00, 28.069 };
const RAhms		GN_RAhms ={ 12, 51, 24};
const DEdms		GN_DEdms={false,  27,  8,  0};
const RA_DEC	GC_RA_DEC(GC_RAhms, GC_DEdms);	// 銀河中心
const RA_DEC	GN_RA_DEC(GN_RAhms, GN_DEdms);	// 銀河の北極
extern const vector3f  vGC, vGN, vGO;


//--------------------
//  Astrometric data
//--------------------
struct astrometric_data {
	RA_DEC		eqPos;			// Right ascension/Declination (in degree)
	float		Plx;			// 視差 (in milli arc sec)
	float		e_Plx;			// 視差の誤差
	float		Vmag;			// V mag (Johnson V)
	float		B_V;			// Johnson B-V colour
};


//------------------
//  Hipparcos data
//------------------
struct Hipparcos_data : public astrometric_data
{
	long		HIP;			// Identifier (HIP number)
	float		pmRA, pmDE;
	float		distance;		// 距離 (in pc)
	float		Mag;			// 絶対等級
	char		SpcType;		// スペクトル型 (O,B,A,F,G,K,M)
	char		SpcIdx2;		// スペクトル型の後の数字 (0-9)
	bool		bDouble;		// Double/Multiple System Flag
	short		Framsteed;
	char		BayerID, BayerSub;
	char		ConstellationID;
	char		ADScomp;
};


//--------------
//  Tycho data
//--------------
struct Tycho_data : public astrometric_data
{
	long		HIP;			// Identifier (HIP number)
};


//------------
//  NGC data
//------------
struct NGC_data
{
	short	NGC_NO;				// NGC or IC number
	bool	bNGC;				// NGC? (or IC)
	RA_DEC	eqPos;
	char	type;				// object classification
	float	size;				// largest dimension (in arcmin)
	float	mag;				// magnitude visual or photograhic
	bool	bPhotographicMag;	//
};

enum {
	NGC_TY_NONE,
	NGC_TY_GALAXY,
	NGC_TY_OPEN_STAR_CLUSTER,
	NGC_TY_GLOBULAR_STAR_CLUSTER,
	NGC_TY_EMISSION_OR_REFLECTION_NEBULA,
	NGC_TY_PLANETARY_NEBULA,
	NGC_TY_CLUSTER_WITH_NEBULOSITY,
	NGC_TY_ASTERISM,
	NGC_TY_KNOT_IN_EXTERNAL_GALAXY,
	NGC_TY_TRIPLE_STAR,
	NGC_TY_DOUBLE_STAR,
	NGC_TY_SINGLE_STAR
};


//-------------
//  Star Name
//-------------
struct star_name
{
	long HIP;			// HIP number
	char flg;			// Flag
	char name_E[30];	// 英名
	char name_J[100];	// 日本名
};


//  Functuons
inline  float
B_V_to_Tc(float B_V)
{
	return  7300.0f / (B_V + 0.73f);	// color temparature, see [2] pg.124 eq.(4-75)
}



//-- Refs -------------------------------------------
//  [1] The Hipparcos Catalogue,
//        ESA, 1997, ESA SP-1200
//  [2] Astrophysical concepts,
//        M.Harwit, 1997
//---------------------------------------------------


#endif
