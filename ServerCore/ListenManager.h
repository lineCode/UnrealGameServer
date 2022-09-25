#pragma once
#include "IocpObject.h"

class ServerManager;
class AcceptEventStorage;

/*---------------------------------------------------------------------------------------------
�̸�     : ListenManager
�뵵     : LIsten�� ���� ������ �����ϴ� ��ü
������   : �̹α�
������¥ : 2022.08.16
----------------------------------------------------------------------------------------------*/
class ListenManager : public IocpObject
{
public:
	ListenManager(shared_ptr<ServerManager> servermanager);
	virtual ~ListenManager();

	bool StartAccept(int32 keepernumber);
	void IocpUpdate(IocpEventStorage* iocpstorage, int32 numOfByte) override;

private:
	void RegisterAccept(AcceptEventStorage * gatekeeper);
	void ProcessAccept(AcceptEventStorage* gatekeeper);

private:
	Gvector<AcceptEventStorage*> _GateKeepers;
	shared_ptr<ServerManager> _ServerManager;
};

