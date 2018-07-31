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
#include  "triangulation.h"
#include  <vector>
#include  <list>
using namespace std;


//------------
//  Triangle
//------------
class dtTriangle  :  public Triangle
{
	vector2f	rc;				// circumcircle center
	float		Rc, Rc2;		// square of circumcircle radius
	float		xmax;
public:
	dtTriangle() {}
	void	calcCircumCircle(const vector2f* vVtx);
	int		checkCircumCircle(const vector2f& r);
	vector2f	get_rc() const		{ return rc; }
	float		get_Rc() const		{ return Rc; }
};


inline void
dtTriangle::calcCircumCircle(const vector2f* vVtx)
{
	const vector2f r1 = vVtx[p[0]];
	const vector2f r2 = vVtx[p[1]];
	const vector2f r3 = vVtx[p[2]];
	const vector2f v1 = r2 - r1;
	const vector2f v2 = r3 - r1;
	const vector2f v3 = r3 - r2;
	const vector2f vp(-v1.y, v1.x);
	const vector2f dr = 0.5f*v1 + 0.5f *(dot_product(v3,v2) / dot_product(vp,v2) ) * vp;
	Rc2 = norm2(dr);
	Rc  = sqrt(Rc2);
	rc  = r1 + dr;
	const float D = 1.0001f * Rc;
	xmax = rc.x + D;
}


inline  int
dtTriangle::checkCircumCircle(const vector2f& r)
{
	if (norm2(r-rc) <= Rc2) {
		return 0;
	}
	
	if (xmax < r.x) {
		return 2;
	}

	return 1;
}


//--------
//  Edge
//--------
struct  dtEdge {
	int p1, p2;
};


//-------------
//  Temporary
//-------------
vector<dtTriangle>	vTri;
vector<dtEdge>		vEdge(1000);


//====================================
//  Delaunay Triangulation
//------------------------------------
//  vVtx は n+3 要素分のスペースが必要
//------------------------------------
int
DelaunayTriangulation(const int n, vector2f* vVtx, Triangle* vtri)
{
	const int tri_order[3] = {1, 2, 0};
	if (vTri.size() < 3*n) {
		vTri.resize(3*n);
	}

	//-----------------
	//  Supertriangle
	//-----------------
	float xmin, xmax, ymin, ymax;
	xmin = xmax = vVtx[0].x;
	ymin = ymax = vVtx[0].y;
	for (int i=1; i<n; i++) {
		const vector2f& r = vVtx[i];
		const float x = r.x;
		const float y = r.y;

		if (x < xmin)		xmin = x;
		else if (x > xmax)	xmax = x;
		if (y < ymin)		ymin = y;
		else if (y > ymax)	ymax = y;
	}

	// 頂点を登録
	float dx = xmax - xmin;
	float dy = ymax - ymin;
	float x1 = xmin - 0.1*dx;
	float y1 = ymin - 0.1*dy;
	float x2 = xmax + 1.1*dx;
	float y2 = ymax + 1.1*dy;
	vVtx[n  ].x = x1;
	vVtx[n  ].y = y1;
	vVtx[n+1].x = x2;
	vVtx[n+1].y = y1;
	vVtx[n+2].x = x1;
	vVtx[n+2].y = y2;

	// 三角形を登録
	vTri[0].p[0] = n;
	vTri[0].p[1] = n+1;
	vTri[0].p[2] = n+2;
	vTri[0].calcCircumCircle(vVtx);
	
	int nTri  = 1;	// work用
	int nTri2 = 0;	// 出力用
	//--------------------------
	//  Delaunay Triangulation
	//--------------------------
	for (int i=0; i<n; i++) {
		vector2f r = vVtx[i];

		//------------------------------------------
		//  外接円内に新しい点を含む三角形の辺を登録する
		//------------------------------------------
		int edge_num = 0;
		for (int j=0; j<nTri; j++) {
			int stat = vTri[j].checkCircumCircle(r);
			if (stat==1) continue;
			dtTriangle&  tri = vTri[j];
			if (stat==2) {
				if (tri.p[0] < n && tri.p[1] < n && tri.p[2] < n) {
					// 出力用配列にコピー
					vtri[nTri2++] = vTri[j];
				}
				// その三角形は削除
				vTri[j] = vTri[nTri-1];
				nTri--;
				j--;
				continue;
			}

			//-------------------
			//  ３つの辺を登録する
			//-------------------
			int p2;
			p2 = tri.p[0];
			for (int k=0; k<3; k++) {
				int p1 = p2;
				p2 = tri.p[ tri_order[k] ];
				
				bool reg = true;
				for (int l=0; l<edge_num; l++) {
					if (vEdge[l].p1 == p2 && vEdge[l].p2 == p1) {
						// すでに登録されている -> それを削除
						vEdge[l].p1 = -1;
						reg = false;
						break;
					}
				}
				if (reg) {
					// 登録されていない -> 登録する
					if (vEdge.size() <= edge_num) {
						vEdge.resize( 2*edge_num );
					}
					
					vEdge[edge_num].p1 = p1;
					vEdge[edge_num].p2 = p2;
					edge_num++;
				}
			}
			
			//------------------
			//  その三角形を削除
			//------------------
			vTri[j] = vTri[nTri-1];		// 一番後ろの要素を持ってくる
			nTri--;
			j--;
		}
		
		//------------------------------------------
		//  登録された辺と新しい点で作られる三角形を登録
		//------------------------------------------
		for (int l=0; l<edge_num; l++) {
			if (vEdge[l].p1 == -1)  continue;
			dtTriangle&  tri = vTri[nTri];
			tri.p[0] = vEdge[l].p1;
			tri.p[1] = vEdge[l].p2;
			tri.p[2] = i;
			tri.calcCircumCircle(vVtx);
			nTri++;
		}
	}

	// 残っているものをコピー
	for (int i=0; i<nTri; i++) {
		const dtTriangle&  tri = vTri[i];
		if (tri.p[0] < n && tri.p[1] < n && tri.p[2] < n) {
			vtri[nTri2++] = vTri[i];
		}
	}

	return  nTri2;
}
