#pragma once

enum
{
	SLIST_ALIGNMENT = 16  //SLIST_HEADER를 사용하기 위해서는 16진수로 정렬을 해야함
};

/*---------------------------------------------------------------------------------------------
이름     : MemoryPool
용도     : 미리 메모리를 할당 받아 놓고 호출 시에 확보된 메모리를 제공하고 회수하는 방식의 객체
수정자   : 이민규
수정날짜 : 2022.08.11
----------------------------------------------------------------------------------------------*/
DECLSPEC_ALIGN(SLIST_ALIGNMENT)
class MemoryPool
{
public:
	MemoryPool(int32 size);
	~MemoryPool();
	
	void Push(void* ptr);
	void* Pop();

private:
	SLIST_HEADER _pool; // 원자성을 보장하는 스택
	int32 _size = 0;
};

