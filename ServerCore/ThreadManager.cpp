#include "pch.h"
#include "ThreadManager.h"
#include "JobQueue_Queue.h"

ThreadManager::ThreadManager()
{
	InitTLS();
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

	_JThreads.push_back(jthread([=]()
		{
			InitTLS();
			callback();
			DestroyTLS();
		}));
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
