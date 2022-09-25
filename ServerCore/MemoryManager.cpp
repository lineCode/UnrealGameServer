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
�̸�     : MemoryManager::Pop
�뵵     : �ش� ũ���� Memorypool�� ���� ��� �޸𸮸� �������� ���� ��� �����ϴ� �Լ�
		   �� 4096�� �Ѿ� �� ��� ���� �����ؼ� ����
������   : �̹α�
������¥ : 2022.08.11
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
�̸�     : MemoryManager::push
�뵵     : �ش� ũ���� Memorypool�� ��ȯ�ϴ� �Լ� �� 4096�� �Ѿ� �� ��� ���� ����
������   : �̹α�
������¥ : 2022.08.11
----------------------------------------------------------------------------------------------*/
void MemoryManager::push(void* ptr , int32 size) const
{
	if(size > MAX_ALLOC_SIZE)
		::_aligned_free(ptr);
	else
		_pools[size]->Push(ptr);
}
