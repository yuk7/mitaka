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
#include  "triangle_mesh.h"
#include  <assert.h>


//=================
//  class TriMesh
//=================

//-----------------------
// Name: add_vertex
// Desc: 頂点を追加する
// Retn: 追加した頂点の番号
//-----------------------
int
TriMesh::add_vertex(const vector3f& _pos)
{
	//-- allocate memory --
	int size = pos.size();
	if (size >= pos.capacity()) {
		size_t  reserve_size = (size < TRIANGLES_RESERVE_SIZE/2) ? TRIANGLES_RESERVE_SIZE : size*2;
		pos.reserve(reserve_size);
		vtx_tri_list.reserve(reserve_size);
	}

	//-- add vertex
	pos.push_back(_pos);
	vtx_tri_list.push_back( vector<int>() );
	bVtxTriListInit = false;

	return  get_vtx_last_idx();
}

void
TriMesh::add_normal(const vector3f& _nv)
{
	if (!(type & TRIMESH_NORMAL)) return;

	//-- allocate memory --
	int size = nv.size();
	if (size >= nv.capacity()) {
		size_t  reserve_size = (size < TRIANGLES_RESERVE_SIZE/2) ? TRIANGLES_RESERVE_SIZE : size*2;
		nv.reserve(reserve_size);
	}

	//-- add normal
	nv.push_back(_nv);
}

void
TriMesh::add_uv_coord(const vector2f& _uv)
{
	if (!(type & TRIMESH_TEXTURE)) return;

	//-- allocate memory --
	int size = uv.size();
	if (size >= uv.capacity()) {
		size_t  reserve_size = (size < TRIANGLES_RESERVE_SIZE/2) ? TRIANGLES_RESERVE_SIZE : size*2;
		uv.reserve(reserve_size);
	}

	//-- add uv coord
	uv.push_back(_uv);
}


void
TriMesh::add_triangle(const triangle& tri)
{
	//-- allocate memory --
	int  size = vTri.size();
	if (size >= vTri.capacity()) {
		size_t  reserve_size = (size < TRIANGLES_RESERVE_SIZE/2) ? TRIANGLES_RESERVE_SIZE : size*2;
		vTri.reserve(reserve_size);
	}

	vTri.push_back(tri);
	bVtxTriListInit = false;
}


void
TriMesh::make_vtx_tri_list()
{
	const int vtx_num = get_vtx_num();

	for (int i=0; i<vtx_num; i++) {
		vtx_tri_list[i].clear();
	}

	const int tri_num = get_tri_num();
	for (int i=0; i<tri_num; i++) {
		triangle&  tri = get_triangle(i);

		//-- クロスリファレンスリストに追加する
		vtx_tri_list[ tri.pidx[0] ].push_back(i);
		vtx_tri_list[ tri.pidx[1] ].push_back(i);
		vtx_tri_list[ tri.pidx[2] ].push_back(i);
	}

	bVtxTriListInit = true;
}


//--- 頂点の法線を自動的に計算する ---
void
TriMesh::calc_normals()
{
	if (!support_normal()) return;
	if (!bVtxTriListInit)	make_vtx_tri_list();
	const int vtx_num = get_vtx_num();
	//-- calc normal of vertices
	for (int i=0; i<vtx_num; ++i) {
		vector3f nv_sum(0,0,0), tu_sum(0,0,0);
		const vector<int>&  vtl = vtx_tri_list[i];
		for (vector<int>::const_iterator it=vtl.begin(); it!=vtl.end(); ++it) {
			const triangle&  tri = get_triangle(*it);
			nv_sum += tri.nv;
		}
		
		if (support_normal()) {
			float nrm = norm(nv_sum);
			if (nrm != 0.0) {
				nv[i] = nv_sum / nrm;
			} else {
				nv[i] = vector3f(-10,0,0);	// スムージングしないフラグとして使う
			}
		} 
	}
}



//------------------------
//  スケーリング、移動、回転
//------------------------
void
TriMesh::scale(const float& scale)
{
	const int vtx_num = get_vtx_num();

	for (int i=0; i<vtx_num; i++) {
		pos[i] *= scale;
	}
}

void
TriMesh::translate(const vector3f& dr)
{
	const int vtx_num = get_vtx_num();

	for (int i=0; i<vtx_num; i++) {
		pos[i] += dr;
	}
}


