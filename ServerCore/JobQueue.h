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
class JobQueue : enable_shared_from_this<JobQueue>
{
	friend JobTimer;
public:
	void Process();

	/*---------------------------------------------------------------------------------------------
	�̸�     : JobQueue::Push
	�뵵     : Jobqueue�� Job�� ���ٷ� �ٷ� �ִ� ���
	������   : �̹α�
	������¥ : 2022.09.01
	----------------------------------------------------------------------------------------------*/
	void Push(CallBackFunc && callbackfunc)
	{
		InPush(ObjectPool<Job>::MakeShared(std::move(callbackfunc)));
	}

	/*---------------------------------------------------------------------------------------------
	�̸�     : JobQueue::Push
	�뵵     : Jobqueue�� Job�� �ɹ��Լ��� �ִ� ���
	������   : �̹α�
	������¥ : 2022.09.01
	----------------------------------------------------------------------------------------------*/
	template<typename T , typename Ret , typename... Args>
	void Push(Ret(T::*memfunc)(Args...) , Args... args)
	{
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		InPush(ObjectPool<Job>::MakeShared(owner, memfunc, std::forward<Args>(args)...));
	}

	/*---------------------------------------------------------------------------------------------
	�̸�     : JobQueue::TimerPush
	�뵵     : GJobTimerManager�� Job�� �ð��� ������ ���� �ð��� ���� ��� Push�ϴ� ���
	������   : �̹α�
	������¥ : 2022.09.02
	----------------------------------------------------------------------------------------------*/
	void TimerPush(uint64 processtime , CallBackFunc&& callbackfunc)
	{
		const shared_ptr<Job> job = ObjectPool<Job>::MakeShared(std::move(callbackfunc));
		GJobTimerManager->Reserve(processtime, shared_from_this(), job);
	}

	/*---------------------------------------------------------------------------------------------------
	�̸�     : JobQueue::TimerPush
	�뵵     : GJobTimerManager�� Job �ɹ��Լ� ����� �ð��� ������ ���� �ð��� ���� ���  Push�ϴ� ���
	������   : �̹α�
	������¥ : 2022.09.02
	----------------------------------------------------------------------------------------------------*/
	template<typename T, typename Ret, typename... Args>
	void TimerPush(uint64 processtime ,Ret(T::* memfunc)(Args...), Args... args)
	{
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		shared_ptr<Job> job =ObjectPool<Job>::MakeShared(owner, memfunc, std::forward<Args>(args)...);
		GJobTimerManager->Reserve(processtime, shared_from_this(), job);
	}


private:
	void InPush(shared_ptr<Job> job , bool pushonly = false);

protected:
	LockQueue<shared_ptr<Job>> _Jobqueue;
	::atomic<int32> _Jobcount;
};

