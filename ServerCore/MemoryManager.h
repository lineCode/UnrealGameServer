#pragma once
#include "CoreGlobal.h"

/*---------------------------------------------------------------------------------------------
이름     : MemoryManager
용도     : Memorypool을 이용하여 다양한 크기의 메모리들을 전체 관리하는 객체
수정자   : 이민규
수정날짜 : 2022.08.11
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
이름     : Gnew
용도     : MemoryManger의 글로벌 변수인 GMemoryManger의 메모리 할당 함수
수정자   : 이민규
수정날짜 : 2022.08.11
----------------------------------------------------------------------------------------------*/
template<typename Type , typename... Args>
Type * Gnew(Args&&... args)
{
	Type* memory = static_cast<Type*>(GMemoryManager->Pop(sizeof(Type)));
	new(memory)Type(forward<Args>(args)...); // PlaceMent New -> 생성자를 직접 호출해 주는 함수

	return memory;
}

/*---------------------------------------------------------------------------------------------
이름     : Gdelete
용도     : MemoryManger의 글로벌 변수인 GMemoryManger의 메모리 반환 함수
수정자   : 이민규
수정날짜 : 2022.08.11
----------------------------------------------------------------------------------------------*/
template<typename Type>
void Gdelete(Type * memory)
{
	memory->~Type();
	GMemoryManager->push(memory , sizeof(Type));
}

/*---------------------------------------------------------------------------------------------
이름     : GMakeShared
용도     : MemoryManger의 글로벌 변수인 GMemoryManger를 Shard_ptr로 반환하는 함수
수정자   : 이민규
수정날짜 : 2022.08.11
----------------------------------------------------------------------------------------------*/
template<typename Type , typename... Args>
shared_ptr<Type> GMakeShared(Args&&... args)
{
	return shared_ptr<Type>{Gnew<Type>(forward<Args>(args)...), Gdelete<Type>};
}