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
#ifndef __MTK_OBJECT_H__
#define __MTK_OBJECT_H__
#include  "../common/math/vector3.h"
#include  "mtk_draw_struct.h"
#include  <vector>
#include  <string>




//---------------
//  Object Type
//---------------
enum {
	OBJ_TYPE_NONE,
	OBJ_TYPE_FLOATING_OBJECT,
	OBJ_TYPE_SUN,
	OBJ_TYPE_PLANET,
	OBJ_TYPE_SATELLITE,
	OBJ_TYPE_SMALL_OBJECT,
	//OBJ_TYPE_COMET,
	OBJ_TYPE_OORT_CLOUD,
	OBJ_TYPE_STAR,
	OBJ_TYPE_S_STAR,
	OBJ_TYPE_BLACKHOLE,
	OBJ_TYPE_GLOBULAR_CLUSTER,
	OBJ_TYPE_VERA_OBJECT,
	OBJ_TYPE_OUR_GALAXY,
	OBJ_TYPE_NEARBY_GALAXY,
	OBJ_TYPE_DISTANT_GALAXY,
	OBJ_TYPE_SPACECRAFT,

	OBJ_TYPE_FIXED_OBJECT,
	OBJ_TYPE_SCALE_CIRCLE,
	OBJ_TYPE_SCALE_CIRCLE_G,
	OBJ_TYPE_LABEL
};



//----------------------
//  Object Draw Region
//----------------------
enum {
	OBJ_DRAW_NONE,
	OBJ_DRAW_NEAR,
	OBJ_DRAW_FAR,
	OBJ_DRAW_BOTH
};



//----------------------
//  Object Draw Status
//----------------------
enum {
	OBJ_DRAW_STAT_REALSIZE,
	OBJ_DRAW_STAT_POINT
};



//--------------------------
//  Object Hit-test Status
//--------------------------
enum {
	OBJ_HITTEST_STAT_NONE,
	OBJ_HITTEST_STAT_REALSIZE,
	OBJ_HITTEST_STAT_POINT
};



//--------------------
//  class mtk_object
//--------------------
class  mtk_object
{
	int		m_type, m_idx;
	float	m_dz;	// 視点座標系でのずらし
	bool	m_bTgt;

	//--- 描画用一時情報
	double		m_Rb;			//  Bounding R
	double		m_Rh;			//  Hit-test R
	vector3d	rpos;
	double		scl;
	float		draw_z;
	float		alpha;

	int	draw_status;
	int	hit_test_status;

public:
	mtk_object() : m_dz(0.0f), m_bTgt(false) {};
	mtk_object(int type, int idx, float dz, bool bTargetable) { init(type, idx, dz, bTargetable); }

	void		init(int type, int idx, float dz, bool bTargetable);

	int			get_type() const  { return  m_type; }
	int			get_index() const { return  m_idx; }
	vector3d	get_pos() const;
	float		get_dz() const		{ return m_dz; }

	double		get_landing_radius() const;

	vector3d	get_parent_pos() const;
	
	double		get_scale() const;

	bool		is_targetable() const  { return m_bTgt; }

	int			prepare_draw(const mtk_draw_struct *v_ds, int Nds, const float z_near, const float z_mid, const float z_far);
	float		get_draw_z() const			{ return  draw_z; }
	int			get_draw_status() const		{ return  draw_status; }
	int			get_hit_test_status() const	{ return hit_test_status; }
	float		get_alpha() const			{ return  alpha; }

	double		get_R() const { return m_Rh; }

	bool		hit_test(const mtk_draw_struct& ds, int mx, int my, const vector3d& r0, const vector3d& nv) const;

	void		draw(const mtk_draw_struct& ds, bool bSelect, bool bChangeObjType) const;

	void		make_object_info(const mtk_draw_struct& ds, std::vector< std::string >& text)  const;
};





//
//
//
float	get_label_alpha(const mtk_draw_struct& ds, const mtk_object& obj);
float	get_orbit_alpha(const mtk_draw_struct& ds, const mtk_object& obj);
bool	is_object_label_visible(const mtk_draw_struct& ds, const mtk_object& obj);
bool	is_object_orbit_visible(const mtk_object& obj);





#endif
