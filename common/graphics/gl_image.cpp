/******************************************************************************

Copyright (c) 2007   Tsunehiko Kato

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

以下に定める条件に従い、本ソフトウェアおよび関連文書のファイル（以下「ソフトウェ
ア」）の複製を取得するすべての人に対し、ソフトウェアを無制限に扱うことを無償で許
可します。これには、ソフトウェアの複製を使用、複写、変更、結合、掲載、頒布、サブ
ライセンス、および/または販売する権利、およびソフトウェアを提供する相手に同じこ
とを許可する権利も無制限に含まれます。

上記の著作権表示および本許諾表示を、ソフトウェアのすべての複製または重要な部分に
記載するものとします。

ソフトウェアは「現状のまま」で、明示であるか暗黙であるかを問わず、何らの保証もな
く提供されます。ここでいう保証とは、商品性、特定の目的への適合性、および権利非侵
害についての保証も含みますが、それに限定されるものではありません。 作者または著
作権者は、契約行為、不法行為、またはそれ以外であろうと、ソフトウェアに起因または
関連し、あるいはソフトウェアの使用またはその他の扱いによって生じる一切の請求、損
害、その他の義務について何らの責任も負わないものとします。
*******************************************************************************/
#include  "gl_image.h"




//--------------------
//  OpenGL functions
//--------------------
void
gl_set_texture(GLenum target, const image& img)
{
	int  width, height, chs;
	GLenum fmt;
	width = img.get_width();
	height = img.get_height();
	chs = img.get_channels();
	fmt = get_GLformat(chs);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if ( is2n(width) && is2n(height) ) {
		glTexImage2D( target, 0, fmt, width, height, 0, fmt, GL_UNSIGNED_BYTE, img.get_pointer() );
	} else {
		const int max_size = 2048;	// 1024
		int w, h;
		w = to2n(width);
		h = to2n(height);
		if (w>max_size) w = max_size;
		if (h>max_size) h = max_size;

		image  img_tmp(w, h, chs);
		img_tmp.scale_image(img, 0,0, float(width), float(height));
		glTexImage2D( target, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, img_tmp.get_pointer() );
	}
}


void
gl_set_texture_mipmaps(GLenum target, GLenum fmt, const image& img, int shrink_rate)
{
	int  width, height, chs;
	width = img.get_width();
	height = img.get_height();
	chs = img.get_channels();

	if (shrink_rate < 1)  shrink_rate = 1;
	if (shrink_rate == 1) {
		gluBuild2DMipmaps(target, chs, width,height, fmt, GL_UNSIGNED_BYTE, img.get_pointer() );
	} else {
		int w = width/shrink_rate;
		int h = height/shrink_rate;
		w = 4*((w+3)/4+1);
		h = 4*((h+3)/4+1);
		GLubyte* p = new GLubyte[width * height * chs];
		gluScaleImage(fmt, width, height, GL_UNSIGNED_BYTE, img.get_pointer(),
						w, h, GL_UNSIGNED_BYTE, p);
		gluBuild2DMipmaps(target, chs, w,h, fmt, GL_UNSIGNED_BYTE, p );
		delete [] p;
	}
}


void
gl_set_texture_mipmaps(GLenum target, const image& img, int shrink_rate)
{
	int  chs;
	GLenum fmt;
	chs = img.get_channels();
	fmt = get_GLformat(chs);
	gl_set_texture_mipmaps(target, fmt, img, shrink_rate);
}


void
gl_read_pixels(image& img, int x, int y, int w, int h)
{
	int  chs = img.get_channels();
	img.alloc(w, h, chs);
	glReadPixels( x, y, w, h, get_GLformat(chs), GL_UNSIGNED_BYTE, img.get_pointer() );
}


void
gl_capture_viewport(image& img, GLenum read_buf)
{
	GLint  viewport[4];
	glGetIntegerv( GL_VIEWPORT, viewport );

	glReadBuffer(read_buf);
	gl_read_pixels(img, viewport[0], viewport[1], viewport[2], viewport[3] );
	glReadBuffer(GL_BACK);
}


