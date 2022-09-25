#pragma once

/*---------------------------------------------------------------------------------------------
이름     : NetAddress
용도     : Network Address에 관한 것을 저장하고 관리하는 객체
수정자   : 이민규
수정날짜 : 2022.08.13
----------------------------------------------------------------------------------------------*/
class NetAddress
{
public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN sockaddr);
	NetAddress(GWString ip, uint16 port);

	SOCKADDR* GetSockAddrptr();
	SOCKADDR_IN& GetSockAddr() { return _SockAddr; }
	GWString GetipAddress();
	uint16 GetPort() { return ::ntohs(_SockAddr.sin_port); }
	IN_ADDR IpToAddress(const WCHAR* ip);

private:
	SOCKADDR_IN _SockAddr = {};
};

