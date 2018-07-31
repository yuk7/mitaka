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
#include  "mtk_dialog_box.h"
#include  <stdio.h>
#include  <process.h>
#include  <commctrl.h>
#include  "../common/interface/joystick.h"
#include  "../common/JulianDate.h"
#include  "../common/cpu.h"
#include  "mtk_command.h"
#include  "mtk_switches.h"
#include  "mtk_text.h"
#include  "mtk_joystick.h"
#include  "mtk_cpu_info.h"
#include  "mtk_directory.h"
#include  "mtk_pbuffer.h"
#include  "mtk_save_image.h"
#include  "mtk_time.h"
#include  "../common/graphics/dib.h"
#include  "../common/win_dlg.h"
#include  "../common/win_ctrl.h"
#include  <gl/gl.h>
#include  "../common/text/unicode_string.h"
#include  "mtk_strings.h"
#include  "mtk_languages.h"
#include  "resource.h"





//-- externs
extern HINSTANCE	g_hInst;
extern HWND			g_hWnd;
extern char szTitle[256];
extern char	szAppName[];
extern const GLubyte *szGLVersion;
extern mtk_camera g_camera;
extern float DT;


/*
const wchar_t* splash_image_fn = L"mitaka_splash.png";
const int splash_w = 512;
const int splash_h = 384;
const COLORREF  ver_col = RGB(255, 255, 255);
const int ver_x = 250;
const int ver_y = 9;
const COLORREF  loading_col = RGB(153, 153, 153);
const int loading_x = 13;
const int loading_y = 362;
*/
const wchar_t szCopyright_E[] = L"Copyright (C) 2003-2016   4-Dimensional Digital Universe Project, NAOJ, Japan";
const wchar_t szCopyright_J[] = L"Copyright (C) 2003-2016   国立天文台４次元デジタル宇宙プロジェクト";
const wchar_t szDeveloper_E[] = L"Developer:  Tsunehiko Kato (4D2U, NAOJ)";
const wchar_t szDeveloper_J[] = L"開発者： 加藤恒彦 （国立天文台 4D2U）";
const wchar_t szAuthor2_E[] = L"Visual Design:  Sorahiko Nukatani (NAOJ)";
const wchar_t szAuthor2_J[] = L"視覚デザイン:  額谷宙彦　（国立天文台）";





//=======================
//  Set Time Dialog Box
//=======================
HWND hWndSetTime = NULL;



LRESULT CALLBACK
SetTimeDialogBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	#define BOLDDAY(ds,iDay) if(iDay>0 && iDay<32) (ds)|=(0x00000001<<(iDay-1))
	#define lpnmDS ((NMDAYSTATE *)lParam)
	#define MAX_MONTHS 12


	static HWND	hStaticDate, hStaticYear, hStaticTime;
	static HWND	hStaticHour, hStaticMinute, hStaticSecond;
	static HWND	hBtnTonight, hBtnPresentTime;
	static HWND	hBtnOK, hBtnApply, hBtnCancel;
	static HWND	hCalendar;
	static SYSTEMTIME st;
	static ctrlEdit edYear, edHour, edMin, edSec;
	static ctrlSpin	spn, spn_year;
	static int hour, minute, sec;
	static int SpnMin, SpnMax;


	MONTHDAYSTATE mds[MAX_MONTHS];
	INT i, iMax;
	LPNMHDR hdr = (LPNMHDR)lParam;

	WORD  code, cmd_id;


	switch( message )
	{
		case WM_INITDIALOG:
			hStaticDate = GetDlgItem(hDlg, IDC_STATIC_TIME_DATE);
			hStaticYear = GetDlgItem(hDlg, IDC_STATIC_TIME_YEAR);
			hStaticTime = GetDlgItem(hDlg, IDC_STATIC_TIME_TIME);
			hStaticHour = GetDlgItem(hDlg, IDC_STATIC_TIME_HOUR);
			hStaticMinute = GetDlgItem(hDlg, IDC_STATIC_TIME_MINUTE);
			hStaticSecond = GetDlgItem(hDlg, IDC_STATIC_TIME_SECOND);
			hBtnTonight = GetDlgItem(hDlg, IDC_BUTTON_TONIGHT);
			hBtnPresentTime = GetDlgItem(hDlg, IDC_BUTTON_PRESENT_TIME);
			hBtnOK = GetDlgItem(hDlg, IDOK);
			hBtnApply = GetDlgItem(hDlg, IDC_BUTTON_APPLY);
			hBtnCancel = GetDlgItem(hDlg, IDCANCEL);
			hCalendar = GetDlgItem(hDlg, IDC_MONTHCALENDAR1);

			//--  Title
			::SetWindowTextW(hDlg, (LPCWSTR)get_wstring("DLG_TIME_TITLE").c_str());

			//--  Static Text
			::SendMessageW(hStaticDate, WM_SETTEXT, 0, (LPARAM)get_wstring("DLG_TIME_DATE").c_str());
			::SendMessageW(hStaticYear, WM_SETTEXT, 0, (LPARAM)get_wstring("DLG_TIME_YEAR").c_str());
			::SendMessageW(hStaticTime, WM_SETTEXT, 0, (LPARAM)get_wstring("DLG_TIME_TIME").c_str());
			::SendMessageW(hStaticHour, WM_SETTEXT, 0, (LPARAM)get_wstring("DLG_TIME_HOUR").c_str());
			::SendMessageW(hStaticMinute, WM_SETTEXT, 0, (LPARAM)get_wstring("DLG_TIME_MINUTE").c_str());
			::SendMessageW(hStaticSecond, WM_SETTEXT, 0, (LPARAM)get_wstring("DLG_TIME_SECOND").c_str());
			::SendMessageW(hBtnTonight, WM_SETTEXT, 0, (LPARAM)get_wstring("DLG_TIME_TONIGHT").c_str());
			::SendMessageW(hBtnPresentTime, WM_SETTEXT, 0, (LPARAM)get_wstring("DLG_TIME_PRESENT_TIME").c_str());
			::SendMessageW(hBtnOK, WM_SETTEXT, 0, (LPARAM)get_wstring("DLG_TIME_OK").c_str());
			::SendMessageW(hBtnApply, WM_SETTEXT, 0, (LPARAM)get_wstring("DLG_TIME_APPLY").c_str());
			::SendMessageW(hBtnCancel, WM_SETTEXT, 0, (LPARAM)get_wstring("DLG_TIME_CANCEL").c_str());


			//-- カレンダーコントロールの設定
			MonthCal_SetColor(hCalendar, MCSC_BACKGROUND, RGB(175,175,175));
			MonthCal_SetColor(hCalendar, MCSC_MONTHBK,    RGB(248,245,225));
			{
				UTC utc = JD_to_UTC(get_time_JD() + DT/24.0);
				st.wYear			= utc.year;
				st.wMonth			= utc.month;
				st.wDay				= utc.day;
				st.wHour			= utc.hour;
				st.wMinute			= 0;
				st.wSecond			= 0;
				st.wMilliseconds	= 0;
				hour	= utc.hour;
				minute	= utc.min;
				sec		= int(utc.sec);
				MonthCal_SetCurSel(hCalendar, &st);
			}
			//-- 年設定エディットボックス
			edYear.Setup(hDlg, IDC_EDIT_YEAR, "%d", st.wYear);
			// スピンボタン
			spn_year.Setup(hDlg, IDC_SPIN_YEAR);
			spn_year.SetBuddy(edYear.GetHandle());
			spn_year.SetRange(0, 3000);
			//-- 時・分・秒設定エディットボックス
			edHour.Setup(hDlg, IDC_EDIT_HOUR,	"%d", hour);
			edMin.Setup(hDlg, IDC_EDIT_MIN,	"%d", minute);
			edSec.Setup(hDlg, IDC_EDIT_SEC,	"%d", sec);
			// スピンボタン
			spn.Setup(hDlg, IDC_SPIN_TIME);
			spn.SetBuddy(edHour.GetHandle());
			SpnMin = 0;
			SpnMax = 23;
			spn.SetRange(SpnMin-1, SpnMax+1);

			return TRUE;
		case WM_COMMAND:
			code = HIWORD(wParam);
			cmd_id = LOWORD(wParam);
			switch (cmd_id)  {
				case IDOK:
					OnChangeTime( -DT/24.0 + get_JD(st.wYear, st.wMonth, st.wDay, hour, minute, sec + 0.5));
					InvalidateRect(g_hWnd, NULL, false);
					SendMessage(hDlg, WM_CLOSE, 0, 0);
					return TRUE;
				case IDCANCEL:
					SendMessage(hDlg, WM_CLOSE, 0, 0);
					return TRUE;

				case IDC_BUTTON_PRESENT_TIME:
					{
						// カレンダーコントロール
						UTC utc = get_present_LocalTime(DT);
						st.wYear			= utc.year;
						st.wMonth			= utc.month;
						st.wDay				= utc.day;
						st.wHour			= 0;
						st.wMinute			= 0;
						st.wSecond			= 0;
						st.wMilliseconds	= 0;
						hour	= utc.hour;
						minute	= utc.min;
						sec		= int(utc.sec);
						MonthCal_SetCurSel(hCalendar, &st);
						// 年・時・分・秒設定エディットボックス
						edYear.Setup(hDlg, IDC_EDIT_YEAR,	"%d", st.wYear);
						edHour.Setup(hDlg, IDC_EDIT_HOUR,	"%d", hour);
						edMin.Setup(hDlg, IDC_EDIT_MIN,		"%d", minute);
						edSec.Setup(hDlg, IDC_EDIT_SEC,		"%d", sec);
					}
					break;

				case IDC_BUTTON_TONIGHT:
					{
						// カレンダーコントロール
						hour	= 20;
						minute	= 0;
						sec		= 0;
						// 年・時・分・秒設定エディットボックス
						edHour.Setup(hDlg, IDC_EDIT_HOUR,	"%d", hour);
						edMin.Setup(hDlg, IDC_EDIT_MIN,	"%d", minute);
						edSec.Setup(hDlg, IDC_EDIT_SEC,	"%d", sec);
					}
					break;

				case IDC_BUTTON_APPLY:
					OnChangeTime( -DT/24.0 + get_JD(st.wYear, st.wMonth, st.wDay, hour, minute, sec + 0.5));
					InvalidateRect(g_hWnd, NULL, false);
					break;

				case IDC_EDIT_YEAR:
					switch (code) {
						case EN_UPDATE:
							{
								int itmp;
								edYear.GetText_int(&itmp);
								if (itmp != st.wYear) {
									if (itmp>=0 && itmp<3000)	{ st.wYear = itmp; MonthCal_SetCurSel(hCalendar, &st); }
									else						{ edYear.SetTextf("%d", st.wYear); }
								}
							}
							break;
						default:
							break;
					}
					break;

				case IDC_EDIT_HOUR:
					switch (code) {
						case EN_UPDATE:
							{
								int itmp;
								edHour.GetText_int(&itmp);
								if (itmp>=0 && itmp<24)		{ hour = itmp; }
								else						{ edHour.SetTextf("%d", hour); }
							}
							break;
						case EN_SETFOCUS:
							spn.SetBuddy(edHour.GetHandle());
							SpnMin = 0;
							SpnMax = 23;
							spn.SetRange(SpnMin-1, SpnMax+1);
							break;
						default:
							break;
					}
					break;
				case IDC_EDIT_MIN:
					switch (code) {
						case EN_UPDATE:
							{
								int itmp;
								edMin.GetText_int(&itmp);
								if (itmp>=0 && itmp<60)		{ minute = itmp; }
								else						{ edMin.SetTextf("%d", minute); }
							}
							break;
						case EN_SETFOCUS:
							spn.SetBuddy(edMin.GetHandle());
							SpnMin = 0;
							SpnMax = 59;
							spn.SetRange(SpnMin-1, SpnMax+1);
							break;
						default:
							break;
					}
					break;
				case IDC_EDIT_SEC:
					switch (code) {
						case EN_UPDATE:
							{
								int itmp;
								edSec.GetText_int(&itmp);
								if (itmp>=0 && itmp<60)		{ sec = itmp; }
								else						{ edSec.SetTextf("%d", sec); }
							}
							break;
						case EN_SETFOCUS:
							spn.SetBuddy(edSec.GetHandle());
							SpnMin = 0;
							SpnMax = 59;
							spn.SetRange(SpnMin-1, SpnMax+1);
							break;
						default:
							break;
					}
					break;
			}
			break;

		
		case WM_NOTIFY:
			if (hdr->idFrom == IDC_MONTHCALENDAR1) {
				switch (hdr->code) {
					// カレンダー
					case MCN_GETDAYSTATE:
						iMax=lpnmDS->cDayState;
						for(i=0;i<iMax;i++){
							mds[i] = (MONTHDAYSTATE)0;
							BOLDDAY(mds[i],15);
						}
						lpnmDS->prgDayState = mds;
						break;
					// カレンダー選択
					case MCN_SELCHANGE:
						{
							int year0 = st.wYear;
							MonthCal_GetCurSel(hCalendar, &st);
							if (year0 != st.wYear) {
								edYear.Setup(hDlg, IDC_EDIT_YEAR, "%d", st.wYear);
							}
						}
						break;
				}
			} else if (hdr->idFrom == IDC_SPIN_TIME) {
				// 時刻のスピンボタン
				switch (hdr->code) {
					case UDN_DELTAPOS:
						{
							// ループ処理
							NMUPDOWN* pud;
							pud = LPNMUPDOWN(lParam);
							if (pud->iPos <= SpnMin && pud->iDelta<0)	{
								spn.SetPos(SpnMax+1);
							} else if (pud->iPos >= SpnMax && pud->iDelta>0) {
								spn.SetPos(SpnMin-1);
							}
						}
						break;
				}
			}
			break;
		case WM_CLOSE:
			DestroyWindow(hDlg);
			hWndSetTime = NULL;
			break;
	}
    return FALSE;
}




void __cdecl
SetTimeDialog_thread(void* pParam)
{
	LPCTSTR  lpDlgTemp = MAKEINTRESOURCE(IDD_DIALOG_TIME);
	hWndSetTime = CreateDialog(g_hInst, lpDlgTemp, g_hWnd, (DLGPROC)SetTimeDialogBox);

	ShowWindow(hWndSetTime, SW_SHOW);

	//--- メッセージ・ループ
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (hWndSetTime == 0 || !IsDialogMessage(hWndSetTime, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	_endthread();
}


void
CreateSetTimeDialog()
{
	if (hWndSetTime == NULL) {
		_beginthread(SetTimeDialog_thread, 0, 0);
	} else {
		SetForegroundWindow(hWndSetTime);
	}
}





//==========================
//　　Save Image Dialog Box
//==========================
LRESULT CALLBACK
SaveImageHighResDialogBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND	hStaticWidth, hStaticHeight;
	static HWND	hStaticMaxWidth, hStaticMaxHeight;
	static HWND	hStaticSave, hStaticCancel;
	static ctrlEdit edWidth, edHeight;
	static int width;
	static int height;
	static bool bFirst = true;


	const int wmax = get_pbuffer_width_max();
	const int hmax = get_pbuffer_height_max();

	if (bFirst) {
		width = min(1024, wmax);
		height = min(1024, hmax);
		bFirst = false;
	}


	WORD  code;

	switch (message)
	{
	case WM_INITDIALOG:
	{
		hStaticWidth = GetDlgItem(hDlg, IDC_STATIC_SAVE_IMAGE_WIDTH);
		hStaticHeight = GetDlgItem(hDlg, IDC_STATIC_SAVE_IMAGE_HEIGHT);
		hStaticMaxWidth = GetDlgItem(hDlg, IDC_STATIC_SAVE_IMAGE_MAX_WIDTH);
		hStaticMaxHeight = GetDlgItem(hDlg, IDC_STATIC_SAVE_IMAGE_MAX_HEIGHT);
		hStaticSave = GetDlgItem(hDlg, IDOK);
		hStaticCancel = GetDlgItem(hDlg, IDCANCEL);

		//--  Title
		::SetWindowTextW(hDlg, (LPCWSTR)get_wstring("DLG_IMAGE_TITLE").c_str());

		//--  Width and Height
		::SendMessageW(hStaticWidth, WM_SETTEXT, 0, (LPARAM)get_wstring("DLG_IMAGE_WIDTH").c_str());
		::SendMessageW(hStaticHeight, WM_SETTEXT, 0, (LPARAM)get_wstring("DLG_IMAGE_HEIGHT").c_str());

		//--  Max Width and Height
		wchar_t szMax_w[256], szMax_h[256];
		swprintf(szMax_w, get_wstring("DLG_IMAGE_MAX_FMT").c_str(), wmax);
		swprintf(szMax_h, get_wstring("DLG_IMAGE_MAX_FMT").c_str(), hmax);
		::SendMessageW(hStaticMaxWidth, WM_SETTEXT, 0, (LPARAM)szMax_w);
		::SendMessageW(hStaticMaxHeight, WM_SETTEXT, 0, (LPARAM)szMax_h);

		//--  Save/Cancel Button
		::SendMessageW(hStaticSave, WM_SETTEXT, 0, (LPARAM)get_wstring("DLG_IMAGE_SAVE_BUTTON").c_str());
		::SendMessageW(hStaticCancel, WM_SETTEXT, 0, (LPARAM)get_wstring("DLG_IMAGE_CANCEL_BUTTON").c_str());


		if (width > wmax)  width = wmax;
		if (height > hmax) height = hmax;
		edWidth.Setup(hDlg, IDC_EDIT_SAVE_IMAGE_W, "%d", width);
		edHeight.Setup(hDlg, IDC_EDIT_SAVE_IMAGE_H, "%d", height);
	}

	return TRUE;

	case WM_COMMAND:
		code = HIWORD(wParam);
		switch (LOWORD(wParam))  {
		case IDOK:
		{
			if (width <= 0 || height <= 0) {
				MessageBoxWf(hDlg, "Error", MB_OK, get_string("DLG_IMAGE_SIZE_MIN_CAUTION").c_str());
				break;
			}

			if (width > wmax || height > hmax) {
				MessageBoxWf(hDlg, "Error", MB_OK, get_string("DLG_IMAGE_SIZE_MAX_CAUTION").c_str(), wmax, hmax);
				break;
			}

			if (!save_image_high_resolution_get_filename(width, height)) {
				break;
			}

			EndDialog(hDlg, 0);
			return TRUE;
		}
		case IDCANCEL:
			EndDialog(hDlg, 0);
			return TRUE;

		case IDC_EDIT_SAVE_IMAGE_W:
			switch (code) {
			case EN_UPDATE:		edWidth.GetText_int(&width);		break;
			default:												break;
			}
			break;
		case IDC_EDIT_SAVE_IMAGE_H:
			switch (code) {
			case EN_UPDATE:		edHeight.GetText_int(&height);		break;
			default:												break;
			}
			break;
		default:		break;
		}


		break;
	}
	return FALSE;
}






//====================
//  About Dialog Box
//====================
LRESULT CALLBACK
About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND	hStaticAppName, hStaticCopyright, hStaticCopyright2, hStaticJoystick;
	static HWND	hStaticCPU, hStaticExtentions, hStaticOpenGLVer;
	wchar_t str[512];
	bool bJ;


	switch (message)
	{
	case WM_INITDIALOG:
		bJ = is_current_language_ISO639("ja");

		::SetWindowTextW(hDlg, (LPCWSTR)get_wstring("DLG_ABOUT_TITLE").c_str());

		//--  App Name
		hStaticAppName = GetDlgItem(hDlg, IDC_STATIC_APPNAME);
		::SendMessageW(hStaticAppName, WM_SETTEXT, 0, (LPARAM)utf8_to_wstring(szTitle).c_str());

		//--  Copyright
		hStaticCopyright = GetDlgItem(hDlg, IDC_STATIC_COPYRIGHT);
		if (bJ)		::SendMessageW(hStaticCopyright, WM_SETTEXT, 0, (LPARAM)szCopyright_J);
		else		::SendMessageW(hStaticCopyright, WM_SETTEXT, 0, (LPARAM)szCopyright_E);

		//--  Author
		hStaticCopyright = GetDlgItem(hDlg, IDC_STATIC_AUTHOR);
		if (bJ)		::SendMessageW(hStaticCopyright, WM_SETTEXT, 0, (LPARAM)szDeveloper_J);
		else		::SendMessageW(hStaticCopyright, WM_SETTEXT, 0, (LPARAM)szDeveloper_E);

		//--  Author2
		hStaticCopyright2 = GetDlgItem(hDlg, IDC_STATIC_AUTHOR2);
		if (bJ)		::SendMessageW(hStaticCopyright2, WM_SETTEXT, 0, (LPARAM)szAuthor2_J);
		else		::SendMessageW(hStaticCopyright2, WM_SETTEXT, 0, (LPARAM)szAuthor2_E);

		//--  Joystick
		hStaticJoystick = GetDlgItem(hDlg, IDC_STATIC_JOYSTICK);

		if (is_joystick_available())	{ wsprintfW(str, L"%s", L"A joystick is available."); }
		else							{ wsprintfW(str, L" "); }
		::SendMessageW(hStaticJoystick, WM_SETTEXT, 0, (LPARAM)str);

		//--  CPU
		hStaticCPU = GetDlgItem(hDlg, IDC_STATIC_CPU);
		wsprintfW(str, L"CPU: %d", get_number_of_cpu());
		::SendMessageW(hStaticCPU, WM_SETTEXT, 0, (LPARAM)str);

		//--  Extentions
		hStaticExtentions = GetDlgItem(hDlg, IDC_STATIC_EXTENTIONS);
		wsprintfW(str, L"Extentions:   ");
		if (is_SSE_Enable())		{ wcscat(str, L"SSE  "); }
		if (isPBufferEnable())		{ wcscat(str, L"pBuffer  "); }
		::SendMessageW(hStaticExtentions, WM_SETTEXT, 0, (LPARAM)str);

		//--  OpenGL version
		hStaticOpenGLVer = GetDlgItem(hDlg, IDC_STATIC_OPENGL_VERSION);
		wsprintfW(str, L"OpenGL Version: %s", utf8_to_wstring((char*)szGLVersion).c_str());
		::SendMessageW(hStaticOpenGLVer, WM_SETTEXT, 0, (LPARAM)str);

		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))  {
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}






//=================
//  Splash Window
//=================
HWND	hSplashWindow = NULL;
dib splash_image;
std::wstring SplashText;
HFONT hSplashFont;
const int splash_text_size = 14;

void
DrawSplashBitmap(HDC hDC)
{
	BitBlt(hDC, 0,0,splash_image.get_width(),splash_image.get_height(), splash_image.getDC(), 0,0, SRCCOPY);
	SetBkMode(hDC, TRANSPARENT);

	//--- 文字列の描画 ---
	HFONT hFontOld = (HFONT)SelectObject(hDC, (HGDIOBJ)hSplashFont);

	// バージョン
	//-- Mitaka
	SetTextColor(hDC, RGB(255,255,255));
	TextOut(hDC, 250, 9, szTitle, strlen(szTitle));
	//--- Mitaka++
	//SetTextColor(hDC, RGB(200,200,200));
	//TextOut(hDC, 475, 5, szTitle, strlen(szTitle));

	// 読み込み状況
	//-- Mitaka
	SetTextColor(hDC, RGB(153,153,153));
	TextOutW(hDC, 13, 362, SplashText.c_str(), SplashText.length());
	//-- Mitka++
	//SetTextColor(hDC, RGB(170,170,170));
	//TextOut(hDC, 5, 83, SplashText, strlen(SplashText));
	
	SelectObject(hDC, (HGDIOBJ)hFontOld);
}


LRESULT CALLBACK
SplashWindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	HDC hDC;
	PAINTSTRUCT ps;
	const char font_name[] = "Tahoma";
	//const char font_name[] = "Arial Unicode MS";

	switch( message )
	{
		case WM_CREATE:
			{
				directory dir = get_directory(DIR_DATA);
				image img;
				img.load( dir.get_path("mitaka_splash.png") );			// Mitaka
				//img.load( dir.get_path("mitaka_pp_splash.png") );		// Mitaka++
				image_to_dib(img, splash_image);

				hSplashFont = CreateFont(
					splash_text_size,
					0, 0, 0, 0,
					FALSE, FALSE, FALSE, SHIFTJIS_CHARSET,
					OUT_DEVICE_PRECIS, 
					CLIP_DEFAULT_PRECIS,
					ANTIALIASED_QUALITY, DEFAULT_PITCH,
					font_name);
			}

			return 0;

		case WM_CLOSE:
			DeleteObject((HGDIOBJ)hSplashFont);
			DestroyWindow(hWnd);
			hSplashWindow = NULL;
			splash_image.free();
			return 0;

		case WM_PAINT:
			hDC = BeginPaint(hWnd, &ps);
			DrawSplashBitmap(hDC);
			EndPaint(hWnd, &ps);
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}


void __cdecl
Splash_thread(void* pParam)
{
	//
	const int w = 512;
	const int h = 384;
	
	//  ウィンドウクラスの登録
	WNDCLASS	wndclass;
	wndclass.style			= CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc	= SplashWindowProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= g_hInst;
	wndclass.hIcon			= NULL;
	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName	= NULL;
	wndclass.lpszClassName	= "MitakaSplash";
	if (!RegisterClass(&wndclass)) {
		MessageBox(NULL, TEXT("This program requires WindowsNT!"), szAppName, MB_ICONERROR);
		return;
	}

	//  画面の中心を取得する
	int x, y;
	x = GetSystemMetrics(SM_CXSCREEN)/2 - w/2;
	y = GetSystemMetrics(SM_CYSCREEN)/2 - h/2;

	//  ウィンドウの生成
	DWORD dwStyle = WS_VISIBLE|WS_POPUP;

	hSplashWindow = CreateWindowEx(WS_EX_APPWINDOW, //WS_EX_TOPMOST,
								"MitakaSplash",			// window class
								szAppName,				// window caption
								dwStyle,				// window style
								x, y,					// window position
								w, h,					// window size
								NULL,					// parent window handle
								NULL,					// window menu handle
								g_hInst,				// program instance handle
								NULL);					// creation parameters


	//--- スプラッシュウィンドウ起動 ---
	ShowWindow(hSplashWindow, SW_SHOW); 


	//--- メッセージ・ループ
	MSG msg;
	while (GetMessage(&msg, NULL, 0,0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	_endthread();
}



//
void
CreateSplashWindow()
{
	_beginthread(Splash_thread, 0, 0);
}

void
DestroySplashWindow()
{
	if (hSplashWindow) {
		SendMessage(hSplashWindow, WM_CLOSE, 0, 0);
	}
}




void
SetSplashText(const std::string& key)
{
	if (hSplashWindow == NULL)  return;

	SplashText = get_wstring(key);

	InvalidateRect(hSplashWindow, NULL, FALSE);
}

