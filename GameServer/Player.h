#pragma once
#include "GameObject.h"
#include "Protocol.pb.h"

class GameRoom;
class ServerSession;
class Inventory;

/*---------------------------------------------------------------------------------------------
이름     : Player
용도     : 서버에서 플레이어 데이터 저장을 위한 객체
수정자   : 이민규
수정날짜 : 2022.09.19
----------------------------------------------------------------------------------------------*/
class Player :public  GameObject
{
public:
	Player();
	virtual ~Player() override;

	GetSetMaker(shared_ptr<ServerSession>, Session, _Session);
	GetSetMaker(int32, PlayerDbId, _playerdbid);
	GetSetMaker(Inventory*, Inventory, _Inventory);

	void LeaveGame();

private:
	shared_ptr<ServerSession> _Session = nullptr;
	int32 _playerdbid = 0;
	Inventory* _Inventory = nullptr;
};
