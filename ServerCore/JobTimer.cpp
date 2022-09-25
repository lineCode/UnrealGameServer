#include "pch.h"
#include "JobTimer.h"
#include "JobQueue.h"

/*---------------------------------------------------------------------------------------------
이름     : JobTimer::Reserve
용도     : Job을 일정 시간에 발동하도록 예약하는 함수
수정자   : 이민규
수정날짜 : 2022.09.02
----------------------------------------------------------------------------------------------*/
void JobTimer::Reserve(uint64 reservetime, weak_ptr<JobQueue> owner, shared_ptr<Job> job)
{
	const uint64 processtime = GetTickCount64() + reservetime;
	JobData* jobdata = ObjectPool<JobData>::Pop(owner, job);

	WRITELOCK;

	_JobTimerQueue.push(JobTimerData{ processtime , jobdata });
}

/*---------------------------------------------------------------------------------------------
이름     : JobTimer::Process
용도     : 예약되어 있는 Job들을 시간 체크 후 담당 소유자에게 넘겨줘서 실행시키는 함수
수정자   : 이민규
수정날짜 : 2022.09.02
----------------------------------------------------------------------------------------------*/
void JobTimer::Process(uint64 nowtime)
{
	if (_Process.exchange(true) == true)
		return;

	WRITELOCK;

	while(_JobTimerQueue.empty() == false)
	{
		const JobTimerData& jobtimerdata = _JobTimerQueue.top();
		if (nowtime < jobtimerdata.ProcessTime)
			break;

		if (shared_ptr<JobQueue> owner = jobtimerdata._JobData->_owner.lock())
			owner->InPush(jobtimerdata._JobData->_Job, true);

		ObjectPool<JobData>::Push(jobtimerdata._JobData);
		_JobTimerQueue.pop();
	}
	

	_Process.store(false);
}

/*---------------------------------------------------------------------------------------------
이름     : JobTimer::Clear
용도     : 예약되어 있는 Job들을 전부 초기화 시키는 함수
수정자   : 이민규
수정날짜 : 2022.09.02
----------------------------------------------------------------------------------------------*/
void JobTimer::Clear()
{
	WRITELOCK;

	while(_JobTimerQueue.empty() == false)
	{
		const JobTimerData& jobtimerdata = _JobTimerQueue.top();
		ObjectPool<JobData>::Push(jobtimerdata._JobData);
		_JobTimerQueue.pop();
	}

}
