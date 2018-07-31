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
#include  <windows.h>
#include  <gl/gl.h>
#include  "mtk_save_image.h"
#include  "../common/graphics/gl_image.h"
#include  "../common/win_dlg.h"
#include  "../common/text/unicode_string.h"
#include  "mtk_draw_struct.h"
#include  "mtk_pbuffer.h"
#include  "mtk_strings.h"


//  externs
void				draw_scene_all(const mtk_draw_struct& ds, bool bClear);
mtk_draw_struct calc_draw_struct(const mtk_camera& camera_base, float aspc_rt, int _vp_x1, int _vp_x2, int _vp_y1, int _vp_y2, vector3f dr, bool bRotBB, float line_w);


//
static wchar_t szFileName[MAX_PATH] = L"\0";
static int ImageType;
static int SaveImageWidth, SaveImageHeight;



const wchar_t*
get_save_image_high_res_filename()
{
	return  szFileName;
}



bool
save_image_high_resolution_get_filename(int w, int h)
{
	static char  fn[MAX_PATH];
	static char  filename16[MAX_PATH], filter16[256], title16[256];
	fn[0] = '\0';


	std::wstring filter, title;
	filter = utf8_to_wstring(apply_null_character(get_string("SAVEIMAGE_FILTER")));
	title  = get_wstring("SAVEIMAGE_TITLE");


	ImageType = GetFileNameW(NULL, szFileName, filter.c_str(), title.c_str(), true);


	if (ImageType <= 0)  {
		szFileName[0] = '\0';
		return false;
	}

	SaveImageWidth = w;
	SaveImageHeight = h;

	return  true;
}




bool
save_image_high_resolution(const mtk_camera camera_base)
{
	if (szFileName[0] == '\0')  return false;

	int w = SaveImageWidth;
	int h = SaveImageHeight;
	int wmax = get_pbuffer_width_max();
	int hmax = get_pbuffer_height_max();
	if (w > wmax)  w = wmax;
	if (h > hmax)  h = hmax;


	if ( !create_pbuffer(w, h) )  return false;
	draw_scene_all(calc_draw_struct(camera_base, float(h)/float(w), 0, w, 0, h, vector3f(0,0,0), true, 1.0), true);
	image  img(0,0,3);
	gl_read_pixels(img, 0,0, w, h);
	destroy_pbuffer();


	switch (ImageType) {
		case 1:	img.saveJPEG(szFileName);	break;
		case 2:	img.savePNG(szFileName);	break;
		case 3:	img.saveBMP(szFileName);	break;
		default:							break;
	}

	return  true;
}