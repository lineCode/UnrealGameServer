#pragma once

/*---------------------------------------------------------------------------------------------
�̸�     : JobQueue_Queue
�뵵     : JobQueue���� Queue���·� �����ϴ� ��ü
������   : �̹α�
������¥ : 2022.09.01
----------------------------------------------------------------------------------------------*/
class JobQueue_Queue
{
public:
	void Push(shared_ptr<JobQueue> jobqueue);
	shared_ptr<JobQueue> Pop();

private:
	LockQueue<shared_ptr<JobQueue>> _jobQueues;
};

