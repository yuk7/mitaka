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
#include  "gl_triangle_mesh.h"



void
gl_draw_triangle_mesh(const TriMesh& tm, bool bNormal, bool bTexCoord, bool bSmooth,
					  int iBegin, int iEnd)
{
	if (!tm.support_normal())	bNormal	  = false;
	if (!tm.support_texture())	bTexCoord = false;
	if (!bNormal)				bSmooth	  = false;

	const int  tri_num = tm.get_tri_num();
	if (iBegin > iEnd) swap(iBegin, iEnd);
	if (iBegin>=tri_num || iEnd < 0)  return;
	if (iBegin < 0) iBegin = 0;
	if (iEnd > tri_num) iEnd = tri_num;

	const vector3f* pt = tm.get_vertex_pointer();
	const vector<vector3f>&  nv = tm.get_nv_pointer();
	const vector<vector2f>&  uv = tm.get_uv_pointer();
	glBegin( GL_TRIANGLES );
		for (int i=iBegin; i<iEnd; ++i) {
			const triangle&  tri = tm.get_triangle(i);
			if (bSmooth) {
				for (int j=0; j<3; ++j) {
					int pidx  = tri.pidx[j];
					int nidx  = tri.nidx[j];
					int uvidx = tri.uvidx[j];
					if (bNormal)	{
						if (nidx==-1) {	// 三角形の法線を使う場合
							glNormal3fv( tri.nv.get_pointer() );
						} else {
							const vector3f& _nv = nv[nidx];
							if (_nv.x >= -1.01f)	glNormal3fv( _nv.get_pointer() );
							else					glNormal3fv( tri.nv.get_pointer() );	// スムージングしない頂点
						}
					}
					if (bTexCoord)	glTexCoord2fv( uv[uvidx].get_pointer() );
					glVertex3fv( pt[pidx].get_pointer() );
				}
			} else {
				for (int j=0; j<3; ++j) {
					int pidx  = tri.pidx[j];
					int uvidx = tri.uvidx[j];
					if (bNormal)	glNormal3fv( tri.nv.get_pointer() );
					if (bTexCoord)	glTexCoord2fv( uv[uvidx].get_pointer() );
					glVertex3fv( pt[pidx].get_pointer() );
				}
			}
		}
	glEnd();
}
