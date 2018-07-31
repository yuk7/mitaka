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
#include  "string_util.h"




std::vector<std::string>
split(const std::string& str, const std::string& delim)
{
	std::vector<std::string> elems;
	std::string elem;

	std::string::size_type pos1, pos2;

	pos1 = 0;
	while ((pos2 = str.find_first_of(delim, pos1)) != str.npos)
	{
		elem = str.substr(pos1, pos2 - pos1);
		elems.push_back(elem);
		pos1 = pos2 + 1;
	}

	elem = str.substr(pos1, pos2 - pos1);
	elems.push_back(elem);

	return elems;
}



std::vector<std::string>
split2(const std::string& str, const std::string& delim)
{
	std::vector<std::string> elems;
	std::string elem;
	bool bLiteral;
	char quote = '\0';


	bLiteral = false;
	for (int i = 0; i < str.size(); i++) {
		char ch = str[i];

		if (!bLiteral) {
			// outside quotation
			if (delim.find_first_of(ch) != delim.npos) {
				elems.push_back(elem);
				elem.clear();
			}
			else if (ch == '\"') {
				quote = ch;
				bLiteral = true;
			}
			else {
				elem += ch;
			}
		}
		else {
			// inside quotation
			if (ch == quote) {
				// exit quotation
				bLiteral = false;
			}
			else {
				elem += ch;
			}
		}
	}

	elems.push_back(elem);
	return elems;
}



std::string
trim(const std::string& str, const std::string& trim_chars)
{
	std::string::size_type  pos1, pos2;

	pos1 = str.find_first_not_of(trim_chars);
	if (pos1 == str.npos) {
		return std::string("");
	}

	pos2 = str.find_last_not_of(trim_chars);	// pos2 != npos  if pos1 != npos  and pos2 >= pos1
	return  str.substr(pos1, pos2 - pos1 + 1);
}



void
trim_all(std::vector<std::string>& elems, const std::string& trim_chars)
{
	for (std::vector<std::string>::iterator it = elems.begin(); it != elems.end(); it++) {
		*it = trim(*it, trim_chars);
	}
}



std::vector<std::string>
split_trim(const std::string& str, const char* delim, const std::string& trim_chars)
{
	std::vector<std::string> elems;

	elems = split(str, delim);
	trim_all(elems, trim_chars);

	return elems;
}



std::vector<std::string>
split2_trim(const std::string& str, const std::string& delim, const std::string& trim_chars)
{
	std::vector<std::string> elems;

	elems = split2(str, delim);
	trim_all(elems, trim_chars);

	return elems;
}





std::string
remove_cpp_comment(const std::string& str)
{
	bool bLiteral;
	char quote = '\0';


	bLiteral = false;
	for (int i = 0; i < str.size() - 1; i++) {
		char ch = str[i];

		if (!bLiteral) {
			if (ch == '/' && str[i + 1] == '/') {
				return  str.substr(0, i);
			}

			if (ch == '\"') {
				quote = ch;
				bLiteral = true;
			}
		}
		else if (ch == quote) {
			// exit quotation
			bLiteral = false;
		}
	}

	// no comment
	return str;
}
