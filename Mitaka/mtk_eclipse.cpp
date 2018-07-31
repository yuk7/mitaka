/**************************************************************************************************

Copyright (c) 2007, 2015-2016   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#include  <windows.h>
#include  "mtk_gl_common.h"
#include  "mtk_eclipse.h"
#include  "mtk_triangle_mesh.h"
#include  "mtk_mode.h"
#include  <vector>
using namespace std;


static eclipse_vertex g_vVtx[TRIANGLE_VTX_MAX];




vector3d
eclipse_get_shadow_center(const vector3d& r1, const double& R1, const vector3d& r2, const vector3d& rs)
{
	vector3d nv = normalize(r2 - rs);
	vector3d _r1 = r1 - r2;

	double b = -dot_product(nv, _r1);
	double c = norm2(_r1) - R1*R1;
	double D = b*b - c;

	if (D <= 0.0)  return vector3d(0,0,0);

	double t = -b - sqrt(D);


	return  t*nv + r2;
}

//********** *********
// r1, R1: 影を落とされる天体
// r2, R2: 影を落とす天体
// rs, Rs: 太陽
eclipse_info
calc_eclipse_info(const vector3d& r1, const double& R1, const vector3d& r2, const double& R2, const vector3d& rs, const double& Rs)
{
	eclipse_info  eci;
	eci.bFlg = false;

	// r2 よりも r1 の方が太陽に近い場合はスキップ
	if (norm2(r1 - rs) <= norm2(r2 - rs))  return eci;

	// r2を通る光線の方向
	vector3d nv = normalize(r2 - rs);
	double L2s = norm(r2 - rs);

	// 本影コーンの始点を求める
	double L2umb = (R2 * L2s) / (Rs - R2);
	vector3d r_umb = L2umb * nv + r2;
	double sinth_umb = R2 / L2umb;
	double costh_umb = sqrt(1.0 - sinth_umb*sinth_umb);

	// 半影コーンの始点rhを求める
	double LB = L2s / (1.0 + R2/Rs);
	double LA = L2s - LB;
	vector3d r_pen = LB * nv + rs;
	double sinth_pen = R2 / LA;
	double costh_pen = sqrt(1.0 - sinth_pen*sinth_pen);

	// r1 と最も近い半円コーン上の点との距離 l を求める
	double L1h_sqr = norm2( r1 - r_pen );
	double a = dot_product(r1-r_pen, nv);
	double l = sqrt(L1h_sqr - a*a)*costh_pen - a*sinth_pen;
	if (l > R1)  return eci;	// コーン領域の外側

	// z 軸を nv 方向とする座標系を作る
	vector3d n1, n2;
	n1 = get_a_normal_unit_vector(nv);
	n2 = cross_product(nv, n1);
	eci.bFlg = true;
	eci.r_umb = r_umb;
	eci.r_pen = r_pen;
	eci.nv = nv;
	eci.n1 = n1;
	eci.n2 = n2;
	eci.sinth_umb = sinth_umb;
	eci.costh_umb = costh_umb;
	eci.sinth_pen = sinth_pen;
	eci.costh_pen = costh_pen;

	return  eci;
}

// 太陽が隠されている割合（完全に隠されているときに１）
double
calc_eclipse_fraction(double R, double Dlt)
{
	if (R > 1.0) {
		// 隠す天体の視半径のほうが太陽の視半径よりも大きい場合
		if (Dlt < R - 1.0)	return  1.0;	// 本影
		if (Dlt > R + 1.0)	return  0.0;	// 範囲外
	} else {
		// 隠す天体の視半径のほうが太陽の視半径よりも小さい場合
		if (Dlt > R + 1.0)	return 0.0;		// 重ならない
		if (Dlt < 1.0 - R)	return R*R;		// 最大の掩蔽率
	}

	// 部分的に隠す（半影）の場合（R > 1, R < 1 で共通）
	double a, b, th1, th2;
	a = (Dlt*Dlt + R*R - 1.0) / (2.0 * Dlt);
	b = (Dlt*Dlt - R*R + 1.0) / (2.0 * Dlt);
	th1 = atan2( sqrt(R*R - a*a), a );
	th2 = atan2( sqrt(1.0 - b*b), b );

	return (R*R*th1 - a* sqrt(R*R - a*a) + th2 - b * sqrt(1.0 - b*b)) / M_PI;
}


// 地表に落ちた影用のポリゴンを作る
int
make_eclipse_shadow_polygon(const vector3d& r1, const double& R1, const vector3d& nz, const float f,
							const vector3d& r2, const double& R2,
							const vector3d& rs, const double& Rs,
							const int N1, const int N2, eclipse_vertex* vVtx)
{
	int vn = 0;
	clear_triangle_vtx();

	eclipse_info  eci;
	eci = calc_eclipse_info(r1, R1, r2, R2, rs, Rs);
	if (!eci.bFlg)  return 0;

	double tn_umb, tn_pen;
	vector3d r_umb, r_pen, nv, n1, n2, r1pen, _r1pen;

	tn_umb = eci.sinth_umb / eci.costh_umb; 
	tn_pen = eci.sinth_pen / eci.costh_pen;
	r_umb = eci.r_umb;
	r_pen = eci.r_pen;
	nv = eci.nv;
	n1 = eci.n1;
	n2 = eci.n2;
	r1pen = r_pen - r1;

	double alp = 1.0 / (1.0 - f);
	double az = (1.0 - alp);
	_r1pen = r1pen - az*dot_product(nz, r1pen) * nz;	// z 方向に縮小した半影コーンの始点の位置

	for (int i=0; i<N1; i++) {
		double tn;
		double u = float(i) / (N1-1);
		tn = tn_pen * (u*u);
		
		for (int j=0; j<N2; j++) {
			if (i==0 && j!=0)  break;

			double th_j = j*2.0*M_PI/N2 + 0.00001;
			double cs = cos(th_j);
			double sn = sin(th_j);
			double x, y;
			x = tn * cs;
			y = tn * sn;

			// コーン上の接線の方向ベクトル
			vector3d nt  = normalize( nv + x*n1 + y*n2 );
			vector3d _nt = nt - az*dot_product(nz, nt) * nz;

			// コーン上の接線と球との交点を求める
			double _a = norm2(_nt);
			double b = dot_product(_nt, _r1pen) / _a;
			double c = (norm2(_r1pen) - R1*R1) / _a;
			double D = b*b - c;
			if (D <= 0.0)  continue;
			double t = -b - sqrt(D);
			vector3d  pos = t*nt + r_pen;			// 黄道座標系での位置

			// 視半径
			double thm, ths, R;
			thm = asin( R2 / norm(pos - r2) );
			ths = asin( Rs / norm(pos - rs) );
			R = thm/ths;

			// 中心間の距離
			double Dlt;
			vector3d nm = normalize( r2 - pos );
			vector3d ns = normalize( rs - pos );
			Dlt = acos( dot_product(nm, ns) ) / ths;

			// 掩蔽率を計算する
			double f = calc_eclipse_fraction(R, Dlt);
			if (f <= 0.0)  continue;
			vVtx[vn].pos = pos;
			vVtx[vn].f   = f;
			add_triangle_xy(x, y);

			vn++;
		}
	}

	// z 軸を n1s 方向とする座標系を作る
	vector3d n1s, n1s1, n1s2;
	n1s  = normalize(rs - r1);
	n1s1 = get_a_normal_unit_vector(n1s);
	n1s2 = cross_product(n1s, n1s1);
	double L1s = norm(r1 - rs);
	double th_max = acos(R1 / L1s);

	const int N3 = 20;
	const int N4 = 3*N3;
	for (int i=0; i<N3; i++) {
		double u, th, rho, z;
		u = float(i) / (N3-1);
		th = th_max * u;
		rho = R1 * sin(th);
		z   = R1 * cos(th);

		for (int j=0; j<N4; j++) {
			if (i==0 && j!=0)  break;
			double th_j = j*2.0*M_PI/N4 + 0.00001;
			double cs = cos(th_j);
			double sn = sin(th_j);
			double x, y;
			x = rho * cs;
			y = rho * sn;
			vector3d  pos = x*n1s1 + y*n1s2 + z*n1s;

			// 自転軸方向を偏率に応じて縮小する
			pos = pos - f * dot_product(nz, pos) * nz;
			pos = pos + r1;

			// 視半径
			double thm, ths, R;
			thm = asin( R2 / norm(pos - r2) );
			ths = asin( Rs / norm(pos - rs) );
			R = thm/ths;

			// 中心間の距離
			double Dlt;
			vector3d nm = normalize( r2 - pos );
			vector3d ns = normalize( rs - pos );
			Dlt = acos( dot_product(nm, ns) ) / ths;

			// 掩蔽率を計算する
			double frac = calc_eclipse_fraction(R, Dlt);
			if (frac <= 0.0)  continue;
			vVtx[vn].pos = pos;
			vVtx[vn].f   = frac;

			// コーン座標に変換
			float _x, _y;
			vector3d _pos = pos - r_pen;
			_x = dot_product(_pos, n1) / dot_product(_pos, nv);
			_y = dot_product(_pos, n2) / dot_product(_pos, nv);
			add_triangle_xy(_x, _y);

			vn++;
		}
	}

	if (vn < 3) return 0;
	int nTri = triangulation();
	if (nTri <= 0)  return 0;

	return  nTri;
}



//--------------------------
//  惑星・衛星の影を描画
//--------------------------
// r1, R1: 影を落とされる天体
// r2, R2: 影を落とす天体
// rs, Rs: 太陽
//--------------------------
void
draw_eclipse_shadow(const mtk_draw_struct& ds,
					const vector3d& r1, const double& R1,
					const vector3d& nz, const float f,		// 影を落とされる天体の軸方向と偏率
					const vector3d& r2, const double& R2,
					const vector3d& rs, const double& Rs,
					const matrix3d& M,						// 黄道座標系から惑星座標系への変換行列
					const float th_c,						// 影を省略する角度(rad)
					const int N1, const int N2,				//
					bool bShadow, bool bBorder,
					const vector3f& shd_col,				// 本影中の光の色（月のみ暗い赤）
					const double& Rc_pl)					// プラネタリウムモード時の投影半径
{
	vector3d eF, eU, eR;
	quaterniond ori0 = ds.camera_base.get_ori() * get_base_camera_ori();
	quaternion_to_axes(ori0, eF, eU, eR);
	vector3d r_cam = get_local_camera_pos();
	const vector3d re = ds.camera.get_pos() + ( r_cam.x*eR + r_cam.y*eU - r_cam.z*eF)/ ds.camera.get_scale_factor();	// 視点の位置

	const double   R = R1;					// 惑星半径
	const double   L = norm(re - r1);		// 視点と惑星中心の距離
	if (R >= L)			return;

	const double th_view = asin(R / L);
	if (th_view < th_c)  return;

	eclipse_info  eci;
	eci = calc_eclipse_info(r1, R1, r2, R2, rs, Rs);
	if (!eci.bFlg)  return;

	double costh_umb, sinth_umb, costh_pen, sinth_pen;
	vector3d r_umb, r_pen, nv, n1, n2;
	
	r_umb = eci.r_umb;
	r_pen = eci.r_pen;
	costh_umb = eci.costh_umb;
	sinth_umb = eci.sinth_umb;
	costh_pen = eci.costh_pen;
	sinth_pen = eci.sinth_pen;
	nv = eci.nv;
	n1 = eci.n1;
	n2 = eci.n2;
	vector3d _r1  = r1 - r2;


	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glPushMatrix();
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	//---  影の表示  ---
	if (bShadow) {
		int nTri = make_eclipse_shadow_polygon(r1, R1, nz, f, r2, R2, rs, Rs, N1, N2, g_vVtx);
		if (nTri > 0) {
			Triangle* vTri = get_triangle_list();
			glEnable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glBlendFunc(GL_ZERO, GL_SRC_COLOR);

			float fR, fG, fB;
			fR = 1.0f - shd_col.r;
			fG = 1.0f - shd_col.g;
			fB = 1.0f - shd_col.b;
			
			glBegin(GL_TRIANGLES);
			for (int i=0; i<nTri; i++) {
				const Triangle& tri = vTri[i];
				for (int j=0; j<3; j++) {
					int idx = tri.p[2-j];

					vector3d pos3;
					if (ds.tyMode == MODE_3D) {
						vector3d  pos2 = (g_vVtx[idx].pos - r1) / R1;	// 影を落とされる天体の中心を原点とした位置
						pos3 = M * pos2;
					} else {
						pos3 = Rc_pl * (g_vVtx[idx].pos - ds.camera.get_pos()).normalized();
					}

					double alp = g_vVtx[idx].f;

					float Rs, Gs, Bs;
					Rs = 1.0f - fR*alp;
					Gs = 1.0f - fG*alp;
					Bs = 1.0f - fB*alp;



					glColor3f(Rs,Gs,Bs);
					glVertex3dv( pos3.get_pointer() );
				}
			}
			glEnd();
		}
	}

	//---  境界線の表示  ---
	if (bBorder) {
		const double fac = 1.004;
		const int Nb_fac = (R1 > R2)  ?  1  :  int(R2/R1);

		vector3d r1umb, _r1umb;
		vector3d r1pen, _r1pen;
		r1umb = r_umb - r1;
		r1pen = r_pen - r1;
		double alp = 1.0 / (1.0 - f);
		double az = (alp - 1.0);
		_r1umb = r1umb + az*dot_product(nz, r1umb) * nz;	// z 方向に縮小した本影コーンの始点の位置
		_r1pen = r1pen + az*dot_product(nz, r1pen) * nz;	// z 方向に縮小した半影コーンの始点の位置

		// 本影領域の境界線
		bool bTotalEclipse = (-dot_product(r1umb, nv) < R1);
		const int Nb1 = 100 * Nb_fac;
		const double c0 = norm2(_r1umb) - R1*R1;
		double tn_umb = sinth_umb / costh_umb;
		glColor3f(1,0.5,0);
		glLineWidth(1.5);

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);


		if (bTotalEclipse)	{ glBegin(GL_LINE_STRIP); }
		else				{ glBegin(GL_LINES); }
		for (int i=0; i<=Nb1; i++) {
			double cs = cos(i*2*3.14159/Nb1);
			double sn = sin(i*2*3.14159/Nb1);

			// コーン上の接線の方向ベクトル
			vector3d nt = normalize( -nv + tn_umb*cs*n1 + tn_umb*sn*n2 );

			// コーン上の接線と球との交点を求める
			vector3d _nt = nt + az*dot_product(nz, nt) * nz;
			double _a = norm2(_nt);
			double b = dot_product(_nt, _r1umb) / _a;
			double c = c0 / _a;
			double D = b*b - c;
			if (D <= 0.0)  continue;
			double t = -b + sqrt(D);

			vector3d  pos1 = t*nt + r_umb;				// 黄道座標系での位置

			vector3d pos3;
			if (ds.tyMode == MODE_3D) {
				vector3d  pos2 = fac * (pos1 - r1)/R1;		// 影を落とされる天体の中心を原点とした位置
				pos3 = M * pos2;
			}
			else {
				pos3 = Rc_pl * (pos1 - ds.camera.get_pos()).normalized();
			}




			glVertex3dv( pos3.get_pointer() );
		}
		glEnd();


		// 半影領域の境界線
		const int Nb2 = 200 * Nb_fac;
		double tn_pen = sinth_pen / costh_pen;
		glColor3f(1,0.5,0);
		glLineWidth(1.5);
		glBegin(GL_LINES);
		for (int i = 0; i <= Nb2; i++) {
			double cs = cos(i*2*3.14159/Nb2);
			double sn = sin(i*2*3.14159/Nb2);

			// コーン上の接線の方向ベクトル
			vector3d nt = normalize( nv + tn_pen*cs*n1 + tn_pen*sn*n2 );

			// コーン上の接線と球との交点を求める
			vector3d _nt = nt + az*dot_product(nz, nt) * nz;
			double _a = norm2(_nt);
			double b = dot_product(_nt, _r1pen) / _a;
			double c = (norm2(_r1pen) - R1*R1) / _a;
			double D = b*b - c;
			if (D <= 0.0)  continue;
			double t = -b - sqrt(D);
			vector3d  pos1 = t*nt + r_pen;				// 黄道座標系での位置

			vector3d pos3;
			if (ds.tyMode == MODE_3D) {
				vector3d  pos2 = fac * (pos1 - r1) / R1;		// 影を落とされる天体の中心を原点とした位置
				pos3 = M * pos2;
			}
			else {
				pos3 = Rc_pl * (pos1 - ds.camera.get_pos()).normalized();
			}

			glVertex3dv( pos3.get_pointer() );
		}
		glEnd();


		// 中心点
		{
			// コーン上の接線と球との交点を求める
			vector3d _nt = nv + az*dot_product(nz, nv) * nz;
			double _a = norm2(_nt);
			double b = dot_product(_nt, _r1pen) / _a;
			double c = (norm2(_r1pen) - R1*R1) / _a;
			double D = b*b - c;

			if (D > 0.0) {
				double t = -b - sqrt(D);
				vector3d  pos1 = t*nv + r_pen;					// 黄道座標系での位置

				vector3d pos3;
				if (ds.tyMode == MODE_3D) {
					vector3d  pos2 = fac * (pos1 - r1) / R1;		// 影を落とされる天体の中心を原点とした位置
					pos3 = M * pos2;
				}
				else {
					pos3 = Rc_pl * (pos1 - ds.camera.get_pos()).normalized();
				}


				glPointSize(3.0);
				glColor3f(1, 0, 0);
				glBegin(GL_POINTS);
					glVertex3dv( pos3.get_pointer() );
				glEnd();
			}
		}
	}

	glPopMatrix();
	glPopAttrib();
}
