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
�̸�     : ServiceManager::CreateSession
�뵵     : Session�� Shared ���·� �����ϰ� IOCP�� ����ϰ� ��ȯ���ִ� �Լ�
������   : �̹α�
������¥ : 2022.08.19
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
�̸�     : ServiceManager::AddSession
�뵵     : _SessionBuffer�� Session�� �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.19
----------------------------------------------------------------------------------------------*/
void ServiceManager::AddSession(shared_ptr<Session> session)
{
	WRITELOCK;
	_Sessionbuffer.insert(session);
	
}

/*---------------------------------------------------------------------------------------------
�̸�     : ServiceManager::ReleaseSession
�뵵     : _SessionBuffer�� Session�� �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.19
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
�̸�     : ServerManager::Start
�뵵     : ������ �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.19
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
�̸�     : ClientManager::Start
�뵵     : Ŭ���̾�Ʈ�� �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.19
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

