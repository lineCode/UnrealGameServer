#pragma once
#include "PacketSession.h"
#include "Protocol.pb.h"

/*---------------------------------------------------------------------------------------------
�̸�     : ServerSession
�뵵     : Ŭ���̾�Ʈ�� ������ ��ſ� ���Ǵ� ���� ��ü
������   : �̹α�
������¥ : 2022.09.12
----------------------------------------------------------------------------------------------*/
class Player;

class ServerSession : public PacketSession
{
public:
	ServerSession() {};
	~ServerSession() {}

#pragma region NetWork
public:
	shared_ptr<ServerSession> GetServerSession() { return static_pointer_cast<ServerSession>(shared_from_this()); }

	void SetMyPlayer(Player* player) { _MyPlayer = player; }
	Player* GetMyPlayer() const { return _MyPlayer; }

protected:
	virtual void ContentsConnect() override;
	virtual void ContentsDisConnect() override;
	virtual void ContentsSend() override;
	virtual void ContentsRecv(BYTE* buffer, int32 datasize) override;

private:
	Player* _MyPlayer = nullptr;
#pragma endregion

#pragma region Game
public:
	void SetServerState(Protocol::PlayerServerState state) { _ServerState = state; }
	Protocol::PlayerServerState GetServerState() { return _ServerState; }

	bool Login(Protocol::CLIENT_LOGIN* pkt);
	bool CreatePlayer(Protocol::CLIENT_CREATEPLAYER* pkt);
	bool EnterPlayer(Protocol::CLIENT_ENTERGAME* pkt);

private:
	Protocol::PlayerServerState _ServerState = Protocol::PlayerServerState::SERVERSTATE_LOGIN;
	int32 _Accountid = 0;
	Gmap<wstring , Protocol::LobbyPlayerInfo> _LobbyPlayers;

#pragma endregion

};

