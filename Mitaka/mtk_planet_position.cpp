/**************************************************************************************************

Copyright (c) 2007-2015   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#include  "mitaka.h"
#include  "mtk_planet_position.h"
#include  "mtk_solar_system.h"
#include  "mtk_planets.h"
#include  "mtk_satellites.h"
#include  "../common/astro/astro_const.h"
#include  "../common/astro/astrometry.h"
#include  "../common/astro/cele_mech.h"
#include  "../common/astro/cf96.h"
#include  "mtk_orbital_element.h"
#include  "mtk_planet_orientation.h"
#include  "mtk_lunar_pos.h"
#include  "mtk_switches.h"
#include  "mtk_sun.h"
#include  "mtk_trajectory.h"






//----------
//  Tables
//----------

//---  惑星の軌道要素  ---//
struct planet_orbital_elements {
    double epoch;	// 元期(JD)
	double a;		// 軌道長半径(AU)
	double e;		// 軌道離心率
	double T;		// 公転周期（年）...未使用
	double i;		// 軌道傾角（度）
	double node;	// 昇交点黄経（度）
	double omg_bar;	// 近日点黄経（度）
	double L;		// 平均黄経（度）
};

const double JD0 = 2452640.5;	// 元期 = 2003/1/1.0
const planet_orbital_elements  plnt_oe[] = {
	{ JD0,  0.38710, 0.20563,   0.240852,  7.0050,  48.3665,  77.5028,  55.4500 },			// 水星
	{ JD0,  0.72333, 0.00677,   0.615207,  3.3947,  76.7069, 131.6058, 137.1556 },			// 金星
	{ JD0,  1.00000, 0.01671,   1.000040,  0.0000,   0.0000, 102.9889, 100.2431 },			// 地球
	{ JD0,  1.52368, 0.09340,   1.880866,  1.8497,  49.5812, 336.1155, 209.5529 },			// 火星
	{ JD0,  5.20260, 0.04850,  11.86155,   1.3031, 100.4950,  14.3796, 125.4198 },			// 木星
	{ JD0,  9.55491, 0.05554,  29.53216,   2.4888, 113.6918,  93.1161,  86.7744 },			// 土星
	{ JD0, 19.21845, 0.04638,  84.25300,   0.7732,  74.0216, 173.0499, 326.9480 },			// 天王星
	{ JD0, 30.11039, 0.00946, 165.2269,    1.7697, 131.8171,  48.1631, 310.9437 },			// 海王星
	{ JD0, 39.54044, 0.24901, 248.6449,   17.1449, 110.3587, 224.1833, 242.8665 }			// 冥王星
};







//-------------
//  Constants
//-------------
const int PlanetNum = 9;

//---------
//  Array
//---------
static vector3d  vPlanetPos[PlanetNum];
static vector3d  vPlanetVel[PlanetNum];


static mtk_trajectory trajVenus;			// Pluto BC


static mtk_trajectory trajPlutoBC;			// Pluto BC
static CF96_planets  cf96;					// cf96, planetary series
static orbital_elements  vPlanetsOrbitalElements[PlanetNum];




//--------------------------
//  Planets and Satellites
//--------------------------
void	update_planets_position(const double& t);
void	update_satellites_position(const double& t);


void
update_planets_and_satellites_position(const double& t, bool force)
{
	const double DT_MIN = 0.01/(24.0*60.0*60.0);
	static bool first = true;
	static double t_last;
	static int SAT_MOON, SAT_CHARON, SAT_STYX, SAT_NIX, SAT_KERBEROS, SAT_HYDRA;

	if (first)	{
		SAT_MOON     = get_satellite_index("SAT_E_MOON");
		SAT_CHARON   = get_satellite_index("SAT_P_CHARON");
		SAT_STYX     = get_satellite_index("SAT_P_STYX");
		SAT_NIX      = get_satellite_index("SAT_P_NIX");
		SAT_KERBEROS = get_satellite_index("SAT_P_KERBEROS");
		SAT_HYDRA    = get_satellite_index("SAT_P_HYDRA");
		first = false;
	}
	else if (!force) { if (fabs(t - t_last) < DT_MIN)  return; }

	update_Sun_position(t);
	update_planets_position(t);
	update_satellites_position(t);

	//--  Earth and Moon
	const double R_EM = 81.30057;
	const vector3d dr_EM   = (vPlanetPos[PLNT_EARTH] - get_satellite_position(SAT_MOON) ) / (R_EM + 1.0);
	vPlanetPos[PLNT_EARTH]  += dr_EM;
	set_satellite_position( SAT_MOON, get_satellite_position(SAT_MOON) + dr_EM );

	//--  Pluto and its satellites
	// Charon...Pluto centric, Others...P-C barycenteric
	const double R_PC = 8.213;
	if (trajPlutoBC.isLoaded()) {
		const double dlt = 1.0 / R_PC;
		const vector3d rB  = vPlanetPos[PLNT_PLUTO];	// この時点ではBCの位置が入っている
		const vector3d rCP = get_satellite_position(SAT_CHARON) - rB;

		vPlanetPos[PLNT_PLUTO] -= dlt / (1.0 + dlt) *rCP;
		set_satellite_position(SAT_CHARON, rB + 1.0 / (1.0 + dlt)*rCP);
	}
	else {
		const vector3d dr_PC = (get_satellite_position(SAT_CHARON) - vPlanetPos[PLNT_PLUTO]) / (R_PC + 1.0);	// barycenter rel to Pluto pos
		set_satellite_position(SAT_STYX, get_satellite_position(SAT_STYX) + dr_PC);
		set_satellite_position(SAT_NIX, get_satellite_position(SAT_NIX) + dr_PC);
		set_satellite_position(SAT_KERBEROS, get_satellite_position(SAT_KERBEROS) + dr_PC);
		set_satellite_position(SAT_HYDRA, get_satellite_position(SAT_HYDRA) + dr_PC);
	}

	t_last = t;
}



//-----------
//  Planets
//-----------

//--- Venus の読み込み
bool
load_Venus(const char *filename)
{
	return trajVenus.load(filename, 100);
}

//--- Pluto Barycenter の読み込み
bool
load_Pluto_BC(const char *filename)
{
	return trajPlutoBC.load(filename, 100);
}



//--- CF96 の読み込み
bool
load_CF96(const char* filename)
{
	return  cf96.load( filename );
}

//---  Captine et al. 2003  ---
void
calc_Earth_ec_matrix_Captine2003(const double& t, matrix3d& M)
{
	const double JD0 = 2451545.0;	// 元期 2000/1/1.5
	const double a1 = 5038.481507; 
	const double a2 = -1.0790069;
	const double a3 = -0.00114045;
	const double a4 = 0.000132851;
	const double a5 = -0.0000000951;
	const double b1 = -0.025754;
	const double b2 =  0.0512623;
	const double b3 = -0.00772503;
	const double b4 = -0.000000467;
	const double b5 =  0.0000003337;
	const double c1 = 10.556403;
	const double c2 = -2.3814292;
	const double c3 = -0.00121197;
	const double c4 =  0.000170663;
	const double c5 = -0.0000000560;
	const double eps0 = 84381.406 / 3600.0;
	double PsiA, omgA, chiA, epsA;
	double d, T, T2, T3, T4, T5;

	d  = t - JD0;
	T  = d/36525.0;		// time in Julian centuries
	T2 = T*T;
	T3 = T*T2;
	T4 = T2*T2;
	T5 = T4*T;

	PsiA = (a1*T + a2*T2 + a3*T3 + a4*T4 + a5*T5) / 3600.0;
	omgA = eps0 + (b1*T + b2*T2 + b3*T3 + b4*T4 + b5*T5) / 3600.0;
	chiA = (c1*T + c2*T2 + c3*T3 + c4*T4 + c5*T5) / 3600.0;
	epsA = eps0 + (-46.836769*T - 0.0001831*T2 + 0.00200340*T3
			-0.000000576*T4 - 0.0000000434*T5) / 3600.0;

	M = Rx3deg(epsA) * Rzxz3deg(-chiA,   omgA, PsiA) * Rx3deg(-eps0);
}




//--- 軌道要素の初期化
void
init_planets_orbital_elements()
{
	for (int i=0; i<PlanetNum; i++) {
		const planet_orbital_elements& oe0 = plnt_oe[i];
		const double n = 0.985607669/pow(oe0.a, 3.0/2.0);
		vPlanetsOrbitalElements[i].set_parameters(oe0.epoch, oe0.a, oe0.e, oe0.L - oe0.omg_bar, n,
												  oe0.node, oe0.i, oe0.omg_bar - oe0.node);
	}
}

void
update_planets_position(const double& t)
{
	static bool first = true;
	static matrix3d  MT_CF96;

	if (first) {
		first = false;
		MT_CF96 = double(_1AU_pc) * Rx3deg(-epsJ2000);
	}

	if (cf96.isInRange(t)) {
		// CF96 が読み込まれていて、有効な時刻の場合は、それを使う
		// (赤道座標系から、黄道座標系になおす)


		const vector3d sun_pos = get_sun_position();
		for (int i=0; i<PlanetNum; i++) {

			if (i == PLNT_VENUS && trajVenus.isLoaded()) {
				vPlanetPos[i] = trajVenus.get_pos(t);
			} else if (i == PLNT_PLUTO && trajPlutoBC.isLoaded()) {
				vPlanetPos[i] = trajPlutoBC.get_pos(t);
			}
			else {
				vPlanetPos[i] = MT_CF96 * cf96.eval_pos(i, t) + sun_pos;
			}
			vPlanetVel[i] = MT_CF96 * cf96.eval_vel(i, t);
		}


		// 軌道の描画用に接触軌道要素を計算しておく
		for (int i = 0; i<PlanetNum; i++) {
			set_orbital_elements(vPlanetPos[i], vPlanetVel[i], t, 1.0, vPlanetsOrbitalElements[i]);
		}

	} else {
		// それ以外は、平均軌道要素により計算する
		for (int i=0; i<PlanetNum; i++) {
			vPlanetsOrbitalElements[i].update_pos_and_velocity(t);
			if (i==2) {
				vPlanetPos[i] = double(_1AU_pc) * vPlanetsOrbitalElements[i].get_pos();
				vPlanetVel[i] = double(_1AU_pc) * vPlanetsOrbitalElements[i].get_velocity();

				//matrix3d M;
				//calc_Earth_ec_matrix_Captine2003(t, M);
				//vPlanetPos[i] = double(_1AU_pc) * M * vPlanetsOrbitalElements[i].get_pos();
			} else {
				vPlanetPos[i] = double(_1AU_pc) * vPlanetsOrbitalElements[i].get_pos();
				vPlanetVel[i] = double(_1AU_pc) * vPlanetsOrbitalElements[i].get_velocity();
			}
		}
	}
}


vector3d
get_planet_position(int idx)
{
	return  vPlanetPos[idx];
}


vector3d
get_planet_velocity(int idx)
{
	return  vPlanetVel[idx];
}



vector3d
calc_planet_orbit_pos(int idx, float phi)
{
	return  double(_1AU_pc) * vPlanetsOrbitalElements[idx].calc_orbit_pos(phi);
}

//--- 軌道離心率 ---
double
get_planet_a(int idx)
{
	return plnt_oe[idx].a;
}





// CF96 のドキュメントに示されている値との比較用ファイルを出力する
void
test_CF96(const double& t)
{
	FILE *fp;

	fp = fopen("CF96_test.txt", "wt");
	if (fp == NULL) return;

	for (int i = 0; i<PlanetNum; i++) {
		vector3d pos, vel;

		pos = cf96.eval_pos(i, t);
		vel = cf96.eval_vel(i, t);
		fprintf(fp, "%d\n", i);
		fprintf(fp, "%.12f %.12f %.12f\n", pos.x, pos.y, pos.z);
		fprintf(fp, "%.12f %.12f %.12f\n", vel.x, vel.y, vel.z);
		fprintf(fp, "\n");
	}

	fclose(fp);
}
