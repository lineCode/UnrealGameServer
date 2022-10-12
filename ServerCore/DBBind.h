#pragma once
#include "DBConnection.h"

template<int32 C>
struct FullBits {enum {value = (1 << (C-1)) | FullBits<C-1>::value};};

template<>
struct FullBits<1> {enum {value = 1};};

template<>
struct FullBits<0> { enum { value = 0 }; };

/*---------------------------------------------------------------------------------------------
이름     : DBBind
용도     : DBConnection의 ParamBind 와 ColBInd를 안정성 있고 편하게 돕는 객체
수정자   : 이민규
수정날짜 : 2022.10.07
----------------------------------------------------------------------------------------------*/
template<int32 ParamCount , int32 ColumnCount>
class DBBind
{
public:
	DBBind(DBConnection& dbconnection, const WCHAR* query) : _dbConnection(dbconnection), _query(query)
	{
		::memset(_paramlens, 0, sizeof(_paramlens));
		::memset(_columlens, 0, sizeof(_columlens));
		_paramFlag = 0;
		_columFlag = 0;
		_paramindex = 0;
		_columindex = 0;
		dbconnection.Unbind();
	}

	/*---------------------------------------------------------------------------------------------
	이름     : DBBind::Validate
	용도     : DBConnection의 Bind가 옳게 되었는지 확인 하는 함수
	수정자   : 이민규
	수정날짜 : 2022.10.07
----------------------------------------------------------------------------------------------*/
	bool Validate()
	{
		return _paramFlag == FullBits<ParamCount>::value && _columFlag == FullBits<ColumnCount>::value;
	}

	/*---------------------------------------------------------------------------------------------
	이름     : DBBind::Execute
	용도     : DBConnection의 Bind가 옳게 되었는지 확인 후 Execute를 호출 하는 함수
	수정자   : 이민규
	수정날짜 : 2022.10.07
	----------------------------------------------------------------------------------------------*/
	bool Execute()
	{
		CRASH_IF(Validate() == false);
		return _dbConnection.Execute(_query);
	}

	/*---------------------------------------------------------------------------------------------
	이름     : DBBind::Fetch
	용도     : DBConnection의 Fetch를 호출하는 함수
	수정자   : 이민규
	수정날짜 : 2022.10.07
	----------------------------------------------------------------------------------------------*/
	bool Fetch()
	{
		return _dbConnection.Fetch();
	}

public:
	template<typename T>
	void BindParam(T& value)
	{
		_dbConnection.BindParam(_paramindex +1, &value, &_paramlens[_paramindex]);
		_paramFlag |= (1LL << _paramindex++);
	}

	void BindParam(const WCHAR* value)
	{
		_dbConnection.BindParam(_paramindex +1, value, &_paramlens[_paramindex]);
		_paramFlag |= (1LL << _paramindex++);
	}

	template<typename T, int32 N>
	void BindParam(T(&value)[N])
	{
		_dbConnection.BindParam(_paramindex + 1, (BYTE*)value, sizeof(T) * N, &_paramlens[_paramindex]);
		_paramFlag |= (1LL << _paramindex++);
	}

	template<typename T>
	void BindParam(T* value, int32 N)
	{
		_dbConnection.BindParam(_paramindex + 1, (BYTE*)value, sizeof(T) * N, &_paramlens[_paramindex]);
		_paramFlag |= (1LL << _paramindex++);
	}

	template<typename T>
	void BindCol(T& value)
	{
		_dbConnection.BindCol(_columindex + 1, &value, &_columlens[_columindex]);
		_columFlag |= (1LL << _columindex++);
	}

	template<int32 N>
	void BindCol( WCHAR(&value)[N])
	{
		_dbConnection.BindCol(_columindex + 1, value, N - 1, &_columlens[_columindex]);
		_columFlag |= (1LL << _columindex++);
	}

	void BindCol(WCHAR* value, int32 len)
	{
		_dbConnection.BindCol(_columindex + 1, value, len - 1, &_columlens[_columindex]);
		_columFlag |= (1LL << _columindex++);
	}

	template<typename T, int32 N>
	void BindCol(T(&value)[N])
	{
		_dbConnection.BindCol(_columindex + 1, value, sizeof(T) * N, &_columlens[_columindex]);
		_columFlag |= (1LL << _columindex++);
	}

protected:
	DBConnection& _dbConnection;
	const WCHAR* _query;
	SQLLEN _paramlens[ParamCount > 0 ? ParamCount : 1] = {};
	SQLLEN _columlens[ColumnCount > 0 ? ColumnCount : 1] = {};
	int32 _paramindex = 0;
	int32 _columindex = 0;
	uint64 _paramFlag = 0;
	uint64 _columFlag = 0;

};

