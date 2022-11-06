#include "pch.h"
#include "RoomManager.h"
#include "GameRoom.h"

shared_ptr<RoomManager> RoomManager::_Instance = nullptr;

/*---------------------------------------------------------------------------------------------
�̸�     : RoomManager::GetInstance
�뵵     : RoomManager�� �޾ƿ��� �̱��� �Լ�
������   : �̹α�
������¥ : 2022.11.06
----------------------------------------------------------------------------------------------*/
shared_ptr<RoomManager> RoomManager::GetInstance()
{
	if (_Instance == nullptr)
		_Instance = GMakeShared<RoomManager>();

	return _Instance;
}

/*---------------------------------------------------------------------------------------------
�̸�     : RoomManager::DeleteInstance
�뵵     : RoomManager�� Instance�� �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.11.06
----------------------------------------------------------------------------------------------*/
void RoomManager::DeleteInstance()
{
	_Instance = nullptr;
}

/*---------------------------------------------------------------------------------------------
�̸�     : RoomManager::Add
�뵵     : GameRoom�� Map�� �߰��ϴ� �Լ�
������   : �̹α�
������¥ : 2022.10.05
----------------------------------------------------------------------------------------------*/
shared_ptr<GameRoom> RoomManager::Add(int32 mapid , int32 cell)
{
	shared_ptr<GameRoom> gameroom = GMakeShared<GameRoom>();

	gameroom->PushAsync(&GameRoom::Init , mapid , cell);

	WRITELOCK;
	gameroom->SetRoomId(_Roomid);
	_Rooms[_Roomid] = gameroom;
	_Roomid++;

	return gameroom;
}

/*---------------------------------------------------------------------------------------------
�̸�     : RoomManager::Remove
�뵵     : GameRoom�� Map���� �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.09.12
----------------------------------------------------------------------------------------------*/
bool RoomManager::Remove(int32 roomid)
{
	WRITELOCK;

	if (_Rooms.find(roomid) == _Rooms.end())
		return false;

	_Rooms.erase(roomid);

	return true;
}

/*---------------------------------------------------------------------------------------------
�̸�     : RoomManager::Find
�뵵     : GameRoom�� Map���� ã�Ƽ� ��ȯ���ִ� �Լ�
		   ���� ��� nullptr ��ȯ
������   : �̹α�
������¥ : 2022.09.12
----------------------------------------------------------------------------------------------*/
shared_ptr<GameRoom> RoomManager::Find(int32 roomid)
{
	WRITELOCK;

	if (_Rooms.find(roomid) == _Rooms.end())
		return nullptr;

	return _Rooms[roomid];
}



