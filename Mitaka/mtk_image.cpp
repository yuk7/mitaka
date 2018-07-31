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
#include  "mtk_image.h"
#include  <stdio.h>


void
mtk_image::set_image(const char* fn)
{
	if (bImageLoaded && stricmp(szFileName, fn)==0)  return;

	strcpy(szFileName, fn);
	bImageLoaded = false;
}

void
mtk_image::load()
{
	if (szFileName[0] == '\0')  return;

	if ( bImageLoaded )  return;

	if (texName!=0) {
		glDeleteTextures(1, &texName);
	}
	glGenTextures(1, &texName);

	image img;
	img.load(szFileName);
	set_texture(texName, img);

	w = img.get_width();
	h = img.get_height();
	bImageLoaded = true;
}

void
mtk_image::draw(float scr_w, float scr_h)
{
	if (szFileName[0] == '\0')  return;

	//--  読み込んでいない時は読み込む
	if (!bImageLoaded) {
		load();
	}

	//-- ウィンドウにフィットさせる（中央に配置）
	float imgw, imgh;
	float rt_wn = float(scr_h)/float(scr_w);
	float rt_img = float(h)/float(w);
	if (rt_img > rt_wn) {
		// ウィンドウよりも画像の方が縦長
		imgw = 1.0f/rt_img;
		imgh = 1;
	} else {
		// 画像よりもウィンドウの方が縦長
		float f = scr_w / scr_h;
		imgw = f;
		imgh = f * rt_img;
	}

	//--  画像を表示
	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, texName);
		glScalef(imgw, imgh, 1.0f);

		glBegin(GL_QUADS);
			glTexCoord2f(1,1);
			glVertex3f( 1, 1,0);
			glTexCoord2f(0,1);
			glVertex3f(-1, 1,0);
			glTexCoord2f(0,0);
			glVertex3f(-1,-1,0);
			glTexCoord2f(1,0);
			glVertex3f( 1,-1,0);
		glEnd();
	glPopMatrix();
}
