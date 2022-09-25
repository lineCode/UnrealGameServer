#pragma once

#define WSA_DISCONNECT 64
/*---------------------------------------------------------------------------------------------
이름     : IocpManager
용도     : IOCP의 모든 부분을 관리하는 객체
수정자   : 이민규
수정날짜 : 2022.08.14
----------------------------------------------------------------------------------------------*/
class IocpManager
{
public:
	IocpManager();
	~IocpManager();

	HANDLE GetIocpHandle() const { return _IocpHandle; }

	bool Register(HANDLE socket) const;
	bool IocpUpdate(uint32 timeoutms = INFINITE);

private:
	HANDLE _IocpHandle;
};

