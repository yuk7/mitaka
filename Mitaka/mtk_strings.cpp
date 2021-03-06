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
#include  <stdio.h>
#include  <string.h>
#include  "mtk_strings.h"
#include  "../common/text/unicode_string_table.h"
#include  "mtk_font.h"



static unicode_string_table vStrings;





//----------------
//  Clear / Load
//----------------
void
clear_strings()
{
	vStrings.clear();
}


bool
load_strings(FILE *fp, const std::string& lang_id)
{
	return vStrings.load_utf8(fp, lang_id);
}







//-------------
//  Get / Set
//-------------
int
set_string(const std::string& key, const std::string& str)
{
	return vStrings.set_string(key, str);
}


int
get_string_ID(const std::string& key)
{
	return vStrings.get_ID(key);
}


bool
is_defined_key(const std::string& key)
{
	return vStrings.is_defined_key(key);
}


const std::string&
get_string(int ID)
{
	return vStrings.get_string(ID);
}


const std::string&
get_string(const std::string& key)
{
	return vStrings.get_string(key);
}


const std::string&
get_string_key(int ID)
{
	return vStrings.get_key(ID);
}

std::wstring
get_wstring(const std::string& key)
{
	return utf8_to_wstring(get_string(key));
}




//
//
//
std::string
apply_null_character(const std::string& str)
{
	std::string str2;


	for (int i = 0; i < str.size(); i++) {
		char ch;
		ch = str[i];

		if (ch == '\\' && (i < str.size()-1) && str[i+1]=='0') {
			str2 += '\0';
			i++;
		}
		else {
			str2 += ch;
		}
	}

	return str2;
}


