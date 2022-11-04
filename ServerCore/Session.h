#pragma once
#include "IocpEventStorage.h"
#include "IocpObject.h"
#include "NetAddress.h"
#include "RecvBuffer.h"
#include "SendBuffer.h"
#include "BufferWriter.h"
#include "JobSerializer.h"

class ServiceManager;

/*---------------------------------------------------------------------------------------------
�̸�     : PacketHeader
�뵵     : TCP ������ �����Ͱ� ��Ƽ� ���� ������ Ȯ���� ���� �ʿ��� ��ü
		   _size�� ������ ������ ũ�� _protocolID�� ������ ��ſ� �� ����
������   : �̹α�
������¥ : 2022.08.19
----------------------------------------------------------------------------------------------*/
struct PacketHeader
{
	uint16 _size;
	uint16 _ProtocoliD;
};

/*---------------------------------------------------------------------------------------------
�̸�     : Session
�뵵     : Ŭ���̾�Ʈ�� ������ ���� �� ��ȣ�ۿ뿡 ����ϴ� ��ü
������   : �̹α�
������¥ : 2022.11.2
----------------------------------------------------------------------------------------------*/
class Session : public IocpObject 
{
	friend class ListenManager;
	friend class ThreadManager;

	enum
	{
		RECVBUFFER_SIZE = 0x10000,
	};

public:
	Session();
	virtual ~Session();
	virtual void IocpUpdate(IocpEventStorage* IocpStorage, int32 numOfByte) override;

	void SetServiceManager(shared_ptr<ServiceManager> service) { _serviceManager = service; }
	shared_ptr<ServiceManager> GetServiceManager() { return _serviceManager; }

	bool Connect();
	bool DisConnect(const WCHAR * cause);

	template <typename... Args>
	bool WriteSend(uint16 protocolid, Args&&... args);
	template <typename T>
	bool WriteSendtoBuffer(uint16 protocolid, T* buffer ,int32 size);

	void SetServerAddress(GWString ip, uint16 port);
	void SetServerAddress(NetAddress address);
	void SetMyAddress(GWString ip, uint16 port);
	void SetMyAddress(NetAddress address);
	
	NetAddress GetMyAddress() const { return _MyAddress; }
	NetAddress GetServerAddress() const;
	bool GetConnected() { return _Connected; }
	shared_ptr<Session> GetSession() { return reinterpret_pointer_cast<Session>(shared_from_this()); }
	bool Send(shared_ptr<class SendBuffer> sendbuffer);

	char  _AcceptBuffer[1000];

// PakcetSession���� ����� �ִ� ��Ŷ�� ������ ContentsRecv�� �����Ŵ
protected:
	virtual int32 PacketCheckRecv(BYTE* buffer, int32 datasize) { return datasize; }

// ������ �������� �����ؾ� �ϴ� �Լ�
protected:
	virtual void ContentsConnect() {};
	virtual void ContentsDisConnect() {};
	virtual void ContentsSend() {};

private:
	bool RegisterConnect();
	bool RegisterDisConnect();
	bool RegisterRecv();
	bool RegisterSend();

	bool ProcessConnect();
	bool ProcessDisConnect();
	bool ProcessRecv(int32 numofbyte);
	bool ProcessSend(int32 numOfBytes);

protected:
	// ���庸���� ������ üũ�ϴ� ����
	atomic<bool> _Send = false;

private:
	RWLOCK_USE

	NetAddress _MyAddress= {};
	::atomic<bool> _Connected = false;

	ConnectEventStorage _ConnectEventStorage = {};
	DisConnectEventStorage _DisConnectEventStorage ={};
	RecvEventStorage _RecvEventStorage ={};
	SendEventStorage _SendEventStorage = {};

	::queue<shared_ptr<SendBuffer>> _SendBufferQueue;

	RecvBuffer _RecvBuffer;

	shared_ptr<ServiceManager> _serviceManager;

};

/*---------------------------------------------------------------------------------------------
�̸�     : Session::WriteSend
�뵵     : �����͸� ���� ���� ������ �Լ�
           GSendBufferManager�� ���ؼ� SendBuffer�� �Ҵ�ް�
           PacketHeader�� size �� id�� �ְ� �����͸� ���� ����
           BufferHelper�� ���� �����͸� �ְ�
           Send�� ������ ����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.23
----------------------------------------------------------------------------------------------*/
template <typename... Args>
bool Session::WriteSend(uint16 protocolid , Args&&... args)
{
	const shared_ptr<SendBuffer> sendbuffer = GSendBufferManager->Open(0x400);
	BufferWriter BufferHelper(sendbuffer->GetBuffer(), 0x400);
	PacketHeader* packet = BufferHelper.Reserver<PacketHeader>();

	BufferHelper.WriteVarible(forward<Args>(args)...);

	packet->_size = BufferHelper.GetWriteSize();
	packet->_ProtocoliD = protocolid;

	sendbuffer->Close(BufferHelper.GetWriteSize());
	Send(sendbuffer);

	return true;
}

/*---------------------------------------------------------------------------------------------
�̸�     : Session::WriteSendtoBuffer
�뵵     : ������ �����͸� ���� ���� ������ �Լ�
		   GSendBufferManager�� ���ؼ� SendBuffer�� �Ҵ�ް�
		   PacketHeader�� size �� id�� �ְ� �����͸� ���� ����
		   BufferWriter�� ���� �����͸� �ְ�
		   Send�� ������ ����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.23
------------------------------------------------------------	----------------------------------*/
template <typename T>
bool Session::WriteSendtoBuffer(uint16 protocolid, T* buffer , int32 size)
{
	const shared_ptr<SendBuffer> sendbuffer = GSendBufferManager->Open(0x400);
	BufferWriter BufferHelper(sendbuffer->GetBuffer(), 0x400);

	PacketHeader* packet = BufferHelper.Reserver<PacketHeader>();

	BufferHelper.WriteBuffer(buffer ,size);

	packet->_size = BufferHelper.GetWriteSize();
	packet->_ProtocoliD = protocolid;

	sendbuffer->Close(BufferHelper.GetWriteSize());
	Send(sendbuffer);

	return true;
}

