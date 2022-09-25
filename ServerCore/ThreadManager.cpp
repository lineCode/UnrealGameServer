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
�̸�     : ThreadManager::Launch
�뵵     : Launch�� �Լ��� ���� ��� �� ���·� �����带 �������ִ� �Լ�
������   : �̹α�
������¥ : 2022.08.13
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
�̸�     : ThreadManager::Join
�뵵     : ThreadManager���� �����ϴ� ��� �����忡 Join�� �Ŵ� �Լ�
������   : �̹α�
������¥ : 2022.08.13
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
�̸�     : ThreadManager::InitTLS
�뵵     : �ʱ� TLS���� �������ִ� �Լ�
������   : �̹α�
������¥ : 2022.08.13
----------------------------------------------------------------------------------------------*/
void ThreadManager::InitTLS()
{
	static ::atomic<uint32> ThreadId = 1;
	LThreadId = ThreadId.fetch_add(1);
}

/*---------------------------------------------------------------------------------------------
�̸�     : hreadManager::DestroyTLS
�뵵     : TLS�� �ı��ؾ��� �κ��� �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.13
----------------------------------------------------------------------------------------------*/
void ThreadManager::DestroyTLS()
{
}

/*---------------------------------------------------------------------------------------------
�̸�     : ThreadManager::GlobalJobQueueProcess
�뵵     : GJobQueueManager�� JobQueue�� ��ȯ �޾� ���� �ð� ���� ��������ִ� �Լ�
������   : �̹α�
������¥ : 2022.09.02
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
�̸�     : ThreadManager::GlobalJobTImerProcess
�뵵     : GJobTimerManager�� �ð��� �� Job�� ��������ִ� �Լ�
������   : �̹α�
������¥ : 2022.09.02
----------------------------------------------------------------------------------------------*/
void ThreadManager::GlobalJobTImerProcess()
{
	const uint64 nowtime = ::GetTickCount64();

	GJobTimerManager->Process(nowtime);
}
