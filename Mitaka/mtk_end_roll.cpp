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
#include  <gl/GL.h>
#include  <stdio.h>
#include  <string>
#include  "mtk_directory.h"
#include  "mtk_text.h"



using namespace std;



// テキスト
static mtk_text  end_credits;


//
static float width    = 100;
static float DY       = 0.0;
static float duration = 10.0;
static float angle    = 0.0;

static bool bLoadedEndCredits = false;



void
load_end_credits(const char* fn)
{
	FILE* fp;

	end_credits.clear();
	bLoadedEndCredits = false;

	directory dir = get_directory(DIR_LOCALE);
	if ((fp = dir.fopen_utf8(fn, "rt")) == NULL)  return;


	string buf;
	int ch;
	while ((ch = getc(fp)) != EOF) {
		buf.push_back(char(ch));
	}
	buf.push_back('\0');
	fclose(fp);


	bLoadedEndCredits = end_credits.set_text(buf.c_str());

	if (!bLoadedEndCredits) {
		end_credits.set_text("Failed!");
	}

	bLoadedEndCredits = true;
}




void
draw_end_credits(float frac)
{
	if (!bLoadedEndCredits)  return;


	mtk_text_block_size  bsz;
	bsz = end_credits.get_total_size();


	float scl = width / bsz.w;
	float total_h = bsz.hu - bsz.hd;
	float dy = total_h * frac;
	float y = dy - total_h + DY / scl;

	glPushMatrix();
		glScalef(scl, scl, scl);
		glRotatef(-angle, 1, 0, 0);
		glTranslatef(-0.5*bsz.w, y, 0);
		glColor3f(1, 1, 1);
		end_credits.draw();
	glPopMatrix();
}




//-----------
//  Set/Get
//-----------
void
set_credit_roll_width(float w)
{
	width = w;
}

float
get_credit_roll_width()
{
	return width;
}

void
set_credit_roll_dy(float dy)
{
	DY = dy;
}

float
get_credit_roll_dy()
{
	return DY;
}

void
set_credit_roll_duration(float dur)
{
	duration = dur;
}

float
get_credit_roll_duration()
{
	return duration;
}

void
set_credit_roll_angle(float ang)
{
	angle = ang;
}

float
get_credit_roll_angle()
{
	return angle;
}


