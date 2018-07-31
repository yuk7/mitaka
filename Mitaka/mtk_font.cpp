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
//#include  <windows.h>
#include  "mtk_font.h"
#include  <ft2build.h>
#include  FT_FREETYPE_H
#include  "../common/graphics/gl_win.h"
#include  "../common/graphics/gl_bmp_font.h"
#include  "../common/graphics/bmp_font.h"
#include  "../common/graphics/image.h"
#include  <vector>


// for Visual Studio
#define snprintf _snprintf




//--  Critical section
static CRITICAL_SECTION cs;


//--  Freetype objects
static FT_Library	library;
static std::vector<FT_Face>		vFace;


//--  Bitmap font
static gl_bmp_font  fnt;


//-- char size / bmp size
static int font_char_size = 64;
static int font_bmp_size = 1024;





//---------------------
//  Init/Free FeeType
//--------------------
bool
init_FT()
{
	FT_Error  error;

	//--  Init library
	error = FT_Init_FreeType(&library);
	if (error)  return false;

	//--  cs
	InitializeCriticalSection(&cs);

	return true;
}

void
free_FT()
{
	free_font_face();
	FT_Done_FreeType(library);

	DeleteCriticalSection(&cs);
}





//--------
//  Face
//--------
FT_Face
init_font_face_sub(const char* sys_font_path, const char *font, int face_index)
{
	char  filename[1024];
	FT_Face face;
	FT_Error  error;

	error = FT_New_Face(library, font, face_index, &face);
	if (!error)  return face;

	snprintf(filename, 1024, "%s\\%s", sys_font_path, font);
	error = FT_New_Face(library, filename, face_index, &face);
	if (!error)  return face;

	error = FT_New_Face(library, font, 0, &face);
	if (!error)  return face;

	error = FT_New_Face(library, filename, 0, &face);
	if (!error)  return face;

	return NULL;
}


bool
init_font_face(const char* sys_font_path, const std::vector<std::string>& vfont, int face_index)
{
	free_font_face();

	fnt.init(font_bmp_size, font_char_size);


	//--  Create face object
	for (int i = 0; i<vfont.size(); i++) {
		FT_Face face;

		EnterCriticalSection(&cs);
		face = init_font_face_sub(sys_font_path, vfont[i].c_str(), face_index);
		LeaveCriticalSection(&cs);

		if (face == NULL)  continue;

		vFace.push_back(face);
		fnt.add_face(face);
	}

	return (vFace.size() > 0);
}




void
free_font_face()
{
	EnterCriticalSection(&cs);
	for (int i = 0; i < vFace.size(); i++) {
		FT_Done_Face( vFace[i] );
	}
	LeaveCriticalSection(&cs);
	vFace.clear();
}



//--------------
//  Pixel size
//--------------
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

void
set_font_rendering_sizes(int bmp_sz, int char_sz)
{
	bmp_sz = to2n(bmp_sz);
	if (bmp_sz < 64) bmp_sz = 64;
	if (bmp_sz > 4096) bmp_sz = 4096;

	if (char_sz > bmp_sz) char_sz = bmp_sz;
	if (char_sz < 8) char_sz = 8;

	font_bmp_size  = bmp_sz;
	font_char_size = char_sz;
}



//--------
//  Size
//--------
void
get_text_size(const std::string& text, float& w, float& h)
{
	EnterCriticalSection(&cs);
	fnt.get_text_size(text, w, h);
	LeaveCriticalSection(&cs);
}


int
get_text_size_raw(const std::string& text, float& w, float& h)
{
	int count;

	EnterCriticalSection(&cs);
	count = fnt.get_text_size_raw(text, w, h);
	LeaveCriticalSection(&cs);

	return count;
}





//-------------
//  Draw text
//-------------
void
draw_text(const std::string& text, bool bDrawRuby)
{
	EnterCriticalSection(&cs);
	fnt.draw_text(text, bDrawRuby);
	LeaveCriticalSection(&cs);
}


void
draw_text_raw(const std::string& text, const float &D)
{
	EnterCriticalSection(&cs);
	fnt.draw_text_raw(text, D);
	LeaveCriticalSection(&cs);
}


void
draw_textf(const char* szFormat, ...)
{
	const int N = 1024;
	static char		szBuffer[N];
	va_list			pArgs;

	va_start(pArgs, szFormat);
	vsnprintf(szBuffer, N, szFormat, pArgs);
	va_end(pArgs);

	draw_text(szBuffer);
}




//-------------
//  Centering
//-------------
void
draw_text_centering(const std::string& text, bool bDrawRuby)
{
	EnterCriticalSection(&cs);
	fnt.draw_text_centering(text, bDrawRuby);
	LeaveCriticalSection(&cs);
}

void
draw_text_right(const std::string& text, bool bDrawRuby)
{
	EnterCriticalSection(&cs);
	fnt.draw_text_right(text, bDrawRuby);
	LeaveCriticalSection(&cs);
}



//---------------------
//  文字列を１行表示する
//---------------------
void
draw_text_line(const std::string& str, char pos, bool bNewLine)
{
	glPushMatrix();

	switch (pos) {
	case TEXT_CENTER:	draw_text_centering(str);	break;
	case TEXT_RIGHT:	draw_text_right(str);		break;
	default:			draw_text(str);				break;
	}
	glPopMatrix();

	// New line
	if (bNewLine)	{ glTranslatef(0, -1, 0); }
}










//----------
//  Output
//----------
void
output_bmp_font(const char* fn, int page, int lv)
{
	const bmp_font_buffer_info& bfi = fnt.get_buffer_info(lv);

	int size = bfi.bmp_size;
	image img(size, size, 1);


	unsigned char *bmp = fnt.get_bmp(page) + bfi.offset;
	unsigned char *p = img.get_pointer();
	for (int i = 0; i < size*size; i++) {
		p[i] = bmp[i];
	}

	img.savePNG(fn);
}
