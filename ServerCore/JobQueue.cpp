#include "pch.h"
#include "JobQueue.h"

#include "JobQueue_Queue.h"

/*---------------------------------------------------------------------------------------------
�̸�     : JobQueue::InPush
�뵵     : Jobqueue�� �����͸� �ִ� ���� �Լ�
		   JobQueue�� Process ���� ������ ���� �����忡�� �����Ű��
		   �ϰ� ���� ��� GJobQueueManager�� �� �ڽ��� JobQueue�� ����
������   : �̹α�
������¥ : 2022.09.02
----------------------------------------------------------------------------------------------*/
void JobQueue::InPush(shared_ptr<Job> job , bool pushonly)
{
	const int32 Count = _Jobcount.fetch_add(1);
	_Jobqueue.Push(job);

	if(Count == 0)
	{
		if(LJobQueueProcess == false && pushonly == false)
		{
			Process();
		}
		else
		{
			GJobQueueManager->Push(shared_from_this());
		}
	}
}

/*---------------------------------------------------------------------------------------------
�̸�     : JobQueue::Process
�뵵     : JobQueue�� ����Ǿ��ִ� Job���� ���������� ��������ִ� �Լ�
		   JobQueue�� Job�� ���� ��� �����ϰ� ���� ��� ��� ���������� ����
		   �� LJobTickCount���� ���� �ð� ���� �ʰ� �� ��� �������� �ٸ� ��ó���� ���Ͽ�
		   GJobQueueManager�� �� �ڽ��� JobQueue�� �ְ� ����
������   : �̹α�
������¥ : 2022.09.01
----------------------------------------------------------------------------------------------*/
void JobQueue::Process()
{
	LJobQueueProcess = true;

	while(true)
	{
		Gvector<shared_ptr<Job>> jobs;
		_Jobqueue.PopAll(jobs);

		const int32 jobcount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < jobcount; i++)
			jobs[i]->Process();

		if(_Jobcount.fetch_sub(jobcount) == jobcount)
		{
			LJobQueueProcess = false;
			return;
		}

		const uint64 NowTickCount = ::GetTickCount64();
		if(NowTickCount >= LJobEndTime)
		{
			LJobQueueProcess = false;
			GJobQueueManager->Push(shared_from_this());
			return;
		}
	}
}

