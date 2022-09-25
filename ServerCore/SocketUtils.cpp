#include "pch.h"
#include "SocketUtils.h"
#include "NetAddress.h"

LPFN_CONNECTEX SocketUtils::ConnectEx = nullptr;
LPFN_DISCONNECTEX SocketUtils::DisConnectEx = nullptr;
LPFN_ACCEPTEX SocketUtils::AcceptEx = nullptr;

/*---------------------------------------------------------------------------------------------
이름     : SocketUtils::Init
용도     : SocketUtil의 초기값을 설정하는 함수
           WindowServer버전 2.2 시작 , ExConnect ExDisConnect ExAccept 설정
수정자   : 이민규
수정날짜 : 2022.08.13
----------------------------------------------------------------------------------------------*/
void SocketUtils::Init()
{
	WSADATA wsadata;
	CRASH_IF(::WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)

	SOCKET dummySocket = CreateSocket();
	CRASH_IF(ExFunctionSetting(dummySocket, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&ConnectEx)) == false)
	CRASH_IF(ExFunctionSetting(dummySocket, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisConnectEx)) == false)
	CRASH_IF(ExFunctionSetting(dummySocket, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx)) == false)
	closesocket(dummySocket);
;}

/*---------------------------------------------------------------------------------------------
이름     : SocketUtils::Clear
용도     : 윈도우 WSADATA를 종료하는 함수
수정자   : 이민규
수정날짜 : 2022.08.13
----------------------------------------------------------------------------------------------*/
void SocketUtils::Clear()
{
	::WSACleanup();
}

/*---------------------------------------------------------------------------------------------
이름     : SocketUtils::CreateSocket
용도     : IPV4 , TCP , OVERLAPPED가 설정되어있는 소켓을 생성 해주는 함수
수정자   : 이민규
수정날짜 : 2022.08.13
----------------------------------------------------------------------------------------------*/
SOCKET SocketUtils::CreateSocket()
{
	return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

/*------------------------------------------------------------------------------------------------------
이름     : SocketUtils::ExFunctionSetting
용도     : 비동기 함수들을 설정하는 함수
           비동기 함수들을 직접 호출 할 경우 mswsock.dll에서 매번 호출하여 오버헤드가 발생하여
           mswsock.dll에 함수포인터값을 비동기 함수에 직접 설정하여 오버헤드를 막기 위해 설정하는 함수
수정자   : 이민규
수정날짜 : 2022.08.13
--------------------------------------------------------------------------------------------------------*/
bool SocketUtils::ExFunctionSetting(SOCKET socket, GUID guid, LPVOID* exfunc)
{
	DWORD byte = 0;

	return SOCKET_ERROR != WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guid, sizeof(guid), exfunc, sizeof(*exfunc),
		&byte, NULL, NULL);
}

/*---------------------------------------------------------------------------------------------
이름     : SocketUtils::CloseSocket
용도     : 소켓을 닫고 INVALID_SOCKET으로 설정해주는 함수
수정자   : 이민규
수정날짜 : 2022.08.13
----------------------------------------------------------------------------------------------*/
void SocketUtils::CloseSocket(SOCKET& socket)
{
	if(socket != INVALID_SOCKET)
		::closesocket(socket);
	socket = INVALID_SOCKET;

}

/*---------------------------------------------------------------------------------------------
이름     : SocketUtils::SetLinger
용도     : 종료시 전송되지 않은 데이터를 어떻게 처리할지 결정하는 함수
           onoff = 0 -> 소켓버퍼에 남아있는 모든 데이터를 전송
           onoff = 1 -> 소켓버퍼에 남아있는 모든 데이터를 버림
           Linger -> 지정한 시간동안 대기하고 남아있는 모든 데이터를 버림
수정자   : 이민규
수정날짜 : 2022.08.13
----------------------------------------------------------------------------------------------*/
bool SocketUtils::SetLinger(SOCKET socket, uint16 onoff, uint16 linger)
{
	LINGER option;
	option.l_onoff = onoff;
	option.l_linger = linger;

	return SetSocketOption(socket, SOL_SOCKET, SO_LINGER, option);
}

/*---------------------------------------------------------------------------------------------
이름     : SocketUtils::SetReuseAddress
용도     : Time-wait 상태에 있는 소켓의 PORT번호를 할당 가능하게 만드는 함수
		   flag = true -> 재 할당 가능
		   flat = false -> 재 할당 불가능
수정자   : 이민규
수정날짜 : 2022.08.13
----------------------------------------------------------------------------------------------*/
bool SocketUtils::SetReuseAddress(SOCKET socket, bool flag)
{
	return SetSocketOption(socket, SOL_SOCKET, SO_REUSEADDR, flag);
}

/*---------------------------------------------------------------------------------------------
이름     : SocketUtils::SetTcpNoDelay
용도     : Nagle 알고리즘의 사용 여부를 결정하는 함수
		   flag = true -> 패킷들을 받는 즉시 보내서 전송 속도가 빠름(부하상승)
		   flag = false -> 작은 패킷들을 모아서 한꺼번에 보내서 전송 속도가 느림(부하감소) 
수정자   : 이민규
수정날짜 : 2022.08.13
----------------------------------------------------------------------------------------------*/
bool SocketUtils::SetTcpNoDelay(SOCKET socket, bool flag)
{
	return SetSocketOption(socket, SOL_SOCKET, TCP_NODELAY, flag);
}

/*---------------------------------------------------------------------------------------------
이름     : SocketUtils::SetUpdateAcceptSocket
용도     : 소켓의 속성을 다른 소켓에게 넘겨주는 함수
           Acceptex로 생성된 소켓은 OVERLAPEED같은 서버의 속성을 받지 않아 상속을 위해 사용
수정자   : 이민규
수정날짜 : 2022.08.13
----------------------------------------------------------------------------------------------*/
bool SocketUtils::SetUpdateAcceptSocket(SOCKET acceptsocket, SOCKET listensocket)
{
	return SetSocketOption(acceptsocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, listensocket);
}

/*---------------------------------------------------------------------------------------------
이름     : SocketUtils::SetRecvBufferSize
용도     : RecvBuffer의 사이즈를 결정하는 함수 
수정자   : 이민규
수정날짜 : 2022.08.13
----------------------------------------------------------------------------------------------*/
bool SocketUtils::SetRecvBufferSize(SOCKET socket, int32 size)
{
	return SetSocketOption(socket, SOL_SOCKET, SO_RCVBUF, size);
}

/*---------------------------------------------------------------------------------------------
이름     : SocketUtils::SetSendBufferSize
용도     : SendBuffer의 사이즈를 결정하는 함수
수정자   : 이민규
수정날짜 : 2022.08.13
----------------------------------------------------------------------------------------------*/
bool SocketUtils::SetSendBufferSize(SOCKET socket, int32 size)
{
	return SetSocketOption(socket, SOL_SOCKET, SO_SNDBUF, size);
}

/*---------------------------------------------------------------------------------------------
이름     : SocketUtils::SocketGameSetting
용도     : 소켓의 Linger ReuseAddr Naggle을 게임환경에 맞게 설정하는 함수
수정자   : 이민규
수정날짜 : 2022.08.13
----------------------------------------------------------------------------------------------*/
bool SocketUtils::SocketGameSetting(SOCKET socket)
{
	CRASH_IF(SetLinger(socket, 0, 0) == false);
	CRASH_IF(SetReuseAddress(socket, true) == false);
	CRASH_IF(SetTcpNoDelay(socket, true) == false);

	return true;
}

/*---------------------------------------------------------------------------------------------
이름     : SocketUtils::Bind
용도     : 소켓에 ip , 포트 , ipv4를 넣는 함수
수정자   : 이민규
수정날짜 : 2022.08.13
----------------------------------------------------------------------------------------------*/
bool SocketUtils::Bind(SOCKET socket, GWString ip , uint16 port)
{
	NetAddress address(ip, port);
	return SOCKET_ERROR != ::bind(socket, address.GetSockAddrptr(), sizeof(SOCKADDR_IN));
}

/*---------------------------------------------------------------------------------------------
이름     : SocketUtils::DefaultBind
용도     : 소켓에 INADDR_ANY로 IP , 포트 , ipv4를 넣는 함수
수정자   : 이민규
수정날짜 : 2022.08.13
----------------------------------------------------------------------------------------------*/
bool SocketUtils::DefaultBind(SOCKET socket, uint16 port)
{
	SOCKADDR_IN address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = ::htonl(INADDR_ANY);
	address.sin_port = ::htons(port);

	return SOCKET_ERROR != ::bind(socket, NetAddress(address).GetSockAddrptr(), sizeof(SOCKADDR_IN));
}

/*---------------------------------------------------------------------------------------------
이름     : SocketUtils::Listen
용도     : 소켓을 리슨 소켓으로 변경하고 backlog만큼 대기열을 설정하는 함수
수정자   : 이민규
수정날짜 : 2022.08.13
----------------------------------------------------------------------------------------------*/
bool SocketUtils::Listen(SOCKET socket, int32 backlog)
{
	return SOCKET_ERROR != ::listen(socket, backlog);
}
