#pragma once
#include "Job.h"
#include "LockQueue.h"
#include "JobTimer.h"

/*---------------------------------------------------------------------------------------------
이름     : JobQueue
용도     : Job들을 Queue형태로 관리하는 객체
수정자   : 이민규
수정날짜 : 2022.09.01
----------------------------------------------------------------------------------------------*/
class JobQueue : public enable_shared_from_this<JobQueue>
{
	friend JobTimer;

public:
	void Process();

	/*---------------------------------------------------------------------------------------------
	이름     : JobQueue::PushAsync
	용도     : Jobqueue에 람다를 Job으로 변환시켜 넣는 방식
	수정자   : 이민규
	수정날짜 : 2022.09.01
	----------------------------------------------------------------------------------------------*/
	void PushAsync(CallBackFunc && callbackfunc)
	{
		Push(ObjectPool<Job>::MakeShared(std::move(callbackfunc)));
	}

	/*---------------------------------------------------------------------------------------------
	이름     : JobQueue::PushAsync
	용도     : Jobqueue에 맴버함수를 Job으로 변환시켜 넣는 함수
	수정자   : 이민규
	수정날짜 : 2022.09.01
	----------------------------------------------------------------------------------------------*/
	template<typename T , typename Ret , typename... Args>
	void PushAsync(Ret(T::*memfunc)(Args...) , Args... args)
	{
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		Push(ObjectPool<Job>::MakeShared(owner, memfunc, std::forward<Args>(args)...));
	}

	/*---------------------------------------------------------------------------------------------
	이름     : JobQueue::TimerPush
	용도     : GJobTimerManager에 Job을 시간을 예약해 놓고 시간이 됐을 경우 Push하는 방식
	           만든 Job을 반환해서 Cancle을 수정이 가능함
	수정자   : 이민규
	수정날짜 : 2022.10.31
	----------------------------------------------------------------------------------------------*/
	shared_ptr<Job> TimerPush(uint64 processtime , CallBackFunc&& callbackfunc)
	{
		const shared_ptr<Job> job = ObjectPool<Job>::MakeShared(std::move(callbackfunc));
		GJobTimerManager->Reserve(processtime, shared_from_this(), job);
		return job;
	}

	/*---------------------------------------------------------------------------------------------------
	이름     : JobQueue::TimerPush
	용도     : GJobTimerManager에 Job 맴버함수 방식을 시간을 예약해 놓고 시간이 됐을 경우  Push하는 방식
			   만든 Job을 반환해서 Cancle을 수정이 가능함
	수정자   : 이민규
	수정날짜 : 2022.10.31
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

