#pragma once

#include "DBConnection.h"

/*---------------------------------------------------------------------------------------------
�̸�     : DBConnectionPool
�뵵     : DBConnection�� Pool���·� ����ִ� ��ü
������   : �̹α�
������¥ : 2022.10.06
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

