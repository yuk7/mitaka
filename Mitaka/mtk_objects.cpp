/**************************************************************************************************

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
#include  "mtk_objects.h"
#include  "mtk_solar_system.h"
#include  "mtk_spacecraft.h"
#include  "mtk_stars_2.h"
#include  "mtk_galactic_center.h"
#include  "mtk_nearby_galaxies.h"
#include  "mtk_label.h"
#include  "mtk_fixed_object.h"
#include  <vector>
#include  <map>
#include  <string>
#include  <algorithm>

#include  "mtk_stars.h"
#include  "mtk_minor_planets.h"

#include  "mtk_point_block.h"
#include  "mtk_switches.h"




//---------------
//  z_list_elem
//---------------
class z_list_elem
{
public:
	int		idx;	// idx
	float	z;		// z-値

	bool operator<(const z_list_elem& zl) const  { return z < zl.z; }
};




//---  全オブジェクトのベクタ  ---
static std::vector<mtk_object>	vObject;

//---  オブジェクト検索用のキー  ---
static std::map<std::string, int>  vObjectKey;



//---  表示用リスト ---
static std::vector<z_list_elem>	vZ_near, vZ_far;
static int num_near, num_far;

//---  選択用リスト  ---
static std::vector<z_list_elem>	vZ_tgt;
static int num_tgt;

static int selected_object_index = -1;
static int hit_mx, hit_my;







//--------
//  init
//--------
void
clear_objects()
{
	vObject.clear();
	vObjectKey.clear();

	vZ_near.clear();
	vZ_far.clear();
	vZ_tgt.clear();
}


int
register_object(const mtk_object& obj, const std::string& key)
{
	vObject.push_back(obj);

	int ID;
	ID = vObject.size() - 1;

	vObjectKey[key] = ID;

	return ID;
}



//-------
//  Get
//-------
const mtk_object&
get_object(int idx)
{
	return  vObject[idx];
}


int
get_object_ID(const std::string& key)
{
	std::map<std::string, int>::iterator  it = vObjectKey.find( std::string(key) );
	if (it == vObjectKey.end())  return  -1;
	return  it->second;
}


const mtk_object&
get_object(const std::string& key)
{
	int ID = get_object_ID(key);

	//********** Error  **********//
	if (ID == -1)  return  vObject[0];

	return  vObject[ID];
}



//-------------------
//  Selected object
//-------------------
bool
is_there_selected_object()
{
	return  (selected_object_index != -1);
}

int
get_selected_object_ID()
{
	return  selected_object_index;
}

const mtk_object&
get_selected_object()
{
	if (selected_object_index == -1) {
		return  vObject[0];	//***** Error *****
	}

	return  vObject[ selected_object_index ];
}

void
reset_selected_object()
{
	selected_object_index = -1;
}


void
get_object_hit_pos(int* pmx, int* pmy)
{
	*pmx = hit_mx;
	*pmy = hit_my;
}



//--------
//  Draw
//--------
void
prepare_draw_objects(const mtk_draw_struct* v_ds, int Nds)
{
	static bool first = true;
	const int N = vObject.size();
	const float z_near = get_z_near();
	const float z_mid  = get_z_mid();
	const float z_far  = get_z_far();

	const mtk_draw_struct& ds = v_ds[0];



	vector3f  eRf, eUf;
	eRf = ds.eRf;
	eUf = ds.eUf;
	//***
	const vector3d  cpos = ds.camera_base.get_pos();
	//***



	//---------------
	//  Init z-list
	//---------------
	num_near = num_far = num_tgt = 0;

	if (vZ_near.size() < N) {
		vZ_near.resize(N);
	}
	if (vZ_far.size() < N) {
		vZ_far.resize(N);
	}
	if (vZ_tgt.size() < N) {
		vZ_tgt.resize(N);
	}





	//-----------------------------------
	//  Register objects to the z_lists
	//-----------------------------------
	
	
	//-- For stars
	prepare_draw_stars_2(ds);
	prepare_draw_S_stars_2(ds);



	
	if ((swStar & STAR_DISP) && ds.cnorm <= 1.5e5) {
		
		const vector<int>& list = get_stars_draw_list(ds);

		for (int i = 0; i < list.size(); i++) {

			const int obj_idx = list[i];
			mtk_object& obj = vObject[obj_idx];

			int  draw_rgn = obj.prepare_draw(v_ds, Nds, z_near, z_mid, z_far);
			if (draw_rgn == OBJ_DRAW_NONE)  continue;

			float z = obj.get_draw_z() + obj.get_dz();



			//--  Near region
			if (draw_rgn == OBJ_DRAW_NEAR || draw_rgn == OBJ_DRAW_BOTH) {
				vZ_near[num_near].idx = obj_idx;
				vZ_near[num_near].z = z;
				num_near++;
			}


			//--  Far region
			if (draw_rgn == OBJ_DRAW_FAR || draw_rgn == OBJ_DRAW_BOTH) {
				vZ_far[num_far].idx = obj_idx;
				vZ_far[num_far].z = z;
				num_far++;
			}


			//--  Target list
			if (obj.get_hit_test_status() != OBJ_HITTEST_STAT_NONE) {
				vZ_tgt[num_tgt].idx = obj_idx;
				vZ_tgt[num_tgt].z = z;
				num_tgt++;
			}



			//--  pre calc billboard orientation
			if (!ds.bRotateBillboard) {
				continue;
			}


			int idx = obj.get_index();
			vector3d rpos = obj.get_pos() - cpos;

			// ベースカメラを向いたビルボード
			vector3f  eF2, eU2, eR2;
			eF2 = normalize(rpos);
			eR2 = normalize(cross_product(eF2, eUf));
			eU2 = cross_product(eR2, eF2);


			mtk_star&  st = get_star(idx);
			st.eR = eR2;
			st.eU = eU2;
		}
	}
	



	//-- For other objects
	for (int i = 0; i<N; i++) {
		mtk_object& obj = vObject[i];
		

		const int obj_ty = obj.get_type();
		//***
		if (obj_ty == OBJ_TYPE_STAR) {
			continue;
		}
		//***


		int  draw_rgn = obj.prepare_draw(v_ds, Nds, z_near, z_mid, z_far);
		if (draw_rgn == OBJ_DRAW_NONE)  continue;


		float z = obj.get_draw_z() + obj.get_dz();

		//--  Near region
		if (draw_rgn == OBJ_DRAW_NEAR || draw_rgn == OBJ_DRAW_BOTH) {
			vZ_near[num_near].idx = i;
			vZ_near[num_near].z   = z;
			num_near++;
		}

		//--  Far region
		if (draw_rgn == OBJ_DRAW_FAR || draw_rgn == OBJ_DRAW_BOTH) {
			vZ_far[num_far].idx = i;
			vZ_far[num_far].z   = z;
			num_far++;
		}

		//--  Target list
		if ( obj.get_hit_test_status() != OBJ_HITTEST_STAT_NONE ) {
			vZ_tgt[num_tgt].idx = i;
			vZ_tgt[num_tgt].z   = z;
			num_tgt++;
		}



	
		//--  pre calc billboard orientation
		if (!ds.bRotateBillboard) {
			continue;
		}


		if (obj_ty != OBJ_TYPE_STAR && obj_ty != OBJ_TYPE_SMALL_OBJECT && obj_ty != OBJ_TYPE_NEARBY_GALAXY) {
			continue;
		}



		int obj_idx = obj.get_index();
		vector3d rpos = obj.get_pos() - cpos;

		// ベースカメラを向いたビルボード
		vector3f  eF2, eU2, eR2;
		eF2 = normalize(rpos);
		eR2 = normalize(cross_product(eF2, eUf));
		eU2 = cross_product(eR2, eF2);

		if (obj_ty == OBJ_TYPE_SMALL_OBJECT) {
			small_object&  sm = get_small_object(obj_idx);

			sm.eR = eR2;
			sm.eU = eU2;
		}
		else if (obj_ty == OBJ_TYPE_NEARBY_GALAXY) {
			nearby_galaxy2& ng = get_nearby_galaxy(obj_idx);

			ng.eR = eR2;
			ng.eU = eU2;
		}
		
	}

	
	//---  z-sort  ---
	std::sort(vZ_near.begin(), vZ_near.begin() + num_near);
	std::sort(vZ_far.begin(),  vZ_far.begin()  + num_far);
	std::sort(vZ_tgt.begin(), vZ_tgt.begin() + num_tgt);
	


	/*
	{
		char str[256];
		sprintf(str, "near=%d  far=%d  tgt=%d\n", num_near, num_far, num_tgt);
		OutputDebugString(str);
	}
	*/
}





int
get_draw_object_num_near()
{
	return  num_near;
}

int
get_draw_object_num_far()
{
	return  num_far;
}

int
get_target_object_num()
{
	return  num_tgt;
}




//--------
//  Draw
//--------
void
draw_objects(const mtk_draw_struct& ds, bool bNear)
{
	const std::vector<z_list_elem>&  zl = (bNear)  ?  vZ_near  :  vZ_far;
	const int num  =  (bNear)  ?  num_near  :  num_far;

	glPushAttrib(GL_ALL_ATTRIB_BITS);


	//---  遠い方から描画する  ---
	int obj_ty_prev = -1;
	int draw_stat_prev = -1;
	for (int i=num-1; i>=0; i--) {
		int idx = zl[i].idx;
		bool flg = (idx == selected_object_index);

		int obj_ty = vObject[idx].get_type();
		int draw_stat = vObject[idx].get_draw_status();

		bool bChange = ((obj_ty_prev != obj_ty) || (draw_stat_prev != draw_stat));
		vObject[idx].draw(ds, flg, bChange);
		
		obj_ty_prev = obj_ty;
		draw_stat_prev = draw_stat;
	}


	glPopAttrib();

	glDisable(GL_TEXTURE_2D);
}




//-------------
//  Hit test
//-------------
bool
hit_test_objects(const mtk_draw_struct& ds, int mx, int my)
{
	int sel_obj_idx_old = selected_object_index;
	selected_object_index = -1;

	hit_mx = mx;
	hit_my = my;



	//--  カメラの方向ベクトルを得る
	vector3d  eF, eU, eR;

	
	ds.camera.get_axes(eF, eU, eR);
	
	
	//--  x,y 方向の方向ベクトル
	const frustum frs = ds.get_frustum();
	const float x1 = frs.get_x1();
	const float x2 = frs.get_x2();
	const float y1 = frs.get_y1();
	const float y2 = frs.get_y2();
	const float z1 = frs.get_z1();
	const float sclx = (x2 - x1) / ds.vp_w;
	const float scly = (y2 - y1) / ds.vp_h;
	const float xx = sclx*float(mx - ds.vp_x1) + x1;
	const float yy = scly*float(my - ds.vp_y1) + y1;
	const vector3d n0 = normalize(vector3d(xx, yy, -z1));	// 視点座標系での方向ベクトル
	//--  黄道座標系での方向ベクトル
	const vector3d n = n0.x*eR + n0.y*eU - n0.z*eF;
	const vector3d  cpos = ds.camera_base.get_pos();

	for (int i=0; i<num_tgt; i++) {
		int idx = vZ_tgt[i].idx;
		mtk_object&  obj = vObject[idx];
		if ( obj.hit_test(ds, mx, my, cpos, n) ) {
			//---  hit  ---

			//*******
			if (obj.get_type() == OBJ_TYPE_LABEL) {
				idx = get_label_parent_object_index( obj.get_index() );
			}
			if (get_object(idx).get_type() == OBJ_TYPE_FIXED_OBJECT) {
				if (get_fixed_object_type( get_object(idx).get_index() ) == OBJ_TYPE_OUR_GALAXY) {
					idx = get_object_ID("OurGalaxy");
				}
			}
			//*******

			selected_object_index = idx;
			return  true;
		}
	}


	return  (selected_object_index != sel_obj_idx_old);
}

