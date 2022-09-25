#pragma once

class JobQueue;

/*---------------------------------------------------------------------------------------------
이름     : JobData
용도     : JobQueue 와 Job을 들고있는 객체
수정자   : 이민규
수정날짜 : 2022.09.02
----------------------------------------------------------------------------------------------*/
struct JobData
{
	JobData(weak_ptr<JobQueue> owner , shared_ptr<Job> job) :  _owner(owner) ,_Job(job)
	{
		
	}

	weak_ptr<JobQueue> _owner;
	shared_ptr<Job> _Job;
};

/*---------------------------------------------------------------------------------------------
이름     : JobTimerData
용도     : JobData 와 Job을 실행시켜야 할 시간을 들고있는 객체
수정자   : 이민규
수정날짜 : 2022.09.02
----------------------------------------------------------------------------------------------*/
struct JobTimerData
{
	bool operator>(const JobTimerData & other) const
	{
		return ProcessTime > other.ProcessTime;
	}

	uint64 ProcessTime = 0;
	JobData* _JobData = nullptr;
};

/*---------------------------------------------------------------------------------------------
이름     : JobTimer
용도     : Job을 예약시스템으로 일정 시간에 발동되게 구현한 객체
수정자   : 이민규
수정날짜 : 2022.09.02
----------------------------------------------------------------------------------------------*/
class JobTimer
{
public:
	void Reserve(uint64 reservetime, weak_ptr<JobQueue> owner, shared_ptr<Job> job);
	void Process(uint64 nowtime);
	void Clear();

private:
	RWLOCK_USE;
	GpriorityQueue<JobTimerData> _JobTimerQueue;
	::atomic<bool> _Process = false;
};

