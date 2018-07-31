/******************************************************************************

Copyright (c) 2007   Tsunehiko Kato

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

以下に定める条件に従い、本ソフトウェアおよび関連文書のファイル（以下「ソフトウェ
ア」）の複製を取得するすべての人に対し、ソフトウェアを無制限に扱うことを無償で許
可します。これには、ソフトウェアの複製を使用、複写、変更、結合、掲載、頒布、サブ
ライセンス、および/または販売する権利、およびソフトウェアを提供する相手に同じこ
とを許可する権利も無制限に含まれます。

上記の著作権表示および本許諾表示を、ソフトウェアのすべての複製または重要な部分に
記載するものとします。

ソフトウェアは「現状のまま」で、明示であるか暗黙であるかを問わず、何らの保証もな
く提供されます。ここでいう保証とは、商品性、特定の目的への適合性、および権利非侵
害についての保証も含みますが、それに限定されるものではありません。 作者または著
作権者は、契約行為、不法行為、またはそれ以外であろうと、ソフトウェアに起因または
関連し、あるいはソフトウェアの使用またはその他の扱いによって生じる一切の請求、損
害、その他の義務について何らの責任も負わないものとします。
*******************************************************************************/
#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__
#include  <dinput.h>


//-----------------------------------------------------
//  class joystick
//    注意: 初期化後に SetCooperativeLevel を実行すること
//-----------------------------------------------------
class joystick
{
private:
	LPDIRECTINPUT8			pDI;		// DirectInput8デバイス 
	LPDIRECTINPUTDEVICE8	pJoystick;	// Joystickデバイス
	int	nAxis, nButton;
public:
	joystick();
	~joystick();
	HRESULT	SetCooperativeLevel(HWND hWnd, DWORD dwFlags);
	bool	isValid() const	{ return ((pDI!=NULL) && (pJoystick!=NULL)); }
	HRESULT	GetJoystickState(DIJOYSTATE2* pJs) const;
	int		get_axis_num() const	{ return nAxis; }
	int		get_button_num() const	{ return nButton; }
	HRESULT	Acquire()	{ if (!isValid()) return E_FAIL;  return pJoystick->Acquire(); }
	HRESULT	Unacquire()	{ if (!isValid()) return E_FAIL;  return pJoystick->Unacquire(); }
private:
	static BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance,
												VOID* pContext );
	static BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,
											  VOID* pContext );
};


//-------------------------------
// Input (ELECOM 用のキーバインド）
//-------------------------------
enum axis_type
{
	AXIS_X,
	AXIS_Y,
	AXIS_RZ,
	AXIS_Z
};

enum button_type
{
	BTN_TRI,
	BTN_CIR,
	BTN_CRS,
	BTN_SQR,
	BTN_L2,
	BTN_R2,
	BTN_L1,
	BTN_R1,
	BTN_PLAY,
	BTN_SELECT,
	BTN_L3,
	BTN_R3,
	BTN_UP,
	BTN_RIGHT,
	BTN_DOWN,
	BTN_LEFT
};


class axis_state
{
public:
	int		value, last_value;
	bool	bToggle;
	int		axis_no;
	int		thr_val, max_val;
	int		key_code1, key_code2;
	int		key_val1, key_val2;
	float	power_index;
public:
	axis_state() : power_index(1)  {};
	void	set(int _axis_no, int _thr_val, int _max_val, int _key_code1, int _key_val1, int _key_code2, int _key_val2);
	void	key_in();
	void	joystick_in(DIJOYSTATE2& djs);
	float	get_normalized_value() const  { return float(value)/(max_val+1); }
	void	set_power_index(float pwi)	{ power_index = pwi; }
	float	get_power_index() const		{ return power_index; }
};


class button_state
{
public:
	bool	value, last_value;
	bool	bToggle;
	int		button_no;
	int		key_code;
public:
	void	set(int _button_no, int _key_code)  { button_no=_button_no; key_code=_key_code; }
	void	key_in();
	void	joystick_in(DIJOYSTATE2& djs);
};


class joystick_state
{
public:
	axis_state		axis[4];
	button_state	button[16];
public:
	void update(joystick& jstk);
};


#endif
