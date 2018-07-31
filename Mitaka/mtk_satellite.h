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
#ifndef __MTK_SATELLITE_H__
#define __MTK_SATELLITE_H__
#include  <windows.h>
#include  "../common/math/vector3.h"
#include  "../common/text/unicode.h"
#include  "../common/graphics/gl_common.h"
#include  "../common/win_file.h"
#include  "mtk_topography.h"
#include  "mtk_places.h"
#include  "mtk_planet_eclipse.h"
#include  <string>




//--------------
//  Orbit type
//--------------
enum {
	SAT_ORBIT_MOON,
	SAT_ORBIT_ECLIPTIC,
	SAT_ORBIT_LAPLACE,
	SAT_ORBIT_EQUATORIAL,
	SAT_ORBIT_ICRF
};




//------------------
//  Satellite data
//------------------
class satellite {
	//---  key
	std::string key;

	//---  planet key
	std::string	planet_key;
	int	planet_idx;

	//---  roman number
	std::string roman_no;

	//---  texture
	std::string	texture_name;	// texture filename
	GLuint tex_idx;				// OpenGL textur name

	//---  places
	Places *plc;

	//---  topography
	TopoSphere *topo;

	//---  orientation
	int		ori_idx;

	//---  Eclipse info
	std::vector<eclipse_obj_info>  vEclipse;


	//---  Phys params
	double	Rx, Ry, Rz;			// size(radius, in km): 惑星方向、垂直方向、自転方向

	//---  orbital elements
	int		orbit_type;	// type
	double	epoch;		// epoch
	double	a;			// semi-major axis（軌道長半径）
	double	e;			// eccentricity（離心率）
	double	omg;		// argument of periapsis（近点引数）
	double	M0;			// mean anomaly（平均近点角）
	double	inc;		// inclination（reference plane に対する軌道傾斜角）
	double	Omg;		// longitude of the ascending node（昇交点:reference planeとICRF equator のnode から測った角度）
	double	n;			// mean daily motion（平均日々運動）
	double	P;			// sidereal period（周期（恒星年））
	double	Pw;			// argument of periapsis precession period
	double	Pnode;		// longitude of the ascending node precession period
	double	RA;			// right ascension
	double	DEC;		// declination of the Laplace plane pole with respect to the ICRF
	double  Tilt;

	//
	vector3d pos;		// position
	matrix3d M_orb;		// orbit matrix
	double  orbit_amp_factor;


public:
	bool	load_from_file(FILE *fp);

	GLuint  load_texture(int shrink_rate);
	GLuint	get_texture_index() const;

	const std::string&	get_key() const				{ return key; }
	const std::string&	get_planet_key() const		{ return planet_key; }
	const std::string&  get_roman_number() const	{ return roman_no;  }

	int			get_planet_index() const		{ return planet_idx; }

	void		update_position(const double& t);
	vector3d	get_position() const		{ return pos; }
	void		set_position(const vector3d& _pos)	{ pos = _pos; }
	matrix3d	get_orbit_matrix() const	{ return M_orb; }
	vector3d	get_planetocentric_position() const;
	vector3d	calc_orbit_pos(const double& t, float phi) const;
	int			get_orbit_type() const		{ return orbit_type;  }

	double		get_Rx() const { return Rx; }
	void		get_real_size(double &_Rx, double& _Ry, double& _Rz) const { _Rx = Rx; _Ry = Ry; _Rz = Rz; }

	void		set_orbit_amp_factor(const double& fac) { orbit_amp_factor = fac; }

	void		set_places(Places *p)  { plc = p; }
	Places*		get_places() { return plc; }

	void		set_topography(TopoSphere *tp)  { topo = tp; }
	TopoSphere*	get_topography() { return topo; }

	void		set_orientation_index(int idx) { ori_idx = idx; }
	int			get_orientation_index() const  { return ori_idx; }

	// Eclipse
	void		add_eclipse(const vector3f& col);
	const std::vector<eclipse_obj_info>&  get_eclipse_info() const  { return vEclipse; }




private:
	matrix3d	calc_orbit_matrix(const double& t);
	vector3d	calc_planetocentric_positon(const double& t) const;
	vector3d	calc_position(const double& t) const;
};











#endif
