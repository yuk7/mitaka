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
#include  "win_ctrl.h"


//============
//  Controls
//============

// definition of static member
char ctrlEdit::szBuffer[1024], ctrlEdit::dummy[1024];

void
ctrlEdit::Setup(HWND hDlg, int nDlgItem, const char* szFormat, ...)
{
	SetHandleDlg( hDlg, nDlgItem );

	int			iReturn;
	va_list		pArgs;
	va_start( pArgs, szFormat );
		iReturn = vsprintf( szBuffer, szFormat, pArgs );
	va_end( pArgs );

	SetText(szBuffer);
}

void
ctrlEdit::SetTextf(const char* szFormat, ...)
{
	int			iReturn;
	va_list		pArgs;

	va_start( pArgs, szFormat );
		iReturn = vsprintf( szBuffer, szFormat, pArgs );
	va_end( pArgs );

	SetText(szBuffer);
}

bool
ctrlEdit::GetText_double(double* x)
{
	double dm;

	GetText(szBuffer, 1024);
	if (sscanf(szBuffer, "%lf%s", &dm, dummy)==1) {		// １フィールドが正常に入力できたかチェック
														// %s は、余計な入力がないことをチェックするために使用
		*x = dm;
		return true;
	}

	return false;
}

bool
ctrlEdit::GetText_udouble(double* x)
{
	double dm;

	if (GetText_double(&dm) && dm>=0.0) {
			*x = dm;
			return true;
	}

	return false;
}

bool
ctrlEdit::GetText_int(int* i)
{
	int dm;
	GetText(szBuffer, 1024);
	
	if (sscanf(szBuffer, "%d%s", &dm, dummy)==1) {		// １フィールドが正常に入力できたかチェック
		*i = dm;
		return true;
	}
	return false;
}

bool
ctrlEdit::GetText_ulong(ulong* ul)
{
	ulong dm;
	GetText(szBuffer, 1024);
	if (sscanf(szBuffer, "%lu%s", &dm, dummy)==1) {		// １フィールドが正常に入力できたかチェック
		*ul = dm;
		return true;
	}
	return false;
}

void
ListBox::InitList(int nItem, const char **pszListText)
{
	Reset();
	for (int i=0; i<nItem; i++) {
		AddString( pszListText[i] );
	}
}

void
ComboBox::InitList(int nItem, const char **pszListText)
{
	Reset();
	for (int i=0; i<nItem; i++) {
		AddString( pszListText[i] );
	}
}
