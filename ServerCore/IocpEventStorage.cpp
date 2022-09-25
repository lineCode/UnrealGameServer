#include "pch.h"
#include "IocpEventStorage.h"

IocpEventStorage::IocpEventStorage(IocpType type) :_Type(type)
{
	Init();
}

/*---------------------------------------------------------------------------------------------
이름     : IocpEventStorage::Init
용도     : OVERLAPPED 필수 값들을 초기화 해주는 함수
수정자   : 이민규
수정날짜 : 2022.08.14
----------------------------------------------------------------------------------------------*/
void IocpEventStorage::Init()
{
	OVERLAPPED::hEvent = 0;
	OVERLAPPED::Internal = 0;
	OVERLAPPED::InternalHigh = 0;
	OVERLAPPED::Offset = 0;
	OVERLAPPED::OffsetHigh = 0;
}
