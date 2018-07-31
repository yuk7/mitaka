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
#include  <stdio.h>
#include  "dib.h"


//==============
//  class  dib
//==============
dib::dib() : hBmp(0), pBits(0)
{
	hDC = CreateCompatibleDC(NULL);
}

dib::dib(const BITMAPINFOHEADER& _bmih)
{
	hDC = CreateCompatibleDC(NULL);
	alloc(_bmih);
}

dib::dib(int w, int h, int bitcount) : hBmp(0), pBits(0)
{
	hDC = CreateCompatibleDC(NULL);
	alloc(w, h, bitcount);
}

dib::~dib()
{
	free();
	DeleteDC(hDC);
}

void
dib::alloc(const BITMAPINFOHEADER& _bmih)
{
	if (_bmih.biWidth  <= 0 || _bmih.biHeight <= 0)  return; 
	bmih = _bmih;

	int pal_ent;
	switch (bmih.biBitCount) {
		case 1:		pal_ent = 2;	break;
		case 4:		pal_ent = 16;	break;
		case 8:		pal_ent = 256;	break;
		case 16:
		case 24:
		case 32:	pal_ent = 0;	break;
		default:	pal_ent = 0;	break;	// エラー
	}

	BITMAPINFO*  pbmi;
	pbmi = (BITMAPINFO*)malloc(sizeof(BITMAPINFO) + (pal_ent-1)*sizeof(RGBQUAD));
	pbmi->bmiHeader = bmih;

	UINT iUsage;
	if (pal_ent > 0) {
		iUsage = DIB_PAL_COLORS;
		for (int i=0; i<pal_ent; ++i) {
			unsigned char val;
			val = 255*i/(pal_ent-1);
			pbmi->bmiColors[i].rgbRed   = val;
			pbmi->bmiColors[i].rgbGreen = val;
			pbmi->bmiColors[i].rgbBlue  = val;
			pbmi->bmiColors[i].rgbReserved = 0;
		}
	} else {
		iUsage = DIB_RGB_COLORS;
	}

	//-- allocate memory for pixel bits
	free();
	RowLength = 4 * ((bmih.biWidth * bmih.biBitCount + 31) / 32);

	hBmp = CreateDIBSection(hDC, pbmi, iUsage, (void**)&pBits, NULL, 0);
	if (hBmp) {
		hBmpOld = (HBITMAP)SelectObject(hDC, hBmp);
	}
	::free(pbmi);
}

void
dib::alloc(int w, int h, int bitcount)
{
	if (pBits!=0 && w==bmih.biWidth && h==bmih.biHeight && bitcount==bmih.biBitCount) {
		return;
	}
	RowLength = 4 * ((w * bitcount + 31) / 32);
	//-- set BITMAPINFOHEADER 
	BITMAPINFOHEADER _bmih;
	_bmih.biSize = 40;
	_bmih.biWidth = w;
	_bmih.biHeight = h;
	_bmih.biPlanes = 1;		// always 1
	_bmih.biBitCount = bitcount;
	_bmih.biCompression = BI_RGB;
	_bmih.biSizeImage = RowLength * h;
	_bmih.biXPelsPerMeter = 0;
	_bmih.biYPelsPerMeter = 0;
	_bmih.biClrUsed = 0;
	_bmih.biClrImportant = 0;
	alloc(_bmih);
}

void
dib::free()
{
	if (hBmp==0)  return;
	HBITMAP  hBmpTmp = (HBITMAP)SelectObject(hDC, hBmpOld); 
	DeleteObject(hBmp);
	hBmp = 0;
	pBits = 0;
}


bool
dib::set_pixelformat(const PIXELFORMATDESCRIPTOR& pfd)
{
	int iPixelFormat = ChoosePixelFormat(getDC(), &pfd);
	BOOL  success = SetPixelFormat(getDC(), iPixelFormat, &pfd);
	return  (success == TRUE);
}

bool
dib::set_pixelformat(DWORD dwFlags, int nDepth, int nStencil)
{
	PIXELFORMATDESCRIPTOR  pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,					// version
		dwFlags,			// Flags
		PFD_TYPE_RGBA,		// Pixel type
		bmih.biBitCount,	// Bitcount
		0, 0, 0, 0, 0, 0,	// R,G,B
		8, 0,				// A
		0, 0, 0, 0, 0,		// Accum
		nDepth,				// Depth
		nStencil,			// Stencil
		0,					// Aux
		PFD_MAIN_PLANE,		// Layer type (ignored)
		0,					// Reserved
		0, 0, 0				// Mask
	};

	return  set_pixelformat(pfd);
}


void
dib::getRGB(int ix, int iy, BYTE& R, BYTE& G, BYTE& B) const
{
	BYTE* p = pBits + RowLength*iy + 3*ix;
	p[0] = B;
	p[1] = G;
	p[2] = R;
}

void
dib::setRGB(int ix, int iy, const BYTE& R, const BYTE& G, const BYTE& B)
{
	BYTE* p = pBits + RowLength*iy + 3*ix;
	p[0] = B;
	p[1] = G;
	p[2] = R;
}

bool
dib::load(FILE* fp)
{
	size_t count;

	//-- Read BMP file header
	BITMAPFILEHEADER  bfh;
	count  = fread(&bfh.bfType, sizeof(BYTE), 2, fp);
	count += fread(&bfh.bfSize, sizeof(DWORD), 1, fp);
	count += fread(&bfh.bfReserved1, sizeof(WORD), 1, fp);
	count += fread(&bfh.bfReserved2, sizeof(WORD), 1, fp);
	count += fread(&bfh.bfOffBits, sizeof(DWORD), 1, fp);
	if (count != 6)  return  false;

	//-- Check BMP ID
	if ( bfh.bfType != 0x4d42 )  return false;

	//-- Read BITMAPINFOHEADER
	BITMAPINFOHEADER  bih;
	count  = fread( &bih.biSize,			sizeof(DWORD), 1, fp );
	count += fread( &bih.biWidth,			sizeof(long), 1, fp );
	count += fread( &bih.biHeight,			sizeof(long), 1, fp );
	count += fread( &bih.biPlanes,			sizeof(WORD), 1, fp );
	count += fread( &bih.biBitCount,		sizeof(WORD), 1, fp );
	count += fread( &bih.biCompression,		sizeof(DWORD), 1, fp );
	count += fread( &bih.biSizeImage,		sizeof(DWORD), 1, fp );
	count += fread( &bih.biXPelsPerMeter,	sizeof(long), 1, fp );
	count += fread( &bih.biYPelsPerMeter,	sizeof(long), 1, fp );
	count += fread( &bih.biClrUsed,			sizeof(DWORD), 1, fp );
	count += fread( &bih.biClrImportant,	sizeof(DWORD), 1, fp );
	if (count != 11)  return false;

	//-- Check data type
	if (bih.biBitCount != 24)  return false;
	if (bih.biCompression != BI_RGB)  return false;

	//-- Allocate memory
	alloc(bih);

	//-- Read pixel data
	if ( fseek(fp, bfh.bfOffBits, SEEK_SET) != 0 )  return false;
	for (int iy=0; iy<bmih.biHeight; iy++) {
		if ( fread( pBits + iy*RowLength, sizeof(byte), RowLength, fp) != RowLength ) {
			free();
			return  false;
		}
	}
	
	return  true;
}

bool
dib::save(FILE* fp) const
{
	BITMAPFILEHEADER  bfh;
	size_t count;

	//---  Set headers  ---
	bfh.bfType		= 0x4d42;
	bfh.bfSize		= get_image_size() + 14 + 40 + 2;
	bfh.bfReserved1	= 0;
	bfh.bfReserved2	= 0;
	bfh.bfOffBits	= 14 + 40;
	if (bmih.biBitCount == 8) {
		bfh.bfSize    += 256*sizeof(RGBQUAD);
		bfh.bfOffBits += 256*sizeof(RGBQUAD);
	}

	//---  Write BMP file header  ---
	count  = fwrite(&bfh.bfType, sizeof(BYTE), 2, fp);
	count += fwrite(&bfh.bfSize, sizeof(DWORD), 1, fp);
	count += fwrite(&bfh.bfReserved1, sizeof(WORD), 1, fp);
	count += fwrite(&bfh.bfReserved2, sizeof(WORD), 1, fp);
	count += fwrite(&bfh.bfOffBits, sizeof(DWORD), 1, fp);
	if (count != 6)  return  false;

	//---  Write BITMAPINFOHEADER  ---
	count  = fwrite( &bmih.biSize,			sizeof(DWORD), 1, fp );
	count += fwrite( &bmih.biWidth,			sizeof(long), 1, fp );
	count += fwrite( &bmih.biHeight,		sizeof(long), 1, fp );
	count += fwrite( &bmih.biPlanes,		sizeof(WORD), 1, fp );
	count += fwrite( &bmih.biBitCount,		sizeof(WORD), 1, fp );
	count += fwrite( &bmih.biCompression,	sizeof(DWORD), 1, fp );
	count += fwrite( &bmih.biSizeImage,		sizeof(DWORD), 1, fp );
	count += fwrite( &bmih.biXPelsPerMeter,	sizeof(long), 1, fp );
	count += fwrite( &bmih.biYPelsPerMeter,	sizeof(long), 1, fp );
	count += fwrite( &bmih.biClrUsed,		sizeof(DWORD), 1, fp );
	count += fwrite( &bmih.biClrImportant,	sizeof(DWORD), 1, fp );
	if (count != 11)  return  false;

	//--- palette
	if (bmih.biBitCount==8) {
		for (int i=0; i<256; i++) {
			RGBQUAD rgb;
			rgb.rgbRed   = i;
			rgb.rgbGreen = i;
			rgb.rgbBlue  = i;
			rgb.rgbReserved = 0;
			fwrite( &rgb, sizeof(RGBQUAD), 1, fp);
		}
	}

	//---  Write pixel data  ---
	for (int iy=0; iy<bmih.biHeight; iy++) {
		if ( fwrite( pBits + iy*RowLength, sizeof(byte), RowLength, fp) != RowLength ) {
			return  false;
		}
	}

	return  true;
}


bool
dib::load(const char* filename)
{
	FILE* fp;
	bool  res;

	if ( (fp=fopen(filename,"rb")) == NULL )	return  false;
	res = load(fp);
	fclose(fp);

	return  res;
}

bool
dib::save(const char* filename) const
{
	FILE* fp;
	bool  res;

	if ( (fp=fopen(filename,"wb")) == NULL )	return  false;
	res = save(fp);
	fclose(fp);

	return  res;
}

void
image_to_dib(const image& img, dib& bmp)
{
	bmp.alloc(img.get_width(), img.get_height(), 24);

	for (int iy=0; iy<img.get_height(); iy++) {
		for (int ix=0; ix<img.get_width(); ix++) {
			BYTE R,G,B;
			img.getRGB(ix,iy,R,G,B);
			bmp.setRGB(ix,iy,R,G,B);
		}
	}
}
