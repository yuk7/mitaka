/**************************************************************************************************

Copyright (c) 2007   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#include  <windows.h>
#include  <process.h>
#include  <math.h>
#include  "../common/math/Triangulation.h"
#include  "../common/win_dlg.h"
#include  "../common/graphics/gl_win.h"
#include  "../common/astro/astrometry.h"
#include  "../common/graphics/color.h"
#include  "../common/graphics/gl_image.h"
#include  "../common/math/random.h"
#include  "mitaka.h"
#include  "mtk_gl_common.h"
#include  "mtk_draw_struct.h"
#include  "../common/cpu.h"
#include  "mtk_cpu_info.h"
#include  "mtk_our_galaxy_rt.h"
#include  "mtk_approx_func.h"
#include  "mtk_switches.h"
#include  "mtk_colors.h"
#include  "mtk_solar_system.h"
#include  "mtk_draw_common.h"
#include  "mtk_thread.h"


bool	isVistaOrLater();



static CS cs;	// Grobal only




//
//
//
void
go_to_galactic_coord()
{
	const float MGL_G2E[16] = {
		M_G2E.m11, M_G2E.m21, M_G2E.m31, 0,
		M_G2E.m12, M_G2E.m22, M_G2E.m32, 0,
		M_G2E.m13, M_G2E.m23, M_G2E.m33, 0,
		0, 0, 0, 1
	};

	glMultMatrixf(MGL_G2E);
}



//--------------------------
//  モジュレーション用イメージ
//--------------------------
const int imgN = 512;
unsigned char thin_mdl[imgN][imgN], dust_mdl[imgN][imgN];
const float img_min_val = 0.3;


//--------------
//  星の点データ
//--------------
static vector<vector3f> vGalaxyPoints[4];
enum {
	GALPT_DISK1,
	GALPT_DISK2,
	GALPT_DISKOB,
	GALPT_BULGE
};





//-----------
//  読み込み
//-----------
bool
load_our_galaxy_model(const directory& dir)
{
	FILE* fp;
	fp = dir.fopen("ourgalaxy_model.dat", "rb");
	if (fp==NULL)  return false;


	//-- Disk
	int w, h;
	fread(&w, sizeof(w), 1, fp);
	fread(&h, sizeof(h), 1, fp);
	w = h = 512; //***
	for (int iy=0; iy<h; iy++) {
		for (int ix=0; ix<w; ix++) {
			float val0;
			fread(&val0, sizeof(val0), 1, fp);
			float val = val0*(1-img_min_val) + img_min_val;
			if (val > 1.0)  val = 1.0;
			thin_mdl[iy][ix] = (unsigned char)(255*val);
		}
	}

	//-- Dust
	fread(&w, sizeof(w), 1, fp);
	fread(&h, sizeof(h), 1, fp);
	w = h = 512;
	for (int iy=0; iy<h; iy++) {
		for (int ix=0; ix<w; ix++) {
			float val;
			fread(&val, sizeof(val), 1, fp);
			if (val > 1.0)  val = 1.0;
			dust_mdl[iy][ix] = (unsigned char)(255*val);
		}
	}



	//-- 星の点データを読み込む
	for (int i=0; i<4; i++) {
		vector<vector3f>& vPt = vGalaxyPoints[i];
		int num;
		fread(&num, sizeof(num), 1, fp);
		vPt.resize(num);

		for (int j=0; j<num; j++) {
			vector3f pos;
			fread(pos.get_pointer(), sizeof(pos), 1, fp);
			vPt[j] = pos;
		}
	}


	fclose(fp);

	return  true;
}






//------------------------
//  銀河系モデルのパラメータ
//------------------------
//--  Thin Disk
const float h1r = 3800;
const float h1z = 350;
const float B_V1 = 0.0;
//--  Thick Disk
const float h2r = 3800;
const float h2z = 900;
const float R2 = 0.019 * exp(R_gal*(1.0/h2r - 1.0/h1r));
const float B_V2 = 1.2;
//--  Elliptical
const float re = 2700;		// 有効半径
const float qe = 0.84;		// 扁平度
const float R3 = 0.0011 * exp(7.669f*pow(R_gal/re, 0.25f) - R_gal/h1r);
//--  Dust
const float hdr = 3800;
const float hdz = 186;
const float kp0 = 0.004;
//const float kp0 = 0.001;	//***
//-- 計算領域
//const float Rmax = 20000;
const float Rmax = 15000;
const float Rmax_sqr = Rmax*Rmax;
const float qg = 0.3;		// 計算領域の扁平度







int GalaxyRayNum    = 6000;
float GalaxyRayStepLen = 40.0f;
int GalaxyStarNum   = 2000;


//----------------
//  描画用の点配列
//----------------
vector3f*	vSmpPt = 0;
vector2f*	vVtx = 0;
Triangle*	vTri = 0;
RGBf*		Intensity = 0;
RGBf*		Extinction = 0;
const int	ExpTblN = 500;
const float ExpTblTauMax = 5.0;
const float ExpTblFac = float(ExpTblN) / ExpTblTauMax;
float		ExpTbl[ExpTblN+1][3];


void
free_memory_for_galaxy()
{
	if (vSmpPt != 0)		{ delete [] vSmpPt;		vSmpPt = 0; }
	if (vVtx != 0)			{ delete [] vVtx;		vVtx = 0; }
	if (vTri != 0)			{ delete [] vTri;		vTri = 0; }
	if (Intensity != 0)		{ delete [] Intensity;	Intensity = 0; }
	if (Extinction != 0)	{ delete [] Extinction;	Extinction = 0; }
}

void
alloc_memory_for_galaxy()
{
	free_memory_for_galaxy();

	if (GalaxyRayNum < 500)		GalaxyRayNum = 500;
	if (GalaxyRayNum > 100000)	GalaxyRayNum = 100000;

	int _N = GalaxyRayNum + 9 * 9;	// 規則メッシュを含んだ頂点数
	vSmpPt		= new vector3f[GalaxyRayNum];
	vVtx		= new vector2f[_N + 3];
	vTri		= new Triangle[3*_N];
	Intensity	= new RGBf[_N];
	Extinction	= new RGBf[_N];
}



void
init_exp_table()
{
	for (int i=0; i<=ExpTblN; i++) {
		float tauR = float(i) / ExpTblFac;

		ExpTbl[i][0] = exp(-tauR);
		ExpTbl[i][1] = exp(-kpG_kpR*tauR);
		ExpTbl[i][2] = exp(-kpB_kpR*tauR);
	}
}

int
sort_cmp(const void* v1, const void* v2)
{
	vector2f* p1 = (vector2f*)v1;
	vector2f* p2 = (vector2f*)v2;

	if (p1->x < p2->x)		return -1;
	else if (p1->x > p2->x)	return  1;
	return 0;
}



//---------------------
//  サンプル点を生成する
//---------------------
void
make_sampling_points(float r_thin, float r_elliptical, float r_halo)
{
	const float total = r_thin + r_elliptical + r_halo;
	const float p_thin = r_thin / total;
	const float p_elliptical = r_elliptical / total;
	const float p_halo = r_halo / total;
	const float P_thin = p_thin;
	const float P_elliptical = P_thin + p_elliptical;


	const float disk_r0 = 12000;


	for (int i=0; i<GalaxyRayNum; i++) {
		vector3f pos;

		float val = rnd();
		if (val < P_thin) {

			//--  Thin Disk 用のサンプル点  --
			float r, z, th;

			r = disk_r0 * (exp(rnd()) - 1);

			z = 3.0*h1z * rndStdExp();
			if (rndUni(0,1) > 0.5) z = -z;
			th = rndUni(0, 2*M_PI);
			pos.set(r*cos(th) + R_gal, r*sin(th), z);

		} else if (val < P_elliptical) {
			
			//--  楕円体成分用のサンプル点  --
			float r = re;
			float cs = rndUni(-1, 1);
			float sn = sqrt(1-cs*cs);
			float z = r*cs;
			float rho = r*sn;
			float th = rndUni(0, 2*M_PI);
			pos.set(rho*cos(th) + R_gal, rho*sin(th), z*qe);
		
		} else {
		
			//--  Halo 成分用のサンプル点  --
			float r = 0.9*Rmax;// * rnd();
			float cs = rndUni(-1, 1);
			float sn = sqrt(1-cs*cs);
			float z = r*cs;
			float rho = r*sn;
			float th = rndUni(0, 2*M_PI);
			pos.set(rho*cos(th) + R_gal, rho*sin(th), z*qg);
		
		}

		vSmpPt[i] = M_G2E * pos;
	}
}





//------------------------------
//  モジュレーション係数を取得する
//------------------------------
const vector2f	img_gc(256.5, 295.5);			// 画像上の銀河中心の位置
const vector2f  img_ss(118, 227.5);			// 画像上の太陽系の位置
const vector2f  img_dr = img_ss - img_gc;
const float img_scl = img_dr.norm() / R_gal;
// 銀河中心を合わせるための回転角
const float img_th = atan2(-img_dr.y, -img_dr.x);
const float img_cs = img_scl * cos(img_th);
const float img_sn = img_scl * sin(img_th);
const float img_gc_x = img_gc.x;
const float img_gc_y = img_gc.y;



inline void
get_modulation_factor(const float& x0, const float& y0, float &mf1, float& mfd)
{
	const float imgNf = float(imgN);

	float x = x0 * img_cs - y0 * img_sn + img_gc_x;
	if (x < 0.0f || x >= imgNf)	{
		mf1 = img_min_val;
		mfd = kp0;
		return;
	}

	float y = x0 * img_sn + y0 * img_cs + img_gc_y;
	if (y < 0.0f || y >= imgNf)	{
		mf1 = img_min_val;
		mfd = kp0;
		return;
	}

	int ix = int(x);
	int iy = int(y);

	// Thin disk
	const float fac = 1.0f/255.0f;
	mf1 = fac*float(thin_mdl[iy][ix]);
	
	// Dust disk
	const float facd = kp0*0.6f/255.0f;
	mfd = kp0 + facd*float(dust_mdl[iy][ix]);
}






//------------
//  楕円体成分
//------------
const int nTblE = 3000;
const float tblE_max = re * 5;
float tblE[nTblE];
float tblEfac = float(nTblE)/float(tblE_max);

void
make_elliptical_table()
{
	const float n0 = R3;
	const float s0 = tblE_max / re;

	for (int i=0; i<nTblE; i++) {
		const float s = s0 * float(i)/(nTblE - 1);
		tblE[i] = n0 * pow(s/R_gal*re, -7.0f/8.0f) * exp(-7.669 * pow(s, 0.25f));
	}
}

inline float
eval_elliptical(const float& s)
{
	int i = int(tblEfac*s);
	if (i >= nTblE)	i=nTblE-1;
	return tblE[i];
}






//---------------
//  Ray-tracing
//---------------
const RGBf col1 = correct_RGB0( blackbody_color( B_V_to_Tc( B_V1 ) ) );
const RGBf col2 = correct_RGB0( blackbody_color( B_V_to_Tc( B_V2 ) ) );
const float _r1 = col1.r;
const float _g1 = col1.g;
const float _b1 = col1.b;
const float _r2 = col2.r;
const float _g2 = col2.g;
const float _b2 = col2.b;
// 逆数
const float qe2 = qe*qe;
const float h1r_inv = 1.0f/h1r;
const float h1z_inv = 1.0f/h1z;
const float h2r_inv = 1.0f/h2r;
const float h2z_inv = 1.0f/h2z;
const float hdr_inv = 1.0f/hdr;
const float hdz_inv = 1.0f/hdz;
const float qe2_inv = 1.0f/qe2;



inline void
calc_our_Galaxy_ray(const vector3f& r1, const vector3f& n, const float l, float tau_max,
					const float& dz0, float& Ir, float& Ig, float& Ib, float& _tauR)
{
	if (tau_max > ExpTblTauMax) {
		tau_max = ExpTblTauMax;
	}


	float x, y, z;
	x = r1.x;
	y = r1.y;
	z = r1.z;
	
	const float nx = n.x;
	const float ny = n.y;
	const float nz = n.z;


	//
	const float dlt = 0.29f;
	const float cs_min = 0.3f;
	float cs = fabs(nz);
	if (cs < cs_min) cs = cs_min;
	const float ds_c1 = dz0 / (1.0f+dlt)/cs;
	const float ds_c2 = dlt / (1.0f+dlt)/cs;

	// 最初の tau と ds
	float tauR, tauG, tauB, ds;
	tauR = tauG = tauB = 0.0f;
	ds = ds_c1 + fabs(z)*ds_c2;

	float _Ir, _Ig, _Ib;
	_Ir = _Ig = _Ib = 0.0f;
	float s = 0.0f;
	while (s < l) {

		//-- modulation factor --
		float mf1 ,mfd;
		get_modulation_factor(x, y, mf1, mfd);
		
		//--  Thin Disk  --
		const float rho2 = x*x + y*y;
		const float rho = sqrtf(rho2);
		const float abs_z = fabs(z);
		const float em1ds = ds * mf1 * exp_d(rho*h1r_inv + abs_z*h1z_inv);

		//--  Thick Disk and Elliptical  --
		const float rs2 = rho2 + abs_z*abs_z*qe2_inv;
		const float rs = sqrtf(rs2);
		const float em2ds = ds * (R2*exp_d(rho*h2r_inv + abs_z*h2z_inv) + eval_elliptical(rs));

		//--  Dust  --
		const float kpRds = ds * mfd * exp_d(rho*hdr_inv + abs_z*hdz_inv);

		const int idx = int(tauR * ExpTblFac);
		_Ir += (_r1*em1ds + _r2*em2ds) * ExpTbl[idx][0];
		_Ig += (_g1*em1ds + _g2*em2ds) * ExpTbl[idx][1];
		_Ib += (_b1*em1ds + _b2*em2ds) * ExpTbl[idx][2];
		
		tauR += kpRds;
		if (tauR > tau_max) break;
		
		x += nx * ds;
		y += ny * ds;
		z += nz * ds;
		s += ds;


		ds = ds_c1 + ds_c2 * abs_z;
	}

	Ir = Ir*exp_d(tauR) + _Ir;
	Ig = Ig*exp_d(kpG_kpR * tauR) + _Ig;
	Ib = Ib*exp_d(kpB_kpR * tauR) + _Ib;
	_tauR = tauR;
}




//-----------------------
//  Intensity Map の計算
//-----------------------
const int our_galaxy_thread_num_max = 128;

// スレッド用変数
static float*	g_M;
static vector3f	g_cpos;

struct mt_info {
	int i1, i2;
};



void
calc_intensity_map_sub(const int i1, const int i2, const vector3f cpos, float M[16])
{
	const float screen_z = get_screen_z();
	const float z0 = screen_z;

	for (int i=i1; i<i2; i++) {
		// 視点座標系での方向
		vector3f n;
		n.x = vVtx[i].x;
		n.y = vVtx[i].y;
		n.z = -z0;

		// 黄道座標系での点の方向
		vector3f ne;
		ne.x = M[0]*n.x + M[1]*n.y + M[ 2]*n.z;
		ne.y = M[4]*n.x + M[5]*n.y + M[ 6]*n.z;
		ne.z = M[8]*n.x + M[9]*n.y + M[10]*n.z;
		ne = normalize(ne);

		// 銀河座標での光線の方向
		const vector3f ng = M_E2G * ne;

		//---------------------------------------
		//  計算の始点と終点を求める（楕円体との交点）
		//---------------------------------------
		const vector3f ngq = normalize(vector3f(ng.x, ng.y, ng.z/qg));
		const vector3f cposq = vector3f(cpos.x, cpos.y, cpos.z/qg);
		const float b = dot_product(ngq, cposq);
		const float c = cposq.norm2() - Rmax_sqr;
		const float D = b*b - c;
		if (D <= 0) {
			Intensity[i].r  = Intensity[i].g  = Intensity[i].b  = 0.0f;
			Extinction[i].r = Extinction[i].g = Extinction[i].b = 1.0f;
			continue;
		}

		const float d = sqrtf(D);
		const float t1 = -b - d;
		const float t2 = -b + d;
		if (t2 <= 0.0f) {
			Intensity[i].r  = Intensity[i].g  = Intensity[i].b  = 0.0f;
			Extinction[i].r = Extinction[i].g = Extinction[i].b = 1.0f;
			continue;
		}
		vector3f r1, dr;
		if (t1 <= 0.0f) {
			r1 = cposq;
			dr = t2*ngq;
		} else {
			r1 = t1*ngq + cposq;
			dr = 2.0f*d*ngq;
		}
		r1.z *= qg;
		dr.z *= qg;
        
		const float l = dr.norm();		// パスの長さ


		//-------------------
		//  レイ・トレーシング
		//-------------------
		float Ir, Ig, Ib, tauR;
		Ir = Ig = Ib = tauR = 0;
		
		calc_our_Galaxy_ray(r1, ng, l, 4, GalaxyRayStepLen, Ir, Ig, Ib, tauR);

		const float fac = 0.0035;	//*** 0.0015
		Ir *= fac;		if (Ir > 1)  Ir=1;
		Ig *= fac;		if (Ig > 1)  Ig=1;
		Ib *= fac;		if (Ib > 1)  Ib=1;
		Intensity[i].r = Ir;
		Intensity[i].g = Ig;
		Intensity[i].b = Ib;
		int idx = int(tauR * ExpTblFac);
		if (idx > ExpTblN) idx = ExpTblN;
		Extinction[i].r = ExpTbl[idx][0];
		Extinction[i].g = ExpTbl[idx][1];
		Extinction[i].b = ExpTbl[idx][2];
	}
}



unsigned  __stdcall
calc_intensity_map_thread(void *p)
{
	mt_info* pif = (mt_info*)p;
	calc_intensity_map_sub(pif->i1, pif->i2, g_cpos, g_M);

	return 1;
}




void
calc_intensity_map_xp(const int vn, int thread_num)
{
	HANDLE hThreads[our_galaxy_thread_num_max];


	// 各スレッドの計算範囲を設定
	mt_info mti[our_galaxy_thread_num_max];
	for (int i=0; i<thread_num; i++) {
		mti[i].i1 = vn*i/thread_num;
		mti[i].i2 = vn*(i+1)/thread_num;
	}

	// スレッドを起動
	for (int i=0; i<thread_num-1; i++) {
		hThreads[i] = (HANDLE)_beginthreadex(NULL, 0, calc_intensity_map_thread, &mti[i + 1], 0, NULL);
	}
	calc_intensity_map_sub(mti[0].i1, mti[0].i2, g_cpos, g_M);


	// 子スレッドが全て終了するのを待つ
	WaitForMultipleObjects(thread_num - 1, hThreads, TRUE, INFINITE);


	for (int i = 0; i < thread_num - 1; i++) {
		CloseHandle(hThreads[i]);
	}
}




//***** スレッドプールバージョン（VISTA 以降） *****
#ifndef XP_VERSION
struct our_galaxy_thread_info {
	volatile long id;
	mt_info  mti_work[our_galaxy_thread_num_max];
};

static our_galaxy_thread_info gOgtif;
static PTP_WORK pOurGalaxyWork;



VOID CALLBACK
calc_our_galaxy_intensity_work(PTP_CALLBACK_INSTANCE pInstance, PVOID pv, PTP_WORK work)
{
	our_galaxy_thread_info* ogtif = (our_galaxy_thread_info*)pv;
	long id = InterlockedIncrement(&(ogtif->id));

	const mt_info& pif = ogtif->mti_work[id - 1];
	calc_intensity_map_sub(pif.i1, pif.i2, g_cpos, g_M);
}


void
calc_intensity_map_vista(const int vn, int cpu_num)
{
	gOgtif.id = 0;

	for (int i = 0; i<cpu_num; i++) {
		gOgtif.mti_work[i].i1 = vn*i / cpu_num;
		gOgtif.mti_work[i].i2 = vn*(i + 1) / cpu_num;
	}


	for (int i = 0; i < cpu_num; i++) {
		SubmitThreadpoolWork(pOurGalaxyWork);
	}

	WaitForThreadpoolWorkCallbacks(pOurGalaxyWork, FALSE);
}


void
init_our_galaxy_threadpool()
{
	if (!isVistaOrLater())  return;

	pOurGalaxyWork = CreateThreadpoolWork(calc_our_galaxy_intensity_work, (PVOID)&gOgtif, NULL);
}

void
cleanup_our_galaxy_threadpool()
{
	if (!isVistaOrLater())  return;

	CloseThreadpoolWork(pOurGalaxyWork);
}
#endif

void
calc_intensity_map(const int vn, const vector3f cpos, float M[16], int thread_num)
{
	if (thread_num <= 0)				thread_num = 1;
	if (thread_num > our_galaxy_thread_num_max)	thread_num = our_galaxy_thread_num_max;

	g_cpos = cpos;
	g_M = M;

#ifndef XP_VERSION
	calc_intensity_map_vista(vn, thread_num);
#else
	calc_intensity_map_xp(vn, thread_num);
#endif
}




//--------
//  Draw
//--------
void
draw_our_galaxy_model_rt(const mtk_draw_struct& ds, const vector3d& rpos)
{
	static RGBf star_color[4];
	const float screen_z = get_screen_z();


	//-----------
	//  表示判定
	//-----------
	const float dist1 = 1000;
	const float dist2 = 2700;
	const float distSun = ds.cnorm;
	const float distGC = norm(ds.camera_base.get_pos() - get_our_galaxy_position());
	float alpha = 1.0f;

	if (distSun < dist1)  return;
	if (distSun < dist2) {
		alpha = (distSun - dist1) / (dist2 - dist1);
		if (alpha < 0.02)  return;
	}


	if (distGC < dist2) {
		alpha *= (distGC - dist1) / (dist2 - dist1);
		if (alpha < 0.02)  return;
	}



	//-----------
	//  初期設定
	//-----------
	static bool bFirst = true;
	if (bFirst) {
		bFirst = false;

		alloc_memory_for_galaxy();
		init_exp_table();
		make_sampling_points(15, 1, 1);
		make_elliptical_table();

		// 星の色
		const float B_V[4] = { 0.3, -0.5, 0.7, 1.7 };
		for (int i=0; i<4; i++) {
			star_color[i] = correct_RGB0( blackbody_color( B_V_to_Tc(B_V[i]) ) );
		}
	}


	//---------------------------------------------
	//  角度による判定および使用するサンプル点の数の設定
	//---------------------------------------------
	const frustum   frs			= ds.get_frustum();
	const vector3d  camera_pos	= ds.camera.get_pos();
	const vector3d  rgc = get_our_galaxy_position();
	const double distG = norm(camera_pos - rgc);
	bool bReduct = false;

	int vn0 = GalaxyRayNum;
	if (distG > 1.05*Rmax) {
		float thc = 10.0f/180.0f*M_PI;
		const float th = asin(Rmax / distG);
		if (th < thc) {
			const float vn0_min = 1000;
			float a = th / thc;
			vn0 = GalaxyRayNum * a*a;		// 面積に比例した点の数
			if (vn0 < vn0_min)  vn0 = vn0_min;
			bReduct = true;
		}
	}


	// 星
	const int star_num_max = vGalaxyPoints[0].size();

	float star_alpha  = 1.0;
	float star_pt_size = 1.0;
	int star_point_num = GalaxyStarNum;
	const float rpp0 = 0.001;

	float f = rpp0 / ds.rad_per_pixel;
	{
		float a = R_gal / distG;
		if (a > 1) a = 1;
		const float th = asin(a);
		const float thc2 = 25.0f/180.0f*M_PI;
		f *= th / thc2;
	}
	
	if (f < 1.0) {
		star_alpha = f*f;
		while (star_alpha < 0.02 && star_point_num > 1) {
			star_alpha *= 2.0f;
			star_point_num /= 2;
		}
		if (star_point_num <= 1) star_point_num = 0;
	} else {
		star_pt_size = f;
	}





	//------------------------
	//  射影した点のリストを作る
	//------------------------
	float z0;
	z0 = screen_z;

	float  M[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, M);		//  get current coordinates in view frame

	int vn = 0;
	for (int i=0; i<vn0; i++) {
		vector3f pos = vSmpPt[i] - camera_pos;

		float x, y, z;
		x = M[0]*pos.x + M[4]*pos.y + M[8] *pos.z + M[12];
		y = M[1]*pos.x + M[5]*pos.y + M[9] *pos.z + M[13];
		z = M[2]*pos.x + M[6]*pos.y + M[10]*pos.z + M[14];
		z = -z;

		char bf = frs.cull_test(x, y, z);
		if (bf & (FRUSTUM_CULL_XY | FRUSTUM_CULL_Z1) )  continue;

		vVtx[vn].x = z0*x/z;
		vVtx[vn].y = z0*y/z;
		vn++;
	}

	if (vn <= 0)  return;



	//----------------------------------------
	//  サンプル点が少ないとき、グリッドを追加する
	//----------------------------------------
	if (!bReduct) {
		int gN;

		if (vn < GalaxyRayNum - 30*30) {
			gN = 30;
		}
		else {
			gN = 9;
		}

		if (gN > 0) {
			const float sclf = 1.02 * z0 / frs.get_z1();
			float x1, y1, x2, y2;
			x1 = sclf * frs.get_x1();
			x2 = sclf * frs.get_x2();
			y1 = sclf * frs.get_y1();
			y2 = sclf * frs.get_y2();

			for (int iy = 0; iy < gN; iy++) {
				float t = float(iy) / (gN - 1);
				float y = (1 - t)*y1 + t*y2;

				for (int ix = 0; ix < gN; ix++) {
					t = float(ix) / (gN - 1);
					float x = (1 - t)*x1 + t*x2;
					vVtx[vn].x = x;
					vVtx[vn].y = y;
					vn++;
				}
			}
		}
	}



	//--------------------------
	//  Delaunay Triangulation
	//--------------------------
	qsort(vVtx, vn, sizeof(vector2f), sort_cmp);
	int nTri = DelaunayTriangulation(vn, vVtx, vTri);



	//----------------------
	//  Calc Intensity Map
	//----------------------
	// 黄道座標系での真のカメラの位置
	vector3d cpos0 = camera_pos;
	const float a = 1/(M[0]*M[0] + M[1]*M[1] + M[2]*M[2]);
	cpos0.x -= a * (M[0]*M[12] + M[1]*M[13] + M[2 ]*M[14]);
	cpos0.y -= a * (M[4]*M[12] + M[5]*M[13] + M[6 ]*M[14]);
	cpos0.z -= a * (M[8]*M[12] + M[9]*M[13] + M[10]*M[14]);

	// 銀河座標でのカメラの位置
	const vector3f rg(R_gal,0,0);
	const vector3d rge = M_G2E * rg;
	const vector3f cpos = M_E2G * cpos0 - rg;
	const int cpu_num = get_number_of_cpu();


	calc_intensity_map(vn, cpos, M, cpu_num);


	//-------
	//  描画
	//-------
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);


	//-----------------
	//  銀河系全体の描画
	//-----------------
	bool bWire = false;
	//bool bWire = true;
	const float aa = 1 - alpha;
	const RGBf col0(aa,aa,aa);

	glPushMatrix();
		glLoadIdentity();
		glTranslatef(0,0,-z0);

		//------------
		//  背景の減光
		//------------
		glBlendFunc(GL_ZERO, GL_SRC_COLOR);
		glBegin(GL_TRIANGLES);
			for (int i=0; i<nTri; i++) {
				const Triangle& tri = vTri[i];
				for (int j=0; j<3; j++) {
					int pti = tri.p[j];
							
					const RGBf col = col0 + alpha*Extinction[pti];
					glColor3fv(col.get_pointer());
					vector2f pos = vVtx[ pti ];
					glVertex3f(pos.x, pos.y, 0);
				}
			}
		glEnd();

		//-----------
		//  銀河本体
		//-----------
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glBegin(GL_TRIANGLES);
			for (int i=0; i<nTri; i++) {
				const Triangle& tri = vTri[i];
				for (int j=0; j<3; j++) {
					int pti = tri.p[j];
							
					const RGBf& col = Intensity[pti];
					glColor4f(col.r, col.g, col.b, alpha);
					vector2f pos = vVtx[ pti ];
					glVertex3f(pos.x, pos.y, 0);
				}
			}
		glEnd();
		
		if (bWire) {
			glColor3f(0.8, 0.6, 0);
			glLineWidth(0.3);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glBegin(GL_TRIANGLES);
				for (int i=0; i<nTri; i++) {
					const Triangle& tri = vTri[i];
					for (int j=0; j<3; j++) {
						vector2f pos = vVtx[ tri.p[j] ];
						glVertex3f(pos.x, pos.y, 0);
					}
				}
			glEnd();
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	glPopMatrix();


	//-----------------
	//  銀河系の星の描画
	//-----------------
	const float alp[4] = { 0.4, 0.4, 0.3, 0.3 };
	const float star_alpha2 = star_alpha * alpha;
	const int N0 = vGalaxyPoints[0].size();

	//-----
	//---  カメラ座標系の軸
	vector3d  eF, eU, eR;
	ds.camera.get_axes(eF, eU, eR);
	const double aaa = 0.3 / get_screen_z() * get_screen_w() / 180;
	//-----


	if (star_alpha2 > 0.01f && star_point_num > 10) {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glPointSize(star_pt_size);

		glPushMatrix();
			for (int ty=0; ty<4; ty++) {
				const vector<vector3f>& vPt = vGalaxyPoints[ty];
				float c = alp[ty] * star_alpha2;
				RGBf col = c * star_color[ty];

				int imax = star_point_num;
				imax *= float(vPt.size())/N0;
				if (imax > vPt.size()) { imax = vPt.size(); }

				for (int i=0; i<imax; i++) {
					const vector3f pos = vPt[i];

					float tauR = calc_galactic_optical_depth(camera_pos, pos, 3);
					if (tauR > 2.9)  continue;

					int idx = int(tauR * ExpTblFac);
					if (idx > ExpTblN) idx = ExpTblN;

					float _r, _g, _b;
					_r = col.r * ExpTbl[idx][0];
					_g = col.g * ExpTbl[idx][1];
					_b = col.b * ExpTbl[idx][2];
					glColor3f(_r, _g, _b);


					if (true) {
						vector3f  rr = pos - rge + rpos;

						double R2, R;
						R2 = norm2(rr);
						if (R2 < 1e-100)  continue;
							
						R = sqrt(R2);
						draw_point(ds, aaa, R, rr, ds.eRf, ds.eUf);
					} else {
						glBegin(GL_POINTS);
							glVertex3fv( pos.get_pointer() );
						glEnd();
					}
				}
			}
		glPopMatrix();
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}




//---------------------------------------
//  r1 と r2 の間の optical depth を求める
//  （r1g, r2g は銀河座標系）
//---------------------------------------
float
calc_galactic_optical_depth_g(const vector3f& r1g, const vector3f& r2g, const float tau_max)
{
	//--  計算の始点と終点を求める（楕円体との交点） --
	const float qgi = 1.0f/qg;
	const vector3f r12 = r2g - r1g;
	const vector3f r12q(r12.x, r12.y, r12.z*qgi);
	const float l12q = norm(r12q);
	const vector3f nq = r12q / l12q;
	const vector3f r1q = vector3f(r1g.x, r1g.y, r1g.z*qgi);

	const float b = dot_product(nq, r1q);
	const float c = r1q.norm2() - Rmax_sqr;
	const float D = b*b - c;
	if (D <= 0.0f)  return 0;	// 交差しない

	const float d = sqrtf(D);
	float t1, t2;
	t1 = -b -d;
	t2 = -b +d;

	if (t1 > l12q) {
		return 0.0f;	// 銀河系よりも十分手前
	}
	if (t2 > l12q) {
		t2 = l12q;
	}
	if (t1 < 0.0f) {
		t1 = 0.0f;
	}

	vector3f r, dr;
	r = t1*nq + r1q;
	const float l = t2 - t1;		// パスの長さ


	//--  Optical depth の計算  --
	const vector3f n  = normalize(r12);
	const float nx = n.x;
	const float ny = n.y;
	const float nz = n.z;

	float x, y, z;
	x = r.x;
	y = r.y;
	z = r.z;


	//--  最初の ds の決定  --
	const float dlt = 0.27f;
	const float cs_min = 0.3f;
	float cs = fabs(nz);
	if (cs < cs_min) cs = cs_min;

	const float ds_c1 = GalaxyRayStepLen / (1.0f+dlt)/cs;
	const float ds_c2 = dlt / (1.0f+dlt)/cs;
	float ds = ds_c1 + fabs(z)*ds_c2;

	float s, tau;
	s = 0.0f;
	tau = 0.0f;

	while (s < l) {
		const float rho = sqrtf(x*x + y*y);
		const float kpRds = ds * kp0 * exp_d(rho*hdr_inv + fabs(z)*hdz_inv);
		tau += kpRds;
		if (tau > tau_max)  return tau;

		x += nx * ds;
		y += ny * ds;
		z += nz * ds;
		s += ds;

		ds = ds_c1 + fabs(z)*ds_c2;
	}

	return tau;
}
