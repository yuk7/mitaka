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
#include  "mtk_spacecraft.h"
#include  "mtk_spacecraft_model.h"
#include  "mtk_directory.h"
#include  "mtk_gl_common.h"
#include  "mtk_font.h"
#include  "../common/astro/astro_const.h"
#include  "mtk_planet.h"
#include  "mtk_planet_position.h"
#include  "mtk_switches.h"
#include  "mtk_objects.h"
#include  "mtk_label.h"
#include  "mtk_colors.h"
#include  "mtk_solar_system.h"
#include  "mtk_calc_alpha.h"
#include  "mtk_draw_common.h"
#include  "mtk_strings.h"
#include  "mtk_make_info.h"
#include  "mtk_target.h"

#include  <string>
#include  <map>



//***
//--  Externs
void	set_light_from_sun(const vector3d& pos);





//****  表示上の半径  *****
const float Spacecraft_R = 1.0 * _1km_pc;




// 探査機データ

struct spacecraft_data {
	const char *key;
	const char *fn_traj;
	const char *fn_model;
	vector3f r0;
	const int origin;
};

spacecraft_data spccft[] = {
	{"SCF_PIONEER_10", "Pioneer10.ppi", "Pioneer.3ds", vector3f(0, 0, 0), ORIGIN_SS_BC},
	{"SCF_PIONEER_11", "Pioneer11.ppi", "Pioneer.3ds", vector3f(0, 0, 0), ORIGIN_SS_BC},
	{"SCF_VOYAGER_I", "Voyager_I.ppi", "Voyager.3ds", vector3f(0, 0, 0), ORIGIN_SS_BC},
	{"SCF_VOYAGER_II", "Voyager_II.ppi", "Voyager.3ds", vector3f(0, 0, 0), ORIGIN_SS_BC},
	{"SCF_CASSINI", "Cassini.ppi", "Cassini.obj", vector3f(0, 0, 0.5), ORIGIN_SS_BC},
	//	{"Cassini", "Cassini.ppi", "Cassini", "カッシーニ", "Cassini.3ds", vector3f(0, 0, 0.5)},
	{"SCF_NEW_HORIZONS", "NewHorizons.ppi", "NewHorizons.obj", vector3f(0, 0, 0), ORIGIN_SS_BC},
	{"SCF_GALILEO", "Galileo.ppi", "Galileo.obj", vector3f(0, 0, 0), ORIGIN_SS_BC},
	{"SCF_HAYABUSA_2", "Hayabusa2.ppi", "Hayabusa2.obj", vector3f(0, 0, 0), ORIGIN_SS_BC},
	{"SCF_AKATSUKI", "Akatsuki.ppi", "Akatsuki.obj", vector3f(0, 0, 0), ORIGIN_SUN}
};
const int Nscf = sizeof(spccft)/sizeof(spccft[0]);






//-----------
//  Objects
//-----------
static spacecraft  vSpacecraft[Nscf];
static vector3d vSpacecraftPosition[Nscf];



int
get_spacecraft_num()
{
	return Nscf;
}



void
init_spacecraft()
{
	directory  dir = get_directory(DIR_SPACECRAFT);

	//-- Load models
	for (int i = 0; i < Nscf; i++) {
		const spacecraft_data &sd = spccft[i];

		int idx;
		idx = load_3D_model_file(DIR_SPACECRAFT, sd.fn_model);

		vSpacecraft[i].set(sd.key, idx, sd.origin);
	}

	//-- init spacecraft trajectory
	const int Ntbl = 100;
	for (int i = 0; i < Nscf; i++) {
		vSpacecraft[i].load_trajectory(dir.get_path(spccft[i].fn_traj), Ntbl);
	}
}







//------------
//  Register
//------------
void
register_spacecraft()
{
	mtk_object  obj;

	for (int i=0; i<Nscf; i++) {
		const spacecraft_data &sd = spccft[i];
		
		//  register object
		obj.init(OBJ_TYPE_SPACECRAFT, i, 0.0f, true);
		int obj_idx = register_object(obj, sd.key);
	
		// register label
		float thc = 5 * D2R;
		add_label(sd.key, obj_idx, MTK_COLOR_NAME_SPACECRAFT, 1.0f, 0.9*get_object_name_size(), 1.5*Spacecraft_R, thc, true);
	}
}




//------------------
//  Landing radius
//------------------
double
get_landing_radius_spacecraft(int idx)
{
	return  Spacecraft_R;		//****
}



//----------------
//  Prepare draw
//----------------
bool
prepare_draw_spacecraft(const mtk_draw_struct& ds, int idx, double* pRb, double* pRh)
{
	//---  Visible
	if  (!bSpacecraft)  return false;
	if (ds.JD < vSpacecraft[idx].get_t1())  return false;

	//---  Radius
	*pRb = *pRh = Spacecraft_R;		//****

	return  true;
}



//---------
//  Scale
//---------
double
get_scale_spacecraft(int idx)
{
	return  30.0 * Spacecraft_R;	//****
}



//---------
//  Label
//---------
bool
is_spacecraft_label_visible(const mtk_draw_struct& ds, int idx)
{
	if (!bSpacecraft)  return false;

	const spacecraft& spccrf = vSpacecraft[idx];
	if (ds.JD < spccrf.get_t1())  return false;

	//****
	return  test_disp_switch(DISPSW_PLANET, DISPM_NAME);

}


float
get_spacecraft_label_alpha(const mtk_draw_struct& ds, int idx)
{
	const spacecraft& spccrf = vSpacecraft[idx];
	const vector3d pos  = get_spacecraft_position( idx );
	const vector3d rpos = pos - ds.camera.get_pos();

	float Rv;
	if (!spccrf.get_draw_trajectory_mode()) {
		// 軌道表示をしていない時は、近づいた時のみ名前を表示
		Rv = Spacecraft_R;
	} else {
		// 軌道表示中は、惑星と同じ条件
		Rv = norm(pos);
	}

	float  Dth = 2.0f * Rv / rpos.norm();
	return  calc_alpha_by_apparent_diameter( Dth );
}




//--------
//  Info
//--------
void
make_object_info_spacecraft(const mtk_draw_struct& ds, int idx, std::vector<std::string>& text)
{
	text.push_back(get_string(vSpacecraft[idx].get_key()));
	add_info_line(text, "INFO_TYPE", "INFO_TYPE_SPACECRAFT");
}




//-------
//  Key
//-------
const std::string&
get_spacecraft_key(int idx)
{
	return vSpacecraft[idx].get_key();
}


//--------
//  Name
//--------
const std::string&
get_spacecraft_name(int idx)
{
	return  get_string(vSpacecraft[idx].get_key());
}






//------------
//  Position
//------------
void
update_spacecraft_position(const double& t)
{
	for (int i=0; i<Nscf; i++) {
		vSpacecraftPosition[i] = vSpacecraft[i].get_pos(t);
	}
}

vector3d
get_spacecraft_position(int idx)
{
	return  vSpacecraftPosition[idx];
}


const mtk_trajectory&
get_spacecraft_trajectory(int idx)
{
	return  vSpacecraft[idx].get_trajectory();
}



//-------------
//  Draw Path
//-------------
bool
get_spacecraft_trajectory_mode(int idx)
{
	return  vSpacecraft[idx].get_draw_trajectory_mode();
}

void
set_spacecraft_trajectory_mode(int idx, bool bFlg)
{
	vSpacecraft[idx].set_draw_trajectory_mode(bFlg);
}





//---------------------
//  探査機のモデルの表示
//---------------------
void
draw_a_spacecraft(const mtk_draw_struct& ds, int idx, const vector3d& rpos, const double& scl)
{
	const spacecraft& spccrf = vSpacecraft[idx];
	const vector3d  pos = spccrf.get_pos(ds.JD);		// 探査機の位置

	// 光源の設定
	if (!bSideLight)	{ set_light_from_sun(pos); }
	else				{ set_side_light(ds); }

	// 探査機を地球の方向に向ける行列を作る
	const vector3d  posE = get_planet_position(PLNT_EARTH);	// 地球の位置
	vector3d nz = -normalize( posE - pos );
	vector3d nu = vector3d(0,0,1);
	vector3d nx = normalize( cross_product(nu, nz) );
	vector3d ny = normalize( cross_product(nz, nx) );

	double MR[16];
	MR[0] = nx.x;
	MR[1] = nx.y;
	MR[2] = nx.z;
	MR[3] = 0.0;

	MR[4] = ny.x;
	MR[5] = ny.y;
	MR[6] = ny.z;
	MR[7] = 0.0;

	MR[8]  = nz.x;
	MR[9]  = nz.y;
	MR[10] = nz.z;
	MR[11] = 0.0;

	MR[12] = 0.0;
	MR[13] = 0.0;
	MR[14] = 0.0;
	MR[15] = 1.0;

	vector3d rr = rpos;

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();
		glTranslated(rr.x,rr.y,rr.z);
		glMultMatrixd(MR);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		glEnable(GL_NORMALIZE);
		glEnable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		const float R = scl * Spacecraft_R;
		glScalef(R, R, R);


		spccrf.draw_model();

	glPopMatrix();
	glPopAttrib();
}


void
draw_spacecraft_trajectory(const mtk_draw_struct& ds)
{
	const vector3d camera_pos = ds.camera.get_pos();
	for (int i=0; i<Nscf; i++) {
		const spacecraft& spccrf = vSpacecraft[i];

		if (!spccrf.get_draw_trajectory_mode()) {
			continue;
		}

		const vector3d pos  = spccrf.get_pos(ds.JD);
		const vector3d rpos = pos - ds.camera.get_pos();
		float  Rv = norm(pos);
		float  Dth = 2.0f * Rv / rpos.norm();
		float  af = calc_alpha_by_apparent_diameter( Dth );

		if (af <= 0.01f)  continue;
		RGBf  col;
		if (ds.JD <= spccrf.get_t2())	{ col.set(1,1,0); }
		else							{ col.set(0.7,0.5,0); }
		spccrf.draw_trajectory(ds, ds.JD, camera_pos, col, af);
	}
}

