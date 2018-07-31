/**************************************************************************************************

Copyright (c) 2007-2015   Tsunehiko Kato

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
#include  "mtk_satellite.h"
#include  "../common/astro/astrometry.h"
#include  "../common/astro/cele_mech.h"
#include  "mtk_objects.h"
#include  "mtk_data_file.h"
#include  "mtk_OpenGL.h"
#include  "mtk_orbital_element.h"
#include  "mtk_planets.h"
#include  "mtk_planet_orientation.h"
#include  "mtk_planet_position.h"
#include  "mtk_lunar_pos.h"









bool
satellite::load_from_file(FILE *fp)
{
	key_data kd;


	kd = read_a_key_data(fp);
	if (kd.key.size() == 0) {
		return false;
	}


	// Key / names
	key = kd.key;
	planet_key = kd.data[0];
	roman_no = kd.data[1];
	texture_name = kd.data[2];


	//*** 惑星情報が先に読み込まれている必要がある
	planet_idx = get_object(planet_key).get_index();


	// places and topography
	plc = 0;
	topo = 0;

	tex_idx = 0;	// invalid texture name
	ori_idx = -1;


	// Radius
	Rx = stod(kd.data[3]);
	Ry = stod(kd.data[4]);
	Rz = stod(kd.data[5]);



	// Orbital plane type
	const string& obt_plane = kd.data[6];
	if (obt_plane == "ORBIT_MOON") {
		orbit_type = SAT_ORBIT_MOON;
	}
	else if (obt_plane == "ORBIT_ECLIPTIC") {
		orbit_type = SAT_ORBIT_ECLIPTIC;
	}
	else if (obt_plane == "ORBIT_LAPLACE") {
		orbit_type = SAT_ORBIT_LAPLACE;
	}
	else if (obt_plane == "ORBIT_EQUATORIAL") {
		orbit_type = SAT_ORBIT_EQUATORIAL;
	}
	else if (obt_plane == "ORBIT_ICRF") {
		orbit_type = SAT_ORBIT_ICRF;
	}
	else {
		// error
	}



	// Orbital elements
	epoch = stod(kd.data[7]);

	a   = stod(kd.data[8]);
	e   = stod(kd.data[9]);
	omg = stod(kd.data[10]);
	M0  = stod(kd.data[11]);
	inc = stod(kd.data[12]);
	Omg = stod(kd.data[13]);
	n   = stod(kd.data[14]);
	P   = stod(kd.data[15]);
	Pw  = stod(kd.data[16]);
	Pnode = stod(kd.data[17]);


	if (orbit_type == SAT_ORBIT_LAPLACE || orbit_type == SAT_ORBIT_ICRF) {
		RA   = stod(kd.data[18]);
		DEC  = stod(kd.data[19]);
		Tilt = stod(kd.data[20]);
	}


	// zoom rate
	//zoom_rate = 1.0;
	orbit_amp_factor = 1.0;


	return true;
}



//---  Textures
GLuint
satellite::load_texture(int shrink_rate)
{
	if (texture_name != "") {
		tex_idx = LoadTexture(texture_name.c_str(), shrink_rate, false);
	}
	else {
		tex_idx = 0;
	}

	return tex_idx;
}

GLuint
satellite::get_texture_index() const
{
	return  tex_idx;
}





//---  Eclipse
void
satellite::add_eclipse(const vector3f& col)
{
	eclipse_obj_info eci;

	eci.obj_idx = planet_idx;
	eci.col = col;
	vEclipse.push_back(eci);
}







//---  Position
//--------------------------------------
//  衛星座標から黄道座標への変換行列を求める
//--------------------------------------
//******** 月の変換行列は未実装  ***************
matrix3d
satellite::calc_orbit_matrix(const double& t)
{
	if (orbit_type == SAT_ORBIT_MOON) {
		return  matrix3d(1, 0, 0, 0, 1, 0, 0, 0, 1);
	}


	double domg, dOmg;
	double dt, _omg, _Omg;
	domg = (Pw != 0.0) ? 360.0 / Pw / 365.25 : 0.0;
	dOmg = (Pnode != 0.0) ? 360.0 / Pnode / 365.25 : 0.0;
	dt = t - epoch;
	_omg = omg + domg*dt;
	_Omg = Omg + dOmg*dt;

	switch (orbit_type) {
	case SAT_ORBIT_LAPLACE:		// ラプラス面を基準
		return  M_R2E * Rzxz3deg(90 + RA, 90 - DEC, 0.0) * Rzxz3deg(_Omg, inc, _omg);
	case SAT_ORBIT_ICRF:		// ICRFを基準
		return  M_R2E * Rzxz3deg(90 + RA, 90 - DEC, 0.0) * Rzxz3deg(_Omg, inc, _omg);
	case SAT_ORBIT_EQUATORIAL:	// 惑星赤道面を基準
		return  get_planet_orientation(planet_idx, false) * Rzxz3deg(_Omg, inc, _omg);
	case SAT_ORBIT_ECLIPTIC:	// 黄道面を基準
	default:
		return  Rzxz3deg(_Omg, inc, _omg);
	}
}


//---------------------------------
//  衛星の位置を求める(惑星中心座標)
//---------------------------------
vector3d
satellite::calc_planetocentric_positon(const double& t)  const
{
	//--  Moon  --
	if (orbit_type == SAT_ORBIT_MOON) {
		// Moon
		return  double(orbit_amp_factor * _1km_pc) * calc_lunar_pos(t);
	}



	//--  Other satellites  --
	vector3d  ro, rL, rICRF, rE;

	// omg, node の precession
	double domg, dOmg, _n;
	domg = (Pw != 0.0) ? 360.0 / Pw / 365.25 : 0.0;
	dOmg = (Pnode != 0.0) ? 360.0 / Pnode / 365.25 : 0.0;
	_n = n - domg - dOmg;

	// 軌道面上の位置
	ro = calc_orbital_pos(a*_1km_AU, e, M0, _n, epoch, t);
	rE = M_orb * ro;

	return  double(orbit_amp_factor * _1AU_pc) * rE;
}


//---------------------------------
//  衛星の位置を求める(日心黄道座標)
//  x が春分点、xy-平面が黄道面
//---------------------------------
vector3d
satellite::calc_position(const double& t) const
{
	return  calc_planetocentric_positon(t) + get_planet_position(planet_idx);
}


void
satellite::update_position(const double& t)
{
	M_orb = calc_orbit_matrix(t);
	pos = calc_position(t);
}


vector3d
satellite::get_planetocentric_position() const
{
	return  pos - get_planet_position(planet_idx);
}



vector3d
satellite::calc_orbit_pos(const double& t, float phi) const
{
	//--  Position in planetocentric coord in km
	vector3d pos0;
	if (orbit_type == SAT_ORBIT_MOON) {
		// Moon
		pos0 = calc_lunar_pos(t - 27.3*((phi - M_PI) / (2 * M_PI)));
	}
	else {
		// Other satellites
		pos0 = M_orb * calc_orbital_position(a, e, phi);
	}

	return  (_1km_pc * orbit_amp_factor) * pos0 + get_planet_position(planet_idx);
}
