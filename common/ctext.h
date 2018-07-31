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
#ifndef  __CTEXT_H__
#define  __CTEXT_H__
#include  <vector>
#include  <string>
using namespace std;



//==============
//  制御コマンド
//==============
struct tagCTEXT_CTRL_CODE {
	int			ID;
	char		optn_type;
	const char	*name;
};

extern tagCTEXT_CTRL_CODE  CTEXT_CTRL_CODE[];


enum {
	CCMD_NULL,
	CCMD_ALIGN_LEFT,
	CCMD_ALIGN_CENTER,
	CCMD_ALIGN_RIGHT,
	CCMD_ALIGN_KINTOU,
	CCMD_COLOR,
	CCMD_SIZE,
	CCMD_RSIZE,			// 相対サイズ
	CCMD_TABLE,
	CCMD_DELIMITER,
	CCMD_HSPACE,
	CCMD_VSPACE,
	CCMD_SUBSCRIPT,
	CCMD_SUPERSCRIPT,
	CCMD_CDOT,
	CCMD_COPYRIGHT
};


//-----------
//  文字揃え
//-----------
enum {
	CTEXT_ALIGN_LEFT,
	CTEXT_ALIGN_CENTER,
	CTEXT_ALIGN_RIGHT,
	CTEXT_ALIGN_KINTOU
};


//==============
//  ctext_code
//==============
enum
{
	CTEXTCODE_NULL,				// 何もなし
	CTEXTCODE_COMMAND,			// コマンド
	CTEXTCODE_STRING,			// 文字列
	CTEXTCODE_ENTER_BLOCK,		// ブロックに入る
	CTEXTCODE_LEAVE_BLOCK,		// ブロックから出る
	CTEXTCODE_NEW_LINE,			// 次の行へ移る
	CTEXTCODE_UPWARD_NEW_LINE	// 次の行へ移る（上側に改行）
};


struct ctext_code {
	char	type;		// コマンドまたは文字列
	int		idx;		// 配列番号
};


//=================
//  ctext_command
//=================
class  ctext_command
{
public:
	int				idx;		// コマンド index
	vector<int>		opti;		// オプションの整数値
	vector<float>	optf;		// オプションの浮動小数点数値
public:
	ctext_command()  {};
	void	clear()	{ idx = -1; opti.clear(); optf.clear(); }
	bool	parse_command(const char*& p);
private:
	bool	parse_command_options(const char*& p);
	bool	parse_command1(const char*& p);
	bool	parse_command2(const char*& p);
};



//=========
//  ctext
//=========
class ctext
{
private:
	int		block_lv;
	int		elem_no;
public:
	vector<ctext_code>		vCode;
	vector<ctext_command>	vCommand;
	vector<string>			vString;
	const char*	p_last;
public:
	ctext()	{};
	ctext(const char*p)	{ set_text(p); }
	void	clear()		{ vCode.clear(); vString.clear(); vCommand.clear(); }
	bool	append_text(const char* p);
	bool	set_text(const char* p)		{ clear(); return append_text(p); }
private:
	void	push_command(const ctext_command& cmd);
	bool	parse_command(const char*& p);
	bool	parse_string(const char*& p);
	bool	parse_token(const char*& p);
	bool	parse_element(const char*& p);
	bool	parse_block(const char*& p);
	void	skip_comment(const char*& p);
};



#endif