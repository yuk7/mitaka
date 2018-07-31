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
#include  "mtk_planetarium.h"
#include  "mtk_switches.h"
#include  "mtk_stars.h"
#include  "mtk_constellation.h"
#include  "mtk_solar_system.h"
#include  "mtk_sun.h"
#include  "mtk_planets.h"
#include  "mtk_satellites.h"
#include  "mtk_planet_position.h"
#include  "mtk_planet_orientation.h"
#include  "mtk_atmosphere.h"
#include  "mtk_objects.h"
#include  "mtk_target.h"
#include  "mtk_camera_setting.h"
#include  "mtk_draw_struct.h"
#include  "mtk_font.h"
#include  "mtk_text.h"
#include  "mtk_gl_common.h"
#include  "mtk_stereo.h"
#include  "mtk_mode.h"
#include  "mtk_draw_places.h"
#include  "mtk_strings.h"
#include  "mtk_selection.h"
#include  "mtk_eclipse.h"
#include  "mtk_target_planetarium.h"
#include  "../common/graphics/3D_common.h"
#include  "../common/graphics/gl_common.h"
#include  "../common/math/fast_pow.h"





//-----------
//  Externs
//-----------
extern GLuint  dlClstSphere;
extern float  landing_lon, landing_lat;
extern GLuint  texStar;
extern fast_pow  pow_for_stars;



void	draw_celestial_grid(const mtk_draw_struct& ds, bool bDigit);
void	draw_ecliptic(const mtk_draw_struct& ds);
void	draw_galactic_grid(const mtk_draw_struct& ds, bool bDigit);
void	draw_milkyway(const mtk_draw_struct& ds);


//-----------
//  Defines
//-----------

//--  NGC data  --
struct NGC_data2 : public NGC_data
{
	char	name[40];
};



//-------------
//  Constants
//-------------
const char szCompassKey[8][50] = {
	"COMPASS_S",
	"COMPASS_SE",
	"COMPASS_E",
	"COMPASS_NE",
	"COMPASS_N",
	"COMPASS_NW",
	"COMPASS_W",
	"COMPASS_SW"
};



const RGBAf		col_NGC(0.2,0.9,0.5,0.8);			// NGC



//-----------
//  Objects
//-----------
static std::vector<int>	vDispStarList;			// 表示用のリスト
map<short, NGC_data2>		NGCdata, ICdata;	// NGC

static float  g_plnt_angle = plnt_angle_ini;	// 視野角	
static float  g_plnt_R;						// 球殻半径
static float  g_plnt_d = 0;					// 球殻の中心のずれ


// 視点の方位と高度
static float g_plnt_long = 180;
//static float g_plnt_long = 0;
static float g_plnt_lati = plnt_lati_ini;

// 大気の明るさ
float AtmosphereIntensityPlanetarium;
float AtmosphereIntensityPlanetariumDef = 20.0;

float	get_plnt_R0() { return 0.001 * get_z_far(); }
float	get_plnt_angle()	{ return g_plnt_angle; }
float	get_plnt_R()		{ return g_plnt_R; }
float	get_plnt_d()		{ return g_plnt_d; }
void	set_plnt_R(float R)	{ g_plnt_R = R; }
float	get_plnt_long()		{ return g_plnt_long; }
float	get_plnt_lati()		{ return g_plnt_lati; }
void	set_plnt_long(const double& lon)	{ g_plnt_long = lon;  }
void	set_plnt_lati(const double& lat)	{ g_plnt_lati = lat; }

float	get_planetarium_zooming_factor()	{ return  get_plnt_R() / get_plnt_R0(); }



//------------
//  NGC data
//------------
int
read_a_NGC_record(FILE* fp, NGC_data2& nd)
{
	//--- read data
	int count;
	count = fread(&nd.NGC_NO,	sizeof(nd.NGC_NO),	1, fp);
	if (count < 1)  return FILE_FAILED;
	fread(&nd.bNGC,	sizeof(nd.bNGC),	1, fp);
	fread(&nd.eqPos.RAdeg,	sizeof(nd.eqPos.RAdeg),		1, fp);
	fread(&nd.eqPos.DEdeg,	sizeof(nd.eqPos.DEdeg),		1, fp);
	fread(&nd.type,	sizeof(nd.type),	1, fp);
	fread(&nd.size,	sizeof(nd.size),	1, fp);
	fread(&nd.mag,	sizeof(nd.mag),		1, fp);
	fread(&nd.bPhotographicMag,	sizeof(nd.bPhotographicMag),	1, fp);
	nd.name[0] = '\0';
	return  FILE_SUCCESS;
}

void
set_NGC_name(FILE* fp)
{
	const int record_size = 70+1;
	char  buf[record_size+1+1];
	int count;

	while (true) {
		//--- read one line
		buf[0] = 0;
		if (fgets(buf+1, record_size, fp) == NULL)  return;

		//--- read data
		short no;
		count = sscanf(buf+38, "%4hd", &no);
		if (count == 0)  continue;

		map<short, NGC_data2>::iterator it;
		if (buf[37]!='I') {
			it = NGCdata.find(no);
			if (it==NGCdata.end())  continue;
		} else {
			it = ICdata.find(no);
			if (it==ICdata.end())  continue;
		}

		char* p = buf+35;
		while (*p==' ') { p--; }
		p++;
		*p = '\0';
		strcpy(it->second.name, buf+1);
	}
}


bool
read_NGC_data(FILE* fp)
{
	int  res;
	NGC_data2  nd;

	while (true) {
		res = read_a_NGC_record(fp, nd);
		if (res == FILE_FAILED)  break;
		if (res == FILE_SUCCESS)	{
			if (nd.bNGC)	NGCdata.insert( pair<short,NGC_data2>(nd.NGC_NO, nd) );
			else			ICdata.insert(  pair<short,NGC_data2>(nd.NGC_NO, nd) );
		}
	}

	return  true;
}




//-------------------------------
//  プラネタリウムの視野角を設定する
//-------------------------------
void
set_planetarium_angle(float angle)
{
	g_plnt_angle = angle;
	const float th0 = plnt_angle_ini/2 / 180 * pi;
	const float th  = g_plnt_angle /2 / 180 * pi;
	const float R0 = get_plnt_R0();
	float R, d;
	R = R0 / (sin(th)/tan(th0) + 1 -cos(th));
	d = R - R0;
	g_plnt_R = R;
	g_plnt_d = d;
}



//----------------
//  東西南北を書く
//----------------
void
draw_grid_compass(const float r, const float sz)
{
	const float  a = pi / 180.0f;
	const float  b = pi / 12.0f;

	for (int i=0; i<8; i++) {
		float th_d,phi_d;
		th_d  = 0 + 0.3;

		phi_d = float(i)*45;
		if (i==4)		phi_d += 1.5;	// 北
		if (i%2 == 1)	phi_d += 1.5;
		
		glPushMatrix();
			glRotatef(phi_d-90,0,0,1);
			glRotatef(th_d,1,0,0);
			glTranslatef(0,r,0);
			glRotatef(90, 1,0,0);
			glScalef(sz,sz,sz);
			draw_string(szCompassKey[i]);
		glPopMatrix();
	}


	// 緯度
	for (int i=-90; i<=90; i+=15) {
		float th_d,phi_d;
		th_d  = float(i) + 0.3;
		phi_d = 0 -0.3;
		
		glPushMatrix();
			glRotatef(phi_d-90 +180,0,0,1);
			glRotatef(th_d,1,0,0);
			glTranslatef(0,r,0);
			glRotatef(90, 1,0,0);

			glScalef(sz,sz,sz);
			draw_textf("%d\xc2\xb0", i);
		glPopMatrix();
	}
}



//--------------------
//  地平座標
//--------------------
void
go_to_horizon_coord(const mtk_draw_struct& ds)
{
	set_planet_orientation_GL( get_object(ds.tgt_ID).get_index() );

	glRotatef(landing_lon, 0,0,1);
	glRotatef(90-landing_lat, 0,1,0);
}

void
draw_horizon_coord(const mtk_draw_struct& ds)
{
	int tgt_ty = get_object( ds.tgt_ID ).get_type();
	if (tgt_ty != OBJ_TYPE_PLANET && tgt_ty != OBJ_TYPE_SATELLITE)	return;

	const float r = 1.00001;


	const float lw = ds.line_width_fac / (ds.rad_per_pixel / 0.001);
	glLineWidth(lw);

	glColor4f(0,0.7,0.9,0.8f);
	glPushMatrix();
		go_to_horizon_coord(ds);
		draw_grid_compass(r, 1.0f/40.0f * 0.5 * ds.ltf);
		glScalef(r,r,r);
		glCallList(dlClstSphere);
	glPopMatrix();
}



//----------
//  空を描く
//----------
//const float sky_doom_fac = 0.95;

void
draw_sky2(const mtk_draw_struct& ds, const double& Rs, int plnt_idx, const vector3f& n_sun, bool bMultipleScattering)
{
	const int imax = 30;
	const int jmax = 30;
	RGBAf rgba[imax+1][jmax+1];
	static vector3f  pos[imax+1][jmax+1];

	if (!has_atmosphere(plnt_idx)) {
		return;
	}


	const vector3d r_sun = get_sun_position();


	// parameter for atmosphere
	const atmosphere& atm = get_atmosphere(plnt_idx);
	const float zrt = get_zoom_rate().zmpPlanet;
	const float R1 = atm.R * zrt;
	const float H  = atm.H * zrt;
	const float R2 = R1 + 20 * H;
	const float n0 = atm.n0 / zrt;	// Earth
	const float I0 = AtmosphereIntensityPlanetarium;	// incident intensity

	//---  観測地点  ---
	planet& pnt = get_planet(plnt_idx);
	TopoSphere* tp = pnt.get_topography();

	double elv = 0.0;

	if (tp!=0) {
		elv = tp->get_elevation(landing_lon, landing_lat);
	}
	const vector3f r0 = vector3f(0,0,R1+elv);


	// 太陽光に照らされた大気が見えない場合は何もしない
	const float thc = 2 * acos(R1/R2);
	const float th  = asin(-n_sun.z);

	if (th > thc)  return;


	//---  Eclipse  ---
	bool bEclipse = false;
	//if (satellite_cast_shadow_on_planet(0)) {
	if (swEclipse & ECLIPSE_DISP) {
		eclipse_info  eci;
		vector3d r1, r2;
		double R1, R2, Rs, Rx, Ry, Rz;
		r1 = get_planet_position(plnt_idx);
		R1 = get_planet_Re(ds.tyMode, plnt_idx) * _1km_pc;
		r2 = get_satellite_position(0);
		get_satellite_size(MODE_PLANETARIUM, 0, Rx, Ry, Rz);
		R2 = Rx * _1km_pc;
		Rs = R_sun_km * _1km_pc;

		eci = calc_eclipse_info(r1, R1, r2, R2, r_sun, Rs);
		if (eci.bFlg)  bEclipse = true;
	}
	const double fe = 0.96;


	//
	GLfloat M[16];
	glGetFloatv( GL_MODELVIEW_MATRIX,  M);
	const vector3f ex  = normalize( vector3f(M[0], M[4], M[8]) );
	const vector3f ey  = normalize( vector3f(M[1], M[5], M[9]) );
	const vector3f ez  = normalize( vector3f(M[2], M[6], M[10]));
	const vector3f _ex = normalize( vector3f(M[0], M[1], M[2]) );
	const vector3f _ey = normalize( vector3f(M[4], M[5], M[6]) );
	const vector3f _ez = normalize( vector3f(M[8], M[9], M[10]));

	// 画角
	const frustum& frs = ds.get_frustum();
	const float z1 = frs.get_z1();
	const float x1 = frs.get_x1()/z1;
	const float x2 = frs.get_x2()/z1;
	const float y1 = frs.get_y1()/z1;
	const float y2 = frs.get_y2()/z1;
	const float R  = ds.plnt_R;
	const float R0 = get_plnt_R0();
	const float th0 = 0.5 * plnt_angle_ini / 180.0 * M_PI;
	const float th1 = 0.5 * ds.plnt_angle  / 180.0 * M_PI;
	const float h = R0*cos(th0);
	const float d = (R - R0)/h;



	//-- Make color table
	const float xl = fabs(x2-x1);
	const float yl = fabs(y2-y1);
	const float l1 = min(xl, yl);
	const float l2 = max(xl, yl);
	const float angle_max = 2 * atan2(l2*sin(th1), l1*cos(th1)) / M_PI * 180;
	const float opq_fac = 14;	// 明るさから不透明度を決める因子




	if (angle_max < 90) {
		// 視野が90度以下

		for (int j=0; j<=jmax; j++) {
			float f = float(j)/jmax;
			float x = (x2-x1)*f + x1;
			float _y1 = y1;
			
			for (int i=0; i<=imax; i++) {
				f = float(i)/imax;
				float y = (y2-_y1)*f + _y1;

				// 視線方向ベクトルの計算
				vector3f nv;
				const float _R = Rs * R/h;
				vector3f nv0 = normalize( vector3f(x,y,-1) );
				vector3f _r0(0,0,-d);

				// スクリーンの回転の補正
				{
					vector3d  eF, eU, eR;
					quaterniond  ori = get_local_camera_orientation() * get_base_camera_ori();
					quaternion_to_axes( ori.inv(), eF, eU, eR);
					_r0 = eF * d;
				}
				
				const float b = dot_product(nv0, _r0);
				const float D = b*b - d*d + _R*_R;
				if (D >= 0.0) {
					const float t = -b + sqrt(D);
					nv = normalize( t*nv0 + _r0 );
				} else {
					nv.set(0,0,0);
				}

				vector3f ev;
				ev = ex*nv.x + ey*nv.y + ez*nv.z;
				if (ev.z <= 0.00001) {
					ev.z = 0.00001;
				}
				ev = normalize(ev);
				
				pos[i][j] = ev;
				const vector3f lv = n_sun;
				

				float I = I0;
				if (bEclipse) {
					vector3d r1, r2;
					double R1, R2, Rs, Rx, Ry, Rz;
					r1 = get_planet_position(plnt_idx);
					R1 = get_planet_Re(ds.tyMode, plnt_idx) * _1km_pc;
					r2 = get_satellite_position(0);
					get_satellite_size(MODE_PLANETARIUM, 0, Rx, Ry, Rz);
					R2 = Rx * _1km_pc;
					Rs = R_sun_km * _1km_pc;

					vector3d pos0 = R1 * get_planet_nv_ec(plnt_idx, landing_lon, landing_lat) + r1;


					vector3d ev2;
					ev2 = Rz3deg(landing_lon) * Ry3deg(90 - landing_lat) * ev;	// Hori -> obj
					ev2 = get_planet_orientation(plnt_idx, true).transposed() * ev2;
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



				RGBf col = calc_scattered_light_intensity(R1, R2, n0, H, I, r0, ev, lv, 22);
				if (bMultipleScattering) {
					//--  多重散乱は、実スケールの地球について計算しているので r0/zrt とする
					float I2 = fe*I + (1 - fe)*I0;
					col += I2 * calc_multiple_scattered_light_intensity(r0/zrt, ev, lv);
				}
				
				rgba[i][j].rgb = correct_RGB(col);
				const RGBf& rgb = rgba[i][j].rgb;
				float aa = opq_fac * max(rgb.r, max(rgb.g, rgb.b));
				if (aa > 1) aa = 1;
				rgba[i][j].a   = aa;
			}
		}
	} else {
		// 視野が90度以上
		for (int i=0; i<=imax; i++) {
			float f = float(i)/imax;
			const float th = 0.5*f*M_PI;
			const float z = cos(th);
			const float rho = sin(th);
			for (int j=0; j<=jmax; j++) {
				f = float(j)/jmax;
				const float phi = 2 * f * M_PI;
				const float x = rho * cos(phi);
				const float y = rho * sin(phi);

				vector3f ev(x,y,z);
				ev = normalize(ev);

				pos[i][j] = ev;
				const vector3f lv = n_sun;
				

				float I = I0;
				if (bEclipse) {
					vector3d r1, r2;
					double R1, R2, Rs, Rx, Ry, Rz;
					r1 = get_planet_position(plnt_idx);
					R1 = get_planet_Re(ds.tyMode, plnt_idx) * _1km_pc;
					r2 = get_satellite_position(0);
					get_satellite_size(MODE_PLANETARIUM, 0, Rx, Ry, Rz);
					R2 = Rx * _1km_pc;
					Rs = R_sun_km * _1km_pc;

					vector3d pos0 = R1 * get_planet_nv_ec(plnt_idx, landing_lon, landing_lat) + r1;

					vector3d ev2;
					ev2 = Rz3deg(landing_lon) * Ry3deg(90 - landing_lat) * ev;	// Hori -> obj
					ev2 = get_planet_orientation(plnt_idx, true).transposed() * ev2;
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


				RGBf col = calc_scattered_light_intensity(R1, R2, n0, H, I, r0, ev, lv, 22);
				if (bMultipleScattering) {
					//--  多重散乱は、実スケールの地球について計算しているので r0/zrt とする
					float I2 = fe*I + (1 - fe)*I0;
					col += I2 * calc_multiple_scattered_light_intensity(r0/zrt, ev, lv);
				}


				rgba[i][j].rgb = correct_RGB(col);

				const RGBf& rgb = rgba[i][j].rgb;
				float aa = opq_fac * max(rgb.r, max(rgb.g, rgb.b));
				if (aa > 1) aa = 1;
				rgba[i][j].a   = aa;
			}
		}
	}


	//--
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	glDisable(GL_DEPTH_TEST);	//***
	glEnable(GL_CULL_FACE);		//***


	for (int i=0; i<imax; i++) {
		glBegin(GL_TRIANGLE_STRIP);
		for (int j=0; j<=jmax; j++) {
			glColor4fv(rgba[i+1][j].get_pointer());
			glVertex3fv(pos[i+1][j].get_pointer());
			glColor4fv(rgba[i  ][j].get_pointer());
			glVertex3fv(pos[i  ][j].get_pointer());
		}
		glEnd();
	}

	glDisable(GL_BLEND);
}



//------------
//  地面を書く
//------------
void
draw_ground0()
{
	vector3f  pos[19][73];
	glColor3f(0,0,0);

	//-- Make pos table
	for (int i=0; i<=18; i++) {
		float  th= 5*float(i) * M_PI / 180.0;
		float z = -cos(th);
		float rho = sin(th);
		for (int j=0; j<=72; j++) {
			float phi = 5*float(j) * M_PI/180.0;
			pos[i][j].set(rho*cos(phi), rho*sin(phi), z);
		}
	}

	//-- 地面を書く
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glBegin(GL_QUADS);
	for (int i=0; i<18; i++) {
		for (int j=0; j<72; j++) {
			glVertex3fv(pos[i+1][j].get_pointer());
			glVertex3fv(pos[i+1][j+1].get_pointer());
			glVertex3fv(pos[i][j+1].get_pointer());
			glVertex3fv(pos[i][j].get_pointer());
		}
	}
	glEnd();

	// 東西南北のラベルを書く
	glColor3f(0.6,0.6,0.5);
	for (int i=0; i<8; i++) {
		float th_d,phi_d;
		th_d  = 0 - 3;
		phi_d = float(i)*45;
		
		glPushMatrix();
			glScalef(0.99,0.99,0.99);
			glRotatef(phi_d-90,0,0,1);
			glRotatef(th_d,1,0,0);
			glTranslatef(0,1,0);
			glRotatef(90, 1,0,0);

			glScalef(0.04,0.04,0.04);
			draw_string(szCompassKey[i]);
		glPopMatrix();
	}
}


void
draw_ground(const mtk_draw_struct& ds, const double& Rg)
{
	glPushMatrix();
	go_to_horizon_coord(ds);
	glScalef(Rg, Rg, Rg);

	draw_ground0();
	glPopMatrix();
}


void
draw_sky(const mtk_draw_struct& ds, const double& Rs)
{
	const float d2r = M_PI/180.0;

	const mtk_object&  tgt = get_object(ds.tgt_ID);

	if (tgt.get_type() != OBJ_TYPE_PLANET) return;
	const int plnt_idx = tgt.get_index();

	//--  地平座標系での太陽の位置を計算  --
	const vector3d r_sun = get_sun_position();
	vector3d rs, n_sun;
	
	rs = r_sun - ds.camera.get_pos();							// 惑星地表黄道座標での太陽の位置
	
	rs = get_planet_orientation(plnt_idx, true) * rs;			// ec -> pl 
	rs = Ry3deg(-90+landing_lat) * Rz3deg(-landing_lon) * rs;	// pl -> 地平座標
	n_sun = rs.normalized();

	glPushMatrix();
		go_to_horizon_coord(ds);
		glScalef(Rs, Rs, Rs);

		if (plnt_idx == PLNT_EARTH && (AtmosphereMode != ATMOSPHERE_NONE)) {	// 地球のみ空を描画する
			draw_sky2(ds, Rs, PLNT_EARTH, n_sun, (AtmosphereMode == ATMOSPHERE_MULTIPLE));
		}
	glPopMatrix();
}



//-------------
//  NGC の表示
//-------------
void
draw_NGC(const mtk_draw_struct& ds)
{
	char str[100];
	const float R = 0.9955;
	const float  a = pi / 180.0f;
	const float screen_w = get_screen_w();
	const float screen_z = get_screen_z();


	glColor4fv(col_NGC.get_pointer());
	glPointSize(3.5f);

	// 球の中心の座標を得る
	float M[16];
	vector3f  rc;
	glGetFloatv( GL_MODELVIEW_MATRIX, M);
	rc.x = M[12];
	rc.y = M[13];
	rc.z = M[14];

	for (map<short, NGC_data2>::const_iterator it=NGCdata.begin(); it!=NGCdata.end(); ++it) {
		const NGC_data2& nd = it->second;

		if (nd.name[0] == '\0')  continue;
		
		vector3f  xyz = nd.eqPos.get_dirc_vec();
		vector3f  xyzE = (M_R2E * xyz) * R;
		glBegin(GL_POINTS);
			glVertex3fv( xyzE.get_pointer() );			
		glEnd();


		glPushMatrix();
			glTranslatef( xyzE.x, xyzE.y, xyzE.z );
			prepare_3Dtext_2(rc, 0, 3*ds.ltf, screen_w, screen_z, true, false);
			
			if (nd.name[0]!='\0')	{ sprintf(str, " NGC%hd(%s)", nd.NGC_NO, nd.name);
			} else					{ sprintf(str, " NGC%hd", nd.NGC_NO); }

			if (nd.name[0] != '\0') {
				draw_text(str);
			}
		glPopMatrix();
	}
}



//----------------
//  星の名前の表示
//----------------
void
draw_star_name_planetarium(const mtk_draw_struct& ds)
{
	if (!(swStar & (STAR_NAME | STAR_BAYER | STAR_FRAMSTEED | STAR_HIP)))  return;

	const double R = 0.995;
	const vector3d camera_pos = ds.camera.get_pos();
	const float screen_w = get_screen_w();
	const float screen_z = get_screen_z();


	// 球の中心の座標を得る
	float M[16];
	vector3f  rc;
	glGetFloatv( GL_MODELVIEW_MATRIX, M);
	rc.x = M[12];
	rc.y = M[13];
	rc.z = M[14];

	glDisable(GL_DEPTH_TEST);

	const int N = get_stars_num();
	for (int i=0; i<N; i++) {
		const mtk_star& st = get_star(i);


		bool bNameLarge, bNameSmall;
		bNameLarge = st.get_bNameLarge();
		bNameSmall = st.get_bNameSmall();
		if (!bNameLarge && !bNameSmall)		continue;	// 表示する名前が無い場合




		vector3d xyz = R * normalize(st.get_pos() - camera_pos);


		//**** culling


		glPushMatrix();
			glTranslatef(xyz.x, xyz.y, xyz.z);

			const float sz = 0.2 * st.get_disp_size();	//******
			prepare_3Dtext_2(rc, sz, 3.3*ds.ltf, screen_w, screen_z, true, false);
    			
			glColor4fv(col_star_name.get_pointer());
			
			
			
			if (bNameLarge) {
				draw_text(st.get_NameLarge());
			}
			
			if (bNameSmall) {
				glColor3f(0.7,0.7,0.6);
				draw_text(st.get_NameSmall());
			}
		glPopMatrix();
	}

	glEnable(GL_DEPTH_TEST);
}



//-----------
//  カリング
//-----------
void
prepare_draw_planetarium(mtk_draw_struct *v_ds, int Nds)
{

	//  基準の明るさの設定(スケール依存）
	float lv0 = 100000 / get_L_ref();
	switch (get_display_mode()) {
	case DISPMODE_SIDE_BY_SIDE:
	case DISPMODE_TOP_AND_BOTTOM:
		lv0 *= 2.5;
		break;
	case DISPMODE_DOME_MASTER:
		lv0 *= 4.0;
		break;
	default:
		break;
	}






	const vector3d cpos = v_ds[0].camera_base.get_pos();
	const float z_mid = get_z_mid();
	const float fac = 0.001226 * get_plnt_R0() / v_ds[0].plnt_R;
	const float fac2 = v_ds[0].scale_factor * v_ds[0].plnt_R;


	// calc culling vector
	vector3d nv[8];
	for (int j = 0; j < Nds; j++) {
		v_ds[j].frs_near.calc_cull_vectors(&nv[j * 4]);
	}


	vector3d eF, eU, eR;
	v_ds[0].camera_base.get_axes(eF, eU, eR);

	vector3d d;
	d = double(v_ds[0].plnt_d) * eF;


	vDispStarList.clear();
	for (int i = 0; i < get_stars_num(); i++) {
		mtk_star& st = get_star(i);

		vector3d pos = st.get_pos();
		vector3d rpos = pos - cpos;

		double dist_sqr = rpos.norm2();
		double lv = lv0 * st.get_L() / dist_sqr;
		
		float size;
		if (lv > 1) {
			size = pow_for_stars.eval(lv);
		}
		else {
			size = 1;
			continue;
		}

		float Rb0, Rb;
		Rb0 = size * fac;
		Rb = Rb0 * fac2;


		vector3d npos = rpos / sqrt(dist_sqr);
		vector3d r = double(v_ds[0].plnt_R) * npos - d;
		vector3d rv0 = v_ds[0].calc_pos_on_viewframe(r);
		if (-rv0.z + Rb < z_mid) continue;

		
		bool bOutside = true;
		for (int j = 0; j < Nds; j++) {
			vector3d rv = v_ds[j].calc_pos_on_viewframe(r);

			bool bInside = true;
			for (int k = 0; k < 4; k++) {
				double d = dot_product(nv[j*4+k], rv);

				if (d > Rb) {
					bInside = false;
					break;
				}
			}
			if (bInside) {
				bOutside = false;
				break;
			}
		}
		if (bOutside) continue;


		st.set_disp_pos(npos);
		st.set_disp_size(size);
		st.set_lv(lv);
		st.Rb = Rb0;
		


		// ベースカメラを向いたビルボード
		if (v_ds[0].bRotateBillboard) {
			vector3f  eF2, eU2, eR2;
			eF2 = normalize(rpos);
			eR2 = normalize(cross_product(eF2, v_ds[0].eUf));
			eU2 = cross_product(eR2, eF2);
			
			st.eR = eR2;
			st.eU = eU2;
		}


		vDispStarList.push_back(i);
	}
}



//-----------
//  星の表示 
//-----------
int
draw_stars_planetarium_sub(const mtk_draw_struct& ds, const int LumRankThr)
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);


	int count = 0;
	for (int i = 0; i < vDispStarList.size(); i++) {
		int idx = vDispStarList[i];
		const mtk_star& st = get_star(idx);

		const float lv = st.get_lv();
		const star_picture& STP = get_star_picture();
		int  LumRank = STP.get_Lum_rank(lv);


		// 暗い星は表示しない
		const vector3d xyz = st.get_disp_pos();
		if (LumRank > LumRankThr) {
			int	TempIdx = st.TempIdx;
			RGBf RGBcr = STP.get_correct_RGB(TempIdx, lv);

			glColor3f(1, 1, 1);
			draw_a_star(ds, xyz, st.Rb, RGBcr, idx);
			count++;
		}

		if (tyStarMark != STARMARK_NONE) {
			float alp = 1.0;
			const float sz = st.get_disp_size();

			switch (tyStarMark) {
			case STARMARK_DOUBLE:		draw_double_star_mark(ds, xyz, st, sz, alp);	break;
			case STARMARK_EXOPLANETS:	mark_stars_with_planets(ds, xyz, st, sz, alp);	break;
			default:																	break;
			}
		}
	}

	return count;
}




int
draw_stars_planetarium(const mtk_draw_struct& ds, bool _bDrawLowRes, bool bFar)
{
	if (ds.tyMode != MODE_PLANETARIUM)  return 0;

	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glBindTexture(GL_TEXTURE_2D, texStar);

	int count = draw_stars_planetarium_sub(ds, 7);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_TEXTURE_2D);
	glDepthMask(GL_TRUE);
	return count;
}



void
draw_planetarium(mtk_draw_struct ds)
{
	ds.set_far_region();
	gl_set_frustum(ds.get_frustum());
	glMatrixMode(GL_MODELVIEW);
	


	//---  ズームイン・ズームアウト  ---
	if (get_display_mode() != DISPMODE_DOME_MASTER) {
		vector3d eF, eU, eR;
		ds.camera_base.get_axes(eF, eU, eR);
		double l = -ds.plnt_d;
		glTranslated(l*eF.x, l*eF.y, l*eF.z);
	}


	//---  天球のサイズにスケールする  ---
	const float scl = ds.plnt_R;
	glScalef(scl, scl, scl);



	const vector3d r_sun = get_sun_position();


	bool bDrawStars = true;
	if (AtmosphereMode != ATMOSPHERE_NONE) {
		const mtk_object&  tgt = get_object(ds.tgt_ID);

		if (tgt.get_type() == OBJ_TYPE_PLANET) {
			const int plnt_idx = tgt.get_index();

			if (plnt_idx == PLNT_EARTH) {
				//--  地平座標系での太陽の位置を計算  --
				vector3d rs, n_sun;

				rs = r_sun - ds.camera.get_pos();					// 惑星地表黄道座標での太陽の位置

				rs = get_planet_orientation(plnt_idx, true) * rs;				// ec -> pl 
				rs = Ry3deg(-90 + landing_lat) * Rz3deg(-landing_lon) * rs;	// pl -> 地平座標
				n_sun = rs.normalized();

				if (n_sun.z > 0) {
					bDrawStars = false;

					if (swEclipse & ECLIPSE_DISP) {
						eclipse_info  eci;
						vector3d r1, r2;
						double R1, R2, Rs, Rx, Ry, Rz;
						r1 = get_planet_position(plnt_idx);
						R1 = get_planet_Re(ds.tyMode, plnt_idx) * _1km_pc;
						r2 = get_satellite_position(0);
						get_satellite_size(MODE_PLANETARIUM, 0, Rx, Ry, Rz);
						R2 = Rx * _1km_pc;
						Rs = R_sun_km * _1km_pc;

						eci = calc_eclipse_info(r1, R1, r2, R2, get_sun_position(), Rs);
						if (eci.bFlg) {
							bDrawStars = true;
						}
					}
				}
			}
		}
	}



	if (bDrawStars) {

		draw_milkyway(ds);


		if (swConstellation & CNST_BND)		{ draw_constellation_boundary(ds); }
		if (swConstellation & CNST_ILLUST)	{ draw_constellation_illust(ds); }
		if (swConstellation & CNST_LINE)	{ draw_constellation_lines(ds); }
		if (bCelestialGrid)					{ draw_celestial_grid(ds, true); }
		if (bEcliptic)						{ draw_ecliptic(ds); }
		if (bGalacticGrid)					{ draw_galactic_grid(ds, true); }
		if (bHorizonCoord)					{ draw_horizon_coord(ds); }
	}


	//***
	glEnable(GL_DEPTH_TEST);

	
	const double Rc = 1.0;

	if (bDrawStars) {

		if (ds.tgt_pos.norm() < 0.1) {
			if (test_disp_switch(DISPSW_PLANET, DISPM_DISP))		{ draw_planets_planetarium(ds); }
			if (test_disp_switch(DISPSW_SATELLITE, DISPM_DISP))		{ draw_satellites_planetarium(ds, Rc); }
			if (bNGC)												{ draw_NGC(ds); }
			if (swConstellation & CNST_NAME)						{ draw_constellation_name(ds); }
		}

		draw_star_name_planetarium(ds);
		if (swStar & STAR_DISP)	{ draw_stars_planetarium(ds, false, true); }
	}

	glDisable(GL_DEPTH_TEST);

	// Sun and the Moon
	draw_sky(ds, Rc);
	if (test_disp_switch(DISPSW_PLANET, DISPM_DISP)) { draw_sun_planetarium_2(ds, Rc, AtmosphereIntensityPlanetarium); }
	if (test_disp_switch(DISPSW_SATELLITE, DISPM_DISP)) { draw_moon_planetarium(ds, Rc, AtmosphereMode, AtmosphereIntensityPlanetarium); }

	if (test_disp_switch(DISPSW_PLANET, DISPM_DISP))	{ draw_sun_label_planetarium(ds, Rc); }
	if (test_disp_switch(DISPSW_SATELLITE, DISPM_DISP)) { draw_moon_label_planetarium(ds, Rc); }


	// Ground
	draw_ground(ds, Rc);
}
