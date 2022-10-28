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

#pragma region Transaction
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
	int32 hp = player->GetHp();
	int32 id = player->GetPlayerDbId();

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
	// 1) �޸𸮿� ������ ������� ���� 
	int32 slot = player->GetInventory()->GetEmptySlot();
	if (slot == -1)
		return;

	int32 playerid = player->GetPlayerDbId();
	int32 itemid = reward->itemid;
	int32 count = reward->count;

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
						
						Item* item = Item::MakeItem(itemdbid, itemid, count, slot , 0);
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


#pragma endregion

#pragma region Noti
/*---------------------------------------------------------------------------------------------
�̸�     : DBJobManager::EquipItemNoti
�뵵     : �÷��̾��� �κ��丮�� �����ϴ� �������� �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.10.26
----------------------------------------------------------------------------------------------*/
void DBJobManager::EquipItemNoti(Player* player,Item* item)
{
	if (player == nullptr || item == nullptr)
		return;

	int32 itemdbid = item->GetItemDbID();
	int32 itemequip = item->GetEquip();
	int32 itemslot = item->GetSlot();

	PushAsync([player, itemdbid , itemequip , itemslot]()
		{
			DBConnection* dbConn = GDBConnectionPool->Pop();
			ProcedureManager::ItemEquipChange equipchange(*dbConn);

			equipchange.Param_Itemdbid(static_cast<int32>(itemdbid));
			equipchange.Param_Equip(static_cast<int32>(itemequip));
			equipchange.Param_Slot(static_cast<int32>(itemslot));

			if (equipchange.Execute())
				GDBConnectionPool->Push(dbConn);
		});

}
#pragma endregion