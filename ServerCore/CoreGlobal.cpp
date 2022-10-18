#include "pch.h"
#include "CoreGlobal.h"
#include "ConsoleLog.h"
#include "DBConnectionPool.h"
#include "JobSerializer.h"
#include "MemoryManager.h"
#include "SendBuffer.h"
#include "SocketUtils.h"
#include "ThreadManager.h"

ThreadManager* GThreadManager = nullptr;
MemoryManager* GMemoryManager = nullptr;
SendBufferManager* GSendBufferManager = nullptr;
JobSerializer* GJobSerializerManager = nullptr;
JobTimer* GJobTimerManager = nullptr;
DBConnectionPool* GDBConnectionPool = nullptr;
ConsoleLog* GConsoleLogger = nullptr;

/*---------------------------------------------------------------------------------------------
이름     : CoreGlobal
용도     : ServerCore에서 필요한 전역 변수들을 관리하는 객체
수정자   : 이민규
수정날짜 : 2022.08.11
----------------------------------------------------------------------------------------------*/
class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new ThreadManager();
		GMemoryManager = new MemoryManager();
		GSendBufferManager = new SendBufferManager();
		GJobSerializerManager = new JobSerializer();
		GJobTimerManager = new JobTimer();
		GDBConnectionPool = new DBConnectionPool();
		GConsoleLogger = new ConsoleLog;
		SocketUtils::Init();
	}

	~CoreGlobal()
	{
		delete GThreadManager;
		delete GMemoryManager;
		delete GSendBufferManager;
		delete GJobSerializerManager;
		delete GJobTimerManager;
		delete GDBConnectionPool;
		delete GConsoleLogger;
		SocketUtils::Clear();
	}

} GCoreGlobal;

