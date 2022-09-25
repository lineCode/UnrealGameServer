#include "pch.h"
#include "ReadWriteLock.h"

/*---------------------------------------------------------------------------------------------
�̸�     : ReadWriteLock::WriteLock
�뵵     : expected���� Flag�� EMPTY�� �� �� ���� ��ٷȴٰ� �ڽ��� ThreadID�� �ְ� ���
������   : �̹α�
������¥ : 2022.08.12
----------------------------------------------------------------------------------------------*/
void ReadWriteLock::WriteLock()
{
	// ������ �������� ��� ���������
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
�̸�     : ReadWriteLock::WriteUnLock()
�뵵     : Flag�� EMPTY�� ���� �� READMASK�� ���� �� 0�� �ƴ� ���� ���� ��� READ����������
           �ɰ��� ���� ���ɼ��� �־� CRASH
������   : �̹α�
������¥ : 2022.08.12
----------------------------------------------------------------------------------------------*/
void ReadWriteLock::WriteUnLock()
{
	if((_lockFlag.load() & READ_MASK) != 0 )
		CRASH("WriteUnLock Error")


	_lockFlag.store(EMPTY_MASK);
}

/*----------------------------------------------------------------------------------------------------------
�̸�     : ReadWriteLock::ReadLock()
�뵵     : Flag�� ReadMASK�� ���� �� ���� �����ȴٸ� Wirte���µ� �ٸ� �� ���µ� �ƴ����� ���� 1�ø��� ���
������   : �̹α�
������¥ : 2022.08.12
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
�̸�     : ReadWriteLock::ReadUnLock()
�뵵     : Flag�� 1���� ������ �� 0�� �� ��� �̹� 0�� �����̹Ƿ� CRASH
������   : �̹α�
������¥ : 2022.08.12
----------------------------------------------------------------------------------------------*/
void ReadWriteLock::ReadUnLock()
{
	if ((_lockFlag.fetch_sub(1) & READ_MASK) == 0)
		CRASH("ReadUnLock Error")
}
