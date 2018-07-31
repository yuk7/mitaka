/**************************************************************************************************

Copyright (c) 2007-2015 Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#include  "mtk_places.h"
#include  "mtk_data_file.h"
#include  <map>





//--------------
//  Place Type
//--------------
struct  PlaceType
{
	float sz;		// size
	float R1, R2;	// radius for display criterion
	RGBAf col1, col2;
};




//-- Objects
static std::map<std::string, Places> vPlaces;









//--------
//  Load
//--------
static void
add_place(const std::string& obj_key, const PlaceItem& pli)
{
	if (obj_key == "") {
		return;
	}

	vPlaces[obj_key].add_place(pli);
}


bool
load_places(const char *fn)
{
	std::vector<PlaceType> vPty;	// 地名の表示タイプ
	std::string  obj_key;
	FILE *fp;


	fp = fopen(fn, "rt");
	if (fp == NULL)  return false;


	vPty.clear();
	vPlaces.clear();


	while (true) {
		key_data kd;

		kd = read_a_key_data(fp);
		if (kd.key == "") {
			break;
		}

		if (kd.key == "OBJECT") {
			//--  Object  --
			obj_key = kd.data[0];
		}
		else if (kd.key == "TYPE") {
			//--  Type  --
			PlaceType pt;
			int type_no;
			const char *p;

			type_no = stoi(kd.data[0]);		// type no
			pt.sz = stof(kd.data[1]);		// size
			pt.R1 = stof(kd.data[2]);		// fade radius R1
			pt.R2 = stof(kd.data[3]);		// fade radius R2

			// col1
			p = kd.data[4].c_str();
			if (!is_RGBAref(p))  { return false; }
			pt.col1 = get_RGBA_from_RGBAref(p);

			// col2
			p = kd.data[5].c_str();
			if (!is_RGBAref(p))  { return false; }
			pt.col2 = get_RGBA_from_RGBAref(p);

			if (type_no < vPty.size()) {
				vPty[type_no] = pt;
			}
			else {
				while (vPty.size() <= type_no) {
					vPty.push_back(pt);
				}
			}
		}
		else {
			//--  Place data  --
			PlaceItem  pli;
			pli.key = kd.key;			// key
			pli.lon = stof(kd.data[0]);	// lon
			pli.lat = stof(kd.data[1]);	// lat
			pli.ty  = stoi(kd.data[2]);	// type
			pli.align = kd.data[3][0];	// align

			if (pli.ty >= vPty.size()) {
				return false;
			}

			PlaceType& pt = vPty[pli.ty];
			pli.size = pt.sz;
			pli.R1 = pt.R1;
			pli.R2 = pt.R2;
			pli.col1 = pt.col1;
			pli.col2 = pt.col2;
			pli.bDispLabel = true;

			add_place(obj_key, pli);
		}

	}


	return true;
}



Places*
get_places(const std::string& obj_key)
{
	std::map<std::string, Places>::iterator it = vPlaces.find(obj_key);

	if (it != vPlaces.end()) {
		return &it->second;
	}

	return 0;
}
