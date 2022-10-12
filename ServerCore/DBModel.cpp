#include "pch.h"
#include "DBModel.h"
#include <format>
#include <regex>

using namespace DBModel;

/*---------------------------------------------------------------------------------------------
이름     : Column::CreateText
용도     : Colum을 위한 텍스트를 만들어주는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
GWString Column::CreateText()
{
	//return format(L"[{}] {} {} {}",
	//              _name,
	//              _typeText,
	//              _nullable ? L"NULL" : L"NOT NULL",
	//              _identity ? format(L"IDENTITY({}, {})", _seedValue, _incrementValue) : L""
	//).data();

	return DBModel::Helpers::Format(
			L"[%s] %s %s %s",
			_name.c_str(),
			_typeText.c_str(),
			_nullable ? L"NULL" : L"NOT NULL",
			_identity ? DBModel::Helpers::Format(L"IDENTITY(%d, %d)", _seedValue, _incrementValue).c_str() : L"");
}

/*---------------------------------------------------------------------------------------------
이름     : Index::GetUniqueName
용도     : Index의 UniqueName을 가져오는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
GWString Index::GetUniqueName()
{
	GWString ret;

	ret += _primaryKey ? L"PK " : L" ";
	ret += _uniqueConstraint ? L"UK " : L" ";
	ret += (_type == IndexType::Clustered ? L"C " : L"NC ");

	for (const shared_ptr<Column>& column : _columns)
	{
		ret += L"*";
		ret += column->_name;
		ret += L" ";
	}

	return ret;
}

/*---------------------------------------------------------------------------------------------
이름     : Index::CreateName
용도     : Index의 Colum의 이름을 텍스트로 만드는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
GWString Index::CreateName(const GWString & tableName)
{
	GWString ret = L"IX_" + tableName;

	for (const shared_ptr<Column>& column : _columns)
	{
		ret += L"_";
		ret += column->_name;
	}

	return ret;
}

/*---------------------------------------------------------------------------------------------
이름     : Index::GetTypeText
용도     : Index의 IndexType을 가져오는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
GWString Index::GetTypeText()
{
	return (_type == IndexType::Clustered ? L"CLUSTERED" : L"NONCLUSTERED");
}

/*---------------------------------------------------------------------------------------------
이름     : Index::GetKeyText
용도     : Index의 KeyText를 가져오는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
GWString Index::GetKeyText()
{
	if (_primaryKey)
		return L"PRIMARY KEY";

	if (_uniqueConstraint)
		return L"UNIQUE";

	return L"";
}

/*---------------------------------------------------------------------------------------------
이름     : Index::CreateColumnsText
용도     : Index의 Cloum을 생성하기 위하 텍스트를 만드는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
GWString Index::CreateColumnsText()
{
	GWString ret;

	const int32 size = static_cast<int32>(_columns.size());
	for (int32 i = 0; i < size; i++)
	{
		if (i > 0)
			ret += L", ";

		ret += DBModel::Helpers::Format(L"[%s]", _columns[i]->_name.c_str());
	}

	return ret;
}

/*---------------------------------------------------------------------------------------------
이름     : Index::DependsOn
용도     : Index에 이름을 통해 항목이 존재하는지 확인하는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
bool Index::DependsOn(const GWString& columnName)
{
	auto findIt = std::find_if(_columns.begin(), _columns.end(),
		[&](const shared_ptr<Column>& column) { return column->_name == columnName; });

	return findIt != _columns.end();
}

/*---------------------------------------------------------------------------------------------
이름     : Table::FindColumn
용도     : Table의 Colum을 찾아주는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
shared_ptr<Column> Table::FindColumn(const GWString& columnName)
{
	auto findIt = std::find_if(_columns.begin(), _columns.end(),
		[&](const shared_ptr<Column>& column) { return column->_name == columnName; });

	if (findIt != _columns.end())
		return *findIt;

	return nullptr;
}

/*---------------------------------------------------------------------------------------------
이름     : Procedure::GenerateCreateQuery
용도     : Procedure의 쿼리를 만드는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
GWString Procedure::GenerateCreateQuery()
{
	const WCHAR* query = L"CREATE PROCEDURE [dbo].[%s] %s AS BEGIN %s END";

	GWString paramString = GenerateParamString();
	return DBModel::Helpers::Format(query, _name.c_str(), paramString.c_str(), _body.c_str());
}

/*---------------------------------------------------------------------------------------------
이름     : Procedure::GenerateAlterQuery
용도     : Procedure의 ALTER 쿼리를 만드는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
GWString Procedure::GenerateAlterQuery()
{
	const WCHAR* query = L"ALTER PROCEDURE [dbo].[%s] %s AS	BEGIN %s END";

	GWString paramString = GenerateParamString();
	return DBModel::Helpers::Format(query, _name.c_str(), paramString.c_str(), _body.c_str());
}

/*---------------------------------------------------------------------------------------------
이름     : Procedure::GenerateParamString
용도     : MS SQL의 Procedure을 C++ 타입으로 만든 객체
		   DB라고 주석되어 있는건 DB에서만 사용하는 변수
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
GWString Procedure::GenerateParamString()
{
	GWString str;

	const int32 size = static_cast<int32>(_parameters.size());
	for (int32 i = 0; i < size; i++)
	{
		if (i < size - 1)
			str += DBModel::Helpers::Format(L"\t%s %s,\n", _parameters[i]._name.c_str(), _parameters[i]._type.c_str());
		else
			str += DBModel::Helpers::Format(L"\t%s %s", _parameters[i]._name.c_str(), _parameters[i]._type.c_str());
	}

	return str;
}

/*---------------------------------------------------------------------------------------------
이름     : Helpers::Format
용도     : C++ 20 전에 생성된 Format함수
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
GWString Helpers::Format(const WCHAR* format, ...)
{
	WCHAR buf[4096];

	va_list ap;
	va_start(ap, format);
	::vswprintf_s(buf, 4096, format, ap);
	va_end(ap);

	return GWString(buf);
}

/*---------------------------------------------------------------------------------------------
이름     : Helpers::DataTypeToString
용도     : DataType을 GWString으로 변환하는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
GWString Helpers::DataTypeToString(SQLDataType type)
{
	switch (type)
	{
	case SQLDataType::TinyInt:		return L"TinyInt";
	case SQLDataType::SmallInt:	return L"SmallInt";
	case SQLDataType::Int:			return L"Int";
	case SQLDataType::Real:		return L"Real";
	case SQLDataType::DateTime:	return L"DateTime";
	case SQLDataType::Float:		return L"Float";
	case SQLDataType::Bit:			return L"Bit";
	case SQLDataType::Numeric:		return L"Numeric";
	case SQLDataType::BigInt:		return L"BigInt";
	case SQLDataType::VarBinary:	return L"VarBinary";
	case SQLDataType::Varchar:		return L"Varchar";
	case SQLDataType::Binary:		return L"Binary";
	case SQLDataType::NVarChar:	return L"NVarChar";
	default:					return L"None";
	}
}

/*---------------------------------------------------------------------------------------------
이름     : Helpers::RemoveWhiteSpace
용도     : GWstring의 공백을 전부 제거해주는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
GWString Helpers::RemoveWhiteSpace(const GWString& str)
{
	GWString ret = str;

	ret.erase(
		std::remove_if(ret.begin(), ret.end(), [=](WCHAR ch) { return ::isspace(ch); }),
		ret.end());

	return ret;
}

/*---------------------------------------------------------------------------------------------
이름     : Helpers::StringToDataType
용도     : GWstring을 SQLDataType으로 변환해주는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
SQLDataType Helpers::StringToDataType(const WCHAR* str, OUT int32& maxLen)
{
	// a부터z까지의 문자가 1개 이상이고 max가 있거나 숫자가 1개 이상 그대상 들은 1개 또는 없어야한다
	std::wregex reg(L"([a-z]+)(\\((max|\\d+)\\))?");
	std::wcmatch ret;

	if (std::regex_match(str, OUT ret, reg) == false)
		return SQLDataType::None;

	if (ret[3].matched)
		maxLen = ::_wcsicmp(ret[3].str().c_str(), L"max") == 0 ? -1 : _wtoi(ret[3].str().c_str());
	else
		maxLen = 0;

	if (::_wcsicmp(ret[1].str().c_str(), L"TinyInt") == 0) return SQLDataType::TinyInt;
	if (::_wcsicmp(ret[1].str().c_str(), L"SmallInt") == 0) return SQLDataType::SmallInt;
	if (::_wcsicmp(ret[1].str().c_str(), L"Int") == 0) return SQLDataType::Int;
	if (::_wcsicmp(ret[1].str().c_str(), L"Real") == 0) return SQLDataType::Real;
	if (::_wcsicmp(ret[1].str().c_str(), L"DateTime") == 0) return SQLDataType::DateTime;
	if (::_wcsicmp(ret[1].str().c_str(), L"Float") == 0) return SQLDataType::Float;
	if (::_wcsicmp(ret[1].str().c_str(), L"Bit") == 0) return SQLDataType::Bit;
	if (::_wcsicmp(ret[1].str().c_str(), L"Numeric") == 0) return SQLDataType::Numeric;
	if (::_wcsicmp(ret[1].str().c_str(), L"BigInt") == 0) return SQLDataType::BigInt;
	if (::_wcsicmp(ret[1].str().c_str(), L"VarBinary") == 0) return SQLDataType::VarBinary;
	if (::_wcsicmp(ret[1].str().c_str(), L"Varchar") == 0) return SQLDataType::Varchar;
	if (::_wcsicmp(ret[1].str().c_str(), L"Binary") == 0) return SQLDataType::Binary;
	if (::_wcsicmp(ret[1].str().c_str(), L"NVarChar") == 0) return SQLDataType::NVarChar;

	return SQLDataType::None;
}
