#pragma once
#include "Job.h"
#include "LockQueue.h"
#include "JobTimer.h"

/*---------------------------------------------------------------------------------------------
�̸�     : JobQueue
�뵵     : Job���� Queue���·� �����ϴ� ��ü
������   : �̹α�
������¥ : 2022.09.01
----------------------------------------------------------------------------------------------*/
class JobQueue : public enable_shared_from_this<JobQueue>
{
	friend JobTimer;

public:
	void Process();

	/*---------------------------------------------------------------------------------------------
	�̸�     : JobQueue::PushAsync
	�뵵     : Jobqueue�� ���ٸ� Job���� ��ȯ���� �ִ� ���
	������   : �̹α�
	������¥ : 2022.09.01
	----------------------------------------------------------------------------------------------*/
	void PushAsync(CallBackFunc && callbackfunc)
	{
		Push(ObjectPool<Job>::MakeShared(std::move(callbackfunc)));
	}

	/*---------------------------------------------------------------------------------------------
	�̸�     : JobQueue::PushAsync
	�뵵     : Jobqueue�� �ɹ��Լ��� Job���� ��ȯ���� �ִ� �Լ�
	������   : �̹α�
	������¥ : 2022.09.01
	----------------------------------------------------------------------------------------------*/
	template<typename T , typename Ret , typename... Args>
	void PushAsync(Ret(T::*memfunc)(Args...) , Args... args)
	{
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		Push(ObjectPool<Job>::MakeShared(owner, memfunc, std::forward<Args>(args)...));
	}

	/*---------------------------------------------------------------------------------------------
	�̸�     : JobQueue::TimerPush
	�뵵     : GJobTimerManager�� Job�� �ð��� ������ ���� �ð��� ���� ��� Push�ϴ� ���
	           ���� Job�� ��ȯ�ؼ� Cancle�� ������ ������
	������   : �̹α�
	������¥ : 2022.10.31
	----------------------------------------------------------------------------------------------*/
	shared_ptr<Job> TimerPush(uint64 processtime , CallBackFunc&& callbackfunc)
	{
		const shared_ptr<Job> job = ObjectPool<Job>::MakeShared(std::move(callbackfunc));
		GJobTimerManager->Reserve(processtime, shared_from_this(), job);
		return job;
	}

	/*---------------------------------------------------------------------------------------------------
	�̸�     : JobQueue::TimerPush
	�뵵     : GJobTimerManager�� Job �ɹ��Լ� ����� �ð��� ������ ���� �ð��� ���� ���  Push�ϴ� ���
			   ���� Job�� ��ȯ�ؼ� Cancle�� ������ ������
	������   : �̹α�
	������¥ : 2022.10.31
	----------------------------------------------------------------------------------------------------*/
	template<typename T, typename Ret, typename... Args>
	shared_ptr<Job> TimerPush(uint64 processtime ,Ret(T::* memfunc)(Args...), Args... args)
	{
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		shared_ptr<Job> job =ObjectPool<Job>::MakeShared(owner, memfunc, std::forward<Args>(args)...);
		GJobTimerManager->Reserve(processtime, shared_from_this(), job);
		return job;
	}

	void ClearJobs() { _Jobs.Clear(); }

private:
	void Push(const shared_ptr<Job> job , bool pushonly = false);

protected:
	LockQueue<shared_ptr<Job>> _Jobs;
	::atomic<int32> _Jobcount;
};

