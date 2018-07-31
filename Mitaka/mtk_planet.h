/******************************************************************************

Written by Tsunehiko Kato
Copyright (c) 2015   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#ifndef __MTK_PLANET_H__
#define __MTK_PLANET_H__
#include  "mtk_places.h"
#include  "mtk_topography.h"
#include  "../common/graphics/gl_common.h"
#include  "mtk_planetary_ring.h"
#include  "mtk_planet_eclipse.h"
#include  <string>


enum
{
	PLNT_MERCURY,
	PLNT_VENUS,
	PLNT_EARTH,
	PLNT_MARS,
	PLNT_JUPITER,
	PLNT_SATURN,
	PLNT_URANUS,
	PLNT_NEPTUNE,
	PLNT_PLUTO
};



class planet
{
	//---  Key
	std::string key;


	//---  Textures
	std::string tex_file_name;
	GLuint tex_idx;


	//---  Ring
	int ring_idx;


	//---  Places
	Places *plc;

	//---  Topography
	TopoSphere *topo;

	//---  orientation
	int		ori_idx;

	//---  Parameters
	double	Re;			// 赤道半径(km)
	double  f;			// 扁率

	//---  Eclipse info
	std::vector<eclipse_obj_info>  vEclipse;

	//---  Position
	//vector3d pos;



public:
	void	init(const std::string& _key, double _Re, double _f, const std::string& tex_fn);

	// Key
	const std::string& get_key() const { return key;  }

	// Parameters
	double		get_Re() const	{ return Re; }
	double		get_f() const	{ return f; }

	// Textures
	GLuint		load_texture(int shrink_rate);
	GLuint		get_texture_index() const	{ return tex_idx; }

	// Ring
	void		add_ring(int _ring_idx) { ring_idx = _ring_idx; }
	bool				hasRing() const	{ return (ring_idx >= 0); }
	particle_ring&	     get_ring()       { return get_particle_ring(ring_idx); }
	const particle_ring& get_ring() const { return get_particle_ring(ring_idx); }

	// Places
	bool		has_places() const		{ return (plc != 0); }
	void		set_places(Places *p)	{ plc = p; }
	Places*		get_places() { return plc; }

	// Topography
	bool		has_topography() const	{ return (topo != 0); }
	void		set_topography(TopoSphere *tp)  { topo = tp; }
	TopoSphere*	get_topography() { return topo; }

	// Orientation
	void		set_orientation_index(int idx) { ori_idx = idx; }
	int			get_orientation_index() const  { return ori_idx; }

	// Eclipse
	void		add_eclipse(const std::string& sat_key, const vector3f& col);
	const std::vector<eclipse_obj_info>&  get_eclipse_info() const  { return vEclipse; }
};


#endif
