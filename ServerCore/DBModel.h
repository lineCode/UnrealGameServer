#pragma once

NAMESPACE_BEGIN(DBModel)

/*---------------------------------------------------------------------------------------------
�̸�     : SQLDataType
�뵵     : MS SQL ������ ������Ʈ ��ȣ
������   : �̹α�
������¥ : 2022.10.08
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
�̸�     : Column
�뵵     : MS SQL�� Column�� C++ Ÿ������ ���� ��ü
		   DB��� �ּ��Ǿ� �ִ°� DB������ ����ϴ� ����
������   : �̹α�
������¥ : 2022.10.08
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
�̸�     : IndexType
�뵵     : Index�� Cluster NonCluster�� �����ϱ� ���� Enum 
������   : �̹α�
������¥ : 2022.10.08
----------------------------------------------------------------------------------------------*/
enum class IndexType
{
	Clustered = 1,
	NonClustered = 2
};

/*---------------------------------------------------------------------------------------------
�̸�     : Index
�뵵     : MS SQL�� Index�� C++ Ÿ������ ���� ��ü
		   DB��� �ּ��Ǿ� �ִ°� DB������ ����ϴ� ����
������   : �̹α�
������¥ : 2022.10.08
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
�̸�     : Table
�뵵     : MS SQL�� Table�� C++ Ÿ������ ���� ��ü
		   DB��� �ּ��Ǿ� �ִ°� DB������ ����ϴ� ����
������   : �̹α�
������¥ : 2022.10.08
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
�̸�     : Param
�뵵     : Procedure�� �̸��� Ÿ���� �����ϴ� ����ü

������   : �̹α�
������¥ : 2022.10.08
----------------------------------------------------------------------------------------------*/
struct Param
{
	GWString				_name;
	GWString				_type;
};

/*---------------------------------------------------------------------------------------------
�̸�     : Procedure
�뵵     : MS SQL�� Procedure�� C++ Ÿ������ ���� ��ü
		   DB��� �ּ��Ǿ� �ִ°� DB������ ����ϴ� ����
		   XML��� �ּ��Ǿ� �ִ°� XML������ ����ϴ� ����
������   : �̹α�
������¥ : 2022.10.08
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
�̸�     : Helpers
�뵵     : DBMode�� ���ϰ� ���� ��ü
������   : �̹α�
������¥ : 2022.10.08
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