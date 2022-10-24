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
	void SavePlayerStatus(class Player* player,  shared_ptr<class GameRoom> room);
	void RewardPlayer(class Player* player, class RewardData* reward, shared_ptr<class GameRoom> room);

private:
	static shared_ptr<DBJobManager> _Instance;
};

