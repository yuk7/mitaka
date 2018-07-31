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
#include  "mtk_nearby_galaxies.h"
#include  "mitaka.h"
#include  "mtk_elliptical_galaxy.h"
#include  "mtk_objects.h"
#include  "mtk_label.h"
#include  "mtk_switches.h"
#include  "mtk_colors.h"
#include  "mtk_OpenGL.h"
#include  "mtk_gl_common.h"
#include  <vector>
#include  <map>

#include  "mtk_solar_system.h"
#include  "mtk_our_galaxy.h"
#include  "mtk_calc_alpha.h"
#include  "mtk_draw_common.h"

#include  "mtk_strings.h"
#include  "mtk_make_info.h"
#include  "mtk_data_file.h"
#include  "mtk_selection.h"






//------------
//  Typedefs
//------------

//--  Draw Type
enum {
	GALAXY_DRAW_TYPE_NORMAL,
	GALAXY_DRAW_TYPE_CD
};

//--  Texture Type
enum {
	GALAXY_IMG_UNDETERMINED,
	GALAXY_IMG_ELLIPTICAL,
	GALAXY_IMG_S0,
	GALAXY_IMG_SPIRAL,
	GALAXY_IMG_IRREGULAR,
	GALAXY_IMG_LMC,
	GALAXY_IMG_SMC,
	GALAXY_IMG_M31
};





//-----------
//  Objects
//-----------
const int NBG_DATA_NUM = 10;
//--  Nearby Galaxies
static vector<nearby_galaxy2>	NGLX;
//--  Ref. tables of nearby galaxy catalogue no and index
static std::map<int, int>		NGC_table, UGC_table, PGC_table, Tully_table, undefined_table;



//--------------------
//  Color / Textures
//--------------------
const float galaxy_B_V[5] = { 0.0, 0.96, 0.85, 0.57, 0.27 };
static RGBf		galaxy_color[5];
static GLuint	texGalaxy;
static GLuint	dlGalaxy;




//---------------
//  Load / Init
//---------------

//--  Init galaxy color
void
init_galaxy_color()
{
	for (int i=0; i<5; ++i) {
        float Tc = B_V_to_Tc( galaxy_B_V[i] );
		RGBf rgb = blackbody_color(Tc);
		galaxy_color[i] = correct_RGB( rgb );
	}
}





int
read_a_nearby_galaxy_record(FILE* fp, nearby_galaxy2& gl)
{
	key_data  kd;

	kd = read_a_key_data(fp);
	if (kd.key=="") {
		return -1;
	}

	if (kd.key.size()==0 || kd.data.size() != NBG_DATA_NUM) {
		// Error or comment
		return 0;
	}


	gl.key = kd.key;

	gl.cat_type = kd.data[0][0];
	gl.cat_no = stoi(kd.data[1]);

	gl.eqPos.RAdeg = stof(kd.data[2]);
	gl.eqPos.DEdeg = stof(kd.data[3]);
	gl.distance = stof(kd.data[4]);
	gl.D = stof(kd.data[5]);
	gl.d_D = stof(kd.data[6]);
	gl.mtype = stoi(kd.data[7]);
	gl.bBar  = (stoi(kd.data[8]) != 0);
	gl.group = stoi(kd.data[9]);


	//---  銀河の表示タイプ
	char tex_idx;
	if (gl.mtype <= -3)						tex_idx = GALAXY_IMG_ELLIPTICAL;
	if (gl.mtype >= -2 && gl.mtype <= 0)	tex_idx = GALAXY_IMG_S0;
	if (gl.mtype >= 1 && gl.mtype <= 9)		tex_idx = GALAXY_IMG_SPIRAL;
	if (gl.mtype >= 10 && gl.mtype <= 13)	tex_idx = GALAXY_IMG_IRREGULAR;
	if (gl.mtype >= 14)						tex_idx = GALAXY_IMG_UNDETERMINED;

	if (gl.cat_type == 'N') {
		switch (gl.cat_no) {
		case 224:	tex_idx = GALAXY_IMG_M31;	break;		// M31
		default:							break;
		}
	}
	if (gl.cat_type == 'T') {
		switch (gl.cat_no) {
		case 445:	tex_idx = GALAXY_IMG_LMC;	break;		// LMC
		case 58:	tex_idx = GALAXY_IMG_SMC;	break;		// SMC
		default:							break;
		}
	}

	gl.tex_idx = tex_idx;




	//--- 半径 (in pc)
	gl.R = 0.5 * (pi / 180.0 * 1.0 / 60.0) * gl.D * gl.distance;

	//--- 天球面上の位置（単位半径）
	vector3f Cxyz = gl.eqPos.get_dirc_vec();

	//--- 日心黄道座標
	gl.xyz = M_R2E * (gl.distance * Cxyz);

	//---  Draw type
	if (gl.cat_type == 'N' && gl.cat_no == 4486) {
		gl.dtype = GALAXY_DRAW_TYPE_CD;			// M87
	}
	else {
		gl.dtype = GALAXY_DRAW_TYPE_NORMAL;
	}

	return  1;
}



bool
read_nearby_galaxy_data(FILE* fp)
{
	int  res;
	nearby_galaxy2 gl;
	mtk_object  obj;

	while (true) {


		res = read_a_nearby_galaxy_record(fp, gl);

		if (res == -1) {
			break;
		}
		if (res == 0) {
			continue;
		}


		int idx = NGLX.size();
		int cat_no = gl.cat_no;


		char obj_key[256];
		switch (gl.cat_type) {
		case 'N':	//  NGC
			NGC_table.insert(pair<int, int>(cat_no, idx));
			sprintf(obj_key, "NBG_NGC_%d", cat_no);
			break;
		case 'U':	//  UGC
			UGC_table.insert(pair<int, int>(cat_no, idx));
			sprintf(obj_key, "NBG_UGC_%d", cat_no);
			break;
		case 'P':	// PGC
			PGC_table.insert(pair<int, int>(cat_no, idx));
			sprintf(obj_key, "NBG_PGC_%d", cat_no);
			break;
		case 'T':	// Tully
			Tully_table.insert(pair<int, int>(cat_no, idx));
			sprintf(obj_key, "NBG_TULLY_%d", cat_no);
			break;
		case '*':	// Undefined
			undefined_table.insert(pair<int, int>(cat_no, idx));
			sprintf(obj_key, "NBG_UNDEF_%d", idx);
			break;
		default:
			break;
		}

		//--  Register a galaxy
		obj.init(OBJ_TYPE_NEARBY_GALAXY, idx, 0.0f, true);
		int obj_idx = register_object(obj, obj_key);
		gl.obj_idx = obj_idx;


		//---  Register label
		if (is_defined_key(gl.key)) {
			add_label(gl.key, obj_idx, MTK_COLOR_NAME_OBJECT, 1.0f, get_object_name_size(), 1.4 * gl.R,  8 * D2R, true);
		}


		NGLX.push_back(gl);
	}

	return  true;
}










bool
load_nearby_galaxies(const directory& dir)
{
	//---  Init color
	init_galaxy_color();

	//---  Load data
	FILE* fp = dir.fopen("NBG.dat", "rb");
	if (fp == NULL)  return false;
	bool res = read_nearby_galaxy_data(fp);
	fclose(fp);
	if ( !res ) return  false;

	return  res;
}



//------------
//  Textures
//------------
void
load_galaxies_textures()
{
	texGalaxy = LoadTexture("galaxy_class.png", TexShrinkRate, false);
}



//-------------------------------------------
//  近傍銀河のイメージ用ディスプレイリストの初期化
//-------------------------------------------
void
init_galaxy_display_list()
{
	dlGalaxy = glGenLists(8);
	for (int i=0; i<8; ++i) {
		float du = 1.0f/8.0f;
		float u1 = du*i;
		float u2 = du*(i+1);
		
		glNewList(dlGalaxy+i, GL_COMPILE);
			glBegin(GL_QUADS);
				glTexCoord2f(u2,1);		glVertex3f(1,1,0);
				glTexCoord2f(u1,1);		glVertex3f(-1,1,0);
				glTexCoord2f(u1,0);		glVertex3f(-1,-1,0);
				glTexCoord2f(u2,0);		glVertex3f(1,-1,0);
			glEnd();
		glEndList();
	}
}



//
//
//
nearby_galaxy2&
get_nearby_galaxy(int idx)
{
	return  NGLX[idx];
}


//------------
//  Position
//------------
vector3d
get_nearby_galaxy_position(int idx)
{
	return  NGLX[idx].xyz;
}



//----------
//  Radius
//----------
double
get_nearby_galaxy_radius(int idx)
{
	return  NGLX[idx].R;
}



//---------
//  Scale
//---------
double
get_scale_nearby_galaxy(int idx)
{
	return  30.0 * get_nearby_galaxy_radius(idx);
}



//---------
//  Label
//---------
const double nbg_lc1 = 10000;
const double nbg_lc2 = 30000;


bool
is_nearby_galaxy_label_visible(const mtk_draw_struct& ds, int idx, bool bTgt)
{
	const nearby_galaxy2& gl = NGLX[idx];
	if ( !(swGalaxy & GLXY_NEARBY_GALAXY) )  return false;
	if ( !test_disp_switch(DISPSW_PLANET, DISPM_NAME) )  return false;
	if (bTgt) return true;

	if ( ds.cnorm < nbg_lc1 )  return  false;
	if ((swStar & STAR_NAME_SELECT) && !is_selected(gl.key))  return  false;

	return  true;
}


float
get_nearby_galaxy_label_alpha(const mtk_draw_struct& ds, int idx)
{
	const double lc = ds.cnorm;
	if (lc < nbg_lc2) {
		return  (lc - nbg_lc1) / (nbg_lc2 - nbg_lc1);
	}

	//****
	const mtk_object& tgt = get_object(ds.tgt_ID);
	if ( tgt.get_type() == OBJ_TYPE_NEARBY_GALAXY &&  tgt.get_index() == idx) {
		//  銀河系ラベルと同じ
		const float scl1 = 1e8;
		const float scl2 = 1e9;
		const float scale = ds.scale;
		if (scale > scl2)  return 0.0f;
		if (scale > scl1) {
			return  1 - (scale - scl1)/(scl2 - scl1);
		} else {
			return 1.0f;
		}
	}
	//****

	const vector3d   pos = get_nearby_galaxy_position(idx);
	const vector3d  cpos = ds.camera.get_pos();
	const vector3d  rpos = pos - cpos;
	const double l = rpos.norm();

	const vector3d  gpos = pos - get_our_galaxy_position();
	float  Rv  = 0.5f * gpos.norm();
	float  Dth = 2.0f * Rv / l;
	float af1, af2;
	af1 = calc_alpha_by_apparent_diameter( Dth );
	if (af1 < 0.01f)  return 0.0f;

	const vector3d  tpos = pos - ds.tgt_pos;
	Rv = 0.5f * tpos.norm();
	Dth = 2.0f * Rv / l;
	af2 = calc_alpha_by_apparent_diameter( Dth );
	if (af2 < 0.01f)  return 0.0f;

	return  (af1 < af2)  ?  af1  :  af2;
}



//------------------
//  Landing radius
//------------------
double
get_landing_radius_nearby_galaxy(int idx)
{
	return  0.3 * get_nearby_galaxy_radius(idx);
}



//----------------
//  Prepare draw
//----------------
bool
prepare_draw_nearby_galaxy(const mtk_draw_struct& ds, int idx, double* pRb, double* pRh, float* pAlp)
{
	//---  Visible
	if (! (swGalaxy & GLXY_NEARBY_GALAXY) )  return false;



	const double  scale = ds.scale;
	if (scale > 2e11)	return false;

	const double cnorm = ds.cnorm;
	float af0 = 1.0f;
	if (cnorm < 3e4) {
		af0 = 1 - (3e4 - cnorm)/(3e4 - 1e4);
	}
	if (af0 < 0.01)		return false;


	//---  Radius
	*pRb = *pRh = 0.3 * get_nearby_galaxy_radius(idx);
	*pAlp = af0;


	return  true;
}



//--------
//  Draw
//--------
void
draw_a_nearby_galaxy(const mtk_draw_struct& ds, int idx, const vector3d& rpos, const double& scl)
{
	const float screen_z = get_screen_z();
	const float scale = ds.scale;
	const double cnorm = ds.cnorm;
	
	//*********
	float af0 = 1.0f;

	if (cnorm > 1e9) {
		af0 = 1e9/cnorm;
	} else if (cnorm < 3e4) {
		af0 = 1 - (3e4 - cnorm)/(3e4 - 1e4);
	}
	if (af0 < 0.01)		return;


	if (scale > 2e11)  return;
	//**********


	const vector3d target_pos = ds.tgt_pos;
	const vector3d camera_pos = ds.camera.get_pos();
	const frustum  frs = ds.get_frustum();
	const double target_dist_sqr = norm2(target_pos - camera_pos);
	const float Rmax = 0.5*get_Rmax();
	const float Rth  = 0.5*Rmax;
	const nearby_galaxy2& gl = NGLX[idx];


	//-- draw
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glBindTexture(GL_TEXTURE_2D, texGalaxy);


	const float R_gal = gl.R;		// 銀河の半径
	float sz = R_gal * ds.scale_factor;

	float af = af0;
	const float dist = rpos.norm();
	if (dist > Rth) {
		af = 1 - (dist-Rth)/(Rmax-Rth);
	}
	if (af < 0.01) return;

	glPushMatrix();
		//--  Go to the center of the galaxy
		glTranslatef(rpos.x, rpos.y, rpos.z);

		float M[16];
		glGetFloatv( GL_MODELVIEW_MATRIX, M);
		float r = sqrt( M[12]*M[12] + M[13]*M[13] + M[14]*M[14] );
		float sz2 = R_gal/scale /r*screen_z * 2000;
		if (sz2 < 0.5) sz=2;

		if (sz2 >= 0.5) {
			switch (gl.dtype) {
				case GALAXY_DRAW_TYPE_CD:
					draw_elliptical_galaxy(ds, rpos);
					break;
				default:
					glBlendFunc(GL_ONE, GL_ONE);

					glEnable(GL_TEXTURE_2D);
					if (bVirgoCluster && gl.group==11)	{ glColor4f(0.2,1,0.4,af); }
					else								{ glColor4f(af,af,af,af); }

					toward_eye_pos();
					glScalef(sz,sz,1);
					glCallList(dlGalaxy + gl.tex_idx);
					glDisable(GL_TEXTURE_2D);
					break;
			}
		}
	glPopMatrix();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_TEXTURE_2D);
	glDepthMask(GL_TRUE);
}


void
draw_a_nearby_galaxy_as_point(const mtk_draw_struct& ds, int idx, const vector3d& rpos, float alpha)
{
	vector3f  rr = rpos;
	float R2;
	R2 = norm2(rr);
	if (R2 < 1e-100) return;


	const nearby_galaxy2& gl = NGLX[idx];


	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	float fac;
	if (bVirgoCluster && gl.group == 11)	{ glColor4f(0.2, 1.0, 0.4, alpha); fac = 1.167; }	// For Virgo cluster
	else									{ glColor4f(0.9, 0.9, 0.9, alpha); fac = 1.0; }


	if (true) {
		// QUAD
		vector3f eR, eU;
		if (ds.bRotateBillboard) {
			eR = gl.eR;
			eU = gl.eU;
		}
		else {
			eR = ds.eRf;
			eU = ds.eUf;
		}

		draw_point(ds, 0.0012, float(sqrt(R2)), rpos, eR, eU);
	} else {
		const float ptsz_scl = 1 / (ds.rad_per_pixel / 0.001);
		glPointSize(3.0*fac*ptsz_scl);

		glBegin(GL_POINTS);
			glVertex3dv( rpos.get_pointer() );
		glEnd();
	}
}




//--------
//  Info
//--------
void
make_object_info_nearby_galaxy(const mtk_draw_struct& ds, int idx, std::vector<std::string>& text)
{
	const nearby_galaxy2& gl = NGLX[idx];


	//---  Designation
	std::string dsg;
	switch (gl.cat_type) {
	case 'N':	dsg = "NGC";	break;
	case 'U':	dsg = "UGC";	break;
	case 'P':	dsg = "PGC";	break;
		//case 'T':	str = "Tully";	break;
	default:					break;
	}

	if (dsg != "") {
		dsg = dsg + " " + to_string(gl.cat_no);
	}


	//---  Name  ---
	bool bName = false;
	if (is_defined_key(gl.key)) {
		text.push_back(get_string(gl.key));
		bName = true;
	}

	if (dsg != "") {
		text.push_back(dsg);
		bName = true;
	}

	if (!bName) {
		text.push_back(get_string("INFO_TYPE_GALAXY"));
	}


	//---  Type  ---
	add_info_line(text, "INFO_TYPE", "INFO_TYPE_GALAXY");

	//---  Galaxy type  ---
	const char dsg2[][10] = {
		"a", "ab", "b", "bc", "c", "cd", "d", "dm", "m"
	};


	std::string type_key;
	if (gl.mtype <= -5) {
		type_key = "INFO_GALAXY_TYPE_E";
		dsg = "E";
	}
	else if (gl.mtype <= -3) {
		type_key = "INFO_GALAXY_TYPE_E_S0";
		dsg = "E/S0";
	}
	else if (gl.mtype <= -1) {
		type_key = "INFO_GALAXY_TYPE_S0";
		dsg = "S0";
	}
	else if (gl.mtype == 0) {
		type_key = "INFO_GALAXY_TYPE_S0_A";
		dsg = "S0/a";
	}
	else if (gl.mtype <= 8) {
		int dsg2_idx = gl.mtype - 1;
		if (gl.bBar) {
			type_key = "INFO_GALAXY_TYPE_SB";
			dsg = "SB" + std::string(dsg2[dsg2_idx]);
		}
		else {
			type_key = "INFO_GALAXY_TYPE_S";
			dsg = "S" + std::string(dsg2[dsg2_idx]);
		}
	}
	else if (gl.mtype == 9) {
		type_key = "INFO_GALAXY_TYPE_SI";
		dsg = "Sm";
	}
	else if (gl.mtype <= 11) {
		type_key = "INFO_GALAXY_TYPE_IR";
		dsg = "Ir";
	}
	else if (gl.mtype == 12) {
		type_key = "INFO_GALAXY_TYPE_SI";
		dsg = "S";
	}
	else if (gl.mtype == 13) {
		type_key = "INFO_GALAXY_TYPE_P";
		dsg = "P";
	}
	else {
		type_key = "INFO_GALAXY_TYPE_UNDETERMINED";
		dsg = "";
	}

	std::string str;
	str = get_string(type_key);
	if (dsg != "") {
		str = str + " (" + dsg + ")";
	}

	add_info_line_str(text, "INFO_GALAXY_TYPE", str.c_str());

}








//----------
//  Number
//----------
int
get_number_of_nearby_galaxies()
{
	return  NGLX.size();
}
