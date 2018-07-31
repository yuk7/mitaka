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
#include  "mtk_satellite_orientation.h"
#include  "../common/astro/astrometry.h"
#include  "mtk_object_orientation.h"
#include  "mtk_satellites.h"




enum {
	SAT_ORI_MOON,
	SAT_ORI_PHOBOS,
	SAT_ORI_DEIMOS,
	SAT_ORI_IO,
	SAT_ORI_EUROPA,
	SAT_ORI_GANYMEDE,
	SAT_ORI_CALLISTO,
	SAT_ORI_MIMAS,
	SAT_ORI_ENCELADUS,
	SAT_ORI_TITAN,
	SAT_ORI_IAPETUS,
	SAT_ORI_TRITON,
	SAT_ORI_CHARON,
	SAT_ORI_NUM			// Number
};



struct sat_ori_info {
	const char*  obj_key;
	int ori_idx;
};



const sat_ori_info soi[SAT_ORI_NUM] = {
	{"SAT_E_MOON",      SAT_ORI_MOON},
	{"SAT_M_PHOBOS",    SAT_ORI_PHOBOS},
	{"SAT_M_DEIMOS",    SAT_ORI_DEIMOS},
	{"SAT_J_IO",        SAT_ORI_IO},
	{"SAT_J_EUROPA",    SAT_ORI_EUROPA},
	{"SAT_J_GANUMEDE",  SAT_ORI_GANYMEDE},
	{"SAT_J_CALLISTO",  SAT_ORI_CALLISTO},
	{"SAT_S_MIMAS",     SAT_ORI_MIMAS},
	{"SAT_S_ENCELADUS", SAT_ORI_ENCELADUS},
	{"SAT_S_TITAN",     SAT_ORI_TITAN},
	{"SAT_S_IAPETUS",   SAT_ORI_IAPETUS},
	{"SAT_N_TRITON",    SAT_ORI_TRITON},
	{"SAT_P_CHARON",    SAT_ORI_CHARON}
};




void
init_satellite_orientation()
{
	for (int i = 0; i < SAT_ORI_NUM; i++) {
		const sat_ori_info& so = soi[i];
		int ori_idx, sat_idx;

		const std::string& key = so.obj_key;
		sat_idx = get_satellite_index(key);

		if (sat_idx != -1) {
			satellite& st = get_satellite(sat_idx);

			ori_idx = add_orientation(key, ORIENTATION_SATELLITE, i);
			st.set_orientation_index(ori_idx);
		}
	}
}




const matrix3d&
get_satellite_orientation(int idx, bool bRotation)
{
	const satellite& st = get_satellite(idx);
	return get_orientation(st.get_orientation_index(), bRotation);
}


void
set_satellite_orientation_GL(int idx, const double& W_shift)
{
	const satellite& st = get_satellite(idx);
	int ori_idx = st.get_orientation_index();

	set_object_orientation_GL(ori_idx);
	glRotated(W_shift, 0, 0, 1);
}




//-------------------------
//  月の向きを求める  Ref[1]
//-------------------------
void
calc_moon_orientation_angle(const double& t, double& alp, double& dlt, double& W)
{
	const double  JD0 = 2451545.0;	// 元期 2000/1/1.5
	const double  d = t - JD0;
	const double  T = d / 36525.0;

	double E1, E2, E3, E4, E5, E6, E7, E8, E9, E10, E11, E12, E13;
	E1 = 125.045 - 0.0529921 *d;
	E2 = 250.089 - 0.1059842 *d;
	E3 = 260.008 + 13.0120009*d;
	E4 = 176.625 + 13.3407154*d;
	E5 = 357.529 + 0.9856003 *d;
	E6 = 311.589 + 26.4057084*d;
	E7 = 134.963 + 13.0649930*d;
	E8 = 276.617 + 0.3287146 *d;
	E9 = 34.226 + 1.7484877 *d;
	E10 = 15.134 - 0.1589763 *d;
	E11 = 119.743 + 0.0036096 *d;
	E12 = 239.961 + 0.1643573 *d;
	E13 = 25.053 + 12.9590088*d;

	const double& a = D2R;
	double sinE1, sinE2, sinE3, sinE4, sinE5, sinE6, sinE7;
	double sinE8, sinE9, sinE10, sinE11, sinE12, sinE13;
	double cosE1, cosE2, cosE3, cosE4, cosE6, cosE7, cosE10, cosE13;
	sinE1 = sin(a*E1);
	sinE2 = sin(a*E2);
	sinE3 = sin(a*E3);
	sinE4 = sin(a*E4);
	sinE5 = sin(a*E5);
	sinE6 = sin(a*E6);
	sinE7 = sin(a*E7);
	sinE8 = sin(a*E8);
	sinE9 = sin(a*E9);
	sinE10 = sin(a*E10);
	sinE11 = sin(a*E11);
	sinE12 = sin(a*E12);
	sinE13 = sin(a*E13);
	cosE1 = cos(a*E1);
	cosE2 = cos(a*E2);
	cosE3 = cos(a*E3);
	cosE4 = cos(a*E4);
	cosE6 = cos(a*E6);
	cosE7 = cos(a*E7);
	cosE10 = cos(a*E10);
	cosE13 = cos(a*E13);

	alp = 269.9949 + 0.0031*T - 3.8787*sinE1 - 0.1204*sinE2
		+ 0.0700*sinE3 - 0.0172*sinE4 + 0.0072*sinE6
		- 0.0052*sinE10 + 0.0043*sinE13;
	dlt = 66.5392 + 0.0130*T + 1.5419*cosE1 + 0.0239*cosE2
		- 0.0278*cosE3 + 0.0068*cosE4 - 0.0029*cosE6
		+ 0.0009*cosE7 + 0.0008*cosE10 - 0.0009*cosE13;
	W = 38.3213 + 13.17635815*d - 1.4e-12 *d*d + 3.5610*sinE1
		+ 0.1208*sinE2 - 0.0642*sinE3 + 0.0158*sinE4
		+ 0.0252*sinE5 - 0.0066*sinE6 - 0.0047*sinE7
		- 0.0046*sinE8 + 0.0028*sinE9 + 0.0052*sinE10
		+ 0.0040*sinE11 + 0.0019*sinE12 - 0.0044*sinE13;
}

//---------------------------
//  衛星の向きを求める  Ref[2]
//---------------------------
void
calc_satellite_orientation_angle(int ori_id, const double& t, double& alp, double& dlt, double& W)
{
	if (ori_id == SAT_ORI_MOON) {
		calc_moon_orientation_angle(t, alp, dlt, W);
		return;
	}

	const double  JD0 = 2451545.0;	// 元期 2000/1/1.5
	const double  d = t - JD0;
	const double  T = d / 36525.0;
	const double  T2 = T*T;

	switch (ori_id) {
		//== Mars ==
	case SAT_ORI_PHOBOS:	// Phobos
	{
		const double M1 = D2R * (169.51 - 0.4357640*d);
		const double M2 = D2R * (192.93 + 1128.4096700*d + 8.864*T2);
		const double sinM1 = sin(M1);
		alp = 317.68 - 0.108*T + 1.79*sinM1;
		dlt = 52.90 - 0.061*T - 1.08*cos(M1);
		W = 35.06 + 1128.8445850*d + 8.864*T2 - 1.42*sinM1 - 0.78*sin(M2);
	}
	break;
	case SAT_ORI_DEIMOS:	// Deimos
	{
		const double M3 = D2R * (53.47 - 0.0181510*d);
		const double sinM3 = sin(M3);
		const double cosM3 = cos(M3);
		alp = 316.65 - 0.108*T + 2.98*sinM3;
		dlt = 53.52 - 0.061*T - 1.78*cosM3;
		W = 79.41 + 285.1618970*d - 0.520*T2 - 2.58*sinM3 + 0.19*cosM3;
	}
	break;
	//== Jupiter ==
	case SAT_ORI_IO:	// Io
	{
		const double J3 = D2R * (283.90 + 4850.7*T);
		const double J4 = D2R * (355.80 + 1191.3*T);
		const double sinJ3 = sin(J3);
		const double sinJ4 = sin(J4);
		alp = 268.05 - 0.009*T + 0.094*sinJ3 + 0.024*sinJ4;
		dlt = 64.50 + 0.003*T + 0.040*cos(J3) + 0.011*cos(J4);
		W = 200.39 + 203.4889538*d - 0.085*sinJ3 - 0.022*sinJ4;
	}
	break;
	case SAT_ORI_EUROPA:	// Europa
	{
		const double J4 = D2R * (355.80 + 1191.3*T);
		const double J5 = D2R * (119.90 + 262.1*T);
		const double J6 = D2R * (229.80 + 64.3*T);
		const double J7 = D2R * (352.25 + 2382.6*T);
		const double sinJ4 = sin(J4);
		const double sinJ5 = sin(J5);
		const double sinJ6 = sin(J6);
		const double sinJ7 = sin(J7);
		alp = 268.08 - 0.009*T + 1.086*sinJ4 + 0.060*sinJ5 + 0.015*sinJ6 + 0.009*sinJ7;
		dlt = 64.51 + 0.003*T + 0.468*cos(J4) + 0.026*cos(J5) + 0.007*cos(J6) + 0.002*cos(J7);
		W = 36.022 + 101.3747235*d - 0.980*sinJ4 - 0.054*sinJ5 - 0.014*sinJ6 - 0.008*sinJ7;
	}
	break;
	case SAT_ORI_GANYMEDE:	// Ganymede
	{
		const double J4 = D2R * (355.80 + 1191.3*T);
		const double J5 = D2R * (119.90 + 262.1*T);
		const double J6 = D2R * (229.80 + 64.3*T);
		const double sinJ4 = sin(J4);
		const double sinJ5 = sin(J5);
		const double sinJ6 = sin(J6);
		alp = 268.20 - 0.009*T - 0.037*sinJ4 + 0.431*sinJ5 + 0.091*sinJ6;
		dlt = 64.57 + 0.003*T - 0.016*cos(J4) + 0.186*cos(J5) + 0.039*cos(J6);
		W = 44.064 + 50.3176081*d + 0.033*sinJ4 - 0.389*sinJ5 - 0.082*sinJ6;
	}
	break;
	case SAT_ORI_CALLISTO:	// Callisto
	{
		const double J5 = D2R * (119.90 + 262.1*T);
		const double J6 = D2R * (229.80 + 64.3*T);
		const double J8 = D2R * (113.35 + 6070.0*T);
		const double sinJ5 = sin(J5);
		const double sinJ6 = sin(J6);
		const double sinJ8 = sin(J8);
		alp = 268.72 - 0.009*T - 0.068*sinJ5 + 0.590*sinJ6 + 0.010*sinJ8;
		dlt = 64.83 + 0.003*T - 0.029*cos(J5) + 0.254*cos(J6) - 0.004*cos(J8);
		W = 259.51 + 21.5710715*d + 0.061*sinJ5 - 0.533*sinJ6 - 0.009*sinJ8;
	}
	break;

	//== Saturn ==
	case SAT_ORI_MIMAS:		// Mimas [2]
	{
		const double S3 = D2R * (177.40 - 36505.5 * T);
		const double S5 = D2R * (316.45 + 506.2 * T);
		alp = 40.66 - 0.036*T + 13.56 * sin(S3);
		dlt = 83.52 - 0.004 * T - 1.53 * cos(S3);
		W = 333.46 + 381.9945550 * d - 13.48 * sin(S3) - 44.85 * sin(S5);
	}
	break;
	case SAT_ORI_ENCELADUS:	// Enceladus [2]
		alp = 40.66 - 0.036*T;
		dlt = 83.52 - 0.004 * T;
		W = 6.32 + 262.7318996 * d;
		break;
	case SAT_ORI_TITAN:		// Titan [2]
		alp = 39.4827;
		dlt = 83.4279;
		W = 186.5855 + 22.5769768 * d;
		break;
	case SAT_ORI_IAPETUS:	// Iapetus [2]
		alp = 318.16 - 3.949 * T;
		dlt = 75.03 - 1.143 * T;
		W = 355.2 + 4.5379572 * d;
		break;

		//== Neptune ==
	case SAT_ORI_TRITON:	// Triton [2]
	{
		const double N7 = D2R * (177.85 + 52.316 * T);
		alp = 299.36 - 32.35 * sin(N7) - 6.28*sin(2 * N7) - 2.08*sin(3 * N7)
			- 0.74 * sin(4 * N7) - 0.28*sin(5 * N7) - 0.11 * sin(6 * N7)
			- 0.07 * sin(7 * N7) - 0.02 * sin(8 * N7) - 0.01*sin(9 * N7);
		dlt = 41.17 + 22.55 * cos(N7) + 2.10 * cos(2 * N7) + 0.55 * cos(3 * N7)
			+ 0.16 * cos(4 * N7) + 0.05 * cos(5 * N7) + 0.02 * cos(6 * N7)
			+ 0.01 * cos(7 * N7);
		W = 296.53 - 61.2572637*d + 22.25 * sin(N7) + 6.73*sin(2 * N7)
			+ 2.05*sin(3 * N7) + 0.74*sin(4 * N7) + 0.28*sin(5 * N7)
			+ 0.11*sin(6 * N7) + 0.05*sin(7 * N7) + 0.02 * sin(8 * N7)
			+ 0.01 * sin(9 * N7);
	}
	break;

	//== Pluto ==
	case SAT_ORI_CHARON:	// Charon [2]+[3]
		alp = 132.993;
		dlt = -6.163;
		W = 122.695 + 56.3625225*d;
		break;

	default:
		alp = W = 0.0;
		dlt = 90.0;
		break;
	}
}




void
calc_satellite_ec_to_sat_matrix(int ori_id, const double& t, matrix3d& M0, matrix3d& M)
{
	static matrix3d  M_J2000;
	static bool first = true;
	if (first) {
		first = false;
		M_J2000 = Rx3deg(epsJ2000);
	}

	// 衛星の回転
	double alp, dlt, W;
	calc_satellite_orientation_angle(ori_id, t, alp, dlt, W);

	M0 = Rzxz3deg(0.0, -90 + dlt, -90 - alp) * M_J2000;
	M  = Rz3deg(-W) * M0;
}




//------------------------------------------------
//  衛星中心からベクトル nv の方向の経度・緯度を求める
//  nv の成分は黄道座標(J2000)での値とする
//------------------------------------------------
vector2d
get_satellite_lon_lat_ec(int idx, const vector3d& nv)
{
	//--  惑星座標系のベクトル
	vector3d  ex, ey, ez;
	get_satellite_orientation(idx, true).get_bases_tr(ex, ey, ez);

	//--  lon, lat を求める
	double x, y, z, rho;
	double th, phi;
	double lon, lat;
	x = dot_product(nv, ex);
	y = dot_product(nv, ey);
	z = dot_product(nv, ez);
	rho = sqrt(x*x + y*y);

	th = atan2(rho, z);
	phi = atan2(y, x);

	lon = R2D * phi;
	lat = 90.0 - R2D * th;
	if (lon > 180.0) lon -= 360.0;

	return  vector2d(lon, lat);
}



////////////////////////////////////////
/// References
//
// [1] Seidelmann et al., 2002, Celestial Mechanics and Dynamical Astronomy, 82, 83-100
// [2] Archinal et al., 2011, Celestial Mechanics and Dynamical Astronomy, 109, 101-135
// [3] Archinal et al., 2011, Celestial Mechanics and Dynamical Astronomy, 110, 401-403 (Erratum)
