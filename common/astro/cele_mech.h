/******************************************************************************

Copyright (c) 2007-2014   Tsunehiko Kato

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
#ifndef  __CELE_MECH_H__
#define  __CELE_MECH_H__
#include  "../math/vector3.h"
#include  "../JulianDate.h"


//----------------------------------------------
//  軌道上の位置を求める（軌道座標系：焦点が原点）
//    a:  軌道長半径
//    e:  離心率
//    M0: 平均近点離角（元期、度）
//    n:  平均運動（度、単位時間あたり）
//    epoch: 元期
//    t:  位置を求める時刻
//----------------------------------------------
vector3d	calc_orbital_pos(const double& a, const double& e, const double& M0, const double& n, const JD& epoch, const JD& t);

//  軌道座標系での速度ベクトルを求める
vector3d	calc_orbital_velocity(const vector3d& pos, const double& a, const double& e, const double& n);


//---------------------------------------------------
//  ２つの座標間の変換
//---------------------------------------------------
//   node: 昇交点黄経, i: 軌道傾斜角, omg: 近(日)点引数
//  * 角度は全て度を単位とする
//---------------------------------------------------
inline matrix3d
calc_trans_matrix(const double& node, const double& i, const double& omg)
{
	return  Rzxz3(D2R*node, D2R*i, D2R*omg);
}

inline vector3d
calc_pos_on_ref_plane(const double& node, const double i, const double& omg, const vector3d& r)
{
	return  Rzxz3(D2R*node, D2R*i, D2R*omg) * r;
}


#endif
