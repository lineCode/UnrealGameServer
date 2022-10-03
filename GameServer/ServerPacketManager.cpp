#include "pch.h"
#include "ServerPacketManager.h"
#include "DataManager.h"
#include "GameRoom.h"
#include "ObjectManager.h"
#include "ObjectUtils.h"
#include "Player.h"
#include "RoomManager.h"

PacketFunc GPacketFuncArray[UINT16_MAX];

/*---------------------------------------------------------------------------------------------
이름     : PACKET_INVALID
용도     : 비정상적인 패킷일 때 사용되는 함수
수정자   : 이민규
수정날짜 : 2022.08.29
----------------------------------------------------------------------------------------------*/
bool PACKET_INVALID(shared_ptr<ServerSession>& session, BYTE* byte, int32 len)
{
	return true;
}

/*---------------------------------------------------------------------------------------------
이름     : CLIENT_ENTERGAME_FUNC
용도     : 플레이어를 타입에 맞게 생성하고 방으로 입장시켜주는 함수
수정자   : 이민규
수정날짜 : 2022.09.24
----------------------------------------------------------------------------------------------*/
bool CLIENT_ENTERGAME_FUNC(shared_ptr<ServerSession>& session, Protocol::CLIENT_ENTERGAME& pkt)
{
	auto playerinfo = pkt.playerinfo();

	Player* player = ObjectManager::GetInstance().Add<Player>(pkt.objecttype());

	session->SetMyPlayer(player);
	player->GetInfo().set_name(pkt.playerinfo().GetTypeName());
	player->SetStat(DataManager::GetInstacnce()->GetPlayerStatData(1));
	player->SetVector(pkt.playerinfo().vector());
	player->SetRotator(pkt.playerinfo().rotator());
	player->SetSession(session->GetServerSession());

	RoomManager::GetInstance().Find(1)->EnterGame(player);
	return true;
}

/*---------------------------------------------------------------------------------------------
이름     : CLIENT_MOVE_FUNC
용도     : 플레이어의 좌표를 받아서 다른 플레이어에게 좌표를 갱신시켜주는 함수
수정자   : 이민규
수정날짜 : 2022.09.16
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
이름     : CLIENT_SKILL_FUNC
용도     : 플레이어 스킬 패킷을 받아서 다른 플레이어에게 갱신 시켜주는 함수
수정자   : 이민규
수정날짜 : 2022.09.16
----------------------------------------------------------------------------------------------*/
bool CLIENT_SKILL_FUNC(shared_ptr<ServerSession>& session, Protocol::CLIENT_SKILL& pkt)
{
	auto clientsession = reinterpret_pointer_cast<ServerSession>(session);
	if (clientsession == nullptr)
		return false;

	auto player = clientsession->GetMyPlayer();


	auto room = player->GetRoom();
	if (room == nullptr)
		return false;

	room->PlayerSkill(player, pkt);

	return true;
}

/*---------------------------------------------------------------------------------------------
이름     : CLIENT_DAMAGE_FUNC
용도     : 크리쳐의 데미지 패킷을 받아서 다른 플레이어에게 갱신 시켜주는 함수
수정자   : 이민규
수정날짜 : 2022.10.03
----------------------------------------------------------------------------------------------*/
bool CLIENT_DAMAGE_FUNC(shared_ptr<ServerSession>& session, Protocol::CLIENT_DAMAGE& pkt)
{
	auto clientsession = reinterpret_pointer_cast<ServerSession>(session);
	if (clientsession == nullptr)
		return false;

	auto player = clientsession->GetMyPlayer();

	auto room = player->GetRoom();
	if (room == nullptr)
		return false;

	room->OnDamage(pkt);

}
