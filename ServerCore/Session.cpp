#include "pch.h"
#include "Session.h"
#include "SendBuffer.h"
#include "SocketUtils.h"
#include "ServiceManager.h"

Session::Session() : _RecvBuffer(RECVBUFFER_SIZE)
{
	_socket = SocketUtils::CreateSocket();
	cout << "[Session] : 생성자 호출" << endl;
}

Session::~Session()
{
	cout << "[Session] : 소멸자 호출!" << endl;
	SocketUtils::CloseSocket(_socket);
}

/*---------------------------------------------------------------------------------------------
이름     : Session::IocpUpdate
용도     : IOCP Manager에서 받은 입출력 완료 데이터를 이벤트 타입에 맞게 업데이트 해주는 함수
수정자   : 이민규
수정날짜 : 2022.08.17
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
이름     : Session::Connect
용도     : 외부에서 Connect를 사용하기 위해 필요한 함수
수정자   : 이민규
수정날짜 : 2022.08.19
----------------------------------------------------------------------------------------------*/
bool Session::Connect()
{
	return RegisterConnect();
}

/*---------------------------------------------------------------------------------------------
이름     : Session::DisConnect
용도     : 외부에서 DisConnect를 사용하기 위해 필요한 함수
		   연결을 끊고 이유를 기록하기 위해 사용하는 함수
수정자   : 이민규
수정날짜 : 2022.08.17
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
이름     : Session::SetMyAddress
용도     : 자신의 주소 설정에 사용하는 함수
수정자   : 이민규
수정날짜 : 2022.08.17
----------------------------------------------------------------------------------------------*/
void Session::SetMyAddress(GWString ip, uint16 port)
{
	const NetAddress address(ip, port);
	_MyAddress = address;
}

/*---------------------------------------------------------------------------------------------
이름     : Session::SetServerAddress
용도     : 자신의 주소 설정에 사용하는 함수
수정자   : 이민규
수정날짜 : 2022.08.17
----------------------------------------------------------------------------------------------*/
void Session::SetMyAddress(NetAddress address)
{
	_MyAddress = address;
}

/*---------------------------------------------------------------------------------------------
이름     : Session::GetServerAddress
용도     : 서버의 주소를 불러오는 함수
수정자   : 이민규
수정날짜 : 2022.08.17
----------------------------------------------------------------------------------------------*/
NetAddress Session::GetServerAddress() const
{
	return _serviceManager->GetNetAddress();
}

/*---------------------------------------------------------------------------------------------
이름     : Session::RegisterConnect
용도     : 서버에 연결 할 때 사용 하는 함수
수정자   : 이민규
수정날짜 : 2022.08.17
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
이름     : Session::ProcessConnect
용도     : 서버에 연결이 되고 난 후 사용하는 함수
수정자   : 이민규
수정날짜 : 2022.08.17
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
이름     : Session::RegisterDisConnect
용도     : 서버와 연결을 끊는 함수
수정자   : 이민규
수정날짜 : 2022.08.17
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
이름     : Session::ProcessDisConnect
용도     : 서버와 연결을 끊고 난 후 사용하는 함수
수정자   : 이민규
수정날짜 : 2022.08.17
----------------------------------------------------------------------------------------------*/
bool Session::ProcessDisConnect()
{
	_DisConnectEventStorage._Object = nullptr;
	ContentsDisConnect();

	GetServiceManager()->ReleaseSession(GetSession());
	return true;
}

/*---------------------------------------------------------------------------------------------
이름     : Session::RegisterRecv
용도     : Recv를 받기 위해 사용하는 함수
수정자   : 이민규
수정날짜 : 2022.08.18
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
이름     : Session::ProcessRecv
용도     : Recv를 받고 난 후 사용하는 함수
           PacketCheckRecv를 통해 흩어져있는 패킷을 조립 시키고 다시 RegisterRecv를 걸어줌
수정자   : 이민규
수정날짜 : 2022.08.19
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
이름     : Session::SendCheck
용도     : Send를 보내기 전 SendBufferQueue에 데이터를 모으고 한명의 쓰레드만
           Send를 담당할 수 있도록 점검 하고 RegisterSend 해주는 함수
수정자   : 이민규
수정날짜 : 2022.08.19
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
이름     : Session::RegisterSend
용도     : Send를 보내기 위해 사용하는 함수
           Scatter-Gather 기법을 이용해서 _SendBufferQueue에 보낼 데이터를 모아 놓고
           WSABUF에 전부 옮긴 후 한번에 보냄
수정자   : 이민규
수정날짜 : 2022.08.19
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
이름     : Session::ProcessSend
용도     : Send를 받고 나서 사용하는 함수
           SendBufferQueue가 비어 있을 경우 종료하지만 비어있지 않을 경우
           이 쓰레드가 다시 RegisterSend를 하게 함으로 써
           한 개의 쓰레드만 한 세션에 대해 Send하도록 함
수정자   : 이민규
수정날짜 : 2022.08.18
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

