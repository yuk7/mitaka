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
#include  "joystick.h"
#include  <math.h>


#define  SAFE_RELEASE(p)  { if (p) { (p)->Release(); (p)=NULL; } }


//------------------
//  Class joystick
//------------------
joystick::joystick()
:  pDI(NULL), pJoystick(NULL), nAxis(0), nButton(0)
{
	HRESULT  hr;
	
	//-- DirectInputインターフェイスの取得
	hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION,
							IID_IDirectInput8, (VOID**)&pDI, NULL );
    if (FAILED(hr))  return;

	//-- ジョイスティックを列挙する
	hr = pDI->EnumDevices( DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback,
							 (void*)this, DIEDFL_ATTACHEDONLY );
	if (FAILED(hr))  return;

	//-- ジョイスティックが使えることを確認する
	if (pJoystick == NULL)  return;

	//-- ジョイスティックのデータフォーマットを設定する
	hr = pJoystick->SetDataFormat( &c_dfDIJoystick2 );
	if (FAILED(hr))  return;

	//-- ジョイスティックの設定をする
	hr = pJoystick->EnumObjects( EnumObjectsCallback, (void*)this, DIDFT_ALL );
	if (FAILED(hr))  return;
}


joystick::~joystick()
{
	//--  デバイスを開放する
	if (pJoystick)	pJoystick->Unacquire();
	//--  全ての DirectInput オブジェクトを開放する
	SAFE_RELEASE( pJoystick );
	SAFE_RELEASE( pDI );
}

BOOL CALLBACK
joystick::EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext )
{
	joystick* pJ = (joystick*)pContext;
	//-- ジョイスティックのインターフェイスを取得する
	HRESULT  hr;
	hr = pJ->pDI->CreateDevice( pdidInstance->guidInstance, &pJ->pJoystick, NULL);
	if (FAILED(hr))  return  DIENUM_CONTINUE;
	return  DIENUM_STOP;
}

BOOL CALLBACK
joystick::EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext )
{
	joystick* pJ = (joystick*)pContext;

	static int nSliderCount = 0;	// スライダーコントロールの数
	static int nPOVCount = 0;		// POV コントロールの数
	//-- 軸の時は、軸の値の範囲を設定する
	if (pdidoi->dwType & DIDFT_AXIS) {
		pJ->nAxis++;
		DIPROPRANGE  diprg;
		diprg.diph.dwSize		= sizeof(DIPROPRANGE);
		diprg.diph.dwHeaderSize	= sizeof(DIPROPHEADER);
		diprg.diph.dwHow		= DIPH_BYID;
		diprg.diph.dwObj		= pdidoi->dwType;
		diprg.lMin				= -1000;
		diprg.lMax				= +1000;
		HRESULT  hr;
		hr = pJ->pJoystick->SetProperty( DIPROP_RANGE, &diprg.diph );
		//if (FAILED(hr))  return DIENUM_STOP;
	}
	if (pdidoi->dwType & DIDFT_BUTTON) {
		pJ->nButton++;
	}
	if (pdidoi->guidType == GUID_Slider) {
		nSliderCount++;
	}
	if (pdidoi->guidType == GUID_POV) {
		nPOVCount++;
	}
	return  DIENUM_CONTINUE;
}


HRESULT
joystick::SetCooperativeLevel(HWND hWnd, DWORD dwFlags)
{
	HRESULT  hr;

	if (!isValid())  return E_FAIL;

	//-- 協調レベルの設定
	hr = pJoystick->SetCooperativeLevel(hWnd, dwFlags);
	//if (FAILED(hr))  return hr;

	return hr;
}


//-------------------------------
//  ジョイスティックの状態を取得する
//-------------------------------
HRESULT
joystick::GetJoystickState(DIJOYSTATE2* pJs) const
{
	HRESULT		hr;
	if (!isValid())  return  E_FAIL;

	//-- Poll the devie to read the current state
	hr = pJoystick->Poll();
	if (FAILED(hr)) {
		hr = pJoystick->Acquire();
		while (hr == DIERR_INPUTLOST)
			hr = pJoystick->Acquire();
		return  S_OK;
	}

	//-- デバイスの状態を取得する
	hr = pJoystick->GetDeviceState( sizeof(DIJOYSTATE2), pJs );
	if (FAILED(hr))	return  hr;

	return  S_OK;
}


//-------
// Input
//-------
void
axis_state::set(int _axis_no, int _thr_val, int _max_val, int _key_code1, int _key_val1, int _key_code2, int _key_val2)
{
	axis_no = _axis_no;
	thr_val = _thr_val;
	max_val = _max_val;
	key_code1 = _key_code1;
	key_val1  = _key_val1;
	key_code2 = _key_code2;
	key_val2  = _key_val2;
}

void
axis_state::key_in()
{
	int key_state;
	if (key_code1 != 0) {
		key_state = GetAsyncKeyState(key_code1);
		if (key_state < 0)  {
			bToggle = (key_state & 1) ? true : false;
			value = key_val1;
			return;
		}
	}
	if (key_code2 != 0) {
		key_state = GetAsyncKeyState(key_code2);
		if (key_state < 0) {
			bToggle = (key_state & 1) ? true : false;
			value = key_val2;
			return;
		}
	}
}

void
axis_state::joystick_in(DIJOYSTATE2& djs)
{
	if (value != 0)  return;	// キーボードを優先

	int j_in;
	switch (axis_no) {
		case AXIS_X:	j_in = djs.lX;	break;
		case AXIS_Y:	j_in = djs.lY;	break;
		case AXIS_RZ:	j_in = djs.lRz;	break;
		case AXIS_Z:	j_in = djs.lZ;	break;
		default:						return;
	}

	if (abs(j_in)>=thr_val && abs(j_in)<=max_val) {
		// 再規格化
		float x = float(abs(j_in)-thr_val)/float(max_val-thr_val);
		float y = pow(x, power_index);
		value = int(max_val * y);
		if (value > max_val)  value = max_val;
		if (j_in < 0)  value = -value;
		bToggle = (last_value == 0);
	}
}

void
button_state::key_in()
{
	if (key_code != 0) {
		int key_state = GetAsyncKeyState(key_code);
		if (key_state < 0)  {
			bToggle = (key_state & 1) ? true : false;
			value = true;
		}
	}
}

void
button_state::joystick_in(DIJOYSTATE2& djs)
{
	if (value != 0)  return;	// キーボードを優先
	value = djs.rgbButtons[button_no];
	bToggle = (last_value != value);
}

void
joystick_state::update(joystick& jstk)
{
	const int num_of_axes	 = 4;
	const int num_of_buttons = 16;
	//--- init
	for (int i=0; i<num_of_axes; ++i) {
		axis[i].last_value = axis[i].value;
		axis[i].value = 0;
	}
	for (int i=0; i<num_of_buttons; ++i) {
		button[i].last_value = button[i].value;
		button[i].value = false;
	}

	//----------
	// Keyboard
	//----------
	//-- Axes
	for (int i=0; i<num_of_axes; ++i) {
		axis[i].key_in();
	}
	//-- Buttons
	for (int i=0; i<num_of_buttons; ++i) {
		button[i].key_in();
	}

	//----------
	// Joystick
	//----------
	DIJOYSTATE2 djs;
	if (!jstk.isValid())						return;
	if (FAILED(jstk.Acquire()))					return;
	if (FAILED(jstk.GetJoystickState(&djs)))	return;
	//-- Axes
	for (int i=0; i<num_of_axes; i++) {
		axis[i].joystick_in(djs);
	}
	//-- Buttons
	for (int i=0; i<num_of_buttons; ++i) {
        button[i].joystick_in(djs);
	}
}
