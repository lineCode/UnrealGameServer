#include "pch.h"
#include "DataManager.h"

 DataManager* DataManager::_Instance = nullptr;

 /*---------------------------------------------------------------------------------------------
 �̸�     : DataManager::GetInstance
 �뵵     : DataManager �ν��Ͻ��� ��ȯ���ִ� �Լ�
 ������   : �̹α�
 ������¥ : 2022.10.2
 ----------------------------------------------------------------------------------------------*/
 DataManager* DataManager::GetInstacnce()
 {
 	if (_Instance == nullptr)
 		_Instance = new DataManager();

 	return _Instance;
 }

 /*---------------------------------------------------------------------------------------------
 �̸�     : DataManager::DestroyInstance
 �뵵     : DataManager �ν��Ͻ��� �Ҹ����ִ� �Լ�
 ������   : �̹α�
 ������¥ : 2022.10.2
 ----------------------------------------------------------------------------------------------*/
 void DataManager::DestroyInstance()
 {
 	if (_Instance != nullptr)
 		delete _Instance;

 	_Instance = nullptr;
 }

 /*---------------------------------------------------------------------------------------------
 �̸�     : DataManager::LodaData
 �뵵     : DataManager�� �⺻ �� �ʱ�ȭ �� ���� �Լ�
 ������   : �̹α�
 ������¥ : 2022.10.2
 ----------------------------------------------------------------------------------------------*/
 void DataManager::LodaData()
 {
 	// TODO : PlayerStat ������ �ҷ����� �κ�
     _PlayerStatDatas = LoadJson<playerStatData, int32, Protocol::StatInfo>("StatData").MakeGHashMap();

     // TODO : SkillStat ������ �ҷ����� �κ�
     _SkillStatDatas = LoadJson<SkillStatData, int32, SkillStat>("SkillData").MakeGHashMap(); \
 }





