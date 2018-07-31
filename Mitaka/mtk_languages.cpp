/******************************************************************************

Written by Tsunehiko Kato
Copyright (c) 2008-2009   Tsunehiko Kato

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
#include  "mtk_languages.h"
#include  "mtk_data_file.h"
#include  "../common/text/unicode.h"
#include  "../common/text/unicode_string.h"
#include  "../common/text/unicode_string_table.h"
#include  "../common/text/string_util.h"
#include  "mtk_strings.h"
#include  "mtk_font.h"
#include  "mtk_menu_bar.h"
#include  "mtk_end_roll.h"
#include  "mtk_title_page.h"


#include  <vector>
#include  <shlobj.h>



extern char szAppName[];



struct language
{
	std::string					name;
	std::string					name_key;
	std::string					iso639;	//*** 不要？
	std::string					lang_id;
	std::vector<std::string>	filepath_strings;
	std::vector<std::string>	font;
	std::string					caution_text_filename;
	std::string					title_image_filename;
	std::string					end_credits_filename;
};


static std::vector<language>  languages;
static int  lang_idx = 0;

static std::string  default_lang;



//-----------
//  Default
//-----------
void
set_default_language_ios639(const std::string& iso639_dsg)
{
	default_lang = iso639_dsg;
}




//--------------------
//  Language Setting
//--------------------
bool
load_language_settings(const directory& dir, const char* fn)
{
	language  lang;
	unicode_string_table ust;
	key_data kd;
	FILE* fp;

	fp = fopen_utf8( dir.get_path(fn), "rt" );
	if (fp == NULL)  return false;


	// STRINGS と FONT は1行に複数記述可。また、複数行にわたって書ける
	// それ以外は最初の要素のみ。また、複数回現れた場合は上書きする
	while (true) {
		kd = read_a_key_data(fp);
		if (kd.key == "") break;	// EOF

		if (kd.data.size() == 0) continue;

		const char* key = kd.key.c_str();
		if (stricmp(key, "NAME") == 0)			{ lang.name = kd.data[0]; }
		else if (stricmp(key, "NAME_KEY") == 0)	{ lang.name_key = kd.data[0]; }
		else if (stricmp(key, "ISO639") == 0)	{ lang.iso639 = kd.data[0]; }
		else if (stricmp(key, "LANG_ID") == 0) { lang.lang_id = kd.data[0]; }
		else if (stricmp(key, "CAUTION_TEXT") == 0)	{ lang.caution_text_filename = kd.data[0]; }
		else if (stricmp(key, "TITLE_IMAGE") == 0)	{ lang.title_image_filename = kd.data[0]; }
		else if (stricmp(key, "END_CREDITS") == 0)	{ lang.end_credits_filename = kd.data[0]; }
		else if (stricmp(key, "STRINGS") == 0) { 
			for (int i = 0; i < kd.data.size(); i++) {
				lang.filepath_strings.push_back(kd.data[i]);
			}
		}
		else if (stricmp(key, "FONT") == 0) {
			for (int i = 0; i < kd.data.size(); i++) {
				lang.font.push_back(kd.data[i]);
			}
		}
	}

	if (lang.lang_id == "") {
		lang.lang_id = lang.iso639;
	}

	lang.font.push_back("arialuni.ttf");	// Default font


	languages.push_back(lang);

	return  true;
}



int
load_language_files(const directory& dir)
{
	const int BUF_N = 1024;
	char  fn[BUF_N];
	FILE* fp;


	fp = fopen_utf8( dir.get_path("languages.dat"), "rt" );
	if (fp == NULL)  return 0;


	languages.clear();
	while ( true ) {
		char *p;
		p = fgets(fn, BUF_N, fp);
		if (p==NULL) break;
		if (fn[0] == '%') continue;			// コメント行はスキップする

		int len = strlen(fn);
		if (fn[len-1]=='\n') {
			fn[len-1] = '\0';
		}

		load_language_settings(dir, fn);
	}


	fclose(fp);
	return  languages.size();
}




//-----------
//  Get/Set
//-----------
int
get_num_languages()
{
	return  languages.size();
}


bool
select_language(const directory& dir, int idx)
{
	if (idx < 0 || idx >= languages.size())  return false;
	const language&  lang = languages[idx];


	set_current_directory();


	//---  Clear strings  ---
	clear_strings();



	//---  Set Freetype Face  ---//
	char  font_path[1024];

	free_font_face();

	SHGetFolderPath(NULL, CSIDL_FONTS, NULL, 0, font_path);
	init_font_face(font_path, lang.font, 0);




	//---  Register language name  ---
	for (int i=0; i<languages.size(); i++) {
		set_string(languages[i].name_key, languages[i].name);
	}


	//---  Register strings  ---
	const std::vector<std::string>& vstr = lang.filepath_strings;
	for (int i=0; i<vstr.size(); i++) {
		FILE* fp;

		const char *fn = vstr[i].c_str();
		fp = fopen_utf8( dir.get_path(fn), "rt" );
		
		if (fp != NULL) {
			load_strings(fp, lang.lang_id);
			fclose(fp);
		}
	}


	

	//---  Create Formatted Strings  ---//
	char str[512];
	

	//-- Exit dialog box
	_snprintf(str, 512, get_string("DLG_MB_EXIT_QUERY_FMT").c_str(), szAppName);
	set_string("DLG_MB_EXIT_QUERY", str);

	
	//-- About dialog box
	_snprintf(str, 512, get_string("MENUB_HELP_ABOUT_FMT").c_str(), szAppName);
	set_string("MENUB_HELP_ABOUT", str);
	_snprintf(str, 512, get_string("DLG_ABOUT_TITLE_FMT").c_str(), szAppName);
	set_string("DLG_ABOUT_TITLE", str);





	lang_idx = idx;



	
	//---  Title / Caution / End Credits  ---//
	load_caution_text( languages[idx].caution_text_filename.c_str() );
	load_title_image( languages[idx].title_image_filename.c_str() );
	load_end_credits( languages[idx].end_credits_filename.c_str() );
	


	return  true;
}



void
select_default_language(const directory& dir)
{
	for (int i=0; i<languages.size(); i++) {
		const language& lang = languages[i];
		if ( stricmp(lang.iso639, default_lang) == 0 ) {
			select_language(dir, i);
			return;
		}
	}

	select_language(dir, 0);
}




int
get_current_language()
{
	return  lang_idx;
}


const std::string&
get_language_id(int idx)
{
	return  languages[idx].lang_id;
}


const std::string&
get_current_language_id()
{
	return  languages[lang_idx].lang_id;
}



const std::string&
get_language_name_key(int idx)
{
	return  languages[idx].name_key;
}


bool
is_default_language_ISO639(const std::string& str)
{
	return  (stricmp(default_lang.c_str(), str.c_str()) == 0);
}


bool
is_current_language_ISO639(const std::string& str)
{
	return  (stricmp(languages[lang_idx].iso639.c_str(), str.c_str()) == 0);
}

