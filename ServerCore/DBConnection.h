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
	bool BindParam(int32 paramindex, bool* value, SQLLEN* index) { return BindParam(paramindex, SQL_C_TINYINT, SQL_TINYINT, sizeof(bool), value, index); }
	bool BindParam(int32 paramindex, float* value, SQLLEN* index){ return BindParam(paramindex, SQL_C_FLOAT, SQL_REAL, 0, value, index); }
	bool BindParam(int32 paramindex, double* value, SQLLEN* index) { return BindParam(paramindex, SQL_C_DOUBLE, SQL_DOUBLE, 0, value, index); }
	bool BindParam(int32 paramindex, int8* value, SQLLEN* index){ return BindParam(paramindex, SQL_C_TINYINT, SQL_TINYINT, sizeof(int8), value, index); }
	bool BindParam(int32 paramindex, int16* value, SQLLEN* index){ return BindParam(paramindex, SQL_C_SHORT, SQL_SMALLINT, sizeof(int16), value, index); }
	bool BindParam(int32 paramindex, int32* value, SQLLEN* index){ return BindParam(paramindex, SQL_C_LONG, SQL_INTEGER, sizeof(int32), value, index); }
	bool BindParam(int32 paramindex, int64* value, SQLLEN* index) { return BindParam(paramindex, SQL_C_SBIGINT, SQL_BIGINT, sizeof(int64), value, index); }
	bool BindParam(int32 paramindex, TIMESTAMP_STRUCT* value, SQLLEN* index) { return BindParam(paramindex, SQL_C_TIMESTAMP, SQL_TYPE_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), value, index); }
	bool BindParam(int32 paramindex, const WCHAR* str, SQLLEN* index);
	bool BindParam(int32 paramindex, BYTE* bin , int32 size, SQLLEN* index);

	bool BindCol(int32 columindex, bool* value, SQLLEN* index) { return BindCol(columindex, SQL_C_TINYINT, sizeof(bool), value, index); }
	bool BindCol(int32 columindex, float* value, SQLLEN* index) { return BindCol(columindex, SQL_C_DOUBLE, sizeof(float), value, index); }
	bool BindCol(int32 columindex, double* value, SQLLEN* index) { return BindCol(columindex, SQL_C_DOUBLE, sizeof(double), value, index); }
	bool BindCol(int32 columindex, int8* value, SQLLEN* index) { return BindCol(columindex, SQL_C_TINYINT, sizeof(int8), value, index); }
	bool BindCol(int32 columindex, int16* value, SQLLEN* index){ return BindCol(columindex, SQL_C_SHORT, sizeof(int16), value, index); }
	bool BindCol(int32 columindex, int32* value, SQLLEN* index){ return BindCol(columindex, SQL_C_LONG, sizeof(int32), value, index); }
	bool BindCol(int32 columindex, int64* value, SQLLEN* index) { return BindCol(columindex, SQL_C_SBIGINT, sizeof(int64), value, index); }
	bool BindCol(int32 columindex, TIMESTAMP_STRUCT* value, SQLLEN* index) { return BindCol(columindex, SQL_C_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), value, index); }
	bool BindCol(int32 columindex, const WCHAR* str, int32 size ,SQLLEN* index) { return BindCol(columindex, SQL_C_WCHAR, size, (SQLPOINTER)str, index); }
	bool BindCol(int32 columindex, BYTE* bin,  int32 size ,  SQLLEN* index) { return BindCol(columindex, SQL_BINARY, size, bin, index); }

private:
	bool BindParam(SQLUSMALLINT paramindex, SQLSMALLINT ctype, SQLSMALLINT sqltype, SQLULEN len, SQLPOINTER ptr, SQLLEN* index);
	bool BindCol(SQLUSMALLINT columnindex, SQLSMALLINT ctype, SQLULEN len, SQLPOINTER value, SQLLEN* index);
	void HandleError(SQLRETURN ret);

private:
	SQLHDBC _Connection = SQL_NULL_HANDLE;
	SQLHSTMT _Statement = SQL_NULL_HANDLE;
};

