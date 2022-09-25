#pragma once
#include "MemoryPool.h"

/*---------------------------------------------------------------------------------------------
이름     : ObjectPool
용도     : MemoryPool을 객체 별로 관리하기 위한 메모리 풀링 객체
수정자   : 이민규
수정날짜 : 2022.08.11
----------------------------------------------------------------------------------------------*/
template<typename Type>
class ObjectPool
{
public:
	template<typename... Args>
	static Type * Pop(Args&&... args)
	{
		Type* memory = static_cast<Type*>(_pool.Pop());

		new(memory)Type(forward<Args>(args)...);
		return memory;
	}

	static void Push(Type * memory)
	{
		memory->~Type();
		_pool.Push(memory);
	}

	template<typename... Args>
	static shared_ptr<Type> MakeShared(Args&&... args)
	{
		return shared_ptr<Type>{Pop(forward<Args>(args)...), Push};
	}
private:
	static MemoryPool _pool;
};

template<typename Type>
MemoryPool ObjectPool<Type>::_pool = sizeof(Type);