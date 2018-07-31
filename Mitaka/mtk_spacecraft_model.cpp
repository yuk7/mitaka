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
#include  "mtk_3D_model.h"
#include  "mtk_spacecraft_model.h"
#include  <windows.h>
#include  <stdio.h>
#include  "../common/graphics/gl_common.h"
#include  "../common/astro/astro_const.h"
#include  "mtk_spacecraft.h"
#include  "mtk_sun.h"




bool
spacecraft::load_trajectory(const char* filename, int Ntbl)
{
	return  trajectory.load(filename, Ntbl);
}


double
spacecraft::get_t1() const
{
	return  trajectory.get_t_begin();
}

double
spacecraft::get_t2() const
{
	return  trajectory.get_t_end();
}


// 指定時刻の位置を得る
vector3d
spacecraft::get_pos(const double& t)  const
{
	vector3d ro;

	switch (origin) {
	case ORIGIN_SUN:
		ro = get_sun_position();
		break;
	case ORIGIN_SS_BC:
	default:
		ro = vector3d(0, 0, 0);
		break;
	}

	return  trajectory.get_pos(t) + ro;
}



// 軌道の表示
void
spacecraft::draw_trajectory(const mtk_draw_struct& ds, const double& t, const vector3d& camera_pos, const RGBf& col, float af) const
{
	if (!bDrawTrajectory)  return;
	if (!trajectory.isLoaded()) return;


	int idx = 0;
	vector3d pos;

	int Nseg, Ndiv;
	Nseg = trajectory.get_segment_num();
	Ndiv = 10;


	vector3d ro;
	switch (origin) {
	case ORIGIN_SUN:
		ro = get_sun_position();
		break;
	case ORIGIN_SS_BC:
	default:
		ro = vector3d(0, 0, 0);
		break;
	}


	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glColor4f(col.r, col.g, col.b, af);
	
	const float lw = 1.5 * ds.line_width_fac / (ds.rad_per_pixel / 0.001);
	glLineWidth(lw);

	glBegin(GL_LINE_STRIP);


	double _t;
	for (int i = 0; i < Nseg; i++) {
		double t1, t2;

		const trajectory_segment &ts = trajectory.get_segment(i);
		t1 = ts.get_t_begin();
		t2 = ts.get_t_end();
		
		if (t < t1) break;

		
		double dt = (t2 - t1) / Ndiv;
		for (int j = 0; j < Ndiv; j++) {
			vector3d pos;

			_t = j * dt + t1;
			if (_t > t) {
				break;
			}
			pos = ts.get_pos(_t) - camera_pos + ro;

			glVertex3dv(pos.get_pointer());
		}
	}

	pos = get_pos(t) - camera_pos;
	glVertex3dv(pos.get_pointer());

	glEnd();
	glPopAttrib();
}




// モデルの表示
void
spacecraft::draw_model() const
{
	if ( ModelIdx < 0 )  return;

	glModel3D *pModel = get_3DModel(ModelIdx);
	if (pModel == 0) return;

	glPushMatrix();
		const float		scl = 3 * pModel->get_scale();
		const vector3f	r0  = pModel->get_r0();
		glScalef(scl,scl,scl);
		glTranslatef(r0.x, r0.y, r0.z);

		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
		pModel->draw();
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	glPopMatrix();
}



//
void
spacecraft::set(const char *_key, int _ModelIdx, int org)
{
	key = _key;
	set_model(_ModelIdx);
	set_origin(org);
}

