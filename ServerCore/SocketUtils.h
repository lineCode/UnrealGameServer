#pragma once

/*---------------------------------------------------------------------------------------------
이름     : SocketUtils
용도     : 소켓에 관한 모든 기능을 관리하는 객체
수정자   : 이민규
수정날짜 : 2022.08.13
----------------------------------------------------------------------------------------------*/
class SocketUtils
{
public:
	static void Init();
	static void Clear();

	static SOCKET CreateSocket(); 
	static bool ExFunctionSetting(SOCKET socket , GUID  guid , LPVOID * exfunc);
	static void CloseSocket(SOCKET& socket);

	template<typename T>
	static bool SetSocketOption(SOCKET socket, int32 level, int32 optname, T optval);
	static bool SetLinger(SOCKET socket, uint16 onoff, uint16 linger);
	static bool SetReuseAddress(SOCKET socket, bool flag);
	static bool SetTcpNoDelay(SOCKET socket, bool flag);
	static bool SetUpdateAcceptSocket(SOCKET acceptsocket, SOCKET listensocket);
	static bool SetRecvBufferSize(SOCKET socket, int32 size);
	static bool SetSendBufferSize(SOCKET socket, int32 size);
	static bool SocketGameSetting(SOCKET socket);

	static bool Bind(SOCKET socket, GWString ip, uint16 port);
	static bool DefaultBind(SOCKET socket, uint16 port);
	static bool Listen(SOCKET socket, int32 backlog = SOMAXCONN);

public:
	static LPFN_CONNECTEX ConnectEx;
	static LPFN_DISCONNECTEX DisConnectEx;
	static LPFN_ACCEPTEX AcceptEx;
};

/*---------------------------------------------------------------------------------------------
이름     : SocketUtils::SetSocketOption
용도     : setsockopt 함수를 편하게 사용하는 함수
수정자   : 이민규
수정날짜 : 2022.08.13
----------------------------------------------------------------------------------------------*/
template <typename T>
bool SocketUtils::SetSocketOption(SOCKET socket, int32 level, int32 optname, T optval)
{
	return SOCKET_ERROR != setsockopt(socket, level, optname , reinterpret_cast<char*>(&optval), sizeof(T));
}



