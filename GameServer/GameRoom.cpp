#include "pch.h"
#include "GameRoom.h"
#include "DataManager.h"
#include "Player.h"
#include "Monster.h"
#include "Projectile.h"
#include "ObjectManager.h"
#include "Protocol.pb.h"
#include "ServerPacketManager.h"
#include "ServerSession.h"
#include "ObjectUtils.h"

GameRoom::GameRoom()
{
	
}

/*---------------------------------------------------------------------------------------------
�̸�     : GameRoom::Init
�뵵     : ���� �ʱ�ȭ �ϴ� �κ�
������   : �̹α�
������¥ : 2022.09.19
----------------------------------------------------------------------------------------------*/
void GameRoom::Init(int32 mapid)
{
	Monster * monster = ObjectManager::GetInstance().Add<Monster>(Protocol::MonsterType::SAVAROG);
	Protocol::Vector vector;
	monster->SetStat(DataManager::GetInstacnce()->GetPlayerStatData(3));
	vector.set_x(-2200.f);
	vector.set_y(-150.f);
	vector.set_z(97.6f);
	monster->SetVector(vector);
	monster->SetRoom(GetGameRoom());

	PushAsync(&GameRoom::EnterGame, static_cast<GameObject*>(monster));
}

/*---------------------------------------------------------------------------------------------
�̸�     : GameRoom::EnterGame
�뵵     : �÷��̾ �濡 �����Ű�� �Լ�
������   : �̹α�
������¥ : 2022.09.19
----------------------------------------------------------------------------------------------*/
void GameRoom::EnterGame(GameObject* gameobject)
{
	if (gameobject == nullptr)
		return;

	Protocol::ObjectType type = ObjectUtils::GetObjectType(gameobject->GetId());

	// TODO : �������� ���� ����
	{
		switch (type)
		{
		case Protocol::ObjectType::PLAYER:
		{
			// TODO : �� �ڽſ��� �������� �˷���
			Player* player = reinterpret_cast<Player*>(gameobject);
			if (player == nullptr)
				return;

			Protocol::SERVER_ENTERGAME enterpacket;
			ObjectUtils::SetEnterPacket(enterpacket, gameobject);
			_Players.insert({ player->GetId() , player });
			player->SetRoom(GetGameRoom());
			player->GetSession()->SendCheck(ServerPacketManager::MakeSendBuffer(enterpacket));

			// TODO : ���� �濡 �ִ� �÷��̾���� �����ϱ� ���� ������ ������ ����
			if (!_Players.empty())
			{
				Protocol::SERVER_SPAWN spawnpacket;
				for (const auto [id, roomplayer] : _Players)
				{
					if (id != gameobject->GetId())
						ObjectUtils::SetSpawnPacket(spawnpacket, roomplayer);
				}

				for (const auto [id, roommonster] : _Monsters)
					ObjectUtils::SetSpawnPacket(spawnpacket, roommonster);

				player->GetSession()->SendCheck(ServerPacketManager::MakeSendBuffer(spawnpacket));
			}
			break;
		}

		case Protocol::ObjectType::MONSTER:
		{
			// TODO : ���͸� _Monsters�� ����ϴ� �κ�
			Monster* monster = reinterpret_cast<Monster*>(gameobject);
			_Monsters.insert({ monster->GetId() , monster });
			monster->SetRoom(GetGameRoom());
			break;
		}

		case Protocol::ObjectType::PROJECTILE:
		{
			// TODO : �߻�ü�� _Projectiles�� ����ϴ� �κ�
			Projectile* projectile = reinterpret_cast<Projectile*>(gameobject);
			_Projectiles.insert({ projectile->GetId() , projectile });
			projectile->SetRoom(GetGameRoom());
			break;
		}

		default:
		{
			// TODO : ���� �κ�
			cout << "[GameRoom] : ObjectType Missing Error" << endl;
			break;
		}
		}

	}

	// TODO : �����濡 �ִ� �ٸ� �÷��̾�Ե� ������ ���� ������ ����
	{
		Protocol::SERVER_SPAWN spawnpacket;

		switch (type)
		{
		case Protocol::ObjectType::PROJECTILE:
		{
			// TODO : �߻�ü�� ���
			if (_Projectiles.contains(gameobject->GetId()) == false)
				return;

			int32 ownerId = _Projectiles[gameobject->GetId()]->GetOwner()->GetId();

			auto ownerpacket = ObjectUtils::SetSpawnPacket(spawnpacket, gameobject);

			for (const auto [id, roomplayer] : _Players)
				roomplayer->GetSession()->SendCheck(ServerPacketManager::MakeSendBuffer(spawnpacket));

			break;
		}

		default:
		{
			// TODO : �� �� ��� ���
			ObjectUtils::SetSpawnPacket(spawnpacket, gameobject);

			for (const auto [id, roomplayer] : _Players)
			{
				if (id != gameobject->GetId())
					roomplayer->GetSession()->SendCheck(ServerPacketManager::MakeSendBuffer(spawnpacket));
			}
			break;
		}

		}

	}
}

/*---------------------------------------------------------------------------------------------
�̸�     : GameRoom::LeaveGame
�뵵     : ������Ʈ�� �濡�� �����Ű�� �Լ�
������   : �̹α�
������¥ : 2022.09.19
----------------------------------------------------------------------------------------------*/
void GameRoom::LeaveGame(int32 objectid)
{
	Protocol::ObjectType type = ObjectUtils::GetObjectType(objectid);

	switch (type)
	{
		case Protocol::ObjectType::PLAYER:
		{
			// TODO : �÷��̾ �濡�� ���Ž�Ű�� �κ�
			if (ObjectManager::GetInstance().Find(objectid) == nullptr)
				return;

			Player * player = _Players[objectid];

			// TODO : �ڽſ��� �濡�� �����ߴٴ� ���� ����
			Protocol::SERVER_LEAVEGAME leavepacket;
			player->GetSession()->SendCheck(ServerPacketManager::MakeSendBuffer(leavepacket));

			// TODO : �÷��̾ �濡�� ������ �Լ� ȣ��
			player->LeaveGame();

			// TODO : ���� �濡���� �÷��̾� ����
			_Players.erase(objectid);

			// TODO : ObjectManager���� �÷��̾ ����
			ObjectManager::GetInstance().Remove(objectid);
			break;
		}

		case Protocol::ObjectType::MONSTER:
		{
			// TODO : ���͸� �濡�� ���Ž�Ű�� �κ�
			if (_Monsters.contains(objectid) == false)
				return;

			Gdelete(_Monsters[objectid]);
			_Monsters.erase(objectid);
			break;
		}

		case Protocol::ObjectType::PROJECTILE:
		{
			// TODO : �߻�ü�� �濡�� ���Ž�Ű�� �κ�
			if (_Projectiles.contains(objectid) == false)
				return;

			Gdelete(_Projectiles[objectid]);
			_Projectiles.erase(objectid);

			break;
		}
	}

	// TODO : ���� �濡 �ִ� �÷��̾�� ������ ���� ���� ����
	{
		Protocol::SERVER_DESTROY destroypacket;
		destroypacket.add_objectids(objectid);

		for (const auto [id, roomplayer] : _Players)
		{
			if (objectid != roomplayer->GetId())
				roomplayer->GetSession()->SendCheck(ServerPacketManager::MakeSendBuffer(destroypacket));
		}
	}

}

/*---------------------------------------------------------------------------------------------
�̸�     : GameRoom::PlayerMove
�뵵     : �濡 �ִ� �÷��̾�鿡�� �̵��� �÷��̾��� ��ǥ ��Ŷ�� �����ִ� �Լ�
������   : �̹α�
������¥ : 2022.09.28
----------------------------------------------------------------------------------------------*/
void GameRoom::PlayerMove(Player* player, Protocol::CLIENT_MOVE pkt)
{
	if (player == nullptr)
		return;

	// TODO : �������� ���� �ʿ� �߰� ����
	printf("[CLIENT_MOVE_FUNC_ID[%d]] : %.1lf %.1lf %.1lf\n", player->GetInfo().objectid(), pkt.vector().x(), pkt.vector().y(), pkt.vector().z());

	// TODO : �ϴ� �������� ���� ��ǥ �� ���� �̵�
	player->SetVector(pkt.vector());
	player->SetRotator(pkt.rotator());

	// TODO : �ٸ� �÷��̾����׵� �˷��ش�
	Protocol::SERVER_MOVE movepacket;
	ObjectUtils::SetMovePacket(movepacket, player);
	BroadCast(ServerPacketManager::MakeSendBuffer(movepacket));
}

/*---------------------------------------------------------------------------------------------
�̸�     : GameRoom::PlayerSkill
�뵵     : �濡 �ִ� �÷��̾�鿡�� ��ų�� ����� �÷��̾��� ��ų ��Ŷ�� �����ִ� �Լ�
������   : �̹α�
������¥ : 2022.09.25
----------------------------------------------------------------------------------------------*/
void GameRoom::PlayerSkill(Player* player, Protocol::CLIENT_SKILL pkt)
{
	if (player == nullptr)
		return;

	// TODO : �÷��̾� ��ų ����� ����

	Protocol::SERVER_SKILL skillpacket;
	ObjectUtils::SetSkillPacket(skillpacket, player, pkt);
	BroadCast(ServerPacketManager::MakeSendBuffer(skillpacket));

	printf("[CLINET_Skill_FUNC_ID[%d]] : %d Use\n", skillpacket.objectid(), skillpacket.skillinfo().skillid());

	// TODO : ��ų �浹üũ �ϴ��� Ŭ���Ʈ���� ��� �±�

	// TODO : ��Ÿ�� ������ �߰� ����
}

/*---------------------------------------------------------------------------------------------
�̸�     : GameRoom::OnDamage
�뵵     : �濡 �ִ� ũ���Ŀ��� �������� �ִ� �Լ�
������   : �̹α�
������¥ : 2022.10.03
----------------------------------------------------------------------------------------------*/
void GameRoom::OnDamage(Protocol::CLIENT_DAMAGE pkt)
{
	Protocol::ObjectType type = ObjectUtils::GetObjectType(pkt.victimeid());

	switch (type)
	{
	case Protocol::PLAYER:
	{
		if (_Players.contains(pkt.victimeid()) == false || _Monsters.contains(pkt.attackerid()) == false)
			return;

		_Players[pkt.victimeid()]->OnDamaged(_Monsters[pkt.attackerid()], pkt.damage());
		break;
	}

	case Protocol::MONSTER:
	{
		if (_Monsters.contains(pkt.victimeid()) == false || _Players.contains(pkt.attackerid()) == false)
			return;

		_Monsters[pkt.victimeid()]->OnDamaged(_Players[pkt.attackerid()], pkt.damage());
		break;
	}

	default:
		{
		cout << format("[GameRoom] : OnDamage Error") << endl;
		};
	}

	cout << format("Damage : Attacker : {} vicitim : {} Damage : {}", pkt.attackerid(), pkt.victimeid(), pkt.damage()) << endl;
}

/*---------------------------------------------------------------------------------------------
�̸�     : GameRoom::BroadCast
�뵵     : �濡 �ִ� ��� �÷��̾�� ��Ŷ�� �������ִ� �Լ�
������   : �̹α�
������¥ : 2022.09.13
----------------------------------------------------------------------------------------------*/
void GameRoom::BroadCast(shared_ptr<SendBuffer> sendbuffer)
{
	for (const auto [id, player] : _Players)
		player->GetSession()->SendCheck(sendbuffer);
}

/*---------------------------------------------------------------------------------------------
�̸�     : GameRoom::update
�뵵     : �濡 �ִ� ũ���ĵ��� ���¸� ������Ʈ ���ִ� �Լ�
������   : �̹α�
������¥ : 2022.10.03
----------------------------------------------------------------------------------------------*/
void GameRoom::update()
{
	for (const auto& [ID, monster] : _Monsters)
		monster->Update();
}

/*---------------------------------------------------------------------------------------------
�̸�     : GameRoom::FindPlayer
�뵵     : �濡 �ִ� �÷��̾ ���ǿ� ���� �˻� �� ��ȯ�ϴ� �Լ�
������   : �̹α�
������¥ : 2022.09.29
----------------------------------------------------------------------------------------------*/
Player * GameRoom::FindPlayer(function<bool(Player*)> condition)
{
	for(const auto [ID, player] : _Players)
	{
		if (condition(player) == true)
			return player;
	}

	return nullptr;
}

