/**************************************************************************************************

Copyright (c) 2007   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#include  <windows.h>
#include  <gl/gl.h>
#include  <gl/wglext.h>
#include  "../common/win_dlg.h"


//
bool bIniEnablePBuffer = false;
static bool bEnablePBuffer = false;
static int pbuffer_max_width  = 0;
static int pbuffer_max_height = 0;
static int pbuffer_pixelformat = 0;

//
PFNWGLGETEXTENSIONSSTRINGARBPROC  wglGetExtensionsStringARB = NULL;

//--  WGL_ARB_pbuffer
PFNWGLCREATEPBUFFERARBPROC		wglCreatePbufferARB		= NULL;
PFNWGLGETPBUFFERDCARBPROC		wglGetPbufferDCARB		= NULL;
PFNWGLRELEASEPBUFFERDCARBPROC	wglReleasePbufferDCARB	= NULL;
PFNWGLDESTROYPBUFFERARBPROC		wglDestroyPbufferARB	= NULL;
PFNWGLQUERYPBUFFERARBPROC		wglQueryPbufferARB		= NULL;
//--  WGL_ARB_pixel_format
PFNWGLGETPIXELFORMATATTRIBIVARBPROC	wglGetPixelFormatAttribivARB = NULL;
PFNWGLGETPIXELFORMATATTRIBFVARBPROC	wglGetPixelFormatAttribfvARB = NULL;
PFNWGLCHOOSEPIXELFORMATARBPROC		wglChoosePixelFormatARB		 = NULL;




bool
isPBufferEnable()
{
	return  bEnablePBuffer;
}



bool
init_pbuffer_sub()
{
	HDC hDC = wglGetCurrentDC();

	//------------------
	//  wgl extensions
	//------------------
	wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
	char *ext = NULL;

	if ( wglGetExtensionsStringARB ) {
		ext = (char*)wglGetExtensionsStringARB( hDC );
	} else {
		return  false;
	}

	//-------------------
	//  WGL_ARB_pbuffer
	//-------------------
	if (strstr( ext, "WGL_ARB_pbuffer") == NULL) {
		return false;
	}
	wglCreatePbufferARB		= (PFNWGLCREATEPBUFFERARBPROC)wglGetProcAddress("wglCreatePbufferARB");
	wglGetPbufferDCARB		= (PFNWGLGETPBUFFERDCARBPROC)wglGetProcAddress("wglGetPbufferDCARB");
	wglReleasePbufferDCARB	= (PFNWGLRELEASEPBUFFERDCARBPROC)wglGetProcAddress("wglReleasePbufferDCARB");
	wglDestroyPbufferARB	= (PFNWGLDESTROYPBUFFERARBPROC)wglGetProcAddress("wglDestroyPbufferARB");
	wglQueryPbufferARB		= (PFNWGLQUERYPBUFFERARBPROC)wglGetProcAddress("wglQueryPbufferARB");

	if ( !wglCreatePbufferARB || !wglGetPbufferDCARB || !wglGetPbufferDCARB ||
		 !wglDestroyPbufferARB || !wglQueryPbufferARB )
	{
		return false;
	}


	//------------------------
	//  WGL_ARB_pixel_format
	//------------------------
	if (strstr( ext, "WGL_ARB_pixel_format" ) == NULL) {
		return false;
	}

	wglGetPixelFormatAttribivARB	= (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)wglGetProcAddress("wglGetPixelFormatAttribivARB");
	wglGetPixelFormatAttribfvARB	= (PFNWGLGETPIXELFORMATATTRIBFVARBPROC)wglGetProcAddress("wglGetPixelFormatAttribfvARB");
	wglChoosePixelFormatARB			= (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");

	if ( !wglGetPixelFormatAttribivARB || !wglGetPixelFormatAttribfvARB || !wglChoosePixelFormatARB ) {
		return false;
	}


	//----------------
	//  Pixel foemat
	//----------------
	int pf_attr[] = {
		WGL_SUPPORT_OPENGL_ARB,		TRUE,
		WGL_DRAW_TO_PBUFFER_ARB,	TRUE,
		WGL_RED_BITS_ARB,			8,
		WGL_GREEN_BITS_ARB,			8,
		WGL_BLUE_BITS_ARB,			8,
		WGL_ALPHA_BITS_ARB,			8,
		WGL_DEPTH_BITS_ARB,			16,
		WGL_DOUBLE_BUFFER_ARB,		FALSE,
		0
	};

	unsigned int count = 0;
	wglChoosePixelFormatARB( hDC, (const int*)pf_attr, NULL, 1, &pbuffer_pixelformat, &count );
	if ( count == 0 ) {
		return false;
	}


	//----------------
	//  Maximun size
	//----------------
	const int iattr[] = {
		WGL_MAX_PBUFFER_WIDTH_ARB,
		WGL_MAX_PBUFFER_HEIGHT_ARB,
		WGL_MAX_PBUFFER_PIXELS_ARB
	};
	int ival[3];
	wglGetPixelFormatAttribivARB(hDC, pbuffer_pixelformat, 0, 3, iattr, ival);
	pbuffer_max_width  = ival[0];
	pbuffer_max_height = ival[1];



	bEnablePBuffer = true;

	return  true;
}


bool
init_pbuffer(HWND hWnd)
{
	bool res;

	HDC hDC = GetDC( hWnd );
	HGLRC hRC = wglCreateContext( hDC );
	wglMakeCurrent( hDC, hRC );

	res = init_pbuffer_sub();

	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( hRC );
	ReleaseDC (hWnd, hDC);

	return  res;
}


int
get_pbuffer_width_max()
{
	return  pbuffer_max_width;
}

int
get_pbuffer_height_max()
{
	return  pbuffer_max_height;
}




struct PBUFFER {
	HPBUFFERARB	hPBuffer;
	HDC			hDC;
	HGLRC		hRC;
	int			w, h;
};

static PBUFFER pbf;



bool
create_pbuffer(int w, int h)
{
	if ( !isPBufferEnable() )  return false;

	pbf.hPBuffer	= NULL;
	pbf.w			= w;
	pbf.h			= h;
	if (pbf.w > pbuffer_max_width)   pbf.w = pbuffer_max_width;
	if (pbf.h > pbuffer_max_height)  pbf.h = pbuffer_max_height;

	HDC hDC = wglGetCurrentDC();

	//--  Create pBuffer
	pbf.hPBuffer	= wglCreatePbufferARB( hDC, pbuffer_pixelformat, pbf.w, pbf.h, NULL);
	if ( !pbf.hPBuffer ) {
		return false;
	}
	pbf.hDC			= wglGetPbufferDCARB( pbf.hPBuffer );
	pbf.hRC			= wglCreateContext( pbf.hDC );


	// 実際に確保された大きさ
	wglQueryPbufferARB(pbf.hPBuffer, WGL_PBUFFER_WIDTH_ARB,  &pbf.w);
    wglQueryPbufferARB(pbf.hPBuffer, WGL_PBUFFER_HEIGHT_ARB, &pbf.h);

	//--  Share lists  --
	HGLRC  hRC = wglGetCurrentContext();
	if ( !wglShareLists( hRC, pbf.hRC ) ) {
		return false;
	}

	//--  Make current  --
	if ( !wglMakeCurrent( pbf.hDC, pbf.hRC ) ) {
		return false;
	}

	// init GL
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT,  GL_NICEST);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);

	//  Lights
	glEnable(GL_LIGHT0);
	GLfloat ambient[]  = {0.0, 0.0, 0.0, 1.0 };
	GLfloat diffuse[]  = {1.0, 1.0, 1.0, 1.0 };
	GLfloat specular[] = {0.5, 0.5, 0.5, 1.0 };
	glLightfv( GL_LIGHT0, GL_AMBIENT, ambient );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );
	glLightfv( GL_LIGHT0, GL_SPECULAR, specular );
	glShadeModel(GL_SMOOTH);
	GLfloat ambient1[]  = {0.5, 0.5, 0.5, 1.0 };
	GLfloat diffuse1[]  = {0.5, 0.5, 0.5, 1.0 };
	GLfloat position1[] = {0.0, 0.0, 1.0, 0.0 };
	glLightfv( GL_LIGHT1, GL_AMBIENT,  ambient1 );
	glLightfv( GL_LIGHT1, GL_DIFFUSE,  diffuse1 );
	glLightfv( GL_LIGHT1, GL_POSITION, position1 );
	glDisable(GL_LIGHT1);
	GLfloat ambient2[]  = {0.55, 0.55, 0.55, 1.0 };
	GLfloat diffuse2[]  = {0.99, 0.99, 0.99, 1.0 };
	GLfloat position2[] = {0.0, 0.0, 0.0, 1.0 };
	glLightfv( GL_LIGHT2, GL_AMBIENT,  ambient2 );
	glLightfv( GL_LIGHT2, GL_DIFFUSE,  diffuse2 );
	glLightfv( GL_LIGHT2, GL_POSITION, position2 );
	glDisable(GL_LIGHT2);

	return true;
}


void
destroy_pbuffer()
{
	if ( !isPBufferEnable() )  return;

	if ( pbf.hRC != NULL ) {
		wglMakeCurrent( pbf.hDC, pbf.hRC );
		wglDeleteContext( pbf.hRC );
		wglReleasePbufferDCARB( pbf.hPBuffer, pbf.hDC );
		wglDestroyPbufferARB( pbf.hPBuffer );
		pbf.hRC = NULL;
		pbf.hDC = NULL;
		pbf.hPBuffer = NULL;
	}
}
