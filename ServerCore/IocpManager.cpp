#include "pch.h"
#include "IocpManager.h"
#include "IocpObject.h"
#include "IocpEventStorage.h"

/*---------------------------------------------------------------------------------------------
이름     : IocpManager::IocpManager
용도     : IOCP의 생성자
           CreateIoCompletionPort에 모든인자를 NULL로 맞출경우 IOCP 커널 핸들 반환
수정자   : 이민규
수정날짜 : 2022.08.14
----------------------------------------------------------------------------------------------*/
IocpManager::IocpManager()
{
	_IocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CRASH_IF(_IocpHandle == INVALID_HANDLE_VALUE);
}

IocpManager::~IocpManager()
{
	::CloseHandle(_IocpHandle);
}

/*---------------------------------------------------------------------------------------------
이름     : IocpManager::Register
용도     : IOCP 커널에 소켓을 등록하는 함수
수정자   : 이민규
수정날짜 : 2022.08.14
----------------------------------------------------------------------------------------------*/
bool IocpManager::Register(HANDLE socket) const
{
	return ::CreateIoCompletionPort(socket, _IocpHandle, 0, 0);
}

/*---------------------------------------------------------------------------------------------
이름     : IocpManager::Update
용도     : IOCP 커널에 입출력 완료 데이터를 업데이트 해주는 함수
수정자   : 이민규
수정날짜 : 2022.08.20
----------------------------------------------------------------------------------------------*/
bool IocpManager::IocpUpdate(uint32 timeoutms)
{
	DWORD NumOfByte = 0;
	ULONG_PTR Key = 0;
	IocpEventStorage * iocpstorage;

	if(::GetQueuedCompletionStatus(_IocpHandle , &NumOfByte , &Key ,reinterpret_cast<LPOVERLAPPED*>(&iocpstorage) ,timeoutms))
	{
		shared_ptr<IocpObject> object = iocpstorage->_Object;
		object->IocpUpdate(iocpstorage , NumOfByte);
	}
	else
	{
		const int32 ErrCode = ::WSAGetLastError();

		switch (ErrCode)
		{
		case WAIT_TIMEOUT:
			return false;

			//WSA_DISCONNECT 일 경우 Update를 다시 실행해서 Recv가 0 바이트를 확인하고 DisConnect를 실행해주는 부분
		case WSA_DISCONNECT:
			iocpstorage->_Object->IocpUpdate(iocpstorage, NumOfByte);
			break;

		default:
			cout << "[IOCPMANAGER] : Update Error" << endl;
			break;
		}
	}

	return true;
}
