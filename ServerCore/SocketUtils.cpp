#include "pch.h"
#include "SocketUtils.h"
#include "NetAddress.h"

LPFN_CONNECTEX SocketUtils::ConnectEx = nullptr;
LPFN_DISCONNECTEX SocketUtils::DisConnectEx = nullptr;
LPFN_ACCEPTEX SocketUtils::AcceptEx = nullptr;

/*---------------------------------------------------------------------------------------------
�̸�     : SocketUtils::Init
�뵵     : SocketUtil�� �ʱⰪ�� �����ϴ� �Լ�
           WindowServer���� 2.2 ���� , ExConnect ExDisConnect ExAccept ����
������   : �̹α�
������¥ : 2022.08.13
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
�̸�     : SocketUtils::Clear
�뵵     : ������ WSADATA�� �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.13
----------------------------------------------------------------------------------------------*/
void SocketUtils::Clear()
{
	::WSACleanup();
}

/*---------------------------------------------------------------------------------------------
�̸�     : SocketUtils::CreateSocket
�뵵     : IPV4 , TCP , OVERLAPPED�� �����Ǿ��ִ� ������ ���� ���ִ� �Լ�
������   : �̹α�
������¥ : 2022.08.13
----------------------------------------------------------------------------------------------*/
SOCKET SocketUtils::CreateSocket()
{
	return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

/*------------------------------------------------------------------------------------------------------
�̸�     : SocketUtils::ExFunctionSetting
�뵵     : �񵿱� �Լ����� �����ϴ� �Լ�
           �񵿱� �Լ����� ���� ȣ�� �� ��� mswsock.dll���� �Ź� ȣ���Ͽ� ������尡 �߻��Ͽ�
           mswsock.dll�� �Լ������Ͱ��� �񵿱� �Լ��� ���� �����Ͽ� ������带 ���� ���� �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.13
--------------------------------------------------------------------------------------------------------*/
bool SocketUtils::ExFunctionSetting(SOCKET socket, GUID guid, LPVOID* exfunc)
{
	DWORD byte = 0;

	return SOCKET_ERROR != WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guid, sizeof(guid), exfunc, sizeof(*exfunc),
		&byte, NULL, NULL);
}

/*---------------------------------------------------------------------------------------------
�̸�     : SocketUtils::CloseSocket
�뵵     : ������ �ݰ� INVALID_SOCKET���� �������ִ� �Լ�
������   : �̹α�
������¥ : 2022.08.13
----------------------------------------------------------------------------------------------*/
void SocketUtils::CloseSocket(SOCKET& socket)
{
	if(socket != INVALID_SOCKET)
		::closesocket(socket);
	socket = INVALID_SOCKET;

}

/*---------------------------------------------------------------------------------------------
�̸�     : SocketUtils::SetLinger
�뵵     : ����� ���۵��� ���� �����͸� ��� ó������ �����ϴ� �Լ�
           onoff = 0 -> ���Ϲ��ۿ� �����ִ� ��� �����͸� ����
           onoff = 1 -> ���Ϲ��ۿ� �����ִ� ��� �����͸� ����
           Linger -> ������ �ð����� ����ϰ� �����ִ� ��� �����͸� ����
������   : �̹α�
������¥ : 2022.08.13
----------------------------------------------------------------------------------------------*/
bool SocketUtils::SetLinger(SOCKET socket, uint16 onoff, uint16 linger)
{
	LINGER option;
	option.l_onoff = onoff;
	option.l_linger = linger;

	return SetSocketOption(socket, SOL_SOCKET, SO_LINGER, option);
}

/*---------------------------------------------------------------------------------------------
�̸�     : SocketUtils::SetReuseAddress
�뵵     : Time-wait ���¿� �ִ� ������ PORT��ȣ�� �Ҵ� �����ϰ� ����� �Լ�
		   flag = true -> �� �Ҵ� ����
		   flat = false -> �� �Ҵ� �Ұ���
������   : �̹α�
������¥ : 2022.08.13
----------------------------------------------------------------------------------------------*/
bool SocketUtils::SetReuseAddress(SOCKET socket, bool flag)
{
	return SetSocketOption(socket, SOL_SOCKET, SO_REUSEADDR, flag);
}

/*---------------------------------------------------------------------------------------------
�̸�     : SocketUtils::SetTcpNoDelay
�뵵     : Nagle �˰����� ��� ���θ� �����ϴ� �Լ�
		   flag = true -> ��Ŷ���� �޴� ��� ������ ���� �ӵ��� ����(���ϻ��)
		   flag = false -> ���� ��Ŷ���� ��Ƽ� �Ѳ����� ������ ���� �ӵ��� ����(���ϰ���) 
������   : �̹α�
������¥ : 2022.08.13
----------------------------------------------------------------------------------------------*/
bool SocketUtils::SetTcpNoDelay(SOCKET socket, bool flag)
{
	return SetSocketOption(socket, SOL_SOCKET, TCP_NODELAY, flag);
}

/*---------------------------------------------------------------------------------------------
�̸�     : SocketUtils::SetUpdateAcceptSocket
�뵵     : ������ �Ӽ��� �ٸ� ���Ͽ��� �Ѱ��ִ� �Լ�
           Acceptex�� ������ ������ OVERLAPEED���� ������ �Ӽ��� ���� �ʾ� ����� ���� ���
������   : �̹α�
������¥ : 2022.08.13
----------------------------------------------------------------------------------------------*/
bool SocketUtils::SetUpdateAcceptSocket(SOCKET acceptsocket, SOCKET listensocket)
{
	return SetSocketOption(acceptsocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, listensocket);
}

/*---------------------------------------------------------------------------------------------
�̸�     : SocketUtils::SetRecvBufferSize
�뵵     : RecvBuffer�� ����� �����ϴ� �Լ� 
������   : �̹α�
������¥ : 2022.08.13
----------------------------------------------------------------------------------------------*/
bool SocketUtils::SetRecvBufferSize(SOCKET socket, int32 size)
{
	return SetSocketOption(socket, SOL_SOCKET, SO_RCVBUF, size);
}

/*---------------------------------------------------------------------------------------------
�̸�     : SocketUtils::SetSendBufferSize
�뵵     : SendBuffer�� ����� �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.13
----------------------------------------------------------------------------------------------*/
bool SocketUtils::SetSendBufferSize(SOCKET socket, int32 size)
{
	return SetSocketOption(socket, SOL_SOCKET, SO_SNDBUF, size);
}

/*---------------------------------------------------------------------------------------------
�̸�     : SocketUtils::SocketGameSetting
�뵵     : ������ Linger ReuseAddr Naggle�� ����ȯ�濡 �°� �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.13
----------------------------------------------------------------------------------------------*/
bool SocketUtils::SocketGameSetting(SOCKET socket)
{
	CRASH_IF(SetLinger(socket, 0, 0) == false);
	CRASH_IF(SetReuseAddress(socket, true) == false);
	CRASH_IF(SetTcpNoDelay(socket, true) == false);

	return true;
}

/*---------------------------------------------------------------------------------------------
�̸�     : SocketUtils::Bind
�뵵     : ���Ͽ� ip , ��Ʈ , ipv4�� �ִ� �Լ�
������   : �̹α�
������¥ : 2022.08.13
----------------------------------------------------------------------------------------------*/
bool SocketUtils::Bind(SOCKET socket, GWString ip , uint16 port)
{
	NetAddress address(ip, port);
	return SOCKET_ERROR != ::bind(socket, address.GetSockAddrptr(), sizeof(SOCKADDR_IN));
}

/*---------------------------------------------------------------------------------------------
�̸�     : SocketUtils::DefaultBind
�뵵     : ���Ͽ� INADDR_ANY�� IP , ��Ʈ , ipv4�� �ִ� �Լ�
������   : �̹α�
������¥ : 2022.08.13
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
�̸�     : SocketUtils::Listen
�뵵     : ������ ���� �������� �����ϰ� backlog��ŭ ��⿭�� �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.13
----------------------------------------------------------------------------------------------*/
bool SocketUtils::Listen(SOCKET socket, int32 backlog)
{
	return SOCKET_ERROR != ::listen(socket, backlog);
}
