#include "pch.h"
#include "ThreadManager.h"

#include "JobQueue_Queue.h"

ThreadManager::ThreadManager()
{
	InitTLS();
}

ThreadManager::~ThreadManager()
{
	Join();
}

/*---------------------------------------------------------------------------------------------
이름     : ThreadManager::Launch
용도     : Launch에 함수를 넣을 경우 그 형태로 쓰레드를 생성해주는 함수
수정자   : 이민규
수정날짜 : 2022.08.13
----------------------------------------------------------------------------------------------*/
void ThreadManager::Launch(function<void()> callback)
{
	WRITELOCK

	_threads.push_back(thread([=]()
		{
			InitTLS();
			callback();
			DestroyTLS();
		}));
}

/*---------------------------------------------------------------------------------------------
이름     : ThreadManager::Join
용도     : ThreadManager에서 관리하는 모든 쓰레드에 Join을 거는 함수
수정자   : 이민규
수정날짜 : 2022.08.13
----------------------------------------------------------------------------------------------*/
void ThreadManager::Join()
{
	for(auto& th : _threads)
	{
		if (th.joinable())
			th.join();
	}

	_threads.clear();
}

/*---------------------------------------------------------------------------------------------
이름     : ThreadManager::InitTLS
용도     : 초기 TLS들을 생성해주는 함수
수정자   : 이민규
수정날짜 : 2022.08.13
----------------------------------------------------------------------------------------------*/
void ThreadManager::InitTLS()
{
	static ::atomic<uint32> ThreadId = 1;
	LThreadId = ThreadId.fetch_add(1);
}

/*---------------------------------------------------------------------------------------------
이름     : hreadManager::DestroyTLS
용도     : TLS중 파괴해야할 부분을 제거하는 함수
수정자   : 이민규
수정날짜 : 2022.08.13
----------------------------------------------------------------------------------------------*/
void ThreadManager::DestroyTLS()
{
}

/*---------------------------------------------------------------------------------------------
이름     : ThreadManager::GlobalJobQueueProcess
용도     : GJobQueueManager에 JobQueue를 반환 받아 일정 시간 동안 실행시켜주는 함수
수정자   : 이민규
수정날짜 : 2022.09.02
----------------------------------------------------------------------------------------------*/
void ThreadManager::GlobalJobQueueProcess()
{
	while(true)
	{
		uint64 nowtime = ::GetTickCount64();

		if (nowtime > LJobEndTime)
			break;

		shared_ptr<JobQueue> jobqueue = GJobQueueManager->Pop();
		if (jobqueue == nullptr)
			break;

		jobqueue->Process();
	}
}

/*---------------------------------------------------------------------------------------------
이름     : ThreadManager::GlobalJobTImerProcess
용도     : GJobTimerManager에 시간이 된 Job을 실행시켜주는 함수
수정자   : 이민규
수정날짜 : 2022.09.02
----------------------------------------------------------------------------------------------*/
void ThreadManager::GlobalJobTImerProcess()
{
	const uint64 nowtime = ::GetTickCount64();

	GJobTimerManager->Process(nowtime);
}
