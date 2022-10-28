#include "pch.h"
#include "ServerPacketManager.h"

#include "GameRoom.h"
#include "Player.h"
#include "ProcedureManager.h"

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
�̸�     : CLIENT_LOGIN_FUNC
�뵵     : Ŭ���̾�Ʈ�� ���̵� ���� �α��� ��û�� �޴� �Լ�
           ������ ��� �α��� ��Ŷ�� �����ְ� ���� �� ��� ����
������   : �̹α�
������¥ : 2022.10.15
----------------------------------------------------------------------------------------------*/
bool CLIENT_LOGIN_FUNC(shared_ptr<ServerSession>& session, Protocol::CLIENT_LOGIN& pkt)
{
	return session->Login(&pkt);
}

/*---------------------------------------------------------------------------------------------
�̸�     : CLIENT_CREATEPLAYER_FUNC
�뵵     : ������ ĳ���͸� �����ϴ� �κ��� ó�����ִ� �Լ�
		   Ŭ���̾�Ʈ�� ��û�ϴ� ������ ĳ���͸� ������ ��
������   : �̹α�
������¥ : 2022.10.16
----------------------------------------------------------------------------------------------*/
bool CLIENT_CREATEPLAYER_FUNC(shared_ptr<ServerSession>& session, Protocol::CLIENT_CREATEPLAYER& pkt)
{
	return session->CreatePlayer(&pkt);
}

/*---------------------------------------------------------------------------------------------
�̸�     : CLIENT_ENTERGAME_FUNC
�뵵     : �������� Ŭ���̾�Ʈ�� ������ ĳ���͸� �����Ű�� �Լ�
		   Ŭ���̾�Ʈ�� ĳ������ �̸��� �����ָ� ���� ��� ����Ǿ��ִ� ������ ĳ���͸� ����
������   : �̹α�
������¥ : 2022.10.16
----------------------------------------------------------------------------------------------*/
bool CLIENT_ENTERGAME_FUNC(shared_ptr<ServerSession>& session, Protocol::CLIENT_ENTERGAME& pkt)
{
	return session->EnterPlayer(&pkt);
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

	room->PushAsync(&GameRoom::PlayerMove, player, &pkt);

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

	auto room = player->GetRoom();
	if (room == nullptr)
		return false;

	room->PushAsync(&GameRoom::PlayerSkill, player, &pkt);

	return true;
}

/*---------------------------------------------------------------------------------------------
�̸�     : CLIENT_EUQIPITEM_FUNC
�뵵     : �÷��̾ ������ ������ ������ ��û�� �Լ�
������   : �̹α�
������¥ : 2022.10.26
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
�̸�     : CLIENT_DAMAGE_FUNC
�뵵     : ũ������ ������ ��Ŷ�� �޾Ƽ� �ٸ� �÷��̾�� ���� �����ִ� �Լ�
������   : �̹α�
������¥ : 2022.10.03
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
