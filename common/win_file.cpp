/******************************************************************************

Copyright (c) 2007-2015   Tsunehiko Kato

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
#include  "win_file.h"
#include  <stdlib.h>
#include  <string.h>
#include  <string>





//-------------------
//  Open UTF-8 file
//-------------------
FILE*
fopen_utf8(const char* fn, const char* mode)
{
	FILE *fp;

	fp = fopen(fn, mode);
	if (fp == NULL)  return NULL;

	unsigned char buf[3];
	fread(buf, 1, 3, fp);

	// BOM
	if (buf[0] == 0xef && buf[1] == 0xbb && buf[2] == 0xbf) {
		return  fp;
	}

	fseek(fp, 0, SEEK_SET);
	return  fp;
}




//--------------
//  ディレクトリ
//--------------
void
directory::set(const char* _dir)
{
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath(_dir, drive, dir, fname, ext);
	if ( strlen(fname) > 0) {
		char _dir2[_MAX_PATH];
		sprintf(_dir2, "%s\\", _dir);
		_splitpath(_dir2, drive, dir, fname, ext);
	}
}

const char*
directory::get_path(const char* filename)  const
{
	static char path[_MAX_PATH];
	char _dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	
	_splitpath(filename, NULL, _dir, fname, ext);

	std::string dir2 = std::string(dir) + std::string(_dir);

	_makepath(path, drive, dir2.c_str(), fname, ext);

	return path;
}

FILE*
directory::fopen(const char* filename, const char* mode)  const
{
	return  ::fopen( get_path(filename), mode );
}


FILE*
directory::fopen_utf8(const char* filename, const char* mode)  const
{
	return  ::fopen_utf8(get_path(filename), mode);
}
