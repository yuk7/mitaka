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
#include  "mtk_joystick.h"
static joystick		jstk;
static joystick_state	jsst;
static float power_index = 1;

int  JoystickAxisIni[4];
int	 JoystickButtonIni[16];

char szIniAxisText[4][20] = {
	"AXIS_X",
	"AXIS_Y",
	"AXIS_RZ",
	"AXIS_Z"
};

char szIniButtonText[16][20] = {
	"BTN_TRI",
	"BTN_CIR",
	"BTN_CRS",
	"BTN_SQR",
	"BTN_L2",
	"BTN_R2",
	"BTN_L1",
	"BTN_R1",
	"BTN_START",
	"BTN_SELECT",
	"BTN_L3",
	"BTN_R3",
	"BTN_UP",
	"BTN_RIGHT",
	"BTN_DOWN",
	"BTN_LEFT"
};

char chButtonKey[16] = {
	'S',	// △
	'X',	// ○
	'Z',	// ×
	'A',	// □
	'1',	// L1
	'2',	// R1
	'3',	// L2
	'4',	// R2
	'W',	// Play
	'Q',	// Select
	'5',	// LA
	'6',	// RA
	'U',	// Up
	'R',	// Right
	'D',	// Down
	'L'		// Left
};

void
set_joystick_power_index(float pwi)
{
	power_index = pwi;
}

float
get_joystick_power_index()
{
	return  power_index;
}

//--------------------
//  Joystick の初期化
//--------------------
void
init_joystick()
{
	const int unit    = 1000;
	const int thr_val = 0.3*unit;
	const int max_val = unit;

	button_state*	btn = get_joystick_button_state();
	axis_state*		axi = get_joystick_axis_state();

	//-- key mapping
	axi[0].set(JoystickAxisIni[0], thr_val, max_val, VK_LEFT,  -unit, VK_RIGHT, unit);
	axi[1].set(JoystickAxisIni[1], thr_val, max_val, VK_UP,    -unit, VK_DOWN,  unit);
	axi[2].set(JoystickAxisIni[2], thr_val, max_val, VK_PRIOR, -unit, VK_NEXT,  unit);
	axi[3].set(JoystickAxisIni[3], thr_val, max_val, 'O', -unit, 'P',  unit);

	//-- power index
	for (int i=0; i<4; i++) {
		axi[i].set_power_index(power_index);
	}

	for (int i=0; i<16; i++) {
		btn[i].set(JoystickButtonIni[i], chButtonKey[i]);
	}
}

void
joystick_SetCooperativeLevel(HWND hWnd)
{
	if (jstk.isValid()) {
		jstk.SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	}
}

void
joystick_Acquire(bool bAcquite)
{
	if (bAcquite)		{ jstk.Acquire(); }
	else				{ jstk.Unacquire(); }
}

//------------
//  状態の取得
//------------
bool
is_joystick_available()
{
	return  jstk.isValid();
}

void
update_joystick_state()
{
	jsst.update(jstk);
}

button_state*
get_joystick_button_state()
{
	return  jsst.button;
}

axis_state*
get_joystick_axis_state()
{
	return  jsst.axis;
}
