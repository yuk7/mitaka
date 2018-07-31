/**************************************************************************************************

Copyright (c) 2007   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#include  "mtk_object_position.h"
#include  "mtk_object.h"
#include  "mtk_object_orientation.h"

#include  "mtk_floating_object.h"
#include  "mtk_sun.h"
#include  "mtk_planet_position.h"
#include  "mtk_minor_planets.h"
#include  "mtk_stars.h"
#include  "mtk_galactic_center.h"
#include  "mtk_blackhole.h"
#include  "mtk_globular_clusters.h"
#include  "mtk_our_galaxy.h"
#include  "mtk_nearby_galaxies.h"
#include  "mtk_spacecraft.h"
#include  "mtk_scale_line.h"
#include  "mtk_label.h"
#include  "mtk_fixed_object.h"

#include  "mtk_planet_orientation.h"
#include  "mtk_satellites.h"
#include  "mtk_VERA.h"




void
update_objects_position(const double& t, bool force)
{
	update_planets_and_satellites_position(t, force);
	update_small_objects_position(t);
	//update_comets_position(t);
	update_spacecraft_position(t);
	update_S_stars_position(t);
}



vector3d
get_object_positon(const mtk_object&  obj)
{
	int idx = obj.get_index();
	switch ( obj.get_type() ) {
		case OBJ_TYPE_FLOATING_OBJECT:	return  get_floating_object_position();
		case OBJ_TYPE_SUN:				return  get_sun_position();
		case OBJ_TYPE_PLANET:			return  get_planet_position(idx);
		case OBJ_TYPE_SATELLITE:		return  get_satellite_position(idx);
		case OBJ_TYPE_SMALL_OBJECT:		return  get_small_object_position(idx);
		//case OBJ_TYPE_COMET:			return  get_comet_position(idx);
		case OBJ_TYPE_STAR:				return  get_star_position(idx);
		case OBJ_TYPE_S_STAR:			return	get_S_star_position(idx);
		case OBJ_TYPE_BLACKHOLE:		return	get_blackhole_position();
		case OBJ_TYPE_GLOBULAR_CLUSTER:	return  get_globular_cluster_position(idx);
		case OBJ_TYPE_VERA_OBJECT:		return  get_VERA_object_position(idx);
		case OBJ_TYPE_OUR_GALAXY:		return  get_our_galaxy_position();
		case OBJ_TYPE_NEARBY_GALAXY:	return  get_nearby_galaxy_position(idx);
		case OBJ_TYPE_SPACECRAFT:		return  get_spacecraft_position(idx);
		case OBJ_TYPE_OORT_CLOUD:		return  get_sun_position();

		case OBJ_TYPE_FIXED_OBJECT:		return  get_fixed_object_position(idx);
		case OBJ_TYPE_SCALE_CIRCLE:		return  get_scale_circle_position(idx);
		case OBJ_TYPE_SCALE_CIRCLE_G:	return  get_scale_circle_g_position(idx);
		case OBJ_TYPE_LABEL:			return  get_label_position(idx);
		default:		break;
	}

	return  vector3d(0,0,0);
}



void
update_objects_position_and_orientation(const double& t, bool force)
{
	//*** 天王星の衛星軌道面が惑星の姿勢に依存するので、姿勢の更新が先
	update_all_object_orientation(t);
	update_objects_position(t, force);
}

