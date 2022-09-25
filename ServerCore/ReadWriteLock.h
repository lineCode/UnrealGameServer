#pragma once

/*---------------------------------------------------------------------------------------------
�̸�     : ReadWriteLock
�뵵     : WirteLock�� ��� �ڿ� ��û�� ���ؼ� �� ReadLock�� Write�� �����ϰ� ��� ���
           Write�� �ڿ��� �ϰ����� �����Ҽ� ���� Read�� ������ �� �־ ���� ��ü
������   : �̹α�
������¥ : 2022.08.12
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
�̸�     : ReadWriteLock_Guard
�뵵     : ReadLock �� WriteLock�� RAII ���
������   : �̹α�
������¥ : 2022.08.12
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