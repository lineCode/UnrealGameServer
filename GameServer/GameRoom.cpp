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
이름     : GameRoom::Init
용도     : 방을 초기화 하는 부분
수정자   : 이민규
수정날짜 : 2022.11.03
----------------------------------------------------------------------------------------------*/
void GameRoom::Init(int32 mapid , int32 zonesize)
{
	// TODO : 나중에는 맵데이터도 긁어 와서 수정
	_MapSize = {8000 ,8000};

	// ZONE
	_ZoneSize = zonesize;

	//TODO : 맵의 YSize XSize 나중에 받아서 수정 할것..
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

	// TODO : 나중에 삭제 예정
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
이름     : GameRoom::EnterGame
용도     : 플레이어를 방에 입장시키는 함수
수정자   : 이민규
수정날짜 : 2022.11.05
----------------------------------------------------------------------------------------------*/
void GameRoom::EnterGame(GameObject* gameobject)
{
	if (gameobject == nullptr)
		return;

	Protocol::ObjectType type = ObjectUtils::GetObjectType(gameobject->GetId());

	// 본인한테 정보 전송
	{
		switch (type)
		{
		case Protocol::ObjectType::PLAYER:
		{
			// 내 자신에게 들어왔음을 알려줌
			Player* player = reinterpret_cast<Player*>(gameobject);
			if (player == nullptr)
				return;

			player->RefreshStat();
			Protocol::SERVER_ENTERGAME enterpacket;
			ObjectUtils::SetEnterPacket(enterpacket, gameobject);
			_Players.insert({ player->GetId() , player });
			player->SetRoom(GetGameRoom());
			player->GetSession()->Send(ServerPacketManager::MakeSendBuffer(enterpacket));

			// 존 입장
			GetZone(player->GetVector())->AddPlayer(player);

			// 시야각 업데이트
			player->GetVision()->Update();
			break;
		}

		case Protocol::ObjectType::MONSTER:
		{
			// 몬스터를 _Monsters에 등록하는 부분
			Monster* monster = reinterpret_cast<Monster*>(gameobject);
			_Monsters.insert({ monster->GetId() , monster });
			monster->SetRoom(GetGameRoom());

			// 존 입장
			GetZone(monster->GetVector())->AddMonster(monster);

			monster->Update();
			break;
		}

		case Protocol::ObjectType::PROJECTILE:
		{
			// 발사체를 _Projectiles에 등록하는 부분
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

	// 타인에게 정보 전송
	{
		Protocol::SERVER_SPAWN spawnpkt;
		spawnpkt.add_objects()->MergeFrom(gameobject->GetInfo());
		BroadCast(ServerPacketManager::MakeSendBuffer(spawnpkt), gameobject->GetVector());
	}
}

/*---------------------------------------------------------------------------------------------
이름     : GameRoom::LeaveGame
용도     : 오브젝트를 방에서 퇴장시키는 함수
수정자   : 이민규
수정날짜 : 2022.11.05
----------------------------------------------------------------------------------------------*/
void GameRoom::LeaveGame(int32 objectid)
{
	Protocol::ObjectType type = ObjectUtils::GetObjectType(objectid);

	Protocol::Vector pos;

	switch (type)
	{
		case Protocol::ObjectType::PLAYER:
		{
			// 플레이어를 방에서 제거시키는 부분
			if (ObjectManager::GetInstance().Find(objectid) == nullptr)
				return;

			Player * player = _Players[objectid];

			pos = player->GetVector();

			// 자신에게 방에서 퇴장했다는 정보 전송
			Protocol::SERVER_LEAVEGAME leavepacket;
			player->GetSession()->Send(ServerPacketManager::MakeSendBuffer(leavepacket));

			// 존에서 퇴장
			GetZone(player->GetVector())->RemovePlayer(player);

			// 플레이어도 방에서 나가는 함수 호출
			player->LeaveGame();

			// 나의 방에서도 플레이어 제거
			_Players.erase(objectid);

			// ObjectManager에서 플레이어를 제거
			ObjectManager::GetInstance().Remove(objectid);
			break;
		}

		case Protocol::ObjectType::MONSTER:
		{
			// 몬스터를 방에서 제거시키는 부분
			if (_Monsters.contains(objectid) == false)
				return;

			pos = _Monsters[objectid]->GetVector();
			// 존에서 퇴장
			GetZone(_Monsters[objectid]->GetVector())->RemoveMonster(_Monsters[objectid]);

			Gdelete(_Monsters[objectid]);
			_Monsters.erase(objectid);
			break;
		}

		case Protocol::ObjectType::PROJECTILE:
		{
			// 발사체를 방에서 제거시키는 부분
			if (_Projectiles.contains(objectid) == false)
				return;

			Gdelete(_Projectiles[objectid]);
			_Projectiles.erase(objectid);

			break;
		}
	}

	// 타인에게 정보 전송
	{
		Protocol::SERVER_DESTROY destroypkt;
		destroypkt.add_objectids(objectid);
		BroadCast(ServerPacketManager::MakeSendBuffer(destroypkt) , pos);
	}
}

/*---------------------------------------------------------------------------------------------
이름     : GameRoom::OnDamage
용도     : 방에 있는 크리쳐에게 데미지를 주는 함수
수정자   : 이민규
수정날짜 : 2022.10.03
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
이름     : GameRoom::BroadCast
용도     : 방에 있는  플레이어게 Zone 단위로 패킷을 전송해주는 함수
수정자   : 이민규
수정날짜 : 2022.11.05
----------------------------------------------------------------------------------------------*/
void GameRoom::BroadCast(shared_ptr<SendBuffer> sendbuffer , Protocol::Vector pos)
{
	// 해당좌표에 인접한 존을 탐색
	GhashSet<Zone*> zones = GetAdjacentZones(pos);

	for (auto zone : zones)
	{
		for (auto player : zone->GetPlayers())
		{
			//인접한 영역이더라도 내 시야각 보다 멀면 PASS
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
이름     : GameRoom::GetZone
용도     : 좌표에 해당하는 Zone을 반환하는 함수
           존재하지 않는 Zone의 좌표의 경우 nullptr 반환
수정자   : 이민규
수정날짜 : 2022.11.04
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
이름     : GameRoom::GetAdjacentZones
용도     : 좌표에 해당하는 인접한 Zone을 Gvector로 반환하는 함수
		   존재하지 않는 Zone의 좌표의 경우 GhashSet size = 0
수정자   : 이민규
수정날짜 : 2022.11.04
----------------------------------------------------------------------------------------------*/
GhashSet<Zone*> GameRoom::GetAdjacentZones(Protocol::Vector pos, int32 cell)
{
	GhashSet<Zone*> zones;

	// y기준으로 위 밑 / x기준으로 왼쪽 오른쪽 순차 검색
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
이름     : GameRoom::MonsterUpdate
용도     : 방에 있는 크리쳐들의 상태를 업데이트 해주는 함수
수정자   : 이민규
수정날짜 : 2022.11.05
----------------------------------------------------------------------------------------------*/
void GameRoom::MonsterUpdate(Monster* monster)
{
	if (monster == nullptr || monster->GetRoom() != GetGameRoom())
		return;

	monster->Update();
}

/*---------------------------------------------------------------------------------------------
이름     : GameRoom::VisionUpdate
용도     : 플레이어의 시야각을 업데이트 해주는 함수
수정자   : 이민규
수정날짜 : 2022.11.05
----------------------------------------------------------------------------------------------*/
void GameRoom::VisionUpdate(Player* player)
{
	if (player == nullptr || player->GetRoom() != GetGameRoom())
		return;

	player->Update();
}

/*---------------------------------------------------------------------------------------------
이름     : GameRoom::FindPlayer
용도     : 방에 있는 플레이어를 조건에 따라 검사 후 반환하는 함수
수정자   : 이민규
수정날짜 : 2022.09.29
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
이름     : GameRoom::PlayerMove
용도     : 방에 있는 플레이어들에게 이동한 플레이어의 좌표 패킷을 보내주는 함수
수정자   : 이민규
수정날짜 : 2022.11.04
----------------------------------------------------------------------------------------------*/
void GameRoom::PlayerMove(Player* player, Protocol::CLIENT_MOVE* pkt)
{
	if (player == nullptr)
		return;

	// TODO : 서버에서 검증 필요 추가 예정

	// 일단 서버에서 먼저 좌표 및 방향 이동
	Zone* zonenow = player->GetRoom()->GetZone(player->GetVector());
	Zone* zoneafter = player->GetRoom()->GetZone(pkt->vector());

	// 존 위치가 바뀌엇을 경우 존변경
	if (zonenow != zoneafter)
	{
		zonenow->RemovePlayer(player);
		zoneafter->AddPlayer(player);
	}

	player->SetVector(pkt->vector());
	player->SetRotator(pkt->rotator());

	// 다른 플레이어한테도 알려준다
	Protocol::SERVER_MOVE movepacket;
	ObjectUtils::SetMovePacket(movepacket, player);
	BroadCast(ServerPacketManager::MakeSendBuffer(movepacket) , player->GetVector());
}

/*---------------------------------------------------------------------------------------------
이름     : GameRoom::PlayerSkill
용도     : 방에 있는 플레이어들에게 스킬을 사용한 플레이어의 스킬 패킷을 보내주는 함수
수정자   : 이민규
수정날짜 : 2022.09.25
----------------------------------------------------------------------------------------------*/
void GameRoom::PlayerSkill(Player* player, Protocol::CLIENT_SKILL *pkt)
{
	if (player == nullptr)
		return;

	// 플레이어 스킬 사용을 보냄

	Protocol::SERVER_SKILL skillpacket;
	ObjectUtils::SetSkillPacket(skillpacket, player, *pkt);
	BroadCast(ServerPacketManager::MakeSendBuffer(skillpacket) , player->GetVector());

	printf("[CLINET_Skill_FUNC_ID[%d]] : %d Use\n", skillpacket.objectid(), skillpacket.skillinfo().skillid());

	// TODO : 스킬 충돌체크 일단은 클라언트에게 모두 맞김

	// TODO : 쿨타임 검증만 추가 예정
}

#pragma endregion

#pragma region Item
/*---------------------------------------------------------------------------------------------
이름     : GameRoom::PlayerEquipItem
용도     : 방에있는 플레이어의 아이템을 장착시키는 함수
수정자   : 이민규
수정날짜 : 2022.10.26
----------------------------------------------------------------------------------------------*/
void GameRoom::PlayerEquipItem(Player* player, Protocol::CLIENT_EQUIPITEM * pkt)
{
	if (player == nullptr)
		return;

	player->ItemEquip(pkt);
}
#pragma endregion