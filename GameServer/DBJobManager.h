#pragma once

/*---------------------------------------------------------------------------------------------
�̸�     : DBJobManager
�뵵     : �̱������� �����ͺ��̽��� ���� ó���� ����ϴ� ��ü
������   : �̹α�
������¥ : 2022.10.18
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

