/**************************************************************************************************

Copyright (c) 2015   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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

以下に定める条件に従い、本ソフトウェアおよび関連文書のファイル（以下「ソフトウェア」）の複製を取得す
るすべての人に対し、ソフトウェアを無制限に扱うことを無償で許可します。これには、ソフトウェアの複製を
使用、複写、変更、結合、掲載、頒布、サブライセンス、および/または販売する権利、およびソフトウェアを
提供する相手に同じことを許可する権利も無制限に含まれます。

上記の著作権表示および本許諾表示を、ソフトウェアのすべての複製または重要な部分に記載するものとしま
す。

ソフトウェアは「現状のまま」で、明示であるか暗黙であるかを問わず、何らの保証もなく提供されます。ここ
でいう保証とは、商品性、特定の目的への適合性、および権利非侵害についての保証も含みますが、それに限定
されるものではありません。 作者または著作権者は、契約行為、不法行為、またはそれ以外であろうと、ソフ
トウェアに起因または関連し、あるいはソフトウェアの使用またはその他の扱いによって生じる一切の請求、損
害、その他の義務について何らの責任も負わないものとします。
***************************************************************************************************/
#include  "mtk_trajectory.h"
#include  "../common/astro/astro_const.h"




// polint: from NR 3.1
double
polint(const double xa[], const double ya[], int n, double x)
{
	int ns = 0;
	double y, dy, den, dif, dift, ho, hp, w;
	double c[4], d[4];

	dif = fabs(x - xa[0]);


	for (int i = 0; i < n; i++) {
		dift = fabs(x - xa[i]);
		if (dift < dif) {
			ns = i;
			dif = dift;
		}
		c[i] = ya[i];
		d[i] = ya[i];
	}

	y = ya[ns--];
	for (int m = 0; m < n - 1; m++) {
		for (int i = 0; i < n - m - 1; i++) {
			ho = xa[i] - x;
			hp = xa[i + m + 1] - x;
			w = c[i + 1] - d[i];
			den = ho - hp;
			if (den == 0.0) {
				return 0; // Error 
			}

			den = w / den;
			d[i] = hp*den;
			c[i] = ho*den;
		}

		dy = (2 * (ns + 1) < n - m - 1) ? c[ns + 1] : d[ns--];
		y += dy;
	}

	return y;
}




//----------------------------
//  class trajectory_segment
//----------------------------

bool
trajectory_segment::read_data(FILE *fp)
{
	if (fread(_t, sizeof(double), 4, fp) != 4)  return false;
	if (fread(_x, sizeof(double), 4, fp) != 4)  return false;
	if (fread(_y, sizeof(double), 4, fp) != 4)  return false;
	if (fread(_z, sizeof(double), 4, fp) != 4)  return false;

	return true;
}


void
trajectory_segment::scale_pos(const double& fac)
{
	for (int i = 0; i < 4; i++) {
		_x[i] *= fac;
		_y[i] *= fac;
		_z[i] *= fac;
	}
}


vector3d
trajectory_segment::get_pos(const double& t) const
{
	if (!is_inside(t)) {
		return vector3d(0, 0, 0);	// Error
	}


	vector3d pos;
	pos.x = polint(_t, _x, 4, t);
	pos.y = polint(_t, _y, 4, t);
	pos.z = polint(_t, _z, 4, t);

	return pos;
}




//------------------------
//  class mtk_trajectory
//------------------------

int
mtk_trajectory::find_segment(const double &t, int idx1, int idx2) const
{
	// 範囲内かどうかをチェック
	if (t < vSeg[idx1].get_t_begin())	{ return -1; }
	if (t > vSeg[idx2].get_t_end())		{ return -1; }

	// t を含むセグメントを二分法により見つける
	while (true) {
		int idxm;

		idxm = (idx1 + idx2) / 2;

		const trajectory_segment& ts = vSeg[idxm];
		if (ts.is_inside(t)) {
			return idxm;
		}

		if (idx2 == idx1 + 1) {
			return idx2;
		}

		if (t < ts.get_t_begin()) {
			idx2 = idxm;
		}

		if (t > ts.get_t_end()) {
			idx1 = idxm;
		}
	}
}


void
mtk_trajectory::make_index_table(int N)
{
	int Nseg;

	Nseg = vSeg.size();
	t_begin = vSeg[0].get_t_begin();
	t_end   = vSeg[Nseg - 1].get_t_end();

	int idx1, idx2;
	idx1 = 0;
	idx2 = Nseg - 1;

	double dt;
	dt = (t_end - t_begin) / N;

	vSegTable.resize(N + 1);
	for (int i = 0; i < N; i++) {
		double ti;
		int idx;

		ti = i*dt + t_begin;
		idx = find_segment(ti, idx1, idx2);
		vSegTable[i] = idx;

		idx1 = idx;
	}
	vSegTable[N] = idx2;
}


int
mtk_trajectory::find_segment(const double& t) const
{
	int N, idx_end, i, idx1, idx2;

	N = vSegTable.size() - 1;
	idx_end = vSeg.size() - 1;
	i = (t - t_begin) / (t_end - t_begin) * N;

	if (i<0 || i > N) { return -1; }
	if (i == N) { return idx_end; }

	idx1 = vSegTable[i];
	idx2 = vSegTable[i + 1];
	if (idx1 == idx2)  return idx1;

	return find_segment(t, idx1, idx2);
}


bool
mtk_trajectory::load(const char *fn, int Ntbl)
{
	FILE *fp;
	if ((fp = fopen(fn, "rb")) == NULL)  return  false;


	vSeg.clear();
	while (true) {
		trajectory_segment  ts;

		if (!ts.read_data(fp)) {
			break;
		}
		ts.scale_pos(_1km_pc);	// 単位を km から pc にする

		vSeg.push_back(ts);
	}
	fclose(fp);

	make_index_table(Ntbl);
	bLoaded = true;

	return  true;
}


vector3d
mtk_trajectory::get_pos(const double& t) const
{
	if (!bLoaded) {
		return vector3d(0, 0, 0);
	}

	if (t <= t_begin)	{ return vSeg[0].get_pos(t_begin); }
	if (t >= t_end)		{ return vSeg[ vSeg.size() - 1 ].get_pos(t_end); }

	int idx;
	idx = find_segment(t);
	if (idx == -1) {
		return  vector3d(0, 0, 0);
	}

	return  vSeg[idx].get_pos(t);
}
