/**************************************************************************************************

Copyright (c) 2016   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#include  "mtk_location.h"
#include  <stdio.h>
#include  "../common/win_file.h"
#include  "../common/text/string_util.h"
#include  "../common/text/key_value_file.h"
#include  "mtk_strings.h"
#include  "mtk_directory.h"




static location initital_location;
const location default_location = { 139.54125, 35.6725, "Mitaka" };



void
set_initial_location(const location& loc)
{
	initital_location = loc;

	// 
	set_string("LANDING_POS_NAME", initital_location.name);
	

	// Set menu strings
	char str[512];

	// Screen menu
	_snprintf(str, 512, get_string("MENU_LANDING_POS_NAME_FMT").c_str(), get_string("LANDING_POS_NAME").c_str());
	set_string("MENU_LANDING_POS_NAME", str);
	// Menu bar
	_snprintf(str, 512, get_string("MENUB_LANDING_POS_NAME_FMT").c_str(), get_string("LANDING_POS_NAME").c_str());
	set_string("MENUB_LANDING_POS_NAME", str);
}



bool
load_initial_location(const char *fn, const std::string& lang_id)
{
	directory  dir = get_directory(DIR_LOCALE);

	FILE* fp = dir.fopen_utf8(fn, "rt");
	if (fp == NULL)  return false;


	std::vector<key_value> vKeyValue;
	bool res = load_key_value_file(fp, lang_id, vKeyValue);
	fclose(fp);

	if (!res) {
		set_initial_location(default_location);
		return false;
	}



	location loc;
	bool bLon, bLat, bName;

	bLon = bLat = bName = false;
	for (int i = 0; i < vKeyValue.size(); i++) {
		const key_value& kv = vKeyValue[i];


		if (stricmp(kv.key, "LONGITUDE") == 0) {
			int count = sscanf(kv.value.c_str(), "%e", &loc.lon);
			if (count == 1) {
				bLon = true;
			}
		}
		else if (stricmp(kv.key, "LATITUDE") == 0) {
			int count = sscanf(kv.value.c_str(), "%e", &loc.lat);
			if (count == 1) {
				bLat = true;
			}
		}
		else if (stricmp(kv.key, "NAME") == 0) {
			loc.name = kv.value;
			if (loc.name != "") {
				bName = true;
			}
		}
		else {
			continue;
		}
	}

	if (!bLon || !bLat || !bName) {
		set_initial_location(default_location);
		return false;
	}


	set_initial_location(loc);
	

	return true;
}


const location&
get_initial_location()
{
	return initital_location;
}