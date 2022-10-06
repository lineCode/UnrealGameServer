#include "pch.h"
#include "ServerSession.h"

#include "GameRoom.h"
#include "Player.h"
#include "ObjectManager.h"
#include "RoomManager.h"
#include "ServerPacketManager.h"

/*---------------------------------------------------------------------------------------------
이름     : ServerSession::ContentsConnect
용도     : 서버에 연결이 성공했을 때 발동하는 함수
           연결이 되었음을 클라이언트에게 패킷을 전달해서 알려줌
수정자   : 이민규
수정날짜 : 2022.09.24
----------------------------------------------------------------------------------------------*/
void ServerSession::ContentsConnect()
{
	Protocol::SERVER_CONNECT connectpacket;
	connectpacket.set_connect(1);
		
	SendCheck( ServerPacketManager::MakeSendBuffer(connectpacket));
}

/*---------------------------------------------------------------------------------------------
이름     : ServerSession::ContentsDisConnect
용도     : 서버에서 연결이 끊어졌을 경우 발동하는 함수
		   플레이어를 방에서 퇴장시키고 플레이어를 서버에서 제거함
수정자   : 이민규
수정날짜 : 2022.09.12
----------------------------------------------------------------------------------------------*/
void ServerSession::ContentsDisConnect()
{
	auto room = RoomManager::GetInstance().Find(1);
	room->PushAsync(&GameRoom::LeaveGame, _MyPlayer->GetInfo().objectid());
}

void ServerSession::ContentsSend()
{
	PacketSession::ContentsSend();
}

/*---------------------------------------------------------------------------------------------
이름     : ServerSession::ContentsRecv
용도     : 서버에서 클라의 Send를 받았을 때 발동하는 함수
		   완성된 패킷만 오며 패킷에 맞게 PacketUpdate를 통해 처리해줌
수정자   : 이민규
수정날짜 : 2022.09.12
----------------------------------------------------------------------------------------------*/
void ServerSession::ContentsRecv(BYTE* buffer, int32 datasize)
{
	shared_ptr<ServerSession> session = GetServerSession();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	
	ServerPacketManager::PacketUpdate(session, buffer, datasize);
}
