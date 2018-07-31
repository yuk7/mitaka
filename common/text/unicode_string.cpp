/**************************************************************************************************

Written by Tsunehiko Kato
Copyright (c) 2008   Tsunehiko Kato

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
#include  "unicode_string.h"





//------------------
//  UTF8 <-> UTF32
//------------------
string_utf32
UTF8_to_UTF32_string(const std::string& src8)
{
	const char* p0 = src8.c_str();
	string_utf32 dest32;

	int i = 0;
	while (i < src8.size()) {
		int adv;
		utf32  uc32;

		uc32 = UTF8_to_UTF32(p0 + i, adv);
		i += adv;

		dest32.push_back(uc32);
	}
	dest32.push_back(0);


	return dest32;
}



//-----------------
//  UTF8 <-> UTF16
//-----------------
string_utf16
UTF8_to_UTF16_string(const std::string& src8)
{
	const char* p0 = src8.c_str();
	string_utf16 dest16;

	int i = 0;
	while (i < src8.size()) {
		int adv, count;
		utf16*  p16;

		p16 = UTF8_to_UTF16(p0 + i, adv, count);
		i += adv;

		dest16.push_back(*p16++);
		if (count > 1) {
			dest16.push_back(*p16++);
		}
	}

	dest16.push_back(utf16(0));

	return dest16;
}



std::string
UTF16_to_UTF8_string(const string_utf16& src16)
{
	const utf16* p0 = src16.data();
	std::string dest8;

	int i = 0;
	while (i < src16.size()) {
		int adv, count;
		char* p8;

		p8 = UTF16_to_UTF8(p0 + i, adv, count);
		i += adv;

		while (count != 0) {
			dest8.push_back(*p8++);
			count--;
		}
	}

	return dest8;
}




// wstring version
std::wstring
utf8_to_wstring(const std::string& str)
{
	std::wstring wstr;

	string_utf16 str16 = UTF8_to_UTF16_string(str);

	for (int i = 0; i < str16.size(); i++) {
		wstr += wchar_t(str16[i]);
	}

	return wstr;
}


