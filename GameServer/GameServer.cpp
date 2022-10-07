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
#include "DBBind.h"

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

int main(void)
{
	CRASH_IF(GDBConnectionPool->Connect(1, L"Driver={SQL Server};Server=DESKTOP-U76H54L\\SQLEXPRESS;Database=Test;Uid=UnrealAdmin;Pwd=lej123!;") == false);

	//TODO : Create Table
	{
		auto query = L"		\
		DROP TABLE IF EXISTS [dbo].[Gold];\
		CREATE TABLE [dbo].[Gold] \
		(\
		[id] INT NOT NULL PRIMARY KEY IDENTITY,\
		[gold] INT NULL, \
		[name] NVARCHAR(50) NULL, \
		[createDate] DATETIME NULL \
		);";
		
		DBConnection* dbcon = GDBConnectionPool->Pop();
		CRASH_IF(dbcon->Execute(query) == false);
		GDBConnectionPool->Push(dbcon);
	}

	// TODO :  Add Data
	for(int32 i = 0; i < 3; i++)
	{
		DBConnection* dbcon = GDBConnectionPool->Pop();

		DBBind<3, 0> dbBind(*dbcon, L"INSERT INTO [dbo].[Gold]([gold] , [name] , [createDate]) VALUES(? , ? , ?)");

		int32 gold = 100;
		WCHAR name[100] = L"테스트";
		TIMESTAMP_STRUCT ts = { 2022 , 10 , 8 };
		dbBind.BindParam( gold);
		dbBind.BindParam(name);
		dbBind.BindParam( ts);

		CRASH_IF(dbBind.Execute() == false);

		// TODO : 구버전
		//{
		//	// 기존에 바인딩 된 정보 날림
		//	dbcon->Unbind();

		//	// 넘길 인자 바인딩
		//	int32 gold = 100;
		//	SQLLEN len = 0;

		//	WCHAR name[100] = L"테스트";
		//	SQLLEN namelen = 0;

		//	TIMESTAMP_STRUCT ts = {};
		//	ts.year = 2022;
		//	ts.month = 10;
		//	ts.day = 7;
		//	SQLLEN tslen = 0;

		//	CRASH_IF(dbcon->BindParam(1, &gold, &len) == false);
		//	CRASH_IF(dbcon->BindParam(2, name, &namelen) == false);
		//	CRASH_IF(dbcon->BindParam(3, &ts, &tslen) == false);

		//	// SQL 실행
		//	CRASH_IF(dbcon->Execute(L"INSERT INTO [dbo].[Gold]([gold] , [name] , [createDate]) VALUES(? , ? , ?)") == false);
		//}

		GDBConnectionPool->Push(dbcon);
	}

	// TODO : SQL Read
	{
		DBConnection* dbcon = GDBConnectionPool->Pop();

		DBBind<1, 4> DbBind(*dbcon , L"SELECT id , gold , name , createDate FROM [dbo].[Gold] WHERE gold = (?)");
		int32 gold = 100;
		DbBind.BindParam( gold);

		int32 outid = 0;
		int32 outgold = 0;
		WCHAR outName[100];
		TIMESTAMP_STRUCT outdate = {};

		DbBind.BindCol( outid);
		DbBind.BindCol( outgold);
		DbBind.BindCol( outName);
		DbBind.BindCol(outdate);

		CRASH_IF(DbBind.Execute() == false);
		//// 기존에 바인딩 된 정보 날림
		//dbcon->Unbind();

		//// 넘길 인자 바인딩
		//int32 gold = 100;
		//SQLLEN len = 0;

		//CRASH_IF(dbcon->BindParam(1, &gold, &len) == false);

		//int32 outid = 0;
		//SQLLEN outidlen = 0;
		//dbcon->BindCol(1,  &outid, &outidlen);

		//int32 outgold = 0;
		//SQLLEN outgoldlen = 0;
		//dbcon->BindCol(2,  &outgold, &outgoldlen);

		//WCHAR outName[100];
		//SQLLEN namelen = 0;
		//dbcon->BindCol(3, outName, sizeof(outName),&namelen);

		//TIMESTAMP_STRUCT outdate = {};
		//SQLLEN outdatelen = 0;
		//dbcon->BindCol(4, &outdate, &outdatelen);

		//// SQL 실행
		//CRASH_IF(dbcon->Execute(L"SELECT id , gold , name , createDate FROM [dbo].[Gold] WHERE gold = (?)") == false);

		wcout.imbue(locale("kor"));
		while(dbcon->Fetch())
		{
			wcout << "name : " << outName << endl;
			cout << format("id : {} Gold : {} Date : {} {} {}", outid, outgold  , outdate.year , outdate.minute , outdate.day) << endl;
		}

		
		GDBConnectionPool->Push(dbcon);
	}

	ConfigManager::GetInstacnce()->LoadConfig();
	DataManager::GetInstacnce()->LodaData();
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
