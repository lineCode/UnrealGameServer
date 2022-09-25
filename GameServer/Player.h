#pragma once
#include "GameObject.h"
#include "Protocol.pb.h"

class GameRoom;
class ServerSession;

/*---------------------------------------------------------------------------------------------
�̸�     : Player
�뵵     : �������� �÷��̾� ������ ������ ���� ��ü
������   : �̹α�
������¥ : 2022.09.19
----------------------------------------------------------------------------------------------*/
class Player :public  GameObject
{
public:
	Player();
	virtual ~Player() override;

	void SetSession(shared_ptr<ServerSession> session) { _Session = session; }
	shared_ptr<ServerSession> GetSession(){ return _Session; }
private:
	shared_ptr<ServerSession> _Session = nullptr;
};
