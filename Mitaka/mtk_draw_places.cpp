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
#include  "mtk_draw_places.h"
#include  "../common/graphics/gl_common.h"
#include  "mtk_gl_common.h"
#include  "mtk_colors.h"
#include  "mtk_strings.h"
#include  "mtk_font.h"


const float D2Rf = M_PI/180.0f;



float LocationNameSize;



void
prepare_3Dtext_place_name(const vector3f& rc, const vector3f& rp, float w, const float sz, const float screen_w, const float screen_z, bool bHorizontalMode)
{
	double M[16];

	// カメラ座標系での位置座標を取得
	glGetDoublev(GL_MODELVIEW_MATRIX, M);

	vector3f dr = rp - rc;
	vector3f eF, eU, eR;

	quaternionf ori1, ori2, ori;

	// 遠い場合
	eF = normalize(-dr);
	eU = get_eU_of_base_on_local_camera();	// カメラ座標系での基準座標系の軸
	eR = normalize(cross_product(eU, -eF));
	eU = normalize(cross_product(-eF, eR));
	ori1 = axes_to_quaternion(eF, eU);

	if (bHorizontalMode) {
		// 近い場合
		eF = normalize(rp);
		eU = normalize(dr);
		
		vector3f _eR = cross_product(eU, -eF);
		float nrm = _eR.norm();
		const float c1 = 0.2;
		const float c2 = 0.4;
		if (nrm < c1) {
			w = 1;
		} else if (nrm < c2) {
			w = 1 - (1-w) * ((nrm - c1) / (c2 - c1));
		}

		eR = normalize(_eR);
		eU = normalize(cross_product(-eF, eR));
		ori2 = axes_to_quaternion(eF, eU);

		// 球面線形補間
		ori = slerp(ori2, ori1, w);
	}
	else {
		ori = ori1;
	}

	quaternion_to_axes(ori, eF, eU, eR);
	M[0] = eR.x;
	M[1] = eR.y;
	M[2] = eR.z;
	M[3] = 0;
	M[4] = eU.x;
	M[5] = eU.y;
	M[6] = eU.z;
	M[7] = 0;
	M[8] = -eF.x;
	M[9] = -eF.y;
	M[10] = -eF.z;
	M[11] = 0;
	glLoadMatrixd(M);


	// サイズの分だけずらす
	float fac0 = rp.norm() / screen_z;
	float scl2 = sz*fac0 * screen_w / 180.0;
	glScalef(scl2, scl2, scl2);

}




vector3f
get_pos_from_lon_lat(float lon, float lat)
{
	float th, phi, x, y, z, rho;
	th = D2Rf * (90.0f - lat);
	phi = D2Rf * lon;
	z = cos(th);
	rho = sin(th);
	x = rho * cos(phi);
	y = rho * sin(phi);
	return  vector3f(x, y, z);
}



void
draw_places_on_TopoSphere_sub(const Places &places, const TopoSphere* tp, float screen_w, float screen_z, float ltf, bool bHorizontalMode)
{
	const float thr1    = 1.5 * D2Rf;
	const float thr2    = 2.5 * D2Rf;
	const float thr1sqr = thr1*thr1;
	const float thr2sqr = thr2*thr2;
	const double fac = 0.0005;   //***
	const float R0 = (tp != 0) ? tp->get_R0() : 1.0f;

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);


	float M[16];
	vector3f  rc;
	glGetFloatv( GL_MODELVIEW_MATRIX, M);
	rc.x = M[12];
	rc.y = M[13];
	rc.z = M[14];

	//rc -= get_base_origin_on_local_camera();	// 視点のずらしをもどす

	const float R2 = M[0]*M[0] + M[1]*M[1] + M[2]*M[2];


	// 線形補間の重み
	float w;
	//w = 3 * (rc.norm() - sqrt(R2)) / rc.norm();
	w = 3.5 * (rc.norm() - sqrt(R2)) / rc.norm();
	if (w > 1) { w = 1; }
	if (w < 0) { w = 0; }

	float ww = 0.3;
	w = (w - ww) / (1 - ww);
	if (w < 0) { w = 0; }




	for (int i=0; i<places.size(); i++) {
		const PlaceItem&  pli = places[i];


		vector3f  r = (tp != 0) ? tp->get_pos(pli.lon, pli.lat) : get_pos_from_lon_lat(pli.lon, pli.lat);
		r *= 1.002f;


		glPushMatrix();
			glPushMatrix();
				glTranslatef(r.x, r.y, r.z);
				glGetFloatv( GL_MODELVIEW_MATRIX, M);
			glPopMatrix();
			
			vector3f rp(M[12], M[13], M[14]);
			rp -= get_base_origin_on_local_camera();	// 視点のずらしをもどす

			const float L2 = rp.norm2();
			float th1_2 = R2 * pli.R1*pli.R1 / L2;

			if (th1_2 > thr1sqr) {
				float af = (th1_2 - thr1sqr) / (thr2sqr - thr1sqr);
				if (af > 1.0f)  af = 1.0f;


				// 近すぎる場合フェードアウトする
				float w2 = L2 / R2;
				if (w2 < 0.01) {
					af *= w2/(0.01*0.01);
				}


				if (af > 0.05) {

					//-- 名前
					if (pli.bDispLabel) {
						r *= (1+fac);
						glTranslatef(r.x, r.y, r.z);


						/**** 文字の縁取りに関係した処理 *****/
						/*
						if (fnt.get_polygon_mode()) {
							const RGBAf& col = pli.col1;
							glColor4f(col.rgb.r, col.rgb.g, col.rgb.b, col.a*af);
						}
						else {
							const RGBAf& col = pli.col2;
							glColor4f(col.rgb.r, col.rgb.g, col.rgb.b, col.a*af);
						}
						*/

						//**** 仮に col1 だけで書く
						const RGBAf& col = pli.col1;
						glColor4f(col.rgb.r, col.rgb.g, col.rgb.b, col.a*af);


						float fnt_sz = LocationNameSize * pli.size * ltf / 1.2;
						prepare_3Dtext_place_name(rc, rp, w, fnt_sz, screen_w, screen_z, bHorizontalMode);


						const std::string& str = get_string(pli.key);


						switch (pli.align) {
						case 'L':	draw_text_right(str);						break;
						case 'R':
						default:	glTranslatef(0.2, 0, 0);	draw_text(str);	break;
						}
					}
				}
			}
		glPopMatrix();
	}
}





void
draw_places_on_TopoSphere(const Places& places, const TopoSphere* tp, float screen_w, float screen_z, float ltf, bool bHorizontalMode)
{
	draw_places_on_TopoSphere_sub(places, tp, screen_w, screen_z, ltf, bHorizontalMode);
	/*
	// 縁取りのため、複数回に分けて描画
	fnt.set_polygon_mode(true);
	draw_places_on_TopoSphere_sub(places, tp, screen_w, screen_z, ltf, bHorizontalMode);
	fnt.set_polygon_mode(false);
	draw_places_on_TopoSphere_sub(places, tp, screen_w, screen_z, ltf, bHorizontalMode);
	fnt.set_polygon_mode(true);
	*/
}







//--------------
//  地表ポインタ
//--------------
void
draw_pointer_on_sphere(float screen_w, float screen_z, float lon, float lat, float ltf)
{
	const float thR = tan(20.0f * D2Rf);		// ポインタを表示する時の球の視直径
	const float thR2 = thR * thR;

	// 距離の判定
	GLfloat MM[16];
	glGetFloatv( GL_MODELVIEW_MATRIX,  MM);

	const float L2  = MM[12]*MM[12] + MM[13]*MM[13] + MM[14]*MM[14];	// 中心までの距離
	const float R2  = MM[0] *MM[0]  + MM[1] *MM[1]  + MM[2] *MM[2];		// 半径
		
	//-- 球の中の場合
	float L02 = L2 - R2;
	if (L02 <= 0.0f)	{ return; }

	//--- 遠い場合は、球を表示
	if (R2/L2 < thR2)	{ return; }


	glPushMatrix();
		float af = (R2/L2)/thR2 - 1.0f;
		if (af > 1.0f) af = 1.0f;

		vector3f  r0(MM[12], MM[13], MM[14]);

		const double th  = D2R * (90.0 - lat);
		const double phi = D2R * lon;
		const double rho = sin(th);
		vector3f  pos(rho*cos(phi), rho*sin(phi), cos(th));
		//pos *= 1.004;

		glTranslatef(pos.x, pos.y, pos.z);

		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_DEPTH_TEST);

		// クロスマークの描画
		prepare_3Dtext_2(r0, 0, 4.0*ltf/1.2, screen_w, screen_z, false, true);

		glColor4f(0,1,0,af);
		glLineWidth(2.0f);
		const float d = 0.3;
		glBegin(GL_LINES);
			glVertex3f(-d,0,0);
			glVertex3f( d,0,0);
			glVertex3f(0,-d,0);
			glVertex3f(0, d,0);
		glEnd();

		// 経度・緯度を表示
		char szText[100];
		char EW = (lon >= 0.0f)  ?  'E' : 'W';
		char NS = (lat >= 0.0f)  ?  'N' : 'S';
		
		sprintf(szText, "%3.1f%c, %3.1f%c", fabs(lon), EW, fabs(lat), NS);
		glPushMatrix();
			glTranslatef(1,0,0);
			draw_text(szText);
		glPopMatrix();

		glPopAttrib();

	glPopMatrix();

}



void
draw_pointer_on_TopoSphere(const TopoSphere* tp, float screen_w, float screen_z, float lon, float lat, float ltf)
{
	if (tp==0 || !tp->is_loaded()) {
		glPushMatrix();
			draw_pointer_on_sphere(screen_w, screen_z, lon, lat, ltf);
		glPopMatrix();
		return;
	}


	const float thR = tan(20.0f * D2Rf);		// ポインタを表示する時の球の視直径
	const float thR2 = thR * thR;

	//判定
	GLfloat MM[16];
	glGetFloatv( GL_MODELVIEW_MATRIX,  MM);

	// 距離の判定
	const float L2  = MM[12]*MM[12] + MM[13]*MM[13] + MM[14]*MM[14];	// 中心までの距離
	const float R2  = MM[0] *MM[0]  + MM[1] *MM[1]  + MM[2] *MM[2];		// 半径
		
	//-- 球の中の場合
	float L02 = L2 - R2;
	if (L02 <= 0.0f)	return;

	//--- 遠い場合
	if (R2/L2 < thR2)	return;

	glPushMatrix();
		float af = (R2/L2)/thR2 - 1.0f;
		if (af > 1.0f) af = 1.0f;

		vector3f  r0(MM[12], MM[13], MM[14]);	// 球の中心の位置
		vector3f  pos = tp->get_pos(lon, lat);
		float elv = float(tp->get_elevation(lon, lat));

		glTranslatef(pos.x, pos.y, pos.z);

		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_DEPTH_TEST);

		// クロスマークの描画
		prepare_3Dtext_2(r0, 0, 4.0*ltf/1.2, screen_w, screen_z, false, true);

		glColor4f(0,1,0,af);
		glLineWidth(2.0f);
		const float d = 0.3;
		glBegin(GL_LINES);
			glVertex3f(-d,0,0);
			glVertex3f( d,0,0);
			glVertex3f(0,-d,0);
			glVertex3f(0, d,0);
		glEnd();

		// 経度・緯度を表示
		char szText[100];
		char EW = (lon >= 0.0f)  ?  'E' : 'W';
		char NS = (lat >= 0.0f)  ?  'N' : 'S';
		
		sprintf(szText, "%3.1f%c, %3.1f%c", fabs(lon), EW, fabs(lat), NS);
		glPushMatrix();
			glTranslatef(1,0,0);
			draw_text(szText);
		glPopMatrix();

		sprintf(szText, "%4.1fm", elv);
		glPushMatrix();
			glTranslatef(1,-1,0);
			draw_text(szText);
		glPopMatrix();
		glPopAttrib();
	glPopMatrix();
}




