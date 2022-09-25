#pragma once

/*---------------------------------------------------------------------------------------------
이름     : ReadWriteLock
용도     : WirteLock은 모든 자원 요청에 대해서 블럭 ReadLock은 Write을 제외하고 모두 허용
           Write는 자원을 일관성을 보장할수 없고 Read는 보장할 수 있어서 만든 객체
수정자   : 이민규
수정날짜 : 2022.08.12
----------------------------------------------------------------------------------------------*/
class ReadWriteLock
{
	enum : uint32
	{
		TIMEOUT_TICK = 1000000,
		MAX_SPIN = 10000,
		WRITE_MASK = 0xFFFF'0000,
		READ_MASK = 0x0000'FFFF,
		EMPTY_MASK = 0x0000'0000
	};

public:
	void WriteLock();
	void WriteUnLock();
	void ReadLock();
	void ReadUnLock();

private:
	::atomic<uint32> _lockFlag = EMPTY_MASK;
};

/*---------------------------------------------------------------------------------------------
이름     : ReadWriteLock_Guard
용도     : ReadLock 과 WriteLock의 RAII 방식
수정자   : 이민규
수정날짜 : 2022.08.12
----------------------------------------------------------------------------------------------*/
class ReadLockGuard
{
public:
	ReadLockGuard(ReadWriteLock & lock) : _Lock(lock)
	{
		_Lock.ReadLock();
	}
	~ReadLockGuard()
	{
		_Lock.ReadUnLock();
	}

private:
	ReadWriteLock & _Lock;
};

class WriteLockGuard
{
public:
	WriteLockGuard(ReadWriteLock & lock) : _Lock(lock)
	{
		_Lock.WriteLock();
	}
	~WriteLockGuard()
	{
		_Lock.WriteUnLock();
	}

private:
	ReadWriteLock& _Lock;
};