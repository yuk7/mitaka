/**************************************************************************************************

Copyright (c) 2007-2015   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#ifndef  __MTK_MENU_H__
#define  __MTK_MENU_H__
#include  <string.h>
#include  <typeinfo.h>
#include  <string>
#include  <vector>
#include  <assert.h>




//-------------------
//  メニュー項目の種類
//-------------------
enum menu_item_type {
	MENU_TY_BASE,
	MENU_TY_CHILD,
	MENU_TY_COMMAND,
	MENU_TY_CHECK,
	MENU_TY_FVALUE
};


//------------------------
//  メニュー項目の基本クラス
//------------------------
class menu;

class menu_item_base
{
	std::string name_key;

public:
	menu_item_base()										{}
	menu_item_base(const std::string& key)	{ set_name_key(key); }
	~menu_item_base()										{} 

	void	set_name_key(const std::string& key) {
		name_key = key;
	}

	virtual	menu_item_type	get_type() const = 0;
	
	const std::string& get_name_key() const	{ return name_key; }
};



//------------
//  子メニュー
//------------
class menu_item_child  :  public menu_item_base
{
	menu* pChildMenu;
public:
	menu_item_child(const std::string& key, menu* pMenu) : menu_item_base(key), pChildMenu(pMenu)  {};

	menu_item_type	get_type() const					{ return MENU_TY_CHILD; }
	void			set_child_menu(menu* pMenu)			{ pChildMenu = pMenu; }
	menu*			get_child_menu()					{ return pChildMenu; }
};


//-----------
//  コマンド
//-----------
class menu_item_command  :  public menu_item_base
{
	int command_ID;
	int	idx;

public:
	menu_item_command(const std::string& key, int cmdID, int _idx) : menu_item_base(key), command_ID(cmdID), idx(_idx)  {};

	menu_item_type	get_type() const			{ return MENU_TY_COMMAND; }
	void			set_ID(int ID)				{ command_ID = ID; }
	int				get_ID() const				{ return command_ID; }
	void			set_idx(int _idx)			{ idx = _idx; }
	int				get_idx() const				{ return idx; }
};


//----------
//  チェック
//----------
class menu_item_check  :  public menu_item_base
{
	bool	bCheck;
	int		check_ID;
	int		idx;

public:
	menu_item_check(const std::string& key, bool bChk, int ID, int _idx) : menu_item_base(key), bCheck(bChk), check_ID(ID), idx(_idx)  {};

	menu_item_type	get_type() const				{ return MENU_TY_CHECK; }
	void			set_check(bool bChk)			{ bCheck = bChk; }
	bool			get_check() const				{ return bCheck; }
	void			set_ID(int ID)					{ check_ID = ID; }
	int				get_ID() const					{ return check_ID; }
	void			set_idx(int _idx)				{ idx = _idx; }
	int				get_idx() const					{ return idx; }
};


//--------------
//  浮動小数点値
//--------------
class menu_item_fvalue  :  public menu_item_base
{
	float	value;
public:
	menu_item_fvalue(const std::string& key, const float& val)  :  menu_item_base(key), value(val)  {};

	menu_item_type	get_type() const				{ return MENU_TY_FVALUE; }
	void			set_value(const float& val)		{ value = val; }
	float			get_value() const				{ return value; }
};


//--------------
//  class menu
//--------------
class menu  :  public  std::vector<menu_item_base*>
{
	menu* pParentMenu;
	std::string  title_key;
	
	int current_pos;
public:
	menu() : current_pos(0)  {};
	~menu()	{ free(); }
	void	free();

	//---
	void	add_child_menu(menu* pChildMenu, const std::string& key)	{
		this->push_back(new menu_item_child(key, pChildMenu) );
	}
	void	add_command(int command_id, int idx, const std::string& key) {
		this->push_back( new menu_item_command(key, command_id, idx) );
	}
	void	add_check(int check_id, const std::string& key, int idx, bool bInitCheck) {
		this->push_back( new menu_item_check(key, bInitCheck, check_id, idx) );
	}

	void	set_current_pos(int pos)	{ current_pos = pos; assert(pos>=0); }
	int		get_current_pos() const		{ return current_pos; }
	virtual void set_current_pos_auto()		{ set_current_pos(0); }

	void	set_title(const std::string& key)	{
		title_key = key;
	}

	const std::string& get_title_key() const	{ return  title_key; }
	void	set_check(int idx, bool bChk);
};


//---------------------
//  class menu_option
//---------------------
class menu_option  :  public menu
{
	int  check_pos;

public:
	menu_option() : check_pos(0)  {};
	menu_option(int idx) : check_pos(idx)  {};
	~menu_option() { menu::free(); }

	void set_current_pos_auto()	{
		assert(check_pos >= 0);
		set_current_pos(check_pos);
	}

	void	set_check(int idx)
		{
			assert(idx >= 0);
			check_pos = idx;
			int i=0;
			for (vector<menu_item_base*>::iterator it=this->begin(); it!=this->end(); ++it, ++i) {
				menu_item_check*  mi = dynamic_cast<menu_item_check*>(*it);
				if (mi != 0) {
					mi->set_check(i==idx);
				}
			}
		}

	void	set_check_by_index(int idx)
		{
			assert(idx >= 0);
			check_pos = 0;
			int i = 0;
			for (vector<menu_item_base*>::iterator it=this->begin(); it!=this->end(); ++it, ++i) {
				menu_item_check*  mi = dynamic_cast<menu_item_check*>(*it);
				if (mi != 0) {
					int  miidx = mi->get_idx();
					mi->set_check( miidx==idx );
					if (miidx == idx) {
						check_pos = i;
					}
				}
			}
		}


	int		get_check() const	{ return check_pos; }

	void	reset_check()	{
		for (vector<menu_item_base*>::iterator it=this->begin(); it!=this->end(); ++it) {
			menu_item_check*  mi = dynamic_cast<menu_item_check*>(*it);
			mi->set_check(false);
		}
	}
};


//------------------
//  menu selection
//------------------
class  menu_selection  :  public  std::vector<menu*>
{};



#endif
