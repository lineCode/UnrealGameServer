#pragma once
#include "SocketUtils.h"

/*---------------------------------------------------------------------------------------------
�̸�     : IocpObject
�뵵     : Iocp���� ���� ������ ��ü 
            Update �Լ��� ���� �̺�Ʈ Ÿ�Կ� �°� �����͸� ó����
������   : �̹α�
������¥ : 2022.08.14
----------------------------------------------------------------------------------------------*/
class IocpObject : public enable_shared_from_this<IocpObject>
{
public:
	HANDLE GetSocketHandle() const { return reinterpret_cast<HANDLE>(_socket); }
	SOCKET GetSocket() const { return _socket; }
	void CloseSocket() { SocketUtils::CloseSocket(_socket); }

	virtual void IocpUpdate(class IocpEventStorage * IocpStorage, int32 numOfByte = 0) abstract;

protected:
	SOCKET _socket = INVALID_SOCKET;
};
