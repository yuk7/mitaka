/**************************************************************************************************

Written by Tsunehiko Kato
Copyright (c) 2015   Tsunehiko Kato

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
#ifndef __UNICODE_STRING_TABLE_H__
#define __UNICODE_STRING_TABLE_H__
#include  <stdio.h>
#include  <vector>
#include  <map>
#include  <string>
#include  "unicode_string.h"




//-------------------------------
//  class  unocode_string_table
//-------------------------------
class unicode_string_table
{
	std::vector<std::string > vStrings;
	std::map<std::string, int>  key_ID_map;
	std::vector<std::string> vKey;


public:
	//--  Init
	void	clear();

	//--  Size
	int		size() const  { return vStrings.size(); }

	//--  Get
	int				get_ID(const std::string& key)  const;
	bool			is_defined_key(const std::string& key) const	{ return  (get_ID(key) >= 0); }
	const std::string&	get_string(const std::string& key)  const;
	const std::string&	get_string(int ID)  const				{ return vStrings[ID]; }
	const std::string&	get_key(int ID)     const				{ return vKey[ID]; }

	//--  Add
	int		set_string(const std::string& key, const std::string& str);

	//--  Load
	bool	load_utf8(FILE *fp, const std::string& lang_id);
};




#endif
