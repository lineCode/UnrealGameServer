#pragma once

class JobQueue;

/*---------------------------------------------------------------------------------------------
�̸�     : JobData
�뵵     : JobQueue �� Job�� ����ִ� ��ü
������   : �̹α�
������¥ : 2022.09.02
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
�̸�     : JobTimerData
�뵵     : JobData �� Job�� ������Ѿ� �� �ð��� ����ִ� ��ü
������   : �̹α�
������¥ : 2022.09.02
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
�̸�     : JobTimer
�뵵     : Job�� ����ý������� ���� �ð��� �ߵ��ǰ� ������ ��ü
������   : �̹α�
������¥ : 2022.09.02
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

