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
#include  <stdio.h>
#include  "3ds_loader.h"
#include  <vector>
#include  <list>
#include  <map>
#include  <algorithm>
using namespace std;




//=============
//  Chunk IDs
//=============
//--- Primary chunk
#define		MAIN3DS					0x4D4D
//--- Main chunks
#define		EDIT3DS					0x3D3D		// Editor config
#define		KEYF3DS					0xB000		// Keyframer config
#define		VERSION3DS				0x0002		// Version
//--- Sub defines of EDIT3DS
#define		EDIT_MATERIAL			0xAFFF		// Material block
#define		EDIT_OBJECT				0x4000		// Object block
//--- Sub defines for Material
#define		MAT_NAME				0xA000
#define		MAT_AMBIENT				0xA010
#define		MAT_DIFFUSE				0xA020
#define		MAT_SPECULAR			0xA030
#define		MAT_SHININESS			0xA040
#define		MAT_SHININESS_STR		0xA041
#define		MAT_TRANSPARENCY		0xA050
#define		MAT_TRANSP_FALL_OFF		0xA052
#define		MAT_REFLECT_BLUR		0xA053
#define		MAT_TWO_SIDED			0xA081
#define		MAT_SELF_ILLUM			0xA084
#define		MAT_TYPE				0xA100
#define		MAT_TEXTURE1			0xA200
#define		MAT_SPECULAR_MAP		0xA204
#define		MAT_TEXTURE_FILENAME	0xA300
#define		MAT_TEXTURE_OPTIONS		0xA351
//--- Sub defines for Object
#define		OBJ_TRIMESH				0x4100
#define		TRI_VERTEXL				0x4110
#define		TRI_FACEL1				0x4120
#define		TRI_FACEL2				0x4111
#define		TRI_MATERIAL			0x4130
#define		TRI_UV					0x4140
#define		TRI_SMOOTH_GROUP		0x4150
#define		OBJ_LOCAL_COORD			0x4160
#define		OBJ_COLOR				0x4165
#define		OBJ_HIDDEN				0x4010
//--- Color chunk
#define		RGB_FLOAT				0x0010
#define		RGB_BYTE				0x0011
#define		RGBgm_BYTE				0x0012
#define		RGBgm_FLOAT				0x0013
//--- Percent chunk
#define		PER_INT					0x0030
#define		PER_FLOAT				0x0031
//--- Types of material
#define		MAT_TYPE_FLAT			1
#define		MAT_TYPE_GOUR			2
#define		MAT_TYPE_PHONG			3
#define		MAT_TYPE_METAL			4


vector<int>  new_tri_no_ref;


//-----------------
//  Struct  chunk
//-----------------
struct chunk
{
	unsigned short	ID;
	unsigned int	length;
	size_t		bytesRead;
};


//----------------
//  Read a chunk
//----------------
static void
read_chunk(FILE* fp, chunk& chk)
{
	chk.bytesRead  = fread(&chk.ID, 1, 2, fp);		// Read ID
	chk.bytesRead += fread(&chk.length, 1, 4, fp);	// Read length
}


//----------------
//  Skip a chunk
//----------------
static void
skip_chunk(FILE* fp, chunk& chk)
{
	int len;
	len = chk.length - chk.bytesRead;
	if (len < 0)  return;
	fseek(fp, len, SEEK_CUR);
	chk.bytesRead += len;
}


//----------------------
//  Read a color chunk
//----------------------
static void
read_color_chunk(FILE* fp, float* pRGBAf, chunk& prev_chk)
{
	unsigned char color_byte[3];	
	chunk  chk;

	read_chunk(fp, chk);
	switch (chk.ID) {
		case RGB_FLOAT:
		case RGBgm_FLOAT:
			chk.bytesRead      += fread(pRGBAf, 1, 12, fp);
			break;
		case RGB_BYTE:
		case RGBgm_BYTE:
			chk.bytesRead      += fread(color_byte, 1, 3, fp);
			for (int i=0; i<3; ++i) {
				pRGBAf[i] = float(color_byte[i])/255;
			}
			break;
		default:
			skip_chunk(fp, chk);
			break;
	}
	prev_chk.bytesRead += chk.bytesRead;
	pRGBAf[3] = 1.0f;
}


//------------------------
//  Read a percent chunk
//------------------------
static void
read_percent_chunk(FILE* fp, float* pPercent, chunk& prev_chk)
{
	unsigned short percent_word;
	chunk  chk;

	read_chunk(fp, chk);
	switch (chk.ID) {
		case PER_FLOAT:
			chk.bytesRead += fread(pPercent, 1, 4, fp);
			*pPercent *= 100.0f;
			break;
		case PER_INT:
			chk.bytesRead += fread(&percent_word, 1, 2, fp);
			*pPercent = float(percent_word);
			break;
		default:
			skip_chunk(fp, chk);
			break;
	}

	prev_chk.bytesRead += chk.bytesRead;
}


//-----------------
//  Read a string
//-----------------
static long
read_string(FILE* fp, char* buf)
{
	long len=0;
	int  ch;

	while ((ch=fgetc(fp))!='\0' && ch!=EOF) { *buf++=char(ch); len++; }
	*buf='\0';
	assert(len < 255);

	return len + 1;
}



//-----------------
//  Read Vertices
//-----------------
static void
read_vertices(FILE* fp, Object3D& obj, chunk& prev_chk)
{
	unsigned short num_of_vertices;

	prev_chk.bytesRead += fread(&num_of_vertices, 1, 2, fp);
	obj.clear();
	const int imax = int(num_of_vertices);
	for (int i=0; i<imax; ++i) {
		// read a vertex position
		vector3f  pos;
        prev_chk.bytesRead += fread(pos.get_pointer(), 1, 12, fp);
		assert( prev_chk.bytesRead <= prev_chk.length );
		// swap y and z values
		swap(pos.y, pos.z);
		pos.z = -pos.z;

		// add the vertex
		obj.add_vertex(pos);
	}
}


//-----------------------
//  Read UV coordinates
//-----------------------
static void
read_UV_coordinates(FILE* fp, Object3D& obj, chunk& prev_chk)
{
	unsigned short num_of_uv_coord;

	prev_chk.bytesRead += fread(&num_of_uv_coord, 1, 2, fp);
	const int imax = int(num_of_uv_coord);
	for (int i=0; i<imax; ++i) {
		// read a uv coord
		vector2f  uv;
        prev_chk.bytesRead += fread(uv.get_pointer(), 1, 8, fp);
		assert( prev_chk.bytesRead <= prev_chk.length );
		// add the uv coord
		obj.add_uv_coord(uv);
	}
}


//--------------------------
//  Read Local Coordinates
//--------------------------
static void
read_local_coordinates(FILE* fp, Object3D& obj, chunk& prev_chk)
{
	vector3f  v[4];
	prev_chk.bytesRead += fread(v, 1, 48, fp);
}


//--------------
//  Read faces
//--------------
static void
read_faces(FILE* fp, Object3D& obj, chunk& prev_chk)
{
	unsigned short  num_of_faces;

	prev_chk.bytesRead += fread(&num_of_faces, 1, 2, fp);
	obj.clear_triangles();

	const int imax = int(num_of_faces);
	new_tri_no_ref.resize(imax);
	for (int i=0; i<imax; ++i) {
		triangle  tri;
		unsigned short  idx[4];
		prev_chk.bytesRead += fread(&idx, 1, 8, fp);
		for (int j=0; j<3; ++j) {
			int index = int(idx[j]);
			tri.pidx[j]  = index;
			///
			tri.nidx[j]  = -1;		// 三角形の法線を使うフラグをセット
			tri.uvidx[j] = index;
		}
		assert(prev_chk.bytesRead <= prev_chk.length);

		if (tri.pidx[0]==tri.pidx[1] || tri.pidx[1]==tri.pidx[2] || tri.pidx[2]==tri.pidx[0]) {
			new_tri_no_ref[i] = -1;		// スキップしたフラグ
			continue;
		}

		tri.calc_normal( obj.get_vertex_pointer() );
        obj.add_triangle(tri);
		new_tri_no_ref[i] = obj.get_tri_num() - 1;
	}
}


//-----------------------------
//  Read Smoothing group List
//-----------------------------
struct nv_info {
	int		tri_no;
	int		k;
};

	
static void
read_smooth_group_list(FILE* fp, Object3D& obj, chunk& prev_chk)
{
	list<int>	smooth_group[32];

	// それぞれのグループにフェイスを登録する
	const int num_of_tri = new_tri_no_ref.size();
	for (int i=0; i<num_of_tri; i++) {
		unsigned long  flg, mask;
		prev_chk.bytesRead += fread(&flg, 1, 4, fp);

		int tri_no = new_tri_no_ref[i];
		if (tri_no == -1)  continue;

		mask=1;
		for (int j=0; j<32; j++) {
			if (flg & mask) { smooth_group[j].push_back(tri_no); }
			mask <<= 1;
		}
	}

	// それぞれのグループごとに頂点を計算し登録する
	for (int i=0; i<32; i++) {
		if (smooth_group[i].size() == 0)  continue;
	
		// 各頂点を含む三角形のリストを作る
		map<int, list<nv_info> >  nvi;
		for (list<int>::const_iterator it=smooth_group[i].begin(); it!=smooth_group[i].end(); ++it) {
			const int tri_no = *it;
			const triangle& tri = obj.get_triangle( tri_no );
			for (int k=0; k<3; k++) {
				nv_info ni;
				ni.tri_no = tri_no;
				ni.k = k;
				int pidx = tri.pidx[k];

				map<int, list<nv_info> >::iterator it = nvi.find( pidx );
				if (it == nvi.end()) {
					list<nv_info>  list0;
					list0.push_back(ni);
					nvi.insert(pair<int, list<nv_info> >(pidx, list0));
				} else {
					it->second.push_back(ni);
				}
			}
		}

		// 各頂点で平均の法線を計算する
		for (map<int, list<nv_info> >::const_iterator it=nvi.begin(); it!=nvi.end(); ++it) {
			const list<nv_info>&  li = it->second;
			if (li.size() <= 1) continue;	// 頂点を含む三角形が１つだけの時は法線を作らない


			// 平均の法線を計算する
			const int nidx = obj.get_normal_num();
			vector3f nv_sum(0,0,0);
			for (list<nv_info>::const_iterator itl=li.begin(); itl!=li.end(); ++itl) {
				triangle& tri = obj.get_triangle(itl->tri_no);
				nv_sum += tri.nv;
				tri.nidx[itl->k] = nidx;	// 三角形に法線の番号を登録する
			}

			obj.add_normal( normalize(nv_sum) );
		}
	}

	// skip the rest of the chunk
	skip_chunk(fp, prev_chk);
}


//------------------------
//  Read Object Material
//------------------------
static void
read_object_material(FILE* fp, Model3D& mdl, Object3D& obj, chunk& prev_chk)
{
	material_list  ml;

	// read material name
	char str[500];	////
	prev_chk.bytesRead += read_string(fp, str);
	
	// set material ID
	const int imax = mdl.get_materials_num();
	int i;
	for (i=0; i<imax; ++i) {
		Material& mat = mdl.get_material(i);
		if (stricmp(str, mat.szName)==0)	break;
	}
	ml.material_ID = i;

	// read face list
	unsigned short  faces_num;
	prev_chk.bytesRead += fread(&faces_num, 1, 2, fp);
	const int imax2 = int(faces_num);
	for (i=0; i<imax2; ++i) {
		unsigned short  face_no;
		prev_chk.bytesRead += fread(&face_no, 1, 2, fp);	
		int tri_no = new_tri_no_ref[face_no];	// 元のフェイス番号から三角形の番号に直す
		if (tri_no == -1)  continue;
		ml.vFaceNo.push_back(tri_no);
	}
	obj.add_material_list(ml);
}




//
//
//
void
process_load_texture(FILE* fp, Material& mat, chunk& prev_chk, bool bLoadSpecMap)
{
	chunk  chk;

	while (prev_chk.bytesRead < prev_chk.length) {
		read_chunk(fp, chk);
		switch (chk.ID) {

		case MAT_TEXTURE_FILENAME:	// Texture file name
		{
			char *fn;
			char dummy[256];

			fn = dummy;



			if (bLoadSpecMap) {
				mat.bHasSpecMap = true;
				fn = mat.szSpecMapFileName;
			}
			else {
				mat.bHasTexture = true;
				fn = mat.szTexFileName;
			}

			chk.bytesRead += fread(fn, 1, chk.length - chk.bytesRead, fp);
		}

		break;

		case MAT_TEXTURE_OPTIONS:
		{
			short int  opt;
			chk.bytesRead += fread(&opt, 1, 2, fp);
			if (opt & 1)	mat.bDecal = true;
		}
		break;

		default:					// Unknown chunk
			skip_chunk(fp, chk);
			break;
		}

		prev_chk.bytesRead += chk.bytesRead;
	}
}

//-------------------------------
//  Process Next Material Chunk
//-------------------------------
static void
process_next_material_chunk(FILE* fp, Material& mat, chunk& prev_chk)
{
	chunk  chk;
	float spec[4], shininess;
	memset(spec, 0, 4*sizeof(float));
	shininess = 0.0f;
	mat.shininess = 1.0f;
	mat.bHasTexture = mat.bHasSpecMap = false;


	for (int i=0; i<4; i++) {
		mat.diffuse_RGBAf[i] = 1.0f;
	}
	while (prev_chk.bytesRead < prev_chk.length) {
		read_chunk(fp, chk);
		switch (chk.ID) {
			case MAT_NAME:				// Material Name
				chk.bytesRead += fread(mat.szName, 1, chk.length-chk.bytesRead, fp);
				break;
			case MAT_AMBIENT:			// Ambient color
				read_color_chunk(fp, mat.ambient_RGBAf, chk);
				break;
			case MAT_DIFFUSE:			// Diffuse color
				read_color_chunk(fp, mat.diffuse_RGBAf, chk);
				break;
			case MAT_SPECULAR:			// Specular color
				read_color_chunk(fp, spec, chk);
				break;
			case MAT_SHININESS:			// Shininess
				read_percent_chunk(fp, &shininess, chk);
				break;
			case MAT_SHININESS_STR:
				read_percent_chunk(fp, &mat.shininess, chk);
				mat.shininess *= 1.28f;
				break;

			case MAT_TEXTURE1:			// Texture
				process_load_texture(fp, mat, chk, false);
				break;

			case MAT_SPECULAR_MAP:		// Texture
				process_load_texture(fp, mat, chk, true);
				break;

			case MAT_TYPE:
				{
					short int  ty;
					chk.bytesRead += fread(&ty, 1, 2, fp);
					if (ty==1)  mat.bFlat = true;
				}
				break;
			case MAT_TWO_SIDED:
				mat.bTwoSided = true;
				break;
			default:					// Unknown chunk
				skip_chunk(fp, chk);
				break;
		}
		prev_chk.bytesRead += chk.bytesRead;
	}



	mat.shininess = shininess;

	for (int i=0; i<3; i++) {
		mat.specular_RGBAf[i] = spec[i];
	}
}


//-----------------------------
//  Process Next Object Chunk
//-----------------------------
static void
process_next_object_chunk(FILE* fp, Model3D& mdl, Object3D& obj, chunk& prev_chk)
{
	chunk  chk;

	while (prev_chk.bytesRead < prev_chk.length) {
		read_chunk(fp, chk);

		switch (chk.ID) {
			case OBJ_TRIMESH:
				process_next_object_chunk(fp, mdl, obj, chk);
				break;

			case TRI_VERTEXL:
				read_vertices(fp, obj, chk);
				break;

			case TRI_FACEL1:
			//case TRI_FACEL2:
				read_faces(fp, obj, chk);
				break;

			case TRI_SMOOTH_GROUP:
				read_smooth_group_list(fp, obj, chk);
				break;

			case TRI_MATERIAL:
				read_object_material(fp, mdl, obj, chk);
				break;

			case TRI_UV:
				read_UV_coordinates(fp, obj, chk);
				break;

			case OBJ_HIDDEN:
				obj.set_visible(false);
				break;

			case OBJ_LOCAL_COORD:
				read_local_coordinates(fp, obj, chk);
				break;
			default:					// Unknown chunk
				skip_chunk(fp, chk);
				break;
		}
		prev_chk.bytesRead += chk.bytesRead;
	}
}


//----------------------
//  Process Next Chunk
//----------------------
static void
process_next_chunk(FILE* fp, Model3D& mdl, chunk& prev_chk)
{
	unsigned int  ver;
	char  str[500];

	while (prev_chk.bytesRead < prev_chk.length) {
		chunk  chk;

		read_chunk(fp, chk);
		switch (chk.ID) {
			case VERSION3DS:
				chk.bytesRead += fread(&ver, 1, chk.length-chk.bytesRead, fp);
				break;

			case EDIT3DS:
				process_next_chunk(fp, mdl, chk);
				break;

			case EDIT_MATERIAL:
				{
					// add new material
					Material  mat;
					memset(&mat, 0, sizeof(mat));
					process_next_material_chunk(fp, mat, chk);
					mdl.add_new_material(mat);
				}
				break;

			case EDIT_OBJECT:
				{
					chk.bytesRead += read_string(fp, str);	// read object name
					// read new object
					Object3D obj(TRIMESH_DEFAULT);
					obj.set_name(str);
					process_next_object_chunk(fp, mdl, obj, chk);
					if (obj.get_tri_num() > 0) {	// メッシュを持たないものは除外する
						mdl.add_new_object(obj);
					}
				}
				break;
			default:					// Unknown chunk
				skip_chunk(fp, chk);
				break;
		}
		prev_chk.bytesRead += chk.bytesRead;
	}
}


//--------------------
//  Load a .3ds file
//--------------------
bool
load_3DS_file(const char* filename, Model3D& mdl)
{
	FILE* fp;
	if ((fp=fopen(filename, "rb")) == NULL) return false;

	chunk  chk;
	read_chunk(fp, chk);

	// Make sure this is a 3DS file
	if (chk.ID != MAIN3DS)  {
		fclose(fp);
		return false;
	}
	mdl.clear();
	process_next_chunk(fp, mdl, chk);
	fclose(fp);

	//-- init
	//mdl.make_edge_lists();
	mdl.make_texture_list();
	//-- モデル名を設定
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char fn[_MAX_PATH];
	_splitpath( filename, drive, dir, fname, ext );
	sprintf(fn, "%s%s", fname, ext);
	mdl.set_name(fn);

	return  true;
}
