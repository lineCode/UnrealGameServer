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
�̸�     : DBConnectionPool::Connect
�뵵     : DBConnection�� Pool���·� ����ִ� ��ü
������   : �̹α�
������¥ : 2022.10.06
----------------------------------------------------------------------------------------------*/
bool DBConnectionPool::Connect(int32 connectioncount, const WCHAR* connectionstring)
{
	WRITELOCK;

	// TODO : [ȯ�� �ڵ鷯 �Ҵ�] ODBC ȯ�� ���� ���� ���õ� ������ ����
	SQLRETURN ret = ::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &_Environment);
	if(ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
		return false;

	// ȯ���ڵ鷯�� ODBC ����̺� ���� 3.0 ����
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
�̸�     : DBConnectionPool::Pop
�뵵     : DBConnections�� �ִ� DBConnection �����͸� ��ȯ �Լ�
           �� ���� ��� nullptr ����
������   : �̹α�
������¥ : 2022.10.06
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
�̸�     : DBConnectionPool::Push
�뵵     : DBConnection�� DBConnections�� ��ȯ �ϴ� �Լ�
������   : �̹α�
������¥ : 2022.10.06
----------------------------------------------------------------------------------------------*/
void DBConnectionPool::Push(DBConnection* connection)
{
	WRITELOCK;
	_Connections.push_back(connection);
}

/*---------------------------------------------------------------------------------------------
�̸�     : DBConnectionPool::Clear
�뵵     : DBConnections�� ��� �����͸� ����
������   : �̹α�
������¥ : 2022.10.06
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
