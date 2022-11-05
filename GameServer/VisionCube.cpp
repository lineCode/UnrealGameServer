#include "pch.h"
#include "VisionCube.h"
#include "GameRoom.h"
#include "Monster.h"
#include "Player.h"
#include "ServerPacketManager.h"
#include "ServerSession.h"
#include "Zone.h"

VisionCube::VisionCube(Player* player) : _Owner(player)
{

}

/*---------------------------------------------------------------------------------------------
이름     : VisionCube::GatherObject
용도     : 나와 인접한 플레이어 및 몬스터를 가져오는 함수
수정자   : 이민규
수정날짜 : 2022.11.5
----------------------------------------------------------------------------------------------*/
GhashSet<GameObject*> VisionCube::GatherObject()
{
	GhashSet<GameObject*> objects;

	if (_Owner == nullptr || _Owner->GetRoom() == nullptr)
		return objects;

	Protocol::Vector pos = _Owner->GetVector();
	GhashSet<Zone*> zones = _Owner->GetRoom()->GetAdjacentZones(pos);

	// 나와 인접한 Zone을 모두 가져옴
	for(auto zone : zones)
	{
		// 내 시야의 플레이어만 가져옴
		for(auto player : zone->GetPlayers())
		{
			int32 dx = player->GetVector().x() - pos.x();
			int32 dy = player->GetVector().y() - pos.y();

			if(abs(dx) > _Owner->GetRoom()->VisionCells)
				continue;

			if (abs(dy) > _Owner->GetRoom()->VisionCells)
				continue;

			objects.insert(player);
		}

		// 내 시야의 몬스터만 가져옴
		for (auto monster : zone->GetMonsters())
		{
			int32 dx = monster->GetVector().x() - pos.x();
			int32 dy = monster->GetVector().y() - pos.y();

			if (abs(dx) > _Owner->GetRoom()->VisionCells)
				continue;

			if (abs(dy) > _Owner->GetRoom()->VisionCells)
				continue;

			objects.insert(monster);
		}
	}

	return objects;
}

/*---------------------------------------------------------------------------------------------
이름     : VisionCube::Update
용도     : 나와 인접한 플레이어 및 몬스터를 소환 및 파괴 해주는 함수
수정자   : 이민규
수정날짜 : 2022.11.5
----------------------------------------------------------------------------------------------*/
void VisionCube::Update()
{
	if (_Owner == nullptr || _Owner->GetRoom() == nullptr)
		return;

	int bcheck = false;

	// 현재 나와 인접한 플레이어와 몬스터를 가져옴
	GhashSet<GameObject*> currentobjects = GatherObject();

	// 기존에는 없었는데 새로 생긴 애들은 Spawn
	Protocol::SERVER_SPAWN spawnpkt;
	for (auto object : currentobjects)
	{
		if (_PreviousObjects.contains(object) == false)
		{
			Protocol::ObjectInfo info;
			info.MergeFrom(object->GetInfo());
			spawnpkt.add_objects()->MergeFrom(info);
			bcheck = true;
		}
	}
	if(bcheck)
		_Owner->GetSession()->Send(ServerPacketManager::MakeSendBuffer(spawnpkt));

	bcheck = false;

	// 기존엔 있었는데 사라진 애들은 Despawn 처리
	Protocol::SERVER_DESTROY destroypkt;
	for (auto object : _PreviousObjects)
	{
		if (currentobjects.contains(object) == false)
		{
			destroypkt.add_objectids(object->GetId());
			bcheck = true;
		}
	}

	if(bcheck)
		_Owner->GetSession()->Send(ServerPacketManager::MakeSendBuffer(destroypkt));

	// 현재로 변경
	_PreviousObjects = currentobjects;

	// 0.5 초 주기로 업데이트
	_Owner->GetRoom()->TimerPush(500, &GameRoom::VisionUpdate , _Owner);

}
