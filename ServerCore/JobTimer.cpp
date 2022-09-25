#include "pch.h"
#include "JobTimer.h"
#include "JobQueue.h"

/*---------------------------------------------------------------------------------------------
�̸�     : JobTimer::Reserve
�뵵     : Job�� ���� �ð��� �ߵ��ϵ��� �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.09.02
----------------------------------------------------------------------------------------------*/
void JobTimer::Reserve(uint64 reservetime, weak_ptr<JobQueue> owner, shared_ptr<Job> job)
{
	const uint64 processtime = GetTickCount64() + reservetime;
	JobData* jobdata = ObjectPool<JobData>::Pop(owner, job);

	WRITELOCK;

	_JobTimerQueue.push(JobTimerData{ processtime , jobdata });
}

/*---------------------------------------------------------------------------------------------
�̸�     : JobTimer::Process
�뵵     : ����Ǿ� �ִ� Job���� �ð� üũ �� ��� �����ڿ��� �Ѱ��༭ �����Ű�� �Լ�
������   : �̹α�
������¥ : 2022.09.02
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
�̸�     : JobTimer::Clear
�뵵     : ����Ǿ� �ִ� Job���� ���� �ʱ�ȭ ��Ű�� �Լ�
������   : �̹α�
������¥ : 2022.09.02
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
