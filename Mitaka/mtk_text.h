/**************************************************************************************************

Copyright (c) 2008   Tsunehiko Kato
Copyright (c) 2015   Tsunehiko Kato  and  4D2U Project, National Astronomical Observatory of Japan

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
#ifndef __MTK_TEXT_H__
#define __MTK_TEXT_H__
#include  "../common/ctext.h"
#include  "../common/graphics/color.h"


struct mtk_text_block_size {
	float w, hu, hd;
};


//---------------------
//  mtk_text_block_nifo
//---------------------
struct mtk_text_block_info {
	float	x1, x2, y;
	mtk_text_block_size  bsz, esz;
	float	size;
	RGBAf	color;
	float   D;
	char	align_mode;

	vector<float> vTabStop;
	int		tab_count;
};


//============
//  mtk_text
//============
class  mtk_text : public  ctext
{
	vector<mtk_text_block_info>	vBlock;
	mtk_text_block_info	cur_blk;
	float cx, cy;
	float bx1, bx2;
	float	sze;	// 現在のエレメント内の相対的サイズ
public:
	mtk_text() {};
	mtk_text(const char* p) : ctext(p)  {}
	void	draw();
	void	draw_center(float w);
	void	draw_right(float w);
	void	draw_kintou(float w);
	mtk_text_block_size	 get_total_size()  {
		int icode = 0;

		sze = 1.0;

		mtk_text_block_size  bsz;
		get_block_size(icode, bsz);
		return  bsz;
	}
private:
	//-- サイズおよび要素数の計算
	void	set_element(int icode);
	int		get_string_size(const char* p, mtk_text_block_size& sz);
	int		get_command_size(const ctext_command& cmd, mtk_text_block_size& sz);
	int		get_element_size(int& icode, mtk_text_block_size& sz);
	int		get_block_size(int& icode, mtk_text_block_size& sz);
	//-- 描画
	void	draw_string(const char* p);
	void	draw0(const mtk_text_block_info& root_blk);
	void	process_command(int idx);
	//-- 個々のコマンド
	void	set_color(const RGBAf& col);
	void	set_size(float size);
	void	set_relative_size(float rsize);
};





#endif