#include "pch.h"
#include <format>
#include "GameRoom.h"
#include "Player.h"
#include "Monster.h"
#include "Projectile.h"
#include "ObjectManager.h"
#include "Protocol.pb.h"
#include "ServerPacketManager.h"
#include "ServerSession.h"
#include "ObjectUtils.h"
#include "VisionCube.h"
#include "Zone.h"

#pragma region GameRoom
GameRoom::GameRoom()
{
	
}

/*---------------------------------------------------------------------------------------------
�̸�     : GameRoom::Init
�뵵     : ���� �ʱ�ȭ �ϴ� �κ�
������   : �̹α�
������¥ : 2022.11.03
----------------------------------------------------------------------------------------------*/
void GameRoom::Init(int32 mapid , int32 zonesize)
{
	// TODO : ���߿��� �ʵ����͵� �ܾ� �ͼ� ����
	_MapSize = {8000 ,8000};

	// ZONE
	_ZoneSize = zonesize;

	//TODO : ���� YSize XSize ���߿� �޾Ƽ� ���� �Ұ�..
	int32 countY =  ceil( (float)_MapSize.y / _ZoneSize);
	int32 countX =  ceil((float)_MapSize.x / _ZoneSize);

	for(int y = 0; y <= countY; y++)
	{
		Gvector<Zone*> zoon;
		for(int x = 0; x <= countX; x++)
		{
			zoon.push_back(Gnew<Zone>(y, x));
		}
		_Zones.push_back(zoon);
	}

	// TODO : ���߿� ���� ����
	Monster * monster = ObjectManager::GetInstance().Add<Monster>(Protocol::MonsterType::SAVAROG);
	monster->Init(1);
	Protocol::Vector vector;
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
������¥ : 2022.11.05
----------------------------------------------------------------------------------------------*/
void GameRoom::EnterGame(GameObject* gameobject)
{
	if (gameobject == nullptr)
		return;

	Protocol::ObjectType type = ObjectUtils::GetObjectType(gameobject->GetId());

	// �������� ���� ����
	{
		switch (type)
		{
		case Protocol::ObjectType::PLAYER:
		{
			// �� �ڽſ��� �������� �˷���
			Player* player = reinterpret_cast<Player*>(gameobject);
			if (player == nullptr)
				return;

			player->RefreshStat();
			Protocol::SERVER_ENTERGAME enterpacket;
			ObjectUtils::SetEnterPacket(enterpacket, gameobject);
			_Players.insert({ player->GetId() , player });
			player->SetRoom(GetGameRoom());
			player->GetSession()->Send(ServerPacketManager::MakeSendBuffer(enterpacket));

			// �� ����
			GetZone(player->GetVector())->AddPlayer(player);

			// �þ߰� ������Ʈ
			player->GetVision()->Update();
			break;
		}

		case Protocol::ObjectType::MONSTER:
		{
			// ���͸� _Monsters�� ����ϴ� �κ�
			Monster* monster = reinterpret_cast<Monster*>(gameobject);
			_Monsters.insert({ monster->GetId() , monster });
			monster->SetRoom(GetGameRoom());

			// �� ����
			GetZone(monster->GetVector())->AddMonster(monster);

			monster->Update();
			break;
		}

		case Protocol::ObjectType::PROJECTILE:
		{
			// �߻�ü�� _Projectiles�� ����ϴ� �κ�
			Projectile* projectile = reinterpret_cast<Projectile*>(gameobject);
			_Projectiles.insert({ projectile->GetId() , projectile });
			projectile->SetRoom(GetGameRoom());
			break;
		}

		default:
		{
			cout << "[GameRoom] : ObjectType Missing Error" << endl;
			break;
		}
		}

	}

	// Ÿ�ο��� ���� ����
	{
		Protocol::SERVER_SPAWN spawnpkt;
		spawnpkt.add_objects()->MergeFrom(gameobject->GetInfo());
		BroadCast(ServerPacketManager::MakeSendBuffer(spawnpkt), gameobject->GetVector());
	}
}

/*---------------------------------------------------------------------------------------------
�̸�     : GameRoom::LeaveGame
�뵵     : ������Ʈ�� �濡�� �����Ű�� �Լ�
������   : �̹α�
������¥ : 2022.11.05
----------------------------------------------------------------------------------------------*/
void GameRoom::LeaveGame(int32 objectid)
{
	Protocol::ObjectType type = ObjectUtils::GetObjectType(objectid);

	Protocol::Vector pos;

	switch (type)
	{
		case Protocol::ObjectType::PLAYER:
		{
			// �÷��̾ �濡�� ���Ž�Ű�� �κ�
			if (ObjectManager::GetInstance().Find(objectid) == nullptr)
				return;

			Player * player = _Players[objectid];

			pos = player->GetVector();

			// �ڽſ��� �濡�� �����ߴٴ� ���� ����
			Protocol::SERVER_LEAVEGAME leavepacket;
			player->GetSession()->Send(ServerPacketManager::MakeSendBuffer(leavepacket));

			// ������ ����
			GetZone(player->GetVector())->RemovePlayer(player);

			// �÷��̾ �濡�� ������ �Լ� ȣ��
			player->LeaveGame();

			// ���� �濡���� �÷��̾� ����
			_Players.erase(objectid);

			// ObjectManager���� �÷��̾ ����
			ObjectManager::GetInstance().Remove(objectid);
			break;
		}

		case Protocol::ObjectType::MONSTER:
		{
			// ���͸� �濡�� ���Ž�Ű�� �κ�
			if (_Monsters.contains(objectid) == false)
				return;

			pos = _Monsters[objectid]->GetVector();
			// ������ ����
			GetZone(_Monsters[objectid]->GetVector())->RemoveMonster(_Monsters[objectid]);

			Gdelete(_Monsters[objectid]);
			_Monsters.erase(objectid);
			break;
		}

		case Protocol::ObjectType::PROJECTILE:
		{
			// �߻�ü�� �濡�� ���Ž�Ű�� �κ�
			if (_Projectiles.contains(objectid) == false)
				return;

			Gdelete(_Projectiles[objectid]);
			_Projectiles.erase(objectid);

			break;
		}
	}

	// Ÿ�ο��� ���� ����
	{
		Protocol::SERVER_DESTROY destroypkt;
		destroypkt.add_objectids(objectid);
		BroadCast(ServerPacketManager::MakeSendBuffer(destroypkt) , pos);
	}
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
		cout << format("[GameRoom] : OnDamage Error") << endl;
	}

	cout << format("Damage : Attacker : {} vicitim : {} Damage : {}", pkt.attackerid(), pkt.victimeid(), pkt.damage()) << endl;
}

/*---------------------------------------------------------------------------------------------
�̸�     : GameRoom::BroadCast
�뵵     : �濡 �ִ�  �÷��̾�� Zone ������ ��Ŷ�� �������ִ� �Լ�
������   : �̹α�
������¥ : 2022.11.05
----------------------------------------------------------------------------------------------*/
void GameRoom::BroadCast(shared_ptr<SendBuffer> sendbuffer , Protocol::Vector pos)
{
	// �ش���ǥ�� ������ ���� Ž��
	GhashSet<Zone*> zones = GetAdjacentZones(pos);

	for (auto zone : zones)
	{
		for (auto player : zone->GetPlayers())
		{
			//������ �����̴��� �� �þ߰� ���� �ָ� PASS
			int32 dx = player->GetVector().x() - pos.x();
			int32 dy = player->GetVector().y() - pos.y();

			if (abs(dx) > VisionCells)
				continue;

			if (abs(dy) > VisionCells)
				continue;

			player->GetSession()->Send(sendbuffer);
		}
	}
}

/*---------------------------------------------------------------------------------------------
�̸�     : GameRoom::GetZone
�뵵     : ��ǥ�� �ش��ϴ� Zone�� ��ȯ�ϴ� �Լ�
           �������� �ʴ� Zone�� ��ǥ�� ��� nullptr ��ȯ
������   : �̹α�
������¥ : 2022.11.04
----------------------------------------------------------------------------------------------*/
Zone* GameRoom::GetZone(Protocol::Vector pos)
{
	int32 x = floor(((_MapSize.x / 2) + pos.x()) / _ZoneSize);
	int32 y = floor(((_MapSize.y / 2) + pos.y()) / _ZoneSize);

	if (x < 0 || x > floor((float)(_MapSize.x)/ _ZoneSize))
		return nullptr;

	if (y < 0 || y > floor((float)(_MapSize.y) / _ZoneSize))
		return nullptr;

	return _Zones[y][x];
}

/*---------------------------------------------------------------------------------------------
�̸�     : GameRoom::GetAdjacentZones
�뵵     : ��ǥ�� �ش��ϴ� ������ Zone�� Gvector�� ��ȯ�ϴ� �Լ�
		   �������� �ʴ� Zone�� ��ǥ�� ��� GhashSet size = 0
������   : �̹α�
������¥ : 2022.11.04
----------------------------------------------------------------------------------------------*/
GhashSet<Zone*> GameRoom::GetAdjacentZones(Protocol::Vector pos, int32 cell)
{
	GhashSet<Zone*> zones;

	// y�������� �� �� / x�������� ���� ������ ���� �˻�
	int32 delta[] = { -cell, +cell };

	for(auto dy : delta)
	{
		for(auto dx : delta)
		{
			int32 y = pos.y() + dy;
			int32 x = pos.x() + dx;

			Protocol::Vector vector;
			vector.set_x(x);
			vector.set_y(y);

			Zone* zone = GetZone(vector);

			if(zone == nullptr || zone->GetPlayers().empty())
				continue;

			zones.insert(zone);
		}
	}
	return zones;
}

/*---------------------------------------------------------------------------------------------
�̸�     : GameRoom::MonsterUpdate
�뵵     : �濡 �ִ� ũ���ĵ��� ���¸� ������Ʈ ���ִ� �Լ�
������   : �̹α�
������¥ : 2022.11.05
----------------------------------------------------------------------------------------------*/
void GameRoom::MonsterUpdate(Monster* monster)
{
	if (monster == nullptr || monster->GetRoom() != GetGameRoom())
		return;

	monster->Update();
}

/*---------------------------------------------------------------------------------------------
�̸�     : GameRoom::VisionUpdate
�뵵     : �÷��̾��� �þ߰��� ������Ʈ ���ִ� �Լ�
������   : �̹α�
������¥ : 2022.11.05
----------------------------------------------------------------------------------------------*/
void GameRoom::VisionUpdate(Player* player)
{
	if (player == nullptr || player->GetRoom() != GetGameRoom())
		return;

	player->Update();
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
#pragma endregion

#pragma region Battle
/*---------------------------------------------------------------------------------------------
�̸�     : GameRoom::PlayerMove
�뵵     : �濡 �ִ� �÷��̾�鿡�� �̵��� �÷��̾��� ��ǥ ��Ŷ�� �����ִ� �Լ�
������   : �̹α�
������¥ : 2022.11.04
----------------------------------------------------------------------------------------------*/
void GameRoom::PlayerMove(Player* player, Protocol::CLIENT_MOVE* pkt)
{
	if (player == nullptr)
		return;

	// TODO : �������� ���� �ʿ� �߰� ����

	// �ϴ� �������� ���� ��ǥ �� ���� �̵�
	Zone* zonenow = player->GetRoom()->GetZone(player->GetVector());
	Zone* zoneafter = player->GetRoom()->GetZone(pkt->vector());

	// �� ��ġ�� �ٲ���� ��� ������
	if (zonenow != zoneafter)
	{
		zonenow->RemovePlayer(player);
		zoneafter->AddPlayer(player);
	}

	player->SetVector(pkt->vector());
	player->SetRotator(pkt->rotator());

	// �ٸ� �÷��̾����׵� �˷��ش�
	Protocol::SERVER_MOVE movepacket;
	ObjectUtils::SetMovePacket(movepacket, player);
	BroadCast(ServerPacketManager::MakeSendBuffer(movepacket) , player->GetVector());
}

/*---------------------------------------------------------------------------------------------
�̸�     : GameRoom::PlayerSkill
�뵵     : �濡 �ִ� �÷��̾�鿡�� ��ų�� ����� �÷��̾��� ��ų ��Ŷ�� �����ִ� �Լ�
������   : �̹α�
������¥ : 2022.09.25
----------------------------------------------------------------------------------------------*/
void GameRoom::PlayerSkill(Player* player, Protocol::CLIENT_SKILL *pkt)
{
	if (player == nullptr)
		return;

	// �÷��̾� ��ų ����� ����

	Protocol::SERVER_SKILL skillpacket;
	ObjectUtils::SetSkillPacket(skillpacket, player, *pkt);
	BroadCast(ServerPacketManager::MakeSendBuffer(skillpacket) , player->GetVector());

	printf("[CLINET_Skill_FUNC_ID[%d]] : %d Use\n", skillpacket.objectid(), skillpacket.skillinfo().skillid());

	// TODO : ��ų �浹üũ �ϴ��� Ŭ���Ʈ���� ��� �±�

	// TODO : ��Ÿ�� ������ �߰� ����
}

#pragma endregion

#pragma region Item
/*---------------------------------------------------------------------------------------------
�̸�     : GameRoom::PlayerEquipItem
�뵵     : �濡�ִ� �÷��̾��� �������� ������Ű�� �Լ�
������   : �̹α�
������¥ : 2022.10.26
----------------------------------------------------------------------------------------------*/
void GameRoom::PlayerEquipItem(Player* player, Protocol::CLIENT_EQUIPITEM * pkt)
{
	if (player == nullptr)
		return;

	player->ItemEquip(pkt);
}
#pragma endregion