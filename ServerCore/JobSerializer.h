#pragma once

/*---------------------------------------------------------------------------------------------
�̸�     : JobSerializer
�뵵     : JobQueue���� Queue���·� �����ϴ� ��ü
������   : �̹α�
������¥ : 2022.10.05
----------------------------------------------------------------------------------------------*/
class JobSerializer
{
public:
	void Push(shared_ptr<JobQueue> jobqueue);
	shared_ptr<JobQueue> Pop();

private:
	LockQueue<shared_ptr<JobQueue>> _jobQueues;
};

