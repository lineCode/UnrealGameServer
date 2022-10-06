#include "pch.h"
#include "ServerSession.h"

#include "GameRoom.h"
#include "Player.h"
#include "ObjectManager.h"
#include "RoomManager.h"
#include "ServerPacketManager.h"

/*---------------------------------------------------------------------------------------------
�̸�     : ServerSession::ContentsConnect
�뵵     : ������ ������ �������� �� �ߵ��ϴ� �Լ�
           ������ �Ǿ����� Ŭ���̾�Ʈ���� ��Ŷ�� �����ؼ� �˷���
������   : �̹α�
������¥ : 2022.09.24
----------------------------------------------------------------------------------------------*/
void ServerSession::ContentsConnect()
{
	Protocol::SERVER_CONNECT connectpacket;
	connectpacket.set_connect(1);
		
	SendCheck( ServerPacketManager::MakeSendBuffer(connectpacket));
}

/*---------------------------------------------------------------------------------------------
�̸�     : ServerSession::ContentsDisConnect
�뵵     : �������� ������ �������� ��� �ߵ��ϴ� �Լ�
		   �÷��̾ �濡�� �����Ű�� �÷��̾ �������� ������
������   : �̹α�
������¥ : 2022.09.12
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
�̸�     : ServerSession::ContentsRecv
�뵵     : �������� Ŭ���� Send�� �޾��� �� �ߵ��ϴ� �Լ�
		   �ϼ��� ��Ŷ�� ���� ��Ŷ�� �°� PacketUpdate�� ���� ó������
������   : �̹α�
������¥ : 2022.09.12
----------------------------------------------------------------------------------------------*/
void ServerSession::ContentsRecv(BYTE* buffer, int32 datasize)
{
	shared_ptr<ServerSession> session = GetServerSession();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	
	ServerPacketManager::PacketUpdate(session, buffer, datasize);
}
