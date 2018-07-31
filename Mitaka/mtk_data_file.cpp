/**************************************************************************************************

Copyright (c) 2015   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#include  "mtk_data_file.h"
#include  "../common//text/string_util.h"




static std::vector<std::string>
get_token(FILE *fp, const std::string& delim, const std::string& trim_chars)
{
	const int buf_size = 1024;
	static char buf[buf_size];

	while (true) {
		std::vector<std::string> elems;
		char *p;

		p = fgets(buf, buf_size, fp);
		if (p == NULL) {
			// EOF
			return std::vector<std::string>();
		}

		elems = split2_trim(remove_cpp_comment(buf), delim, trim_chars);

		if (elems.size() > 1) {
			return elems;
		}
	}
}



key_data
read_a_key_data(FILE *fp)
{
	const std::string trim_chars = " \t\n";
	key_data kd;
	std::string buf;
	std::vector<std::string> elems;



	//--  key
	elems = get_token(fp, ":", trim_chars);

	if (elems.size() != 2) {
		// size < 2: EOF
		// size > 2: multiple keys in one line
		kd.key = std::string("");
		return kd;
	}

	kd.key = elems[0];
	buf = elems[1];



	//--  data
	if (buf.size() != 0) {
		// data exist in the first line
		kd.data = split2_trim(buf, ",", trim_chars);
		if (kd.data.back().size() == 0) {
			// continue data
			kd.data.pop_back();
		}
		else {
			// end data line
			return kd;
		}
	}

	while (true) {
		int sz;
		bool bContinue;

		elems = get_token(fp, ",", trim_chars);
		sz = elems.size();
		if (sz == 0)  break;



		if (elems.back().size() == 0) {
			bContinue = true;
			elems.pop_back();
		}
		else {
			bContinue = false;
		}

		// add data
		kd.data.insert(kd.data.end(), elems.begin(), elems.end());
		if (!bContinue) {
			break;
		}
	}


	return kd;
}
