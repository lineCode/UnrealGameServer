#include "pch.h"
#include "DBJobManager.h"
#include "DBConnectionPool.h"
#include "GameRoom.h"
#include "Player.h"
#include "ProcedureManager.h"
#include "ContentsData.h"
#include "Inventory.h"
#include "Item.h"
#include "ServerPacketManager.h"

shared_ptr<DBJobManager> DBJobManager::_Instance = nullptr;

shared_ptr<DBJobManager> DBJobManager::GetInstance()
{
	if (_Instance == nullptr)
		_Instance = GMakeShared<DBJobManager>();

	return _Instance;
}

void DBJobManager::Delete()
{
	_Instance = nullptr;
}

/*---------------------------------------------------------------------------------------------
�̸�     : DBJobManager::SavePlayerStatus
�뵵     : �濡�� ���� �� �÷��̾��� ���� ���� ������ �����ͺ��̽��� �����Ű�� �Լ�
������   : �̹α�
������¥ : 2022.10.18
----------------------------------------------------------------------------------------------*/
void DBJobManager::SavePlayerStatus(Player* player, shared_ptr<GameRoom> room)
{
	if (player == nullptr || room == nullptr)
		return;

	// TODO : ME (GAMEROOM)
	int hp = player->GetHp();
	int id = player->GetPlayerDbId();

	// TODO : YOU (DBMANAGER)
	PushAsync([room = room , id ,hp]()
		{
			DBConnection* dbConn = GDBConnectionPool->Pop();
			ProcedureManager::PlayerChangeHp changehp(*dbConn);
			changehp.Param_Playerid(static_cast<int32>(id));
			changehp.Param_Hp(static_cast<int32>(hp));

			if (changehp.Execute())
				GDBConnectionPool->Push(dbConn);
		});
}

/*---------------------------------------------------------------------------------------------
�̸�     : DBJobManager::RewardPlayer
�뵵     : �÷��̾��� �κ��丮�� �������� �ִ� �Լ�
������   : �̹α�
������¥ : 2022.10.24
----------------------------------------------------------------------------------------------*/
void DBJobManager::RewardPlayer(Player* player, RewardData* reward, shared_ptr<GameRoom> room)
{
	if (player == nullptr || room == nullptr || reward == nullptr)
		return;

	// TODO : ��¦ ������ �ֱ� �ϴ�
	int32 slot = player->GetInventory()->GetEmptySlot();
	if (slot == -1)
		return;

	int playerid = player->GetPlayerDbId();
	int itemid = reward->itemid;
	int count = reward->count;

	PushAsync([room = room, playerid, itemid , count, player , slot]()
		{
			DBConnection* dbConn = GDBConnectionPool->Pop();
			ProcedureManager::CreateItemInventory createitem(*dbConn);
			int32 itemdbid;

			createitem.Param_Itemid(static_cast<int32>(itemid));
			createitem.Param_Ownerid(static_cast<int32>(playerid));
			createitem.Param_Count(static_cast<int32>(count));
			createitem.Param_Slot(static_cast<int32>(slot));
			createitem.Column_DbId(itemdbid);

			if (createitem.Execute() && createitem.Fetch())
			{
				GDBConnectionPool->Push(dbConn);
				// TODO : ME (GAMEROOM)
				room->PushAsync([player ,itemdbid, itemid , count, slot ]()
					{
						if (player == nullptr)
							return;
						
						Item* item = Item::MakeItem(itemdbid, itemid, count, slot);
						player->GetInventory()->Add(item);

						// TODO : Ŭ���̾�Ʈ���� �߰��� ������ ����
						{
							Protocol::SERVER_ADDITEM itempkt;
							itempkt.add_items()->CopyFrom(item->Getinfo());

							player->GetSession()->SendCheck(ServerPacketManager::MakeSendBuffer(itempkt));
						}
					});
			}
		});
}
