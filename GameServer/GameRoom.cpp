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
이름     : GameRoom::Init
용도     : 방을 초기화 하는 부분
수정자   : 이민규
수정날짜 : 2022.09.19
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
이름     : GameRoom::EnterGame
용도     : 플레이어를 방에 입장시키는 함수
수정자   : 이민규
수정날짜 : 2022.09.19
----------------------------------------------------------------------------------------------*/
void GameRoom::EnterGame(GameObject* gameobject)
{
	if (gameobject == nullptr)
		return;

	Protocol::ObjectType type = ObjectUtils::GetObjectType(gameobject->GetId());

	// TODO : 본인한테 정보 전송
	{
		switch (type)
		{
		case Protocol::ObjectType::PLAYER:
		{
			// TODO : 내 자신에게 들어왔음을 알려줌
			Player* player = reinterpret_cast<Player*>(gameobject);
			if (player == nullptr)
				return;

			Protocol::SERVER_ENTERGAME enterpacket;
			ObjectUtils::SetEnterPacket(enterpacket, gameobject);
			_Players.insert({ player->GetId() , player });
			player->SetRoom(GetGameRoom());
			player->GetSession()->SendCheck(ServerPacketManager::MakeSendBuffer(enterpacket));

			// TODO : 같은 방에 있는 플레이어들을 생성하기 위한 정보를 나에게 보냄
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
			// TODO : 몬스터를 _Monsters에 등록하는 부분
			Monster* monster = reinterpret_cast<Monster*>(gameobject);
			_Monsters.insert({ monster->GetId() , monster });
			monster->SetRoom(GetGameRoom());
			break;
		}

		case Protocol::ObjectType::PROJECTILE:
		{
			// TODO : 발사체를 _Projectiles에 등록하는 부분
			Projectile* projectile = reinterpret_cast<Projectile*>(gameobject);
			_Projectiles.insert({ projectile->GetId() , projectile });
			projectile->SetRoom(GetGameRoom());
			break;
		}

		default:
		{
			// TODO : 에러 부분
			cout << "[GameRoom] : ObjectType Missing Error" << endl;
			break;
		}
		}

	}

	// TODO : 같은방에 있는 다른 플레이어에게도 생성할 나의 정보를 전송
	{
		Protocol::SERVER_SPAWN spawnpacket;

		switch (type)
		{
		case Protocol::ObjectType::PROJECTILE:
		{
			// TODO : 발사체일 경우
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
			// TODO : 그 외 모든 경우
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
이름     : GameRoom::LeaveGame
용도     : 오브젝트를 방에서 퇴장시키는 함수
수정자   : 이민규
수정날짜 : 2022.09.19
----------------------------------------------------------------------------------------------*/
void GameRoom::LeaveGame(int32 objectid)
{
	Protocol::ObjectType type = ObjectUtils::GetObjectType(objectid);

	switch (type)
	{
		case Protocol::ObjectType::PLAYER:
		{
			// TODO : 플레이어를 방에서 제거시키는 부분
			if (ObjectManager::GetInstance().Find(objectid) == nullptr)
				return;

			Player * player = _Players[objectid];

			// TODO : 자신에게 방에서 퇴장했다는 정보 전송
			Protocol::SERVER_LEAVEGAME leavepacket;
			player->GetSession()->SendCheck(ServerPacketManager::MakeSendBuffer(leavepacket));

			// TODO : 플레이어도 방에서 나가는 함수 호출
			player->LeaveGame();

			// TODO : 나의 방에서도 플레이어 제거
			_Players.erase(objectid);

			// TODO : ObjectManager에서 플레이어를 제거
			ObjectManager::GetInstance().Remove(objectid);
			break;
		}

		case Protocol::ObjectType::MONSTER:
		{
			// TODO : 몬스터를 방에서 제거시키는 부분
			if (_Monsters.contains(objectid) == false)
				return;

			Gdelete(_Monsters[objectid]);
			_Monsters.erase(objectid);
			break;
		}

		case Protocol::ObjectType::PROJECTILE:
		{
			// TODO : 발사체를 방에서 제거시키는 부분
			if (_Projectiles.contains(objectid) == false)
				return;

			Gdelete(_Projectiles[objectid]);
			_Projectiles.erase(objectid);

			break;
		}
	}

	// TODO : 같은 방에 있는 플레이어에게 퇴장한 나의 정보 전송
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
이름     : GameRoom::PlayerMove
용도     : 방에 있는 플레이어들에게 이동한 플레이어의 좌표 패킷을 보내주는 함수
수정자   : 이민규
수정날짜 : 2022.09.28
----------------------------------------------------------------------------------------------*/
void GameRoom::PlayerMove(Player* player, Protocol::CLIENT_MOVE pkt)
{
	if (player == nullptr)
		return;

	// TODO : 서버에서 검증 필요 추가 예정
	printf("[CLIENT_MOVE_FUNC_ID[%d]] : %.1lf %.1lf %.1lf\n", player->GetInfo().objectid(), pkt.vector().x(), pkt.vector().y(), pkt.vector().z());

	// TODO : 일단 서버에서 먼저 좌표 및 방향 이동
	player->SetVector(pkt.vector());
	player->SetRotator(pkt.rotator());

	// TODO : 다른 플레이어한테도 알려준다
	Protocol::SERVER_MOVE movepacket;
	ObjectUtils::SetMovePacket(movepacket, player);
	BroadCast(ServerPacketManager::MakeSendBuffer(movepacket));
}

/*---------------------------------------------------------------------------------------------
이름     : GameRoom::PlayerSkill
용도     : 방에 있는 플레이어들에게 스킬을 사용한 플레이어의 스킬 패킷을 보내주는 함수
수정자   : 이민규
수정날짜 : 2022.09.25
----------------------------------------------------------------------------------------------*/
void GameRoom::PlayerSkill(Player* player, Protocol::CLIENT_SKILL pkt)
{
	if (player == nullptr)
		return;

	// TODO : 플레이어 스킬 사용을 보냄

	Protocol::SERVER_SKILL skillpacket;
	ObjectUtils::SetSkillPacket(skillpacket, player, pkt);
	BroadCast(ServerPacketManager::MakeSendBuffer(skillpacket));

	printf("[CLINET_Skill_FUNC_ID[%d]] : %d Use\n", skillpacket.objectid(), skillpacket.skillinfo().skillid());

	// TODO : 스킬 충돌체크 일단은 클라언트에게 모두 맞김

	// TODO : 쿨타임 검증만 추가 예정
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
		{
		cout << format("[GameRoom] : OnDamage Error") << endl;
		};
	}

	cout << format("Damage : Attacker : {} vicitim : {} Damage : {}", pkt.attackerid(), pkt.victimeid(), pkt.damage()) << endl;
}

/*---------------------------------------------------------------------------------------------
이름     : GameRoom::BroadCast
용도     : 방에 있는 모든 플레이어게 패킷을 전송해주는 함수
수정자   : 이민규
수정날짜 : 2022.09.13
----------------------------------------------------------------------------------------------*/
void GameRoom::BroadCast(shared_ptr<SendBuffer> sendbuffer)
{
	for (const auto [id, player] : _Players)
		player->GetSession()->SendCheck(sendbuffer);
}

/*---------------------------------------------------------------------------------------------
이름     : GameRoom::update
용도     : 방에 있는 크리쳐들의 상태를 업데이트 해주는 함수
수정자   : 이민규
수정날짜 : 2022.10.03
----------------------------------------------------------------------------------------------*/
void GameRoom::update()
{
	for (const auto& [ID, monster] : _Monsters)
		monster->Update();
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

