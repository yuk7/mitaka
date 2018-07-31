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
#include  "gl_triangle_mesh.h"
#include  "gl_Model3D.h"
#include  "gl_image.h"
#include  <stdlib.h>


//-----------------
//  Bind textures
//-----------------
void
glModel3D::bind_textures(const directory& dir)
{
	image  img;

	const int tex_num = get_textures_num();
	for (int i=0; i<tex_num; ++i) {
		TextureFile&  tf = get_texture_file_info(i);

		if (!img.load( dir.get_path(tf.filename) )) {
			tf.bLoad = false;
		} else {
			tf.bLoad = true;
		}

		if (tf.bLoad) {
			glGenTextures(1, &texname[i]);
			glBindTexture(GL_TEXTURE_2D, texname[i]);
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
			gl_set_texture_mipmaps(img, 1);
		}
		img.free();
	}
}


//------------------
//  Clear textures
//------------------
void
glModel3D::clear_textures()
{
	const int tex_num = get_textures_num();
	for (int i=0; i<tex_num; ++i) {
		TextureFile&  tf = get_texture_file_info(i);
		
		if (tf.bLoad) {
			glDeleteTextures(1, &texname[i]);
			tf.bLoad = false;
		}
	}
}


//--------------
//  Draw Model
//--------------
void
glModel3D::draw_object(int obj_idx)  const
{
	const Object3D&  obj = get_object(obj_idx);
	if (!obj.is_visible())  return;

	// レンダリングモードの設定
	GLenum  mode = (get_wireframe())  ?  GL_LINE  :  GL_FILL;
	glPolygonMode( GL_FRONT_AND_BACK, mode );
	bool	bFlat		= get_flat_shading();
	bool	bTwoSided	= get_two_sided();
	const float default_amb[4]		= {0.3,0.3,0.3,1};
	const float default_dif[4]		= {1,1,1,1};
	const float default_spc[4]		= {0,0,0,1};
	const float default_shininess	= 5.0f;
	const float default_emis[4]		= {0,0,0,1};
	const int mat_num = get_materials_num();
	const int matlist_num  = obj.get_material_num();

	//-- マテリアルがない場合
	if (mat_num==0 || matlist_num==0) {
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_CULL_FACE);
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT,   default_amb );
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE,   default_dif );
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,  default_spc );
		glMaterialf ( GL_FRONT_AND_BACK, GL_SHININESS, default_shininess );
		glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION,  default_emis );
		gl_draw_triangle_mesh(obj, true, false, !bFlat);
		return;
	}

	//-- マテリアルありの場合
	for (int j=0; j<matlist_num; ++j) {
		const material_list&  ml = obj.get_material_list(j);
		const Material& mat = get_material( ml.material_ID );
		const int faces_num = ml.vFaceNo.size();
		//-- 基本パラメータのセット
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT,   mat.ambient_RGBAf );
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE,   mat.diffuse_RGBAf );
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,  mat.specular_RGBAf );		
		glMaterialf ( GL_FRONT_AND_BACK, GL_SHININESS, mat.shininess );
		glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION,  mat.emission_RGBAf );

		bool	bFlat2 = (mat.bFlat)  ?  true  :  bFlat;
		if (bTwoSided || mat.bTwoSided)			glDisable(GL_CULL_FACE);
		else									glEnable(GL_CULL_FACE);

		/****/
		bool bTexture = false;
		if (mat.bHasTexture) {
			const TextureFile& tf = get_texture_file_info(mat.texture_ID);
			if (tf.bLoad)  bTexture = true;
		}

		if (bTexture) {
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texname[mat.texture_ID]);
			GLint mode = (mat.bDecal)  ?  GL_DECAL  :  GL_MODULATE;
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mode);
		} else {
			glDisable(GL_TEXTURE_2D);
		}





		if (mat.bHasSpecMap) {
			float spc[4] = {0, 0, 0, 1};
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spc);
		}
		else {
		}





		//-- 描画
		for (int k=0; k<faces_num; ++k) {
			int tri_no = ml.vFaceNo[k];
			gl_draw_triangle_mesh(obj, true, bTexture, !bFlat2, tri_no, tri_no+1);
		}


		bool bSpecMap = false;
		if (mat.bHasSpecMap) {
			const TextureFile& tf = get_texture_file_info(mat.SpecMap_ID);
			if (tf.bLoad)  bSpecMap = true;
		}


		if (bSpecMap) {
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat.specular_RGBAf);

			float dif[4] = {0, 0, 0, 1};
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, dif);
			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, dif);


			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texname[mat.SpecMap_ID]);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);


			//-- 描画
			for (int k = 0; k<faces_num; ++k) {
				int tri_no = ml.vFaceNo[k];
				gl_draw_triangle_mesh(obj, true, bSpecMap, !bFlat2, tri_no, tri_no + 1);
			}

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
	}
}


void
glModel3D::draw()  const
{
	for (int i=0; i<get_objects_num(); ++i) {
		glPushMatrix();
			draw_object(i);
		glPopMatrix();
	}
}



void
glModel3D::draw_triangle_mesh(int obj_idx)  const
{
	const Object3D&  obj = get_object(obj_idx);
	if (!obj.is_visible())  return;

	gl_draw_triangle_mesh(obj, false, true, true);
}

