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
#include  <stdio.h>
#include  "win_dlg.h"
#include  "./text/unicode_string.h"


//----------------------------
//  GetFileName
//----------------------------
//  return value:
//   -1 ... cancel or error
//   otherwise ... filter no
//----------------------------
int
GetFileName(HWND hWnd, char* szFileName, const char* szFilter, const char* szTitle, bool bSave)
{
	OPENFILENAME	ofn;
	char ext[4];

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize	= sizeof(OPENFILENAME);
	ofn.hwndOwner	= hWnd;
	ofn.Flags		= OFN_EXPLORER | OFN_HIDEREADONLY;
	ofn.lpstrFilter	= szFilter;
	ofn.lpstrFile	= szFileName;
	ofn.lpstrTitle	= szTitle;
	ofn.nMaxFile	= 256;
	ext[0] = '\0';
	ofn.lpstrDefExt = ext;	// こうすることで、拡張子をフィルタから自動的に補う
	
	BOOL res;
	if (bSave) {
		ofn.Flags |= OFN_OVERWRITEPROMPT;			//  check overwrite
		res = GetSaveFileName(&ofn);
	} else {
		ofn.Flags |= OFN_FILEMUSTEXIST;				//  the file must exist
		res = GetOpenFileName(&ofn);
	}
	if (res != TRUE)  return -1;
	return  ofn.nFilterIndex;	// return current filter index
}


int
GetFileNameW(HWND hWnd, wchar_t* szFileName, const wchar_t* szFilter, const wchar_t* szTitle, bool bSave)
{
	OPENFILENAMEW	ofn;
	wchar_t ext[10];

	ZeroMemory(&ofn, sizeof(OPENFILENAMEW));
	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = hWnd;
	ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
	ofn.lpstrFilter = szFilter;
	ofn.lpstrFile = (LPWSTR)szFileName;
	ofn.lpstrTitle = szTitle;
	ofn.nMaxFile = 256;
	ext[0] = L'\0';
	ofn.lpstrDefExt = ext;	// こうすることで、拡張子をフィルタから自動的に補う

	BOOL res;
	if (bSave) {
		ofn.Flags |= OFN_OVERWRITEPROMPT;			//  check overwrite
		res = GetSaveFileNameW(&ofn);
	}
	else {
		ofn.Flags |= OFN_FILEMUSTEXIST;				//  the file must exist
		res = GetOpenFileNameW(&ofn);
	}
	if (res != TRUE)  return -1;
	return  ofn.nFilterIndex;	// return current filter index
}



//========================
//  formatted MessageBox
//========================
int
MessageBoxf(HWND hWnd, LPCTSTR szCaption, UINT uType, LPCSTR szFormat, ...)
{
	char	szBuffer[256];
	va_list	pArgs;
	va_start( pArgs, szFormat );
		vsprintf( szBuffer, szFormat, pArgs );
	va_end( pArgs );
	return  MessageBox(hWnd, szBuffer, szCaption, uType);
}

int
MessageBoxWf(HWND hWnd, LPCTSTR szCaption, UINT uType, LPCSTR szFormat, ...)
{
	char	szBuffer[256];
	va_list	pArgs;
	va_start(pArgs, szFormat);
		vsprintf(szBuffer, szFormat, pArgs);
	va_end(pArgs);
	return  MessageBoxW(hWnd, utf8_to_wstring(szBuffer).c_str(), utf8_to_wstring(szCaption).c_str(), uType);
}


//-------------------------------------------
//  TextOutf: output a formatted text on DC
//-------------------------------------------
int
TextOutf(HDC hDC, int x, int y, const char* szFormat, ...)
{
	int				length;
	static char		szBuffer[1024];
	va_list			pArgs;
	va_start( pArgs, szFormat );
		length = vsprintf( szBuffer, szFormat, pArgs );
	va_end( pArgs );
	if ( !TextOut(hDC, x,y, szBuffer, length) )
		return 0;

	return length;
}
