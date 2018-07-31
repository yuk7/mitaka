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
#ifndef __MTK_MINOR_PLANETS_H__
#define __MTK_MINOR_PLANETS_H__
#include  <stdio.h>
#include  "mtk_draw_struct.h"
#include  "mtk_space_curve.h"
#include  "mtk_orbital_element.h"



const float DwarfPlanetNameSizeDef	= 2.6f;
const float SmallObjectNameSizeDef	= 2.4f;





//--  Object Type  --
enum {
	SMALLOBJ_TYPE_UNDEF,
	SMALLOBJ_TYPE_ATIRA,
	SMALLOBJ_TYPE_ATEN,
	SMALLOBJ_TYPE_APOLLO,
	SMALLOBJ_TYPE_AMOR,
	SMALLOBJ_TYPE_OBJ_SMALL_Q,
	SMALLOBJ_TYPE_HUNGARIA,
	SMALLOBJ_TYPE_PHOCAEA,
	SMALLOBJ_TYPE_HILDA,
	SMALLOBJ_TYPE_JUPITER_TROJAN,
	SMALLOBJ_TYPE_CENTAUR,
	SMALLOBJ_TYPE_SDO,
	SMALLOBJ_TYPE_TNO
};




//-----------------
//  Small objects
//-----------------
struct  small_object
{
	std::string  key;		// Key

	int  type;				// タイプ
	int  no;				// 小惑星番号
	int  obj_idx;			// オブジェクトのインデックス
	int  phys_param_idx;
	int  ring_idx;
	bool bSelected;			// 選択されているか
	bool bHasLabel;			// ラベルを持つか
	bool bDwarfPlanet;		// Dwarf planet か
	orbital_elements  oe;	// 軌道要素

	int ori_idx;			// orientation

	vector3f	eR, eU;		// ビルボードの辺の向き
};




//---  Load / Init  ---
int			load_small_objects(FILE* fp);


//---  Object
int	get_small_object_index(const std::string& key);


//---  Position  ---
vector3d	get_small_object_position(int idx);
//---  Scale  ---
double		get_scale_small_object(int idx);
//---  Label  ---
bool		is_small_object_label_visible(int idx, bool bTgt);

//--
double		get_landing_radius_small_object(int idx);

//---  Textures  ---
void	load_minor_planet_textures();



//---  Draw  ---
bool		prepare_draw_small_object(const mtk_draw_struct& ds, int idx, double* pRb, double* pRh);
float		get_minor_planet_point_alpha(const mtk_draw_struct& ds, int idx);

void		draw_a_small_object(const mtk_draw_struct& ds, int idx, const vector3d& rpos, const double& scl);
void		draw_a_small_object_point(const mtk_draw_struct& ds, int idx, const vector3d& rpos, float alpha);



int			get_small_objects_type_num();
int			get_small_objects_num();
small_object&	get_small_object(int idx);

void		update_small_objects_position(const double& t);
int			get_small_object_type(int idx);
bool		is_small_object_selected(int idx);
void		get_small_object_size(int idx, double& Rx, double& Ry, double& Rz);
void		setup_drawing_small_object(int idx);

void		register_small_objects_orbit_curves();
void		make_small_objects_orbit_curve(space_curve& sc, int div_N);
float		get_small_object_label_alpha(const mtk_draw_struct& ds, int idx);
float		get_small_object_orbit_alpha(const mtk_draw_struct& ds, int idx);
double		calc_visible_radius_small_object(int idx);

//-- Text size --
float	get_dwarf_planet_name_size();
float	get_small_object_name_size();
void	set_dwarf_planet_name_size(float sz);
void	set_small_object_name_size(float sz);


//--  Hit-test  --
bool	hit_test_small_object(const mtk_draw_struct& ds, int idx, const vector3d& rpos, const vector3d& nv);


//--  Info  --
void	make_object_info_small_object(const mtk_draw_struct& ds, int idx, std::vector<std::string>& text);





#endif
