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
#ifdef WIN32
	#include  <windows.h>
#endif
#define GLEW_STATIC
#include  <gl/glew.h>
#include  "gl_bmp_font.h"
#include  "bmp_font.h"
#include  <vector>




//--  Size
static const float superscript_size = 0.5;
static const float subscript_size = 0.5;
static const float ruby_size = 0.5;





//-------------
//  Text size
//-------------

float
gl_bmp_font::get_sub_text_block_size(const char* p, bool bKern, int& adv)
{
	utf32 uc32;
	float w = 0;

	uc32 = UTF8_to_UTF32(p, adv);
	if (uc32 == L'\0') return 0;

	p += adv;

	if (uc32 != L'{') {
		w = get_dx(uc32);
		//w = get_dx(uc32, bKern, 0);
		return w;
	}


	while (*p != '\0') {
		int adv2;
		uc32 = UTF8_to_UTF32(p, adv2);
		if (uc32 == L'\0') break;

		p += adv2;
		adv += adv2;
		if (uc32 == L'}') break;

		w += get_dx(uc32);
		//w += get_char_size(uc32, bKern, 0);
	}

	return w;
}


float
gl_bmp_font::get_superscript_text_size(const char* p, bool bKern, int& adv)
{
	return  superscript_size * get_sub_text_block_size(p, bKern, adv);
}


float
gl_bmp_font::get_subscript_text_size(const char* p, bool bKern, int& adv)
{
	return  subscript_size * get_sub_text_block_size(p, bKern, adv);
}


int
gl_bmp_font::get_text_size(const std::string& str, float& w, float& h)
{

	//*****
	bool bKern = false;
	//*****

	//bool bKern = bKerning && FT_HAS_KERNING(face);

	int count = 0;
	h = 1;
	w = 0;

	int count2 = 0;


	utf32 uc32_prev = 0;
	const char *p = str.c_str();
	const int sz = str.size();
	while (*p != '\0') {
		utf32  uc32;
		int adv;

		uc32 = UTF8_to_UTF32(p, adv);
		if (uc32 == L'\0') break;

		p += adv;

	
		
		count2 += adv;
		if (count2 > sz) {
			w = 0;
			h = 1;
			break;
		}



		//---  Escape  ---
		if (uc32 == L'\\') {

			uc32 = UTF8_to_UTF32(p, adv);
			p += adv;

			if (uc32 != L'\\') {
				if (uc32 == L'S') {
					w += get_superscript_text_size(p, bKern, adv);
					p += adv;
				}
				else if (uc32 == L'B') {
					w += get_subscript_text_size(p, bKern, adv);
					p += adv;
				}

				uc32_prev = 0;
				continue;
			}
		}



		//---  Ruby  ---
		if (uc32 == L'{') {
			//x = 0;
			continue;
		}
		if (uc32 == L'|') {
			while (*p != '\0') {
				uc32 = UTF8_to_UTF32(p, adv);
				if (uc32 == L'\0') break;

				p += adv;

				if (uc32 == L'}') {
					break;
				}
			}
			continue;
		}
		if (uc32 == L'}') {
			//ch_prev = ch_prev0;
			continue;
		}



		//---  Char  ---
		w += get_dx(uc32, uc32_prev);
		if (bKern) {
			uc32_prev = uc32;
		}

		count++;
	}
	return  count;
}



int
gl_bmp_font::get_text_size_raw(const std::string& str, float& w, float& h)
{
	//bool bKern = bKerning && FT_HAS_KERNING(face);
	//*****
	bool bKern = false;
	//*****


	int count = 0;

	h = 1;
	w = 0;


	utf32 uc32_prev = 0;

	const char *p = str.c_str();
	while (*p != '\0') {
		utf32  uc32;
		int adv;

		uc32 = UTF8_to_UTF32(p, adv);
		if (uc32 == L'\0') break;

		p += adv;

		w += get_dx(uc32, uc32_prev);
		if (bKern) {
			uc32_prev = uc32;
		}

		count++;
	}

	return  count;
}





//--------
//  Draw
//--------
float
gl_bmp_font::draw_char_utf32(const utf32& uc32, GLuint tex_prev)
{
	//***
	static bool first = true;



	bool res;
	res = add_char(uc32);
	if (!res) {
		return 0;
	}


	const bmp_font_char_info& chif = get_char_info(uc32);

	int bmp_size, page;
	bmp_size = get_bmp_size();
	page = chif.page;





	enum {
		BMP_FONT_ALREADY_REGISTERED,
		BMP_FONT_IN_PAGE,
		BMP_FONT_PAGE_ADDED
	};

	int reg_stat;
	if (page + 1 > vGLTextures.size()) {
		reg_stat = BMP_FONT_PAGE_ADDED;

		while (page + 1 > vGLTextures.size()) {
			int _page = int(vGLTextures.size());

			gl_font_info  gfi;
			glGenTextures(1, &gfi.texname);
			gfi.num_chars = get_num_chars(_page);
			vGLTextures.push_back(gfi);

			glBindTexture(GL_TEXTURE_2D, gfi.texname);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			
			const int mipmapN = get_mipmap_num();
			if (mipmapN == 1) {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}
			else {
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmapN - 1);
			}

			
			unsigned char *pbuf = get_bmp(_page);
			for (int i = 0; i < mipmapN; i++) {
				const bmp_font_buffer_info& bfi = get_buffer_info(i);
				glTexImage2D(GL_TEXTURE_2D, i, GL_ALPHA, bfi.bmp_size, bfi.bmp_size, 0, GL_ALPHA, GL_UNSIGNED_BYTE, pbuf + bfi.offset);
			}
		}
	}
	else if (vGLTextures[page].num_chars != get_num_chars(page)) {
		reg_stat = BMP_FONT_IN_PAGE;
		glBindTexture(GL_TEXTURE_2D, vGLTextures[page].texname);

		
		unsigned char *pbuf = get_bmp(page);
		const int mipmapN = get_mipmap_num();
		for (int i = 0; i < mipmapN; i++) {
			const bmp_font_buffer_info& bfi = get_buffer_info(i);
			glTexSubImage2D(GL_TEXTURE_2D, i, 0, 0, bfi.bmp_size, bfi.bmp_size, GL_ALPHA, GL_UNSIGNED_BYTE, pbuf + bfi.offset);
		}
		
		
		vGLTextures[page].num_chars = get_num_chars(page);
	}
	else {
		reg_stat = BMP_FONT_ALREADY_REGISTERED;
		if (vGLTextures[page].texname != tex_prev) {
			//{
			glBindTexture(GL_TEXTURE_2D, vGLTextures[page].texname);
			tex_prev = vGLTextures[page].texname;
		}
	}
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);



	//--  Kerning




	//--  Draw
	float x1, y1, x2, y2, u1, v1, u2, v2;

	x1 = chif.x1;
	y1 = chif.y1;
	x2 = chif.x2;
	y2 = chif.y2;

	u1 = chif.u1;
	v1 = chif.v1;
	u2 = chif.u2;
	v2 = chif.v2;


	glBegin(GL_QUADS);
	glTexCoord2f(u1, v1);	glVertex2d(x1, y1);
	glTexCoord2f(u2, v1);	glVertex2d(x2, y1);
	glTexCoord2f(u2, v2);	glVertex2d(x2, y2);
	glTexCoord2f(u1, v2);	glVertex2d(x1, y2);
	glEnd();

	glTranslatef(chif.dx, chif.dy, 0);



	return chif.dx;
}





void
gl_bmp_font::draw_text_raw(const std::string& text, const float& D)
{
	//***
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//***


	//float x = 0;
	GLuint tex_prev = 0;
	const char *p = text.c_str();

	while (*p != 0) {
		utf32 uc32;
		int adv;

		uc32 = UTF8_to_UTF32(p, adv);
		p += adv;

		draw_char_utf32(uc32, tex_prev);
		//x += draw_char_utf32(fnt, uc32);
		glTranslatef(D, 0, 0);
	}


	//***
	glPopAttrib();
	//***
}




void
gl_bmp_font::draw_ruby(const char* p0, bool bKern, int& adv, float scl, float w)
{
	//--  Calc char count and width
	const char* p = p0;
	int count = 0;
	float wr = 0;
	utf32 ch_prev = 0;
	adv = 0;

	while (*p != '\0') {
		int adv2;
		utf32 uc32;

		uc32 = UTF8_to_UTF32(p, adv2);
		if (uc32 == L'\0') break;

		p += adv2;
		adv += adv2;

		if (uc32 == L'}') {
			break;
		}


		//wr += get_char_size(uc32, bKern, ch_prev);
		if (bKern) {
			wr += get_dx(uc32, ch_prev);
		} else{
			wr += get_dx(uc32);
		}
		count++;
		ch_prev = uc32;
	}
	wr *= scl;


	//--
	if (count == 0) {
		return;
	}
	if (count == 1) {
		glPushMatrix();
		glTranslatef(-0.5*(w + wr), 1, 0);
		glScalef(scl, scl, scl);

		draw_char_utf32(ch_prev, 0);

		glPopMatrix();
		return;
	}

	//--  Calc space
	float d = (w - wr) / (count - 1) / scl;

	glPushMatrix();
	glTranslatef(-w, 1, 0);
	glScalef(scl, scl, scl);

	p = p0;
	ch_prev = 0;
	while (*p != '\0') {
		int adv2;
		utf32 uc32;

		uc32 = UTF8_to_UTF32(p, adv2);
		if (uc32 == L'\0') break;

		p += adv2;

		if (uc32 == L'}') {
			break;
		}

		draw_char_utf32(uc32, 0);
		ch_prev = uc32;

		glTranslatef(d, 0, 0);
	}

	glPopMatrix();
}



void
gl_bmp_font::skip_ruby(const char* p0, int& adv) const
{
	const char* p = p0;
	adv = 0;

	while (*p != '\0') {
		int adv2;
		utf32 uc32;

		uc32 = UTF8_to_UTF32(p, adv2);
		if (uc32 == L'\0') break;

		p += adv2;
		adv += adv2;

		if (uc32 == L'}') {
			break;
		}
	}
}



float
gl_bmp_font::draw_sub_text_block(const char *p, bool bKern, int& adv)
{
	utf32  uc32;
	float w = 0;


	uc32 = UTF8_to_UTF32(p, adv);
	p += adv;

	if (uc32 != L'{') {
		w = draw_char_utf32(uc32, 0);
		return w;
	}


	while (*p != '\0') {
		int adv2;
		uc32 = UTF8_to_UTF32(p, adv2);
		if (uc32 == L'\0') break;

		p += adv2;
		adv += adv2;
		if (uc32 == L'}') break;

		w += draw_char_utf32(uc32, 0);
	}

	return w;
}


void
gl_bmp_font::draw_superscript(const char*p, bool bKern, int& adv)
{
	float w;
	float scl = superscript_size;

	glPushMatrix();
		glTranslatef(0, 1 - scl, 0);
		glScalef(scl, scl, scl);
		w = draw_sub_text_block(p, bKern, adv);
		glPopMatrix();
	glTranslatef(w*scl, 0, 0);
}


void
gl_bmp_font::draw_subscript(const char *p, bool bKern, int& adv)
{
	float w;
	float scl = subscript_size;

	glPushMatrix();
		glScalef(scl, scl, scl);
		w = draw_sub_text_block(p, bKern, adv);
		glPopMatrix();
	glTranslatef(w*scl, 0, 0);
}



void
gl_bmp_font::draw_text(const std::string& text, bool bDrawRuby)
{
	//***
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//***


	utf32 ch_prev, ch_prev0;
	bool bKern = false;



	int count = 0;


	float x = 0;
	GLuint tex_prev = 0;
	const char *p = text.c_str();
	const int sz = text.size();
	ch_prev = ch_prev0 = 0;
	while (*p != 0) {
		utf32 uc32;
		int adv;

		uc32 = UTF8_to_UTF32(p, adv);
		if (uc32 == L'\0') break;

		p += adv;


		count += adv;
		if (count > sz) {
			break;
		}


		//---  Escape  ---
		if (uc32 == L'\\') {

			uc32 = UTF8_to_UTF32(p, adv);
			p += adv;

			if (uc32 != L'\\') {
				if (uc32 == L'S') {
					draw_superscript(p, bKern, adv);
					p += adv;
				}
				else if (uc32 == L'B') {
					draw_subscript(p, bKern, adv);
					p += adv;
				}

				ch_prev = 0;
				continue;
			}
		}


		
		//---  Ruby  ---
		if (uc32 == L'{') {
			x = 0;
			continue;
		}
		if (uc32 == L'|') {
			ch_prev0 = ch_prev;
			
			if (bDrawRuby) {
				draw_ruby(p, bKern, adv, ruby_size, x);
			}
			else {
				skip_ruby(p, adv);
			}
			
			p += adv;
			continue;
		}
		if (uc32 == L'}') {
			ch_prev = ch_prev0;
			continue;
		}
		

		//---  Char  ---
		x += draw_char_utf32(uc32, tex_prev);
	}

	
	//***
	glPopAttrib();
	//***
}





//-------------
//  Centering
//-------------
void
gl_bmp_font::draw_text_centering(const std::string& text, bool bDrawRuby)
{
	float w, h;
	get_text_size(text, w, h);

	glTranslatef(-0.5*w, 0, 0);
	draw_text(text, bDrawRuby);
}


void
gl_bmp_font::draw_text_right(const std::string& text, bool bDrawRuby)
{
	float w, h;
	get_text_size(text, w, h);

	glTranslatef(-w, 0, 0);
	draw_text(text, bDrawRuby);
}

