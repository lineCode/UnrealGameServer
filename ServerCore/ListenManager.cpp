#include "pch.h"
#include "ListenManager.h"
#include "IocpObject.h"
#include "IocpEventStorage.h"
#include "IocpManager.h"
#include "ServiceManager.h"
#include "Session.h"

/*------------------------------------------------------------------------------------------------
�̸�     : ListenManager::ListenManager
�뵵     : ListenManager�� ������
           Port ��ȣ�� ��⿭ �ο� iocp�� �Ŵ����� ����ϰ� �������Ͽ� �°� ���� �� �������� ���
������   : �̹α�
������¥ : 2022.08.16
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
�̸�     : ListenManager::StartAccept
�뵵     : �������Ͽ��� ���Ͽ� ������ �޴°��� �����ϴ� �Լ�
           Keeppernumber��ŭ AcceptStorage Ǯ���� ������
������¥ : 2022.08.16
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
�̸�     : ListenManager::Update
�뵵     : �������Ͽ��� Accept�� �� ��ü�� ������ �����ϱ� ���� �Լ�
������¥ : 2022.08.16
----------------------------------------------------------------------------------------------*/
void ListenManager::IocpUpdate(IocpEventStorage* iocpstorage, int32 numOfByte)
{
	CRASH_IF(iocpstorage->_Type != IocpType::Accept);

	AcceptEventStorage* accept = static_cast<AcceptEventStorage*>(iocpstorage);
	ProcessAccept(accept);
}

/*---------------------------------------------------------------------------------------------
�̸�     : ListenManager::RegisterAccept
�뵵     : AcceptEx�� ������ ������ ����ϴ� �Լ�
������¥ : 2022.08.16
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
�̸�     : ListenManager::ProcessAccept
�뵵     : Accept�� �� ���ǿ� ���� �Ӽ��� �ּҸ� �������ְ� Ŭ���̾�Ʈ�� ����� �������ִ� �Լ�
������¥ : 2022.08.16
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
