#include "pch.h"
#include "JobQueue.h"
#include "JobSerializer.h"

/*---------------------------------------------------------------------------------------------
이름     : JobQueue::Push
용도     : Jobqueue에 데이터를 넣는 내부 함수
		   JobQueue를 Process 하지 않으면 넣은 쓰레드에서 실행시키고
		   Process 중일 경우 GJobQueueManager에 내 자신인 JobQueue를 넣음
수정자   : 이민규
수정날짜 : 2022.09.02
----------------------------------------------------------------------------------------------*/
void JobQueue::Push(const shared_ptr<Job> job , bool pushonly)
{
	const int32 precvcount = _Jobcount.fetch_add(1);
	_Jobs.Push(job);

	// 첫번째 job을 넣은 쓰레드가 실행까지 담당
	if(precvcount == 0)
	{
		if(LJobQueueProcess == false && pushonly == false)
			Process();
		else
			GJobSerializerManager->Push(shared_from_this());
	}
}

/*---------------------------------------------------------------------------------------------
이름     : JobQueue::Process
용도     : JobQueue에 적재되어있는 Job들을 순차적으로 실행시켜주는 함수
		   JobQueue에 Job이 없을 경우 종료하고 있을 경우 계속 순차적으로 실행
		   단 LJobTickCount에서 정한 시간 보다 초과 될 경우 쓰레드의 다른 일처리를 위하여
		   GJobQueueManager에 내 자신인 JobQueue를 넣고 종료
수정자   : 이민규
수정날짜 : 2022.09.01
----------------------------------------------------------------------------------------------*/
void JobQueue::Process()
{
	LJobQueueProcess = true;

	while(true)
	{
		Gvector<shared_ptr<Job>> jobs;
		_Jobs.PopAll(jobs);

		const int32 jobcount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < jobcount; i++)
			jobs[i]->Process();

		// 남은 일감이 0개면 종료
		if(_Jobcount.fetch_sub(jobcount) == jobcount)
		{
			LJobQueueProcess = false;
			return;
		}

		const uint64 NowTickCount = ::GetTickCount64();
		if(NowTickCount >= LJobEndTime)
		{
			LJobQueueProcess = false;
			GJobSerializerManager->Push(shared_from_this());
			return;
		}
	}
}

