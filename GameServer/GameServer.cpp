#include "pch.h"

#include "GameRoom.h"
#include "IocpManager.h"
#include "RoomManager.h"
#include "ServerSession.h"
#include "ServiceManager.h"
#include "ThreadManager.h"
#include "ServerPacketManager.h"

enum
{
	WORKER_TIME = 2000
};

void WorkProcess(shared_ptr<ServerManager> & server)
{
	while(true)
	{
		LJobEndTime = ::GetTickCount64() + WORKER_TIME;

		server->GetIocpManager()->IocpUpdate(10);

		ThreadManager::GlobalJobTImerProcess();

		ThreadManager::GlobalJobQueueProcess();
	}
}

int main(void)
{
	ServerPacketManager::Init();
	RoomManager::GetInstance().Add();

	shared_ptr<ServerManager> server = GMakeShared<ServerManager>(
		NetAddress(L"127.0.0.1", 7777),
		GMakeShared<IocpManager>(),
		GMakeShared<ServerSession>,
		5);

	CRASH_IF(server->Start() == false);

	for(int i = 0; i < 3; i++)
	{
		GThreadManager->Launch([&server]()
		{
			WorkProcess(server);
		});
	}


	while(true)
		RoomManager::GetInstance().Find(1)->update();
	
	GThreadManager->Join();
}
