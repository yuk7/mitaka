/**************************************************************************************************

Copyright (c) 2007-2015   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#include  "mitaka.h"
#include  "mtk_3D_model.h"
#include  "mtk_minor_planets.h"
#include  "mtk_minor_planet_orientation.h"
#include  "../common/astro/astro_const.h"
#include  "../common/astro/astrometry.h"
#include  "mtk_draw_common.h"
#include  "mtk_object.h"
#include  "mtk_objects.h"
#include  "mtk_target.h"
#include  "mtk_orbital_element.h"
#include  "mtk_solar_system.h"
#include  "mtk_sun.h"
#include  "mtk_planets.h"
#include  "mtk_label.h"
#include  "mtk_fixed_object.h"
#include  "mtk_space_curve.h"
#include  "../common/graphics/gl_Model3D.h"
#include  "mtk_calc_alpha.h"
#include  "mtk_switches.h"
#include  "mtk_colors.h"
#include  "mtk_gl_common.h"
#include  "mtk_OpenGL.h"
#include  "mtk_dialog_box.h"
#include  "mtk_directory.h"
#include  "mtk_strings.h"
#include  "mtk_make_info.h"
#include  "mtk_data_file.h"
#include  "mtk_selection.h"
#include  "mtk_object_orientation.h"
#include  <vector>
#include  <string>
#include  <map>




static float  DwarfPlanetNameSize, SmallObjectNameSize;

// textures
static GLuint texCeres;
const char *fn_tex_Ceres = "Ceres.jpg";
// 3D models
static int  idxItokawaModel = -1;




//------------------
//  小惑星帯のラベル
//------------------
const vector3d  minor_planet_label_pos( 2.5*_1AU_pc, 0, 0 );



//--------------------------
//  小天体情報を保持するベクタ
//--------------------------
static std::vector<small_object>	vSmallObjects;



//--------------------------------------
//  小天体のキーとインデックスの参照テーブル
//--------------------------------------
static std::map<std::string, long>		small_object_key_table;





//----------------------------
//  Small objects Phys param
//----------------------------
struct  small_object_info
{
	int mp_no;				// minor planet number
	double Rx, Ry, Rz;		// size
	double B_V;				// B_V color
	double A;				// albedo
	int ring_idx;
};

static small_object_info vSmoi[] = {
	{      1,  487.3,  487.3,  454, 1.0,0.090, -1  },	//  Ceres [1]
	{      2,  260,  260,  260, 1.0, 0.1587, -1  },		//  Pallas
	{      3,  120,  120,  120, 1.0, 0.2383, -1  },		//  Juno
	{      4,  289,  280,  229, 1.0, 0.4228, -1  },		//  Vesta [1]
	{    243, 26.8, 12.0,  7.6, 1.0, 0.2383, -1  },			//  Ida [1]
	{    253,   33,   24,   23, 1.0, 0.0436, -1  },			//  Mathilde
	{    433,   17.0, 5.5,  5.5, 1.0, 0.25, -1  },		//  Eros [1]
	{    951,  9.1, 5.2, 4.4, 1.0, 1.0, -1  },			//  Gaspra [1]
	{   1566,  0.5,  0.5,  0.5, 1.0, 0.51, -1  },			//  Icarus
	{ 10199, 124.0, 124.0, 124.0, 0.84, 0.045, 0 },	//  Chariklo
	{  25143, 0.535, 0.294, 0.209, 1.0, 1.0, -1 },		//  Itokawa [1]
	{ 136199, 1200, 1200, 1200, 0.71, 1.0, -1 },			//  Eris
	{ 136108, 1000, 600, 500, 0.63, 1.0, -1 },			//  Haumea
	{  90377,  900,  900,  900, 1.23, 1.0, -1 },			//  Sedna
	{ 136472,  750,  750,  750, 0.9, 1.0, -1  },			//  Makemake
	{  90482,  500,  500,  500, 0.68, 1.0, -1 },			//  Orcus
	{  50000,  500,  500,  500, 0.94, 1.0, -1 },			//  Quaoar
	{  20000,  300,  300,  300, 0.85, 0.07, -1 }			//  Varuna
};
const int smo_phys_info_num = sizeof(vSmoi)/sizeof(vSmoi[0]);



//---------------
//  リングデータ
//---------------
const int RING_NUM = 1;


const ring_info ringi[RING_NUM] = {
		{(390.6-3*6.665/2.0)/124.0, (404.8+3*3.5/2.0)/124.0, 1.0, "chariklo_rings.png"}	// Chariklo [2]
};



//--------------
//  Orbit type
//--------------
const char szOrbitTypeKey[][100] = {
	"MP_ORBIT_TYPE_UNDEF",
	"MP_ORBIT_TYPE_ATIRA",
	"MP_ORBIT_TYPE_ATEN",
	"MP_ORBIT_TYPE_APOLLO",
	"MP_ORBIT_TYPE_AMOR",
	"MP_ORBIT_TYPE_OBJ_Q_LE_1_665",
	"MP_ORBIT_TYPE_HUNGARIA",
	"MP_ORBIT_TYPE_PHOCAEA",
	"MP_ORBIT_TYPE_HILDA",
	"MP_ORBIT_TYPE_JUPITER_TROJAN",
	"MP_ORBIT_TYPE_CENTAUR",
	"MP_ORBIT_TYPE_SDO",
	"MP_ORBIT_TYPE_TNO"
};



//------------
//  3D モデル
//------------
bool
load_minorplanet_3D_model(const char *fn)
{
	//-- Load models
	idxItokawaModel = load_3D_model_file(DIR_DATA, fn);

	if (idxItokawaModel != -1) {
		Model3D *mdl = get_3DModel(idxItokawaModel);
		if (mdl != 0) { mdl->fit_size(2.0); }
		return true;
	}

	return false;
}



//--------
//  Load
//--------
int
load_small_objects(FILE* fp)
{
	while (true) {
		key_data  kd;
		small_object  smo;

		kd = read_a_key_data(fp);
		if (kd.key == "") {
			break;
		}

		smo.key = kd.key;




		double	epoch;
		float	a, e, M0, n, node, i, omg;
		int no, obj_type, dp_flg;


		no = stoi(kd.data[0]);
		epoch = stod(kd.data[1]);
		M0 = stof(kd.data[2]);
		omg = stof(kd.data[3]);
		node = stof(kd.data[4]);
		i = stof(kd.data[5]);
		e = stof(kd.data[6]);
		n = stof(kd.data[7]);
		a = pow(0.985607669 / n, 2.0 / 3.0);
		obj_type = stoi(kd.data[8]);
		dp_flg = stoi(kd.data[9]);


		smo.type = obj_type;
		smo.no = no;
		smo.obj_idx = -1;
		smo.phys_param_idx = -1;
		smo.bDwarfPlanet = (dp_flg != 0);
		smo.oe.set_parameters(epoch, a, e, M0, n, node, i, omg);

		smo.ori_idx = -1;
		smo.ring_idx = -1;


		smo.bSelected = is_selected(smo.key);
		smo.bHasLabel = is_defined_key(smo.key);


		//--  オブジェクトを登録
		int idx = vSmallObjects.size();
		smo.obj_idx = register_object(mtk_object(OBJ_TYPE_SMALL_OBJECT, idx, 0.0f, true), smo.key);
		vSmallObjects.push_back(smo);


		//--  Register label
		float R = 1.4 * 1000 * _1km_pc;
		float thc = 10 * D2R;
		int color_no;
		float name_size;

		if (smo.bDwarfPlanet) {
			color_no = MTK_COLOR_NAME_DWARF_PLANET;
			name_size = DwarfPlanetNameSize;
		}
		else {
			color_no = MTK_COLOR_NAME_SMALL_OBJECT;
			name_size = SmallObjectNameSize;
		}


		add_label(smo.key, smo.obj_idx, color_no, 1.0f, name_size, R, thc, true);


		//--  小天体のキーとインデックスの参照表を作る  --
		small_object_key_table.insert(pair<std::string, long>(smo.key, idx));
	}


	//---  「小惑星帯」のラベルの登録
	int fixed_obj_idx = add_fixed_object(minor_planet_label_pos, OBJ_TYPE_SMALL_OBJECT);
	mtk_object obj = mtk_object(OBJ_TYPE_FIXED_OBJECT, fixed_obj_idx, 0.0f, false);		//************
	int obj_idx = register_object(obj, "MinorPlanetsLabel0");

	float thc = 10 * D2R;
	add_label("LABEL_MINOR_PLANETS", obj_idx, MTK_COLOR_NAME_OBJECT, 1.0f, get_object_name_size(), 0, thc, false);


	//---  自転情報
	init_minor_planet_orientation();


	//---  物理パラメータとリングの登録
	for (int i = 0; i<smo_phys_info_num; i++) {
		std::string key = "MP_" + to_string(vSmoi[i].mp_no);

		int idx = get_small_object_index(key);
		if (idx == -1)  continue;

		small_object&  smo = vSmallObjects[idx];
		smo.phys_param_idx = i;

		//-- Ring
		int ring_idx = vSmoi[i].ring_idx;
		if (ring_idx >= 0) {
			const ring_info& ri = ringi[ring_idx];

			smo.ring_idx = add_particle_ring(ri.Rr1, ri.Rr2, ri.ring_alp, ri.tex_fn);
		}
	}


	//--- 3Dモデル
	load_minorplanet_3D_model("Itokawa.3ds");


	return  vSmallObjects.size();
}











//**********  ラベルの読み込み後に呼ぶ   ******
void
register_small_objects_orbit_curves()
{
	const int N = vSmallObjects.size();

	for (int i=0; i<N; i++) {
		const small_object&  smo = vSmallObjects[i];

		if ( !is_defined_key(smo.key) )  continue;	// ラベルを持つものすべて

		int color_no = (smo.bDwarfPlanet)  ?  MTK_COLOR_ORBIT_DWARF_PLANET  :  MTK_COLOR_ORBIT_SMALL_OBJECT;
		space_curve  sc( mtk_object(OBJ_TYPE_SMALL_OBJECT, i, 0.0f, false), get_orbit_width(), color_no );
		register_space_curve( sc );
	}
}


void
make_small_objects_orbit_curve(space_curve& sc, int div_N)
{
	const small_object&  smo = vSmallObjects[sc.get_related_object().get_index()];

	sc.clear();
	for (int j=0; j<div_N; j++) {
		double phi = 2.0 * M_PI * double(j) / div_N;
		sc.add_point( double(_1AU_pc) * smo.oe.calc_orbit_pos(phi) );
	}
}




//-------------
//  テクスチャ
//-------------
void
load_minor_planet_textures()
{
	SetSplashText("LOADING_MINOR_PLANET_TEXTURE");

	texCeres = LoadTexture(fn_tex_Ceres, TexShrinkRate, false);
}





//---------
//  Alpha
//---------
float
get_small_object_label_alpha(const mtk_draw_struct& ds, int idx)
{
	const int type = get_small_object_type(idx);
	const float dscl = (type <= SMALLOBJ_TYPE_JUPITER_TROJAN) ? 3.0 * _1AU_pc : 60.0 * _1AU_pc;
	float  alp;

	if (norm( get_small_object_position(idx) - ds.tgt_pos ) > 10000*_1km_pc) {
		alp = calc_alpha_by_disapear_scale_inside( ds.scale, dscl );
	} else {
		alp = 1;
	}

	const vector3d  rpos = get_small_object_position( idx ) - ds.camera.get_pos();
	float  Rv = calc_visible_radius_small_object( idx );
	float  Dth = 2.0f * Rv / rpos.norm();

	return  alp * calc_alpha_by_apparent_diameter( Dth );
}





float
get_small_object_orbit_alpha(const mtk_draw_struct& ds, int idx)
{
	const int type = get_small_object_type(idx);
	const float dscl = (type <= SMALLOBJ_TYPE_JUPITER_TROJAN) ? 3.0 * _1AU_pc : 60.0 * _1AU_pc;
	float  alp;
	
	if (norm( get_small_object_position(idx) - ds.tgt_pos ) > 10000*_1km_pc) {
		alp = calc_alpha_by_disapear_scale_inside( ds.scale, dscl );
	} else {
		alp = calc_alpha_by_disapear_scale_inside( ds.scale, 1000000*_1km_pc );
	}

	const vector3d  rpos = get_small_object_position( idx ) - ds.camera.get_pos();
	float  Rv = calc_visible_radius_small_object( idx );
	float  Dth = 2.0f * Rv / rpos.norm();

	return  alp * calc_alpha_by_apparent_diameter( Dth );
}


//---------
//  index
//---------
int
get_small_object_index(const std::string& key)
{
	std::map<std::string, long>::iterator it = small_object_key_table.find(key);
	if (it == small_object_key_table.end())  return -1;	// no match
	return  it->second;
}


int
get_small_objects_num()
{
	return  vSmallObjects.size();
}


small_object&
get_small_object(int idx)
{
	return vSmallObjects[idx];
}




void
update_small_objects_position(const double& t)
{
	for (std::vector<small_object>::iterator it=vSmallObjects.begin(); it != vSmallObjects.end(); it++) {
		it->oe.update_pos_and_velocity(t);
	}
}


int
get_small_object_type(int idx)
{
	return  vSmallObjects[idx].type;
}

bool
is_small_object_selected(int idx)
{
	return vSmallObjects[idx].bSelected;
}


void
get_small_object_size(int idx, double& Rx, double& Ry, double& Rz)
{
	int  pi_idx = vSmallObjects[idx].phys_param_idx;
	if (pi_idx == -1)  {
		Rx = Ry = Rz = 0.0;
		return;
	}

	Rx = vSmoi[pi_idx].Rx;
	Ry = vSmoi[pi_idx].Ry;
	Rz = vSmoi[pi_idx].Rz;
}


//-------------
//  Text size
//-------------
float
get_dwarf_planet_name_size()
{
	return  DwarfPlanetNameSize;
}

float
get_small_object_name_size()
{
	return  SmallObjectNameSize;
}

void
set_dwarf_planet_name_size(float sz)
{
	DwarfPlanetNameSize = sz;
}

void
set_small_object_name_size(float sz)
{
	SmallObjectNameSize = sz;
}



//------------------
//  Landing radius
//------------------
double
get_landing_radius_small_object(int idx)
{
	double Rx, Ry, Rz, R;
	get_small_object_size(idx, Rx, Ry, Rz);

	if (Rx > 0.0) {
		R = Rx * _1km_pc;
	} else {
		R = 100 * _1km_pc;	//***
	}

	return  R;
}




//----------------
//  Prepare draw
//----------------
bool
prepare_draw_small_object(const mtk_draw_struct& ds, int idx, double* pRb, double* pRh)
{
	if (ds.cnorm > 5.0e-1)  return false;


	//---  Visible
	int ty = get_small_object_type(idx);

	bool bFlg;

	if (ty <= SMALLOBJ_TYPE_CENTAUR) {
		bFlg = test_disp_switch(DISPSW_MINOR_PLANET, DISPM_DISP);
	}
	else {
		bFlg = test_disp_switch(DISPSW_TNO, DISPM_DISP);
	}

	if (!bFlg)  return false;


	//---  Radius
	double Rx, Ry, Rz;
	get_small_object_size(idx, Rx, Ry, Rz);
	if (Rx > 0.0) {
		*pRh = Rx * _1km_pc;
		*pRb = 1.5 * (*pRh);
	} else {
		*pRb = *pRh = 0.01 * _1km_pc;	//***
	}


	return  true;
}


float
get_minor_planet_point_alpha(const mtk_draw_struct& ds, int idx)
{
	const orbital_elements& oe = vSmallObjects[idx].oe;

	bool bTgt = (get_target().get_type() == OBJ_TYPE_SMALL_OBJECT && get_target().get_index()==idx);

	// 小天体の位置
	const vector3d  pos = double(_1AU_pc) * oe.get_pos();

	const float R1 = 0.01;
	const float R2 = 0.04;
	const float R3 = 1;
	const float R4 = 4;
	float R = norm(pos) / ds.scale;
	if (!bTgt && R > R4)  return  0.0;
	if (R < R1)  return  0.0;


	float af = 1.0;
	float sz = 1.5f / (ds.rad_per_pixel / 0.001);
	if (sz < 1) {
		af *= sz;
		sz = 1;
	}

	if (!bTgt && R > R3)	{ af *= 1 - (R-R3)/(R4-R3); }
	if (R < R2)				{ af *= (R-R1)/(R2-R1); }
	
	
	if (af < 0.05)  return 0.0;
	return  af;
}



//------------
//  Position
//------------
vector3d
get_small_object_position(int idx)
{
	return double(_1AU_pc) * vSmallObjects[idx].oe.get_pos();
}


//---------
//  Scale
//---------
double
get_scale_small_object(int idx)
{
	double Rx, Ry, Rz, R;
	get_small_object_size(idx, Rx, Ry, Rz);

	if (Rx > 0.0) {
		R = 30 * Rx * _1km_pc;
	} else {
		R = 30 * 100 * _1km_pc;
	}

	return  R;
}



//---------
//  Label
//---------
bool
is_small_object_label_visible(int idx, bool bTgt)
{
	int type = get_small_object_type( idx );

	int dispsw_idx;
	if (type == SMALLOBJ_TYPE_TNO || type==SMALLOBJ_TYPE_SDO)	dispsw_idx = DISPSW_TNO;
	else														dispsw_idx = DISPSW_MINOR_PLANET;

	if ( !test_disp_switch( dispsw_idx, DISPM_DISP) || !test_disp_switch( dispsw_idx, DISPM_NAME ) )  return  false;

	if (!vSmallObjects[idx].bHasLabel)  return false;


	if (bTgt) return true;
	if (test_disp_switch(dispsw_idx, DISPM_SELECT) && !is_small_object_selected(idx))  return  false;

	return  true;
}







//--------
//  Draw
//--------
void
setup_drawing_small_object(int idx)
{
	int  pi_idx = vSmallObjects[idx].phys_param_idx;
	if (pi_idx == -1)  return;

	//--  texture and albedo
	float A = vSmoi[pi_idx].A;
	if (vSmallObjects[idx].no == 1 && texCeres != -1) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texCeres);
		A = 1.0;
	}
	else {
		glDisable(GL_TEXTURE_2D);
	}


	//--  material
	RGBf  col = correct_RGB( blackbody_color( B_V_to_Tc( vSmoi[pi_idx].B_V ) ) );
	float mat[4];
	mat[0] = A * col.r;
	mat[1] = A * col.g;
	mat[2] = A * col.b;
	mat[3] = 1;
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mat );
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat);

	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);


}

void
draw_a_small_object(const mtk_draw_struct& ds, int idx, const vector3d& rpos, const double& scl)
{
	const small_object& smo = get_small_object(idx);
	int  pi_idx = smo.phys_param_idx;
	if (pi_idx == -1)  return;

	vector3d  pos = get_small_object_position( idx );
	double Rx, Ry, Rz;
	Rx = vSmoi[pi_idx].Rx * scl * _1km_pc;
	Ry = vSmoi[pi_idx].Ry * scl * _1km_pc;
	Rz = vSmoi[pi_idx].Rz * scl * _1km_pc;

	glPushMatrix();
		glTranslatef(rpos.x, rpos.y, rpos.z);


		if (!bSideLight)	set_light_from_sun(pos);
		else				set_side_light(ds);

		const int mp_no = smo.no;
		
		// 自転
		set_minor_planet_orientation_GL(idx, 0);


		if (mp_no == 25143 && idxItokawaModel >= 0) {
			glModel3D *mdl = get_3DModel(idxItokawaModel);
			double R;
			
			R = Rx * mdl->get_scale();

			glScalef(R, R, R);
			setup_drawing_small_object(idx);
			mdl->draw();
		}
		else {
			glScalef(Rx, Ry, Rz);
			setup_drawing_small_object(idx);

			if (smo.ring_idx >= 0) {
				vector3d lv;
				if (bSideLight) {
					lv = get_side_light_vector(ds);
				}
				else {
					lv = normalize(get_sun_position() - pos);
				}


				particle_ring& pr = get_particle_ring(smo.ring_idx);

				double R, f;
				vector3d ex, ey, ez;
				R = vSmoi[pi_idx].Rx * _1km_pc;
				f = 1 - Rz / Rx;
				get_orientation(smo.ori_idx, true).get_bases_tr(ex, ey, ez);

				const float Amin = 0.2;
				float _A = (vSmoi[pi_idx].A + Amin) / (1.0 + Amin);
				RGBf  col = _A * correct_RGB(blackbody_color(B_V_to_Tc(vSmoi[pi_idx].B_V)));

				draw_ring_shaded_sphere(pos, R, f, ex, ey, ez, lv, pr, 0, col);

				pr.calc_shadow_factor(ds, pos, R, f, ex, ey, ez, lv);
				pr.draw(ds, pos, R, ex, ey, lv);
			}
			else {
				draw_celestial_object(1, 1, Rz/Rx, 180.0f);
			}
		}

	glPopMatrix();
}


void
draw_a_small_object_point(const mtk_draw_struct& ds, int idx, const vector3d& rpos, float alpha)
{
	double R2;
	R2 = norm2(rpos);
	if (R2 < 1e-100)  return;

	const orbital_elements& oe = vSmallObjects[idx].oe;

	// 小天体の位置
	vector3d  pos = double(_1AU_pc) * oe.get_pos();


	float sz = 1.5f / (ds.rad_per_pixel / 0.001);
	if (sz < 1) {
		sz = 1;
	}


	// 光度を計算
	const vector3f  lv_e = normalize(get_sun_position()-pos);		// 要素からの光線ベクトル
	const vector3f  nv = normalize(ds.camera.get_pos() - pos);
	float dp = dot_product(lv_e, nv);
	float th = acosf(dp);
	float frac = 1.0 - th / M_PI;

	glEnable(GL_BLEND);

	// 色の決定
	float lv = 0.5 * frac + 0.5;
	glColor4f(lv, lv*0.95, lv*0.95, alpha);


	if (true) {
		// QUAD
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);

		vector3f eR, eU;
		if (ds.bRotateBillboard) {
			const small_object& sm = get_small_object(idx);
			eR = sm.eR;
			eU = sm.eU;
		}
		else {
			eR = ds.eRf;
			eU = ds.eUf;
		}

		draw_point(ds, 0.0012, float(sqrt(R2)), rpos, eR, eU);

	} else {
		glPointSize(sz);
		glBegin(GL_POINTS);
			glVertex3dv( rpos.get_pointer() );
		glEnd();
	}
}





//------------
//  Hit-test
//------------
bool
hit_test_small_object(const mtk_draw_struct& ds, int idx, const vector3d& rpos, const vector3d& nv)
{
	vector3d  _rpos, _nv;
	_rpos = rpos;
	_nv = nv;


	// 扁平率
	double Rx, Ry, Rz;
	get_small_object_size(idx, Rx, Ry, Rz);
	if (Rx <= 0.0)  return  false;
	Rx *= _1km_pc;
	Ry *= _1km_pc;
	Rz *= _1km_pc;

	double fy, fz;
	fy = Ry / Rx;
	fz = Rz / Rx;
	_rpos.y /= fy;
	_rpos.z /= fz;
	_nv.y /= fy;
	_nv.z /= fz;
	_nv = normalize(_nv);

	// 交点
	double R, b, c, D, t;
	R = Rx;
	b = dot_product(_rpos, _nv);
	c = _rpos.norm2() - R*R;
	D = b*b - c;

	if (D <= 0.0)  return false;
	t = -b -sqrt(D);
	if (t < 0.0)  return  false;

	return  true;
}



//--------
//  Info
//--------
void
make_object_info_small_object(const mtk_draw_struct& ds, int idx, std::vector<std::string>& text)
{
	const small_object&  smo = vSmallObjects[idx];



	std::string type_key;
	if (smo.bDwarfPlanet) {
		if (smo.type == SMALLOBJ_TYPE_TNO || smo.type == SMALLOBJ_TYPE_SDO) { type_key = "INFO_TYPE_PLUTOID"; }
		else { type_key = "INFO_TYPE_DWARF_PLANET"; }
	}
	else {
		type_key = "INFO_TYPE_MINOR_PLANET";
	}



	//---  Name  ---
	if (is_defined_key(smo.key)) {
		text.push_back(get_string(smo.key));
	}
	else {
		text.push_back(get_string(type_key));
	}


	//---  Type  ---
	add_info_line(text, "INFO_TYPE", type_key.c_str());


	//---  Minor planet number  ---
	if (smo.no != -1) {
		add_info_line_int(text, "INFO_MP_NUMBER", smo.no);
	}


	//---  Orbit Type  ---
	if (smo.type != SMALLOBJ_TYPE_UNDEF) {
		add_info_line(text, "INFO_MP_ORBIT_TYPE", szOrbitTypeKey[smo.type]);
	}
}





//------------------
//  visible radius
//------------------
double
calc_visible_radius_small_object(int idx)
{
	return  double(_1AU_pc) * vSmallObjects[idx].oe.get_a();
}



////////////////////////////////////////
/// References
//
// [1] Archinal et al., 2011, Celestial Mechanics and Dynamical Astronomy, 109, 101-135
// [2] Braga-Ribas et al., 2014, Nature, 508, 72-75
