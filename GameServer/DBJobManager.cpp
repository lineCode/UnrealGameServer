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
