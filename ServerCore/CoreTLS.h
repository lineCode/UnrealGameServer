#pragma once

/*---------------------------------------------------------------------------------------------
�̸�     : CoreTLS
�뵵     : ThreadLocalStorage�� ���� ������ �����ϴ� ���
������   : �̹α�
������¥ : 2022.09.01
----------------------------------------------------------------------------------------------*/

extern thread_local uint32 LThreadId;
extern thread_local shared_ptr<class SendBufferStorage> LSendBufferStorage;
extern thread_local bool LJobQueueProcess;
extern thread_local uint64 LJobEndTime;