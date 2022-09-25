#pragma once

/*---------------------------------------------------------------------------------------------
�̸�     : LockQueue
�뵵     : Push Pop�� �� �� Lock�� �ɾ� Atomic���� �����ϴ� Queue
������   : �̹α�
������¥ : 2022.08.31
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

