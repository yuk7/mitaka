/**************************************************************************************************

Copyright (c) 2007,2015-2016   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#include  "mtk_mode.h"
#include  "mtk_galaxies.h"
#include  "mtk_switches.h"
#include  "mtk_font.h"
#include  "mtk_OpenGL.h"
#include  "mtk_gl_common.h"
#include  "mtk_colors.h"
#include  "mtk_solar_system.h"
#include  "mtk_elliptical_galaxy.h"
#include  "../common/astro/astrometry.h"
#include  "../common/graphics/bounding_box.h"
#include  "../common/graphics/octree.h"
#include  <math.h>
#include  <vector>
#include  <map>
#include  <algorithm>

#include  "mtk_point_block.h"








//------------
//  gal_data
//------------
struct gal_data {
	int icls;
	vector3f pos;
};





//-----------
//  Objects
//-----------
const int octree_lv = 4;
const int class_N_gal = 5;
const int class_N_qso = 5;
//const int class_N_gal = 10;
//const int class_N_qso = 20;

static int N_gal, N_qso;
static vector<point_block> vPB_gal[class_N_gal], vPB_qso[class_N_qso];
static octree<float>  octree_gal[class_N_gal], octree_qso[class_N_qso];

static std::vector<int> list_tmp;



//
static int SDSS_Thinning = 0;

void
SetDistantGalaxyThinning(int val)
{
	if (val < 0)  val = 0;
	if (val > 10) val = 10;
	SDSS_Thinning = val;
}






//--------------------
//  Distant galaxies
//--------------------
bool
load_distant_galaxies(const directory& dir)
{
	const float ecls_min = -0.35;
	const float ecls_max = 0.5;
	const float d_ecls = ecls_max - ecls_min;
	vector<gal_data> vGalData;


	FILE* fp = dir.fopen("SDSS_GAL.dat", "rb");
	if (fp == NULL)  return false;

	N_gal = 0;
	while (true) {
		RA_DEC eqPos;
		float dist;
		float ecls;

		int n = fread(&eqPos.RAdeg, sizeof(eqPos.RAdeg), 1, fp);
		if (n != 1)  break;
		fread(&eqPos.DEdeg, sizeof(eqPos.DEdeg), 1, fp);
		fread(&dist, sizeof(dist), 1, fp);
		fread(&ecls, sizeof(ecls), 1, fp);

		//-- 位置を求める
		vector3f Cxyz = eqPos.get_dirc_vec();		// 天球面上の位置（単位半径）
		vector3f pos = M_R2E * (dist * Cxyz);		// 日心黄道座標

		//-- クラスを決める
		int icls = int((ecls - ecls_min) / d_ecls * class_N_gal);
		if (icls < 0) icls = 0;
		if (icls >= class_N_gal)  icls = class_N_gal - 1;

		gal_data gd;
		gd.icls = icls;
		gd.pos = pos;
		vGalData.push_back(gd);

		N_gal++;
	}
	fclose(fp);


	// calc bounding box
	bbox3f bb[class_N_gal];
	for (int i = 0; i < vGalData.size(); i++) {
		const gal_data& gd = vGalData[i];

		bb[gd.icls].add_point(gd.pos);
	}


	// build octree
	int N = 1;
	for (int i = 0; i < octree_lv; i++) {
		N *= 2;
	}
	

	int Nmax = 0;
	for (int ic = 0; ic < class_N_gal; ic++) {
		point_blocks vBlks;

		vBlks.init(N, bb[ic]);
		for (int i = 0; i < vGalData.size(); i++) {
			const gal_data& gd = vGalData[i];
			if (gd.icls != ic)  continue;

			vBlks.add_point(0, gd.pos);
		}


		// set point blocks
		vPB_gal[ic].clear();
		const std::map<int, point_block>& blks = vBlks.get_blocks();
		for (std::map<int, point_block>::const_iterator it = blks.begin(); it != blks.end(); it++) {
			const point_block& blk = it->second;

			vPB_gal[ic].push_back(blk);
		}
		if (vPB_gal[ic].size() > Nmax) {
			Nmax = vPB_gal[ic].size();
		}


		// build octree
		octree_gal[ic].init(bb[ic]);
		for (int i = 0; i < vPB_gal[ic].size(); i++) {
			const point_block& blk = vPB_gal[ic][i];
			octree_gal[ic].add_object(i, blk.get_bounding_box());
		}
	}

	if (list_tmp.capacity() < Nmax) {
		list_tmp.reserve(Nmax);
	}


	return true;
}




//--------
//  QSOs
//--------
bool
load_QSOs(const directory& dir)
{
	const float z_min = 0.1;
	const float z_max = 1.75;
	const float dz = z_max - z_min;
	vector<gal_data> vQSOData;


	FILE* fp = dir.fopen("SDSS_QSO.dat", "rb");
	if (fp == NULL)  return false;


	N_qso = 0;
	while (true) {
		RA_DEC eqPos;
		float dist;
		float z;

		int n = fread(&eqPos.RAdeg, sizeof(eqPos.RAdeg),		1, fp);
		if (n != 1)  break;
		fread(&eqPos.DEdeg,	sizeof(eqPos.DEdeg),	1, fp);
		fread(&dist,		sizeof(dist),			1, fp);
		fread(&z,			sizeof(z),				1, fp);
		
		//-- 位置を求める
		vector3f Cxyz = eqPos.get_dirc_vec();		// 天球面上の位置（単位半径）
		vector3f pos  = M_R2E * ( dist * Cxyz );		// 日心黄道座標

		//-- クラスを決める
		int icls = int( (z - z_min) / dz * class_N_qso);
		if (icls < 0) icls = 0;
		if (icls >= class_N_qso)  icls = class_N_qso - 1;

		gal_data qso;
		qso.icls = icls;
		qso.pos = pos;
		vQSOData.push_back(qso);

		N_qso++;
	}
	fclose(fp);


	// calc bounding box
	bbox3f bb[class_N_qso];
	for (int i = 0; i < vQSOData.size(); i++) {
		const gal_data& qso = vQSOData[i];

		bb[qso.icls].add_point(qso.pos);
	}


	// build octree
	int N = 1;
	for (int i = 0; i < octree_lv; i++) {
		N *= 2;
	}


	int Nmax = 0;
	for (int ic = 0; ic < class_N_qso; ic++) {
		point_blocks vBlks;

		vBlks.init(N, bb[ic]);
		for (int i = 0; i < vQSOData.size(); i++) {
			const gal_data& qso = vQSOData[i];
			if (qso.icls != ic)  continue;

			vBlks.add_point(0, qso.pos);
		}


		// set point blocks
		vPB_qso[ic].clear();
		const std::map<int, point_block>& blks = vBlks.get_blocks();
		for (std::map<int, point_block>::const_iterator it = blks.begin(); it != blks.end(); it++) {
			const point_block& blk = it->second;

			vPB_qso[ic].push_back(blk);
		}
		if (vPB_qso[ic].size() > Nmax) {
			Nmax = vPB_qso[ic].size();
		}


		// build octree
		octree_qso[ic].init(bb[ic]);
		for (int i = 0; i < vPB_qso[ic].size(); i++) {
			const point_block& blk = vPB_qso[ic][i];
			octree_qso[ic].add_object(i, blk.get_bounding_box());
		}
	}


	if (list_tmp.capacity() < Nmax) {
		list_tmp.reserve(Nmax);
	}


	return true;
}



void
draw_galaxy_points(const std::vector<point_block>& vPB, const std::vector<int>& list, int skp)
{
	int count = 0;
	glBegin(GL_POINTS);
		for (int i = 0; i < list.size(); i++) {
			const point_block& blk = vPB[list[i]];
			const vector<vector3f>& vPos = blk.get_points();

			for (int j = 0; j < vPos.size(); j += skp) {
				glVertex3fv(vPos[j].get_pointer());
			}
		}
	glEnd();
}


void
draw_bbox(const bbox3f& bb)
{
	glBegin(GL_LINES);
		glVertex3f(bb.x1, bb.y1, bb.z1);
		glVertex3f(bb.x2, bb.y1, bb.z1);
		glVertex3f(bb.x1, bb.y2, bb.z1);
		glVertex3f(bb.x2, bb.y2, bb.z1);
		glVertex3f(bb.x1, bb.y1, bb.z2);
		glVertex3f(bb.x2, bb.y1, bb.z2);
		glVertex3f(bb.x1, bb.y2, bb.z2);
		glVertex3f(bb.x2, bb.y2, bb.z2);

		glVertex3f(bb.x1, bb.y1, bb.z1);
		glVertex3f(bb.x1, bb.y2, bb.z1);
		glVertex3f(bb.x2, bb.y1, bb.z1);
		glVertex3f(bb.x2, bb.y2, bb.z1);
		glVertex3f(bb.x1, bb.y1, bb.z2);
		glVertex3f(bb.x1, bb.y2, bb.z2);
		glVertex3f(bb.x2, bb.y1, bb.z2);
		glVertex3f(bb.x2, bb.y2, bb.z2);

		glVertex3f(bb.x1, bb.y1, bb.z1);
		glVertex3f(bb.x1, bb.y1, bb.z2);
		glVertex3f(bb.x2, bb.y1, bb.z1);
		glVertex3f(bb.x2, bb.y1, bb.z2);
		glVertex3f(bb.x1, bb.y2, bb.z1);
		glVertex3f(bb.x1, bb.y2, bb.z2);
		glVertex3f(bb.x2, bb.y2, bb.z1);
		glVertex3f(bb.x2, bb.y2, bb.z2);
	glEnd();
}


void
draw_distant_galaxies(const mtk_draw_struct& ds, float pt_sz)
{
	const float scale = ds.scale;
	if (ds.tyMode!=MODE_3D || scale < 1000)  return;
	const vector3d camera_pos = ds.camera.get_pos();


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);

	int skp = 1;
	float af = 1.0;
	pt_sz /= ds.rad_per_pixel / 0.001;
	if (pt_sz < 1) {
		af = pt_sz;
		pt_sz = 1;
	}
	glPointSize(pt_sz);

	// 近距離のフェード処理
	const float scl1 = 5.0e7;
	const float scl2 = 5.0e8;
	const float scl3 = 3.0e9;

	if (scale < scl1)  return;
	if (scale < scl2) {
		const float log_scl = log(scale);	
		const float log_scl1 = log(scl1);
		const float log_scl2 = log(scl2);
		af *= (log_scl - log_scl1) / (log_scl2-log_scl1);
	}

	// 遠距離のフェード処理
	if (scale > scl3) {
		float L = scale / scl3;

		af /= L*L;
	}
	af *= 0.7;
	if (af <= 0.00001)  return;
	while (af < 0.02) {
		af *= 2;
		skp *= 2;
	}

	// Thinning out
	for (int i=0; i<SDSS_Thinning; i++) {
		af *= 2;
		skp *= 2;
	}

	if (af > 1) {
		af = 1;
	}



	const vector3d* nv;
	const double *D;
	ds.get_frustum_planes_on_world(nv, D);

	vector3f nvf[6];
	float Df[6];
	for (int i = 0; i < 6; i++) {
		nvf[i] = nv[i];
		Df[i] = D[i];
	}


	glPushMatrix();
	glTranslated(-camera_pos.x, -camera_pos.y, -camera_pos.z);

	
	for (int ic=0; ic<class_N_gal; ic++) {

		list_tmp.clear();
		octree_gal[ic].make_draw_objects_list(nvf, Df, true, list_tmp);

		float t = ic/float(class_N_gal-1);
		float B_V = 2.2*(1-t) - 0.2;
		RGBf col = correct_RGB( 0.9f * blackbody_color( B_V_to_Tc( B_V ) ) );
		glColor4f( col.r, col.g, col.b, af );
		
		draw_galaxy_points(vPB_gal[ic], list_tmp, skp);
	}
	
	glPopMatrix();


	glEnable(GL_DEPTH_TEST);
	glPointSize(1);
}



void
draw_QSOs(const mtk_draw_struct& ds, float pt_sz)
{
	const float scale = ds.scale;
	if (ds.tyMode!=MODE_3D || scale < 1000)  return;
	const vector3d camera_pos = ds.camera.get_pos();

	//-- For culling
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);

	int skp = 1;
	float af = 1.0;
	pt_sz /= ds.rad_per_pixel / 0.001;
	if (pt_sz < 1) {
		af = pt_sz;
		pt_sz = 1;
	}
	glPointSize(pt_sz);

	// 近距離のフェード処理
	const float scl1 = 3.0e8;
	const float scl2 = 1.9e10;
	if (scale < scl1)  return;
	if (scale < scl2) {
		const float log_scl = log(scale);	
		const float log_scl1 = log(scl1);
		const float log_scl2 = log(scl2);
		af *= (log_scl - log_scl1) / (log_scl2-log_scl1);
	}

	// 遠距離のフェード処理
	if (scale > scl2) {
		float L = scale / scl2;
		af /= L*L;
	}
	af *= 0.7;
	if (af <= 0.00001)  return;
	while (af < 0.02) {
		af *= 2;
		skp *= 2;
	}

	// Thinning out
	for (int i=0; i<SDSS_Thinning; i++) {
		af *= 2;
		skp *= 2;
	}

	if (af > 1) {
		af = 1;
	}


	const vector3d* nv;
	const double *D;
	ds.get_frustum_planes_on_world(nv, D);

	vector3f nvf[6];
	float Df[6];
	for (int i = 0; i < 6; i++) {
		nvf[i] = nv[i];
		Df[i] = D[i];
	}



	glPushMatrix();
	glTranslated(-camera_pos.x, -camera_pos.y, -camera_pos.z);
	const RGBf col1 = get_color( MTK_COLOR_QSO );
	const RGBf col2 = get_color( MTK_COLOR_QSO );
	const float z_min = 0.1;
	const float z_max = 1.75;

	for (int ic=0; ic<class_N_qso; ic++) {

		list_tmp.clear();
		octree_qso[ic].make_draw_objects_list(nvf, Df, true, list_tmp);

		float t = ic/float(class_N_qso-1);
		RGBf col = (1-t)*col1 + t*col2;
		glColor4f( col.r, col.g, col.b, af );

		draw_galaxy_points(vPB_qso[ic], list_tmp, skp);
	}
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
	glPointSize(1);
}



void
draw_galaxies(const mtk_draw_struct& ds)
{
	if (swGalaxy & GLXY_DISTANT_GALAXY)		draw_distant_galaxies(ds, 1.7);
}

void
draw_QSOs(const mtk_draw_struct& ds)
{
	draw_QSOs(ds, 1.7);
}


//----------
//  データ数
//----------
int
get_number_of_distant_galaxies()
{
	return N_gal;
}

int
get_number_of_QSOs()
{
	return N_qso;
}
