#pragma once

#include "DBConnection.h"

/*---------------------------------------------------------------------------------------------
이름     : DBConnectionPool
용도     : DBConnection을 Pool형태로 들고있는 객체
수정자   : 이민규
수정날짜 : 2022.10.06
----------------------------------------------------------------------------------------------*/
class DBConnectionPool
{
public:
	DBConnectionPool();
	~DBConnectionPool();

	bool Connect(int32 connectioncount, const WCHAR* connectionstring);
	DBConnection* Pop();
	void Push(DBConnection* connection);
	void Clear();

private:
	RWLOCK_USE;
	SQLHENV _Environment = SQL_NULL_HANDLE;
	Gvector<DBConnection*> _Connections;
};

