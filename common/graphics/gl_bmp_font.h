/**************************************************************************************************

Written by Tsunehiko Kato
Copyright (c) 2008-2015   Tsunehiko Kato

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
#ifndef __GL_BMP_FONT_H__
#define __GL_BMP_FONT_H__
#include  <ft2build.h>
#include  FT_FREETYPE_H
#include  "../text/unicode.h"
#include  <string>
#include  "./bmp_font.h"






struct gl_font_info {
	int num_chars;
	GLuint texname;
};



class  gl_bmp_font : public bmp_font
{
	std::vector<gl_font_info>  vGLTextures;

	float	draw_char_utf32(const utf32& uc32, GLuint tex_prev);
public:
	int		get_text_size(const std::string& str, float& w, float& h);
	int		get_text_size_raw(const std::string& str, float& w, float& h);

	void	draw_text_raw(const std::string& text, const float& D);
	void	draw_text(const std::string& text, bool bDrawRuby);
	void	draw_text_centering(const std::string& text, bool bDrawRuby);
	void	draw_text_right(const std::string& text, bool bDrawRuby);
private:
	float	get_sub_text_block_size(const char* p, bool bKern, int& adv);
	float	get_superscript_text_size(const char* p, bool bKern, int& adv);
	float	get_subscript_text_size(const char* p, bool bKern, int& adv);
	float	draw_sub_text_block(const char *p, bool bKern, int& adv);
	void	draw_superscript(const char *p, bool bKern, int& adv);
	void	draw_subscript(const char *p, bool bKern, int& adv);
	void	draw_ruby(const char* p0, bool bKern, int& adv, float scl, float w);
	void	skip_ruby(const char* p0, int& adv) const;
};






#endif
