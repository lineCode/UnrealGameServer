#pragma once

#include <thread>
#include <functional>
#include <vector>
#include "CoreMacro.h"

/*---------------------------------------------------------------------------------------------
�̸�     : ThreadManager
�뵵     : �������� ��� Thread�� �����ϴ� ��ü
������   : �̹α�
������¥ : 2022.08.13
----------------------------------------------------------------------------------------------*/
class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	void Launch(function<void(void)> callback);
	void Join();

	ReadWriteLock& GetLock() { return _locks[0]; }

	static void InitTLS();
	static void DestroyTLS();
	static void GlobalJobQueueProcess();
	static void GlobalJobTImerProcess();

private:
	RWLOCK_USE
	vector<thread> _threads;
};

