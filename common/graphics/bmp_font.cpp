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
#include  "bmp_font.h"
#include  <vector>
#include  <map>




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






//------------
//  bmp_font
//------------
bmp_font::bmp_font()
{
	init(default_bmp_size, default_char_size);
}



bmp_font::bmp_font(int _bmp_size, int _char_size)
{
	init(_bmp_size, _char_size);
}



bmp_font::~bmp_font()
{
	clear();
}



void
bmp_font::init(int _bmp_size, int _char_size)
{
	clear();

	bmp_size = to2n(_bmp_size);
	char_size = _char_size;

	Nx = Ny = bmp_size / char_size;
	num_chars_per_page = Nx * Ny;


	// make buffer info for mipmap
	bmp_font_buffer_info bfi;
	
	const int char_size_min = 32;
	int mipmapN = 0;

	int ch_sz = char_size;
	while (ch_sz >= char_size_min) {
		mipmapN++;
		ch_sz /= 2;
	}

	if (mipmapN == 0) {
		mipmapN = 1;
	}



	int offset = 0;
	int bmp_sz = bmp_size;
	ch_sz = char_size;
	int mgn = 1 << (mipmapN-1);
	total_buf_size = 0;

	for (int i = 0; i < mipmapN; i++) {
		int buf_size;

		buf_size = bmp_sz * bmp_sz;

		bfi.lv = i;
		bfi.offset = offset;
		bfi.buf_size = buf_size;
		bfi.bmp_size = bmp_sz;
		bfi.char_size = ch_sz;
		bfi.mgn = mgn;
		vBufInfo.push_back(bfi);

		bmp_sz /= 2;
		ch_sz /= 2;
		mgn /= 2;
		offset += buf_size;
		total_buf_size += buf_size;
	}
}



void
bmp_font::clear()
{
	vFace.clear();
	vChars.clear();
	vBufInfo.clear();

	for (int i = 0; i < vBmps.size(); i++) {
		delete[] vBmps[i];
	}
	vBmps.clear();
}


void
bmp_font::add_face(FT_Face face)
{
	vFace.push_back(face);
}




void
bmp_font::add_new_page()
{
	unsigned char *pBmp = new unsigned char[total_buf_size];
	vBmps.push_back(pBmp);
}


bool
bmp_font::render_font(FT_Face face, int page, int ix0, int iy0, const utf32& uc32)
{
	const bmp_font_buffer_info& bfi = get_buffer_info(0);

	const int render_size0 = bfi.char_size - bfi.mgn;
	int render_size = render_size0;



	//--  Load glyph
	FT_UInt glyph_index = FT_Get_Char_Index(face, uc32);
	if (glyph_index == 0)  return false;



	while (true) {
		//--  Set size
		FT_Error error;

		error = FT_Set_Pixel_Sizes(face, render_size, render_size);
		if (error) return false;

		error = FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_BITMAP | FT_LOAD_TARGET_LIGHT);
		//error = FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_BITMAP);
		if (error) return false;

		// check size
		FT_Bitmap&  bmpFT = face->glyph->bitmap;
		if (bmpFT.width <= render_size0 && bmpFT.rows <= render_size0) {
			break;
		}

		render_size--;
	}





	FT_Bitmap&  bmpFT = face->glyph->bitmap;

	unsigned char *pbmp;
	pbmp = vBmps[page] + bfi.offset;

	ix0 >>= bfi.lv;
	iy0 >>= bfi.lv;
	int char_sz = bfi.char_size;
	int bmp_sz = bfi.bmp_size;

	//--  Clear
	for (int iy = 0; iy < char_sz; iy++) {
		for (int ix = 0; ix < char_sz; ix++) {
			pbmp[(iy + iy0)*bmp_sz + ix + ix0] = 0;
		}
	}


	//--  Copy bitmap
	int w, h;
	w = bmpFT.width;
	h = bmpFT.rows;

	unsigned char* p = bmpFT.buffer;

	if (bmpFT.pixel_mode == FT_PIXEL_MODE_GRAY) {
		for (int iy = 0; iy<h; iy++) {
			for (int ix = 0; ix<w; ix++) {
				pbmp[(h - 1 - iy + iy0)*bmp_sz + ix + ix0] = p[iy*w + ix];
			}
		}
	}
	else if (bmpFT.pixel_mode == FT_PIXEL_MODE_MONO) {
		int ww = w / 8;
		if ((w % 8) != 0)  ww++;

		for (int iy = 0; iy<h; iy++) {
			for (int ix = 0; ix<ww; ix++) {

				unsigned char m = 128;
				unsigned char c = p[iy*ww + ix];

				for (int i = 0; i<8; i++) {
					unsigned char val;
					val = (c & m) ? 255 : 0;
					pbmp[(h - 1 - iy + iy0)*bmp_sz + (ix + ix0) * 8 + i] = val;
					m >>= 1;
				}
			}
		}
	}



	return true;
}


void
bmp_font::make_mipmap(int page, int ix0, int iy0)
{
	const bmp_font_buffer_info& bfi0 = vBufInfo[0];
	const int bmp_sz0 = bfi0.bmp_size;
	const int char_sz0 = bfi0.char_size;
	unsigned char* pbmp0 = vBmps[page];



	const int mipmapN = get_mipmap_num();
	for (int lv = 1; lv < mipmapN; lv++) {
		const bmp_font_buffer_info& bfi = vBufInfo[lv];
		unsigned char* pbmp = pbmp0 + bfi.offset;
		const int bmp_sz = bfi.bmp_size;
		const int char_sz = bfi.char_size;
		const int subN = bfi0.char_size / bfi.char_size;

		for (int iy = 0; iy < char_sz0; iy+= subN) {
			for (int ix = 0; ix < char_sz0; ix+=subN) {
				long sum = 0;

				for (int iy2 = 0; iy2 < subN; iy2++) {
					for (int ix2 = 0; ix2 < subN; ix2++) {
						sum += long(*(pbmp0 + (iy0 + iy + iy2)*bmp_sz0 + (ix0 + ix + ix2)));
					}
				}
				*(pbmp + (iy0 + iy) / subN*bmp_sz + (ix0 + ix)/subN) = sum / (subN * subN);
			}
		}
	}
}



bool
bmp_font::add_char(const utf32& uc32)
{
	if (is_registered(uc32)) {
		// already registered
		return true;
	}


	// page
	int char_idx, page;
	char_idx = get_total_num_chars();
	page = char_idx / num_chars_per_page;

	if (page > get_num_pages()-1) {
		add_new_page();
	}


	//--  Set position on bitmap
	int rs, ix0, iy0;
	rs = char_idx % num_chars_per_page;
	iy0 = (rs / Nx) * char_size;
	ix0 = (rs % Nx) * char_size;


	//--  Render font
	FT_Face face;
	bool res;
	for (int i = 0; i < vFace.size(); i++) {
		face = vFace[i];
		res = render_font(face, page, ix0, iy0, uc32);
		if (res) break;
	}

	if (!res) {
		utf32 uc32_2 = L'?';
		//utf32 uc32_2 = 0xFFFD;
		add_char(uc32_2);
		vChars[uc32] = vChars[uc32_2];

		return false;
	}



	//--  Glyph info
	bmp_font_char_info cif;
	FT_Bitmap&  bmpFT = face->glyph->bitmap;
	cif.page  = page;
	cif.ix0   = ix0;
	cif.iy0   = iy0;
	cif.left  = face->glyph->bitmap_left;
	cif.top   = face->glyph->bitmap_top;
	cif.adv_x = face->glyph->advance.x;
	cif.adv_y = face->glyph->advance.y;
	cif.width = bmpFT.width;
	cif.rows  = bmpFT.rows;


	const float scl1   = 1.0f / char_size;
	const float sclTex = 1.0f / bmp_size;
	const float sclFT = 1.0f / (64.0f * char_size);


	float wf, hf;
	cif.x1 = scl1 * cif.left;
	cif.y2 = scl1 * cif.top;
	wf = scl1 * cif.width;
	hf = scl1 * cif.rows;
	cif.x2 = cif.x1 + wf;
	cif.y1 = cif.y2 - hf;
	cif.dx = sclFT * cif.adv_x;
	cif.dy = sclFT * cif.adv_y;

	float dd = 0.0f;
	cif.u1 = sclTex * (cif.ix0 + dd);
	cif.v1 = sclTex * (cif.iy0 + dd);
	cif.u2 = sclTex * (cif.ix0 + cif.width - dd);
	cif.v2 = sclTex * (cif.iy0 + cif.rows - dd);

	//--  Register to or overwrite char map
	vChars[uc32] = cif;

		
	//--  Render for mipmaps
	make_mipmap(page, ix0, iy0);


	return true;
}



const bmp_font_char_info&
bmp_font::get_char_info(const utf32& uc32)
{
	if (!is_registered(uc32)) {
		add_char(uc32);
	}

	return  vChars.at(uc32);
}


float
bmp_font::get_dx(const utf32& uc32)
{
	return  get_char_info(uc32).dx;
}


float
bmp_font::get_dx_kerning(const utf32& uc32, const utf32& uc32_prev)
{
	//*****
	return  get_dx(uc32);

	/*
	const float sclFT = 1.0f / (64.0f * char_size);
	const bmp_font_char_info& chif = get_char_info(uc32);
	const bmp_font_char_info& chif_prev = get_char_info(uc32_prev);

	FT_Vector delta;
	FT_Get_Kerning(face, chif_prev.glyph_index, chif.glyph_index, ft_kerning_default, &delta);
	
	return chif.dx + sclFT * delta.x;
	*/
}



unsigned char*
bmp_font::get_bmp(int page)
{
	if (page < 0 || page > int(vBmps.size()) - 1) {
		return 0;
	}

	return vBmps[page];
}

