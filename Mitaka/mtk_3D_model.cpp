/**************************************************************************************************

Copyright (c) 2015   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#include  "mtk_3D_model.h"
#include  "../common/graphics/3ds_loader.h"
#include  "../common/graphics/obj_loader.h"
#include  "mtk_directory.h"
#include  <string>
#include  <vector>
#include  <map>




struct model_info {
	int			dir_idx;
	glModel3D	mdl;
};

//---
static std::map<std::string, int> ModelIdx;
static std::vector<model_info> v3DModels;




//------------------------
//  Load a 3D model file
//------------------------
bool
load_3D_model_file_sub(const char* filename, Model3D& mdl)
{
	//-- ファイルの種類
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath(filename, drive, dir, fname, ext);

	if (stricmp(ext, ".3ds") == 0) {
		return load_3DS_file(filename, mdl);
	}
	else if (stricmp(ext, ".obj") == 0) {
		return load_OBJ_file(filename, mdl);
	}

	return false;
}



int
load_3D_model_file(int dir_idx, const char* fn)
{
	// fn はファイル名のみ
	const directory& dir = get_directory(dir_idx);
	const char *filename = dir.get_path(fn);		// フルパスのファイル名



	const map<string, int>::const_iterator it = ModelIdx.find(string(filename));
	int idx;

	if (it == ModelIdx.end()) {	// まだ同名のモデルが読み込まれていない時
		model_info mi;
		idx = -1;

		if (load_3D_model_file_sub(filename, mi.mdl)) {
			mi.dir_idx = dir_idx;
			mi.mdl.fit_size(1.0);	//***
			v3DModels.push_back(mi);
			idx = v3DModels.size() - 1;
		}

		ModelIdx.insert(pair<string, int>(string(filename), idx));
	}
	else {
		idx = it->second;
	}

	return idx;
}



void
bind_3D_model_textures()
{
	for (int i = 0; i < v3DModels.size(); i++) {
		model_info &mi = v3DModels[i];

		mi.mdl.bind_textures(get_directory(mi.dir_idx));
	}
}


glModel3D*
get_3DModel(int idx)
{
	if (idx < 0) return 0;

	return &v3DModels[idx].mdl;
}
