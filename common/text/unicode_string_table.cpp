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
#include  "unicode_string_table.h"
#include  <tchar.h>
#include  "string_util.h"
#include  "key_value_file.h"




//--------
//  Init
//--------
void
unicode_string_table::clear()
{
	key_ID_map.clear();
	vStrings.clear();
	vKey.clear();
}



int
unicode_string_table::set_string(const std::string& key, const std::string& str)
{
	int ID;


	if ( !is_defined_key(key) ) {
		// 新規登録
		ID = vStrings.size();
		vStrings.push_back( str );
		key_ID_map[key] = ID;
		vKey.push_back(key);
	} else {
		// 上書き
		ID = key_ID_map[key];
		vStrings[ID] = str;
	}

	return  ID;
}




//-------------
//  Get / Set
//-------------
int
unicode_string_table::get_ID(const std::string& key)  const
{
	std::map<std::string, int>::const_iterator  it = key_ID_map.find(key);

	// 未定義のキー
	if (it == key_ID_map.end())  {
		return -1;
	}

	return  it->second;
}


const std::string&
unicode_string_table::get_string(const std::string& key)  const
{
	int ID = get_ID(key);

	// 未定義のキー
	if (ID < 0)  {
		return key;
	}
	
	return  vStrings[ID];
}





//--------
//  Load
//--------
bool
unicode_string_table::load_utf8(FILE* fp, const std::string& lang_id)
{
	std::vector<key_value> vKeyValue;

	if (load_key_value_file(fp, lang_id, vKeyValue)) {

		//--  Register key and value to string table
		for (int i = 0; i < vKeyValue.size(); i++) {
			const key_value kv = vKeyValue[i];
			set_string(kv.key, kv.value);
		}
	}
	else {
		return false;
	}

	return  true;
}

