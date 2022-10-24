#include "pch.h"
#include "DataManager.h"
#include "GameRoom.h"
#include "IocpManager.h"
#include "RoomManager.h"
#include "ServerSession.h"
#include "ServiceManager.h"
#include "ThreadManager.h"
#include "ServerPacketManager.h"
#include "DBConnectionPool.h"
#include "JsonDBSynchronizer.h"
#include "ProcedureManager.h"

enum
{
	WORKER_TIME = 100
};

void WorkProcess(shared_ptr<ServerManager> & server)
{
	while(true)
	{
		LJobEndTime = ::GetTickCount64() + WORKER_TIME;

		server->GetIocpManager()->IocpUpdate(10);

		ThreadManager::GlobalJobTImerProcess();

		ThreadManager::GlobalJobSerializerProcess();
	}
}

int main()
{
	srand((unsigned int)time(NULL));

	ConfigManager::GetInstacnce()->LoadConfig();
	DataManager::GetInstacnce()->LodaData();

	CRASH_IF(GDBConnectionPool->Connect(5, ConfigManager::GetInstacnce()->GetServerConfig()._DBLogin.c_str()) == false);

	DBConnection* dbConn = GDBConnectionPool->Pop();
	JsonDBSynchronizer jsonsync(*dbConn);
	GDBConnectionPool->Push(dbConn);

	jsonsync.Synchronize(ConfigManager::GetInstacnce()->GetServerConfig()._DBPath.c_str());

	ServerPacketManager::Init();
	RoomManager::GetInstance().Add(1);

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

	// TODO : 추후 수정 (타이머 설정으로 변경)
	while(true)
	{
		const auto gameroom = RoomManager::GetInstance().Find(1);
		gameroom->PushAsync(&GameRoom::update);

		this_thread::sleep_for(std::chrono::seconds(1s));
	}

}
