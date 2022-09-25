#include "pch.h"
#include "MemoryManager.h"
#include "MemoryPool.h"

MemoryManager::MemoryManager()
{
	for (int i = 0; i < MAX_ALLOC_SIZE; i++)
		_pools[i] = nullptr;
}

MemoryManager::~MemoryManager()
{
	for (int i = 0; i < MAX_ALLOC_SIZE; i++)
	{
		if (_pools[i] != nullptr)
			delete _pools[i];
	}
}

/*---------------------------------------------------------------------------------------------
이름     : MemoryManager::Pop
용도     : 해당 크기의 Memorypool이 있을 경우 메모리를 가져오고 없을 경우 생성하는 함수
		   단 4096이 넘어 갈 경우 직접 생성해서 제공
수정자   : 이민규
수정날짜 : 2022.08.11
----------------------------------------------------------------------------------------------*/
void* MemoryManager::Pop(int32 size)
{
	void * memory;

	if (size > MAX_ALLOC_SIZE)
	{
		memory = ::_aligned_malloc(size, SLIST_ALIGNMENT);
	}
	else
	{
		if (_pools[size] == nullptr)
			_pools[size] = new MemoryPool(size);

		memory = _pools[size]->Pop();
	}

	return memory;
}

/*---------------------------------------------------------------------------------------------
이름     : MemoryManager::push
용도     : 해당 크기의 Memorypool에 반환하는 함수 단 4096이 넘어 갈 경우 직접 제거
수정자   : 이민규
수정날짜 : 2022.08.11
----------------------------------------------------------------------------------------------*/
void MemoryManager::push(void* ptr , int32 size) const
{
	if(size > MAX_ALLOC_SIZE)
		::_aligned_free(ptr);
	else
		_pools[size]->Push(ptr);
}
