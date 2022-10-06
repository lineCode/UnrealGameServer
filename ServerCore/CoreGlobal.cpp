#include "pch.h"
#include "CoreGlobal.h"
#include "DBConnectionPool.h"
#include "JobSerializer.h"
#include "MemoryManager.h"
#include "SendBuffer.h"
#include "SocketUtils.h"
#include "ThreadManager.h"

ThreadManager* GThreadManager = nullptr;
MemoryManager* GMemoryManager = nullptr;
SendBufferManager* GSendBufferManager = nullptr;
JobSerializer* GJobSerializerManager;
JobTimer* GJobTimerManager = nullptr;
DBConnectionPool* GDBConnectionPool = nullptr;

/*---------------------------------------------------------------------------------------------
�̸�     : CoreGlobal
�뵵     : ServerCore���� �ʿ��� ���� �������� �����ϴ� ��ü
������   : �̹α�
������¥ : 2022.08.11
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
		SocketUtils::Clear();
	}

} GCoreGlobal;

