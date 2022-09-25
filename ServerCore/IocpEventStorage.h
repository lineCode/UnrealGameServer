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
�̸�     : IocpEventStorage
�뵵     : OVERLAPPED�� ��ӹ޾� IOCP�� �̺�Ʈ Ÿ�԰� ������Ʈ�� ����ִ� ��ü
������   : �̹α�
������¥ : 2022.08.14
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