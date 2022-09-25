#pragma once

/*---------------------------------------------------------------------------------------------
이름     : CoreTLS
용도     : ThreadLocalStorage의 공동 변수를 관리하는 헤더
수정자   : 이민규
수정날짜 : 2022.09.01
----------------------------------------------------------------------------------------------*/

extern thread_local uint32 LThreadId;
extern thread_local shared_ptr<class SendBufferStorage> LSendBufferStorage;
extern thread_local bool LJobQueueProcess;
extern thread_local uint64 LJobEndTime;