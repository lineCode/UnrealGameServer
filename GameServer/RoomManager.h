#pragma once

class GameRoom;

/*---------------------------------------------------------------------------------------------
�̸�     : RoomManager
�뵵     : �������� GameRoom�� Map���� �����ϴ� �̱��� ��ü
������   : �̹α�
������¥ : 2022.10.05
----------------------------------------------------------------------------------------------*/
class RoomManager
{
public:
	static RoomManager& GetInstance();

	shared_ptr<GameRoom> Add(int32 mapid);
	bool Remove(int32 roomid);
	shared_ptr<GameRoom> Find(int32 roomid);

private:
	RWLOCK_USE;
	static RoomManager* _Instance;
	Gmap<int32, shared_ptr<GameRoom>> _Rooms;
	int32 _Roomid = 1;
};

