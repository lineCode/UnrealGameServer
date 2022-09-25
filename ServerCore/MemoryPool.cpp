#include "pch.h"
#include "MemoryPool.h"

MemoryPool::MemoryPool(int32 size) : _size(size)
{
	::InitializeSListHead(&_pool);
}

MemoryPool::~MemoryPool()
{
	while (void* ptr = ::InterlockedPopEntrySList(&_pool))
		::_aligned_free(ptr);
}

/*---------------------------------------------------------------------------------------------
�̸�     : MemoryPool::Push
�뵵     : �޸𸮸� ��ȯ���� �ʰ� ȸ���ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.11
----------------------------------------------------------------------------------------------*/
void MemoryPool::Push(void * ptr)
{
	const auto memory = static_cast<SLIST_ENTRY*>(ptr);
	::InterlockedPushEntrySList(&_pool, memory);
}

/*---------------------------------------------------------------------------------------------
�̸�     : MemoryPool::Pop
�뵵     : �޸�Ǯ�� ���� ��� Ȯ���� �޸𸮸� �����ϰ� ���� ��� 16������ ���� �� ������ ����
������   : �̹α�
������¥ : 2022.08.11
----------------------------------------------------------------------------------------------*/
void* MemoryPool::Pop()
{
	void * memory = ::InterlockedPopEntrySList(&_pool);

	if(memory == nullptr)
		memory = ::_aligned_malloc(_size, SLIST_ALIGNMENT);

	return memory;
}
