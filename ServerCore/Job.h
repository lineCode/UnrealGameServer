#pragma once
#include <functional>

using CallBackFunc = std::function<void(void)>;

/*---------------------------------------------------------------------------------------------
�̸�     : Job
�뵵     : Ŭ���̾�Ʈ�� ���� ��û�� ��ü�� ĸ��ȭ�Ͽ� ���߿� ���� �� �� �ֵ��� �����س��� ��ü 
������   : �̹α�
������¥ : 2022.08.31
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
	�̸�     : Process
	�뵵     : ĸ��ȭ�� Ŭ���̾�Ʈ ��û�� �����ϴ� �Լ�
	������   : �̹α�
	������¥ : 2022.08.31
	----------------------------------------------------------------------------------------------*/
	void Process() { _CallBackFunc(); }

private:
	CallBackFunc _CallBackFunc;
};

