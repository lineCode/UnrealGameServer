#include "pch.h"
#include "ServerPacketManager.h"
#include "GameRoom.h"
#include "ObjectManager.h"
#include "Player.h"
#include "RoomManager.h"

PacketFunc GPacketFuncArray[UINT16_MAX];

/*---------------------------------------------------------------------------------------------
�̸�     : PACKET_INVALID
�뵵     : ���������� ��Ŷ�� �� ���Ǵ� �Լ�
������   : �̹α�
������¥ : 2022.08.29
----------------------------------------------------------------------------------------------*/
bool PACKET_INVALID(shared_ptr<ServerSession>& session, BYTE* byte, int32 len)
{
	return true;
}

/*---------------------------------------------------------------------------------------------
�̸�     : CLIENT_ENTERGAME_FUNC
�뵵     : �÷��̾ Ÿ�Կ� �°� �����ϰ� ������ ��������ִ� �Լ�
������   : �̹α�
������¥ : 2022.09.24
----------------------------------------------------------------------------------------------*/
bool CLIENT_ENTERGAME_FUNC(shared_ptr<ServerSession>& session, Protocol::CLIENT_ENTERGAME& pkt)
{
	auto playerinfo = pkt.playerinfo();

	Player* player = ObjectManager::GetInstance().Add<Player>(pkt.objecttype());

	player->GetInfo().set_name(pkt.playerinfo().GetTypeName());
	session->SetMyPlayer(player);
	player->SetVector(pkt.playerinfo().vector());
	player->SetRotator(pkt.playerinfo().rotator());
	player->SetSession(session->GetServerSession());

	RoomManager::GetInstance().Find(1)->EnterGame(player);
	return true;
}

/*---------------------------------------------------------------------------------------------
�̸�     : CLIENT_MOVE_FUNC
�뵵     : �÷��̾��� ��ǥ�� �޾Ƽ� �ٸ� �÷��̾�� ��ǥ�� ���Ž����ִ� �Լ�
������   : �̹α�
������¥ : 2022.09.16
----------------------------------------------------------------------------------------------*/
bool CLIENT_MOVE_FUNC(shared_ptr<ServerSession>& session, Protocol::CLIENT_MOVE& pkt)
{
	auto clientsession = reinterpret_pointer_cast<ServerSession>(session);
	if (clientsession == nullptr)
		return false;

	auto player = clientsession->GetMyPlayer();
	if (player == nullptr)
		return false;

	auto room = player->GetRoom();
	if (room == nullptr)
		return false;

	room->PlayerMove(player, pkt);

	return true;
}

/*---------------------------------------------------------------------------------------------
�̸�     : CLIENT_SKILL_FUNC
�뵵     : �÷��̾� ��ų ��Ŷ�� �޾Ƽ� �ٸ� �÷��̾�� ���� �����ִ� �Լ�
������   : �̹α�
������¥ : 2022.09.16
----------------------------------------------------------------------------------------------*/
bool CLIENT_SKILL_FUNC(shared_ptr<ServerSession>& session, Protocol::CLIENT_SKILL& pkt)
{
	auto clientsession = reinterpret_pointer_cast<ServerSession>(session);
	if (clientsession == nullptr)
		return false;

	auto player = clientsession->GetMyPlayer();
	if (player == nullptr)
		return false;

	auto room = player->GetRoom();
	if (room == nullptr)
		return false;

	room->PlayerSkill(player, pkt);

	return true;
}
