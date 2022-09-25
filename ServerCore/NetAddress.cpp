#include "pch.h"
#include "NetAddress.h"

NetAddress::NetAddress(SOCKADDR_IN sockaddr) :_SockAddr(sockaddr)
{
}

/*---------------------------------------------------------------------------------------------
�̸�     : NetAddress::NetAddress
�뵵     : SOCKADDR_IN �� IPV4�� ip port�� �����ϴ� �κ�
������   : �̹α�
������¥ : 2022.08.13
----------------------------------------------------------------------------------------------*/
NetAddress::NetAddress(GWString ip, uint16 port)
{
	::memset(&_SockAddr, 0, sizeof(_SockAddr));
	_SockAddr.sin_family = AF_INET;
	_SockAddr.sin_addr = IpToAddress(ip.c_str());
	_SockAddr.sin_port = htons(port);
}

/*---------------------------------------------------------------------------------------------
�̸�     : NetAddress::GetSockAddrptr
�뵵     : ����Ǿ� �ִ� SOCKADDR_IN�� SOCKAADR*���·� ��ȯ���ִ� �Լ�
������   : �̹α�
������¥ : 2022.08.19
----------------------------------------------------------------------------------------------*/
SOCKADDR* NetAddress::GetSockAddrptr()
{
	return reinterpret_cast<SOCKADDR*>(&_SockAddr);
}

/*---------------------------------------------------------------------------------------------
�̸�     : NetAddress::GetipAddress
�뵵     : _SOCKADDR_IN�� ����Ǿ� �ִ� ip�� wstring������ ���� �� ��ȯ���ִ� �Լ�
������   : �̹α�
������¥ : 2022.08.13
----------------------------------------------------------------------------------------------*/
GWString NetAddress::GetipAddress()
{
	WCHAR buffer[100];
	::InetNtopW(AF_INET, &_SockAddr.sin_addr, buffer, sizeof(buffer) / sizeof(buffer[0]));

	return GWString(buffer);
}

/*---------------------------------------------------------------------------------------------
�̸�     : NetAddress::IpToAddress
�뵵     : ip�� ���� ��� NetWork�� ǥ������ ���� �� IN_ADDR������ ��ȯ�ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.13
----------------------------------------------------------------------------------------------*/
IN_ADDR NetAddress::IpToAddress(const WCHAR* ip)
{
	IN_ADDR address;
	::InetPtonW(AF_INET, ip, &address);
	return address;
}
