#include "pch.h"
#include "ListenManager.h"
#include "IocpObject.h"
#include "IocpEventStorage.h"
#include "IocpManager.h"
#include "ServiceManager.h"
#include "Session.h"

/*------------------------------------------------------------------------------------------------
이름     : ListenManager::ListenManager
용도     : ListenManager의 생성자
           Port 번호와 대기열 인원 iocp의 매니저를 등록하고 리슨소켓에 맞게 설정 후 리슨소켓 등록
수정자   : 이민규
수정날짜 : 2022.08.16
-------------------------------------------------------------------------------------------------*/
ListenManager::ListenManager(shared_ptr<ServerManager> servermanager) : _ServerManager(servermanager)
{
	_socket = SocketUtils::CreateSocket();

	CRASH_IF(SocketUtils::SocketGameSetting(_socket) == false);
	CRASH_IF(SocketUtils::DefaultBind(_socket, _ServerManager->GetNetAddress().GetPort()) == false);
	CRASH_IF(SocketUtils::Listen(_socket, 1000) == false);

	_ServerManager->GetIocpManager()->Register(GetSocketHandle());
}

ListenManager::~ListenManager()
{
	for (auto * accept : _GateKeepers)
		Gdelete(accept);
}

/*---------------------------------------------------------------------------------------------
이름     : ListenManager::StartAccept
용도     : 리슨소켓에서 소켓에 연결을 받는것을 시작하는 함수
           Keeppernumber만큼 AcceptStorage 풀링을 생성함
수정날짜 : 2022.08.16
----------------------------------------------------------------------------------------------*/
bool ListenManager::StartAccept(int32 keepernumber)
{
	for(int32 i = 0 ; i< keepernumber; i++)
	{
		AcceptEventStorage* GateKeepre = Gnew<AcceptEventStorage>();
		GateKeepre->_Object = shared_from_this();
		_GateKeepers.push_back(GateKeepre);
		RegisterAccept(GateKeepre);
	}

	return true;
}

/*---------------------------------------------------------------------------------------------
이름     : ListenManager::Update
용도     : 리슨소켓에서 Accept가 된 객체에 다음을 진행하기 위한 함수
수정날짜 : 2022.08.16
----------------------------------------------------------------------------------------------*/
void ListenManager::IocpUpdate(IocpEventStorage* iocpstorage, int32 numOfByte)
{
	CRASH_IF(iocpstorage->_Type != IocpType::Accept);

	AcceptEventStorage* accept = static_cast<AcceptEventStorage*>(iocpstorage);
	ProcessAccept(accept);
}

/*---------------------------------------------------------------------------------------------
이름     : ListenManager::RegisterAccept
용도     : AcceptEx에 세션의 소켓을 등록하는 함수
수정날짜 : 2022.08.16
----------------------------------------------------------------------------------------------*/
void ListenManager::RegisterAccept(AcceptEventStorage* gatekeeper)
{
	DWORD bytesReceived = 0;

	shared_ptr<Session> session = _ServerManager->CreateSession();

	gatekeeper->Init();
	gatekeeper->_session = session;

	if(false == SocketUtils::AcceptEx(_socket ,session->GetSocket() , session->_AcceptBuffer , 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT & bytesReceived, static_cast<LPOVERLAPPED>(gatekeeper)))
	{
		const int32 ErrorCode = ::WSAGetLastError();
		if (ErrorCode != WSA_IO_PENDING)
			RegisterAccept(gatekeeper);
	}
}

/*--------------------------------------------------------------------------------------------------
이름     : ListenManager::ProcessAccept
용도     : Accept가 된 세션에 서버 속성과 주소를 설정해주고 클라이언트와 통신을 설정해주는 함수
수정날짜 : 2022.08.16
---------------------------------------------------------------------------------------------------*/
void ListenManager::ProcessAccept(AcceptEventStorage* gatekeeper)
{
	shared_ptr<Session> session = gatekeeper->_session;

	if(false == SocketUtils::SetUpdateAcceptSocket(session->GetSocket(), _socket))
	{
		RegisterAccept(gatekeeper);
		return;
	}

	SOCKADDR_IN SockAddress;
	int32 SizeofSockAddr = sizeof(SockAddress);

	if(SOCKET_ERROR ==::getpeername(session->GetSocket() , reinterpret_cast<SOCKADDR*>(&SockAddress) ,&SizeofSockAddr))
	{
		RegisterAccept(gatekeeper);
		return;
	}

	session->SetMyAddress(NetAddress(SockAddress));
	session->ProcessConnect();

	RegisterAccept(gatekeeper);
}
