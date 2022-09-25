#include "pch.h"
#include "CoreTLS.h"

thread_local uint32 LThreadId;
thread_local shared_ptr<SendBufferStorage> LSendBufferStorage;
thread_local bool LJobQueueProcess = false;
thread_local uint64 LJobEndTime = 0;