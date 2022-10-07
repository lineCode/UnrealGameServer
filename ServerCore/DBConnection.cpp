#include "pch.h"
#include "DBConnection.h"

/*---------------------------------------------------------------------------------------------
�̸�     : DBConnection::Connect
�뵵     : ODBC�� ���� DataBase�� �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.10.06
----------------------------------------------------------------------------------------------*/
bool DBConnection::Connect(SQLHENV henv, const WCHAR* connectionstring)
{
	// TODO : [���� �ڵ鷯 �Ҵ�] DBMS ���� ������ ����
	SQLRETURN ret = ::SQLAllocHandle(SQL_HANDLE_DBC, henv, &_Connection);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
		return false;

	WCHAR stringbuffer[MAX_PATH] = { 0 };
	::wcscpy_s(stringbuffer, connectionstring);

	WCHAR resultstring[MAX_PATH] = { 0 };
	SQLSMALLINT resultstringlen = 0;

	// ���� �ڵ鷯�� DBMS�� ����
	ret =  ::SQLDriverConnectW(
		_Connection,
		NULL,
		reinterpret_cast<SQLWCHAR*>(stringbuffer),
		_countof(stringbuffer),
		reinterpret_cast<SQLWCHAR*>(resultstring),
		_countof(resultstring),
		&resultstringlen,
		SQL_DRIVER_NOPROMPT);

	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
		return false;

	// TODO : [��� �ڵ鷯 �Ҵ�] ���� ���� ������ ����
	ret = ::SQLAllocHandle(SQL_HANDLE_STMT, _Connection, &_Statement);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
		return false;

	return true;
}

/*---------------------------------------------------------------------------------------------
�̸�     : DBConnection::Execute
�뵵     : �������� �������ִ� �Լ�
������   : �̹α�
������¥ : 2022.10.06
----------------------------------------------------------------------------------------------*/
bool DBConnection::Execute(const WCHAR* query)
{
	SQLRETURN ret = ::SQLExecDirectW(_Statement, (SQLWCHAR*)query, SQL_NTSL);

	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
		return true;

	HandleError(ret);
	return false;
}

/*---------------------------------------------------------------------------------------------
�̸�     : DBConnection::Fetch
�뵵     : ���� ��� ���� �� �پ� �޾ƿ��� �Լ� 
������   : �̹α�
������¥ : 2022.10.06
----------------------------------------------------------------------------------------------*/
bool DBConnection::Fetch()
{
	SQLRETURN ret = ::SQLFetch(_Statement);

	switch (ret)
	{
	case SQL_SUCCESS :
	case SQL_SUCCESS_WITH_INFO:
		return true;

	case SQL_NO_DATA:
		return false;

	case SQL_ERROR:
		HandleError(ret);
		return false;
	default:
		return true;
	}
}

/*---------------------------------------------------------------------------------------------
�̸�     : DBConnection::BindParam
�뵵     : �÷��� �����͸� �ֱ� ���� ������ ���ε� �ϴ� �Լ�
������   : �̹α�
������¥ : 2022.10.06
----------------------------------------------------------------------------------------------*/
bool DBConnection::BindParam(SQLUSMALLINT paramindex, SQLSMALLINT ctype, SQLSMALLINT sqltype, SQLULEN len ,SQLPOINTER ptr, SQLLEN* index)
{
	SQLRETURN ret =  ::SQLBindParameter(_Statement, paramindex, SQL_PARAM_INPUT, ctype, sqltype, len, 0, ptr, 0, index);

	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		HandleError(ret);
		return false;
	}

	return true;
}

bool DBConnection::BindParam(int32 paramindex, const WCHAR* str, SQLLEN* index)
{
	SQLULEN size = static_cast<SQLULEN>((::wcslen(str) + 1) * 2);
	*index = SQL_NTSL;

	if (size > WVARCHAR_MAX)
		return BindParam(paramindex, SQL_C_WCHAR, SQL_WLONGVARCHAR, size, (SQLPOINTER)str, index);
	else
		return BindParam(paramindex, SQL_C_WCHAR, SQL_WVARCHAR, size, (SQLPOINTER)str, index);
}

bool DBConnection::BindParam(int32 paramindex, BYTE* bin, int32 size, SQLLEN* index)
{
	if (bin == nullptr)
	{
		*index = SQL_NULL_DATA;
		size = 1;
	}
	else
		*index = size;

	if(size > BINARY_MAX)
		return BindParam(paramindex, SQL_C_BINARY, SQL_LONGVARBINARY, size, (BYTE*)bin, index);
	else
		return BindParam(paramindex, SQL_C_BINARY, SQL_BINARY, size, (BYTE*)bin, index);
}


/*---------------------------------------------------------------------------------------------
�̸�     : DBConnection::BindCol
�뵵     : �÷��� ���� ������ �޾ƿ��� ���� ������ ���ε� �ϴ� �Լ�
������   : �̹α�
������¥ : 2022.10.06
----------------------------------------------------------------------------------------------*/
bool DBConnection::BindCol(SQLUSMALLINT columnindex, SQLSMALLINT ctype, SQLULEN len, SQLPOINTER value, SQLLEN* index)
{
	SQLRETURN ret = ::SQLBindCol(_Statement, columnindex, ctype, value, len, index);

	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		HandleError(ret);
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------------------------------
�̸�     : DBConnection::Unbind
�뵵     : ��� ���ε带 �ʱ�ȭ �ϴ� �Լ�
������   : �̹α�
������¥ : 2022.10.06
----------------------------------------------------------------------------------------------*/
void DBConnection::Unbind()
{
	::SQLFreeStmt(_Statement, SQL_UNBIND);
	::SQLFreeStmt(_Statement, SQL_RESET_PARAMS);
	::SQLFreeStmt(_Statement, SQL_CLOSE);
}

/*---------------------------------------------------------------------------------------------
�̸�     : DBConnection::Clear
�뵵     : ȯ�� / ���� / ��� �ڵ��� �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.10.06
----------------------------------------------------------------------------------------------*/
void DBConnection::Clear()
{
	if (_Connection != SQL_NULL_HANDLE)
	{
		::SQLFreeHandle(SQL_HANDLE_DBC, _Connection);
		_Connection = SQL_NULL_HANDLE;
	}

	if (_Statement != SQL_NULL_HANDLE)
	{
		::SQLFreeHandle(SQL_HANDLE_STMT, _Connection);
		_Statement = SQL_NULL_HANDLE;
	}
}

/*---------------------------------------------------------------------------------------------
�̸�     : DBConnection::GetRowCount
�뵵     : ������ ������ ���� ���̺��� �� ������ �������� �Լ�
������   : �̹α�
������¥ : 2022.10.06
----------------------------------------------------------------------------------------------*/
int32 DBConnection::GetRowCount()
{
	SQLLEN count = 0;
	SQLRETURN ret = ::SQLRowCount(_Statement, &count);

	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
		return static_cast<int32>(count);

	return -1;
}

/*---------------------------------------------------------------------------------------------
�̸�     : DBConnection::HandleError
�뵵     : ������ �߻� �� ��� �޼����� ������ִ� �Լ�
������   : �̹α�
������¥ : 2022.10.06
----------------------------------------------------------------------------------------------*/
void DBConnection::HandleError(SQLRETURN ret)
{
	if (ret == SQL_SUCCESS)
		return;

	SQLSMALLINT index = 1;
	SQLWCHAR sqlstate[MAX_PATH] = { 0 };
	SQLINTEGER nativeErr = 0;
	SQLWCHAR errmsg[MAX_PATH] = { 0 };
	SQLSMALLINT msglen = 0;
	SQLRETURN errorret = 0;

	while(true)
	{
		errorret = ::SQLGetDiagRecW(
			SQL_HANDLE_STMT,
			_Statement,
			index,
			sqlstate,
			&nativeErr,
			errmsg,
			_countof(errmsg),
			&msglen);

		if (errorret == SQL_NO_DATA)
			break;

		if(errorret != SQL_SUCCESS && errorret != SQL_SUCCESS_WITH_INFO)
			break;

		// TODO : Log
		wcout.imbue(locale("kor"));
		wcout << errmsg << endl;

		index++;
	}

}
