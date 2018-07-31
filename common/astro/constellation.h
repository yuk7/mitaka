/******************************************************************************

Copyright (c) 2007-2015   Tsunehiko Kato

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
#ifndef  __CONSTELLATION_H__
#define  __CONSTELLATION_H__
#include  "astrometry.h"




//----------------------
//  Constellation Name
//----------------------
enum CNST_ID {
	CNST_AND,
	CNST_MON,
	CNST_SGR,
	CNST_DEL,
	CNST_IND,
	CNST_PSC,
	CNST_LEP,
	CNST_BOO,
	CNST_HYA,
	CNST_ERI,
	CNST_TAU,
	CNST_CMA,
	CNST_LUP,
	CNST_UMA,
	CNST_VIR,
	CNST_ARI,
	CNST_ORI,
	CNST_PIC,
	CNST_CAS,
	CNST_DOR,
	CNST_CNC,
	CNST_COM,
	CNST_CHA,
	CNST_CRV,
	CNST_CRB,
	CNST_TUC,
	CNST_AUR,
	CNST_CAM,
	CNST_PAV,
	CNST_CET,
	CNST_CEP,
	CNST_CEN,
	CNST_MIC,
	CNST_CMI,
	CNST_EQU,
	CNST_VUL,
	CNST_UMI,
	CNST_LMI,
	CNST_CRT,
	CNST_LYR,
	CNST_CIR,
	CNST_ARA,
	CNST_SCO,
	CNST_TRI,
	CNST_LEO,
	CNST_NOR,
	CNST_SCT,
	CNST_CAE,
	CNST_SCL,
	CNST_GRU,
	CNST_MEN,
	CNST_LIB,
	CNST_LAC,
	CNST_HOR,
	CNST_VOL,
	CNST_PUP,
	CNST_MUS,
	CNST_CYG,
	CNST_OCT,
	CNST_COL,
	CNST_APS,
	CNST_GEM,
	CNST_PEG,
	CNST_SER,
	CNST_OPH,
	CNST_HER,
	CNST_PER,
	CNST_VEL,
	CNST_TEL,
	CNST_PHE,
	CNST_ANT,
	CNST_AQR,
	CNST_HYI,
	CNST_CRU,
	CNST_PSA,
	CNST_CRA,
	CNST_TRA,
	CNST_SGE,
	CNST_CAP,
	CNST_LYN,
	CNST_PYX,
	CNST_DRA,
	CNST_CAR,
	CNST_CVN,
	CNST_RET,
	CNST_FOR,
	CNST_SEX,
	CNST_AQL,
	CNST_NONE
};

int	get_constellation_ID(const char* str);
const char*	get_constellation_abbr(const int CnstID);



#endif
