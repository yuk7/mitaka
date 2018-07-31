/**************************************************************************************************

Copyright (c) 2014-2015   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#include  "mtk_video.h"



#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }


 

//----------------------
//  Window mode (VMR7)
//----------------------

mtk_video_window_mode::mtk_video_window_mode() : m_pVW(NULL), m_pBV(NULL)
{
}


mtk_video_window_mode::~mtk_video_window_mode()
{
	if (m_pVW) {
		m_pVW->put_Visible(OAFALSE);
		m_pVW->put_Owner(NULL);
	}

	SAFE_RELEASE(m_pVW);
	SAFE_RELEASE(m_pBV);
}



HRESULT mtk_video_window_mode::Init(HWND hwnd, IGraphBuilder *pGraph)
{
	pGraph->QueryInterface(IID_IVideoWindow, (void**)&m_pVW);
	pGraph->QueryInterface(IID_IBasicVideo, (void**)&m_pBV);
	
	return  S_OK;
}


HRESULT mtk_video_window_mode::SetViewWindow(HWND hwnd, int x, int y, int w, int h, bool bFullScreen)
{
	HRESULT hr;



	hr = m_pVW->put_AutoShow(OAFALSE);


	hr = m_pVW->put_Owner((OAHWND)hwnd);
	if (FAILED(hr)) {
		return hr;
	}


	hr = m_pVW->HideCursor(OATRUE);
	if (FAILED(hr)) {
		return hr;
	}


	hr = m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
	if (FAILED(hr)) {
		return hr;
	}
	
	
	hr = m_pVW->SetWindowPosition(x, y, w, h);
	if (FAILED(hr)) {
		return hr;
	}


	hr = m_pVW->put_Visible(OATRUE);
	if (FAILED(hr)) {
		return hr;
	}
	

	if (bFullScreen) {
		hr = m_pVW->put_FullScreenMode(OATRUE);
		
		if (FAILED(hr)) {
			return hr;
		}
	}


	return hr;
}



HRESULT mtk_video_window_mode::SetWindowPosition(int x, int y, int w, int h)
{
	return  m_pVW->SetWindowPosition(x, y, w, h);
}


HRESULT mtk_video_window_mode::GetVideoSize(long *pw, long *ph)
{
	if (m_pBV == NULL) {
		return E_NOINTERFACE;
	}

	return  m_pBV->GetVideoSize(pw, ph);
}


HRESULT mtk_video_window_mode::OnPaint(HWND hwnd, HDC hDC)
{
	// NO-OP
	return S_OK;
}


HRESULT mtk_video_window_mode::OnDisplayChange()
{
	// NO-OP
	return S_OK;
}


HRESULT mtk_video_window_mode::OnSize(const LPRECT pRect)
{
	// NO-OP
	return S_OK;
}





//-------------------
//  Windowless VMR7
//-------------------

mtk_video_windowless_VMR7::mtk_video_windowless_VMR7() : m_pWC(NULL)
{
}


mtk_video_windowless_VMR7::~mtk_video_windowless_VMR7()
{
	SAFE_RELEASE(m_pWC);
}



HRESULT mtk_video_windowless_VMR7::Init(HWND hwnd, IGraphBuilder *pGraph)
{
	IBaseFilter *pVMR = NULL;
	IVMRFilterConfig* pConfig = NULL;
	IVMRWindowlessControl *pWC = NULL;
	HRESULT hr;


	//--  Create VMR
	hr = CoCreateInstance(CLSID_VideoMixingRenderer, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void**)&pVMR);
	if (FAILED(hr))  { goto done; }

	//--  Add VMR to graph
	hr = pGraph->AddFilter(pVMR, L"Video Mixing Renderer");
	if (FAILED(hr))  { goto done; }

	//--  Set rendering mode
	hr = pVMR->QueryInterface(IID_IVMRFilterConfig, (void**)&pConfig);
	if (FAILED(hr))  { goto done; }

	hr = pConfig->SetRenderingMode(VMRMode_Windowless);
	if (FAILED(hr))  { goto done; }

	//--  Set clipping window
	hr = pVMR->QueryInterface(IID_IVMRWindowlessControl, (void**)&pWC);
	if (FAILED(hr))  { goto done; }

	hr = pWC->SetVideoClippingWindow(hwnd);
	if (FAILED(hr))  { goto done; }

	m_pWC = pWC;
	m_pWC->AddRef();


done:
	SAFE_RELEASE(pConfig);
	SAFE_RELEASE(pWC);
	SAFE_RELEASE(pVMR);
	return hr;
}


HRESULT mtk_video_windowless_VMR7::SetViewWindow(HWND hwnd, int x, int y, int w, int h, bool bFullScreen)
{
	HRESULT hr;

	RECT rect, rect2;

	GetClientRect(hwnd, &rect2);

	rect.left = x;
	rect.right = x + w;
	rect.bottom = y + h;
	rect.top = y;

	hr = m_pWC->SetVideoPosition(NULL, &rect);
	if (FAILED(hr)) {
		return hr;
	}

	//
	InvalidateRect(hwnd, NULL, false);

	return hr;
}



HRESULT mtk_video_windowless_VMR7::SetWindowPosition(int x, int y, int w, int h)
{
	RECT rect;

	if (m_pWC == NULL) {
		return S_OK;
	}

	rect.left = x;
	rect.right = x + w;
	rect.bottom = y + h;
	rect.top = y;

	return  m_pWC->SetVideoPosition(NULL, &rect);
}


HRESULT mtk_video_windowless_VMR7::GetVideoSize(long *pw, long *ph)
{
	if (m_pWC == NULL) {
		return E_NOINTERFACE;
	}

	return  m_pWC->GetNativeVideoSize(pw, ph, NULL, NULL);
}


HRESULT mtk_video_windowless_VMR7::OnPaint(HWND hwnd, HDC hDC)
{
	if (m_pWC == NULL) {
		return S_OK;
	}

	return m_pWC->RepaintVideo(hwnd, hDC);
}


HRESULT mtk_video_windowless_VMR7::OnDisplayChange()
{
	if (m_pWC == NULL) {
		return S_OK;
	}

	return m_pWC->DisplayModeChanged();
}


HRESULT mtk_video_windowless_VMR7::OnSize(const LPRECT pRect)
{
	if (m_pWC == NULL) {
		return S_OK;
	}

	return  m_pWC->SetVideoPosition(NULL, pRect);
}





//-------------------
//  Windowless VMR9
//-------------------

mtk_video_windowless_VMR9::mtk_video_windowless_VMR9() : m_pWC(NULL)
{
}


mtk_video_windowless_VMR9::~mtk_video_windowless_VMR9()
{
	SAFE_RELEASE(m_pWC);
}



HRESULT mtk_video_windowless_VMR9::Init(HWND hwnd, IGraphBuilder *pGraph)
{
	IBaseFilter *pVMR = NULL;
	IVMRFilterConfig9* pConfig = NULL;
	IVMRWindowlessControl9 *pWC = NULL;
	HRESULT hr;


	//--  Create VMR
	hr = CoCreateInstance(CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void**)&pVMR);
	if (FAILED(hr))  { goto done; }

	//--  Add VMR to graph
	hr = pGraph->AddFilter(pVMR, L"VMR-9");
	if (FAILED(hr))  { goto done; }

	//--  Set rendering mode
	hr = pVMR->QueryInterface(IID_IVMRFilterConfig9, (void**)&pConfig);
	if (FAILED(hr))  { goto done; }

	hr = pConfig->SetRenderingMode(VMR9Mode_Windowless);
	if (FAILED(hr))  { goto done; }

	//--  Set clipping window
	hr = pVMR->QueryInterface(IID_IVMRWindowlessControl9, (void**)&pWC);
	if (FAILED(hr))  { goto done; }

	hr = pWC->SetVideoClippingWindow(hwnd);
	if (FAILED(hr))  { goto done; }

	m_pWC = pWC;
	m_pWC->AddRef();


done:
	SAFE_RELEASE(pConfig);
	SAFE_RELEASE(pWC);
	SAFE_RELEASE(pVMR);
	return hr;
}


HRESULT mtk_video_windowless_VMR9::SetViewWindow(HWND hwnd, int x, int y, int w, int h, bool bFullScreen)
{
	HRESULT hr;

	RECT rect, rect2;

	GetClientRect(hwnd, &rect2);

	rect.left = x;
	rect.right = x + w;
	rect.bottom = y + h;
	rect.top = y;

	hr = m_pWC->SetVideoPosition(NULL, &rect);
	if (FAILED(hr)) {
		return hr;
	}

	//
	InvalidateRect(hwnd, NULL, false);

	return hr;
}



HRESULT mtk_video_windowless_VMR9::SetWindowPosition(int x, int y, int w, int h)
{
	RECT rect;

	if (m_pWC == NULL) {
		return S_OK;
	}

	rect.left = x;
	rect.right = x + w;
	rect.bottom = y + h;
	rect.top = y;

	return  m_pWC->SetVideoPosition(NULL, &rect);
}


HRESULT mtk_video_windowless_VMR9::GetVideoSize(long *pw, long *ph)
{
	if (m_pWC == NULL) {
		return E_NOINTERFACE;
	}

	return  m_pWC->GetNativeVideoSize(pw, ph, NULL, NULL);
}


HRESULT mtk_video_windowless_VMR9::OnPaint(HWND hwnd, HDC hDC)
{
	if (m_pWC == NULL) {
		return S_OK;
	}

	return m_pWC->RepaintVideo(hwnd, hDC);
}


HRESULT mtk_video_windowless_VMR9::OnDisplayChange()
{
	if (m_pWC == NULL) {
		return S_OK;
	}

	return m_pWC->DisplayModeChanged();
}


HRESULT mtk_video_windowless_VMR9::OnSize(const LPRECT pRect)
{
	if (m_pWC == NULL) {
		return S_OK;
	}

	return  m_pWC->SetVideoPosition(NULL, pRect);
}





//-------
//  EVR
//-------

mtk_video_EVR::mtk_video_EVR() : m_pEVR(NULL), m_pVD(NULL)
{
}


mtk_video_EVR::~mtk_video_EVR()
{
	if (!m_pEVR) {
		BOOL b;
		m_pVD->GetFullscreen(&b);
		if (b) {
			m_pVD->SetFullscreen(FALSE);
		}
	}
	
	SAFE_RELEASE(m_pEVR);
	SAFE_RELEASE(m_pVD);
}



HRESULT mtk_video_EVR::Init(HWND hwnd, IGraphBuilder *pGraph)
{
	IBaseFilter *pEVR = NULL;

	IMFGetService *pGS = NULL;
	IMFVideoDisplayControl *pDisplay = NULL;

	HRESULT hr;


	//--  Create EVR
	hr = CoCreateInstance(CLSID_EnhancedVideoRenderer, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void**)&pEVR);
	if (FAILED(hr))  { goto done; }

	//--  Add EVR to graph
	hr = pGraph->AddFilter(pEVR, L"EVR");
	if (FAILED(hr))  { goto done; }

	//--  Set rendering mode
	hr = pEVR->QueryInterface(IID_IMFGetService, (void**)&pGS);
	if (FAILED(hr))  { goto done; }

	hr = pGS->GetService(MR_VIDEO_RENDER_SERVICE, IID_IMFVideoDisplayControl, (void**)&pDisplay);
	if (FAILED(hr))  { goto done; }


	//--  Set clipping window
	hr = pDisplay->SetVideoWindow(hwnd);
	if (FAILED(hr))  { goto done; }

	//***
	hr = pDisplay->SetAspectRatioMode(MFVideoARMode_PreservePicture);
	if (FAILED(hr))  { goto done; }

	m_pVD = pDisplay;
	m_pVD->AddRef();


done:
	SAFE_RELEASE(pGS);
	SAFE_RELEASE(pDisplay);
	return hr;
}


HRESULT mtk_video_EVR::SetViewWindow(HWND hwnd, int x, int y, int w, int h, bool bFullScreen)
{
	HRESULT hr;

	RECT rect, rect2;

	GetClientRect(hwnd, &rect2);

	rect.left = x;
	rect.right = x + w;
	rect.bottom = y + h;
	rect.top = y;

	hr = m_pVD->SetVideoPosition(NULL, &rect);
	if (FAILED(hr)) {
		return hr;
	}


	/*
	BOOL b;
	if (bFullScreen) {
		m_pVD->SetFullscreen(TRUE);
		do {
			Sleep(10);
			m_pVD->GetFullscreen(&b);
		} while (!b);
	}
	*/


	return hr;
}



HRESULT mtk_video_EVR::SetWindowPosition(int x, int y, int w, int h)
{
	RECT rect;

	if (m_pVD == NULL) {
		return S_OK;
	}

	rect.left = x;
	rect.right = x + w;
	rect.bottom = y + h;
	rect.top = y;

	return  m_pVD->SetVideoPosition(NULL, &rect);
}


HRESULT mtk_video_EVR::GetVideoSize(long *pw, long *ph)
{
	if (m_pVD == NULL) {
		return E_NOINTERFACE;
	}

	HRESULT hr;
	SIZE sz;
	
	hr = m_pVD->GetNativeVideoSize(&sz, NULL);
	if (SUCCEEDED(hr)) {
		*pw = sz.cx;
		*ph = sz.cy;
	}

	return hr;
}


HRESULT mtk_video_EVR::OnPaint(HWND hwnd, HDC hDC)
{
	if (m_pVD == NULL) {
		return S_OK;
	}

	return m_pVD->RepaintVideo();
}


HRESULT mtk_video_EVR::OnDisplayChange()
{
	return S_OK;
}


HRESULT mtk_video_EVR::OnSize(const LPRECT pRect)
{
	if (m_pVD == NULL) {
		return S_OK;
	}

	return  m_pVD->SetVideoPosition(NULL, pRect);
}



