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
//-------
//  TCP
//-------
#include  "../common/tcp_udp.h"
#include  <windows.h>
#include  <process.h>
#include  "mtk_tcp.h"

#include  "../common/frame_rate.h"
#include  "mtk_switches.h"
#include  "mtk_strings.h"




void	OnCommandNone();
void	OnTCPCommand(char cmd);
void	OnDataChar(char type, char val);
void	OnDataSwitch(char type, char idx, char val);
void	OnDataInt(int_packet *pintp);
void	OnDataFloat(float_packet *pflp);
void	OnDataDouble(double_packet *pdblp);
void	OnDataVector3f(vector3f_packet *pv3fp);
void	OnDataLONGLONG(LONGLONG_packet *pllp);
void	OnDataCustom(char* pData);
void	OnUpdateView(view_info* pVwif);
void	OnUpdateView2(view_info2* pVwif2);
void	OnAddMediaFile(int media_list_type, const media_file_info& mfi);
void	OnSetLocationInfo(const location& loc);




//--- TCP/IP ---
const int TCP_PORT_DEFAULT = 50004;
int		TCP_PORT = TCP_PORT_DEFAULT;		// デフォルト TCP ポート
char	szHostName[MAX_PATH];				// ホスト名
SOCKET  soc_ctrl = INVALID_SOCKET;			// コントローラ用のソケット		
SOCKET  soc_svr[svr_num_max];				// サーバ用のソケット
int		svr_num = 0;
char	szSvrName[svr_num_max][MAX_PATH];
int		svr_port[svr_num_max];
sockaddr_in  svr_addr[svr_num_max];
bool    bConnect = false;
bool	bCtrl;



//--- Critical section ---
static CRITICAL_SECTION cs;


//prototypes
void __cdecl	TCP_thread(void* pParam);



//--------
//  Util
//--------
int
pack_strings_data(char *buf, std::vector<std::string> vstr)
{
	int total_size = 0;

	char *p = buf;

	// number of strings
	int *pi = (int *)p;
	*pi = vstr.size();
	pi++;
	p = (char*)pi;
	total_size += sizeof(int);

	for (int i = 0; i < vstr.size(); i++) {
		const std::string& str = vstr[i];

		// size
		int sz = str.size();
		int *pi = (int *)p;
		*pi = sz;
		pi++;
		p = (char*)pi;
		total_size += sizeof(int);

		// string ('\0'は含まない)
		memcpy(p, str.c_str(), sz);
		total_size += sizeof(char) * sz;

		p += sz;
	}

	return total_size;
}


std::vector<std::string>
unpack_strings(const char* buf)
{
	std::vector<std::string> vstr;

	const char *p = buf;

	// number of strings
	int N;
	int *pi = (int *)p;
	N = *pi;
	pi++;
	p = (char*)pi;


	for (int i = 0; i < N; i++) {
		char str_buf[1024];

		// size
		int *pi = (int *)p;
		int sz = *pi;
		pi++;
		p = (char*)pi;

		// string (\0は含まない)
		memcpy(str_buf, p, sz);
		str_buf[sz] = '\0';
		p += sz;

		vstr.push_back(str_buf);
	}

	return vstr;
}


//--------
//  Init
//--------
inline  void
close_socket(SOCKET& soc)
{
    if (soc != INVALID_SOCKET) {
		closesocket(soc);
		soc = INVALID_SOCKET;
	}
}

void
close_winsock()
{
	if (bCtrl) {
		for (int i=0; i<svr_num; i++) {
			if (soc_svr[i] == INVALID_SOCKET)  continue;
			close_socket(soc_svr[i]);
			soc_svr[i] = INVALID_SOCKET;
		}
	} else {
		close_socket(soc_ctrl);
	}
	WSACleanup();

	DeleteCriticalSection(&cs);
}


bool
init_controler()
{
	bool bSocket[svr_num_max];
	for (int i=0; i<svr_num; ++i) {
		bSocket[i] = false;
	}
	// Connect to servers
	while (true) {
		bool flg = true;

		for (int i=0; i<svr_num; i++) {
			if (bSocket[i])  continue;
			flg = false;
			soc_svr[i] = init_TCP_client(szSvrName[i], svr_port[i]);
			if (soc_svr[i] == INVALID_SOCKET)  continue;
			const char on = 1;
			setsockopt( soc_svr[i], IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on) );
			bSocket[i] = true;
		}
		if (flg)  break;
		Sleep(100);
	}
	return  true;
}


bool
init_server()
{
	_beginthread(TCP_thread, 0, 0);
	return true;
}

bool
init_TCP(bool bControler)
{
	bCtrl = bControler;
	//-- Critical section
	InitializeCriticalSection(&cs);
	//--- init UDP ---
	init_winsock( close_winsock );
    if (bCtrl)		{ init_controler(); }
	else			{ init_server(); }
	//--- set host name ---
	gethostname(szHostName, 256);
	return true;
}




//-------------------------------
// パケットの構造
//-------------------------------
//*** 一般 ***
// Byte 0: Command ID
// Byte 1-: 各コマンドごとのデータ
//-------------------------------
//*** View frame ***
// Byte 0: Command ID
// Byte 1-: view_info 構造体
//-------------------------------
//*** Movie, Image ***
// Byte 0: Command ID
// Byte 1: インデックス
// Byte 2-: ファイル名
//-------------------------------

//==================
//  Thread for TCP
//==================
bool
process_TCP_command(SOCKET& soc)
{
	//-- buffer
	const int BUF_SIZE = 1024;
	static char	buf[BUF_SIZE];
	const void* pData = (void*)(&buf[1]);
	static char fn_dbg[256];


	bool first = true;
	fps_counter fps(1);
	fps.begin_frame();



	while (true) {
		int len, rcv_len;
		//-- Read data size
		rcv_len = readn_TCP(soc, (char*)&len, sizeof(len));
		if (rcv_len == SOCKET_ERROR || rcv_len == 0)	return true;
		//--  Read data
		rcv_len = readn_TCP(soc, buf, len);
		if (rcv_len == SOCKET_ERROR || rcv_len == 0)	return true;


		/*
		//if (bDebug) {
		if (true) {
			FILE *fp;

			if (first) {
				int i = 0;
				
				
				while (true) {
					sprintf(fn_dbg, "tcp_log_%d.dat", i);
					if ((fp = fopen(fn_dbg, "rt")) == NULL) {
						break;
					}
					fclose(fp);
					i++;
				}

				fp = fopen(fn_dbg, "wt");
				first = false;
			}
			else {
				fp = fopen(fn_dbg, "at");
			}
			fprintf(fp, "%d", buf[0]);
			if (buf[0] == CMD_COMMAND) {
				fprintf(fp, " %d", *((char*)pData));
			}
			else {
				fprintf(fp, " -");
			}
			fprintf(fp, "\n");
			fclose(fp);
		}
		*/


		char cmd = buf[0];
		switch (cmd) {
			case CMD_COMMAND:
				{
					char ch = *((char*)pData);
					OnTCPCommand(ch);
					//--- 終了コマンドの場合 ---
					if (ch == COMMAND_EXIT_ALL) {
						//****
						CoUninitialize();	//---   DirectX を解放する
						return  false;
					}
				}
				break;
			case CMD_DATA_CHAR:
				{
					char_packet*  pChp = (char_packet*)pData;
					OnDataChar(pChp->type, pChp->val);
				}
				break;
			case CMD_DATA_SWITCH:
				{
					switch_packet* pSwp = (switch_packet*)pData;
					OnDataSwitch(pSwp->type, pSwp->idx, pSwp->val);
				}
				break;
			case CMD_DATA_INT:
				{
					OnDataInt((int_packet*)pData);
				}
				break;
			case CMD_DATA_FLOAT:
				{
					OnDataFloat((float_packet*)pData);
				}
				break;
			case CMD_DATA_DOUBLE:
				{
					OnDataDouble((double_packet*)pData);
				}
				break;
			case CMD_DATA_VECTOR3F:
				{
					OnDataVector3f((vector3f_packet*)pData);
				}
				break;
			case CMD_DATA_LONGLONG:
				{
					OnDataLONGLONG((LONGLONG_packet*)pData);
				}
				break;
			case CMD_DATA_CUSTOM:
				{
					OnDataCustom((char*)pData);
				}
				break;

			case CMD_UPDATE_VIEW:
				OnUpdateView((view_info*)pData);
				break;
			case CMD_UPDATE_VIEW_2:
				OnUpdateView2((view_info2*)pData);
				break;
			case CMD_ADD_MEDIA_FILE:
				{
					int media_list_type;
					media_file_info mfi;

					media_list_type = *((char*)pData);
					mfi.type = *((char*)pData + 1);

					char *p_str = ((char*)pData) + 2;
					std::vector<std::string> vstr = unpack_strings(p_str);

					mfi.file_path = vstr[0];
					mfi.menu_title = vstr[1];
					mfi.disp_title = vstr[2];

					OnAddMediaFile(media_list_type, mfi);
				}
				break;
			case CMD_LOCATION_INFO:
				{
					location loc;
					char str[512+1];

					float *pf = (float*)pData;
					loc.lon = *pf;
					pf++;
					
					loc.lat = *pf;
					pf++;

					int *pi = (int*)pf;
					int sz = *pi;
					pi++;

					if (sz > 512) {
						sz = 512;
					}

					char *p = (char*)pi;
					memcpy(str, p, sz);
					str[sz] = '\0';
					loc.name = str;

					OnSetLocationInfo(loc);
				}
				break;
			default:
				OnCommandNone();
				break;
		}
	}
	return  true;
}






void __cdecl
TCP_thread(void* pParam)
{
	//***
	CoInitialize(NULL);		//---  DirectShowのために必要
	soc_ctrl = init_TCP_server(TCP_PORT);
	if (soc_ctrl == INVALID_SOCKET)  return;

	// 接続要求を受け付ける
	listen(soc_ctrl, SOMAXCONN);


	// 接続処理ループ
	while (true) {
		SOCKET soc;
		struct sockaddr_in peer;
		int peerlen = sizeof(peer);
		memset(&peer, 0, peerlen);

		soc = accept(soc_ctrl, (struct sockaddr*)&peer, &peerlen);
		if (soc == INVALID_SOCKET)  continue;

		const char on = 1;
		setsockopt( soc, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on) );
		bConnect = true;
		bool res = process_TCP_command(soc);
		closesocket(soc);
		soc = INVALID_SOCKET;
		bConnect = false;
		if (!res)  break;
		OnTCPCommand(COMMAND_DRAW_HIGHRES);		// 画面を更新する
	}
	_endthread();
}



//-------------------
//  Send TCP packet
//-------------------
bool
send_data_to(SOCKET& soc, const char* buf, const int buf_size)
{
	if (soc == INVALID_SOCKET)  return  false;

	int len;
	// データサイズを送る
	len = send(soc, (char*)&buf_size, sizeof(buf_size), 0);
	if (len==SOCKET_ERROR || len!=sizeof(buf_size))  return  false;
	
	// データ本体を送る
	len = send(soc, buf, buf_size, 0);
	if (len==SOCKET_ERROR || len!=buf_size)  return  false;

	return  true;
}

bool
send_data_to_all(const char* buf, const int buf_size)
{
	bool res = true;
	for (int i=0; i<svr_num; i++) {
		if ( !send_data_to(soc_svr[i], buf, buf_size) )  { res = false; }
	}
	return  res;
}



//----------------
//  Send Command
//----------------
bool
send_command(char cmd)
{
	const int buf_size = 2;
	char buf[buf_size];

	EnterCriticalSection(&cs);
		buf[0] = CMD_COMMAND;
		buf[1] = cmd;
		bool res = send_data_to_all(buf, buf_size);
	LeaveCriticalSection(&cs);

	return res;
}


//-------------
//  Send data
//-------------
bool
send_data_char(char type, char val)
{
	const int buf_size = 1 + sizeof(char_packet);
	char buf[buf_size];
	char_packet* const pCharp = (char_packet*)&buf[1];
	EnterCriticalSection(&cs);
		buf[0] = CMD_DATA_CHAR;
		pCharp->type = type;
		pCharp->val  = val;
		bool res = send_data_to_all(buf, buf_size);
	LeaveCriticalSection(&cs);
	return res;
}


bool
send_data_switch(char type, char idx, char val)
{
	const int buf_size = 1 + sizeof(switch_packet);
	char buf[buf_size];
	switch_packet* const pSwp = (switch_packet*)&buf[1];
	EnterCriticalSection(&cs);
		buf[0] = CMD_DATA_SWITCH;
		pSwp->type = type;
		pSwp->idx  = idx;
		pSwp->val  = val;

		bool res = send_data_to_all(buf, buf_size);
	LeaveCriticalSection(&cs);
	return res;
}


bool
send_data_int(char type, int val)
{
	const int buf_size = 1 + sizeof(int_packet);
	char buf[buf_size];
	int_packet* const pIntp = (int_packet*)&buf[1];
	EnterCriticalSection(&cs);
		buf[0] = CMD_DATA_INT;
		pIntp->type = type;
		pIntp->val  = val;
		bool res = send_data_to_all(buf, buf_size);
	LeaveCriticalSection(&cs);
	return  res;
}


bool
send_data_float(char type, float val)
{
	const int buf_size = 1 + sizeof(float_packet);
	char buf[buf_size];
	float_packet* const pFloatp = (float_packet*)&buf[1];
	EnterCriticalSection(&cs);
		buf[0] = CMD_DATA_FLOAT;
		pFloatp->type = type;
		pFloatp->val  = val;
		bool res = send_data_to_all(buf, buf_size);
	LeaveCriticalSection(&cs);
	return  res;
}


bool
send_data_double(char type, const double& val)
{
	const int buf_size = 1 + sizeof(double_packet);
	char buf[buf_size];
	double_packet* const pDoublep = (double_packet*)&buf[1];
	EnterCriticalSection(&cs);
		buf[0] = CMD_DATA_DOUBLE;
		pDoublep->type = type;
		pDoublep->val  = val;
		bool res = send_data_to_all(buf, buf_size);
	LeaveCriticalSection(&cs);
	return  res;
}


bool
send_data_vector3f(char type, const vector3f& val)
{
	const int buf_size = 1 + sizeof(vector3f_packet);
	char buf[buf_size];
	vector3f_packet* const pVector3fp = (vector3f_packet*)&buf[1];
	EnterCriticalSection(&cs);
		buf[0] = CMD_DATA_VECTOR3F;
		pVector3fp->type = type;
		pVector3fp->val  = val;
		bool res = send_data_to_all(buf, buf_size);
	LeaveCriticalSection(&cs);
	return  res;
}


bool
send_data_LONGLONG(char type, const LONGLONG& val)
{
	const int buf_size = 1 + sizeof(LONGLONG_packet);
	char buf[buf_size];
	LONGLONG_packet* const pLONGLONGp = (LONGLONG_packet*)&buf[1];

	EnterCriticalSection(&cs);
		buf[0] = CMD_DATA_LONGLONG;
		pLONGLONGp->type = type;
		pLONGLONGp->val  = val;
		bool res = send_data_to_all(buf, buf_size);
	LeaveCriticalSection(&cs);
	return res;
}

bool
send_data_custom(char type, const char* buf0, int size)
{
	const int buf_size = 2 + size;
	char buf[100];


	EnterCriticalSection(&cs);
		buf[0] = CMD_DATA_CUSTOM;
		buf[1] = type;
		memcpy(buf+2, buf0, size);
		bool res = send_data_to_all(buf, buf_size);
	LeaveCriticalSection(&cs);
	return res;
}


//---------
//  Frame
//---------
bool
send_frame(const view_info& vwif)
{
	const int buf_size = 1 + sizeof(view_info);
	char buf[buf_size];
	view_info* const pViewInfo = (view_info*)&buf[1];
	EnterCriticalSection(&cs);
		buf[0] = CMD_UPDATE_VIEW;
		*pViewInfo = vwif;
		bool res = send_data_to_all(buf, buf_size);
	LeaveCriticalSection(&cs);
	return  res;
}


bool
send_frame2(const view_info2& vwif2)
{
	const int buf_size = 1 + sizeof(view_info2);
	char buf[buf_size];
	view_info2* const pViewInfo2 = (view_info2*)&buf[1];
	EnterCriticalSection(&cs);
		buf[0] = CMD_UPDATE_VIEW_2;
		*pViewInfo2 = vwif2;
		bool res = send_data_to_all(buf, buf_size);
	LeaveCriticalSection(&cs);
	return  res;
}


//---------
//  Media
//---------
void
send_media_info(const MediaFiles& mdf, char media_list_type)
{
	if (svr_num <= 0)  return;
	if (media_list_type != MEDIATYPE_IMAGE && media_list_type != MEDIATYPE_MOVIE && media_list_type != MEDIATYPE_AUDIO) {
		return;
	}


	const int buf_size = 3 + 2048;
	char buf[buf_size];
	


	EnterCriticalSection(&cs);

	// clear media
	send_data_char(DATA_CHAR_CLEAR_MEDIA_FILES, media_list_type);


	buf[0] = CMD_ADD_MEDIA_FILE;	// command
	for (int idx = 0; idx < mdf.size(); idx++) {
		const media_file_info& mfi = mdf[idx];


		buf[1] = media_list_type;	// media list type
		buf[2] = mfi.type;			// mediatype

		for (int i = 0; i < svr_num; i++) {
			// file path, menu title, and disp title
			std::vector<std::string> vstr;
			vstr.push_back(mfi.all_file_path[i]);
			vstr.push_back(mfi.menu_title);
			vstr.push_back(mfi.disp_title);

			// send
			int data_len;
			data_len = 3 + pack_strings_data(&buf[3], vstr);
			send_data_to(soc_svr[i], buf, data_len);
		}
	}

	LeaveCriticalSection(&cs);

}




//------------
//  Location
//------------
void
send_location_info(const location& loc)
{
	if (svr_num <= 0)  return;


	const int buf_size = 3 + 512;
	char buf[buf_size];



	EnterCriticalSection(&cs);


	char *p;
	float *pf;
	int *pi;

	int data_len = 0;
	buf[0] = CMD_LOCATION_INFO;	// command
	data_len++;

	p = &buf[1];
	pf = (float*)p;
	
	*pf = loc.lon;
	pf++;
	data_len += sizeof(float);

	*pf = loc.lat;
	pf++;
	data_len += sizeof(float);

	int sz = loc.name.size();
	pi = (int*)pf;
	*pi = sz;
	pi++;
	data_len += sizeof(int);

	// string ('\0'は含まない)
	p = (char*)pi;
	memcpy(p, loc.name.c_str(), sz);
	data_len += sz;

	for (int i = 0; i < svr_num; i++) {
		send_data_to(soc_svr[i], buf, data_len);
	}


	LeaveCriticalSection(&cs);
}
