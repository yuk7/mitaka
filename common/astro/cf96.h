/******************************************************************************

Copyright (c) 2007   Tsunehiko Kato
Copyright (c) 2014   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#ifndef __CF96_H__
#define __CF96_H__
#include  <vector>
using namespace std;
#include  "../math/vector3.h"


//=================================
// Reference:
//   Planetary Series (1996)
//   by J.Chapront and G. Francou 
//=================================

//---------
//  const
//---------
const double  CF96_t1 = 2415020.5;
const double  CF96_t2 = 2487980.5;


//---------
//  block
//---------
class CF96_block {
	vector<double>						sec[3];
	vector<vector<vector<double> > >	vvvct;
	vector<vector<vector<double> > >	vvvst;
public:
	bool	load(FILE* fp);
	vector3d	eval_pos(const double& x, const double& fx, const vector<int>& nf, const vector<vector<double> >& vfq)  const;
	vector3d	eval_vel(const double& dt, const double& x, const double& fx, const vector<int>& nf, const vector<vector<double> >& vfq)  const;
};


//----------
//  planet
//----------
class CF96_planet
{
	double	t0, dt;
	int		mx, imax, iblock, max_sec;
	vector<int>	nf;
	vector<vector<double> > vfq;
	vector<CF96_block>	vBlock;
public:
	bool	load(FILE* fp);
	vector3d	eval_pos(const double& t)  const;
	vector3d	eval_vel(const double& t)  const;
};


//-----------
//  planets
//-----------
class CF96_planets
{
	bool	bLoad;
	vector<CF96_planet>  vPS;
public:
	CF96_planets() : bLoad(false) {};
	bool		isLoaded() const { return bLoad; }
	bool		isInRange(const double& t)	{ return (t >= CF96_t1 && t <= CF96_t2); }
	bool		load(FILE* fp);
	bool		load(const char* fn);
	int			get_planet_num() const	{ return vPS.size(); }
	vector3d	eval_pos(int idx, const double& t) const;
	vector3d	eval_vel(int idx, const double& t) const;
};


#endif