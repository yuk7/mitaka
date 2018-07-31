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
#ifndef __MTK_SPACE_CURVE_H__
#define __MTK_SPACE_CURVE_H__
#include  <vector>
#include  "../common/math/vector3.h"
#include  "mtk_draw_struct.h"
#include  "mtk_object.h"
#include  "mtk_colors.h"


//---------------
//  space curve
//---------------
class space_curve
{
	mtk_object	m_related_object;
	int     m_color_no;
	float	m_line_width;
	std::vector<vector3d>  m_vpos;
public:
	space_curve()
		:  m_related_object(OBJ_TYPE_NONE, 0, 0.0f, false), m_line_width(1.0f), m_color_no(MTK_COLOR_ORBIT_PLANET)  {};
	space_curve(const mtk_object& obj, float width, int color_no)
		: m_related_object(obj), m_line_width(width), m_color_no(color_no) {};

	void		clear()	{ m_vpos.clear(); }
	void		init(const mtk_object& obj, float width, int color_no) {
					clear();
					m_related_object	= obj;
					m_line_width		= width;
					m_color_no			= color_no;
				}

	void		add_point(const vector3d& pos)	{ m_vpos.push_back(pos); }

	void			set_related_object(const mtk_object& obj)	{ m_related_object = obj; }
	void			set_color_no(int col_no)			{ m_color_no = col_no; }
	void			set_line_width(float width)			{ m_line_width = width; }
	void			set_num_points(int num)				{ m_vpos.resize(num); }
	void			set_point(int idx, vector3d& pos)	{ m_vpos[idx] = pos; }

	int				get_num_points() const				{ return m_vpos.size(); }
	const vector3d&	get_point(int idx) const			{ return m_vpos[idx]; }
	const mtk_object&	get_related_object() const		{ return m_related_object; }
	int				get_color_no() const				{ return m_color_no; }
	float			get_line_width() const				{ return m_line_width; }
};

//--  Init / Update
int		register_space_curve(const space_curve&  sc);
void	init_space_curves(const double& t);
void	update_space_curves(const double& t);

//--  Draw
void	draw_space_curves(const mtk_draw_struct& ds);

#endif
