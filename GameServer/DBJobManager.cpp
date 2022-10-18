#include "pch.h"
#include "DBJobManager.h"
#include "DBConnectionPool.h"
#include "GameRoom.h"
#include "Player.h"
#include "ProcedureManager.h"
#include <format>

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
			{
				GDBConnectionPool->Push(dbConn);
				// TODO : ME (GAMEROOM)
				room->PushAsync([id , hp]()
					{
						cout << format("[DBJobManager] : SavePlayerStatus playerid : {} HP : {}", id, hp) << endl;
						
					});
			}
		});
}
