/******************************************************************************

Copyright (c) 2016   Tsunehiko Kato

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
#ifndef  __OCTREE_H__
#define  __OCTREE_H__
#include  "../math/vector3.h"
#include  "bounding_box.h"
#include  <vector>


enum {
	OCTREE_INSIDE,
	OCTREE_PARTIAL,
	OCTREE_OUTSIDE
};


const int octree_lv_max = 7;



//---------------
//  Octree node
//---------------
template  <class T>
class octree_node
{
	bbox3<T>	region;
	bbox3<T>	octant[8];
	const octree_node<T> *parent_node;
	octree_node<T> *child_nodes[8];
	std::vector<int> vObjIndex;
	T Lx, Ly, Lz;
	vector3<T> rc;

public:
	octree_node(const octree_node<T>* _parent_node, const bbox3<T>& _region);
	~octree_node();

	const bbox3<T>& get_bbox() const { return region; }
	const bbox3<T>* get_octant_bbox() const { return octant; }
	octree_node<T>** get_child_nodes() { return child_nodes; }

	void	add_object(const int& obj_idx);

	int		visible_check(const vector3<T>* nv, const T* A, const T* B, const T*C, const T* D, bool bCullFar) const;


	void	register_objects_to_draw_list(std::vector<int>& list) const;
	void	register_all_to_draw_list(std::vector<int>& list) const;
	void	make_draw_objects_list(const vector3<T>* nv, const T* A, const T* B, const T*C, const T* D, bool bCullFar, std::vector<int>& list) const;
};



template  <class T>
octree_node<T>::octree_node(const octree_node<T>* _parent_node, const bbox3<T>& _region) : parent_node(_parent_node), region(_region)
{
	// clear child nodes
	for (int i = 0; i < 8; i++) {
		child_nodes[i] = 0;
	}

	// calc octant
	T x1, x2, y1, y2, z1, z2, xc, yc, zc;
	x1 = region.x1;
	x2 = region.x2;
	y1 = region.y1;
	y2 = region.y2;
	z1 = region.z1;
	z2 = region.z2;
	xc = T(0.5) * (x1 + x2);
	yc = T(0.5) * (y1 + y2);
	zc = T(0.5) * (z1 + z2);

	Lx = x2 - x1;
	Ly = y2 - y1;
	Lz = z2 - z1;

	rc.set(xc, yc, zc);

	octant[0].set(x1, xc, y1, yc, z1, zc);
	octant[1].set(xc, x2, y1, yc, z1, zc);
	octant[2].set(x1, xc, yc, y2, z1, zc);
	octant[3].set(xc, x2, yc, y2, z1, zc);
	octant[4].set(x1, xc, y1, yc, zc, z2);
	octant[5].set(xc, x2, y1, yc, zc, z2);
	octant[6].set(x1, xc, yc, y2, zc, z2);
	octant[7].set(xc, x2, yc, y2, zc, z2);
}



template  <class T>
octree_node<T>::~octree_node()
{
	for (int i = 0; i < 8; i++) {
		if (child_nodes[i] != 0) {
			delete child_nodes[i];
		}
	}
}



template  <class T>
void
octree_node<T>::add_object(const int& obj_idx)
{
	vObjIndex.push_back(obj_idx);
}



template  <class T>
int
octree_node<T>::visible_check(const vector3<T>* nv, const T* A, const T* B, const T*C, const T* D, bool bCullFar) const
{
	const int N = (bCullFar) ? 6 : 5;

	int count = 0;
	for (int face = 0; face < N; face++) {
		float r_eff, d;

		r_eff = Lx*A[face] + Ly*B[face] + Lz*C[face];
		d = dot_product(rc, nv[face]) + D[face];

		// completely outside
		if (d > r_eff) {
			return OCTREE_OUTSIDE;
		}

		// completely inside
		if (d < -r_eff) {
			count++;
		}
	}

	if (count == N) {
		return OCTREE_INSIDE;
	}
	else {
		return OCTREE_PARTIAL;
	}
}



template  <class T>
void
octree_node<T>::register_objects_to_draw_list(std::vector<int>& list) const
{
	for (std::vector<int>::const_iterator it = vObjIndex.begin(); it != vObjIndex.end(); it++) {
		list.push_back(*it);
	}
}



template  <class T>
void
octree_node<T>::register_all_to_draw_list(std::vector<int>& list) const
{
	register_objects_to_draw_list(list);

	for (int i = 0; i < 8; i++) {
		const octree_node<T>* p = child_nodes[i];
		if (p != 0) {
			p->register_all_to_draw_list(list);
		}
	}
}



template  <class T>
void
octree_node<T>::make_draw_objects_list(const vector3<T>* nv, const T* A, const T* B, const T*C, const T* D, bool bCullFar, std::vector<int>& list) const
{
	int res;

	res = visible_check(nv, A, B, C, D, bCullFar);

	if (res == OCTREE_OUTSIDE) {
		return;
	}

	if (res == OCTREE_INSIDE) {
		register_all_to_draw_list(list);
		return;
	}

	register_objects_to_draw_list(list);
	for (int i = 0; i < 8; i++) {
		const octree_node<T>* p = child_nodes[i];
		if (p != 0) {
			p->make_draw_objects_list(nv, A, B, C, D, bCullFar, list);
		}
	}
}





//----------
//  Octree
//----------
template  <class T>
class octree
{
	octree_node<T>* root_node;

public:
	octree() : root_node(0) {};
	octree(const bbox3<T>& _region);
	~octree();

	void	init(const bbox3<T>& _region);
	void	add_object(const int& obj, const bbox3<T>& bb);
	void	add_object(const int& obj, const vector3<T>& pos);

	void	make_draw_objects_list(const vector3<T>* nv, const T *D, bool bCullFar, std::vector<int>& list) const;

	const bbox3<T>& get_bbox() const { return root_node->get_bbox(); }
};



template  <class T>
octree<T>::octree(const bbox3<T>& _region)
{
	root_node = new octree_node(0, _region);
}



template  <class T>
octree<T>::~octree()
{
	if (root_node != 0) {
		delete root_node;
	}
}



template  <class T>
void
octree<T>::init(const bbox3<T>& _region)
{
	if (root_node != 0) {
		delete root_node;
	}

	root_node = new octree_node<T>(0, _region);
}



template  <class T>
void
octree<T>::add_object(const int& obj_idx, const bbox3<T>& bb)
{
	int lv;
	octree_node<T>* node;

	node = root_node;
	lv = 0;

	while (lv < octree_lv_max) {
		const bbox3<T>* octant = node->get_octant_bbox();

		bool bChild = false;
		for (int i = 0; i < 8; i++) {
			const bbox3<T>& octbb = octant[i];

			if (octbb.contains(bb)) {
				octree_node<T>** child_nodes = node->get_child_nodes();

				if (child_nodes[i] == 0) {
					child_nodes[i] = new octree_node<T>(node, octbb);
				}

				lv++;
				node = child_nodes[i];
				bChild = true;
				break;
			}
		}
		if (!bChild) {
			break;
		}
	}
	node->add_object(obj_idx);
}



template  <class T>
void
octree<T>::add_object(const int& obj_idx, const vector3<T>& pos)
{
	int lv;
	octree_node<T>* node;

	node = root_node;
	lv = 0;

	while (lv < octree_lv_max) {
		const bbox3<T>* octant = node->get_octant_bbox();

		bool bChild = false;
		for (int i = 0; i < 8; i++) {
			const bbox3<T>& octbb = octant[i];

			if (octbb.contains(pos)) {
				octree_node<T>** child_nodes = node->get_child_nodes();

				if (child_nodes[i] == 0) {
					child_nodes[i] = new octree_node<T>(node, octbb);
				}

				lv++;
				node = child_nodes[i];
				bChild = true;
				break;
			}
		}
		if (!bChild) {
			break;
		}
	}
	node->add_object(obj_idx);
}



template <class T>
void
octree<T>::make_draw_objects_list(const vector3<T>* nv, const T *D, bool bCullFar, std::vector<int>& list) const
{
	T A[6], B[6], C[6];
	for (int face = 0; face < 6; face++) {
		vector3<T>  _nv = T(0.5) * nv[face];

		A[face] = fabs(_nv.x);
		B[face] = fabs(_nv.y);
		C[face] = fabs(_nv.z);
	}

	root_node->make_draw_objects_list(nv, A, B, C, D, bCullFar, list);
}






#endif
