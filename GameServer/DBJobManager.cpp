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
이름     : DBJobManager::SavePlayerStatus
용도     : 방에서 나갈 때 플레이어의 현재 스텟 정보를 데이터베이스에 저장시키는 함수
수정자   : 이민규
수정날짜 : 2022.10.18
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
이름     : DBJobManager::RewardPlayer
용도     : 플레이어의 인벤토리에 아이템을 주는 함수
수정자   : 이민규
수정날짜 : 2022.10.24
----------------------------------------------------------------------------------------------*/
void DBJobManager::RewardPlayer(Player* player, RewardData* reward, shared_ptr<GameRoom> room)
{
	if (player == nullptr || room == nullptr || reward == nullptr)
		return;

	// TODO : 살짝 문제가 있긴 하다
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

						// TODO : 클라이언트에게 추가된 아이템 전송
						{
							Protocol::SERVER_ADDITEM itempkt;
							itempkt.add_items()->CopyFrom(item->Getinfo());

							player->GetSession()->SendCheck(ServerPacketManager::MakeSendBuffer(itempkt));
						}
					});
			}
		});
}
