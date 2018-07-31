/******************************************************************************

Copyright(c) 2015   Tsunehiko Kato

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

以下に定める条件に従い、本ソフトウェアおよび関連文書のファイル（以下「ソフトウェ
ア」）の複製を取得するすべての人に対し、ソフトウェアを無制限に扱うことを無償で許
可します。これには、ソフトウェアの複製を使用、複写、変更、結合、掲載、頒布、サブ
ライセンス、および / または販売する権利、およびソフトウェアを提供する相手に同じこ
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
#include  <stdio.h>
#include  "obj_loader.h"
#include  <vector>
#include  <list>
#include  <map>
#include  <algorithm>
#include  <ctype.h>
using namespace std;



static char*
skip_space(char *p)
{
	while (isspace(*p) && *p!='\0') {
		p++;
	}

	return p;
}

static char*
get_token(char *p, char **ppn)
{
	p = skip_space(p);

	if (*p == '\0') return p;

	char *p2 = p;
	while (!isspace(*p2) && *p2!='\0') {
		p2++;
	}

	*p2 = '\0';

	*ppn = p2+1;

	return p;
}




static void
clear_material(Material &mat)
{
	memset(&mat, 0, sizeof(mat));
}


static void
set_texture(char *p, Material& mat, bool bSpecMap)
{
	char str[1024], dms[256];
	int count;
	float base, gain;

	count = sscanf(p, "%s %e %e %s", dms, &base, &gain, str);
	if (count != 4) {
		count = sscanf(p, "%s", str);
		if (count != 1) {
			return;
		}

		gain = 1.0;
	}

	if (str[0] != '\0') {
		if (bSpecMap) {
			mat.bHasSpecMap = true;
			strcpy(mat.szSpecMapFileName, str);
			mat.specular_RGBAf[0] = gain;
			mat.specular_RGBAf[1] = gain;
			mat.specular_RGBAf[2] = gain;
			mat.specular_RGBAf[3] = 1;
		}
		else {
			mat.bHasTexture = true;
			strcpy(mat.szTexFileName, str);
			mat.diffuse_RGBAf[0] = gain;
			mat.diffuse_RGBAf[1] = gain;
			mat.diffuse_RGBAf[2] = gain;
			mat.diffuse_RGBAf[3] = 1;
		}
	}
}


static bool
parse_mtl_file_one_line(FILE *fp, Model3D &mdl, Material &mat)
{
	char buf[1024];
	char *p, *pn, *res;
	int count;
	float r, g, b, a;


	res = fgets(buf, 1024, fp);
	if (res == NULL) {
		return false;
	}


	if (strlen(buf) > 0) {
		buf[strlen(buf) - 1] = '\0';
	}

	p = buf;
	p = get_token(p, &pn);


	if (p[0] == '\0')  return true;


	// comment
	if (p[0] == '#')  return true;


	// newmtl
	if (stricmp(p, "newmtl") == 0) {
		if (mat.szName[0] != '\0') {
			mdl.add_new_material(mat);
			clear_material(mat);
		}

		count = sscanf(pn, "%s", mat.szName);
		if (count != 1) return true;

		return true;
	}


	// Ka
	if (stricmp(p, "Ka") == 0) {
		count = sscanf(pn, "%e %e %e", &r, &g, &b);
		if (count != 3) return true;

		mat.ambient_RGBAf[0] = r;
		mat.ambient_RGBAf[1] = g;
		mat.ambient_RGBAf[2] = b;
		mat.ambient_RGBAf[3] = 1;
		return true;
	}

	// Kd
	if (stricmp(p, "Kd") == 0) {
		count = sscanf(pn, "%e %e %e", &r, &g, &b);
		if (count != 3) return true;

		mat.diffuse_RGBAf[0] = r;
		mat.diffuse_RGBAf[1] = g;
		mat.diffuse_RGBAf[2] = b;
		mat.diffuse_RGBAf[3] = 1;
		return true;
	}

	// Ks
	if (stricmp(p, "Ks") == 0) {
		count = sscanf(pn, "%e %e %e", &r, &g, &b);
		if (count != 3) return true;

		mat.specular_RGBAf[0] = r;
		mat.specular_RGBAf[1] = g;
		mat.specular_RGBAf[2] = b;
		mat.specular_RGBAf[3] = 1;
		return true;
	}

	// Ns
	if (stricmp(p, "Ns") == 0) {
		count = sscanf(pn, "%e", &r);
		if (count != 1) return true;

		mat.shininess = r;
		return true;
	}


	// map_Kd
	if (stricmp(p, "map_Kd") == 0) {
		set_texture(pn, mat, false);
		return true;
	}

	// map_Ks
	if (stricmp(p, "map_Ks") == 0) {
		set_texture(pn, mat, true);
		return true;
	}



	return true;
}


static char*
read_one_vertex(char *p, int &iv, int &it, int &in)
{
	char *p0;


	// vertex no
	p = skip_space(p);
	if (!isdigit(*p)) return NULL;	// error
	p0 = p;
	while (isdigit(*p)) {
		p++;
	}
	if (*p != '/') return NULL;	// error

	if (p0 == p) {
		iv = -1;
	}
	else {
		*p = '\0';
		sscanf(p0, "%d", &iv);
		iv--;
		p++;
	}


	// texcoord no
	p = skip_space(p);
	if (!isdigit(*p)) return NULL;	// error
	p0 = p;
	while (isdigit(*p)) {
		p++;
	}
	if (*p != '/') return NULL;	// error

	if (p0 == p) {
		it = -1;
	}
	else {
		*p = '\0';
		sscanf(p0, "%d", &it);
		it--;
		p++;
	}


	// normal no
	p = skip_space(p);
	if (!isdigit(*p)) return NULL;	// error
	p0 = p;
	while (isdigit(*p)) {
		p++;
	}
	if ( !isspace(*p) && *p!='\0' ) return NULL;	// error

	if (p0 == p) {
		in = -1;
	}
	else {
		*p = '\0';
		sscanf(p0, "%d", &in);
		in--;
		p++;
	}


	return p;
}


static void
set_triangle(triangle &tri, int *iv, int *it, int *in, int i0, int i1, int i2)
{
	tri.set_index(iv[i0], iv[i1], iv[i2]);

	if (it[i0] != -1 && it[i1] != -1 && it[i2] != -1) {
		tri.set_uv_index(it[i0], it[i1], it[i2]);
	}

	if (in[i0] != -1 && in[i1] != -1 && in[i1] != -1) {
		tri.set_normal_index(in[i0], in[i1], in[i2]);
	}
}


static void
read_face(char *p, Object3D &obj, material_list& ml)
{
	int i, iv[4], it[4], in[4];
	int vtn_num = 0;

	triangle  tri;


	int vtx_num = 0;
	for (i = 0; i < 4; i++) {
		p = read_one_vertex(p, iv[i], it[i], in[i]);
		if (p == 0) break;

		vtx_num++;
	}

	if (vtx_num >= 3) {
		set_triangle(tri, iv, it, in, 0, 1, 2);
		tri.calc_normal(obj.get_vertex_pointer());
		obj.add_triangle(tri);
		if (ml.material_ID != -1) {
			ml.vFaceNo.push_back( obj.get_tri_last_idx() );
		}
	}

	if (vtx_num >= 4) {
		set_triangle(tri, iv, it, in, 0, 2, 3);
		tri.calc_normal(obj.get_vertex_pointer());
		obj.add_triangle(tri);
		if (ml.material_ID != -1) {
			ml.vFaceNo.push_back(obj.get_tri_last_idx());
		}
	}
}



static bool
parse_obj_file_one_line(FILE *fp, Model3D &mdl, Object3D &obj, material_list &ml, char *drive, char *dir)
{
	char buf[1024], str[1024];
	char *p, *pn, *res;
	int count;
	float x, y, z;

	
	res = fgets(buf, 1024, fp);
	if (res == NULL) {
		return false;
	}


	if (strlen(buf) > 0) {
		buf[strlen(buf) - 1] = '\0';
	}

	p = buf;
	p = get_token(p, &pn);


	if (p[0] == '\0')  return true;


	// comment
	if (p[0] == '#')  return true;


	// mtllib
	if (stricmp(p, "mtllib") == 0) {
		char fn[256];
		FILE *fp2;

		count = sscanf(pn, "%s", &str);
		if (count != 1) return true;

		sprintf(fn, "%s%s%s", drive, dir, str);
		if ((fp2 = fopen(fn, "rt")) != NULL) {
			Material mat;
			clear_material(mat);

			while (parse_mtl_file_one_line(fp2, mdl, mat)) {
			}

			if (mat.szName[0] != '\0') {	// 最後のマテリアルを追加
				mdl.add_new_material(mat);
			}

			fclose(fp2);
		}

		return true;
	}


	// vertex
	if (stricmp(p, "v") == 0) {
		count = sscanf(pn, "%e %e %e", &x, &y, &z);
		if (count != 3) return true;

		obj.add_vertex( vector3f(x, y, z) );
		return true;
	}


	// texture coordinate
	if (stricmp(p, "vt") == 0) {
		count = sscanf(pn, "%e %e", &x, &y);
		if (count != 2) return true;

		obj.add_uv_coord( vector2f(x, y) );
		return true;
	}


	// normal
	if (stricmp(p, "vn") == 0) {
		count = sscanf(pn, "%e %e %e", &x, &y, &z);
		if (count != 3) return true;

		obj.add_normal( vector3f(x, y, z) );
		return true;
	}


	// face
	if (stricmp(p, "f") == 0) {
		read_face(pn, obj, ml);
		return true;
	}


	// usemtl
	if (stricmp(p, "usemtl") == 0) {
		count = sscanf(pn, "%s", &str);
		if (count != 1) return true;


		// register
		if (ml.material_ID != -1) {
			obj.add_material_list(ml);
			ml.material_ID = -1;
			ml.vFaceNo.clear();
		}


		// set material ID
		const int imax = mdl.get_materials_num();
		int i;
		for (i = 0; i<imax; ++i) {
			Material& mat = mdl.get_material(i);
			if (stricmp(str, mat.szName) == 0)	break;
		}
		if (i == imax) {
			i = -1;
		}
		ml.material_ID = i;

		return true;
	}

	return true;
}



//--------------------
//  Load a .obj file
//--------------------
bool
load_OBJ_file(const char* filename, Model3D& mdl)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char fn[_MAX_PATH];
	FILE* fp;

	// check
	if ((fp = fopen(filename, "rt")) == NULL) return false;
	fclose(fp);

	_splitpath(filename, drive, dir, fname, ext);



	// Load obj file
	if ((fp = fopen(filename, "rt")) == NULL) return false;

	Object3D obj(TRIMESH_DEFAULT);
	obj.set_name("Default");

	material_list ml;
	ml.material_ID = -1;
	ml.vFaceNo.clear();


	while (parse_obj_file_one_line(fp, mdl, obj, ml, drive, dir)) {
	}


	if (ml.material_ID != -1) {
		obj.add_material_list(ml);
	}


	if (obj.get_tri_num() > 0) {	// メッシュを持たないものは除外する
		mdl.add_new_object(obj);
	}


	fclose(fp);




	//-- init
	//mdl.make_edge_lists();
	mdl.make_texture_list();
	//-- モデル名を設定
	sprintf(fn, "%s%s", fname, ext);
	mdl.set_name(fn);

	return  true;
}
