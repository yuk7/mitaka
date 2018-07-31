/**************************************************************************************************

Copyright (c) 2014-2015   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#ifndef __MTK_MOVIE_H__
#define __MTK_MOVIE_H__
#include  <dshow.h>
#include  <vector>
using namespace std;
#include  "mtk_video.h"
#include  "../common/win_file.h"
#include  <string>



enum {
	STATE_NO_MOVIE,
	STATE_PLAYING,
	STATE_PAUSED,
	STATE_STOPPED
};




class mtk_movie
{
public:
	mtk_movie();
	~mtk_movie();

	void	CleanUpGraph();

	HRESULT	Open(const char *fn, HWND hwnd, int x0, int y0, int w, int h, UINT u_WM_NOTIFY, int iRenderer);
	void	Play();
	void	Pause();
	void	Home();
	void	End();
	void	Close();

	LONGLONG	GetPosition() const;
	LONGLONG	GetEndPosition() const;
	void		SetPosition(LONGLONG pos);

	void		GetWindowPosition(int &x, int &y, int &w, int &h) const;

	int	GetState() const;

	HRESULT	OnPaint(HWND hwnd, HDC hdc);
	HRESULT	OnDisplayChange();
	bool	GetEvent(long& evCode, long& prm1, long& prm2);


private:
	IGraphBuilder *m_pGB;
	IMediaControl *m_pMC;
	IMediaEventEx *m_pEv;
	IMediaSeeking *m_pMS;

	mtk_video  *m_pVideo;
	int	m_state;
	LONGLONG m_end_pos;
	int m_w, m_h;	// video size
	int m_x, m_y;	// video position
};





//=======================
//  メディアファイルの管理
//=======================

//---------------------
// メディアファイルの種類
//---------------------
enum {
	MEDIATYPE_IMAGE,
	MEDIATYPE_POPUP_IMAGE,
	MEDIATYPE_MOVIE,
	MEDIATYPE_AUDIO
};



//-----------------------
//  メディアファイルの情報
//-----------------------
struct  media_file_info
{
	char			type;					// メディアの種類
	std::string		menu_title;				// メニュー用タイトルのキー
	std::string		disp_title;				// 表示用タイトルのキー
	std::string		menu_title_key;			// メニュー用タイトルのキー
	std::string		disp_title_key;			// 表示用タイトルのキー

	std::string file_path;
	std::vector<std::string>	all_file_path;	// すべてのノードのメディアファイルのパス（制御用PCでのみ使用）
};




//----------------------------
//  メディアファイルを扱うクラス
//----------------------------
class  MediaFiles : public vector<media_file_info>
{
	std::string list_prefix;

public:
	MediaFiles(const std::string& _list_prefix) : list_prefix(_list_prefix) {}
	bool	open_info_file(const directory& dir, const char* fn, const std::string& lang_id);
	void	add_media_file(const media_file_info& mfi);
};




#endif
