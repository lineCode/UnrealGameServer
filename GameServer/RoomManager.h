#pragma once

class GameRoom;

/*---------------------------------------------------------------------------------------------
�̸�     : RoomManager
�뵵     : �������� GameRoom�� Map���� �����ϴ� �̱��� ��ü
������   : �̹α�
������¥ : 2022.09.12
----------------------------------------------------------------------------------------------*/
class RoomManager
{
public:
	static RoomManager& GetInstance();

	GameRoom* Add();
	bool Remove(int32 roomid);
	GameRoom* Find(int32 roomid);

private:
	RWLOCK_USE;
	static RoomManager* _Instance;
	Gmap<int32, GameRoom*> _Rooms;
	int32 _Roomid = 1;
};

