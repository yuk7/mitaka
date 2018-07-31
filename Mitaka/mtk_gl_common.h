/**************************************************************************************************

Copyright (c) 2007   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#ifndef __MTK_GL_COMMON_H__
#define __MTK_GL_COMMON_H__
#include  "mitaka.h"
#include  "../common/graphics/gl_common.h"
#include  "../common/math/vector3.h"
#include  "../common/graphics/gl_image.h"
#include  "../common/astro/astro_const.h"
#include  "mtk_camera_setting.h"
#include  "mtk_screen.h"


extern float eye_offset_x;

//-------------------------------------
//  その点が描画領域に入っているか判定する
//-------------------------------------
inline bool
PointIsInside(const frustum& frs, const vector3f& r, const GLdouble* MM)
{
	// 視点座標系での位置ベクトル
	vector3f  rv;
	rv.x = MM[0]*r.x + MM[4]*r.y + MM[8] *r.z + MM[12];
	rv.y = MM[1]*r.x + MM[5]*r.y + MM[9] *r.z + MM[13];
	rv.z = MM[2]*r.x + MM[6]*r.y + MM[10]*r.z + MM[14];

	// スクリーンの角度によるカリング
	char bf = frs.cull_test(rv.x, rv.y, -rv.z);
	return  (bf == 0);
}

// オブジェクトを現在の視点の方向に向かせる
inline float
toward_pos(const vector3d& r0)
{
	quaterniond camera_ori;
	double M[16];
	vector3d  r, eU, eF2, eU2, eR2;
	double R, R0;

	// カメラ座標系での位置座標を取得
	glGetDoublev( GL_MODELVIEW_MATRIX, M);
	r.x = M[12];
	r.y = M[13];
	r.z = M[14];
	r -= get_base_origin_on_local_camera();	// 視点のずらしをもどす
	R0 = r.norm();
	r -= r0;
	eU = get_eU_of_base_on_local_camera();	// カメラ座標系での基準座標系の上方向
	R   = r.norm();
	
	eF2 = (r/R);			// r0 からオブジェクトへ向かう単位ベクトル
	eR2 = normalize( cross_product(  eU, -eF2 ) );
	eU2 = cross_product( -eF2, eR2 );
	M[0]  = eR2.x;
	M[1]  = eR2.y;
	M[2]  = eR2.z;
	M[3]  = 0;
	M[4]  = eU2.x;
	M[5]  = eU2.y;
	M[6]  = eU2.z;
	M[7]  = 0;
	M[8]  = -eF2.x;
	M[9]  = -eF2.y;
	M[10] = -eF2.z;
	M[11] = 0;
	glLoadMatrixd(M);

	return  R0;
}



inline float
toward_eye_pos()
{
	quaterniond camera_ori;
	double M[16];
	vector3d  r, eU, eF2, eU2, eR2;
	double R;

	// カメラ座標系での位置座標を取得
	glGetDoublev( GL_MODELVIEW_MATRIX, M);
	r.x = M[12];
	r.y = M[13];
	r.z = M[14];

	r += get_base_origin_on_local_camera();	// 視点のずらしをもどす
	eU = get_eU_of_base_on_local_camera();	// カメラ座標系での基準座標系の軸

	R   = r.norm();
	eF2 = (r/R);			// 視点（両目の中間）からオブジェクトへ向かうベクトル
	eR2 = normalize( cross_product(  eU, -eF2 ) );
	eU2 = cross_product( -eF2, eR2 );
	M[0]  = eR2.x;
	M[1]  = eR2.y;
	M[2]  = eR2.z;
	M[3]  = 0;
	M[4]  = eU2.x;
	M[5]  = eU2.y;
	M[6]  = eU2.z;
	M[7]  = 0;
	M[8]  = -eF2.x;
	M[9]  = -eF2.y;
	M[10] = -eF2.z;
	M[11] = 0;
	glLoadMatrixd(M);
	return  R;
}




//--  オブジェクトを現在の視点の方向に向かせる
//    スケールはそのままのバージョン
inline void
toward_eye_pos2()
{
	quaterniond camera_ori;
	double M[16];
	vector3d  r, eU, eF2, eU2, eR2;
	double R;

	// カメラ座標系での位置座標を取得
	glGetDoublev( GL_MODELVIEW_MATRIX, M);
	r.x = M[12];
	r.y = M[13];
	r.z = M[14];

	r += get_base_origin_on_local_camera();	// 視点のずらしをもどす
	eU = get_eU_of_base_on_local_camera();	// カメラ座標系での基準座標系の軸

	R   = r.norm();
	eF2 = (r/R);			// 視点（両目の中間）からオブジェクトへ向かうベクトル
	eR2 = normalize( cross_product(  eU, -eF2 ) );
	eU2 = cross_product( -eF2, eR2 );

	float scl = sqrt(M[0]*M[0] + M[1]*M[1] + M[2]*M[2]);
	eF2 *= scl;
	eR2 *= scl;
	eU2 *= scl;
	M[0]  = eR2.x;
	M[1]  = eR2.y;
	M[2]  = eR2.z;
	M[3]  = 0;
	M[4]  = eU2.x;
	M[5]  = eU2.y;
	M[6]  = eU2.z;
	M[7]  = 0;
	M[8]  = -eF2.x;
	M[9]  = -eF2.y;
	M[10] = -eF2.z;
	M[11] = 0;
	glLoadMatrixd(M);
}

//  中央のスクリーン上に移動する
inline void
goto_center_screen(float rot_ang)
{
	const double* Mbc;
	Mbc = get_local_camera_matrix_GL();

	glLoadIdentity();
	glMultMatrixd(Mbc);
	double M[16];
	quaternion_to_matrix_GL_tr( get_base_camera_ori(), M );
	glMultMatrixd(M);

	glRotatef(rot_ang, 1, 0, 0);

	glTranslatef(0,0,-get_screen_z());
}

inline void
goto_each_screen()
{
	glLoadIdentity();
	glTranslatef(0,0,-get_screen_z());
}

//-----------------------------
//  面を視点を向くように設定する。
//  また、距離によらず同じサイズに
//  なるように文字サイズも調整する
//-----------------------------
inline void
prepare_3Dtext_1(const float R, const float th_deg, const float sz, const float scl_fac, bool bConstSize)
{
	// 面の向きを設定する
	float ra;
	ra = toward_eye_pos();
	float fac0 = ra/get_screen_z();

	// サイズの分だけずらす
	float d = R;
	d *= scl_fac;
	double th = pi/180.0*th_deg;
	if (bConstSize) { d *= fac0; }
	glTranslatef(d*cos(th), d*sin(th), 0);

	float scl = sz*fac0 * get_screen_w()/180.0;
	glScalef(scl,scl,scl);
}

inline void
prepare_3Dtext_2(const vector3f& rc, const float R, const float sz, const float screen_w, const float screen_z, bool bConstSize, bool bRev)
{
	float ra = toward_pos(rc);
	if (bRev) {
		glRotatef(180, 0,1,0);
	}

	// サイズの分だけずらす
	float fac0 = ra/screen_z;
	float d = R / 1.4142f;
	if (bConstSize) { d *= fac0; }
	glTranslatef(d, d, 0);
	float scl = sz*fac0 * screen_w / 180.0;
	glScalef(scl,scl,scl);
}


//---------------------
//  地球赤道面に移動する
//---------------------
inline void
go_to_equ_coord()
{
	glRotated(-epsJ2000, 1,0,0);	// 赤道座標系へ
}


//-----------
//  Texture
//-----------
void	set_texture(GLuint& texName, image& img);
void	set_texture_alpha(GLuint& texName, image& img);

#endif
