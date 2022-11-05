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
�̸�     : VisionCube::GatherObject
�뵵     : ���� ������ �÷��̾� �� ���͸� �������� �Լ�
������   : �̹α�
������¥ : 2022.11.5
----------------------------------------------------------------------------------------------*/
GhashSet<GameObject*> VisionCube::GatherObject()
{
	GhashSet<GameObject*> objects;

	if (_Owner == nullptr || _Owner->GetRoom() == nullptr)
		return objects;

	Protocol::Vector pos = _Owner->GetVector();
	GhashSet<Zone*> zones = _Owner->GetRoom()->GetAdjacentZones(pos);

	// ���� ������ Zone�� ��� ������
	for(auto zone : zones)
	{
		// �� �þ��� �÷��̾ ������
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

		// �� �þ��� ���͸� ������
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
�̸�     : VisionCube::Update
�뵵     : ���� ������ �÷��̾� �� ���͸� ��ȯ �� �ı� ���ִ� �Լ�
������   : �̹α�
������¥ : 2022.11.5
----------------------------------------------------------------------------------------------*/
void VisionCube::Update()
{
	if (_Owner == nullptr || _Owner->GetRoom() == nullptr)
		return;

	int bcheck = false;

	// ���� ���� ������ �÷��̾�� ���͸� ������
	GhashSet<GameObject*> currentobjects = GatherObject();

	// �������� �����µ� ���� ���� �ֵ��� Spawn
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

	// ������ �־��µ� ����� �ֵ��� Despawn ó��
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

	// ����� ����
	_PreviousObjects = currentobjects;

	// 0.5 �� �ֱ�� ������Ʈ
	_Owner->GetRoom()->TimerPush(500, &GameRoom::VisionUpdate , _Owner);

}
