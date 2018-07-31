/**************************************************************************************************

Copyright (c) 2007-2014   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#include  <windows.h>
#include  "mtk_colors.h"



struct ini_color_tag {
	const char* szName;
	const char* szColor;
};


const ini_color_tag DefaultColor[COLOR_NUM] = {
	{ "NameObject",				"#00BFFF" },
	{ "NameSpecialObject",		"#FFFFDD" },
	{ "NameSatellite",			"#F0E68C" },
	{ "NameDwarfPlanet",		"#D8BFD8" },
	{ "NameSmallObject",		"#98FB98" },
	{ "NameSpacecraft",			"#A6A6B3" },
	{ "OrbitPlanet",			"#4682B4" },
	{ "OrbitSatellite",			"#8C916A" },
	{ "OrbitDwarfPlanet",		"#6666CB" },
	{ "OrbitSmallObject",		"#008B8B" },
	{ "ConstellationName",		"#AED75B" },
	{ "ConstellationLine",		"#778899" },
	{ "ConstellationBoundary",	"#434D56" },
	{ "Scale",					"#EB7847" },
	{ "ScaleText",				"#EBBE47" },
	{ "ScaleSquare",			"#00FFFF" },
	{ "ScaleSquareText",		"#EBBE47" },
	{ "LocationName",			"#AAAAAA" },
	{ "Date",					"#BCE9D0" },
	{ "DateRealTime",			"#F3CDB1" },
	{ "Oort",					"#87CEEB" },
	{ "QSO",					"#5099DE" },
};


static RGBf  vcol[COLOR_NUM];




//-------------
//  get color
//-------------
const RGBf&
get_color(int idx)
{
	return  vcol[idx];
}




//--------
//  load
//--------
void
load_colors_from_ini_file(const char* ini_file_path)
{
	const int buffer_size = 256;
	char str[buffer_size];

	for (int i=0; i<COLOR_NUM; i++) {
		const ini_color_tag& dc = DefaultColor[i];
		GetPrivateProfileString("Color", dc.szName, dc.szColor, str, buffer_size, ini_file_path);

		RGBf col;
		if (is_colorref(str))	{ col = get_color_from_colorref(str); }
		else					{ col = get_color_from_colorref(dc.szColor); }
		vcol[i] = col;
	}
}



//------------
//  Colorref
//------------
bool
is_colorref(const char* str)
{
	if (strlen(str) != 7)  return false;

	if (str[0]!='#')  return false;
	for (int i=1; i<=6; i++) {
		int c = str[i];
		if (!isdigit(c)) {
			if (!isupper(c) && !islower(c))  return false;
			char ch = char( toupper(c) );
			if (ch < 'A' || ch > 'F')  return false;
		}
	}

	return true;
}



RGBf
get_color_from_colorref(const char* str)
{
	if (!is_colorref(str))  return RGBf(0,0,0);

	int buf[6];
	for (int i=0; i<6; i++) {
		int c = str[i+1];
		if (isdigit(c))	{ buf[i] = c - '0'; }
		else			{ buf[i] = toupper(c) - 'A' + 10; }
	}

	float r, g, b;
	r = (16*buf[0] + buf[1])/255.0f; 
	g = (16*buf[2] + buf[3])/255.0f; 
	b = (16*buf[4] + buf[5])/255.0f;

	return RGBf(r, g, b);
}



bool
is_RGBAref(const char* str)
{
	if (strlen(str) != 9)  return false;

	if (str[0] != '#')  return false;
	for (int i = 1; i <= 8; i++) {
		int c = str[i];
		if (!isdigit(c)) {
			if (!isupper(c) && !islower(c))  return false;
			char ch = char(toupper(c));
			if (ch < 'A' || ch > 'F')  return false;
		}
	}

	return true;
}



RGBAf
get_RGBA_from_RGBAref(const char* str)
{
	if (!is_RGBAref(str))  return RGBAf(0, 0, 0, 1);

	int buf[8];
	for (int i = 0; i<8; i++) {
		int c = str[i + 1];
		if (isdigit(c))	{ buf[i] = c - '0'; }
		else			{ buf[i] = toupper(c) - 'A' + 10; }
	}

	float r, g, b, a;
	r = (16 * buf[0] + buf[1]) / 255.0f;
	g = (16 * buf[2] + buf[3]) / 255.0f;
	b = (16 * buf[4] + buf[5]) / 255.0f;
	a = (16 * buf[6] + buf[7]) / 255.0f;

	return RGBAf(r, g, b, a);
}




