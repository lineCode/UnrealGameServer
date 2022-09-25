#include "pch.h"
#include "RoomManager.h"
#include "GameRoom.h"

RoomManager* RoomManager::_Instance;

/*---------------------------------------------------------------------------------------------
�̸�     : RoomManager::GetInstance
�뵵     : RoomManager�� �޾ƿ��� �̱��� �Լ�
������   : �̹α�
������¥ : 2022.09.12
----------------------------------------------------------------------------------------------*/
RoomManager& RoomManager::GetInstance()
{
	if(_Instance == nullptr)
		_Instance = Gnew<RoomManager>();

	return *_Instance;
}

/*---------------------------------------------------------------------------------------------
�̸�     : RoomManager::Add
�뵵     : GameRoom�� Map�� �߰��ϴ� �Լ�
������   : �̹α�
������¥ : 2022.09.12
----------------------------------------------------------------------------------------------*/
GameRoom* RoomManager::Add()
{
	GameRoom* gameroom = Gnew<GameRoom>();

	{
		WRITELOCK;
		gameroom->SetRoomId(_Roomid);
		_Rooms[_Roomid] = gameroom;
		_Roomid++;
	}

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
GameRoom* RoomManager::Find(int32 roomid)
{
	WRITELOCK;

	if (_Rooms.find(roomid) == _Rooms.end())
		return nullptr;

	return _Rooms[roomid];
}

