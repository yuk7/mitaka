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
#include  "Model3D.h"


//============
//  Object3D
//============
bbox3f
Object3D::get_bounding_box() const
{
	bbox3f  bb;
	const int imax = get_vtx_num();
	for (int i=0; i<imax; ++i) {
		bb.add_point( get_vtx_pos(i) );
	}
	return bb;
}



//============
//  3D Model
//============
int
Model3D::get_total_vtx_num() const
{
	int vtx_num = 0;
	for (vector<Object3D>::const_iterator it=vObject.begin(); it!=vObject.end(); ++it) {
		vtx_num += it->get_vtx_num();
	}
	return  vtx_num;
}

int
Model3D::get_total_normal_num() const
{
	int normal_num = 0;
	for (vector<Object3D>::const_iterator it=vObject.begin(); it!=vObject.end(); ++it) {
		normal_num += it->get_normal_num();
	}
	return  normal_num;
}

int
Model3D::get_total_uv_coord_num() const
{
	int uv_coord_num = 0;
	for (vector<Object3D>::const_iterator it=vObject.begin(); it!=vObject.end(); ++it) {
		uv_coord_num += it->get_uv_coord_num();
	}
	return  uv_coord_num;
}

int
Model3D::get_total_tri_num() const
{
	int tri_num = 0;
	for (vector<Object3D>::const_iterator it=vObject.begin(); it!=vObject.end(); ++it) {
		tri_num += it->get_tri_num();
	}
	return  tri_num;
}

Object3D&
Model3D::add_new_object(const Object3D& obj)
{
	vObject.push_back(obj);
	return  vObject.back();
}

Material&
Model3D::add_new_material(const Material& mat)
{
	vMaterial.push_back(mat);
	return  vMaterial.back();
}

bbox3f
Model3D::get_bounding_box() const
{
	bbox3f  bb;
	const int obj_num = get_objects_num();
	for (int i=0; i<obj_num; ++i) {
		const Object3D& obj = get_object(i);
		if (!obj.is_visible())  continue;
		bb.merge( obj.get_bounding_box() );
	}
	return  bb;
}

void
Model3D::update_triangles()
{
	for (vector<Object3D>::iterator it=vObject.begin(); it!=vObject.end(); ++it) {
		it->update_triangles();
	}
}

void
Model3D::calc_normals()
{
	for (vector<Object3D>::iterator it=vObject.begin(); it!=vObject.end(); ++it) {
		it->calc_normals();
	}
}




int
Model3D::register_textures(const char *fn)
{
	for (int j = 0; j<vTextureFile.size(); j++) {
		if (stricmp(vTextureFile[j].fullpath, fn) == 0) {
			return j;
		}
	}

	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath(fn, drive, dir, fname, ext);

	TextureFile tf;
	strcpy(tf.fullpath, fn);
	sprintf(tf.filename, "%s%s", fname, ext);

	tf.bLoad = false;
	vTextureFile.push_back(tf);

	return vTextureFile.size() - 1;
}


void
Model3D::make_texture_list()
{
	vTextureFile.clear();
	
	
	for (int i=0; i<get_materials_num(); ++i) {
		Material& mat = get_material(i);

		if (!mat.bHasTexture && !mat.bHasSpecMap)				  continue;


		if (mat.bHasTexture) {
			mat.texture_ID = register_textures(mat.szTexFileName);
		}

		if (mat.bHasSpecMap) {
			mat.SpecMap_ID = register_textures(mat.szSpecMapFileName);
		}

	}
}

void
Model3D::fit_size(const float& size)
{
	// バウンディングボックスを計算する
	bbox3f  bb = get_bounding_box();
	if (bb.isValid()) {
		vector3f  vc(bb.x1+bb.x2, bb.y1+bb.y2, bb.z1+bb.z2);
		vc *= 0.5f;
		set_r0(-vc);
		vector3f dd(bb.x2-bb.x1, bb.y2-bb.y1, bb.z2-bb.z1);
		maxL = dd.x;
		if (dd.y > maxL)	maxL = dd.y;
		if (dd.z > maxL)	maxL = dd.z;
		set_scale(size/maxL);
	}
}
