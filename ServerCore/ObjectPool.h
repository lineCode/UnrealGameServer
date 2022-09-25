#pragma once
#include "MemoryPool.h"

/*---------------------------------------------------------------------------------------------
�̸�     : ObjectPool
�뵵     : MemoryPool�� ��ü ���� �����ϱ� ���� �޸� Ǯ�� ��ü
������   : �̹α�
������¥ : 2022.08.11
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