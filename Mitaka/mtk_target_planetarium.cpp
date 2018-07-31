/**************************************************************************************************

Copyright (c) 2016   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#include  "mtk_target_planetarium.h"
#include  "mtk_planetarium.h"
#include  "mtk_sun.h"
#include  "mtk_planet.h"
#include  "mtk_planet_position.h"
#include  "mtk_planet_orientation.h"
#include  "mtk_satellites.h"



//***
extern float  landing_lon, landing_lat;


static int target_planetarium = TARGET_PLNT_NONE;





void
set_planetarium_target(int tgt)
{
	target_planetarium = tgt;
}



int
get_planetarium_target()
{
	return target_planetarium;
}



void
follow_planetarium_target_sub(const vector3d& tgt_pos, const vector3d& cam_pos)
{
	vector3d r, nv;

	r = tgt_pos - cam_pos;										// 惑星地表黄道座標でのターゲットの位置
	r = get_planet_orientation(PLNT_EARTH, true) * r;			// ec -> pl 
	r = Ry3deg(-90 + landing_lat) * Rz3deg(-landing_lon) * r;	// pl -> 地平座標
	nv = r.normalized();


	double rho, lon, lat;
	rho = sqrt(nv.x*nv.x + nv.y*nv.y);
	lat = atan2(nv.z, rho) / M_PI * 180.0;
	lon = atan2(nv.y, nv.x) / M_PI * 180.0;
	if (lat < 0) {
		lat = 0;
	}


	set_plnt_long(lon);
	set_plnt_lati(lat);
}



void
follow_planetarium_target(const vector3d& cam_pos)
{
	static bool first = true;
	static int idxMoon;

	if (first) {
		first = false;
		idxMoon = get_satellite_index("SAT_E_MOON");
	}


	vector3d tgt_pos;
	switch (target_planetarium) {
	case TARGET_PLNT_SUN:
		tgt_pos = get_sun_position();
		break;
	case TARGET_PLNT_MOON:
		tgt_pos = get_satellite_position(idxMoon);
		break;
	case TARGET_PLNT_JUPITER:
		tgt_pos = get_planet_position(PLNT_JUPITER);
		break;
	case TARGET_PLNT_NONE:
	default:
		return;
	}

	follow_planetarium_target_sub(tgt_pos, cam_pos);
}

