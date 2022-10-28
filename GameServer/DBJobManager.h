#pragma once

/*---------------------------------------------------------------------------------------------
이름     : DBJobManager
용도     : 싱글톤으로 데이터베이스에 대한 처리를 담당하는 객체
수정자   : 이민규
수정날짜 : 2022.10.18
----------------------------------------------------------------------------------------------*/
class DBJobManager : public JobQueue
{
public:
	static shared_ptr<DBJobManager> GetInstance();
	static void Delete();

#pragma region Transaction
	void SavePlayerStatus(class Player* player,  shared_ptr<class GameRoom> room);
	void RewardPlayer(class Player* player, class RewardData* reward, shared_ptr<class GameRoom> room);
#pragma endregion

#pragma region Noti
	void EquipItemNoti(class Player* player, class Item * item);
#pragma endregion

private:
	static shared_ptr<DBJobManager> _Instance;
};

