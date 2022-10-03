#include "pch.h"
#include "DataManager.h"

 DataManager* DataManager::_Instance = nullptr;

 /*---------------------------------------------------------------------------------------------
 이름     : DataManager::GetInstance
 용도     : DataManager 인스턴스를 반환해주는 함수
 수정자   : 이민규
 수정날짜 : 2022.10.2
 ----------------------------------------------------------------------------------------------*/
 DataManager* DataManager::GetInstacnce()
 {
 	if (_Instance == nullptr)
 		_Instance = new DataManager();

 	return _Instance;
 }

 /*---------------------------------------------------------------------------------------------
 이름     : DataManager::DestroyInstance
 용도     : DataManager 인스턴스를 소멸해주는 함수
 수정자   : 이민규
 수정날짜 : 2022.10.2
 ----------------------------------------------------------------------------------------------*/
 void DataManager::DestroyInstance()
 {
 	if (_Instance != nullptr)
 		delete _Instance;

 	_Instance = nullptr;
 }

 /*---------------------------------------------------------------------------------------------
 이름     : DataManager::LodaData
 용도     : DataManager의 기본 값 초기화 및 설정 함수
 수정자   : 이민규
 수정날짜 : 2022.10.2
 ----------------------------------------------------------------------------------------------*/
 void DataManager::LodaData()
 {
 	// TODO : PlayerStat 데이터 불러오는 부분
     _PlayerStatDatas = LoadJson<playerStatData, int32, Protocol::StatInfo>("StatData").MakeGHashMap();

     // TODO : SkillStat 데이터 불러오는 부분
     _SkillStatDatas = LoadJson<SkillStatData, int32, SkillStat>("SkillData").MakeGHashMap(); \
 }





