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
#include  "mtk_object_orientation.h"
#include  "../common/astro/astrometry.h"
#include  "mtk_planet_orientation.h"
#include  "mtk_satellite_orientation.h"
#include  "mtk_minor_planet_orientation.h"
#include  "mtk_OpenGL.h"
#include  <vector>
#include  <map>





struct orientation {
	int obj_type;
	int ori_id;
	matrix3d  M;	// EC to OBJ matrix
	matrix3d  M0;	// without rotation
};



//---  Objects
static std::vector<orientation> vOri;
static std::map<std::string, int> mOri;





int
add_orientation(const std::string& key, int obj_type, int ori_id)
{
	orientation ori;
	ori.obj_type = obj_type;
	ori.ori_id = ori_id;
	vOri.push_back(ori);

	int idx;
	idx = vOri.size() - 1;
	mOri[key] = idx;

	return idx;
}


int
get_orientation_index(const std::string& key)
{
	std::map<std::string, int>::const_iterator it = mOri.find(key);

	if (it == mOri.end()) {
		return -1;
	}

	return it->second;
}






void
update_orientation(int ori_idx, const double& t)
{
	if (ori_idx < 0 || ori_idx >= vOri.size()) {
		return;
	}



	orientation& ori = vOri[ori_idx];

	switch (ori.obj_type) {
	case ORIENTATION_PLANET:
		calc_planet_ec_to_pl_matrix(ori.ori_id, t, ori.M0, ori.M);
		break;
	case ORIENTATION_SATELLITE:
		calc_satellite_ec_to_sat_matrix(ori.ori_id, t, ori.M0, ori.M);
		break;
	case ORIENTATION_SMALL_OBJECT:
		calc_minor_planet_ec_to_mp_matrix(ori.ori_id, t, ori.M0, ori.M);
		break;
	default:
		break;
	}
}


void
update_all_object_orientation(const double& t)
{
	const double DT_MIN = 0.01 / (24.0*60.0*60.0);
	static bool first = true;
	static double t_last;

	if (first)	{ first = false; }
	else { if (fabs(t - t_last) < DT_MIN)  return; }


	for (int i = 0; i < vOri.size(); i++) {
		update_orientation(i, t);
	}

	t_last = t;
}



const matrix3d&
get_orientation(int ori_idx, bool bRotation)
{
	static const matrix3d I(1, 0, 0, 0, 1, 0, 0, 0, 1);

	if (ori_idx < 0 || ori_idx >= vOri.size()) {
		return I;
	}

	if (bRotation) {
		return vOri[ori_idx].M;
	}
	else {
		return vOri[ori_idx].M0;
	}
}





//---------------
//  Orientation
//---------------

//-- 黄道座標系から天体座標系へ移る
void
set_object_orientation(const matrix3d& MT)
{
	float M[16];

	M[0] = MT.m11;
	M[1] = MT.m21;
	M[2] = MT.m31;
	M[3] = 0.0;

	M[4] = MT.m12;
	M[5] = MT.m22;
	M[6] = MT.m32;
	M[7] = 0.0;

	M[8] = MT.m13;
	M[9] = MT.m23;
	M[10] = MT.m33;
	M[11] = 0.0;

	M[12] = 0.0;
	M[13] = 0.0;
	M[14] = 0.0;
	M[15] = 1.0;

	glMultMatrixf(M);
}


// 自転軸の向きに合わせる
void
set_object_orientation_GL(int ori_idx)
{
	if (ori_idx >= 0) {
		set_object_orientation(get_orientation(ori_idx, true).transposed());
	}
}

