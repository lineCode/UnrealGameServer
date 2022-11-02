#include "pch.h"
#include "ThreadManager.h"
#include "JobSerializer.h"
#include "ServiceManager.h"
#include "Session.h"

atomic<int> ThreadManager::_send = false;

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
�̸�     : ThreadManager::GlobalJobSerializerProcess
�뵵     : GJobSerializerManager�� JobQueue�� ��ȯ �޾� ���� �ð� ���� ��������ִ� �Լ�
������   : �̹α�
������¥ : 2022.09.02
----------------------------------------------------------------------------------------------*/
void ThreadManager::GlobalJobSerializerProcess()
{
	while(true)
	{
		uint64 nowtime = ::GetTickCount64();

		if (nowtime > LJobEndTime)
			break;

		shared_ptr<JobQueue> jobqueue = GJobSerializerManager->Pop();
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

/*---------------------------------------------------------------------------------------------
�̸�     : ThreadManager::GlobalFlushSend
�뵵     : ������ Session���� ��ȸ�ϸ鼭 Send ���ִ� �Լ�
������   : �̹α�
������¥ : 2022.11.02
----------------------------------------------------------------------------------------------*/
void ThreadManager::GlobalFlushSend(shared_ptr<ServerManager> server)
{
	// �Ѱ��� �����常 �� �� �ֵ���
	if (_send == true)
		return;

	_send.store(true);

	auto sessions = server->GetSessions();

	for(auto session : sessions)
		session->RegisterSend();

	_send.store(false);
}
