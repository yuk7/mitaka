/**************************************************************************************************

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
#include  "mtk_dome_master.h"
#include  <windows.h>
#include  "mtk_3D_chart.h"
#include  "mtk_gl_common.h"
#include  "mtk_font.h"
#include  "mtk_colors.h"
#include  "mtk_screen.h"
#include  "mtk_planets.h"
#include  "mtk_planet_orientation.h"
#include  "mtk_satellite_orientation.h"
#include  "mtk_minor_planets.h"
#include  "mtk_title_page.h"
#include  "mtk_tcp.h"
#include  "mtk_strings.h"
#include  "mtk_mode.h"




//****
// Extern
extern bool	bControler;




static float chart_lon_angle = 0.0;
static float chart_lat_angle = 0.0;
static float chart_zoom_factor = 1.0;
const float chart_zoom_factor_min = 0.2;
const float chart_zoom_factor_max = 5.0;



float
get_chart_lon_angle()
{
	return  chart_lon_angle;
}

float
get_chart_lat_angle()
{
	return  chart_lat_angle;
}

float
get_chart_zoom_factor()
{
	return  chart_zoom_factor;
}


void
OnChartLonAngle(float angle)
{
	chart_lon_angle = angle;
	if (chart_lon_angle < 0)   chart_lon_angle += 360;
	if (chart_lon_angle > 360) chart_lon_angle -= 360;

	if (bControler)	{ send_data_float(DATA_FLOAT_CHART_LON_ANGLE, chart_lon_angle); }
}

void
OnChartLatAngle(float angle)
{
	chart_lat_angle = angle;
	if (chart_lat_angle < -90)  chart_lat_angle = -90;
	if (chart_lat_angle > 90)   chart_lat_angle = 90;

	if (bControler)	{ send_data_float(DATA_FLOAT_CHART_LAT_ANGLE, chart_lat_angle); }
}

void
OnChartZoomFactor(float fac)
{
	chart_zoom_factor = fac;
	if (chart_zoom_factor < chart_zoom_factor_min)  chart_zoom_factor = chart_zoom_factor_min;
	if (chart_zoom_factor > chart_zoom_factor_max)  chart_zoom_factor = chart_zoom_factor_max;

	if (bControler)	{ send_data_float(DATA_FLOAT_CHART_ZOOM_FACTOR, chart_zoom_factor); }
}


void
set_chart_pos()
{
	const double* Mbc;
	Mbc = get_local_camera_matrix_GL();

	glLoadIdentity();
	glMultMatrixd(Mbc);


	double M[16];
	quaternion_to_matrix_GL_tr(get_base_camera_ori(), M);
	glMultMatrixd(M);


	if (get_display_mode() == DISPMODE_DOME_MASTER && get_mode() == MODE_PLANETARIUM) {
		glRotatef(get_dome_master_elev_angle() - get_dome_master_elev_angle_planetarium(), 1, 0, 0);
	}


	glRotatef(-chart_lon_angle, 0, 1, 0);
	glRotatef(-chart_lat_angle, 1, 0, 0);
	glTranslatef(0, 0, -1.2*get_screen_z());
}





//--------------
//  太陽系の惑星
//--------------
void
draw_3DChart_planets()
{
	const int planets_num = 8;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glColor3f(1,1,1);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	float mat_d[] = {1,1,1,1}, mat_a[] = {0.5,0.5,0.5,1};
	glMaterialfv( GL_FRONT_AND_BACK , GL_DIFFUSE,   mat_d );
	glMaterialfv( GL_FRONT_AND_BACK , GL_AMBIENT,   mat_a );

	const float scl = get_screen_w() * get_title_width_ratio();
	glScalef(scl, scl, scl);
	glTranslatef(0,0,-0.5);
	float sz = 0.11;
	glScalef(sz, sz, sz);



	float pos[4];
	pos[0] = -0.4;
	pos[1] = 0;
	pos[2] = 1;
	pos[3] = 0.0f;
	glLightfv( GL_LIGHT0, GL_POSITION, pos );


	// タイトル
	const std::string& str = get_string("3DCHART_PLANETS_TITLE");

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glColor3f(0.8,0.8,0.8);
	glPushMatrix();
		glTranslatef(0,2.4,0);
		draw_text_centering(str);
	glPopMatrix();



	float size0 = get_planet(5).get_Re();	// 土星のサイズを基準に取る
	float dmin1 = 1.2, dmin2 = 1.5;
	float dl[planets_num+2];
	float dtot = 0;
	for (int i=0; i<planets_num; i++) {
		planet& pnt = get_planet(i);

		double Re = pnt.get_Re() / size0;
		float _dl;
		if (pnt.hasRing()) {
			const particle_ring& pr = pnt.get_ring();
			_dl = 2*Re*pr.get_R2();
		} else {
			_dl = 2*Re;
		}
		float dmin = (i<=3) ? dmin1 : dmin2;
		dl[i] = (_dl < dmin) ? dmin : _dl;
		dtot += dl[i];
	}
	dl[planets_num+1]=0;


	
	glTranslatef(-0.5*dtot + 0.5*dl[0],0,0);
	for (int i=0; i<planets_num; ++i) {
		planet& pnt = get_planet(i);

		//
		glPushMatrix();
			glRotatef(-90, 1,0,0);
			set_planet_orientation_GL(i);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_LIGHTING);
			float sz = pnt.get_Re()/size0;
			glScalef(sz,sz,sz);
			draw_planet2(i);
		glPopMatrix();


		// 名前
		glPushMatrix();
			glTranslatef(0,-1.9,0.0);
			const float sz2 = 0.35;
			glColor3f(0.8,0.8,0.8);
			glScalef(sz2, sz2, sz2);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_LIGHTING);
			draw_text_centering(get_string(pnt.get_key()));
		glPopMatrix();

		glTranslatef(0.5*(dl[i]+dl[i+1]),0,0);
	}

	glDisable(GL_NORMALIZE);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
}







//--------
//  TNOs
//--------
void
draw_3DChart_TNOs()
{
	const int dwarf_planets_num = 8;


	struct dp_info {
		const char* key;
		int MP_no;
	};

	const dp_info DP[dwarf_planets_num] = {
		{ "MP_136199",	136199 },		// Eris
		{ "PLNT_PLUTO", 134340 },		// Pluto
		{ "MP_136108",	136108 },		// Haumea
		{ "MP_90377",	90377 },		// Sedna
		{ "MP_136472",	136472 },		// Makemake
		{ "MP_90482",	90482 },		// Orcus
		{ "MP_50000",	50000 },		// Quaoar
		{ "MP_20000",	20000 }			// Varuna
	};


	const double R0 = get_planet(PLNT_PLUTO).get_Re();	// 冥王星のサイズを基準
	double Rx, Ry, Rz;
	get_small_object_size( get_small_object_index(DP[4].key), Rx, Ry, Rz );
	const float dy = 1 - Rx / R0;


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glColor3f(1,1,1);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	float mat_d[] = {1,1,1,1}, mat_a[] = {0.5,0.5,0.5,1};
	glMaterialfv( GL_FRONT_AND_BACK , GL_DIFFUSE,   mat_d );
	glMaterialfv( GL_FRONT_AND_BACK , GL_AMBIENT,   mat_a );

	const float scl = get_screen_w();
	glScalef(scl, scl, scl);
    glTranslatef(0,0,-0.56);


	float pos[4];
	pos[0] = -0.4;
	pos[1] = 0;
	pos[2] = 1;
	pos[3] = 0.0f;
	glLightfv( GL_LIGHT0, GL_POSITION, pos );


	glDisable(GL_BLEND);
	glScalef(0.2, 0.2, 0.2);


	//--  Title
	const std::string& str = get_string("3DCHART_TNOS_TITLE");
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glColor3f(0.8,0.8,0.8);
	glPushMatrix();
		glTranslatef(0,3.2,0);
		float w, h;
		get_text_size(str, w, h);
		float sz = 7.0 / w;

		//float sz = 15.0 / len;
		glScalef(sz,sz,sz);
		draw_text_centering(str);
	glPopMatrix();




	//--
	glTranslatef(-3, 1, 0);
	for (int iy=0; iy<2; iy++) {
		glPushMatrix();

		for (int ix=0; ix<4; ix++) {
			int i = iy*4 + ix;

			double Rx, Ry, Rz;
			if (i==1) {	// Pluto
				Rx = Ry = Rz = R0;
			} else {
				get_small_object_size(get_small_object_index(DP[i].key), Rx, Ry, Rz);
			}

			glPushMatrix();
				glEnable(GL_LIGHTING);
				float szx = 0.8 * Rx / R0;
				float szy = 0.8 * Ry / R0;
				float szz = 0.8 * Rz / R0;
				glScalef(szx,szy,szz);
				
				//****
				if (i==1) {
					glMaterialfv( GL_FRONT_AND_BACK , GL_DIFFUSE,   mat_d );
					glMaterialfv( GL_FRONT_AND_BACK , GL_AMBIENT,   mat_a );

					glEnable(GL_TEXTURE_2D);
					glRotatef(-50, 1,0,0);
					glRotatef(90, 0, 0, 1);
					draw_planet2(PLNT_PLUTO);
				} else {
					glRotatef(-90, 1, 0, 0);

					glDisable(GL_TEXTURE_2D);
					setup_drawing_small_object( get_small_object_index( DP[i].key ) );
					gl_draw_LOD_sphere();
				}
			glPopMatrix();

			// 名前
			glPushMatrix();
				glTranslatef(0,-1.25,0.0);
				if (iy>=1) {
					glTranslatef(0,dy,0.0);
				}

				glColor3f(0.8,0.8,0.8);
				glScalef(0.3,0.3,0.3);
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_LIGHTING);

				glPushMatrix();
					draw_text_centering( get_string(DP[i].key) );
				glPopMatrix();

				glTranslatef(0,-1,0);
				glColor3f(0.5,0.7,0.8);
				glScalef(0.7, 0.7, 0.7);
				draw_text_centering(to_string(DP[i].MP_no));
			glPopMatrix();


			glTranslatef(2,0,0);
		}

		glPopMatrix();
		glTranslatef(0,-2-dy,0);
	}


	glDisable(GL_NORMALIZE);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
}




//-----------------
//  星の色と温度の図
//-----------------
void
draw_3DChart_star_color()
{
	float mat_d[] = {1.2,1.2,1.2,1}, mat_a[] = {1,1,1,1};


	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glEnable(GL_LIGHT2);

	glDisable(GL_TEXTURE_2D);

	mat_a[0] = 0;
	mat_a[1] = 0;
	mat_a[2] = 0;
	mat_a[3] = 1.0;
	glMaterialfv( GL_FRONT_AND_BACK , GL_AMBIENT,   mat_a );


	const int N = 5;
	double  Temp[N] = { 45000, 29000, 9600, 5800, 3800 };
	const char starnames_key[N][50] = {
		"3DCHART_STARS_ZETA_ORIONIS",
		"3DCHART_STARS_SPICA",
		"3DCHART_STARS_VEGA",
		"3DCHART_STARS_SUN",
		"3DCHART_STARS_BETELGEUSE"
	};

	std::string str;
	glPushMatrix();
		const float scl = get_screen_w() * get_title_width_ratio();
		glScalef(scl, scl, scl);
		glTranslatef(0,0,-0.5);

		float pos[4];
		pos[0] = 0;
		pos[1] = 0;
		pos[2] = -1;
		pos[3] = 0.0f;
		glLightfv( GL_LIGHT2, GL_POSITION, pos );

		float sz = 0.105;
		glScalef(sz, sz, sz);

		// Title
		glDisable(GL_LIGHTING);
		glColor3f(0.8,0.8,0.8);
		glPushMatrix();
			glTranslatef(0,3.3,0);
			draw_text_centering(get_string("3DCHART_STARS_TITLE"));
		glPopMatrix();



		glTranslatef(-N-1.2,0,0);

		float mlv = max_level( blackbody_color(5800) );
		for (int i=0; i<N; i++) {
			RGBf rgb = blackbody_color( Temp[i] );

			rgb = scale_by_max(rgb, true);

			RGBf rgbE = correct_RGB0(0.8f * rgb);
			RGBf rgbD = correct_RGB0(0.4f * rgb);

			mat_a[0] = rgbE.r;
			mat_a[1] = rgbE.g;
			mat_a[2] = rgbE.b;
			mat_a[3] = 1.0;
			glMaterialfv( GL_FRONT_AND_BACK , GL_EMISSION,   mat_a );

			mat_d[0] = rgbD.r;
			mat_d[1] = rgbD.g;
			mat_d[2] = rgbD.b;
			mat_d[3] = 1.0;
			glMaterialfv( GL_FRONT_AND_BACK , GL_DIFFUSE,   mat_d );


			// 正面から照明を当てる
			glPushMatrix();
				toward_eye_pos();
				float pos[4];
				pos[0] = 0;
				pos[1] = 0;
				pos[2] = -1;
				pos[3] = 0.0f;
				glLightfv( GL_LIGHT2, GL_POSITION, pos );
			glPopMatrix();


			glEnable(GL_LIGHTING);
			gl_draw_LOD_sphere();

			glDisable(GL_LIGHTING);
			glColor3f(0.8,0.8,0.8);

			// 温度
			glPushMatrix();
				glTranslatef(0,1.5,0);
				glScalef(0.5,0.5,0.5);
				str = to_string(int(Temp[i])) + " " + get_string("3DCHART_STARS_DEG");
				draw_text_centering(str);
			glPopMatrix();

			// 星の名前
			glPushMatrix();
				glTranslatef(0,-1.6,0);
				glScalef(0.4,0.4,0.4);
				draw_text_centering(get_string(starnames_key[i]));
			glPopMatrix();
			
			glTranslatef(3,0,0);
		}
	glPopMatrix();

	mat_a[0] = 0;
	mat_a[1] = 0;
	mat_a[2] = 0;
	mat_a[3] = 1.0;
	glMaterialfv( GL_FRONT_AND_BACK , GL_EMISSION,   mat_a );


	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT2);
	glEnable(GL_LIGHT0);
	glDisable(GL_NORMALIZE);
}







void
draw_3D_chart(int no)
{
	switch (no) {
		case _3DCHART_PLANETS:			draw_3DChart_planets();			break;
		case _3DCHART_DWARF_PLANETS:	draw_3DChart_TNOs();			break;
		case _3DCHART_STAR_COLOR:		draw_3DChart_star_color();		break;
		default:														break;
	}
}
