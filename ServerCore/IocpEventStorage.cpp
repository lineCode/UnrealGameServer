#include "pch.h"
#include "IocpEventStorage.h"

IocpEventStorage::IocpEventStorage(IocpType type) :_Type(type)
{
	Init();
}

/*---------------------------------------------------------------------------------------------
�̸�     : IocpEventStorage::Init
�뵵     : OVERLAPPED �ʼ� ������ �ʱ�ȭ ���ִ� �Լ�
������   : �̹α�
������¥ : 2022.08.14
----------------------------------------------------------------------------------------------*/
void IocpEventStorage::Init()
{
	OVERLAPPED::hEvent = 0;
	OVERLAPPED::Internal = 0;
	OVERLAPPED::InternalHigh = 0;
	OVERLAPPED::Offset = 0;
	OVERLAPPED::OffsetHigh = 0;
}
