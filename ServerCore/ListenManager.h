#pragma once
#include "IocpObject.h"

class ServerManager;
class AcceptEventStorage;

/*---------------------------------------------------------------------------------------------
이름     : ListenManager
용도     : LIsten에 관한 모든것을 관리하는 객체
수정자   : 이민규
수정날짜 : 2022.08.16
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

