#pragma once

#include <thread>
#include <functional>
#include <vector>
#include "CoreMacro.h"

/*---------------------------------------------------------------------------------------------
이름     : ThreadManager
용도     : 서버내의 모든 Jthread를 관리하는 객체
수정자   : 이민규
수정날짜 : 2022.10.03
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

