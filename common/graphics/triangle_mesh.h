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
#ifndef __TRIANGLE_MESH_H__
#define __TRIANGLE_MESH_H__
#include  <vector>
#include  "../math/vector3.h"
#include  <assert.h>
using namespace  std;


const size_t  TRIANGLES_RESERVE_SIZE = 65536;


//----------
//  三角形
//----------
class triangle
{
public:
	int			pidx[3];		// 頂点配列へのインデックス
	int			nidx[3];		// 法線配列へのインデックス
	int			uvidx[3];		// テクスチャ座標配列へのインデックス
	vector3f	nv;				// 三角形の法線ベクトル
public:
	triangle() {};
	triangle(int idx0, int idx1, int idx2)	{ set_index(idx0, idx1, idx2); }
	void	set_index(int idx0, int idx1, int idx2);
	void	set_normal_index(int i0, int i1, int i2);
	void	set_uv_index(int i0, int i1, int i2);
	void	calc_normal( const vector3f* pos);
	vector3f	get_rc(const vector3f* pos)  const;		// 重心
};


inline  void
triangle::set_index(int idx0, int idx1, int idx2)
{
	pidx[0] = idx0;
	pidx[1] = idx1;
	pidx[2] = idx2;
}

inline  void
triangle::set_normal_index(int i0, int i1, int i2)
{
	nidx[0] = i0;
	nidx[1] = i1;
	nidx[2] = i2;
}

inline  void
triangle::set_uv_index(int i0, int i1, int i2)
{
	uvidx[0] = i0;
	uvidx[1] = i1;
	uvidx[2] = i2;
}

inline  void
triangle::calc_normal(const vector3f* pos)
{
    nv = normalize( cross_product(pos[pidx[1]]-pos[pidx[0]], pos[pidx[2]]-pos[pidx[0]]) );
}

inline  vector3f
triangle::get_rc(const vector3f* pos)  const
{
	return (1.0f/3.0f) * (pos[pidx[0]] + pos[pidx[1]] + pos[pidx[2]]);
}


//-----------------
//  class TriMesh
//-----------------
const unsigned char  TRIMESH_ONLY_POSITION  = 0x00;
const unsigned char  TRIMESH_NORMAL  = 0x01;	// 法線情報
const unsigned char  TRIMESH_TEXTURE = 0x02;	// テクスチャー座標
const unsigned char  TRIMESH_DEFAULT = TRIMESH_NORMAL | TRIMESH_TEXTURE;

class TriMesh
{
private:
	unsigned char			type;			// タイプ
	vector<vector3f>		pos;			// 頂点の位置
	vector<vector3f>		nv;				// 頂点の法線
	vector<vector2f>		uv;				// 頂点のテクスチャ座標
	vector<triangle>		vTri;			// 三角形
	vector<vector<int> >	vtx_tri_list;	// その頂点を含む三角形のリスト
	bool					bVtxTriListInit;

public:
	TriMesh() : type(TRIMESH_DEFAULT), bVtxTriListInit(false)			{};
	TriMesh(unsigned char _type) : type(_type), bVtxTriListInit(false)	{};

	void			init(unsigned char _type)		{ clear(); type=_type; }
	//
	unsigned char	get_type() const				{ return type; }
	bool			support_normal() const			{ return (type & TRIMESH_NORMAL)  ? true : false; }
	bool			support_texture() const			{ return (type & TRIMESH_TEXTURE) ? true : false; }
	//-- Vertex
    int				add_vertex(const vector3f& _pos);
	int				get_vtx_num() const				{ return pos.size(); }
	int				get_vtx_last_idx() const		{ return pos.size()-1; }
	void			set_vtx_pos(int idx, const vector3f& _pos);
	//-- Normal
	void			add_normal(const vector3f& _nv);
	int				get_normal_num() const				{ return nv.size(); }
	void			set_normal(int idx, const vector3f& _nv);
	//-- Tex coord
	void			add_uv_coord(const vector2f& _uv);
	int				get_uv_coord_num() const		{ return uv.size(); }
	void			set_uv_coord(int idx, const vector2f& _uv);
	//--
	vector3f		get_vtx_pos(int idx) const		{ return pos[idx]; }
	const vector3f*	get_vertex_pointer() const		{ return &pos[0]; }
	const vector<vector3f>&		get_nv_pointer() const		{ return nv; }
	const vector<vector2f>&		get_uv_pointer() const		{ return uv; }
	vector<vector2f>&			get_uv_pointer()			{ return uv; }
	void			calc_normals();
	//-- Triangle
	void			add_triangle(const triangle& tri);
	int				get_tri_num() const				{ return vTri.size(); }
	int				get_tri_last_idx() const		{ return vTri.size()-1; }
	void			set_triangle(int tidx, const triangle& tri);
	void			update_triangles();
	triangle&		get_triangle(int tidx)			{ return vTri[tidx]; }
	const triangle&	get_triangle(int tidx) const 	{ return vTri[tidx]; }
	//--- Cross ref
	void			make_vtx_tri_list();
	//-- Clear
	void			clear()							{ pos.clear(); nv.clear(); uv.clear(); clear_triangles(); vtx_tri_list.clear(); }
	void			clear_triangles()				{ vTri.clear(); }
	//-- スケール、移動。回転
	void			scale(const float& scale);
	void			translate(const vector3f& dr);
};


inline  void
TriMesh::set_vtx_pos(int idx, const vector3f& _pos)
{
	if (idx<0 || idx>=pos.size())	return;
	pos[idx] = _pos;
}

inline  void
TriMesh::set_normal(int idx, const vector3f& _nv)
{
	nv[idx] = _nv;
}

inline  void
TriMesh::set_uv_coord(int idx, const vector2f& _uv)
{
	uv[idx] = _uv;
}


//--- 三角形の法線と接線を更新する ---
inline void
TriMesh::update_triangles()
{
	const vector3f* _pos = get_vertex_pointer();
	const vector<vector2f>&  _uv  = get_uv_pointer();
	const int tri_num = get_tri_num();
	for (int i=0; i<tri_num; i++) {
		if (support_normal())	{ vTri[i].calc_normal(_pos); }
	}
}


//------------------
// triangle z order
//------------------
template <class T>
class tri_z
{
public:
	T		z;
	short	surf_no;	// サーフェスの番号
	int		idx;		// その中の三角形の番号
public:
	bool operator<(const tri_z& tz) const  { return z < tz.z; }
};


#endif