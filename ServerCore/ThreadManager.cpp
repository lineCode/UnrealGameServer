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
이름     : ThreadManager::Launch
용도     : Launch에 함수를 넣을 경우 그 형태로 쓰레드를 생성해주는 함수
수정자   : 이민규
수정날짜 : 2022.08.13
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
이름     : ThreadManager::GlobalJobSerializerProcess
용도     : GJobSerializerManager에 JobQueue를 반환 받아 일정 시간 동안 실행시켜주는 함수
수정자   : 이민규
수정날짜 : 2022.09.02
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

/*---------------------------------------------------------------------------------------------
이름     : ThreadManager::GlobalFlushSend
용도     : 서버의 Session들을 순회하면서 Send 해주는 함수
수정자   : 이민규
수정날짜 : 2022.11.02
----------------------------------------------------------------------------------------------*/
void ThreadManager::GlobalFlushSend(shared_ptr<ServerManager> server)
{
	// 한개의 쓰레드만 할 수 있도록
	if (_send == true)
		return;

	_send.store(true);

	auto sessions = server->GetSessions();

	for(auto session : sessions)
		session->RegisterSend();

	_send.store(false);
}
