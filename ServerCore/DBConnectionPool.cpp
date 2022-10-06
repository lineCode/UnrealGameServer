#include "pch.h"
#include "DBConnectionPool.h"

DBConnectionPool::DBConnectionPool()
{
}

DBConnectionPool::~DBConnectionPool()
{
	Clear();
}

/*---------------------------------------------------------------------------------------------
이름     : DBConnectionPool::Connect
용도     : DBConnection을 Pool형태로 들고있는 객체
수정자   : 이민규
수정날짜 : 2022.10.06
----------------------------------------------------------------------------------------------*/
bool DBConnectionPool::Connect(int32 connectioncount, const WCHAR* connectionstring)
{
	WRITELOCK;

	// TODO : [환경 핸들러 할당] ODBC 환결 설정 값과 관련된 정보를 저장
	SQLRETURN ret = ::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &_Environment);
	if(ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
		return false;

	// 환경핸들러에 ODBC 드라이브 버전 3.0 설정
	ret = ::SQLSetEnvAttr(_Environment, SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), 0);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
		return false;

	for(int32 i = 0; i < connectioncount; i++)
	{
		DBConnection* connection = Gnew<DBConnection>();
		if (connection->Connect(_Environment, connectionstring) == false)
			return false;

		_Connections.push_back(connection);
	}

	return  true;
}

/*---------------------------------------------------------------------------------------------
이름     : DBConnectionPool::Pop
용도     : DBConnections에 있는 DBConnection 데이터를 반환 함수
           단 없을 경우 nullptr 제공
수정자   : 이민규
수정날짜 : 2022.10.06
----------------------------------------------------------------------------------------------*/
DBConnection* DBConnectionPool::Pop()
{
	WRITELOCK;

	if (_Connections.empty())
		return nullptr;

	DBConnection* connection = _Connections.back();
	_Connections.pop_back();

	return connection;
}

/*---------------------------------------------------------------------------------------------
이름     : DBConnectionPool::Push
용도     : DBConnection을 DBConnections에 반환 하는 함수
수정자   : 이민규
수정날짜 : 2022.10.06
----------------------------------------------------------------------------------------------*/
void DBConnectionPool::Push(DBConnection* connection)
{
	WRITELOCK;
	_Connections.push_back(connection);
}

/*---------------------------------------------------------------------------------------------
이름     : DBConnectionPool::Clear
용도     : DBConnections에 모든 데이터를 제거
수정자   : 이민규
수정날짜 : 2022.10.06
----------------------------------------------------------------------------------------------*/
void DBConnectionPool::Clear()
{
	WRITELOCK;

	if(_Environment != SQL_NULL_HANDLE)
	{
		::SQLFreeHandle(SQL_HANDLE_ENV, _Environment);
		_Environment = SQL_NULL_HANDLE;
	}

	for (auto& connection : _Connections)
		Gdelete(connection);

	_Connections.clear();
}
