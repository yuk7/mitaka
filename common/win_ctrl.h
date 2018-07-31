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
#ifndef  __WIN_CTRL_H__
#define  __WIN_CTRL_H__
#include  <windows.h>
#include  <commctrl.h>
#include  <stdio.h>
#include  <assert.h>
typedef unsigned long  ulong;


//============
//  Controls
//============
class  Control
{
private:
	HWND  hWnd;
public:
	Control()  :  hWnd(NULL)  {}
	Control(HWND _hWnd)  :  hWnd( _hWnd )			{}
	void	SetHandle(HWND _hWnd)					{ hWnd = _hWnd; }
	void	SetHandleDlg(HWND hDlg, int nDlgItem)	{ hWnd = GetDlgItem( hDlg, nDlgItem ); }
	HWND	GetHandle() const						{ return  hWnd; }
	LRESULT	SendMsg( UINT Msg, WPARAM wParam, LPARAM lParam ) const
					{ assert(::IsWindow(hWnd)); return  ::SendMessage( hWnd, Msg, wParam, lParam ); }
};


//----------------------
//  PushButton Control
//----------------------
class ctrlButton  :  public  Control
{
public:
	ctrlButton()  :  Control()  {}
	void	Setup(HWND hDlg, int nDlgItem)				{ SetHandleDlg(hDlg, nDlgItem); }
	UINT	GetState() const							{ return UINT(SendMsg(BM_GETSTATE, 0, 0)); }
	void	SetState(BOOL bHighlight)					{ SendMsg(BM_SETSTATE, bHighlight, 0); }
	int		GetCheck() const							{ return int(SendMsg(BM_GETCHECK, 0, 0)); }
	void	SetCheck(int nCheck)						{ SendMsg(BM_SETCHECK, nCheck, 0); }
	UINT	GetButtonStyle() const						{ return UINT(GetWindowLong(GetHandle(), GWL_STYLE)) & 0xff; }
	void	SetButtonStyle(UINT nStyle, BOOL bRedraw)	{ SendMsg(BM_SETSTYLE, nStyle, (LPARAM)bRedraw); }
	void	SetIcon(HICON hIcon)						{ SendMsg(BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon); }
	HICON	GetIcon() const								{ return HICON(SendMsg(BM_GETIMAGE, IMAGE_ICON, 0L)); }
	void	SetBitmap(HBITMAP hBitmap)					{ SendMsg(BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap); }
	HBITMAP	GetBitmap() const							{ return HBITMAP(SendMsg(BM_GETIMAGE, IMAGE_BITMAP, 0L)); }
	void	SetCursor(HCURSOR hCursor)					{ SendMsg(BM_SETIMAGE, IMAGE_CURSOR, (LPARAM)hCursor); }
	HCURSOR	GetCursor() const							{ return HCURSOR(SendMsg(BM_GETIMAGE, IMAGE_CURSOR, 0L)); }
};


//----------------
//  Edit Control
//----------------
class ctrlEdit  :  public  Control
{
public:
	ctrlEdit() :  Control()  {}
	void	Setup(HWND hDlg, int nDlgItem, const char* szFormat, ...);
	void	SetText(const char* szText);
	void	SetTextf(const char* szFormat, ...);
	void	GetText(char* szBuf, int nmax);
	bool	GetText_double(double* x);
	bool	GetText_udouble(double* x);
	bool	GetText_int(int* i);
	bool	GetText_ulong(ulong* ul);
	void	Enable(bool bEnable);
private:
	static char szBuffer[], dummy[];
};


//------------
//  CheckBox
//------------
class ctrlCheck  :  public  Control
{
public:
	ctrlCheck() : Control() {}
	void	Setup(HWND hDlg, int nDlgItem, bool bCheck);
	void	SetCheck(bool bCheck);
	bool	GetCheck();
};


//-----------
//  ListBox
//-----------
class  ListBox  :  public  Control
{
public:
	ListBox() : Control()  {};
	void	Setup(HWND hDlg, int nDlgItem)		{ SetHandleDlg(hDlg, nDlgItem); }
	void	InitList(int nItem, const char** pszListText);
	void	Reset()								{ SendMsg( LB_RESETCONTENT, 0,0 ); }
	void	AddString(const char* szString)		{ SendMsg( LB_INSERTSTRING, -1, (LPARAM)szString ); }
	void	DeleteString(long idx)				{ SendMsg( LB_DELETESTRING, idx, 0 ); }
	void	GetText(long idx, char* szString)	{ SendMsg( LB_GETTEXT, (WPARAM)idx, (LPARAM)szString ); }
	void	SetCurSel(long idx)					{ SendMsg( LB_SETCURSEL, idx, 0); }
	long	GetCurSel()	const					{ return  SendMsg( LB_GETCURSEL, 0,0 ); } 
	void	SetRedraw(bool bRedraw)				{ SendMsg( WM_SETREDRAW, bRedraw, 0); };
};


//------------
//  ComboBox
//------------
class  ComboBox  :  public  Control
{
public:
	ComboBox()  : Control()  {};
	void	Setup(HWND hDlg, int nDlgItem)		{ SetHandleDlg(hDlg, nDlgItem); }
	void	InitList(int nItem, const char** pszListText);
	void	Reset()								{ SendMsg( CB_RESETCONTENT, 0,0); }
	void	AddString(const char* szString)		{ SendMsg( CB_ADDSTRING, 0, (LPARAM)szString); }
	void	SetCurSel(long idx)					{ SendMsg( CB_SETCURSEL, idx, 0); }
	long	GetCurSel()	const					{ return  SendMsg( CB_GETCURSEL, 0,0 ); } 
	void	SetRedraw(bool bRedraw)				{ SendMsg( WM_SETREDRAW, bRedraw, 0); };
};


//--------------------
//  Progress Control
//--------------------
class ctrlProgress  :  public  Control
{
public:
	ctrlProgress() : Control() {}
	void	Setup(HWND hDlg, int nDlgItem)		{ SetHandleDlg(hDlg, nDlgItem); }
	int		SetPos(int nPos)  { return  (int)SendMsg( PBM_SETPOS, nPos, 0L); }
	int		GetPos() const { return (int) SendMsg( PBM_GETPOS, 0, 0); }
	void	SetRange(int nLower, int nUpper)	{ SendMsg( PBM_SETRANGE32, (WPARAM) nLower, (LPARAM) nUpper); }
	void	GetRange(int& nLower, int& nUpper) const { SendMsg( PBM_GETRANGE, (WPARAM) &nLower, (LPARAM) &nUpper); }
	int SetStep(int nStep)  {return (int) SendMsg(PBM_SETSTEP, nStep, 0L); }
	int StepIt() { return (int) SendMsg(PBM_STEPIT, 0, 0L); }
};


//--------------------
//  Trackbar Control
//--------------------
class ctrlTrackbar  :  public  Control
{
public:
	ctrlTrackbar() : Control() {}
	void	Setup(HWND hDlg, int nDlgItem)		{ SetHandleDlg(hDlg, nDlgItem); }
	int		SetPos(int nPos)	{ return  (int)SendMsg( TBM_SETPOS, (WPARAM) TRUE, (LPARAM)nPos); }
	int		GetPos() const		{ return  (int)SendMsg( TBM_GETPOS, 0, 0); }
	void	SetRange(int nLower, int nUpper)	{ SendMsg( TBM_SETRANGE, (WPARAM) TRUE, (LPARAM) MAKELONG(nLower, nUpper) ); }
	int		GetRangeMax() const	{ return  (int)SendMsg( TBM_GETRANGEMAX, 0, 0); }
	int		GetRangeMin() const	{ return  (int)SendMsg( TBM_GETRANGEMIN, 0, 0); }
};


//----------------
//  Spin Control
//----------------
class ctrlSpin  :  public  Control
{
public:
	ctrlSpin() : Control() {}
	void	Setup(HWND hDlg, int nDlgItem)	{ SetHandleDlg(hDlg, nDlgItem); }
	UINT	GetAccel(int nAccel, UDACCEL* pAccel) const	{ return UINT( LOWORD(SendMsg(UDM_GETACCEL, nAccel, (LPARAM)pAccel)) ); }
	UINT	GetBase() const						{ return UINT( LOWORD(SendMsg(UDM_GETBASE, 0, 0L)) ); }
	int		GetPos() const						{ return int(SendMsg(UDM_GETPOS, 0, 0L)); }
	DWORD	GetRange() const					{ return DWORD(SendMsg(UDM_GETRANGE, 0, 0L)); }
	HWND	GetBuddy() const					{ return HWND(SendMsg(UDM_GETBUDDY, 0, 0L)); }
	void	SetAccel(int nAccel, UDACCEL* pAccel)	{ SendMsg(UDM_SETACCEL, nAccel, (LPARAM)pAccel); }
	void	SetBase(int nBase)					{ SendMsg(UDM_SETBASE, nBase, 0L); }
	void	SetPos(int nPos)					{ SendMsg(UDM_SETPOS,   0, MAKELPARAM(nPos, 0)); }
	void	SetRange(int nLower, int nUpper)	{ SendMsg(UDM_SETRANGE, 0, MAKELPARAM(nUpper, nLower)); }
	void	SetBuddy(HWND hWndBuddy)			{ SendMsg(UDM_SETBUDDY, (WPARAM)hWndBuddy, 0L); }
};


//--- inline ---
inline void
ctrlEdit::SetText(const char *szText)
{
	SendMsg( WM_SETTEXT, 0, (LPARAM)szText );
}

inline void
ctrlEdit::GetText(char* szBuf, int nmax)
{
	*(LPWORD)szBuf = (WORD)nmax;
	int n = SendMsg( EM_GETLINE, 0, (LPARAM)szBuf );
	szBuf[n] = '\0';
}

inline void
ctrlEdit::Enable(bool bEnable)
{
	SendMsg( WM_ENABLE, (WPARAM)bEnable, 0 );
}


inline void
ctrlCheck::Setup(HWND hDlg, int nDlgItem, bool bCheck)
{
	SetHandleDlg( hDlg, nDlgItem );
	SetCheck(bCheck);
}

inline void
ctrlCheck::SetCheck(bool bCheck)
{
	SendMsg( BM_SETCHECK, bCheck, 0 );
}

inline bool
ctrlCheck::GetCheck()
{
	return	( SendMsg( BM_GETCHECK, 0, 0 )!=0 ) ? true : false;
}


#endif
