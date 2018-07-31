/**************************************************************************************************

Copyright (c) 2012-2015   Tsunehiko Kato

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
#include <windows.h>
#include "../common/graphics/gl_image.h"
#include "../common/graphics/gl_win.h"
#include  "mtk_draw_struct.h"
#include  "mtk_camera_setting.h"
#include  "mtk_stereo.h"
#include  "mtk_mode.h"
#include  "mtk_objects.h"
#include  "mtk_planetarium.h"





//--  Externs
void  draw_scene_all(const mtk_draw_struct& ds, bool bClear);
mtk_draw_struct calc_draw_struct(const mtk_camera& camera_base, float aspc_rt, int _vp_x1, int _vp_x2, int _vp_y1, int _vp_y2, vector3f dr, bool bRotBB, float line_w);
void set_latest_draw_struct(const mtk_draw_struct& ds, int idx);


//--  Objects
static float eye_dist = 5.0;




//----------------
//  Eye distance
//----------------
float
get_stereo_eye_distance()
{
	return eye_dist;
}

void
set_stereo_eye_distance(float d)
{
	eye_dist = d;
}



//============
//  アナグリフ
//============

//-------------------------------
//  グレースケールへの変換係数
//-------------------------------

//---   NTSC (係数の 1000 倍）  ---
const int cR = 299;
const int cG = 587;
const int cB = 114;



static image img, imgAnaglyph;


void
make_anaglyph_left(int w, int h)
{
	img.alloc(w,h,3);
	imgAnaglyph.alloc(w,h,3);

	// Read Pixels
	GLubyte* p0 = (GLubyte*)img.get_pointer();
	glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, p0);


	GLubyte* p  = img.get_pointer();
	GLubyte* pa = imgAnaglyph.get_pointer();
	const int imax = w * h;
	for (int i=0; i<imax; i++) {
		int lv = (cR*p[0] + cG*p[1] + cB*p[2])/1000;
		GLubyte val = GLubyte(lv);
		pa[0] = val;		// R-channel
		p  += 3;
		pa += 3;
	}
}


void
make_anaglyph_right(int w, int h)
{
	GLubyte* p0 = (GLubyte*)img.get_pointer();
	glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, p0);


	GLubyte* p  = img.get_pointer();
	GLubyte* pa = imgAnaglyph.get_pointer();
	const int imax = w * h;
	for (int i=0; i<imax; i++) {
		int lv = (cR*p[0] + cG*p[1] + cB*p[2])/1000;
		GLubyte val = GLubyte(lv);
		
		pa[1] = val;
		pa[2] = val;
		p  += 3;
		pa += 3;
	}


	// Draw Pixels
	gl_enter_2D_mode();
		glRasterPos2i(0,h-1);
		glPixelZoom(1,1);
		glDrawPixels(w, h, GL_RGB, GL_UNSIGNED_BYTE, (GLubyte*)imgAnaglyph.get_pointer());
	gl_leave_2D_mode();
}



void
draw_stereo_anaglyph(const mtk_camera camera_base, int w, int h)
{
	float aspc_rt = float(h) / float(w);
	mtk_draw_struct  ds[2];

	float eye_x = get_eye_offset_x();

	vector3f dr = 0.5f*eye_dist*vector3f(1,0,0);

	ds[0] = calc_draw_struct(camera_base, aspc_rt, 0, w, 0, h, -dr, false, 1.0);
	ds[1] = calc_draw_struct(camera_base, aspc_rt, 0, w, 0, h, dr, false, 1.0);
	set_latest_draw_struct(ds[0], 0);


	if (ds[0].tyMode == MODE_3D) {
		prepare_draw_objects(ds, 2);
	}
	else {
		prepare_draw_planetarium(ds, 2);
	}



	// draw L
	draw_scene_all(ds[0], true);
	make_anaglyph_left(w, h);

	// draw R
	draw_scene_all(ds[1], true);
	make_anaglyph_right(w, h);


	set_eye_offset_x(eye_x);
}


void
draw_stereo_color_anaglyph(const mtk_camera camera_base, int w, int h)
{
	float aspc_rt = float(h) / float(w);
	mtk_draw_struct  ds[2];


	float eye_x = get_eye_offset_x();

	vector3f dr = 0.5f*eye_dist*vector3f(1, 0, 0);

	ds[0] = calc_draw_struct(camera_base, aspc_rt, 0, w, 0, h, -dr, false, 1.0);
	ds[1] = calc_draw_struct(camera_base, aspc_rt, 0, w, 0, h, dr, false, 1.0);
	set_latest_draw_struct(ds[0], 0);


	if (ds[0].tyMode == MODE_3D) {
		prepare_draw_objects(ds, 2);
	}
	else {
		prepare_draw_planetarium(ds, 2);
	}


	// draw L
	glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE);
	draw_scene_all(ds[0], true);

	// draw R
	glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_FALSE);
	draw_scene_all(ds[1], true);


	set_eye_offset_x(eye_x);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
}






//============
//  分割方式
//============

//--- Side by Side （左目用画像が左側）
void
draw_stereo_side_by_side(const mtk_camera camera_base, int w, int h)
{
	float aspc_rt = float(h) / float(w);
	mtk_draw_struct  ds[2];

	vector3f dr = 0.5f*eye_dist*vector3f(1, 0, 0);


	ds[0] = calc_draw_struct(camera_base, aspc_rt, 0, w / 2, 0, h, -dr, false, 1.0);
	ds[1] = calc_draw_struct(camera_base, aspc_rt, w / 2, w, 0, h, dr, false, 1.0);
	set_latest_draw_struct(ds[0], 0);
	set_latest_draw_struct(ds[1], 1);


	if (ds[0].tyMode == MODE_3D) {
		prepare_draw_objects(ds, 2);
	}
	else {
		prepare_draw_planetarium(ds, 2);
	}


	// 左目
	draw_scene_all(ds[0], true);

	// 右目
	draw_scene_all(ds[1], false);

	glViewport(0,0, w, h);
}



//--- Above Below （左目用画像が上）
void
draw_stereo_above_below(const mtk_camera camera_base, int w, int h)
{
	float aspc_rt = float(h) / float(w);
	mtk_draw_struct  ds[2];
	
	vector3f dr = 0.5f*eye_dist*vector3f(1, 0, 0);

	//***  glviewportは左下が原点  ***
	ds[0] = calc_draw_struct(camera_base, aspc_rt, 0, w, h / 2, h, -dr, false, 1.0);
	ds[1] = calc_draw_struct(camera_base, aspc_rt, 0, w, 0, h / 2, dr, false, 1.0);
	set_latest_draw_struct(ds[0], 0);
	set_latest_draw_struct(ds[1], 1);


	if (ds[0].tyMode == MODE_3D) {
		prepare_draw_objects(ds, 2);
	}
	else {
		prepare_draw_planetarium(ds, 2);
	}


	// 左目
	draw_scene_all(ds[0], true);

	// 右目
	draw_scene_all(ds[1], false);

	glViewport(0,0, w,h);
}
