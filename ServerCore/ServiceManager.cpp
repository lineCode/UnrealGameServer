#include "pch.h"
#include "ServiceManager.h"
#include "IocpManager.h"
#include "Session.h"
#include "ListenManager.h"

ServiceManager::ServiceManager(ServiceType type, NetAddress address, shared_ptr<IocpManager> iocpmanager,SessionMakeFunction func, int32 sessioncount)
: _type(type) , _NetAddress(address) , _IocpManger(iocpmanager) , _SessionMakeFunction(func) , _CSSettingCount(sessioncount)
{
}

ServiceManager::~ServiceManager()
{
	
}

void ServiceManager::Stop()
{
}

/*---------------------------------------------------------------------------------------------
이름     : ServiceManager::CreateSession
용도     : Session을 Shared 형태로 생성하고 IOCP에 등록하고 반환해주는 함수
수정자   : 이민규
수정날짜 : 2022.08.19
----------------------------------------------------------------------------------------------*/
shared_ptr<Session> ServiceManager::CreateSession()
{
	shared_ptr<Session> session = _SessionMakeFunction();
	session->SetServiceManager(shared_from_this());

	if (_IocpManger->Register(session->GetSocketHandle()) == false)
		return nullptr;

	return session;
}

/*---------------------------------------------------------------------------------------------
이름     : ServiceManager::AddSession
용도     : _SessionBuffer에 Session을 저장하는 함수
수정자   : 이민규
수정날짜 : 2022.08.19
----------------------------------------------------------------------------------------------*/
void ServiceManager::AddSession(shared_ptr<Session> session)
{
	WRITELOCK;
	_Sessionbuffer.insert(session);
	
}

/*---------------------------------------------------------------------------------------------
이름     : ServiceManager::ReleaseSession
용도     : _SessionBuffer에 Session을 제외하는 함수
수정자   : 이민규
수정날짜 : 2022.08.19
----------------------------------------------------------------------------------------------*/
void ServiceManager::ReleaseSession(shared_ptr<Session> session)
{
	WRITELOCK;
	CRASH_IF(_Sessionbuffer.erase(session) == 0);
}




ServerManager::ServerManager(NetAddress address, shared_ptr<IocpManager> iocpmanager, SessionMakeFunction func,int32 listenkeepcount)
	:ServiceManager(ServiceType::Server , address , iocpmanager ,func ,listenkeepcount)
{
}


/*---------------------------------------------------------------------------------------------
이름     : ServerManager::Start
용도     : 서버를 시작하는 함수
수정자   : 이민규
수정날짜 : 2022.08.19
----------------------------------------------------------------------------------------------*/
bool ServerManager::Start()
{
	_listenManager = GMakeShared<ListenManager>(static_pointer_cast<ServerManager>(shared_from_this()));
	CRASH_IF(_listenManager == nullptr);

	CRASH_IF(_listenManager->StartAccept(GetListenKeeprCount()) == false);

	return true;
}

void ServerManager::Stop()
{
	ServiceManager::Stop();
}




ClientManager::ClientManager(NetAddress address, shared_ptr<IocpManager> iocpmanager, SessionMakeFunction func,int32 clientsessioncount)
	:ServiceManager(ServiceType::Client , address , iocpmanager , func , clientsessioncount)
{
}

/*---------------------------------------------------------------------------------------------
이름     : ClientManager::Start
용도     : 클라이언트를 시작하는 함수
수정자   : 이민규
수정날짜 : 2022.08.19
----------------------------------------------------------------------------------------------*/
bool ClientManager::Start()
{
	for(int32 i =  0; i < _CSSettingCount; i++)
	{
		shared_ptr<Session> session = CreateSession();
		if (session->Connect() == false)
			return false;
	}

	return true;
}

void ClientManager::Stop()
{
	ServiceManager::Stop();
}

