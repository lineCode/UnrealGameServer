#pragma once

/*---------------------------------------------------------------------------------------------
�̸�     : NetAddress
�뵵     : Network Address�� ���� ���� �����ϰ� �����ϴ� ��ü
������   : �̹α�
������¥ : 2022.08.13
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

