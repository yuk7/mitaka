/******************************************************************************

Copyright (c) 2007-2015   Tsunehiko Kato

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
#include  <stdlib.h>
#include  <string.h>
#include  <png.h>
#include  <zlib.h>
extern "C" {
#include <jpeglib.h>
}
#include  <math.h>
#include  "image.h"
#include  "../endian.h"


//================
//  class  image
//================
void
image::alloc(int w, int h, int chs)
{
	if (width==w && height==h && channels==chs)  return;

	free();

	width		= w;
	height		= h;
	channels	= chs;

	size = width * height * channels;
	pBits = new byte[size];
}

void
image::free()
{
	if (pBits != 0)  delete [] pBits;
	pBits = 0;

	width = height = channels = 0;
	size = 0;
}


//-------
//  BMP
//-------
//===  Windows BMP (DIB)  ===
typedef unsigned char	BYTE;
typedef unsigned short  WORD;
typedef unsigned long	DWORD;

typedef struct tagBITMAPFILEHEADER {
	BYTE	bfType[2];
	DWORD	bfSize;
	WORD	bfReserved1;
	WORD	bfReserved2;
	DWORD	bfOffBits;
}  BITMAPFILEHEADER;
typedef struct tagBITMAPINFOHEADER {
	DWORD	biSize;
	DWORD	biWidth;
	DWORD	biHeight;
	WORD	biPlanes;
	WORD	biBitCount;
	DWORD	biCompression;
	DWORD	biSizeImage;
	DWORD	biXPelsPerMeter;
	DWORD	biYPelsPerMeter;
	DWORD	biClrUsed;
	DWORD	biClrImportant;
}  BITMAPINFOHEADER;

// constants for the biCompression field
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L

bool
image::loadBMP(FILE* fp)
{
	size_t count;

	//---  Read BMP file header  ---
	BITMAPFILEHEADER  bfh;
	count  = fread( bfh.bfType, sizeof(BYTE), 2, fp);
	count += fread(&bfh.bfSize, sizeof(DWORD), 1, fp);
	count += fread(&bfh.bfReserved1, sizeof(WORD), 1, fp);
	count += fread(&bfh.bfReserved2, sizeof(WORD), 1, fp);
	count += fread(&bfh.bfOffBits, sizeof(DWORD), 1, fp);
	if (count != 6)  throw  img_err("Can't open BITMAPFILEHEADER");

	//  Check BMP ID
	if ( bfh.bfType[0]!='B' && bfh.bfType[1]!= 'M' )
		throw img_err("This file is not a BMP file");

	//---  Read BITMAPINFOHEADER  ---
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
	if (count != 11)  throw  img_err("Can't open BITMAPINFOHEADER");

	//---  Check data type  ---
	//  support only _BI_RGB
	if (bih.biCompression != BI_RGB)  throw img_err("This BMP is not RGB format");

	//  support only 24-bit and 32-bit BMP
	if ( (bih.biBitCount != 24) && (bih.biBitCount != 32))
		throw  img_err("This BMP is not 24- or 32-bit");

	//---  Read pixel data  ---
	if ( fseek(fp, bfh.bfOffBits, SEEK_SET) != 0 )
		throw  img_err("Failed to jump to the start point of the pixel data");

	unsigned int RowLength = 4 * ((bih.biWidth * bih.biBitCount + 31) / 32);
	byte *buf = new byte[RowLength];
	alloc( bih.biWidth, bih.biHeight, 3 );
	
	//  24-bit or 32-bit
	unsigned int  byc = bih.biBitCount / 8;
	for (int y=0; y<height; y++) {
		if ( fread(buf, sizeof(byte), RowLength, fp) != RowLength ) {
			delete [] buf;
			free();
			throw  img_err("error in BMP load");
		}
		for (int x=0; x<width; x++) {
			setRGB( x,y, buf[x*byc+2], buf[x*byc+1], buf[x*byc] );
		}
	}
	delete [] buf;
	return true;
}


bool
image::saveBMP(FILE* fp)
{
	BITMAPFILEHEADER  bfh;
	BITMAPINFOHEADER  bih;
	unsigned int  BitCount;
	size_t count;
	BitCount = 24;	// support only 24-bit color
	unsigned int RowLength = 4 * ((width * BitCount + 31) / 32);

	//---  Set headers  ---
	bfh.bfType[0]	= 'B';
	bfh.bfType[1]	= 'M';
	bfh.bfSize		= RowLength * height + 14 + 40 + 2;
	bfh.bfReserved1	= 0;
	bfh.bfReserved2	= 0;
	bfh.bfOffBits	= 14 + 40;
	bih.biSize			= 40;
	bih.biWidth			= width;
	bih.biHeight		= height;
	bih.biPlanes		= 1;		//  always 1
	bih.biBitCount		= BitCount;
	bih.biCompression	= BI_RGB;
	bih.biSizeImage		= 0;
	bih.biXPelsPerMeter	= 0; //2834;
	bih.biYPelsPerMeter	= 0; //2834;
	bih.biClrUsed		= 0;
	bih.biClrImportant	= 0;

	//---  Write BMP file header  ---
	count  = fwrite( bfh.bfType, sizeof(BYTE), 2, fp);
	count += fwrite(&bfh.bfSize, sizeof(DWORD), 1, fp);
	count += fwrite(&bfh.bfReserved1, sizeof(WORD), 1, fp);
	count += fwrite(&bfh.bfReserved2, sizeof(WORD), 1, fp);
	count += fwrite(&bfh.bfOffBits, sizeof(DWORD), 1, fp);
	if (count != 6)  throw  img_err("Can't write BITMAPFILEHEADER");

	//---  Write BITMAPINFOHEADER  ---
	count  = fwrite( &bih.biSize,			sizeof(DWORD), 1, fp );
	count += fwrite( &bih.biWidth,			sizeof(long), 1, fp );
	count += fwrite( &bih.biHeight,			sizeof(long), 1, fp );
	count += fwrite( &bih.biPlanes,			sizeof(WORD), 1, fp );
	count += fwrite( &bih.biBitCount,		sizeof(WORD), 1, fp );
	count += fwrite( &bih.biCompression,	sizeof(DWORD), 1, fp );
	count += fwrite( &bih.biSizeImage,		sizeof(DWORD), 1, fp );
	count += fwrite( &bih.biXPelsPerMeter,	sizeof(long), 1, fp );
	count += fwrite( &bih.biYPelsPerMeter,	sizeof(long), 1, fp );
	count += fwrite( &bih.biClrUsed,		sizeof(DWORD), 1, fp );
	count += fwrite( &bih.biClrImportant,	sizeof(DWORD), 1, fp );
	if (count != 11)  throw  img_err("Can't write BITMAPINFOHEADER");

	//---  Write pixel data  ---
	byte *buf = new byte[RowLength];

	//  24-bit
	for (int y=0; y<height; y++) {
		for (int x=0; x<width; x++) {
			byte  R, G, B;
			getRGB(x,y,R,G,B);
			buf[x*3  ] = B;
			buf[x*3+1] = G;
			buf[x*3+2] = R;
		}
		if ( fwrite(buf, sizeof(byte), RowLength, fp) != RowLength ) {
			delete [] buf;
			free();
			throw  img_err("error in BMP save");
		}
	}
	delete [] buf;
	return  true;
}


//--------
//  JPEG
//--------
bool
image::loadJPEG(FILE* fp)
{
	struct  jpeg_decompress_struct  cinfo;
	struct  jpeg_error_mgr  jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, fp);				//  specify the source
	jpeg_read_header(&cinfo, TRUE);			//  read the header of the JPEG file
	jpeg_start_decompress(&cinfo);
	alloc( cinfo.output_width, cinfo.output_height, cinfo.num_components );
	JSAMPROW row_pointer[1];
	int row_span = cinfo.output_width * cinfo.num_components;
	while (cinfo.output_scanline < cinfo.output_height) {
		row_pointer[0] = pBits + (cinfo.output_height-1 -cinfo.output_scanline) * row_span;
		jpeg_read_scanlines(&cinfo, &row_pointer[0], 1);
	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	return  true;
}


bool
image::saveJPEG(FILE* fp)
{
	struct jpeg_compress_struct	cinfo;		//  JPEG compression object 
	struct jpeg_error_mgr		jerr;		//  JPEG error handler
	cinfo.err = jpeg_std_error(&jerr);		//  initialize the error handler
	jpeg_create_compress(&cinfo);			//  initialize the JPEG object
	jpeg_stdio_dest(&cinfo, fp);			//  specify the destination
	//  set parameters for compression
	cinfo.image_width  = width;
	cinfo.image_height = height;
	cinfo.input_components = 3;				//  number of color channels
	cinfo.in_color_space = JCS_RGB;			//  color space of source image
	jpeg_set_defaults(&cinfo);
	jpeg_start_compress(&cinfo, TRUE);		//  begin the compression cycle
	//  write all image data
	JSAMPROW row_pointer[1];
	int row_stride = width * cinfo.input_components;	
	while ( cinfo.next_scanline < cinfo.image_height ) {
		row_pointer[0] = pBits + (height-1-cinfo.next_scanline) * row_stride;
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}
	jpeg_finish_compress(&cinfo);			//  complete the compression cycle
	jpeg_destroy_compress(&cinfo);			//  release the JPEG compression object
	return  true;
}


//-------
//  PNG
//-------
const unsigned char  PNG_SIG_BYTES = 8;

bool
image::loadPNG(FILE* fp)
{
	png_structp	png_ptr;
	png_infop	info_ptr;
	unsigned char  sig[PNG_SIG_BYTES];

	//---  check signature  ---
	if ( fread(sig, 1, PNG_SIG_BYTES, fp) != PNG_SIG_BYTES )	throw  img_err("Can not read PNG_SIG_BYTE");
	if ( png_sig_cmp(sig, 0, PNG_SIG_BYTES))  throw  img_err("This is not a PNG file");
	//---  initialize pointers and I/O  ---
	if ( (png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)) == NULL)
		throw  img_err("Failed to cerate PNG read struct");
	if ( (info_ptr = png_create_info_struct(png_ptr)) == NULL )  {
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL );
		throw  img_err("Failed to cerate PNG info struct");
	}
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, PNG_SIG_BYTES);
	//---  read the info structure  ---
	png_uint_32	w, h;
	int			bit_depth, color_type, interlace_type, compression_type, filter_method;
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR( png_ptr, info_ptr, &w, &h, &bit_depth, &color_type,
			  &interlace_type, &compression_type, &filter_method );
	//---  check color type  ---
	//--indexed-- 
	if (color_type == PNG_COLOR_TYPE_PALETTE) {
		png_set_palette_to_rgb(png_ptr);
		color_type = PNG_COLOR_TYPE_RGB;
	}
	//--transparency--
	if ( png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
		png_set_tRNS_to_alpha(png_ptr);
	}
	//--bit depth--
	if (bit_depth != 8) {
		if (bit_depth < 8) {
			png_set_packing(png_ptr);
		} else {
			png_set_strip_16(png_ptr);
		}
	}
	//--gamma--
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_gAMA)) {
		double  gamma;
		png_get_gAMA(png_ptr, info_ptr, &gamma);
		png_set_gamma(png_ptr, 2.2, gamma);
	}  else {
		png_set_gamma(png_ptr, 2.2, 0.45455);
	}
	//--format--
	int chs;
	switch(color_type) {
		case PNG_COLOR_TYPE_GRAY:			chs = 1;	break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:		chs = 2;	break;
		case PNG_COLOR_TYPE_RGB:			chs = 3;	break;
		case PNG_COLOR_TYPE_RGB_ALPHA:		chs = 4;	break;
		case PNG_COLOR_TYPE_PALETTE:
		default:
			png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
			throw  img_err("PNG: unsupported color type");
	}

	//---  read pixels  ---
	alloc( (int)w, (int)h, chs );
	byte** ppBits = new byte*[height];
	for (int i=0; i<height; i++) {
		ppBits[i] = pBits + (height-1-i) * width * channels;
	}
	png_read_image(png_ptr, ppBits);
	delete [] ppBits;
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
	return  true;
}


bool
image::savePNG(FILE* fp)
{
	png_structp	png_ptr;
	png_infop	info_ptr;
	int i;
	//---  initialize pointers and I/O  ---
	if ( (png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)) == NULL)
		return  false;
	if ( (info_ptr = png_create_info_struct(png_ptr)) == NULL )  {
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		return  false;
	}
	png_init_io(png_ptr, fp);
	//---  set parameters  ---
	int  color_type;
	switch (channels) {
	case 1:		color_type = PNG_COLOR_TYPE_GRAY;			break;
	case 2:		color_type = PNG_COLOR_TYPE_GRAY_ALPHA;		break;
	case 3:		color_type = PNG_COLOR_TYPE_RGB;			break;
	case 4:		color_type = PNG_COLOR_TYPE_RGB_ALPHA;		break;
	default:
		png_destroy_write_struct(&png_ptr, &info_ptr);
		throw  img_err("PNG save: error in color type");
	}
	png_set_filter(png_ptr, 0, PNG_ALL_FILTERS);
	png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);
	png_set_IHDR( png_ptr, info_ptr, width, height, 8, color_type, PNG_INTERLACE_NONE,
				  PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	png_set_gAMA(png_ptr, info_ptr, 0.45455);
	//---  write header  ---
	png_write_info(png_ptr, info_ptr);
	//---  write image  ---
	byte** ppBits = new byte*[height];
	for (i=0; i<height; i++) {
		ppBits[i] = pBits + (height-1-i) * width * channels;
	}
	png_write_image(png_ptr, ppBits);
	png_write_end(png_ptr, info_ptr);
	delete [] ppBits;
	//---  clean up  ---
	png_destroy_write_struct(&png_ptr, &info_ptr);
	return  true;
}


//---------
//  TARGA
//---------
typedef struct {
	char	idlength;
	char	colourmaptype;
	char	datatypecode;
	short	colourmaporigin;
	short	colourmaplength;
	char	colourmapdepth;
	short	x_origin;
	short	y_origin;
	short	width;
	short	height;
	char	bitsperpixel;
	char	imagedescriptor;
} TGA_HEADER;

inline  void
set_TGA_pixel(byte* picTGA, byte* pImage, unsigned int readbytes)
{
	if (readbytes==2) {		// unpack 16-bit data to 24-bit
		unsigned short  pic = (picTGA[1]<<8) + picTGA[0];
		pImage[0] = ((pic>>10) & 0x1f) << 3;	// R
		pImage[1] = ((pic>>5) & 0x1f) << 3;		// G
		pImage[2] = (pic & 0x1f) << 3;			// B
	} else {
		pImage[0] = picTGA[2];	// R
		pImage[1] = picTGA[1];	// G
		pImage[2] = picTGA[0];	// B
	}
}


bool
image::loadTGA(FILE* fp)
{
	//---  read header  ---
	TGA_HEADER  hdr;
	size_t count=0;
	count  = fread( &hdr.idlength, 1, 1, fp );
	count += fread( &hdr.colourmaptype, 1, 1, fp );
	count += fread( &hdr.datatypecode, 1, 1, fp );
	if (count != 3)		return  false;

	// True-color image にのみ対応
	if (hdr.datatypecode != 2 && hdr.datatypecode != 10)	return false;
	// カラーマップのチェック
	if (hdr.colourmaptype != 0 && hdr.colourmaptype != 1)	return false;

	count  = fread( &hdr.colourmaporigin, 2, 1, fp );
	count += fread( &hdr.colourmaplength, 2, 1, fp );
	count += fread( &hdr.colourmapdepth, 1, 1, fp );
	count += fread( &hdr.x_origin, 2, 1, fp );
	count += fread( &hdr.y_origin, 2, 1, fp );
	count += fread( &hdr.width, 2, 1, fp );
	count += fread( &hdr.height, 2, 1, fp );
	count += fread( &hdr.bitsperpixel, 1, 1, fp );
	count += fread( &hdr.imagedescriptor, 1, 1, fp );
	if ( count != 9 )		return  false;

	//---  check data type  ---
	if ( (hdr.bitsperpixel != 16) && (hdr.bitsperpixel != 24)
		&& (hdr.bitsperpixel != 32) )
		return  false;
	fseek(fp, hdr.idlength + hdr.colourmaptype * hdr.colourmaplength, SEEK_CUR );
	//---  read image  ---
	alloc(hdr.width, hdr.height, 3);
	unsigned long  image_size = height * width;
	unsigned int readbytes = hdr.bitsperpixel / 8;
	unsigned long  n=0;
	byte*  ptr = pBits;
	byte   p[4];

	if (hdr.datatypecode == 2) {		//---  uncompressed RGB  ---
		while (n < image_size) {
			if ( fread( p, 1, readbytes, fp ) != readbytes )	throw  img_err("error in TGA load");
			set_TGA_pixel(p, ptr, readbytes);
			ptr += 3;
			n++;
		}
	} else if (hdr.datatypecode == 10) {	//---  RLE compressed RGB  ---
		while (n < image_size) {
			unsigned char  rep;
			if ( fread( &rep, 1, 1, fp ) != 1 )	 throw  img_err("error in TGA load");

			if (rep & 0x80) {	// Run-length Packets
				rep &= 0x7f;
				if ( fread( &p, 1, readbytes, fp ) != readbytes )  throw  img_err("error in TGA load");
				for (int i=0; i<rep+1; i++) {
					set_TGA_pixel(p, ptr, readbytes);
					ptr+=3;
					n++;
				}
			} else {			// Raw Packets
				for (int i=0; i<rep+1; i++) {
					if ( fread( &p, 1, readbytes, fp ) != readbytes )  throw  img_err("error in TGA load");
					set_TGA_pixel(p, ptr, readbytes);
					ptr+=3;
					n++;
				}
			}
		}
	}

	return  true;
}


bool
image::saveTGA(FILE* fp)
{
	//---  write header  ---
	TGA_HEADER  hdr;
	hdr.idlength = 0;
	hdr.colourmaptype = 0;
	hdr.datatypecode = 2;
	hdr.colourmaporigin = 0;
	hdr.colourmaplength = 0;
	hdr.colourmapdepth = 0;
	hdr.x_origin = 0;
	hdr.y_origin = 0;
	hdr.width = width;
	hdr.height = height;
	hdr.bitsperpixel = 24;
	hdr.imagedescriptor = 0;

	size_t count=0;
	count  = fwrite( &hdr.idlength, 1, 1, fp );
	count += fwrite( &hdr.colourmaptype, 1, 1, fp );
	count += fwrite( &hdr.datatypecode, 1, 1, fp );
	count += fwrite( &hdr.colourmaporigin, 2, 1, fp );
	count += fwrite( &hdr.colourmaplength, 2, 1, fp );
	count += fwrite( &hdr.colourmapdepth, 1, 1, fp );
	count += fwrite( &hdr.x_origin, 2, 1, fp );
	count += fwrite( &hdr.y_origin, 2, 1, fp );
	count += fwrite( &hdr.width, 2, 1, fp );
	count += fwrite( &hdr.height, 2, 1, fp );
	count += fwrite( &hdr.bitsperpixel, 1, 1, fp );
	count += fwrite( &hdr.imagedescriptor, 1, 1, fp );
	if ( count != 12 )  throw  ("error in TGA save");

	//---  write image  ---
	unsigned long  image_size = height * width;
	byte*  ptr = pBits;
	for (unsigned int i=0; i<image_size; i++) {
		byte  p[3];
		p[2] = *(ptr++);	// R
		p[1] = *(ptr++);	// G
		p[0] = *(ptr++);	// B
		if ( fwrite( p, 1, 3, fp) != 3)		throw("error in TGA save");
	}
	return  true;
}


bool
image::loadBMP(const char* filename)
{
	FILE* fp;
	bool  res;

	if ( (fp=fopen(filename,"rb")) == NULL )	return  false;
	res = loadBMP(fp);
	fclose(fp);

	return  res;
}


bool
image::saveBMP(const char* filename)
{
	FILE* fp;
	bool  res;

	if (pBits==0)  return false;		// empty

	if ( (fp=fopen(filename,"wb")) == NULL ) 	return  false;
	res = saveBMP(fp);
	fclose(fp);

	return  res;
}


bool
image::loadJPEG(const char* filename)
{
	FILE* fp;
	bool  res;

	if ( (fp=fopen(filename,"rb")) == NULL )	return  false;
	res = loadJPEG(fp);
	fclose(fp);

	return  res;
}


bool
image::saveJPEG(const char* filename)
{
	FILE* fp;
	bool  res;

	if (pBits==0)  return false;		// empty

	if ( (fp=fopen(filename,"wb")) == NULL ) 	return  false;
	res = saveJPEG(fp);
	fclose(fp);

	return  res;
}


bool
image::loadPNG(const char* filename)
{
	FILE* fp;
	bool  res;

	if ( (fp=fopen(filename,"rb")) == NULL )	return  false;
	res = loadPNG(fp);
	fclose(fp);

	return  res;
}


bool
image::savePNG(const char* filename)
{
	FILE* fp;
	bool  res;

	if (pBits==0)  return false;		// empty

	if ( (fp=fopen(filename,"wb")) == NULL ) 	return  false;
	res = savePNG(fp);
	fclose(fp);

	return  res;
}


bool
image::loadTGA(const char* filename)
{
	FILE* fp;
	bool  res;

	if ( (fp=fopen(filename,"rb")) == NULL )	return  false;
	res = loadTGA(fp);
	fclose(fp);

	return  res;
}


bool
image::saveTGA(const char* filename)
{
	FILE* fp;
	bool  res;

	if (pBits==0)  return false;		// empty

	if ( (fp=fopen(filename,"wb")) == NULL ) 	return  false;
	res = saveTGA(fp);
	fclose(fp);

	return  res;
}


bool
image::save(const char* filepath)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath_s( filepath, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
	if ( stricmp(ext,".bmp")==0)  return saveBMP(filepath);
	if ( stricmp(ext,".jpg")==0)  return saveJPEG(filepath);
	if ( stricmp(ext,".png")==0)  return savePNG(filepath);
	if ( stricmp(ext,".tga")==0)  return saveTGA(filepath);

	return  false;
}


bool
image::load(const char* filepath)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath_s( filepath, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
	if ( stricmp(ext,".bmp")==0)  return loadBMP(filepath);
	if ( stricmp(ext,".jpg")==0)  return loadJPEG(filepath);
	if ( stricmp(ext,".png")==0)  return loadPNG(filepath);
	if ( stricmp(ext,".tga")==0)  return loadTGA(filepath);

	return  false;
}







bool
image::loadBMP(const wchar_t* filename)
{
	FILE* fp;
	bool  res;

	if ((fp = _wfopen(filename, L"rb")) == NULL)	return  false;
	res = loadBMP(fp);
	fclose(fp);

	return  res;
}


bool
image::saveBMP(const wchar_t* filename)
{
	FILE* fp;
	bool  res;

	if (pBits == 0)  return false;		// empty

	if ((fp = _wfopen(filename, L"wb")) == NULL) 	return  false;
	res = saveBMP(fp);
	fclose(fp);

	return  res;
}


bool
image::loadJPEG(const wchar_t* filename)
{
	FILE* fp;
	bool  res;

	if ((fp = _wfopen(filename, L"rb")) == NULL)	return  false;
	res = loadJPEG(fp);
	fclose(fp);

	return  res;
}


bool
image::saveJPEG(const wchar_t* filename)
{
	FILE* fp;
	bool  res;

	if (pBits == 0)  return false;		// empty

	if ((fp = _wfopen(filename, L"wb")) == NULL) 	return  false;
	res = saveJPEG(fp);
	fclose(fp);

	return  res;
}


bool
image::loadPNG(const wchar_t* filename)
{
	FILE* fp;
	bool  res;

	if ((fp = _wfopen(filename, L"rb")) == NULL)	return  false;
	res = loadPNG(fp);
	fclose(fp);

	return  res;
}


bool
image::savePNG(const wchar_t* filename)
{
	FILE* fp;
	bool  res;

	if (pBits == 0)  return false;		// empty

	if ((fp = _wfopen(filename, L"wb")) == NULL) 	return  false;
	res = savePNG(fp);
	fclose(fp);

	return  res;
}


bool
image::loadTGA(const wchar_t* filename)
{
	FILE* fp;
	bool  res;

	if ((fp = _wfopen(filename, L"rb")) == NULL)	return  false;
	res = loadTGA(fp);
	fclose(fp);

	return  res;
}


bool
image::saveTGA(const wchar_t* filename)
{
	FILE* fp;
	bool  res;

	if (pBits == 0)  return false;		// empty

	if ((fp = _wfopen(filename, L"wb")) == NULL) 	return  false;
	res = saveTGA(fp);
	fclose(fp);

	return  res;
}


bool
image::save(const wchar_t* filepath)
{
	wchar_t drive[_MAX_DRIVE];
	wchar_t dir[_MAX_DIR];
	wchar_t fname[_MAX_FNAME];
	wchar_t ext[_MAX_EXT];

	_wsplitpath_s(filepath, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
	if (wcsicmp(ext, L".bmp") == 0)  return saveBMP(filepath);
	if (wcsicmp(ext, L".jpg") == 0)  return saveJPEG(filepath);
	if (wcsicmp(ext, L".png") == 0)  return savePNG(filepath);
	if (wcsicmp(ext, L".tga") == 0)  return saveTGA(filepath);

	return  false;
}


bool
image::load(const wchar_t* filepath)
{
	wchar_t drive[_MAX_DRIVE];
	wchar_t dir[_MAX_DIR];
	wchar_t fname[_MAX_FNAME];
	wchar_t ext[_MAX_EXT];

	_wsplitpath_s(filepath, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
	if (wcsicmp(ext, L".bmp") == 0)  return loadBMP(filepath);
	if (wcsicmp(ext, L".jpg") == 0)  return loadJPEG(filepath);
	if (wcsicmp(ext, L".png") == 0)  return loadPNG(filepath);
	if (wcsicmp(ext, L".tga") == 0)  return loadTGA(filepath);

	return  false;
}











//---------
//  pixel
//---------
inline  float
image::pixel_ave_xloop(const float&x1, const float& x2, const int& iy, const int& ch)  const
{
	int ix1, ix2;
	float  wx;

	ix1 = int(x1);
	ix2 = int(x2);
	if (ix1 == ix2) {
		wx = x2-x1;
		return  wx * getPixelf(ix1, iy, ch);
	}

	float  sum=0.0;
	wx = float(ix1+1) - x1;
	sum = wx * getPixelf(ix1, iy, ch);
	for (int ix=ix1+1; ix<ix2; ix++) {
		sum += getPixelf(ix, iy, ch);
	}
	wx = x2 - float(ix2);
	sum += wx * getPixelf(ix2, iy, ch);
	return  sum;
}


float
image::getPixelf_ave(float x1, float y1, float x2, float y2, int ch) const
{
	unsigned int  iy1, iy2;
	float fw, fh, wy, S;

	fw = float(width);
	fh = float(height);
	if (x1<0.0) x1=0.0;
	if (y1<0.0) y1=0.0;
	if (x2>fw) x2=fw;
	if (y2>fh) y2=fh;

	S = (x2-x1)*(y2-y1);

	iy1 = unsigned int(y1);
	iy2 = unsigned int(y2);

	if (iy1 == iy2) {
		wy = y2-y1;
		return  wy * pixel_ave_xloop(x1, x2, iy1, ch) / S;
	}

	float  sum = 0.0;
	wy = float(iy1+1) - y1;
	sum = wy * pixel_ave_xloop(x1, x2, iy1, ch);
	for (unsigned int iy=iy1+1; iy<iy2; iy++) {
		sum += pixel_ave_xloop(x1, x2, iy, ch);
	}
	wy = y2 - float(iy2);
	sum += wy * pixel_ave_xloop(x1, x2, iy2, ch);
	return  sum / S;
}


//-----------
//  scaling
//-----------
void
image::scale_image(const image& src_img, float src_x1, float src_y1, float src_x2, float src_y2)
{
	float x, y, dx, dy;
	dx = (src_x2 - src_x1) / width * 0.9999f;		/// !!!
	dy = (src_y2 - src_y1) / height * 0.9999f;
	int ch_max = (channels <= src_img.get_channels()) ? channels : src_img.get_channels();
	
	for (int iy=0; iy<height; iy++) {
		y = src_y1 + dy * iy;
		for (int ix=0; ix<width; ix++) {
			x = src_x1 + dx * ix;
			for (int ch=0; ch<ch_max; ch++) {
                float val = src_img.getPixelf_ave(x, y, x+dx, y+dy, ch);
				setPixelf(ix,iy,ch,val);
			}
		}
	}
}
