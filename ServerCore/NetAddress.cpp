#include "pch.h"
#include "NetAddress.h"

NetAddress::NetAddress(SOCKADDR_IN sockaddr) :_SockAddr(sockaddr)
{
}

/*---------------------------------------------------------------------------------------------
이름     : NetAddress::NetAddress
용도     : SOCKADDR_IN 을 IPV4와 ip port를 설정하는 부분
수정자   : 이민규
수정날짜 : 2022.08.13
----------------------------------------------------------------------------------------------*/
NetAddress::NetAddress(GWString ip, uint16 port)
{
	::memset(&_SockAddr, 0, sizeof(_SockAddr));
	_SockAddr.sin_family = AF_INET;
	_SockAddr.sin_addr = IpToAddress(ip.c_str());
	_SockAddr.sin_port = htons(port);
}

/*---------------------------------------------------------------------------------------------
이름     : NetAddress::GetSockAddrptr
용도     : 저장되어 있는 SOCKADDR_IN값 SOCKAADR*형태로 반환해주는 함수
수정자   : 이민규
수정날짜 : 2022.08.19
----------------------------------------------------------------------------------------------*/
SOCKADDR* NetAddress::GetSockAddrptr()
{
	return reinterpret_cast<SOCKADDR*>(&_SockAddr);
}

/*---------------------------------------------------------------------------------------------
이름     : NetAddress::GetipAddress
용도     : _SOCKADDR_IN에 저장되어 있는 ip를 wstring형으로 변경 후 반환해주는 함수
수정자   : 이민규
수정날짜 : 2022.08.13
----------------------------------------------------------------------------------------------*/
GWString NetAddress::GetipAddress()
{
	WCHAR buffer[100];
	::InetNtopW(AF_INET, &_SockAddr.sin_addr, buffer, sizeof(buffer) / sizeof(buffer[0]));

	return GWString(buffer);
}

/*---------------------------------------------------------------------------------------------
이름     : NetAddress::IpToAddress
용도     : ip를 넣을 경우 NetWork의 표준으로 변경 후 IN_ADDR형으로 반환하는 함수
수정자   : 이민규
수정날짜 : 2022.08.13
----------------------------------------------------------------------------------------------*/
IN_ADDR NetAddress::IpToAddress(const WCHAR* ip)
{
	IN_ADDR address;
	::InetPtonW(AF_INET, ip, &address);
	return address;
}
