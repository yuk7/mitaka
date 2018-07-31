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
#ifndef __MTK_PLANETARY_RING_H__
#define __MTK_PLANETARY_RING_H__
#include  "../common/math/vector3.h"
#include  "../common/graphics/gl_common.h"
#include  "../common/graphics/color.h"
#include  "../common/JulianDate.h"
#include  "../common/astro/astrometry.h"
#include  "mtk_draw_struct.h"
#include  <string>


const int particle_ring_RN   =  30;
const int particle_ring_PHIN = 180;




class  particle_ring
{
private:
	float		R1, R2;
	vector3f	pos[particle_ring_RN][particle_ring_PHIN+1];
	float		s[particle_ring_RN];
	float		shadow_fac[particle_ring_RN][particle_ring_PHIN + 1];
	bool		b_shadow_boundary[particle_ring_RN][particle_ring_PHIN + 1];
	int			last_mode;
	bool		bLastSideLight;
	double		JD;
	float		alp;	// 惑星に落とす影用の透明度

	//---  Textures
	std::string tex_file_name;
	GLuint tex_idx;


public:
	particle_ring(float _R1, float _R2, float _alp, const std::string& tex_fn);
	float	get_R1() const	{ return R1; }
	float	get_R2() const	{ return R2; }
	float	get_alpha() const { return alp; }

	void	calc_shadow_factor(const mtk_draw_struct& ds, const vector3d& r0, const double& R, const double& f, const vector3d& ex, const vector3d& ey, const vector3d& ez, const vector3d& _lv);

	// Textures
	GLuint		load_texture(int shrink_rate);
	GLuint		get_texture_index() const	{ return tex_idx; }


	void	draw_triangles(const vector2f* pt, const float *u, const float *f, const int *pt_idx, const int N, bool bShadow) const;
	void	draw_boundary_mesh(int i, int j) const;

	void	draw(const mtk_draw_struct& ds, const vector3d& r0, const double& R, const vector3d& ex, const vector3d& ey, const vector3d& lv)  const;
	void	draw_3Dchart() const;
};




void	draw_ring_shaded_sphere(const vector3d& r0, const double& R, const double& f, const vector3d& ex, const vector3d& ey, const vector3d& ez, const vector3d &_lv, const particle_ring& pr, const GLuint texPlanet, const RGBf& mat_col);





int		add_particle_ring(float R1, float R2, float alp, const std::string& tex_fn);
particle_ring&	get_particle_ring(int idx);

void	load_ring_textures();




// for init
struct ring_info {
	double  Rr1, Rr2;	// 輪の内半径、外半径（赤道半径を単位として）
	float	ring_alp;	// 惑星に落とす影用の透明度
	const char* tex_fn;	// リングのテクスチャのファイル名
};


#endif
