#pragma once
#include "DBConnection.h"

template<int32 C>
struct FullBits { enum { value = (1 << (C - 1)) | FullBits<C - 1>::value }; };

template<>
struct FullBits<1> { enum { value = 1 }; };

template<>
struct FullBits<0> { enum { value = 0 }; };

/*---------------------------------------------------------------------------------------------
�̸�     : DBBind
�뵵     : DBConnection�� ParamBind �� ColBInd�� ������ �ְ� ���ϰ� ���� ��ü
������   : �̹α�
������¥ : 2022.10.07
----------------------------------------------------------------------------------------------*/
template<int32 ParamCount, int32 ColumnCount>
class DBBind
{
public:
	DBBind(DBConnection& dbConnection, const WCHAR* query)
		: _dbConnection(dbConnection), _query(query)
	{
		::memset(_paramSQLlens, 0, sizeof(_paramSQLlens));
		::memset(_columnSQLlens, 0, sizeof(_columnSQLlens));
		_paramFlag = 0;
		_columnFlag = 0;
		dbConnection.Unbind();
	}

	/*---------------------------------------------------------------------------------------------
	�̸�     : DBBind::Validate
	�뵵     : DBConnection�� Bind�� �ǰ� �Ǿ����� Ȯ�� �ϴ� �Լ�
	������   : �̹α�
	������¥ : 2022.10.07
	----------------------------------------------------------------------------------------------*/
	bool Validate()
	{
		return _paramFlag == FullBits<ParamCount>::value && _columnFlag == FullBits<ColumnCount>::value;
	}

	/*---------------------------------------------------------------------------------------------
	�̸�     : DBBind::Execute
	�뵵     : DBConnection�� Bind�� �ǰ� �Ǿ����� Ȯ�� �� Execute�� ȣ�� �ϴ� �Լ�
	������   : �̹α�
	������¥ : 2022.10.07
	----------------------------------------------------------------------------------------------*/
	bool Execute()
	{
		CRASH_IF(Validate() == false);
		return _dbConnection.Execute(_query);
	}

	/*---------------------------------------------------------------------------------------------
	�̸�     : DBBind::Fetch
	�뵵     : DBConnection�� Fetch�� ȣ���ϴ� �Լ�
	������   : �̹α�
	������¥ : 2022.10.07
	----------------------------------------------------------------------------------------------*/
	bool Fetch()
	{
		return _dbConnection.Fetch();
	}

public:
	template<typename T>
	void BindParam(int32 idx, T& value)
	{
		_dbConnection.BindParam(idx + 1, &value, &_paramSQLlens[idx]);
		_paramFlag |= (1LL << idx);
	}

	void BindParam(int32 idx, const WCHAR* value)
	{
		_dbConnection.BindParam(idx + 1, value, &_paramSQLlens[idx]);
		_paramFlag |= (1LL << idx);
	}

	template<typename T, int32 N>
	void BindParam(int32 idx, T(&value)[N])
	{
		_dbConnection.BindParam(idx + 1, (BYTE*)value, sizeof(T) * N, &_paramSQLlens[idx]);
		_paramFlag |= (1LL << idx);
	}

	template<typename T>
	void BindParam(int32 idx, T* value, int32 N)
	{
		_dbConnection.BindParam(idx + 1, (BYTE*)value, sizeof(T) * N, &_paramSQLlens[idx]);
		_paramFlag |= (1LL << idx);
	}

	template<typename T>
	void BindCol(int32 idx, T& value)
	{
		_dbConnection.BindCol(idx + 1, &value, &_columnSQLlens[idx]);
		_columnFlag |= (1LL << idx);
	}

	template<int32 N>
	void BindCol(int32 idx, WCHAR(&value)[N])
	{
		_dbConnection.BindCol(idx + 1, value, N - 1, &_columnSQLlens[idx]);
		_columnFlag |= (1LL << idx);
	}

	void BindCol(int32 idx, WCHAR* value, int32 len)
	{
		_dbConnection.BindCol(idx + 1, value, len - 1, &_columnSQLlens[idx]);
		_columnFlag |= (1LL << idx);
	}

	template<typename T, int32 N>
	void BindCol(int32 idx, T(&value)[N])
	{
		_dbConnection.BindCol(idx + 1, value, sizeof(T) * N, &_columnSQLlens[idx]);
		_columnFlag |= (1LL << idx);
	}

protected:
	DBConnection& _dbConnection;
	const WCHAR* _query;
	SQLLEN			_paramSQLlens[ParamCount > 0 ? ParamCount : 1];
	SQLLEN			_columnSQLlens[ColumnCount > 0 ? ColumnCount : 1];
	uint64			_paramFlag;
	uint64			_columnFlag;
};

