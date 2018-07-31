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
#include  <ctype.h>
#include  <string.h>
#include  "star_name.h"


//----------------
//  Greek letter
//----------------
const static char GreekLetter[][4] = {
	"Alp", "Bet", "Gam", "Del",
	"Eps", "Zet", "Eta", "The",
	"Iot", "Kap", "Lam", "Mu",
	"Nu",  "Xi",  "Omi", "Pi",
	"Rho", "Sig", "Tau", "Ups",
	"Phi", "Chi", "Psi", "Ome"
};

const static char GreekLetter_J[][4] = {
	"α", "β", "γ", "δ",
	"ε", "ζ", "η", "θ",
	"ι", "κ", "λ", "μ",
	"ν", "ξ", "ο", "π",
	"ρ", "σ", "τ", "υ",
	"φ", "χ", "ψ", "ω"
};


char
get_Greek_ID(const char* str) {
	for (char i=0; i<24; i++) {
		if (stricmp(str, GreekLetter[i])==0)  return i;
	}
	return GREEK_NONE;
}

const char*
get_Greek_letter(const char GreekID) {
	if (GreekID >= 24)  return 0;
	return  GreekLetter[GreekID];
}

const char*
get_Greek_letter_J(const char GreekID) {
	if (GreekID >= 24)  return 0;
	return  GreekLetter_J[GreekID];
}


//---------------------------------
//  Bayer Name / Framsteed Number
//---------------------------------
inline const char*
skip_white_space(const char* p)
{
	if (*p == '\0')  return p;

	char ch;
	while ((ch=*p)== ' ' || ch=='\n' || ch=='\t')  { p++; }
	return p;
}


//-- 指定した文字列のバイエル名のIDを求める
char
get_Bayer_ID(const char* str, char* BayerSubNo)
{
	*BayerSubNo = -1;

	const char* p = str;
	p = skip_white_space(p);	// 空白をスキップ

	//--- Bayer ID
	char buf[4];
	int count=0;
	while ( count<3 && isalpha(*p) )  { buf[count]=*p; p++; count++; }
	if (count != 2 && count != 3)  return -1;	// ギリシャ文字の略符は２文字か３文字

	buf[count] = '\0';
	char BayerID = get_Greek_ID(buf);
	if (BayerID == GREEK_NONE)  return -1;
	p = skip_white_space(p);	// 空白をスキップ
	if (*p=='\0')  return BayerID;
	
	//--- Bayer Sub Number
	*BayerSubNo = (isdigit(*p)) ? *p-'0' : -1;

	//--- あとに何も無いことをチェックする
	p++;
	p = skip_white_space(p);
	if (*p != '\0')  return -1;

	return BayerID;
}


//-- 指定した文字列のフラムスチード番号を得る
short
get_Framsteed_No(const char* str)
{
	const char* p = str;
	p = skip_white_space(p);	// 空白をスキップ

	//--- Framsteed Number
	char buf[4];
	int count = 0;
	while (count<3 && isdigit(*p)) { buf[count]=*p; p++; count++; }
	if (count==0)  return -1;

	buf[count] = '\0';
	short Framsteed;
	sscanf(buf, "%hd", &Framsteed);
	if (*p=='\0')  return Framsteed;

	//--- あとに何も無いことをチェックする
	p++;
	p = skip_white_space(p);
	if (*p != '\0')  return -1;

	return Framsteed;
}

