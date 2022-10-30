#include "pch.h"
#include "ServerSession.h"
#include "DataManager.h"
#include "DBConnectionPool.h"
#include "GameRoom.h"
#include "Player.h"
#include "ObjectManager.h"
#include "ProcedureManager.h"
#include "RoomManager.h"
#include "ServerPacketManager.h"
#include "ConverString.h"
#include "Equipment.h"
#include "Inventory.h"
#include "Item.h"

#pragma region NetWork
/*---------------------------------------------------------------------------------------------
이름     : ServerSession::ContentsConnect
용도     : 서버에 연결이 성공했을 때 발동하는 함수
           연결이 되었음을 클라이언트에게 패킷을 전달해서 알려줌
수정자   : 이민규
수정날짜 : 2022.09.24
----------------------------------------------------------------------------------------------*/
void ServerSession::ContentsConnect()
{
	Protocol::SERVER_CONNECT connectpacket;
	connectpacket.set_connect(1);
		
	SendCheck( ServerPacketManager::MakeSendBuffer(connectpacket));
}

/*---------------------------------------------------------------------------------------------
이름     : ServerSession::ContentsDisConnect
용도     : 서버에서 연결이 끊어졌을 경우 발동하는 함수
		   플레이어를 방에서 퇴장시키고 플레이어를 서버에서 제거함
수정자   : 이민규
수정날짜 : 2022.09.12
----------------------------------------------------------------------------------------------*/
void ServerSession::ContentsDisConnect()
{
	auto room = RoomManager::GetInstance().Find(1);
	room->PushAsync(&GameRoom::LeaveGame, _MyPlayer->GetInfo().objectid());
}

void ServerSession::ContentsSend()
{
	PacketSession::ContentsSend();
}

/*---------------------------------------------------------------------------------------------
이름     : ServerSession::ContentsRecv
용도     : 서버에서 클라의 Send를 받았을 때 발동하는 함수
		   완성된 패킷만 오며 패킷에 맞게 PacketUpdate를 통해 처리해줌
수정자   : 이민규
수정날짜 : 2022.09.12
----------------------------------------------------------------------------------------------*/
void ServerSession::ContentsRecv(BYTE* buffer, int32 datasize)
{
	shared_ptr<ServerSession> session = GetServerSession();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	
	ServerPacketManager::PacketUpdate(session, buffer, datasize);
}

#pragma endregion

#pragma region Game
/*---------------------------------------------------------------------------------------------
이름     : ServerSession::Login
용도     : 서버에 로그인 패킷을 처리하는 함수
		   클라이언트의 아이디를 통해 로그인 요청을 받는 함수
		   성공할 경우 로그인 패킷을 보내주고 실패 할 경우 무시
수정자   : 이민규
수정날짜 : 2022.10.16
----------------------------------------------------------------------------------------------*/
bool ServerSession::Login(Protocol::CLIENT_LOGIN* pkt)
{
	if (_ServerState != Protocol::SERVERSTATE_LOGIN)
		return false;

	_LobbyPlayers.clear();
	DBConnection* dbConn = GDBConnectionPool->Pop();

	WCHAR username[100];
	int32 objectid;
	int32 accountid;
	int32 attack;
	int32 Hp;
	int32 MaxHp;
	int32 level;
	int32 playerid;
	int32 speed;
	int32 exp;

	ConverString accountname(pkt->uniqueid());

	Protocol::SERVER_LOGIN pktlogin;
	pktlogin.set_loginsuccess(1);

	ProcedureManager::FindAccountid findid(*dbConn);
	int32 id;

	findid.Param_Name(accountname.GetWCHAR() , accountname.GetSize());
	findid.Column_Accountid(id);
	if (findid.Execute() == false)
		return false;

	while (findid.Fetch())
	{
		ProcedureManager::FindAccountPlayer accountplayer(*dbConn);
		accountplayer.Param_Name(accountname.GetWCHAR(), accountname.GetSize());

		accountplayer.Column_Playerid(playerid);
		accountplayer.Column_AccountId(accountid);
		accountplayer.Column_Playername(username);
		accountplayer.Column_Objectid(objectid);
		accountplayer.Column_Attack(attack);
		accountplayer.Column_Hp(Hp);
		accountplayer.Column_Level(level);
		accountplayer.Column_MaxHp(MaxHp);
		accountplayer.Column_Speed(speed);
		accountplayer.Column_TotalExp(exp);

	
		if (accountplayer.Execute() == false)
			return false;

		// TODO : 존재할 경우 캐릭터 정보를 보내줌

		while (accountplayer.Fetch())
		{
			Protocol::LobbyPlayerInfo player;
			player.set_playerdbid(playerid);
			const auto stat = player.mutable_statinfo();
			player.set_name(ConverString(username).GetCHAR());
			player.set_objectid(objectid);
			stat->set_level(level);
			stat->set_maxhp(MaxHp);
			stat->set_hp(Hp);
			stat->set_attack(attack);
			stat->set_speed(speed);
			stat->set_totalexp(exp);

			// TODO : 메모리에도 들고 있는다
			_LobbyPlayers[ConverString(username).GetWCHAR()] = player;

			// TODO : 패킷에 넣어준다
			pktlogin.add_players()->CopyFrom(player);
		}

		_Accountid = id;
		GConsoleLogManager->WriteStdOut(Color::GREEN, L"[LOGIN_SUCCESS] : %s\n", accountname.GetWCHAR());
		SendCheck(ServerPacketManager::MakeSendBuffer(pktlogin));
		_ServerState = Protocol::SERVERSTATE_LOBBY;
		GDBConnectionPool->Push(dbConn);
		return true;
	}

	// TODO : 존재하지 않을 경우 계정 생성
	ProcedureManager::CreateAccount createaccount(*dbConn);
	createaccount.Param_Name(accountname.GetWCHAR(), accountname.GetSize());
	if (createaccount.Execute() == false)
		return false;

	ProcedureManager::FindAccountid findaccountid(*dbConn);
	findaccountid.Param_Name(accountname.GetWCHAR(), accountname.GetSize());
	findaccountid.Column_Accountid(id);
	if (findaccountid.Execute() == false)
		return false;

	while (findaccountid.Fetch())
	{
		GConsoleLogManager->WriteStdOut(Color::GREEN, L"[ACCOUNT_CREATE] : %s %d \n", accountname.GetWCHAR(), id);
		_Accountid = id;
	}

	SendCheck(ServerPacketManager::MakeSendBuffer(pktlogin));
	_ServerState = Protocol::SERVERSTATE_LOBBY;

	GDBConnectionPool->Push(dbConn);
	return true;
}

/*---------------------------------------------------------------------------------------------
이름     : ServerSession::CreatePlayer
용도     : 서버에 캐릭터를 생성하는 부분을 처리해주는 함수
		   클라이언트가 요청하는 값으로 캐릭터를 생성해 줌
수정자   : 이민규
수정날짜 : 2022.10.16
----------------------------------------------------------------------------------------------*/
bool ServerSession::CreatePlayer(Protocol::CLIENT_CREATEPLAYER* pkt)
{
	if (_ServerState != Protocol::SERVERSTATE_LOBBY)
		return false;

	// 1레벨 스텟 정보 추출
	Protocol::StatInfo datastat;
	datastat = DataManager::GetInstacnce()->GetPlayerStatData(1);

	DBConnection* dbConn = GDBConnectionPool->Pop();
	ProcedureManager::PlayerCreate playercreate(*dbConn);

	ConverString playername(pkt->name());

	playercreate.Param_Name(playername.GetWCHAR(), playername.GetSize());
	playercreate.Param_Accountid(_Accountid);
	playercreate.Param_Attack(datastat.attack());
	playercreate.Param_Hp(datastat.hp());
	playercreate.Param_Level(datastat.level());
	playercreate.Param_Maxhp(datastat.maxhp());
	playercreate.Param_Speed(datastat.speed());
	playercreate.Param_Totalexp(0);
	playercreate.Param_Objectid(1);
	if (playercreate.Execute() == false)
		return false;

	ProcedureManager::FindAccountid findid(*dbConn);
	int32 id;

	findid.Param_Name(playername.GetWCHAR(), playername.GetSize());
	findid.Column_Accountid(id);
	if (findid.Execute() == false)
		return false;

	if (findid.Fetch() == -1)
		return false;

	Protocol::LobbyPlayerInfo player;
	player.set_playerdbid(id);
	player.mutable_statinfo()->CopyFrom(datastat);
	player.set_name(pkt->name());
	player.set_objectid(1);

	_LobbyPlayers[playername.GetWCHAR()] = player;

	Protocol::SERVER_CREATEPLAYER createpkt;

	createpkt.mutable_player()->CopyFrom(player);

	SendCheck(ServerPacketManager::MakeSendBuffer(createpkt));

	GConsoleLogManager->WriteStdOut(Color::GREEN, L"[PLAYER_CREATE] : %s  \n", playername.GetWCHAR());
	GDBConnectionPool->Push(dbConn);
}

/*---------------------------------------------------------------------------------------------
이름     : ServerSession::EnterPlayer
용도     : 서버에서 클라이언트가 선택한 캐릭터를 입장시키는 함수
		   클라이언트가 캐릭터의 이름을 보내주면 있을 경우 저장되어있는 값으로 캐릭터를 입장
수정자   : 이민규
수정날짜 : 2022.10.28
----------------------------------------------------------------------------------------------*/
bool ServerSession::EnterPlayer(Protocol::CLIENT_ENTERGAME* pkt)
{
	if (_ServerState != Protocol::SERVERSTATE_LOBBY)
		return false;

	DBConnection* dbConn = GDBConnectionPool->Pop();
	ProcedureManager::FindPlayerid findplayer(*dbConn);

	int32 id;
	ConverString playername(pkt->name());
	
	findplayer.Param_Name(playername.GetWCHAR(), playername.GetSize());
	findplayer.Column_Playerid(id);

	if (findplayer.Execute() == false)
		return false;

	while (findplayer.Fetch())
	{
		Player * player = ObjectManager::GetInstance().Add<Player>(1);

		// TODO  : 플레이어 정보를 가져옴
		player->GetInfo().set_name(pkt->name());
		player->SetStat(_LobbyPlayers[playername.GetWCHAR()].statinfo());
		player->SetPlayerDbId(_LobbyPlayers[playername.GetWCHAR()].playerdbid());
		Protocol::Vector vector;
		vector.set_x(-250);
		vector.set_y(-200);
		vector.set_z(97.65);
		Protocol::Rotator rotator;
		rotator.set_roll(0);
		rotator.set_pitch(0);
		rotator.set_yaw(0);
		player->SetVector(vector);
		player->SetRotator(rotator);
		SetMyPlayer(player);
		player->SetSession(GetServerSession());

		// 플레이어 능력치를 보내줌
		Protocol::SERVER_ENTERGAME enterpkt;

		auto setplayer = enterpkt.mutable_player();
		setplayer->set_objectid(player->GetId());
		setplayer->set_name(pkt->name());
		setplayer->mutable_statinfo()->CopyFrom(player->GetStat());

		SendCheck(ServerPacketManager::MakeSendBuffer(enterpkt));

		// 플레이어 아이템을 가져옴
		ProcedureManager::FindPlayerItemList listitem(*dbConn);
		Protocol::SERVER_ITEMLIST itempkt;
		int32 dbid;
		int32 gameid;
		int32 count;
		int32 slot;
		int32 Equip;

		listitem.Param_Playerid(id);
		listitem.Column_DbId(dbid);
		listitem.Column_Gameid(gameid);
		listitem.Column_Count(count);
		listitem.Column_Slot(slot);
		listitem.Column_Equipped(Equip);

		listitem.Execute();
		while(listitem.Fetch())
		{
			Item* item = Item::MakeItem(dbid, gameid ,count ,slot , Equip);
			if (item == nullptr)
				break;

			// 장착 아이템이냐 인벤토리냐 구분
			if (slot >= 0 && slot <= 20)
				player->GetInventory()->Add(item);
			else
				player->GetEquipment()->Add(item);
			
			itempkt.add_items()->CopyFrom(item->Getinfo());
		}
	
		SendCheck(ServerPacketManager::MakeSendBuffer(itempkt));

		// TODO : 방에 입장
		const auto room = RoomManager::GetInstance().Find(1);
		room->PushAsync(&GameRoom::EnterGame , static_cast<GameObject*>(player));

		_ServerState = Protocol::SERVERSTATE_GAME;

		GConsoleLogManager->WriteStdOut(Color::GREEN, L"[ENTER_PLAYER_SUCCESS] : %s  \n", playername.GetWCHAR());
		GDBConnectionPool->Push(dbConn);
		return true;
	}


	GConsoleLogManager->WriteStdOut(Color::RED, L"[ENTER_PLAYER_FAIL] : %s  \n", playername.GetWCHAR());
	GDBConnectionPool->Push(dbConn);
	return false;
}
#pragma endregion
