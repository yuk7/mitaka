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
#ifndef __BMP_FONT_H__
#define __BMP_FONT_H__
#include  <ft2build.h>
#include  FT_FREETYPE_H
#include  <vector>
#include  <map>
#include  "../text/unicode.h"





//-------------
//  char info
//-------------
struct bmp_font_char_info
{
	//-- Bitmap
	int		page;
	int		ix0, iy0;
	//-- Glyph
	FT_UInt glyph_index;
	FT_Int	left, top;
	FT_Int	width, rows;
	FT_Int	adv_x, adv_y;
	//-- for texture
	float x1, y1, x2, y2, dx, dy;
	float u1, v1, u2, v2;
};


//---------------
//  buffer info
//---------------
struct bmp_font_buffer_info {
	int lv;
	int	offset;
	int	buf_size;
	int bmp_size;
	int char_size;
	int mgn;
};





//------------
//  bmp_font
//------------
class bmp_font
{
	static const int default_bmp_size = 1024;
	static const int default_char_size = 32;

	std::vector<FT_Face>  vFace;
	std::map<utf32, bmp_font_char_info>	vChars;
	std::vector<bmp_font_buffer_info> vBufInfo;
	std::vector<unsigned char*> vBmps;
	int total_buf_size;
	int bmp_size, char_size;
	int Nx, Ny, num_chars_per_page;

public:
	bmp_font();
	bmp_font(int _bmp_size, int _char_size);
	~bmp_font();

	void	init(int _bmp_size, int _char_size);
	void	clear();
	void	add_face(FT_Face);

	int		get_num_pages() const { return vBmps.size(); }
	int		get_total_num_chars() const { return vChars.size(); }
	int		get_num_chars(int page) const;
	
	bool	add_char(const utf32& uc32);
	bool	is_registered(const utf32& uc32) const { return (vChars.find(uc32) != vChars.end()); };

	const bmp_font_char_info&	get_char_info(const utf32& uc32);
	float	get_dx(const utf32& uc32);
	float	get_dx_kerning(const utf32& uc32, const utf32& uc32_prev);
	float	get_dx(const utf32& uc32, const utf32& uc32_prev);

	int		get_mipmap_num() const		{ return vBufInfo.size(); }
	const bmp_font_buffer_info&		get_buffer_info(int i) const { return vBufInfo[i]; }

	int	get_bmp_size() const  { return bmp_size; }
	int get_char_size() const { return char_size; }
	unsigned char *get_bmp(int page);

private:
	void	add_new_page();
	bool	render_font(FT_Face face, int page, int ix0, int iy0, const utf32& uc32);
	void	make_mipmap(int page, int ix0, int iy0);
};



inline  int
bmp_font::get_num_chars(int page) const
{
	if (page < 0 || page >= get_num_pages()) {
		return 0;
	}

	int cur_page = get_num_pages() - 1;
	if (page < cur_page) {
		return num_chars_per_page;
	}

	if (get_total_num_chars() == 0) return 0;

	return ((get_total_num_chars() -1) % num_chars_per_page) + 1;
}


inline  float
bmp_font::get_dx(const utf32& uc32, const utf32& uc32_prev)
{
	if (uc32_prev == 0) {
		return get_dx(uc32);		//  Without Kerning
	}
	else {
		
		return  get_dx_kerning(uc32, uc32_prev);		// With Kerning
	}
}





#endif
