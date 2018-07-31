/**************************************************************************************************

Copyright (c) 2007-2016   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#ifndef __MTK_DRAW_STRUCT_H__
#define __MTK_DRAW_STRUCT_H__
#include  <windows.h>
#include  "../common/math/vector3.h"
#include  "mtk_text.h"
#include  "mtk_screen.h"
#include  "mtk_camera.h"




//-------------------
//  描画用情報の構造体
//-------------------
class mtk_draw_struct
{
public:
	int			tyMode;			// モード
	int			tySubMode;



	// 黄道座標から視点座標への変換（スケーリングを含む）
	matrix3d	M_view, M_view_inv;
	vector3d	r_view;

	// ワールド座標系での視錐台平面
	vector3d nv_near[6], nv_far[6];
	double D_near[6], D_far[6];


	double		JD;					// 時刻（ユリウス日）  //***

	double		scale;				// スケール(pc)
	double		scale_factor;		// スケール因子

	float		plnt_R, plnt_angle, plnt_d;
	
	mtk_camera	camera_base0;		// ベースカメラ情報（惑星での視線移動を含まないもの。ヘッドライト用）
	mtk_camera	camera_base;		// ベースカメラ情報
	mtk_camera	camera;				// 描画に使用するカメラ
	double		cnorm;				// カメラの原点からの距離

	vector3d	tgt_pos;			// ターゲットの位置
	int			tgt_ID;

	int			vp_x1, vp_x2, vp_y1, vp_y2, vp_w, vp_h;		// ビューポート
	float		screen_aspect_ratio;

	frustum		frs_near, frs_far;	// Frustum

	vector3f	eye_offset;

	float		rad_per_pixel;		// 1ピクセルあたりの角度（ラジアン単位）
	float		ltf;				// 文字の相対的サイズ *** （仮）
	float		line_width_fac;		// 線の太さに掛ける因子

	bool		bRotateBillboard;	// ビルボードをカメラに向けて回転させるか
	vector3f	eRf, eUf;			// 回転しない場合のビルボードの向き

private:
	bool		bNear;
	void	calc_frustum_planes_on_world(const frustum& frs, vector3d *nv, double *D) const;

public:
	mtk_draw_struct()	{};

	void	set(int mode, int submode, const mtk_camera& _camera_base, const double& _JD, float scrn_aspct_ratio, int _vp_x1, int _vp_x2, int _vp_y1, int _vp_y2, const float font_size, int target_ID, const vector3d& target_pos, vector3f dr, bool bRotBB, float line_w);
	void	set_red_per_pixel();

	vector3d	calc_pos_on_viewframe(const vector3d& pos) const	{ return  M_view * pos + r_view; }
	vector2f	calc_pos_on_screen(const vector3d& pos) const;

	void	set_near_region()		{ bNear = true; }
	void	set_far_region()		{ bNear = false; }
	frustum	get_frustum() const 	{ return  ((bNear) ? frs_near : frs_far); }

	void	recalc_frustum_planes_on_world();		//***** ドームマスター用の暫定措置 *****
	void	get_frustum_planes_on_world(const vector3d* &nv, const double* &D) const;

	bool	is_near_region() const	{ return bNear; }
};


#endif
