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

