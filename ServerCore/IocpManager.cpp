#include "pch.h"
#include "IocpManager.h"
#include "IocpObject.h"
#include "IocpEventStorage.h"

/*---------------------------------------------------------------------------------------------
�̸�     : IocpManager::IocpManager
�뵵     : IOCP�� ������
           CreateIoCompletionPort�� ������ڸ� NULL�� ������ IOCP Ŀ�� �ڵ� ��ȯ
������   : �̹α�
������¥ : 2022.08.14
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
�̸�     : IocpManager::Register
�뵵     : IOCP Ŀ�ο� ������ ����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.14
----------------------------------------------------------------------------------------------*/
bool IocpManager::Register(HANDLE socket) const
{
	return ::CreateIoCompletionPort(socket, _IocpHandle, 0, 0);
}

/*---------------------------------------------------------------------------------------------
�̸�     : IocpManager::Update
�뵵     : IOCP Ŀ�ο� ����� �Ϸ� �����͸� ������Ʈ ���ִ� �Լ�
������   : �̹α�
������¥ : 2022.08.20
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

			//WSA_DISCONNECT �� ��� Update�� �ٽ� �����ؼ� Recv�� 0 ����Ʈ�� Ȯ���ϰ� DisConnect�� �������ִ� �κ�
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
