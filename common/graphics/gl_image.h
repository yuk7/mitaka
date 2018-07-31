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
//  2002/5/17, 7/9
#ifndef __GL_IMAGE_H__
#define __GL_IMAGE_H__
#include  <stdio.h>
#include  "image.h"
#include  <windows.h>
#include  <gl/gl.h>
#include  <gl/glu.h>


void		set_glColor3f(const image &img, int x, int y); 
void		gl_set_texture(GLenum target, const image& img);
void		gl_set_texture_mipmaps(GLenum target, GLenum fmt, const image& img, int shrink_rate);
void		gl_set_texture_mipmaps(GLenum target, const image& img, int shrink_rate);
void		gl_read_pixels(image& img, int x, int y, int w, int h);
void		gl_capture_viewport(image& img, GLenum read_buf);


//--- inline
inline  bool
is2n(int a)
{
	return  (a>0) && (0 == (a & (a - 1)));
}


inline  int
to2n(int a)
{
	if (a == 0 || is2n(a))  return a;

	int b = 1;
	while (b<a) { b <<= 1; }

	return b;
}

inline  GLenum
get_GLformat(int channels)
{
	switch (channels) {
	case 1:	 return  GL_LUMINANCE;
	case 2:  return  GL_LUMINANCE_ALPHA;
	case 3:	 return  GL_RGB;
	case 4:  return  GL_RGBA;
	default: return  GL_RGB;		// error!
	}
}


inline void
gl_set_texture(const image& img)
{
	gl_set_texture(GL_TEXTURE_2D, img);
}

inline void
gl_set_texture_mipmaps(const image& img, int shrink_rate)
{
	gl_set_texture_mipmaps(GL_TEXTURE_2D, img, shrink_rate);
}


//----
inline  void
set_glColor3f(const image& img, int x, int y)
{
	byte R, G, B;
	img.getRGB(x,y, R,G,B);
	glColor3ub( R, G, B );
}


#endif