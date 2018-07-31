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
//---------------------------------
//  TCP_UDP.cpp
//   written by T.N.Kato  2002/6
//   2003/2/15
//---------------------------------
#include  "tcp_udp.h"



//==========
//  common
//==========
bool
init_winsock( void __cdecl close_func() )
{
	WSADATA wsaData;
	WORD version = MAKEWORD(2,0);

	if ( WSAStartup( version, &wsaData ) != 0)	return false;
	if ( version != wsaData.wVersion )			return false;
	if ( atexit( close_func ) != 0)				return false;
	
	return true;
}


struct hostent*
get_host_entry(const char* server_address_or_name)
{
	unsigned long  svraddr;
	struct hostent* hostentry;

	svraddr = inet_addr(server_address_or_name);
	if (svraddr != INADDR_NONE) {
		// 有効なIPアドレスの場合
		hostentry = gethostbyaddr((const char*)&svraddr, sizeof(svraddr), AF_INET);
	} else {
		// server_addres_or_name を名前とみなす
		hostentry = gethostbyname(server_address_or_name);
	}

	return  hostentry;
}



bool
get_server_address(const char* server_address_or_name, int server_port, struct sockaddr_in* svr)
{
	struct hostent* hostentry;
	hostentry = get_host_entry(server_address_or_name);
	if (hostentry == NULL)	return false;

	memset((void*)svr, 0, sizeof(sockaddr_in));
	svr->sin_family = AF_INET;														// TCP/IPを使う
	svr->sin_addr.S_un.S_addr = *((unsigned long*)(hostentry->h_addr_list[0]));		// サーバのアドレス
	svr->sin_port = htons(server_port);												// サーバのポート

	return  true;
}


bool
bind_inaddr_any(SOCKET soc, int port)
{
	struct  sockaddr_in	 socadr;

	memset((void*)&socadr, 0, sizeof(socadr));
	socadr.sin_family = AF_INET;
	socadr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	socadr.sin_port = htons( port );
	if ( bind(soc, (struct sockaddr *)&socadr, sizeof(socadr)) == SOCKET_ERROR )  return false;
	return  true;
}




//=======
//  TCP
//=======
SOCKET
init_TCP_server(int port)
{
	SOCKET soc;

	//-- Create TCP socket --
	if ( (soc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET )  return INVALID_SOCKET;

	//-- Set socket option --
	int opt = 1;
	if ( setsockopt(soc, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) != 0 )  {
		closesocket(soc);
		return INVALID_SOCKET;
	}
    
	//-- Bind with socket --
	if ( !bind_inaddr_any(soc, port) )  {
		closesocket(soc);
		return INVALID_SOCKET;
	}

	return  soc;
}


SOCKET
init_TCP_client(const char* server_name, int server_port)
{
	SOCKET  soc;
	struct sockaddr_in  svradr;

	//-- Create TCP socket --
	if ( (soc = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET )  return INVALID_SOCKET;

	//-- Connect to server --
	if ( !get_server_address(server_name, server_port, &svradr) )  {
		closesocket(soc);
		return INVALID_SOCKET;
	}
	if ( connect(soc, (struct sockaddr *)&svradr, sizeof(svradr)) == SOCKET_ERROR )  {
		closesocket(soc);
		return INVALID_SOCKET;
	}

	return soc;
}


//--- 固定長メッセージを読み取る ---
int
readn_TCP(SOCKET soc, char* buf, const size_t& len)
{
	int count;
	count = len;
	while (count > 0) {
		int rcvn = recv(soc, buf, count, 0);
		
		if (rcvn == SOCKET_ERROR || rcvn == 0) {
			return  rcvn;
		}
		buf += rcvn;
		count -= rcvn;
	}
	return len;
}


int
send_TCP_packet(SOCKET soc, char* buf, int size)
{
	char FAR* p = buf;
	int msg_size = size;
	int sent_bytes = 0;
	int len;
	len = send(soc, (const char FAR*)&size, sizeof(int), 0);
	if (len == SOCKET_ERROR)  return len;
	sent_bytes += len;
	while (size > 0) {
	   len = send(soc, p, msg_size, 0);
	   if (len == SOCKET_ERROR)	 return len;
	   if (len < msg_size && len > 0) msg_size = len;

	   p += len;
	   size -= len;
	   sent_bytes += len;
	}
	return sent_bytes;
}


int
recv_TCP_packet(SOCKET soc, char* buf)
{
	int len;
	//-- パケットサイズを読む
	if ( readn_TCP(soc, (char*)&len, sizeof(len)) < 0) return -1;
	//-- データ本体を読む
	return readn_TCP(soc, buf, len);
}


//=======
//  UDP
//=======
SOCKET
init_UDP_server(int port)
{
	SOCKET soc;
	//-- Create UDP socket --
	if ( (soc = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)  return INVALID_SOCKET;
	//-- Bind with socket --
	if ( !bind_inaddr_any(soc, port) )  {
		closesocket(soc);
		return INVALID_SOCKET;
	}
	return  soc;
}

SOCKET
init_UDP_client()
{
	SOCKET soc;
	//-- Create UDP socket --
	if ( (soc = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)  return INVALID_SOCKET;
	return soc;
}
