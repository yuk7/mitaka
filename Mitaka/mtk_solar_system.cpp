/**************************************************************************************************

Copyright (c) 2007-2012   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#include  "mtk_solar_system.h"
#include  "mtk_colors.h"
#include  "mtk_sun.h"
#include  "mtk_planets.h"
#include  "mtk_satellites.h"
#include  "mtk_minor_planets.h"
#include  "mtk_oort_cloud.h"
#include  "mtk_label.h"
#include  "mtk_objects.h"
#include  "mtk_object_position.h"
#include  "mtk_time.h"
#include  "mtk_mode.h"



//-------------
//  Text size
//-------------
static float  ObjectNameSize, SpecialObjectNameSize;

//---------
//  Orbit
//---------
static float OrbitWidth;

//-------------
//  Zoom mode
//-------------
const zoom_rate ZoomRate[5] = {
	{   1,  1,  1,   1,  1},
	{   2,  2,  1,   2,  2},
	{ 500,  2, 25,  20,  2},
	{1000, 25, 50, 100,  5},
	{1500, 50, 70, 200, 10}
};

static char ZoomMode = 0;


//------------
//  Lighting
//------------
static float LightingAmbient;
float LightingAmbientDef = 0.1;


void	set_side_light(const mtk_draw_struct& ds);



//-----------------
//  Load textures
//-----------------
void
load_solar_system_textures()
{
	//-- Sun
	load_sun_textures();

	//-- Planets
	load_planets_textures();

	//-- Satellite
	load_satellites_textures();

	//-- Minor planets
	load_minor_planet_textures();

	//-- Ring
	load_ring_textures();
}







//-------------
//  Zoom rate
//-------------
zoom_rate
get_zoom_rate()
{
	return ZoomRate[ZoomMode];
}

void
set_zoom_mode(char zoom_mode)
{
	ZoomMode = zoom_mode;
	const double  t = get_time_JD();

	char mode = get_mode();
	set_satellites_zoom_rate(mode);

	update_objects_position_and_orientation(t, true);
	update_space_curves(t);
}

char
get_zoom_mode()
{
	return ZoomMode;
}




//-------------
//  Text size
//-------------
float
get_object_name_size()
{
	return  ObjectNameSize;
}

float
get_special_object_name_size()
{
	return  SpecialObjectNameSize;
}

void
set_object_name_size(float sz)
{
	ObjectNameSize = sz;
}

void
set_special_object_name_size(float sz)
{
	SpecialObjectNameSize = sz;
}


//---------
//  Orbit
//---------
float
get_orbit_width()
{
	return  OrbitWidth;
}

void
set_orbit_width(float w)
{
	OrbitWidth = w;
}


//------------
//  Lighting
//------------
float
get_lighting_ambient()
{
	return  LightingAmbient;
}

void
set_lighting_ambient(float amb)
{
	LightingAmbient = amb;
}




//--------
//  Draw
//--------
void
draw_celestial_object(float Rx, float Ry, float Rz, float tex_shift)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glPushMatrix();
		if (tex_shift != 0.0f) {
			glRotatef(tex_shift, 0, 0, 1);
		}
		glScalef(Rx, Ry, Rz);
		glEnable(GL_CULL_FACE);
		gl_draw_LOD_sphere();
	glPopMatrix();
}





