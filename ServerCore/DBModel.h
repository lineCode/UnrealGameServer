#pragma once

NAMESPACE_BEGIN(DBModel)

/*---------------------------------------------------------------------------------------------
이름     : SQLDataType
용도     : MS SQL 기준의 오브젝트 번호
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
enum class SQLDataType
{
	None = 0,
	TinyInt = 48,
	SmallInt = 52,
	Int = 56,
	Real = 59,
	DateTime = 61,
	Float = 62,
	Bit = 104,
	Numeric = 108,
	BigInt = 127,
	VarBinary = 165,
	Varchar = 167,
	Binary = 173,
	NVarChar = 231,
};

/*---------------------------------------------------------------------------------------------
이름     : Column
용도     : MS SQL의 Column을 C++ 타입으로 만든 객체
		   DB라고 주석되어 있는건 DB에서만 사용하는 변수
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
class Column
{
public:
	GWString			CreateText();

public:
	GWString				_name;
	int32					_columnId = 0; // DB
	SQLDataType				_type = SQLDataType::None;
	GWString				_typeText;
	int32					_maxLength = 0;
	bool					_nullable = false;
	bool					_identity = false;
	int64					_seedValue = 0;
	int64					_incrementValue = 0;
	GWString				_default;
	GWString				_defaultConstraintName; // DB
};

/*---------------------------------------------------------------------------------------------
이름     : IndexType
용도     : Index의 Cluster NonCluster을 구분하기 위한 Enum 
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
enum class IndexType
{
	Clustered = 1,
	NonClustered = 2
};

/*---------------------------------------------------------------------------------------------
이름     : Index
용도     : MS SQL의 Index을 C++ 타입으로 만든 객체
		   DB라고 주석되어 있는건 DB에서만 사용하는 변수
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
class Index
{
public:
	GWString				GetUniqueName();
	GWString				CreateName(const GWString& tableName);
	GWString				GetTypeText();
	GWString				GetKeyText();
	GWString				CreateColumnsText();
	bool				DependsOn(const GWString& columnName);

public:
	GWString				_name; // DB
	int32				_indexId = 0; // DB
	IndexType			_type = IndexType::NonClustered;
	bool				_primaryKey = false;
	bool				_uniqueConstraint = false;
	Gvector<shared_ptr<Column>>	_columns;
};

/*---------------------------------------------------------------------------------------------
이름     : Table
용도     : MS SQL의 Table을 C++ 타입으로 만든 객체
		   DB라고 주석되어 있는건 DB에서만 사용하는 변수
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
class Table
{
public:
	shared_ptr<Column>			FindColumn(const GWString& columnName);

public:
	int32				_objectId = 0; // DB
	GWString				_name;
	Gvector<shared_ptr<Column>>	_columns;
	Gvector<shared_ptr<Index>>	_indexes;
};

/*---------------------------------------------------------------------------------------------
이름     : Param
용도     : Procedure의 이름과 타입을 저장하는 구조체

수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
struct Param
{
	GWString				_name;
	GWString				_type;
};

/*---------------------------------------------------------------------------------------------
이름     : Procedure
용도     : MS SQL의 Procedure을 C++ 타입으로 만든 객체
		   DB라고 주석되어 있는건 DB에서만 사용하는 변수
		   XML라고 주석되어 있는건 XML에서만 사용하는 변수
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
class Procedure
{
public:
	GWString				GenerateCreateQuery();
	GWString				GenerateAlterQuery();
	GWString				GenerateParamString();

public:
	GWString				_name;
	GWString				_fullBody; // DB
	GWString				_body; // XML
	Gvector<Param>		_parameters;  // XML
};

/*---------------------------------------------------------------------------------------------
이름     : Helpers
용도     : DBMode을 편리하게 돕는 객체
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
class Helpers
{
public:
	static GWString		Format(const WCHAR* format, ...);
	static GWString		DataTypeToString(SQLDataType type);
	static GWString		RemoveWhiteSpace(const GWString& str);
	static SQLDataType		StringToDataType(const WCHAR* str, OUT int32& maxLen);
};

NAMESPACE_END