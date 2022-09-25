#pragma once
#include <functional>

using CallBackFunc = std::function<void(void)>;

/*---------------------------------------------------------------------------------------------
이름     : Job
용도     : 클라이언트가 보낸 요청을 객체로 캡슐화하여 나중에 실행 할 수 있도록 저장해놓은 객체 
수정자   : 이민규
수정날짜 : 2022.08.31
----------------------------------------------------------------------------------------------*/
class Job
{
public:
	Job(CallBackFunc&& callbackfunc) : _CallBackFunc(std::move(callbackfunc))
	{
	}

	template<typename T , typename Ret , typename ...Args>
	Job(shared_ptr<T> owner , Ret(T:: *memFunc)(Args...) , Args&&... args)
	{
		_CallBackFunc = [owner, memFunc, args...]()
		{
			owner->*memFunc(args...);
		};
	}

	~Job() { _CallBackFunc = nullptr; }

	/*---------------------------------------------------------------------------------------------
	이름     : Process
	용도     : 캡슐화된 클라이언트 요청을 실행하는 함수
	수정자   : 이민규
	수정날짜 : 2022.08.31
	----------------------------------------------------------------------------------------------*/
	void Process() { _CallBackFunc(); }

private:
	CallBackFunc _CallBackFunc;
};

