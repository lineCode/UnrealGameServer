#include "pch.h"
#include "ServerPacketManager.h"

#include "GameRoom.h"
#include "Player.h"
#include "ProcedureManager.h"

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
이름     : CLIENT_LOGIN_FUNC
용도     : 클라이언트의 아이디를 통해 로그인 요청을 받는 함수
           성공할 경우 로그인 패킷을 보내주고 실패 할 경우 무시
수정자   : 이민규
수정날짜 : 2022.10.15
----------------------------------------------------------------------------------------------*/
bool CLIENT_LOGIN_FUNC(shared_ptr<ServerSession>& session, Protocol::CLIENT_LOGIN& pkt)
{
	return session->Login(&pkt);
}

/*---------------------------------------------------------------------------------------------
이름     : CLIENT_CREATEPLAYER_FUNC
용도     : 서버에 캐릭터를 생성하는 부분을 처리해주는 함수
		   클라이언트가 요청하는 값으로 캐릭터를 생성해 줌
수정자   : 이민규
수정날짜 : 2022.10.16
----------------------------------------------------------------------------------------------*/
bool CLIENT_CREATEPLAYER_FUNC(shared_ptr<ServerSession>& session, Protocol::CLIENT_CREATEPLAYER& pkt)
{
	return session->CreatePlayer(&pkt);
}

/*---------------------------------------------------------------------------------------------
이름     : CLIENT_ENTERGAME_FUNC
용도     : 서버에서 클라이언트가 선택한 캐릭터를 입장시키는 함수
		   클라이언트가 캐릭터의 이름을 보내주면 있을 경우 저장되어있는 값으로 캐릭터를 입장
수정자   : 이민규
수정날짜 : 2022.10.16
----------------------------------------------------------------------------------------------*/
bool CLIENT_ENTERGAME_FUNC(shared_ptr<ServerSession>& session, Protocol::CLIENT_ENTERGAME& pkt)
{
	return session->EnterPlayer(&pkt);
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

	room->PushAsync(&GameRoom::PlayerMove, player, &pkt);

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

	room->PushAsync(&GameRoom::PlayerSkill, player, &pkt);

	return true;
}

/*---------------------------------------------------------------------------------------------
이름     : CLIENT_EUQIPITEM_FUNC
용도     : 플레이어가 아이템 장착을 서버에 요청는 함수
수정자   : 이민규
수정날짜 : 2022.10.26
----------------------------------------------------------------------------------------------*/
bool CLIENT_EQUIPITEM_FUNC(shared_ptr<ServerSession>& session, Protocol::CLIENT_EQUIPITEM& pkt)
{
	auto clientsession = reinterpret_pointer_cast<ServerSession>(session);
	if (clientsession == nullptr)
		return false;

	auto player = clientsession->GetMyPlayer();

	auto room = player->GetRoom();
	if (room == nullptr)
		return false;


	room->PushAsync(&GameRoom::PlayerEquipItem, player, &pkt);

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

	room->PushAsync(&GameRoom::OnDamage, pkt);
}
