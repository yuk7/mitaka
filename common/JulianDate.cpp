/******************************************************************************

Copyright (c) 2007-2012   Tsunehiko Kato

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
#include  <windows.h>
#include  "JulianDate.h"
#include  <time.h>
#include  <math.h>

// <time.h> の __time64_t などは、1970年から3000年までしか扱えないので、使用しない。

//-------
//  定数
//-------
const double  _1day_min = 1440.0;											// １日の分
const double  _1day_sec = 86400.0;											// １日の秒数
const int days1[12] = { 0,31,59,90, 120,151,181,212, 243,273,304,334 };		// 平年の通日
const int days2[12] = { 0,31,60,91, 121,152,182,213, 244,274,305,335 };		// 閏年の通日
const JD	JD_111	 = 1721425.5;											// 西暦１年１月１日０時のユリウス日
const JD	JD_UTC0 = 2415020 + 25567 + 0.5;								// UTCの基準(1970年1月1日）のJD


//------------------------
//  UTCからユリウス日を得る
//------------------------
JD
get_JD(int year, int month, int day, int hour, int min, double sec)
{
	while (month<=0)	{ year--; month+=12; }
	while (month>12)	{ year++; month-=12; }

	double y = double(year-1);
	bool   leap_year = (year%4==0) && ((year%100!=0) || (year%400==0));

	double a,b,c;
	a = floor(365.25*y) - floor(y/100.0) + floor(y/400.0) + 1721425.0;
	b = (leap_year) ? double(days2[month-1]) : double(days1[month-1]);
	c = double(day) + double(hour)/24.0 + double(min)/_1day_min + sec/_1day_sec; 

	return a + b + c - 0.5;
}

JD
UTC_to_JD(const UTC& utc)
{
	return  get_JD(utc.year, utc.month, utc.day, utc.hour, utc.min, utc.sec);
}


//---------------------------
//  ユリウス日からUTCを得る
//  (*** year > -4713 ***)
//---------------------------
UTC
JD_to_UTC(const JD& jd)
{
	double	JD1, JD2, sec;
	int     JD0, L, N, I, J;
	int		year, mon, day, hour, min;
	
	JD0 = int(jd+0.5);
	JD1 = double(JD0)-0.5;

	L = JD0 + 68569;
	N = (4*L) / 146097;
	L = L - (146097*N + 3)/4;
	I = (4000*(L+1)) / 1461001;
	L = L - (1461*I)/4 + 31;
	J = (80*L)/2447;
	day = L - (2447*J)/80;
	L = J/11;
	mon = J+2-12*L;
	year = 100*(N-49) + I + L;

	JD2 = jd - JD1;
	hour = int(JD2*24.0);
	min  = int(JD2*1440.0 - hour*60.0);
	sec  = JD2*86400.0 - hour*3600.0 - min*60.0;

	UTC utc;
	utc.year  = year;
	utc.month = mon;
	utc.day   = day;
	utc.hour  = hour;
	utc.min   = min;
	utc.sec   = sec;
	return  utc;
}


//-----------------------
//  現在のユリウス日を得る
//-----------------------
JD
get_present_JD()
{
	SYSTEMTIME st;
	GetSystemTime(&st);
	UTC utc;
	utc.year	= st.wYear;
	utc.month	= st.wMonth;
	utc.day		= st.wDay;
	utc.hour	= st.wHour;
	utc.min		= st.wMinute;
	utc.sec		= double(st.wSecond) + 0.001 * double(st.wMilliseconds);
	return  UTC_to_JD(utc);
}


//-----------------
//  現在のUTCを得る
//-----------------
UTC
get_present_UTC()
{
	return  JD_to_UTC( get_present_JD() );
}


//--------------------------
//  現在のローカルタイムを得る
//--------------------------
UTC
get_present_LocalTime(const double& DT)
{
	return  JD_to_UTC( get_present_JD() + DT/24.0 );
}
