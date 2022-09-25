#include "pch.h"
#include "ReadWriteLock.h"

/*---------------------------------------------------------------------------------------------
이름     : ReadWriteLock::WriteLock
용도     : expected에서 Flag가 EMPTY가 될 때 까지 기다렸다가 자신의 ThreadID를 넣고 잠금
수정자   : 이민규
수정날짜 : 2022.08.12
----------------------------------------------------------------------------------------------*/
void ReadWriteLock::WriteLock()
{
	// 동일한 쓰레드일 경우 통과시켜줌
	const uint32 ThreadId = (_lockFlag.load() & WRITE_MASK) >> 16;
	if (ThreadId == LThreadId)
		return;

	const int64 BeginTick = ::GetTickCount64();
	const uint32 desired = (LThreadId << 16) & WRITE_MASK;

	while(true)
	{
		for(uint32 spin = 0; spin <= MAX_SPIN; spin++)
		{
			uint32 expected = EMPTY_MASK;
			if(_lockFlag.compare_exchange_strong(expected ,desired))
				return;
		}

		if (::GetTickCount64() - BeginTick >= TIMEOUT_TICK)
			this_thread::yield();

			/*CRASH("DeadLock")*/

		this_thread::yield();
	}
}

/*---------------------------------------------------------------------------------------------
이름     : ReadWriteLock::WriteUnLock()
용도     : Flag를 EMPTY로 만듬 단 READMASK를 했을 때 0이 아닌 값이 나온 경우 READ상태임으로
           심각한 버그 가능성이 있어 CRASH
수정자   : 이민규
수정날짜 : 2022.08.12
----------------------------------------------------------------------------------------------*/
void ReadWriteLock::WriteUnLock()
{
	if((_lockFlag.load() & READ_MASK) != 0 )
		CRASH("WriteUnLock Error")


	_lockFlag.store(EMPTY_MASK);
}

/*----------------------------------------------------------------------------------------------------------
이름     : ReadWriteLock::ReadLock()
용도     : Flag에 ReadMASK를 했을 때 값이 유지된다면 Wirte상태도 다른 값 상태도 아님으로 값을 1올리고 잠금
수정자   : 이민규
수정날짜 : 2022.08.12
----------------------------------------------------------------------------------------------------------*/
void ReadWriteLock::ReadLock()
{
	const uint32 ThreadId = (_lockFlag.load() & WRITE_MASK) >> 16;
	if (ThreadId == LThreadId)
	{
		_lockFlag.fetch_add(1);
		return;
	}

	const int64 BeginTick = ::GetTickCount64();

	while(true)
	{
		for(uint32 spin = 0; spin <= MAX_SPIN; spin++)
		{
			uint32 expected = _lockFlag.load() & READ_MASK;
			if (_lockFlag.compare_exchange_strong(expected, expected + 1))
				return;
		}

		if (::GetTickCount64() - BeginTick >= TIMEOUT_TICK)
			this_thread::yield();

		this_thread::yield();
	}
}

/*---------------------------------------------------------------------------------------------
이름     : ReadWriteLock::ReadUnLock()
용도     : Flag를 1빼고 마무리 단 0이 뜰 경우 이미 0인 상태이므로 CRASH
수정자   : 이민규
수정날짜 : 2022.08.12
----------------------------------------------------------------------------------------------*/
void ReadWriteLock::ReadUnLock()
{
	if ((_lockFlag.fetch_sub(1) & READ_MASK) == 0)
		CRASH("ReadUnLock Error")
}
