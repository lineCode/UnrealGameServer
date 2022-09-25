#pragma once
#include <functional>
#include "NetAddress.h"

class Session;
class IocpManager;

/*---------------------------------------------------------------------------------------------
�̸�     : ServiceType
�뵵     : ���� Ÿ���� ������ �� ���� Enum Class
������   : �̹α�
������¥ : 2022.08.19
----------------------------------------------------------------------------------------------*/
enum class ServiceType : uint8
{
	Server,
	Client
};

using SessionMakeFunction = function<shared_ptr<Session>(void)>;

/*---------------------------------------------------------------------------------------------
�̸�     : ServiceManager
�뵵     : ��Ʈ��ũ ���� ��� ��� ������ ��� Session�� �����ϴ� ���� �� ��ü
������   : �̹α�
������¥ : 2022.08.19
----------------------------------------------------------------------------------------------*/
class ServiceManager : public enable_shared_from_this<ServiceManager>
{
public:
	ServiceManager(ServiceType type , NetAddress address , shared_ptr<IocpManager> iocpmanager , SessionMakeFunction func , int32 sessioncount);
	virtual ~ServiceManager();

	virtual bool Start() abstract;
	virtual void Stop();

	void SetSessionMakeFunction(SessionMakeFunction func) { _SessionMakeFunction = func; }

	shared_ptr<Session> CreateSession();
	void AddSession(shared_ptr<Session> session);
	void ReleaseSession(shared_ptr<Session> session);
	int32 GetCurrentSessionCount() { return _Sessionbuffer.size(); }
	int32 GetListenKeeprCount() { return _CSSettingCount; }

	ServiceType GetServiceType() { return _type; }
	NetAddress GetNetAddress() { return _NetAddress; }
	shared_ptr<IocpManager> GetIocpManager() { return _IocpManger; }

protected:
	RWLOCK_USE;
	ServiceType _type;
	NetAddress _NetAddress = {};
	shared_ptr<IocpManager> _IocpManger;

	Gset<shared_ptr<Session>> _Sessionbuffer;
	int32 _CSSettingCount = 0;
	SessionMakeFunction _SessionMakeFunction;

};

/*---------------------------------------------------------------------------------------------
�̸�     : ServerManager
�뵵     : ������ ���õ� ��Ʈ��ũ ���� ��� ��� ������ ��� Session�� �����ϴ� ���� �� ��ü
������   : �̹α�
������¥ : 2022.08.19
----------------------------------------------------------------------------------------------*/
class ServerManager : public ServiceManager
{
public:
	ServerManager(NetAddress address, shared_ptr<IocpManager> iocpmanager, SessionMakeFunction func, int32 listenkeepcount);
	virtual ~ServerManager() {};

	virtual bool Start();
	virtual void Stop();

private:
	shared_ptr<class ListenManager> _listenManager;
};

/*---------------------------------------------------------------------------------------------
�̸�     : ClientManager
�뵵     : Ŭ���̾�Ʈ�� ���õ� ��Ʈ��ũ ���� ��� ��� ������ ��� Session�� �����ϴ� ���� �� ��ü
������   : �̹α�
������¥ : 2022.08.19
----------------------------------------------------------------------------------------------*/
class ClientManager : public ServiceManager
{
public:
	ClientManager(NetAddress address, shared_ptr<IocpManager> iocpmanager, SessionMakeFunction func, int32 clientsessioncount);
	virtual ~ClientManager() {};

	virtual bool Start();
	virtual void Stop();
};