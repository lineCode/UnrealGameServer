#include "pch.h"
#include "Session.h"
#include "SendBuffer.h"
#include "SocketUtils.h"
#include "ServiceManager.h"

Session::Session() : _RecvBuffer(RECVBUFFER_SIZE)
{
	_socket = SocketUtils::CreateSocket();
	cout << "[Session] : ������ ȣ��" << endl;
}

Session::~Session()
{
	cout << "[Session] : �Ҹ��� ȣ��!" << endl;
	SocketUtils::CloseSocket(_socket);
}

/*---------------------------------------------------------------------------------------------
�̸�     : Session::IocpUpdate
�뵵     : IOCP Manager���� ���� ����� �Ϸ� �����͸� �̺�Ʈ Ÿ�Կ� �°� ������Ʈ ���ִ� �Լ�
������   : �̹α�
������¥ : 2022.08.17
----------------------------------------------------------------------------------------------*/
void Session::IocpUpdate(IocpEventStorage* IocpStorage, int32 numOfByte)
{
	switch (IocpStorage->_Type)
	{
	case IocpType::DisConnect:
		ProcessDisConnect();
		break;

	case IocpType::Connect:
		ProcessConnect();
		break;

	case IocpType::Recv:
		ProcessRecv(numOfByte);
		break;

	case IocpType::Send:
		ProcessSend(numOfByte);
		break;

	default:
		cout << "[Session] : Update Error" << endl;
		break;
	}
}

/*---------------------------------------------------------------------------------------------
�̸�     : Session::Connect
�뵵     : �ܺο��� Connect�� ����ϱ� ���� �ʿ��� �Լ�
������   : �̹α�
������¥ : 2022.08.19
----------------------------------------------------------------------------------------------*/
bool Session::Connect()
{
	return RegisterConnect();
}

/*---------------------------------------------------------------------------------------------
�̸�     : Session::DisConnect
�뵵     : �ܺο��� DisConnect�� ����ϱ� ���� �ʿ��� �Լ�
		   ������ ���� ������ ����ϱ� ���� ����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.17
----------------------------------------------------------------------------------------------*/
bool Session::DisConnect(const WCHAR* cause)
{
	if (_Connected.exchange(false) == false)
		return false;

	wcout << "[DisConnect] : " << cause << endl;
	RegisterDisConnect();

	return true;
}

/*---------------------------------------------------------------------------------------------
�̸�     : Session::SetMyAddress
�뵵     : �ڽ��� �ּ� ������ ����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.17
----------------------------------------------------------------------------------------------*/
void Session::SetMyAddress(GWString ip, uint16 port)
{
	const NetAddress address(ip, port);
	_MyAddress = address;
}

/*---------------------------------------------------------------------------------------------
�̸�     : Session::SetServerAddress
�뵵     : �ڽ��� �ּ� ������ ����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.17
----------------------------------------------------------------------------------------------*/
void Session::SetMyAddress(NetAddress address)
{
	_MyAddress = address;
}

/*---------------------------------------------------------------------------------------------
�̸�     : Session::GetServerAddress
�뵵     : ������ �ּҸ� �ҷ����� �Լ�
������   : �̹α�
������¥ : 2022.08.17
----------------------------------------------------------------------------------------------*/
NetAddress Session::GetServerAddress() const
{
	return _serviceManager->GetNetAddress();
}

/*---------------------------------------------------------------------------------------------
�̸�     : Session::RegisterConnect
�뵵     : ������ ���� �� �� ��� �ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.17
----------------------------------------------------------------------------------------------*/
bool Session::RegisterConnect()
{
	if (GetConnected())
		return false;

	if (SocketUtils::SocketGameSetting(_socket) == false)
		return false;

	if (SocketUtils::DefaultBind(_socket , 0) == false)
		return false;

	_ConnectEventStorage.Init();
	_ConnectEventStorage._Object = shared_from_this();

	DWORD numOfBytes = 0;
	if(false == SocketUtils::ConnectEx(_socket , GetServerAddress().GetSockAddrptr(), sizeof(SOCKADDR_IN) , nullptr, 0 , &numOfBytes, &_ConnectEventStorage))
	{
		int32 Errcode = ::WSAGetLastError();
		if(Errcode != WSA_IO_PENDING)
		{
			_ConnectEventStorage._Object = nullptr;
			return false;
		}
	}

	return true;
}

/*---------------------------------------------------------------------------------------------
�̸�     : Session::ProcessConnect
�뵵     : ������ ������ �ǰ� �� �� ����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.17
----------------------------------------------------------------------------------------------*/
bool Session::ProcessConnect()
{
	_ConnectEventStorage._Object = nullptr;
	_Connected.store(true);

	/// <summary>

	GetServiceManager()->AddSession(GetSession());

	ContentsConnect();
	RegisterRecv();

	return true;
}

/*---------------------------------------------------------------------------------------------
�̸�     : Session::RegisterDisConnect
�뵵     : ������ ������ ���� �Լ�
������   : �̹α�
������¥ : 2022.08.17
----------------------------------------------------------------------------------------------*/
bool Session::RegisterDisConnect()
{
	_DisConnectEventStorage.Init();
	_DisConnectEventStorage._Object = shared_from_this();

	if(false == SocketUtils::DisConnectEx(_socket , &_DisConnectEventStorage , TF_REUSE_SOCKET , 0))
	{
		int32 Errcode = ::WSAGetLastError();
		if (Errcode != WSA_IO_PENDING)
		{
			_DisConnectEventStorage._Object = nullptr;
			return false;
		}
	}

	return true;
}

/*---------------------------------------------------------------------------------------------
�̸�     : Session::ProcessDisConnect
�뵵     : ������ ������ ���� �� �� ����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.17
----------------------------------------------------------------------------------------------*/
bool Session::ProcessDisConnect()
{
	_DisConnectEventStorage._Object = nullptr;
	ContentsDisConnect();

	GetServiceManager()->ReleaseSession(GetSession());
	return true;
}

/*---------------------------------------------------------------------------------------------
�̸�     : Session::RegisterRecv
�뵵     : Recv�� �ޱ� ���� ����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.18
----------------------------------------------------------------------------------------------*/
bool Session::RegisterRecv()
{
	if (GetConnected() == false)
		return false;

	_RecvEventStorage.Init();
	_RecvEventStorage._Object = shared_from_this();

	WSABUF wsaBuf;
	wsaBuf.buf =  reinterpret_cast<char*>(_RecvBuffer.GetWriteBuffer());
	wsaBuf.len = _RecvBuffer.GetFreeSize();

	DWORD NumofByte = 0;
	DWORD flags = 0;

	if(SOCKET_ERROR == ::WSARecv(_socket , &wsaBuf , 1 ,&NumofByte , &flags , &_RecvEventStorage ,nullptr))
	{
		int32 Errcode = ::WSAGetLastError();
		if (Errcode != WSA_IO_PENDING)
		{
			_RecvEventStorage._Object = nullptr;
			return false;
		}
	}

	return true;

}

/*---------------------------------------------------------------------------------------------
�̸�     : Session::ProcessRecv
�뵵     : Recv�� �ް� �� �� ����ϴ� �Լ�
           PacketCheckRecv�� ���� ������ִ� ��Ŷ�� ���� ��Ű�� �ٽ� RegisterRecv�� �ɾ���
������   : �̹α�
������¥ : 2022.08.19
----------------------------------------------------------------------------------------------*/
bool Session::ProcessRecv(int32 numofbyte)
{
	_RecvEventStorage._Object = nullptr;

	if(numofbyte == 0)
	{
		DisConnect(L"Recv Data 0");
		return false;
	}

	if(_RecvBuffer.SetWritePos(numofbyte) == false)
	{
		DisConnect(L"ReadBuffer Error");
		return false;
	}

	int32 datasize = _RecvBuffer.GetDataSize();

	int32 readlen = PacketCheckRecv(_RecvBuffer.GetReadBuffer() , _RecvBuffer.GetDataSize());
	if(readlen < 0 || datasize < readlen || _RecvBuffer.SetReadPos(readlen) == false)
	{
		DisConnect(L"PackCheckRedv Error");
		return false;
	}


	_RecvBuffer.Clear();
	RegisterRecv();
	return true;
}

/*---------------------------------------------------------------------------------------------
�̸�     : Session::SendCheck
�뵵     : Send�� ������ �� SendBufferQueue�� �����͸� ������ �Ѹ��� �����常
           Send�� ����� �� �ֵ��� ���� �ϰ� RegisterSend ���ִ� �Լ�
������   : �̹α�
������¥ : 2022.08.19
----------------------------------------------------------------------------------------------*/
bool Session::SendCheck(shared_ptr<SendBuffer> sendbuffer)
{
	if (GetConnected() == false)
		return false;

	bool sended = false;

	{
		WRITELOCK;

		_SendBufferQueue.push(sendbuffer);

		if (_Sended.exchange(true) == false)
			sended = true;
	}

	if (sended)
		RegisterSend();

	return true;
}

/*---------------------------------------------------------------------------------------------
�̸�     : Session::RegisterSend
�뵵     : Send�� ������ ���� ����ϴ� �Լ�
           Scatter-Gather ����� �̿��ؼ� _SendBufferQueue�� ���� �����͸� ��� ����
           WSABUF�� ���� �ű� �� �ѹ��� ����
������   : �̹α�
������¥ : 2022.08.19
----------------------------------------------------------------------------------------------*/
bool Session::RegisterSend()
{
	if (GetConnected() == false)
		return false;
	
	_SendEventStorage.Init();
	_SendEventStorage._Object = shared_from_this();

	{
		WRITELOCK;

		int32 writesize = 0;

		while (_SendBufferQueue.empty() == false)
		{
			shared_ptr<SendBuffer> sendbuffer = _SendBufferQueue.front();
			writesize += sendbuffer->GetWriteSize();

			_SendBufferQueue.pop();
			_SendEventStorage._SendBuffers.push_back(sendbuffer);
		}
	}

	Gvector<WSABUF> wsaBufs;
	wsaBufs.reserve(_SendEventStorage._SendBuffers.size());
	for(auto Sendbuffer : _SendEventStorage._SendBuffers)
	{
		WSABUF wsabuf;
		wsabuf.buf = reinterpret_cast<char*>(Sendbuffer->GetBuffer());
		wsabuf.len = static_cast<ULONG>(Sendbuffer->GetWriteSize());
		wsaBufs.push_back(wsabuf);
	}

	DWORD NumofByte = 0;
	if (SOCKET_ERROR == ::WSASend(_socket, wsaBufs.data(),  static_cast<DWORD>(wsaBufs.size()), &NumofByte, 0, &_SendEventStorage, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_SendEventStorage._Object = nullptr;
			_SendEventStorage._SendBuffers.clear();
			_Sended.store(false);
			return false;
		}
	}

	return true;
}

/*---------------------------------------------------------------------------------------------
�̸�     : Session::ProcessSend
�뵵     : Send�� �ް� ���� ����ϴ� �Լ�
           SendBufferQueue�� ��� ���� ��� ���������� ������� ���� ���
           �� �����尡 �ٽ� RegisterSend�� �ϰ� ������ ��
           �� ���� �����常 �� ���ǿ� ���� Send�ϵ��� ��
������   : �̹α�
������¥ : 2022.08.18
----------------------------------------------------------------------------------------------*/
bool Session::ProcessSend(int32 numOfBytes)
{
	_SendEventStorage._Object = nullptr;
	_SendEventStorage._SendBuffers.clear();

	if(numOfBytes == 0)
	{
		DisConnect(L"Send Data 0");
		return false;
	}

	ContentsSend();

	{
		WRITELOCK;
		if (_SendBufferQueue.empty())
			_Sended.store(false);
		else
			RegisterSend();
	}

	return true;
}

