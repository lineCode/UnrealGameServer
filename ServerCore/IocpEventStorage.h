#pragma once

class IocpObject;

enum class IocpType : uint8
{
	Connect,
	DisConnect,
	Accept,
	Recv,
	Send
};

/*---------------------------------------------------------------------------------------------
이름     : IocpEventStorage
용도     : OVERLAPPED를 상속받아 IOCP의 이벤트 타입과 오브젝트를 들고있는 객체
수정자   : 이민규
수정날짜 : 2022.08.14
----------------------------------------------------------------------------------------------*/
class IocpEventStorage : public OVERLAPPED
{
public:
	IocpEventStorage(IocpType type);
	void Init();

public:
	IocpType _Type;
	shared_ptr<IocpObject> _Object;
};

class ConnectEventStorage : public IocpEventStorage
{
public:
	ConnectEventStorage() : IocpEventStorage(IocpType::Connect) {}
};

class DisConnectEventStorage : public IocpEventStorage
{
public:
	DisConnectEventStorage() : IocpEventStorage(IocpType::DisConnect) {}
};

class AcceptEventStorage : public IocpEventStorage
{
public:
	AcceptEventStorage() : IocpEventStorage(IocpType::Accept){}

public:
	shared_ptr<class Session> _session = nullptr;
};

class RecvEventStorage : public IocpEventStorage
{
public:
	RecvEventStorage() : IocpEventStorage(IocpType::Recv){}
};

class SendEventStorage : public IocpEventStorage
{
public:
	SendEventStorage() : IocpEventStorage(IocpType::Send){}

	Gvector<shared_ptr<class SendBuffer>> _SendBuffers;
};