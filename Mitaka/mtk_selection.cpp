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

上記の著作権表示および本許諾表示を、ソフトウェアのすべての複製または重要な部分に記載するものとします。

ソフトウェアは「現状のまま」で、明示であるか暗黙であるかを問わず、何らの保証もなく提供されます。ここ
でいう保証とは、商品性、特定の目的への適合性、および権利非侵害についての保証も含みますが、それに限定
されるものではありません。 作者または著作権者は、契約行為、不法行為、またはそれ以外であろうと、ソフ
トウェアに起因または関連し、あるいはソフトウェアの使用またはその他の扱いによって生じる一切の請求、損
害、その他の義務について何らの責任も負わないものとします。
***************************************************************************************************/
#include  "mtk_selection.h"
#include  <set>
#include  <string>
#include  "../common/text/string_util.h"




static std::set<std::string>  selection;





void
clear_selection()
{
	selection.clear();
}


bool
load_selection_sub(FILE* fp)
{
	while (true) {
		static char buf[1024];

		if (fgets(buf, 1024, fp) == NULL) {
			if (feof(fp) != 0)  break;
			continue;
		}
		
		std::string  key = remove_cpp_comment(buf);
		key = trim(key, " \t\n");
		if (key.size() == 0) {
			continue;
		}
		
		//--  Register
		selection.insert(key);
	}

	return  true;
}




bool
load_selection(const directory& dir)
{
	FILE* fp;

	fp = fopen(dir.get_path("selection.dat"), "rt");
	if (fp == NULL)  return false;

	bool res;
	res = load_selection_sub(fp);
	fclose(fp);

	return res;
}




bool
is_selected(const std::string& key)
{
	return  (selection.find(key) != selection.end());
}


void
set_selection(const std::string& key, bool flg)
{
	if (flg) {
		selection.insert(key);
	} else{
		selection.erase(key);
	}
}
