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
이름     : MemoryPool::Push
용도     : 메모리를 반환하지 않고 회수하는 함수
수정자   : 이민규
수정날짜 : 2022.08.11
----------------------------------------------------------------------------------------------*/
void MemoryPool::Push(void * ptr)
{
	const auto memory = static_cast<SLIST_ENTRY*>(ptr);
	::InterlockedPushEntrySList(&_pool, memory);
}

/*---------------------------------------------------------------------------------------------
이름     : MemoryPool::Pop
용도     : 메모리풀에 있을 경우 확보된 메모리를 제공하고 없을 경우 16진수로 정렬 후 생성후 제공
수정자   : 이민규
수정날짜 : 2022.08.11
----------------------------------------------------------------------------------------------*/
void* MemoryPool::Pop()
{
	void * memory = ::InterlockedPopEntrySList(&_pool);

	if(memory == nullptr)
		memory = ::_aligned_malloc(_size, SLIST_ALIGNMENT);

	return memory;
}
