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
#ifndef  __QUADTREE_H__
#define  __QUADTREE_H__
#include  "../math/vector3.h"
#include  "bounding_box.h"
#include  <vector>



//const int lv_max = 8;
const int lv_max = 6;


enum {
	QUADTREE_INSIDE,
	QUADTREE_PARTIAL,
	QUADTREE_OUTSIDE
};



//-----------------
//  Quadtree node
//-----------------
template  <class T>
class quadtree_node
{
	bbox2<T>	region;
	bbox2<T>	quadrant[4];
	const quadtree_node<T> *parent_node;
	quadtree_node<T> *child_nodes[4];
	std::vector<int> vObjIndex;
	T Lx, Ly;
	vector3<T> rc;

public:
	quadtree_node(const quadtree_node<T>* _parent_node, const bbox2<T>& _region);
	~quadtree_node();

	const bbox2<T>& get_bbox() const { return region; }
	const bbox2<T>* get_octant_bbox() const { return quadrant; }
	quadtree_node<T>** get_child_nodes() { return child_nodes; }

	void	add_object(const int& obj_idx);

	int		visible_check(const vector3<T>* nv, const T* A, const T* B, const T*C, const T* D, bool bCullFar) const;

	void	register_objects_to_draw_list(std::vector<int>& list) const;
	void	register_all_to_draw_list(std::vector<int>& list) const;
	void	make_draw_objects_list(const vector3<T>* nv, const T* A, const T* B, const T*C, const T* D, bool bCullFar, std::vector<int>& list) const;
};



template  <class T>
quadtree_node<T>::quadtree_node(const quadtree_node<T>* _parent_node, const bbox2<T>& _region) : parent_node(_parent_node), region(_region)
{
	// clear child nodes
	for (int i = 0; i < 4; i++) {
		child_nodes[i] = 0;
	}

	// calc octant
	T x1, x2, y1, y2, xc, yc;
	x1 = region.x1;
	x2 = region.x2;
	y1 = region.y1;
	y2 = region.y2;
	xc = T(0.5) * (x1 + x2);
	yc = T(0.5) * (y1 + y2);

	Lx = x2 - x1;
	Ly = y2 - y1;

	rc.set(xc, yc, 0);

	quadrant[0].set(x1, xc, y1, yc);
	quadrant[1].set(xc, x2, y1, yc);
	quadrant[2].set(x1, xc, yc, y2);
	quadrant[3].set(xc, x2, yc, y2);
}



template  <class T>
quadtree_node<T>::~quadtree_node()
{
	for (int i = 0; i < 4; i++) {
		if (child_nodes[i] != 0) {
			delete child_nodes[i];
		}
	}
}



template  <class T>
void
quadtree_node<T>::add_object(const int& obj_idx)
{
	vObjIndex.push_back(obj_idx);
}



template  <class T>
int
quadtree_node<T>::visible_check(const vector3<T>* nv, const T* A, const T* B, const T*C, const T* D, bool bCullFar) const
{
	const int N = (bCullFar) ? 6 : 5;

	int count = 0;
	for (int face = 0; face < N; face++) {
		float r_eff, d;

		r_eff = Lx*A[face] + Ly*B[face];
		d = dot_product(rc, nv[face]) + D[face];

		// completely outside
		if (d > r_eff) {
			return QUADTREE_OUTSIDE;
		}

		// completely inside
		if (d < -r_eff) {
			count++;
		}
	}

	if (count == N) {
		return QUADTREE_INSIDE;
	}
	else {
		return QUADTREE_PARTIAL;
	}
}



template  <class T>
void
quadtree_node<T>::register_objects_to_draw_list(std::vector<int>& list) const
{
	for (std::vector<int>::const_iterator it = vObjIndex.begin(); it != vObjIndex.end(); it++) {
		list.push_back(*it);
	}
}



template  <class T>
void
quadtree_node<T>::register_all_to_draw_list(std::vector<int>& list) const
{
	register_objects_to_draw_list(list);

	for (int i = 0; i < 4; i++) {
		const quadtree_node<T>* p = child_nodes[i];
		if (p != 0) {
			p->register_all_to_draw_list(list);
		}
	}
}



template  <class T>
void
quadtree_node<T>::make_draw_objects_list(const vector3<T>* nv, const T* A, const T* B, const T*C, const T* D, bool bCullFar, std::vector<int>& list) const
{
	int res;

	res = visible_check(nv, A, B, C, D, bCullFar);

	if (res == QUADTREE_OUTSIDE) {
		return;
	}

	if (res == QUADTREE_INSIDE) {
		register_all_to_draw_list(list);
		return;
	}

	register_objects_to_draw_list(list);
	for (int i = 0; i < 4; i++) {
		const quadtree_node<T>* p = child_nodes[i];
		if (p != 0) {
			p->make_draw_objects_list(nv, A, B, C, D, bCullFar, list);
		}
	}
}





//------------
//  Quadtree
//------------
template  <class T>
class quadtree
{
	quadtree_node<T>* root_node;
	matrix3<T> M;
	vector3<T> t;

public:
	quadtree() : root_node(0), M(1,0,0,  0,1,0,  0,0,1), t(0,0,0) {};
	quadtree(const bbox2<T>& _region);
	~quadtree();

	void	init(const bbox2<T>& _region);
	void	set_axes_and_origin(const vector3<T>& ex, const vector3<T>& ey, const vector3<T>& ez, const vector3<T>& r0);
	void	add_object(const int& obj_idx, const bbox2<T>& bb);
	void	add_object(const int& obj_idx, const vector2<T>& pos);

	void	make_draw_objects_list(const vector3<T>* nv, const T *D, bool bCullFar, std::vector<int>& list) const;
};



template  <class T>
quadtree<T>::quadtree(const bbox2<T>& _region)
{
	root_node = new quadtree_node(0, _region);
}



template  <class T>
quadtree<T>::~quadtree()
{
	if (root_node != 0) {
		delete root_node;
	}
}



template  <class T>
void
quadtree<T>::init(const bbox2<T>& _region)
{
	if (root_node != 0) {
		delete root_node;
	}

	root_node = new quadtree_node<T>(0, _region);
}


template  <class T>
void
quadtree<T>::set_axes_and_origin(const vector3<T>& ex, const vector3<T>& ey, const vector3<T>& ez, const vector3<T>& r0)
{
	M.set(ex.x, ex.y, ex.z, ey.x, ey.y, ey.z, ez.x, ez.y, ez.z);
	t = -M * r0;
}


template  <class T>
void
quadtree<T>::add_object(const int& obj_idx, const bbox2<T>& bb)
{
	int lv;
	quadtree_node<T, T2>* node;

	node = root_node;
	lv = 0;

	while (lv < lv_max) {
		const bbox2<T>* quadrant = node->get_octant_bbox();

		bool bChild = false;
		for (int i = 0; i < 4; i++) {
			const bbox2<T>& quadbb = quadrant[i];

			if (quadbb.contains(bb)) {
				quadtree_node<T, T2>** child_nodes = node->get_child_nodes();

				if (child_nodes[i] == 0) {
					child_nodes[i] = new quadtree_node<T, T2>(node, quadbb);
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
	node->add_object(obj);
}



template  <class T>
void
quadtree<T>::add_object(const int& obj_idx, const vector2<T>& pos)
{
	int lv;
	quadtree_node<T>* node;

	node = root_node;
	lv = 0;

	while (lv < lv_max) {
		const bbox2<T>* quadrant = node->get_octant_bbox();

		bool bChild = false;
		for (int i = 0; i < 4; i++) {
			const bbox2<T>& quadbb = quadrant[i];

			if (quadbb.contains(pos)) {
				quadtree_node<T>** child_nodes = node->get_child_nodes();

				if (child_nodes[i] == 0) {
					child_nodes[i] = new quadtree_node<T>(node, quadbb);
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
quadtree<T>::make_draw_objects_list(const vector3<T>* nv, const T *D, bool bCullFar, std::vector<int>& list) const
{
	// world -> quadtree plane
	vector3<T> nvp[6];
	T Dp[6];

	for (int face = 0; face < 6; face++) {
		vector3<T> v;
		T vnorm;

		v = M * nv[face];
		vnorm = norm(v);

		nvp[face] = v / vnorm;
		Dp[face] = D[face] / vnorm - dot_product(t, nvp[face]);
	}


	// plane parameter
	T Ap[6], Bp[6], Cp[6];
	for (int face = 0; face < 6; face++) {
		vector3<T>  _nv = T(0.5) * nvp[face];

		Ap[face] = fabs(_nv.x);
		Bp[face] = fabs(_nv.y);
		Cp[face] = fabs(_nv.z);
	}

	root_node->make_draw_objects_list(nvp, Ap, Bp, Cp, Dp, bCullFar, list);
}



#endif
