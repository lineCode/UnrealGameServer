#pragma once
#include "CoreGlobal.h"

/*---------------------------------------------------------------------------------------------
�̸�     : MemoryManager
�뵵     : Memorypool�� �̿��Ͽ� �پ��� ũ���� �޸𸮵��� ��ü �����ϴ� ��ü
������   : �̹α�
������¥ : 2022.08.11
----------------------------------------------------------------------------------------------*/
class MemoryManager
{
	enum
	{
		MAX_ALLOC_SIZE = 4097
	};

public:
	MemoryManager();
	~MemoryManager();

	void* Pop(int32 size);
	void push(void* ptr , int32 size) const;

private:
	class MemoryPool* _pools[MAX_ALLOC_SIZE];
};

/*---------------------------------------------------------------------------------------------
�̸�     : Gnew
�뵵     : MemoryManger�� �۷ι� ������ GMemoryManger�� �޸� �Ҵ� �Լ�
������   : �̹α�
������¥ : 2022.08.11
----------------------------------------------------------------------------------------------*/
template<typename Type , typename... Args>
Type * Gnew(Args&&... args)
{
	Type* memory = static_cast<Type*>(GMemoryManager->Pop(sizeof(Type)));
	new(memory)Type(forward<Args>(args)...); // PlaceMent New -> �����ڸ� ���� ȣ���� �ִ� �Լ�

	return memory;
}

/*---------------------------------------------------------------------------------------------
�̸�     : Gdelete
�뵵     : MemoryManger�� �۷ι� ������ GMemoryManger�� �޸� ��ȯ �Լ�
������   : �̹α�
������¥ : 2022.08.11
----------------------------------------------------------------------------------------------*/
template<typename Type>
void Gdelete(Type * memory)
{
	memory->~Type();
	GMemoryManager->push(memory , sizeof(Type));
}

/*---------------------------------------------------------------------------------------------
�̸�     : GMakeShared
�뵵     : MemoryManger�� �۷ι� ������ GMemoryManger�� Shard_ptr�� ��ȯ�ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.11
----------------------------------------------------------------------------------------------*/
template<typename Type , typename... Args>
shared_ptr<Type> GMakeShared(Args&&... args)
{
	return shared_ptr<Type>{Gnew<Type>(forward<Args>(args)...), Gdelete<Type>};
}