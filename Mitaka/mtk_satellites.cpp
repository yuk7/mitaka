/**************************************************************************************************

Copyright (c) 2014-2015   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan
Copyright (c) 2007-2012   Tsunehiko Kato

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
#include  "mtk_satellites.h"
#include  "mtk_satellite.h"
#include  "mtk_satellite_orientation.h"
#include  "mtk_mode.h"
#include  "mtk_objects.h"
#include  "../common/astro/astro_const.h"
#include  "mtk_gl_common.h"
#include  "mtk_OpenGL.h"
#include  "mtk_draw_common.h"
#include  "mtk_solar_system.h"
#include  "mtk_sun.h"
#include  "mtk_planets.h"
#include  "mtk_planet_position.h"
#include  "mtk_planet_orientation.h"
#include  "mtk_planet_eclipse.h"
#include  "mtk_places.h"
#include  "mtk_draw_places.h"
#include  "mtk_planetarium.h"
#include  "mtk_target.h"
#include  "mtk_label.h"
#include  "mtk_switches.h"
#include  "mtk_colors.h"
#include  "mtk_calc_alpha.h"
#include  "mtk_dialog_box.h"
#include  "mtk_font.h"
#include  "mtk_strings.h"
#include  "mtk_make_info.h"
#include  "mtk_topography.h"
#include  "mtk_selection.h"
#include  "mtk_object_orientation.h"
#include  "mtk_lunar_pos.h"
#include  "mtk_atmosphere.h"
#include  "mtk_eclipse.h"
#include  "mtk_topography.h"
#include  <vector>
#include  <map>






//-----------
//  Externs
//-----------
extern bool bDrawLowRes;  //****
vector2d	get_landing_pos();
extern float  landing_lon, landing_lat;



//-----------
//  Objects
//-----------
static std::vector<satellite> vSatellites;
static std::map<std::string, int> mSatellites;





//-------------
//  Text size
//-------------
static float  SatelliteNameSize;





//---  Number of satellites
int
get_satellites_num()
{
	return vSatellites.size();
}

int
get_satellite_index(const std::string& key)
{
	std::map<std::string, int>::const_iterator it = mSatellites.find(key);

	if (it == mSatellites.end()) {
		return -1;
	}

	return it->second;
}


satellite&
get_satellite(int idx)
{
	return  vSatellites[idx];
}


//---  Key of satellite
const std::string&
get_satellite_key(int idx)
{
	return  vSatellites[idx].get_key();
}




//---  Planet ID
int
get_satellite_planet_index(int idx)
{
	return  vSatellites[idx].get_planet_index();
}





//---  Size
void
get_satellite_size(int mode, int idx, double& Rx, double& Ry, double& Rz)
{
	vSatellites[idx].get_real_size(Rx, Ry, Rz);

	//*****
	if (idx == get_satellite_index("SAT_E_MOON")) {
		double  fac;

		if (mode == MODE_3D)		fac = get_zoom_rate().zmhPlanet;
		else						fac = get_zoom_rate().zmpSunMoon;
		Rx *= fac;
		Ry *= fac;
		Rz *= fac;
	}
}




//-------------------------
//  Places and topography
//-------------------------
void
init_satellite_places_and_topography()
{
	for (int i = 0; i < vSatellites.size(); i++) {
		satellite& st = vSatellites[i];

		const std::string& key = st.get_key();
		st.set_places(get_places(key));
		st.set_topography(get_topography(key));
	}

	//---  Eclipse
	// Moon
	{
		satellite & st = get_satellite( get_satellite_index("SAT_E_MOON") );
		vector3f col = get_moon_eclipse_shadow_color();
		st.add_eclipse(col);
	}
}







//------------
//  Position
//------------
vector3d
calc_lunar_real_position(const double& t)
{
	return  get_planet_position(PLNT_EARTH) + double(_1km_pc) * calc_lunar_pos(t);
}


void
update_satellites_position(const double& t)
{
	for (int i = 0; i<vSatellites.size(); i++) {
		vSatellites[i].update_position(t);
	}
}



vector3d
get_satellite_position(int idx)
{
	return vSatellites[idx].get_position();
}

void
set_satellite_position(int idx, const vector3d& pos)
{
	vSatellites[idx].set_position(pos);
}



vector3d
get_planetocentric_satellite_position(int idx)
{
	return  vSatellites[idx].get_planetocentric_position();
}


vector3d
calc_satellite_orbit_pos(int idx, const double& t, float phi)
{
	return  vSatellites[idx].calc_orbit_pos(t, phi);
}



//-------------
//  Zoom rate
//-------------
void
set_satellites_zoom_rate(int mode)
{
	for (int i = 0; i < vSatellites.size(); i++) {
		double zoom_rate, orbit_amp_factor;
		satellite &sd = vSatellites[i];


		if (sd.get_orbit_type() == SAT_ORBIT_MOON) {
			if (mode == MODE_3D)		zoom_rate = get_zoom_rate().zmhPlanet;
			else						zoom_rate = get_zoom_rate().zmpSunMoon;
		}
		else {
			zoom_rate = 1.0;
		}


		if (sd.get_orbit_type() == SAT_ORBIT_MOON) {
			
			if (mode == MODE_3D)	orbit_amp_factor = get_zoom_rate().zmhMoonOrb;
			else					orbit_amp_factor = 1;
		}
		else {
			orbit_amp_factor = 1.0;
		}

		//sd.set_zoom_rate(zoom_rate);
		sd.set_orbit_amp_factor(orbit_amp_factor);
	}
}





//--------
//  Load
//--------
bool
load_satellites(const directory& dir)
{
	FILE *fp;


	fp = dir.fopen("satellites.dat", "rt");
	if (fp == NULL)  return false;


	vSatellites.clear();
	while (true) {
		bool res;
		satellite sd;

		//---  Read one data
		res = sd.load_from_file(fp);
		if (!res) break;				// ファイル終端
		vSatellites.push_back(sd);

		int idx = vSatellites.size() -1;
		mSatellites[sd.get_key()] = idx;


		//--  register satellites
		int obj_idx = register_object(mtk_object(OBJ_TYPE_SATELLITE, idx, 0.0f, true), sd.get_key());


		//--  register labels
		float R = 1.4 * _1km_pc * sd.get_Rx();
		float thc = 8 * D2R;

		add_label(sd.get_key(), obj_idx, MTK_COLOR_NAME_SATELLITE, 1.0f, get_satellites_name_size(), R, thc, true);
	}


	init_satellite_orientation();


	return true;
}



//-------------
//  Selection
//-------------
bool
is_satellite_selected(int idx)
{
	return  is_selected(vSatellites[idx].get_key());
}



//------------------
//  Landing radius
//------------------
double
get_landing_radius_satellite(int idx)
{
	double Rx, Ry, Rz;
	get_satellite_size( MODE_3D, idx, Rx, Ry, Rz);
	return  Rx * _1km_pc;
}




//----------------
//  Prepare draw
//----------------
bool
prepare_draw_satellite(const mtk_draw_struct& ds, int idx, double* pRb, double* pRh)
{
	//---  Visible
	if  ( !test_disp_switch(DISPSW_SATELLITE,	DISPM_DISP) )  return false;


	if (ds.cnorm > 4.0e-3)  return false;


	//---  Radius
	double Rx, Ry, Rz;
	get_satellite_size(ds.tyMode, idx, Rx, Ry, Rz);
	*pRb = *pRh = Rx * _1km_pc;

	return  true;
}



//---------
//  Scale
//---------
double
get_scale_satellite(int idx)
{
	double Rx, Ry, Rz;
	get_satellite_size(MODE_3D, idx, Rx, Ry, Rz);
	return  30.0 * Rx * _1km_pc;
}



//-------------------
//   Visible radius
//-------------------
double
calc_visible_radius_satellite(int idx)
{
	return  get_planetocentric_satellite_position(idx).norm();
}



//---------
//  Label
//---------
bool
is_satellite_label_visible(int idx, bool bTgt)
{
	if ( !test_disp_switch(DISPSW_SATELLITE, DISPM_DISP) || !test_disp_switch(DISPSW_SATELLITE, DISPM_NAME ) )  return  false;
	if (bTgt) return true;
	if ( test_disp_switch(DISPSW_SATELLITE, DISPM_SELECT) && !is_satellite_selected(idx))  return  false;

	return  true;
}


float
get_satellite_label_alpha(const mtk_draw_struct& ds, int idx)
{
	float scl = 1.5f;
	const vector3d  rpos = get_satellite_position( idx ) - ds.camera.get_pos();
	float  Rv = scl * calc_visible_radius_satellite( idx );
	float  Dth = 2.0f * Rv / rpos.norm();

	return  calc_alpha_by_apparent_diameter( Dth );
}




//-----------------
//  Load Textures
//-----------------
void
load_satellites_textures()
{
	SetSplashText("LOADING_SATELLITES_TEXTURE");
	for (int i = 0; i<vSatellites.size(); i++) {
		vSatellites[i].load_texture(TexShrinkRate);
	}
}





//-------------
//  Text size
//-------------
void
set_satellites_name_size(float sz)
{
	SatelliteNameSize = sz;
}

float
get_satellites_name_size()
{
	return  SatelliteNameSize;
}



//----------
//  Orbits
//----------
void
register_satellites_orbit_curves()
{
	const int N = get_satellites_num();

	const float ow = get_orbit_width();
	for (int i=0; i<N; i++) {
		space_curve  sc( mtk_object(OBJ_TYPE_SATELLITE, i, 0.0f, false), ow, MTK_COLOR_ORBIT_SATELLITE );
		register_space_curve( sc );
	}
}


void
set_satellites_orbit_curve(space_curve& sc, double t)
{
	const int idx = sc.get_related_object().get_index();
	const int N = sc.get_num_points();

	for (int i=0; i<N; i++) {
		double phi = 2.0 * M_PI * double(i) / N;
		vector3d pos = calc_satellite_orbit_pos(idx, t, phi);
		sc.set_point(i, pos);
	}
}

void
make_satellites_orbit_curve(space_curve& sc, double t, int div_N)
{
	sc.set_num_points( div_N );
	set_satellites_orbit_curve(sc, t);
}


float
get_satellite_orbit_alpha(const mtk_draw_struct& ds, int idx)
{
	int pid = get_satellite_planet_index(idx);
	float Re = get_planet_Re(ds.tyMode, pid) * _1km_pc;
	const float dscl = 8 * Re;
	float  alp = calc_alpha_by_disapear_scale_inside( ds.scale, dscl );

	return  alp * get_satellite_label_alpha( ds, idx );
}









//--------
//  Draw
//--------

void
draw_satellite(const mtk_draw_struct& ds, int idx, const vector3d& rpos, double scl)
{
	satellite &st = get_satellite(idx);
	const int idx_Moon = get_satellite_index("SAT_E_MOON");


	vector3d r_sat = st.get_position();		// 衛星の位置（日心黄道座標, in pc）

	double Rx, Ry, Rz;
	get_satellite_size(ds.tyMode, idx, Rx, Ry, Rz);
	Rx *= scl * _1km_pc;
	Ry *= scl * _1km_pc;
	Rz *= scl * _1km_pc;


	const frustum frs = ds.get_frustum();
	bool bWire = false;




	glPushMatrix();
	glTranslatef(rpos.x, rpos.y, rpos.z);

	if (!bSideLight)	set_light_from_sun(r_sat);
	else				set_side_light(ds);

	set_satellite_orientation_GL(idx, 0.0f);		// 自転軸および回転角をセットする


	if (test_disp_switch(DISPSW_PLANET, DISPM_AXIS)) draw_planet_axis(Rz);

	
	GLuint tex_no = st.get_texture_index();
	if (tex_no != 0) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tex_no);
	}
	else {
		glDisable(GL_TEXTURE_2D);
	}


	TopoSphere *tp = st.get_topography();
	Places *plc = st.get_places();

	if (tp!=0 || plc!=0) {
		const float screen_w = get_screen_w();
		const float screen_z = get_screen_z();
		const mtk_object&  tgt = get_target();
		bool bTarget = (tgt.get_type() == OBJ_TYPE_SATELLITE && tgt.get_index() == idx);

		glPushMatrix();

		glScalef(Rx, Ry, Rz);
		glLineWidth(1.0f);
		glColor3f(0, 1, 0);


		// Topography
		if (tp != 0) {
			tp->draw(frs, bWire, bDrawLowRes);
		}
		else {
			draw_celestial_object(1, 1, 1, 180.0f);
		}

		// Surface pointer
		if (bSurfacePointer && bTarget && ds.tySubMode != SUBMODE_3DCHART) {
			glDisable(GL_LIGHTING);
			glDisable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			vector2d lon_lat = get_landing_pos();
			draw_pointer_on_TopoSphere(tp, screen_w, screen_z, lon_lat.x, lon_lat.y, ds.ltf);
		}


		// Place names
		if (test_disp_switch(DISPSW_PLANET, DISPM_NAME) && ds.tySubMode != SUBMODE_3DCHART) {
			if (plc != 0) {
				bool bHorizontalMode = (get_landing_mode() == LANDING_MODE_HORIZONTAL);
				draw_places_on_TopoSphere(*plc, tp, screen_w, screen_z, ds.ltf, bHorizontalMode);
			}
		}
		

		glPopMatrix();
	}
	else {
		draw_celestial_object(Rx, Ry, Rz, 180.0f);
	}


	//  Eclipse
	if (swEclipse & (ECLIPSE_DISP | ECLIPSE_BORDER)) {
		if (idx == idx_Moon) {
			glPushMatrix();
			glScalef(Rx, Ry, Rz);
			draw_satellite_eclipse(ds, idx, 1.0);
			glPopMatrix();
		}
	}
	glPopMatrix();
}




void
draw_a_satellite(const mtk_draw_struct& ds, int idx, const vector3d& rpos, const double& scl)
{
	glEnable(GL_LIGHT0);
	float mat_d[] = {1,1,1,1}, mat_a[] = {0.5,0.5,0.5,1};
	mat_a[0] = mat_a[1] = mat_a[2] = get_lighting_ambient();
	glMaterialfv( GL_FRONT_AND_BACK , GL_DIFFUSE,   mat_d );
	glMaterialfv( GL_FRONT_AND_BACK , GL_AMBIENT,   mat_a );

	draw_satellite(ds, idx, rpos, scl);

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
}





//------------
//  Hit-test
//------------
bool
hit_test_satellite(const mtk_draw_struct& ds, int idx, const vector3d& rpos, const vector3d& nv)
{
	matrix3d  M_E2SAT;
	vector3d  _rpos, _nv;
	M_E2SAT = get_satellite_orientation(idx, true);
	_rpos = M_E2SAT * rpos;
	_nv = M_E2SAT * nv;

	// 扁平率
	double Rx, Ry, Rz;
	get_satellite_size(MODE_3D, idx, Rx, Ry, Rz);
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
make_object_info_satellite(const mtk_draw_struct& ds, int idx, std::vector<std::string>& text)
{
	const char szPlanetNameE[][20] = {"Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune", "Pluto"};
	const satellite& st = get_satellite(idx);

	//---  Name  ---
	text.push_back( get_string(st.get_key()) );

	//---  Roman number  ---
	const std::string  rmn = st.get_roman_number();
	if (rmn != "") {
		std::string str = std::string(szPlanetNameE[st.get_planet_index()]) + " " + rmn;
		text.push_back(str);
	}

	//---  Type  ---
	add_info_line(text, "INFO_TYPE", "INFO_TYPE_SATELLITE");

}







//---------------
//  Planetarium
//---------------
void
draw_a_satellite_sub(const float& R, int idx)
{
	const satellite& st = get_satellite(idx);

	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glEnable(GL_NORMALIZE);

	glColor3f(1,1,1);

	char tex_no = st.get_texture_index();
	if (tex_no != -1) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tex_no);
	} else {
		glDisable(GL_TEXTURE_2D);
	}

	glScalef(R, R, R);
	gl_draw_LOD_sphere();

	glDisable(GL_NORMALIZE);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
}



void
draw_a_satellite_planetarium(const mtk_draw_struct& ds, int idx, const double& Rc)
{
	const satellite& st = get_satellite(idx);

	if (ds.tgt_ID == get_object_ID( st.get_key() ))  return;


	vector3d  spos = st.get_position();


	//---  投影位置とサイズ  ---
	vector3d  rpos = spos - ds.camera.get_pos();
	double  L = rpos.norm();
	// 位置
	const double R0 = 1.002;

	vector3d pos = R0 * rpos/L;
	// 視直径
	double Rx, Ry, Rz, Rs;
	get_satellite_size(ds.tyMode, idx, Rx, Ry, Rz);
	Rs = Rx / L * _1km_pc;




	// 視野角
	double th = Rs/pos.norm();

	th *= get_planetarium_zooming_factor();

	const double thc1 = 3*ds.rad_per_pixel;
	const double thc2 = 0.1*thc1;
	const double thc3 = 0.01*thc1;
	if (th < thc3)  return;

	bool bPoint;
	float af;
	if (th < thc1) {
		bPoint = true;
		af = th/thc2;
		if (af > 1) af = 1;
	} else {
		bPoint = false;
		af = 1.0f;
	}




	const int idx_Moon = get_satellite_index("SAT_E_MOON");



	//---  Draw  ---
	glEnable(GL_DEPTH_TEST);	//****

	glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);
	
		if (!bPoint) {
			//---  実サイズで描画  ---
			glPushMatrix();
				set_light_from_sun( spos );
				set_satellite_orientation_GL(idx, 180);
				draw_a_satellite_sub(Rs, idx);
				if ( swEclipse & (ECLIPSE_DISP | ECLIPSE_BORDER) ) {
					draw_satellite_eclipse(ds, idx, Rc);
				}
			glPopMatrix();
		} else {
			//---  点で描画  ---
			glDisable(GL_LIGHTING);
			glDisable(GL_BLEND);
			glDisable(GL_TEXTURE_2D);
			glPointSize(1.5);
			glBegin(GL_POINTS);
				glColor3f(af,af,af);
				glVertex3f(0,0,0);
			glEnd();
		}

		// 名前
		if ( test_disp_switch(DISPSW_SATELLITE, DISPM_NAME) ) {
			float sz;
			int col_idx;

			if (idx == idx_Moon) {
				sz = get_special_object_name_size();
				col_idx = MTK_COLOR_NAME_SPECIAL_OBJECT;
			} else {
				sz = SatelliteNameSize;
				col_idx = MTK_COLOR_NAME_SPECIAL_OBJECT;
			}

			prepare_3Dtext_1(Rs*1.41*ds.plnt_R, 45, sz*ds.ltf, 1.0f, false);
			
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_LIGHTING);
			glEnable(GL_BLEND);

			const RGBf& col = af * get_color(col_idx);
			glColor3f(col.r, col.g, col.b);

			draw_string(st.get_key());
		}
	glPopMatrix();
}


void
draw_satellites_planetarium(const mtk_draw_struct& ds, const double& Rc)
{
	float mat_d[] = {1,1,1,1};
	float mat_a[] = {0.5,0.5,0.5,1};
	mat_a[0] = mat_a[1] = mat_a[2] = get_lighting_ambient();
	glMaterialfv( GL_FRONT_AND_BACK , GL_DIFFUSE,   mat_d );
	glMaterialfv( GL_FRONT_AND_BACK , GL_AMBIENT,   mat_a );

	glEnable(GL_LIGHT0);

	for (int i=0; i<get_satellites_num(); i++) {
		if (i == 0) {
			continue;	// skip the moon
		}
		draw_a_satellite_planetarium(ds, i, Rc);
	}
}



void
draw_moon_planetarium(const mtk_draw_struct& ds, const double& Rc, char atm_mode, float I0)
{
	const int Nr = 40;
	const int Nth = 80;
	const mtk_object&  tgt = get_object(ds.tgt_ID);


	vector3d  rr[Nr + 1][Nth];
	double  Br[Nr + 1][Nth];
	double  tu[Nr + 1][Nth], tv[Nr + 1][Nth];
	int i, j;


	const vector3d r_sun = get_sun_position();


	int idx = 0;	//***

	const satellite& st = get_satellite(idx);
	vector3d  spos = get_satellite_position(idx);
	vector3d  rpos = spos - ds.camera.get_pos();
	double  L = rpos.norm();
	vector3d  pos0 = rpos / L;

	vector3d  rsun_rel = r_sun - ds.camera.get_pos();
	vector3d  nsun = rsun_rel.normalized();

	double Rx, Ry, Rz;
	get_satellite_size(ds.tyMode, idx, Rx, Ry, Rz);
	double sin_max = Rx * _1km_pc / L;

	vector3d ex, ez;
	ex = get_a_normal_unit_vector(pos0);
	ez = cross_product(pos0, ex);


	vector3d emx, emy, emz;
	get_satellite_orientation(idx, true).get_bases_tr(emx, emy, emz);




	// sky color
	const int plnt_idx = 2;  //***
	planet& plnt = get_planet(plnt_idx);

	RGBf col(0, 0, 0);


	const atmosphere& atm = get_atmosphere(PLNT_EARTH);



	// parameter for atmosphere
	if (has_atmosphere(PLNT_EARTH) && tgt.get_type() == OBJ_TYPE_PLANET && tgt.get_index() == 2) {
		const float zrt = get_zoom_rate().zmpPlanet;
		const float H = atm.H * zrt;
		const float R1 = get_planet_Re(ds.tyMode, plnt_idx) * 1000;
		const float R2 = R1 + 20 * H;
		const float n0 = atm.n0 / zrt;	// Earth


		//--  地平座標系での太陽の位置を計算  --
		const matrix3d Mpl = get_planet_orientation(plnt_idx, true);
		vector3d rs, n_sun;
		rs = r_sun - get_planet_position(plnt_idx);					// 惑星中心黄道座標での太陽の位置
		rs = Mpl * rs;			// ec -> pl 
		rs = Ry3deg(-90 + landing_lat) * Rz3deg(-landing_lon) * rs;	// pl -> 地平座標
		n_sun = rs.normalized();

		vector3d ev;
		ev = Mpl * pos0;						// ec -> obj
		ev = Ry3deg(-90 + landing_lat) * Rz3deg(-landing_lon) * ev;	// obj -> 地平座標



		double elv = 0.0;
		if (plnt.has_topography()) {
			TopoSphere*  tp = plnt.get_topography();
			if (tp->is_loaded()) {
				elv = tp->get_elevation(landing_lon, landing_lat);
			}
		}
		const vector3f r0 = vector3f(0, 0, R1 + elv);



		eclipse_info  eci;
		bool bEclipse = false;
		if (swEclipse & ECLIPSE_DISP) {
			vector3d r1, r2;
			double R1, R2, Rs;
			r1 = get_planet_position(plnt_idx);
			R1 = get_planet_Re(ds.tyMode, plnt_idx) * _1km_pc;
			r2 = get_satellite_position(0);
			R2 = Rx * _1km_pc;
			Rs = R_sun_km * _1km_pc;

			eci = calc_eclipse_info(r1, R1, r2, R2, r_sun, Rs);
			if (eci.bFlg)  bEclipse = true;
		}
		const double fe = 0.96;


		float I = I0;
		if (bEclipse) {
			vector3d r1, r2;
			double R1, R2, Rs;
			r1 = get_planet_position(plnt_idx);
			R1 = get_planet_Re(ds.tyMode, plnt_idx) * _1km_pc;
			r2 = get_satellite_position(0);
			R2 = Rx * _1km_pc;
			Rs = R_sun_km * _1km_pc;

			vector3d pos0 = R1 * get_planet_nv_ec(plnt_idx, landing_lon, landing_lat) + r1;


			vector3d ev2;
			ev2 = Rz3deg(landing_lon) * Ry3deg(90 - landing_lat) * ev;	// Hori -> obj
			ev2 = Mpl.transposed() * ev2;
			vector3d pos = pos0 + 100.0*_1km_pc*ev2;



			// 視半径
			double thm, ths, R;
			thm = asin(R2 / norm(pos - r2));
			ths = asin(Rs / norm(pos - r_sun));
			R = thm / ths;

			// 中心間の距離
			double Dlt;
			vector3d nm = normalize(r2 - pos);
			vector3d ns = normalize(r_sun - pos);
			Dlt = acos(dot_product(nm, ns)) / ths;

			// 掩蔽率を計算する
			double f = calc_eclipse_fraction(R, Dlt);
			if (f <= 0.0)  f = 0;
			I = (1 - f)*I;
		}


		if (atm_mode != ATMOSPHERE_NONE) {
			col = calc_scattered_light_intensity(R1, R2, n0, H, I, r0, ev, n_sun, 22);
			if (atm_mode == ATMOSPHERE_MULTIPLE) {
				//--  多重散乱は、実スケールの地球について計算しているので r0/zrt とする
				float I2 = fe*I + (1 - fe)*I0;
				col += I2 * calc_multiple_scattered_light_intensity(r0 / zrt, ev, n_sun);
			}
			col = correct_RGB(col);
		}
	}



	//---  calc pos, brightness, and texture coordinates
	for (i = 0; i <= Nr; i++) {
		double r;
		r = i * sin_max / Nr;

		for (j = 0; j<Nth; j++) {
			double  th, b, c, D, t, f;
			vector3d  ev, nv;

			th = 2 * 3.14159265 * j / (Nth - 1);
			ev = normalize(cos(th)*ex + sin(th)*ez);
			nv = normalize(pos0 + r* ev);
			rr[i][j] = Rc * nv;

			b = dot_product(nv, pos0);
			c = 1.0 - sin_max*sin_max;
			D = b*b - c;
			if (D < 0.0) D = 0.0;
			t = b - sqrt(D);

			vector3d rm = t * nv;
			vector3d nm = normalize(rm - pos0);

			f = dot_product(nm, nsun);
			if (f < 0.0)  f = 0.0;
			Br[i][j] = f;

			double x, y, z, rho, u, v;
			x = dot_product(nm, emx);
			y = dot_product(nm, emy);
			z = dot_product(nm, emz);
			rho = sqrt(x*x + y*y);
			v = atan2(z, rho) / 3.14159265 + 0.5;
			u = atan2(y, x) / (2 * 3.14159265) + 0.5;

			tu[i][j] = u;
			tv[i][j] = v;
		}
	}



	//---  Attenuation  ---
	RGBf col0(1, 1, 1);
	if (tgt.get_type() == OBJ_TYPE_PLANET && tgt.get_index() == 2) {
		vector3d r0 = ds.camera.get_pos() - get_planet_position(2);
		float zrt = 1;
		const double R = atm.R * zrt;
		const float  H = atm.H * zrt;		// スケールハイト(H=8km)
		const float n0 = atm.n0 / zrt;		// number density


		vector3d  lv = normalize(rpos);


		//-- 入射光の減光率 --
		col0 = calc_attenuated_light_intensity(R, R + 10 * H, n0, H, I0*0.1, r0*_1pc_km * 1000, lv, 1000);
	}



	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);

	char tex_no = st.get_texture_index();
	if (tex_no != -1) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tex_no);
	}
	else {
		glDisable(GL_TEXTURE_2D);
	}


	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);


	glBegin(GL_TRIANGLE_STRIP);
	for (i = 0; i<Nr; i++) {
		for (j = 0; j<Nth; j++) {
			double f, u, v;
			vector3d r;
			RGBf col;

			f = Br[i][j];
			u = tu[i][j];
			v = tv[i][j];
			r = rr[i][j];

			col = correct_RGB(float(f) * col0);
			glColor3fv(col.get_pointer());
			glTexCoord2f(u, v);
			glVertex3f(r.x, r.y, r.z);

			f = Br[i + 1][j];
			u = tu[i + 1][j];
			v = tv[i + 1][j];
			r = rr[i + 1][j];

			col = correct_RGB(float(f) * col0);
			glColor3fv(col.get_pointer());
			glTexCoord2f(u, v);
			glVertex3f(r.x, r.y, r.z);
		}
	}
	glEnd();


	//---  atmosphere
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(col.r, col.g, col.b);
	for (i = 0; i<Nr; i++) {
		for (j = 0; j<Nth; j++) {
			vector3d r;

			r = rr[i][j];
			glVertex3f(r.x, r.y, r.z);

			r = rr[i + 1][j];
			glVertex3f(r.x, r.y, r.z);
		}
	}
	glEnd();


	//  Eclipse
	if (swEclipse & (ECLIPSE_DISP | ECLIPSE_BORDER)) {
		if (idx == 0) {
			glPushMatrix();
			draw_satellite_eclipse(ds, idx, Rc);
			glPopMatrix();
		}
	}


	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
}


void
draw_moon_label_planetarium(const mtk_draw_struct& ds, const double& Rc)
{
	if (!test_disp_switch(DISPSW_SATELLITE, DISPM_NAME))  return;


	int idx = 0;

	vector3d  rpos = get_satellite_position(idx) - ds.camera.get_pos();
	double L = rpos.norm();
	vector3d  pos = Rc * rpos / L;


	double Rx, Ry, Rz;
	get_satellite_size(ds.tyMode, idx, Rx, Ry, Rz);
	double sin_max = Rx * _1km_pc / L;



	glPushMatrix();
	glTranslated(pos.x, pos.y, pos.z);

	prepare_3Dtext_1(1.7*sin_max*ds.plnt_R, 45, get_special_object_name_size()*ds.ltf, 1.0f, false);

	const RGBf &col = get_color(MTK_COLOR_NAME_SPECIAL_OBJECT);
	glColor3f(col.r, col.g, col.b);

	glDisable(GL_DEPTH_TEST);
	draw_text(get_string(get_satellite_key(idx)));
	glPopMatrix();
}

