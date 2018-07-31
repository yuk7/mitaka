/**************************************************************************************************

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
#include  "mtk_cmb.h"
#include  "mtk_OpenGL.h"
#include  "mtk_switches.h"
#include  "mtk_dialog_box.h"
#include  "mtk_our_galaxy_rt.h"




static GLuint texCMB;
const char *fn_tex_CMB = "CMB.jpg";



const float CMB_R = 138.0e8 * _1ly_pc;
static float CMB_R1 = 0.85 * CMB_R;
static float CMB_R2 = 1.15 * CMB_R;
static float CMB_Alpha = 0.4;



void
load_CMB_texture()
{
	SetSplashText("LOADING_CMB");
	texCMB = LoadTexture(fn_tex_CMB, TexShrinkRate, false);
}


void
set_CMB_Alpha(const float alp)
{
	CMB_Alpha = alp;
}


void
set_CMB_fade_scales(const float _R1, const float _R2)
{
	CMB_R1 = _R1 * CMB_R;
	CMB_R2 = _R2 * CMB_R;
}


void
draw_CMB_map(const mtk_draw_struct& ds, bool bFarSide)
{
	if (!bCMB) return;


	const float dist = norm(ds.camera_base.get_pos());
	float af, af1, af2;
	af = CMB_Alpha;

	if (dist < CMB_R2) {
		float t = (dist - CMB_R1) / (CMB_R2 - CMB_R1);
		af *= t;
	}
	if (af <= 0.0) return;


	af1 = af;
	af2 = af*0.5;


	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, texCMB);
	glEnable(GL_CULL_FACE);


	vector3f rpos = -ds.camera.get_pos();


	glPushMatrix();

		glTranslatef(rpos.x, rpos.y, rpos.z);

		go_to_galactic_coord();

		glScalef(CMB_R, CMB_R, CMB_R);
	
		glRotatef(180, 0, 0, 1);
		if (bFarSide) {
			glColor4f(1, 1, 1, af1);
			glFrontFace(GL_CW);
			gl_draw_sphere(5, true);
		}
		else {
			glColor4f(1, 1, 1, af2);
			glFrontFace(GL_CCW);
			gl_draw_sphere(5, true);
		}
		glPopMatrix();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
