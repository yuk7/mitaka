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
#include  "gl_texture.h"
#define GL_CLAMP_TO_EDGE 0x812F


// すでに読み込まれているか
bool
gl_textures::is_loaded(const char* name) const
{
	map<string, GLuint>::const_iterator  it = textures.find(string(name));
	if (it != textures.end()) {
		return true;		// すでに同じファイル名のテクスチャがある場合
	}
	return false;
}


//
GLuint
gl_textures::get_texture_name(const char* name)  const
{
	map<string, GLuint>::const_iterator  it = textures.find(string(name));
	if (it != textures.end()) {
		return it->second;	// すでに同じファイル名のテクスチャがある場合
	}
	return 0;
}


GLuint
gl_textures::add_texture(const char* name, int shrink_rate, bool bRepeatY)
{
	GLuint  texname;
	// すでにロードされている場合はその値を返す
	texname = get_texture_name(name);
	if (texname!=0)  { return texname; }
	// ロードされていない場合は新たにロード
	glGenTextures(1, &texname);
	// ファイルのロード
	image img;
	if (!load(name, img)) {
		// ロードに失敗
		glDeleteTextures(1, &texname);
		return 0;
	}
	// バインド
	GLint RepeatY = (bRepeatY) ? GL_REPEAT : GL_CLAMP_TO_EDGE;
	glBindTexture(GL_TEXTURE_2D, texname);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, RepeatY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	gl_set_texture_mipmaps(img, shrink_rate);
	img.free();
	if (glIsTexture(texname)==GL_TRUE) {
		// 新しくテクスチャを生成できた時
		pair<string, GLuint>  pr;
		pr.first = string(name);
		pr.second = texname;
		textures.insert(pr);
		return  texname;
	}
	// テクスチャの生成に失敗
	return  0;
}


void
gl_textures::delete_texture(const char* name)
{
	GLuint  texname;
	// ロードされていない場合は何もせずリターン
	texname = get_texture_name(name);
	if (texname==0)		return;
	glDeleteTextures(1, &texname);
}

void
gl_textures::delete_texture_name(GLuint texname)
{
	if (texname == 0)  return;
	for (map<string, GLuint>::iterator  it = textures.begin(); it != textures.end(); it++) {
		if (it->second == texname) {
			glDeleteTextures(1, &it->second);
			textures.erase(it);
			return;
		}
	}
}

bool
gl_textures::load(const char* name, image& img)
{
	if (!img.load( dir.get_path(name)) )	{
		return  false;
	}
	return  true;
}
