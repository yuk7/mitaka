/******************************************************************************

Copyright (c) 2007,2014   Tsunehiko Kato

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
#include  "ctext.h"
#include  <ctype.h>


//==============
//  制御コマンド
//==============
// 数値オプションの種類:  [ , , ]
enum {
	CMDOPTNUM_NONE,			// オプションなし
	CMDOPTNUM_INT,			// 整数（複数可）
	CMDOPTNUM_FLOAT			// 浮動小数点数（複数可）
};




struct tagCTEXT_CTRL_CODE1 {
	const char* str;
	int			ID;
};

struct tagCTEXT_CTRL_CODE2 {
	char		ch;
	int			ID;
};



tagCTEXT_CTRL_CODE  CTEXT_CTRL_CODE[] = {
	{CCMD_ALIGN_LEFT, CMDOPTNUM_NONE, "Left"},
	{CCMD_ALIGN_CENTER, CMDOPTNUM_NONE, "Center"},
	{CCMD_ALIGN_RIGHT, CMDOPTNUM_NONE, "Right"},
	{CCMD_ALIGN_KINTOU, CMDOPTNUM_NONE, "Kintou"},
	{CCMD_COLOR, CMDOPTNUM_FLOAT, "Color"},
	{CCMD_SIZE, CMDOPTNUM_FLOAT, "Size"},
	{CCMD_RSIZE, CMDOPTNUM_FLOAT, "Rsize"},
	{CCMD_TABLE, CMDOPTNUM_FLOAT, "Table"},
	{CCMD_DELIMITER, CMDOPTNUM_NONE, "Delimiter"},
	{CCMD_HSPACE, CMDOPTNUM_FLOAT, "Hspace"},
	{CCMD_VSPACE, CMDOPTNUM_FLOAT, "Vspace"},
	{CCMD_SUBSCRIPT, CMDOPTNUM_NONE, "Subscript"},
	{CCMD_SUPERSCRIPT, CMDOPTNUM_NONE, "Superscript"}
};


// '\' で始まる制御コード（２文字目以降は、アルファベットのみの文字列か、それ以外の１文字）
tagCTEXT_CTRL_CODE1  CTEXT_CTRL_CODE1[] = {
	{"Left", CCMD_ALIGN_LEFT},
	{"Center", CCMD_ALIGN_CENTER},
	{"Right", CCMD_ALIGN_RIGHT},
	{"Kintou", CCMD_ALIGN_KINTOU},
	{"Color", CCMD_COLOR},
	{"Size", CCMD_SIZE},
	{"Rsize", CCMD_RSIZE},
	{"Hspace", CCMD_HSPACE},
	{"Vspace", CCMD_VSPACE},
	{"Table", CCMD_TABLE}
};

// '\' 以外で始まる制御コード（必ずアルファベット以外の１文字）
tagCTEXT_CTRL_CODE2  CTEXT_CTRL_CODE2[] = {
	{'&', CCMD_DELIMITER},
	{'_', CCMD_SUBSCRIPT},
	{'^', CCMD_SUPERSCRIPT}
};



// コマンド２の記号文字かどうかを調べる
bool
is_char_for_command2(char ch)
{
	const int n = sizeof(CTEXT_CTRL_CODE2) / sizeof(tagCTEXT_CTRL_CODE2);
	for (int i = 0; i<n; i++) {
		if (ch == CTEXT_CTRL_CODE2[i].ch)  return true;
	}

	return  false;
}

// 文字列以外に使われる特殊文字かどうか
bool
is_special_char(char ch)
{
	if (ch == '\\')		return  true;
	return  is_char_for_command2(ch);
}


// コマンドの配列番号を得る
int
get_command_index1(const char* str)
{
	const int n0 = sizeof(CTEXT_CTRL_CODE) / sizeof(tagCTEXT_CTRL_CODE);
	const int n1 = sizeof(CTEXT_CTRL_CODE1) / sizeof(tagCTEXT_CTRL_CODE1);

	for (int i = 0; i<n1; i++) {
		if (stricmp(str, CTEXT_CTRL_CODE1[i].str) == 0)  {
			int ID = CTEXT_CTRL_CODE1[i].ID;
			for (int j = 0; j<n0; j++) {
				if (ID == CTEXT_CTRL_CODE[j].ID)		return j;
			}
			return -1;
		}
	}

	return  -1;
}


// コマンド２の配列番号を得る
int
get_command_index2(char ch)
{
	const int n0 = sizeof(CTEXT_CTRL_CODE) / sizeof(tagCTEXT_CTRL_CODE);
	const int n2 = sizeof(CTEXT_CTRL_CODE2) / sizeof(tagCTEXT_CTRL_CODE2);

	for (int i = 0; i<n2; i++) {
		if (ch == CTEXT_CTRL_CODE2[i].ch)  {
			int ID = CTEXT_CTRL_CODE2[i].ID;
			for (int j = 0; j<n0; j++) {
				if (ID == CTEXT_CTRL_CODE[j].ID)		return j;
			}
			return -1;
		}
	}

	return  -1;
}



//=================
//  ctext_command
//=================

//--- コマンドのオプションを読み込む
bool
ctext_command::parse_command_options(const char*& p)
{
	const char optn_type = CTEXT_CTRL_CODE[idx].optn_type;

	if (*p != '[' || optn_type == CMDOPTNUM_NONE)  {	// オプションなし
		return true;
	}

	// オプション文字列を分解
	while (*p != ']') {
		if (*p == '\0')  {
			return false;	// 予期しない終了
		}

		p++;

		string elem;
		while (*p != ',' && *p != ']') {
			if (*p == ' ')  { p++; continue; }

			elem.push_back(*p++);
		}
		if (elem.size() == 0)  continue;

		int		count;
		switch (optn_type) {
		case CMDOPTNUM_INT:
		{
			int		value_int;
			count = sscanf(elem.c_str(), "%d", &value_int);
			if (count == 1)	opti.push_back(value_int);
		}
		break;
		case CMDOPTNUM_FLOAT:
		{
			float	value_float;

			const char* pp = elem.c_str();

			count = sscanf(elem.c_str(), "%f", &value_float);
			if (count == 1)  optf.push_back(value_float);
		}
		break;
		default:	count = 0;	break;
		}

		if (count != 1) {
			return 0;
		}
	}
	p++;

	return  p;
}


//--- コマンドを１つ読み取る（通常コマンド）
bool
ctext_command::parse_command1(const char*& p)
{
	p++;

	// コマンド文字列の読み込み
	string  cmd_str;
	if (isalpha(int(*p)) || *p == '_') {
		do {
			cmd_str.push_back(*p++);
		} while (isalpha(int(*p)) || *p == '_');
	}
	else {
		cmd_str.push_back(*p++);
	}

	// コマンド index の取得
	idx = get_command_index1(cmd_str.c_str());
	if (idx < 0)	return  false;

	return  parse_command_options(p);
}



//--- コマンドを１つ読み取る（1文字コマンド）
bool
ctext_command::parse_command2(const char*& p)
{
	idx = get_command_index2(*p);
	if (idx < 0)	return false;

	p++;
	return  parse_command_options(p);
}



//--- コマンドを１つ読み取る
bool
ctext_command::parse_command(const char*& p)
{
	bool res = false;
	if (*p == '\\')						{ res = parse_command1(p); }
	else if (is_char_for_command2(*p))	{ res = parse_command2(p); }

	if (!res)	return false;

	// オプションがなく、直後に空白があるときはそれを削除する
	if (*p == ' ' && opti.size() == 0 && optf.size() == 0) {
		p++;
	}

	return  true;
}




//=========
//  ctext
//=========

void
ctext::push_command(const ctext_command& cmd)
{
	int cmd_idx = vCommand.size();
	vCommand.push_back(cmd);

	ctext_code  code;
	code.type = CTEXTCODE_COMMAND;
	code.idx = cmd_idx;
	vCode.push_back(code);
}


//--- １つのコマンドを読み取る
bool
ctext::parse_command(const char*& p)
{
	ctext_command  cmd;
	if (!cmd.parse_command(p))	return  false;
	push_command(cmd);
	return  true;
}


//--- コメント：'\n' または '\0' が現れるまでスキップする
void
ctext::skip_comment(const char*& p)
{
	p++;
	while (*p != '\n' && *p != '\0') {
		p++;
	}
	if (*p == '\n')  p++;
}


//--- １つの文字列を読み取る
bool
ctext::parse_string(const char*& p)
{
	string  str;

	while (true) {
		//-- 文字列の終了
		if (*p == '}' || *p == '{' || *p == '|' || *p == '\0')  break;
		if (*p == '\\' && *(p + 1) == '\\')  break;
		if (is_special_char(*p))	break;

		//-- コメントをスキップする
		if (*p == '%')	{
			skip_comment(p);
			continue;
		}

		//-- 改行は無視する
		if (*p == '\n')  {
			if (*(p + 1) == '\n')  break;	// 連続した２つ以上の改行は改行コード
			p++;
			continue;
		}

		// 文字をストア
		str.push_back(*p++);
	};

	// 文字列の大きさが０でないとき登録する
	if (str.size() > 0) {
		int str_idx = vString.size();
		vString.push_back(str);

		ctext_code  code;
		code.type = CTEXTCODE_STRING;
		code.idx = str_idx;
		vCode.push_back(code);
	}
	return  true;
}


//--- １トークンを読み取る
bool
ctext::parse_token(const char*& p)
{
	//--- コマンドの処理
	if (*p == '\\' || is_char_for_command2(*p)) {
		return  parse_command(p);
	}

	//--- それ以外は文字列として処理する
	return  parse_string(p);
}


//--- １エレメントを読み取る
bool
ctext::parse_element(const char*& p)
{
	while (true) {
		//--- 新しいブロックの開始
		if (*p == '{') {
			ctext_code  code;
			code.idx = 0;
			code.type = CTEXTCODE_ENTER_BLOCK;
			vCode.push_back(code);

			block_lv++;
			p++;
			if (!parse_block(p))  return false;
			continue;
		}

		//--- 行の終了
		if (*p == '|' || *p == '}') {
			return  true;
		}
		if (*p == '\\' && *(p + 1) == '\\') {
			return  true;
		}

		if (*p == '\n' && *(p + 1) == '\n') {
			while (*(p + 1) == '\n') {
				p++;
			}
			return  true;
		}

		//--- 終端文字
		if (*p == '\0') {
			return  true;
		}

		//--- コマンドか文字列
		if (!parse_token(p))	return false;
	}
}


//--- １ブロックを読み取る
bool
ctext::parse_block(const char*& p)
{
	elem_no = 0;

	while (true) {
		if (!parse_element(p))	return false;

		//-- next line
		if (*p == '\n' || (*p == '\\' && *(p + 1) == '\\')) {
			ctext_code  code;
			code.idx = 0;
			code.type = CTEXTCODE_NEW_LINE;
			vCode.push_back(code);

			elem_no++;

			if (*p == '\n')	p++;
			else			p += 2;
			continue;
		}

		//-- next line (upward newline)
		if (*p == '|') {
			ctext_code  code;
			code.idx = 0;
			code.type = CTEXTCODE_UPWARD_NEW_LINE;
			vCode.push_back(code);

			ctext_command cmd;
			///----- align ----
			const char* szKintou = "\\Kintou";
			cmd.parse_command(szKintou);
			push_command(cmd);
			///----- size -----
			cmd.clear();
			const char* szSize = "\\Rsize[0.5]";
			cmd.parse_command(szSize);
			push_command(cmd);

			elem_no++;
			p++;
			continue;
		}

		//-- end of the block
		if (*p == '}') {
			if (block_lv == 0)  return  false;
			ctext_code  code;
			code.idx = 0;
			code.type = CTEXTCODE_LEAVE_BLOCK;
			vCode.push_back(code);
			block_lv--;
			p++;
			return  true;
		}

		//-- other
		if (*p == '\0' && block_lv == 0) {
			return  true;
		}
		return  false;
	}
}


//---
bool
ctext::append_text(const char* p)
{
	block_lv = 0;
	bool res;
	res = parse_block(p);
	p_last = p;
	return  res;
}


