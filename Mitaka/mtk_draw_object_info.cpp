/**************************************************************************************************

Copyright (c) 2007-2012   Tsunehiko Kato

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
#include  "mtk_draw_object_info.h"
#include  <stdio.h>
#include  "../common/graphics/gl_win.h"
#include  "mtk_mode.h"
#include  "mtk_objects.h"
#include  "mtk_target.h"
#include  "mtk_font.h"
#include  "mtk_stereo.h"


//*****
extern int			win_w, win_h;


static bool  bDispObjectInfo = true;
static float objinfo_size = 1.0;



bool
is_object_info_enable()
{
	return bDispObjectInfo;
}


void
enable_object_info(bool flg)
{
	bDispObjectInfo = flg;
}

void
set_object_info_size(float sz)
{
	objinfo_size = sz;
}



void
draw_selected_object_info_sub(const mtk_draw_struct& ds, float x0, float y0, int mx, int my, float fx, float fy, std::vector<std::string>&  text)
{
	const int csN = 10;
	static bool bFirst = true;
	static float cs[csN], sn[csN];
	if (bFirst) {
		bFirst = false;

		for (int i=0; i<csN; i++) {
			float th = 0.5*M_PI * float(i)/(csN-1);
			cs[i] = cos(th);
			sn[i] = sin(th);
		}
	}
	

	float w, h;
	w = ds.vp_w;
	h = ds.vp_h;
	

	float a = min(ds.vp_w, ds.vp_h);
	float fac = (a < 200) ? a/200 : 1.0;

	fac *= objinfo_size;

	const float sub_size = 0.7f * fac;
	const float text_size = 21.0f * fac;
	const float spc1 = 0.4f;
	const float spc2 = 0.2f;
	const float line_width = 1.0 * fac;

	const float szw = text_size * fx;
	const float szh = text_size * fy;


	float dx1, dy1;
	dx1 = 0.6 * szw;
	dy1 = 0.6 * szh;

	float mgn_x, mgn_y;
	mgn_x = 0.3 * szw;
	mgn_y = 0.3 * szh;

	//---  Calc size  ---
	float wt, ht;

	wt = 0.0f;
	for (int i=0; i<text.size(); i++) {
		float w, h;
		get_text_size(text[i].c_str(), w, h);
		if (i >= 1)  w *= sub_size;
		if (wt < w)  wt = w;
	}


	ht = 1.0f + spc1 + sub_size * ((text.size() - 1) + (text.size() - 2)*spc2);



	wt *= szw;
	ht *= szh;

	//---
	float x1, y1, x2, y2, wm, hm;
	x1 = float(mx) - mgn_x + dx1;
	y1 = float(my) - mgn_y + dy1;
	x2 = x1 + wt + mgn_x;
	y2 = y1 + ht + mgn_y;
	wm = wt + 2*mgn_x;
	hm = ht + 2*mgn_y;


	//-- Adjust position
	float mgn_xx = 0.7*mgn_x;
	float mgn_yy = 0.7*mgn_y;
	if (x2+mgn_x+mgn_xx > x0+w) {
		x2 = x0+w - mgn_xx - mgn_x;
		x1 = x2 - wt - mgn_x;
	}
	if (y2+mgn_y+mgn_yy > y0+h) {
		y2 = y0+h - mgn_yy - mgn_y;
		y1 = y2 - ht - mgn_y;
	}


	//---  Draw  ---
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);

	gl_enter_2D_mode();
		glTranslatef(x1, y1, 0);

		glColor4f(0, 0.15, 0.23, 0.9);
		glBegin(GL_QUADS);
			//-- upper
			glVertex2f(mgn_x,    0);
			glVertex2f(mgn_x+wt, 0);
			glVertex2f(mgn_x+wt, mgn_y);
			glVertex2f(mgn_x,    mgn_y);
			//-- main body
			glVertex2f( 0,  mgn_y +szh);
			glVertex2f(wm,  mgn_y +szh);
			glVertex2f(wm,  ht+mgn_y);
			glVertex2f( 0,  ht+mgn_y);
			//-- lower
			glVertex2f(mgn_x,    ht+mgn_y);
			glVertex2f(mgn_x+wt, ht+mgn_y);
			glVertex2f(mgn_x+wt, hm);
			glVertex2f(mgn_x,    hm);
		glEnd();

		//-- Corner UL
		float Rx = mgn_x;
		float Ry = mgn_y;
		float xc, yc;
		xc = mgn_x;
		yc = mgn_y;
		glBegin(GL_TRIANGLE_FAN);
		glVertex3f(xc, yc, 0);
		for (int i=0; i<csN; i++) {
			float x = -Rx*cs[i] + xc;
			float y = -Ry*sn[i] + yc;
			glVertex3f(x, y, 0);
		}
		glEnd();

		//-- Corner UR
		xc = mgn_x + wt;
		yc = mgn_y;
		glBegin(GL_TRIANGLE_FAN);
		glVertex3f(xc, yc, 0);
		for (int i=0; i<csN; i++) {
			float x =  Rx*cs[i] + xc;
			float y = -Ry*sn[i] + yc;
			glVertex3f(x, y, 0);
		}
		glEnd();

		//-- Corner BR
		xc = mgn_x;
		yc = mgn_y + ht;
		glBegin(GL_TRIANGLE_FAN);
		glVertex3f(xc, yc, 0);
		for (int i=0; i<csN; i++) {
			float x = -Rx*cs[i] + xc;
			float y =  Ry*sn[i] + yc;
			glVertex3f(x, y, 0);
		}
		glEnd();

		//-- Corner BR
		xc = mgn_x + wt;
		yc = mgn_y + ht;
		glBegin(GL_TRIANGLE_FAN);
		glVertex3f(xc, yc, 0);
		for (int i=0; i<csN; i++) {
			float x =  Rx*cs[i] + xc;
			float y =  Ry*sn[i] + yc;
			glVertex3f(x, y, 0);
		}
		glEnd();


		//--  Hi-light
		glBegin(GL_QUADS);
			glColor4f(0, 0.4, 0.7, 1.0);
			glVertex2f( 0,  mgn_y);
			glVertex2f(wm,  mgn_y);
			glColor4f(0, 0.25, 0.4, 0.95);
			glVertex2f(wm,  mgn_y +szh);
			glVertex2f( 0,  mgn_y +szh);
		glEnd();



		//--  Line
		glColor4f(0, 0.4, 0.7, 1.0);
		glLineWidth( line_width );
		glBegin(GL_LINES);
			glVertex3f(mgn_x, 0, 0);
			glVertex3f(wt+mgn_x, 0, 0);

			glVertex3f(mgn_x, hm, 0);
			glVertex3f(wt+mgn_x, hm, 0);

			glVertex3f(0, mgn_y, 0);
			glVertex3f(0, mgn_y+ht, 0);

			glVertex3f(wm, mgn_y, 0);
			glVertex3f(wm, mgn_y+ht, 0);
		glEnd();


		//-- Corner UL
		xc = mgn_x;
		yc = mgn_y;
		glBegin(GL_LINE_STRIP);
		for (int i=0; i<csN; i++) {
			float x = -Rx*cs[i] + xc;
			float y = -Ry*sn[i] + yc;
			glVertex3f(x, y, 0);
		}
		glEnd();

		//-- Corner UR
		xc = mgn_x + wt;
		yc = mgn_y;
		glBegin(GL_LINE_STRIP);
		for (int i=0; i<csN; i++) {
			float x =  Rx*cs[i] + xc;
			float y = -Ry*sn[i] + yc;
			glVertex3f(x, y, 0);
		}
		glEnd();

		//-- Corner DL
		xc = mgn_x;
		yc = mgn_y + ht;
		glBegin(GL_LINE_STRIP);
		for (int i=0; i<csN; i++) {
			float x = -Rx*cs[i] + xc;
			float y =  Ry*sn[i] + yc;
			glVertex3f(x, y, 0);
		}
		glEnd();

		//-- Corner UR
		xc = mgn_x + wt;
		yc = mgn_y + ht;
		glBegin(GL_LINE_STRIP);
		for (int i=0; i<csN; i++) {
			float x =  Rx*cs[i] + xc;
			float y =  Ry*sn[i] + yc;
			glVertex3f(x, y, 0);
		}
		glEnd();



		glTranslatef(mgn_x, mgn_y, 0);
		glScalef(szw, -szh, szw);
		glTranslatef(0,-1,0);
		glTranslatef(0,0.1,0);


		//---  Name  ---
		glColor4f(1, 1, 1, 1);
		glPushMatrix();
			draw_text( text[0] );
		glPopMatrix();


		glScalef(sub_size, sub_size, sub_size);
		glTranslatef(0,-1-spc1,0);
		if (text.size() >= 1) {
			//---  Text  ---
			for (int i=1; i<text.size(); i++) {
				glPushMatrix();
					draw_text( text[i] );
				glPopMatrix();
				if (i < text.size()-1)  { glTranslatef(0,-1-spc2,0); }
			}
		}
	gl_leave_2D_mode();

	glEnable(GL_DEPTH_TEST);
}



void
draw_selected_object_info(const mtk_draw_struct& ds)
{
	if (!bDispObjectInfo) return;


	//***** Planetarium mode は、まだ対応していない
	if (ds.tyMode != MODE_3D)  return;
	//*****
	if ( !is_there_selected_object() )  return;



	//---  Make text  ---
	const mtk_object&  obj = get_selected_object();
	std::vector<std::string>  text;
	obj.make_object_info(ds, text);
	if (text.size() <= 0.0)  return;



	//---  Hit position  ---
	int mx, my;
	get_object_hit_pos(&mx, &my);
	my = win_h - my;

	switch (get_display_mode()) {
		case DISPMODE_SIDE_BY_SIDE:
			if (mx > win_w / 2) { mx -= win_w / 2; }
			draw_selected_object_info_sub(ds, 0,0,       mx, my,         0.5, 1.0, text);
			draw_selected_object_info_sub(ds, win_w/2,0, win_w/2+mx, my, 0.5, 1.0, text);
			break;
		case DISPMODE_TOP_AND_BOTTOM:
			if (my > win_h / 2) { my -= win_h / 2; }
			draw_selected_object_info_sub(ds, 0,0,       mx, my/2,         1.0, 0.5, text);
			draw_selected_object_info_sub(ds, 0,win_h/2, mx, win_h/2+my/2, 1.0, 0.5, text);
			break;
		default:
			draw_selected_object_info_sub(ds, 0,0, mx, my, 1.0, 1.0, text);
			break;
	}
}
