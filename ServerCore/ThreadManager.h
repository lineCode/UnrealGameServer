#pragma once

#include <thread>
#include <functional>
#include <vector>
#include "CoreMacro.h"

/*---------------------------------------------------------------------------------------------
�̸�     : ThreadManager
�뵵     : �������� ��� Jthread�� �����ϴ� ��ü
������   : �̹α�
������¥ : 2022.10.03
----------------------------------------------------------------------------------------------*/
class ThreadManager
{
public:
	ThreadManager();

	void Launch(function<void(void)> callback);

	ReadWriteLock& GetLock() { return _locks[0]; }

	static void InitTLS();
	static void DestroyTLS();
	static void GlobalJobQueueProcess();
	static void GlobalJobTImerProcess();

private:
	RWLOCK_USE
	vector<jthread> _JThreads;
};

