/**************************************************************************************************

Copyright (c) 2015   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#ifndef __MTK_DOME_MASTER_H__
#define __MTK_DOME_MASTER_H__
#include  "../common/graphics/gl_framebuffer.h"
#include  "../common/graphics/gl_win.h"
#include  "../common/graphics/gl_image.h"
#include  "mtk_camera.h"




class gl_dome_master
{
	int N, _Nx[5], _Ny[5], _idx0[5];
	float *vtxbuf, *uvbuf;
	unsigned int *idxbuf;
	float circle_R;

public:
	gl_dome_master();
	~gl_dome_master();

	void	init(int _N);
	void	generate_map(const vector3f& eye_offset, const vector3f& proj_offset, float proj_elev_angle, float proj_azimuth, float proj_slue, float proj_view_angle);
	void	Draw(int face) const;
};



//---------------
//  Dome master
//---------------
void	set_dome_master_buffer_size(int sz);
void	set_dome_master_capture_size(int sz);
void	set_dome_master_mesh_N(int N);
void	set_dome_master_dome_R(float R);
void	set_dome_master_elev_angle(float angle);
float	get_dome_master_elev_angle();
void	set_dome_master_elev_angle_planetarium(float angle);
float	get_dome_master_elev_angle_planetarium();
void	set_dome_master_label_mag(float fac);
float	get_dome_master_label_mag();
void	set_dome_master_line_width_fac(float fac);
float	get_dome_master_line_width_fac();
// projector settings
void	set_dome_master_proj_offset(const vector3f& proj_offset);
void	set_dome_master_proj_elev_angle(float angle);
void	set_dome_master_proj_azimuth(float angle);
void	set_dome_master_proj_slue(float angle);
void	set_dome_master_proj_view_angle(float angle);
// projector settings with intersection point on dome
void	set_dome_master_intersection_mode(bool bFlg);
void	set_dome_master_intersection_elev_angle(float angle);
void	set_dome_master_intersection_azimuth(float angle);
// rotation angle for stereo scopic mode
void	set_dome_master_stereo_rot_angle(float angle);

void	load_dome_master_distortion_map();

void	draw_dome_master(const mtk_camera camera_base, int w, int h);



// Capture
void	capture_dome_master(const mtk_camera camera_base, image& img);




#endif
