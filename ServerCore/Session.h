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
이름     : PacketHeader
용도     : TCP 구조상 데이터가 모아서 오기 때문에 확인을 위해 필요한 객체
		   _size는 보내는 데이터 크기 _protocolID는 앞으로 통신에 쓸 숫자
수정자   : 이민규
수정날짜 : 2022.08.19
----------------------------------------------------------------------------------------------*/
struct PacketHeader
{
	uint16 _size;
	uint16 _ProtocoliD;
};

/*---------------------------------------------------------------------------------------------
이름     : Session
용도     : 클라이언트와 서버가 연결 후 상호작용에 사용하는 객체
수정자   : 이민규
수정날짜 : 2022.11.2
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

// PakcetSession에서 흩어져 있는 패킷을 모으고 ContentsRecv를 실행시킴
protected:
	virtual int32 PacketCheckRecv(BYTE* buffer, int32 datasize) { return datasize; }

// 콘텐츠 영역에서 구현해야 하는 함수
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
	// 샌드보내는 중인지 체크하는 변수
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
이름     : Session::WriteSend
용도     : 데이터를 직접 만들어서 보내는 함수
           GSendBufferManager을 통해서 SendBuffer을 할당받고
           PacketHeader로 size 와 id를 넣고 데이터를 넣은 다음
           BufferHelper를 통해 데이터를 넣고
           Send로 보내서 등록하는 함수
수정자   : 이민규
수정날짜 : 2022.08.23
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
이름     : Session::WriteSendtoBuffer
용도     : 버퍼형 데이터를 직접 만들어서 보내는 함수
		   GSendBufferManager을 통해서 SendBuffer을 할당받고
		   PacketHeader로 size 와 id를 넣고 데이터를 넣은 다음
		   BufferWriter를 통해 데이터를 넣고
		   Send로 보내서 등록하는 함수
수정자   : 이민규
수정날짜 : 2022.08.23
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

