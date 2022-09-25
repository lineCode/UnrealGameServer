#pragma once
#include "SocketUtils.h"

/*---------------------------------------------------------------------------------------------
이름     : IocpObject
용도     : Iocp에서 받은 데이터 객체 
            Update 함수를 통해 이벤트 타입에 맞게 데이터를 처리함
수정자   : 이민규
수정날짜 : 2022.08.14
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
