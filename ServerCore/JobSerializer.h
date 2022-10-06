#pragma once

/*---------------------------------------------------------------------------------------------
이름     : JobSerializer
용도     : JobQueue들을 Queue형태로 관리하는 객체
수정자   : 이민규
수정날짜 : 2022.10.05
----------------------------------------------------------------------------------------------*/
class JobSerializer
{
public:
	void Push(shared_ptr<JobQueue> jobqueue);
	shared_ptr<JobQueue> Pop();

private:
	LockQueue<shared_ptr<JobQueue>> _jobQueues;
};

