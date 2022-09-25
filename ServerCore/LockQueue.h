#pragma once

/*---------------------------------------------------------------------------------------------
이름     : LockQueue
용도     : Push Pop을 할 때 Lock을 걸어 Atomic성을 보장하는 Queue
수정자   : 이민규
수정날짜 : 2022.08.31
----------------------------------------------------------------------------------------------*/
template <typename T>
class LockQueue
{
public:
	void Push(T data)
	{
		WRITELOCK;
		_queue.push(data);
	}

	T Pop()
	{
		WRITELOCK;

		if (_queue.empty())
			return nullptr;

		T data = _queue.front();
		_queue.pop();

		return data;
	}

	void PopAll(Gvector<T> & datavector)
	{
		WRITELOCK;
		while(T data = Pop())
			datavector.push_back(data);
	}

	void Clear()
	{
		WRITELOCK;
		_queue = Gqueue<T>();
	}

private:
	RWLOCK_USE;
	Gqueue<T> _queue;
};

