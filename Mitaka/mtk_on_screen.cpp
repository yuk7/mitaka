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
#include  <windows.h>
#include  "../common/graphics/gl_win.h"
#include  "mtk_gl_common.h"
#include  "mtk_font.h"
#include  "mtk_screen.h"

// for Visual Studio
#define snprintf _snprintf



extern int gl_w, gl_h;




//------------------------
//  ウィンドウのサイズを表示
//------------------------
void
draw_window_size()
{
	int w, h;
	w = gl_w;
	h = gl_h;

	char str[50];
	snprintf(str, 50, "%d x %d", w, h);

	glPushAttrib(GL_ENABLE_BIT);
	
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);



	gl_enter_2D_mode();
	
		//-- 背景を暗くする
		const float af = 0.7;
		glEnable(GL_BLEND);
		glColor4f(0,0,0,1-af);
		glBegin(GL_TRIANGLE_FAN);
			glVertex2f(0,0);
			glVertex2f(w,0);
			glVertex2f(w,h);
			glVertex2f(0,h);
		glEnd();
		
		//-- サイズを描画
		glTranslatef(w/2, h/2, 0);
		const float sz = 16;
		glDisable(GL_BLEND);
		glColor3f(0,1,0);
		glScalef(sz, -sz, sz);
		draw_text_centering(str);
	gl_leave_2D_mode();

	glPopAttrib();
}


void
draw_black_layer(const frustum& frs, float af)
{
	float x1, y1, x2, y2, z1;
	x1 = 1.5*frs.get_x1();
	x2 = 1.5*frs.get_x2();
	y1 = 1.5*frs.get_y1();
	y2 = 1.5*frs.get_y2();
	z1 = 1.01*frs.get_z1();

	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glPushMatrix();
		glLoadIdentity();
		//-- 背景を暗くする
		glEnable(GL_BLEND);
		glColor4f(0,0,0,1-af);
		glTranslatef(0,0,-z1);
		glBegin(GL_TRIANGLE_FAN);
			glVertex2f(x1, y1);
			glVertex2f(x2, y1);
			glVertex2f(x2, y2);
			glVertex2f(x1, y2);
		glEnd();
	glPopMatrix();

	glPopAttrib();
}
