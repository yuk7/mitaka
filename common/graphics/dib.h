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
//  2002/8/9
#ifndef __DIB_H__
#define __DIB_H__
#include  <windows.h>
#include  "image.h"


//=============
//  class dib
//=============
class
dib
{
	HDC  hDC;
	HBITMAP  hBmp, hBmpOld;
	BITMAPINFOHEADER  bmih;
	BYTE*  pBits;
	unsigned  RowLength;
public:
	dib();
	dib(const BITMAPINFOHEADER& _bmih);
	dib(int w, int h, int bitcount);
	~dib();
	void  alloc(const BITMAPINFOHEADER& _bmih);
	void  alloc(int w, int h, int bitcount);
	void  free();
	int		get_width() const	{ return  bmih.biWidth; }
	int		get_height() const	{ return  bmih.biHeight; }
	int		get_row_length() const { return RowLength; }
	int		get_image_size() const { return bmih.biSizeImage; }
	BYTE*	get_pBits()		{ return pBits; }
	BITMAPINFOHEADER*	get_pbmih() { return &bmih; }
	HDC		getDC() const  { return hDC; }
	bool	set_pixelformat(const PIXELFORMATDESCRIPTOR& pfd);
	bool	set_pixelformat(DWORD dwFlags, int nDepth, int nStencil);
	void	getRGB(int ix, int iy, BYTE& R, BYTE& G, BYTE& B) const;
	void	setRGB(int ix, int iy, const BYTE& R, const BYTE& G, const BYTE& B);
	void	fill( DWORD dwRop )	 { ::PatBlt( hDC, 0,0, bmih.biWidth, bmih.biHeight, dwRop ); }
	bool	load(FILE* fp);
	bool	save(FILE* fp) const;
	bool	load(const char* filename);
	bool	save(const char* filename) const;
};

//===  functions  ===
void	image_to_dib(const image& img, dib& bmp);


#endif