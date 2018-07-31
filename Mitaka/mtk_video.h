/**************************************************************************************************

Copyright (c) 2014   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#ifndef __MTK_VIDEO_H__
#define __MTK_VIDEO_H__
#include  <Windows.h>
#include  <dshow.h>
#include  <d3d9.h>
#include  <vmr9.h>
#include  <evr.h>



enum {
	RENDERER_WINDOWED_VMR7,
	RENDERER_WINDOWLESS_VMR7,
	RENDERER_WINDOWLESS_VMR9,
	RENDERER_EVR
};





//------------------
//  Abstract class
//------------------

class  mtk_video
{
public:
	virtual ~mtk_video() {};

	virtual HRESULT Init(HWND hwnd, IGraphBuilder *pGraph) = 0;
	virtual HRESULT SetViewWindow(HWND hwnd, int x, int y, int w, int h, bool bFullScreen) = 0;

	virtual	HRESULT SetWindowPosition(int x, int y, int w, int h) = 0;
	virtual HRESULT GetVideoSize(long *pw, long *ph) = 0;

	virtual HRESULT OnPaint(HWND hwnd, HDC hDC) = 0;
	virtual HRESULT OnDisplayChange() = 0;
	virtual HRESULT OnSize(const LPRECT pRect) = 0;
};



//----------------------
//  Window mode (VMR7)
//----------------------

class  mtk_video_window_mode : public mtk_video
{
	IVideoWindow*   m_pVW;
	IBasicVideo*	m_pBV;

public:
	mtk_video_window_mode();
	~mtk_video_window_mode();

	HRESULT Init(HWND hwnd, IGraphBuilder *pGraph);
	HRESULT SetViewWindow(HWND hwnd, int x, int y, int w, int h, bool bFullScreen);

	HRESULT	SetWindowPosition(int x, int y, int w, int h);
	HRESULT GetVideoSize(long *pw, long *ph);

	HRESULT OnPaint(HWND hwnd, HDC hDC);
	HRESULT OnDisplayChange();
	HRESULT OnSize(const LPRECT pRect);
};



//-------------------
//  Windowless VMR7
//-------------------

class  mtk_video_windowless_VMR7 : public mtk_video
{
	IVMRWindowlessControl *m_pWC;

public:
	mtk_video_windowless_VMR7();
	~mtk_video_windowless_VMR7();

	HRESULT Init(HWND hwnd, IGraphBuilder *pGraph);
	HRESULT SetViewWindow(HWND hwnd, int x, int y, int w, int h, bool bFullScreen);

	HRESULT	SetWindowPosition(int x, int y, int w, int h);
	HRESULT GetVideoSize(long *pw, long *ph);

	HRESULT OnPaint(HWND hwnd, HDC hDC);
	HRESULT OnDisplayChange();
	HRESULT OnSize(const LPRECT pRect);
};



//-------------------
//  Windowless VMR9
//-------------------

class  mtk_video_windowless_VMR9 : public mtk_video
{
	IVMRWindowlessControl9 *m_pWC;

public:
	mtk_video_windowless_VMR9();
	~mtk_video_windowless_VMR9();

	HRESULT Init(HWND hwnd, IGraphBuilder *pGraph);
	HRESULT SetViewWindow(HWND hwnd, int x, int y, int w, int h, bool bFullScreen);

	HRESULT	SetWindowPosition(int x, int y, int w, int h);
	HRESULT GetVideoSize(long *pw, long *ph);

	HRESULT OnPaint(HWND hwnd, HDC hDC);
	HRESULT OnDisplayChange();
	HRESULT OnSize(const LPRECT pRect);
};



//-------
//  EVR
//-------

class  mtk_video_EVR : public mtk_video
{
	IBaseFilter *m_pEVR;
	IMFVideoDisplayControl *m_pVD;

public:
	mtk_video_EVR();
	~mtk_video_EVR();

	HRESULT Init(HWND hwnd, IGraphBuilder *pGraph);
	HRESULT SetViewWindow(HWND hwnd, int x, int y, int w, int h, bool bFullScreen);

	HRESULT	SetWindowPosition(int x, int y, int w, int h);
	HRESULT GetVideoSize(long *pw, long *ph);

	HRESULT OnPaint(HWND hwnd, HDC hDC);
	HRESULT OnDisplayChange();
	HRESULT OnSize(const LPRECT pRect);
};



#endif
