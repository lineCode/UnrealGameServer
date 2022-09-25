#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "MemoryManager.h"
#include "SocketUtils.h"
#include "SendBuffer.h"
#include "JobQueue_Queue.h"

ThreadManager* GThreadManager = nullptr;
MemoryManager* GMemoryManager = nullptr;
SendBufferManager* GSendBufferManager = nullptr;
JobQueue_Queue* GJobQueueManager;
JobTimer* GJobTimerManager = nullptr;

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
		GJobQueueManager = new JobQueue_Queue();
		GJobTimerManager = new JobTimer();
		SocketUtils::Init();
	}

	~CoreGlobal()
	{
		delete GThreadManager;
		delete GMemoryManager;
		delete GSendBufferManager;
		delete GJobQueueManager;
		delete GJobTimerManager;
		SocketUtils::Clear();
	}
} GCoreGlobal;

