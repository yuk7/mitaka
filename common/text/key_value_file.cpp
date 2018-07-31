/**************************************************************************************************

Written by Tsunehiko Kato
Copyright (c) 2016   Tsunehiko Kato

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
#include  "key_value_file.h"
#include  "string_util.h"





bool
is_target_specifier(std::string& str)
{
	int N = str.size();

	if (str[0] != '[' || str[N - 1] != ']') {
		return false;
	}

	str = str.substr(1, N - 2);

	return true;
}



bool
load_key_value(FILE* fp, key_value& kv)
{
	const int NBUF = 512;
	char buf[NBUF];

	// １行読み込み
	if (fgets(buf, NBUF, fp) == NULL) {
		return  false;
	}


	std::vector<std::string> elems;
	elems = split2(remove_cpp_comment(buf), ":");

	if (elems.size() < 2) {
		elems[0] = trim(elems[0], " \t\n");
		if (is_target_specifier(elems[0])) {
			kv.key = elems[0];
		}
		else {
			kv.key.clear();
		}
		return false;
	}
	else if (elems.size() > 2) {
		for (int i = 2; i < elems.size(); i++) {
			elems[1] += ":" + elems[i];
		}
	}

	kv.key = trim(elems[0], " \t\n");
	kv.value = trim(elems[1], " \t\n");
	kv.value = trim(kv.value, "\"");

	return  true;
}



void
set_target_specifier(const std::string& str, std::vector<std::string>& spcf)
{
	std::vector<std::string> elems;
	elems = split2_trim(str, ",", " \t\n");

	spcf.clear();
	for (int i = 0; i < elems.size(); i++) {
		if (elems[i] != "") {
			if (stricmp(elems[i].c_str(), "default") == 0) {
				spcf.clear();
				return;
			}

			spcf.push_back(elems[i]);
		}
	}
}



bool
load_target_specifier(FILE* fp, std::vector<std::string>& spcf)
{
	const int NBUF = 512;
	char buf[NBUF];

	// １行読み込み
	if (fgets(buf, NBUF, fp) == NULL) {
		return  false;
	}


	std::string str = trim(remove_cpp_comment(buf), " \t\n");
	if (!is_target_specifier(str)) {
		return false;
	}

	set_target_specifier(str, spcf);

	return  true;
}



bool
is_in_target_specifier(const std::string& target_id, const std::vector<std::string>& target_specifier)
{
	if (target_specifier.size() == 0) {
		return true;
	}

	for (int i = 0; i < target_specifier.size(); i++) {
		if (target_id == target_specifier[i]) {
			return true;
		}
	}

	return false;
}



bool
load_key_value_file(FILE* fp, const std::string& target_id, std::vector<key_value>& vKeyValue)
{
	std::vector<std::string> target_specifier;
	bool bTarget;

	bTarget = is_in_target_specifier(target_id, target_specifier);
	vKeyValue.clear();
	while (true) {
		if (bTarget) {
			key_value kv;

			//--  Load key and value
			if (!load_key_value(fp, kv)) {
				if (feof(fp) != 0)  break;

				if (kv.key != "") {
					set_target_specifier(kv.key, target_specifier);
					bTarget = is_in_target_specifier(target_id, target_specifier);
				}

				continue;
			}

			//--  Register key and value
			vKeyValue.push_back(kv);
		}
		else {
			if (!load_target_specifier(fp, target_specifier)) {
				if (feof(fp) != 0)  break;
				continue;
			}
			bTarget = is_in_target_specifier(target_id, target_specifier);
		}
	}

	return  true;
}


