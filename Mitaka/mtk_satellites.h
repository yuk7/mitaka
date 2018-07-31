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
#ifndef __MTK_SATELLITES_H__
#define __MTK_SATELLITES_H__
#include  "../common/math/vector3.h"
#include  "../common/win_file.h"
#include  "mtk_satellite.h"
#include  "mtk_draw_struct.h"
#include  "mtk_space_curve.h"


//-------------
//  Constants
//-------------
const float R_moon_km = 1738.092;
const float SatelliteNameSizeDef = 2.5f;



//-------------
//  Functions
//-------------

//--  Scale  --
double	get_scale_satellite(int idx);
//--  Label  --
bool	is_satellite_label_visible(int idx, bool bTgt);
float	get_satellite_label_alpha(const mtk_draw_struct& ds, int idx);

//--
double	get_landing_radius_satellite(int idx);


//--  Load Textures  --
void	load_satellites_textures();


//--  Text size  --//
void	set_satellites_name_size(float sz);
float	get_satellites_name_size();



//--  Orbits  --
void	register_satellites_orbit_curves();
void	make_satellites_orbit_curve(space_curve& sc, double t, int div_N);
void	set_satellites_orbit_curve(space_curve& sc, double t);
float	get_satellite_orbit_alpha(const mtk_draw_struct& ds, int idx);



//--  Draw  --
bool	prepare_draw_satellite(const mtk_draw_struct& ds, int idx, double* pRb, double* pRh);
void	draw_a_satellite(const mtk_draw_struct& ds, int idx, const vector3d& rpos, const double& scl);
void	draw_moon_planetarium(const mtk_draw_struct& ds, const double& Rc, char atm_mode, float I0);
void	draw_moon_label_planetarium(const mtk_draw_struct& ds, const double& Rc);


//--  Hit-test
bool	hit_test_satellite(const mtk_draw_struct& ds, int idx, const vector3d& rpos, const vector3d& nv);

//--  Info
void	make_object_info_satellite(const mtk_draw_struct& ds, int idx, std::vector<std::string>& text);


//****
void	draw_satellites_planetarium(const mtk_draw_struct& ds, const double& Rc_pl);




//-------------
//  Functions
//-------------


//--  Number
int			get_satellites_num();

//--  Satellite
int			get_satellite_index(const std::string& key);
satellite&	get_satellite(int idx);



//---  Planet ID
int		get_satellite_planet_index(int idx);


//--  key
const std::string&	get_satellite_key(int idx);





//--  Size
void		get_satellite_size(int mode, int idx, double& Rx, double& Ry, double& Rz);


//--  Places and topography
void	init_satellite_places_and_topography();


//--  Position
vector3d	get_satellite_position(int idx);
void		set_satellite_position(int idx, const vector3d& pos);

vector3d	calc_satellite_orbit_pos(int idx, const double& t, float phi);
vector3d	get_planetocentric_satellite_position(int idx);
vector3d	calc_lunar_real_position(const double& t);


//--  Zoom rate
void	set_satellites_zoom_rate(int mode);



//--  Load
bool	load_satellites(const directory& dir);


//--  Selection
bool	is_satellite_selected(int idx);

#endif
