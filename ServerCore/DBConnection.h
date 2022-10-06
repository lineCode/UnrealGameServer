#pragma once
#include <sql.h>
#include <sqlext.h>

enum
{
	WVARCHAR_MAX = 4000,
	BINARY_MAX = 8000
};

/*---------------------------------------------------------------------------------------------
이름     : DBConnection
용도     : DB와 연결 후 쿼리를 통해 데이터를 주고 받는 객체
수정자   : 이민규
수정날짜 : 2022.10.06
----------------------------------------------------------------------------------------------*/
class DBConnection
{
public:
	bool Connect(SQLHENV henv, const WCHAR* connectionstring);
	bool Execute(const WCHAR* query);
	bool Fetch();
	void Unbind();
	void Clear();
	int32 GetRowCount();

public:
	bool BindParam(int32 paramindex, bool* value, SQLLEN* index);
	bool BindParam(int32 paramindex, float* value, SQLLEN* index);
	bool BindParam(int32 paramindex, double* value, SQLLEN* index);
	bool BindParam(int32 paramindex, int8* value, SQLLEN* index);
	bool BindParam(int32 paramindex, int16* value, SQLLEN* index);
	bool BindParam(int32 paramindex, int32* value, SQLLEN* index);
	bool BindParam(int32 paramindex, int64* value, SQLLEN* index);
	bool BindParam(int32 paramindex, TIMESTAMP_STRUCT* value, SQLLEN* index);
	bool BindParam(int32 paramindex, const WCHAR* str, SQLLEN* index);
	bool BindParam(int32 paramindex, BYTE* bin , int32 size, SQLLEN* index);

	bool BindCol(int32 columindex, bool* value, SQLLEN* index);
	bool BindCol(int32 columindex, float* value, SQLLEN* index);
	bool BindCol(int32 columindex, double* value, SQLLEN* index);
	bool BindCol(int32 columindex, int8* value, SQLLEN* index);
	bool BindCol(int32 columindex, int16* value, SQLLEN* index);
	bool BindCol(int32 columindex, int32* value, SQLLEN* index);
	bool BindCol(int32 columindex, int64* value, SQLLEN* index);
	bool BindCol(int32 columindex, TIMESTAMP_STRUCT* value, SQLLEN* index);
	bool BindCol(int32 columindex, const WCHAR* str, int32 size ,SQLLEN* index);
	bool BindCol(int32 columindex, BYTE* bin,  int32 size ,  SQLLEN* index);


private:
	bool BindParam(SQLUSMALLINT paramindex, SQLSMALLINT ctype, SQLSMALLINT sqltype, SQLULEN len, SQLPOINTER ptr, SQLLEN* index);
	bool BindCol(SQLUSMALLINT columnindex, SQLSMALLINT ctype, SQLULEN len, SQLPOINTER value, SQLLEN* index);
	void HandleError(SQLRETURN ret);

private:
	SQLHDBC _Connection = SQL_NULL_HANDLE;
	SQLHSTMT _Statement = SQL_NULL_HANDLE;
};

