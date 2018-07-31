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
#ifndef __MTK_TRAJECTORY_H__
#define __MTK_TRAJECTORY_H__
#include  "../common/math/vector3.h"
#include <vector>




//----------------------------
//  class trajectory_segment
//----------------------------

class trajectory_segment {
	double _t[4], _x[4], _y[4], _z[4];

public:
	bool	read_data(FILE *fp);
	void	scale_pos(const double& fac);

	bool	is_inside(const double& t) const;
	double	get_t_begin() const		{ return _t[0]; }
	double	get_t_end() const		{ return _t[3]; }

	vector3d get_pos(const double& t) const;
};


inline bool
trajectory_segment::is_inside(const double& t) const
{
	return (_t[0] <= t && t <= _t[3]);
}





//------------------------
//  class mtk_trajectory
//------------------------

class mtk_trajectory {
	std::vector<trajectory_segment> vSeg;
	std::vector<int>				vSegTable;
	bool bLoaded;
	double t_begin, t_end;

	int		find_segment(const double& t, int idx1, int idx2) const;
	void	make_index_table(int N);

public:
	mtk_trajectory() : bLoaded(false), t_begin(0), t_end(0) {};

	int		find_segment(const double& t) const;
	bool	load(const char* fn, int Ntbl);

	bool		isLoaded() const		{ return bLoaded; }
	double		get_t_begin() const		{ return t_begin; }
	double		get_t_end() const		{ return t_end; }
	bool		isInRange(const double& t)	{ return (t >= t_begin && t <= t_end); }

	int			get_segment_num() const	{ return vSeg.size(); }
	const trajectory_segment&	get_segment(int idx) const  { return vSeg[idx]; }

	vector3d	get_pos(const double& t) const;
};



#endif
