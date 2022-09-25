#pragma once

#define WSA_DISCONNECT 64
/*---------------------------------------------------------------------------------------------
�̸�     : IocpManager
�뵵     : IOCP�� ��� �κ��� �����ϴ� ��ü
������   : �̹α�
������¥ : 2022.08.14
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

