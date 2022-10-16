#include "pch.h"
#include "XmlDBSynchronizer.h"
#include "DBBind.h"
#include "CoreGlobal.h"
#include "XmlParser.h"
#include <regex>

namespace StoredProcedures
{
	/*---------------------------------------------------------------------------------------------
	�̸�     : QTablesAndColumns
	�뵵     : Table �� Colum�� ������ �������� Query
	������   : �̹α�
	������¥ : 2022.10.08
	----------------------------------------------------------------------------------------------*/
	const WCHAR* QTablesAndColumns =
		L"	SELECT c.object_id, t.name AS tableName, c.name AS columnName, c.column_id, c.user_type_id, c.max_length,"
		"		c.is_nullable, c.is_identity, CAST(ic.seed_value AS BIGINT) AS seedValue, CAST(ic.increment_value AS BIGINT) AS incValue,"
		"		c.default_object_id, dc.definition as defaultDefinition, dc.name as defaultConstraintName"
		"	FROM sys.columns AS c"
		"	JOIN sys.tables AS t"
		"		ON c.object_id = t.object_id"
		"	LEFT JOIN sys.default_constraints AS dc"
		"		ON c.default_object_id = dc.object_id"
		"	LEFT JOIN sys.identity_columns AS ic"
		"		ON c.object_id = ic.object_id AND c.column_id = ic.column_id"
		"	WHERE t.type = 'U'"
		"	ORDER BY object_id ASC, column_id ASC;";

	/*---------------------------------------------------------------------------------------------
	�̸�     : GetDBTables
	�뵵     : DBbind ����� �̿��Ͽ� Table�� ������ ���ϰ� �������� ��ü 
	������   : �̹α�
	������¥ : 2022.10.08
	----------------------------------------------------------------------------------------------*/
	class GetDBTables : public DBBind<0, 13>
	{
	public:
		GetDBTables(DBConnection& conn) : DBBind(conn, QTablesAndColumns) {}

		void Out_ObjectId(OUT int32& value) { BindCol( value); }
		template<int32 N> void Out_TableName(OUT WCHAR(&value)[N]) { BindCol(value); }
		template<int32 N> void Out_ColumnName(OUT WCHAR(&value)[N]) { BindCol(value); }
		void Out_ColumnId(OUT int32& value) { BindCol( value); }
		void Out_UserType(OUT int32& value) { BindCol(value); }
		void Out_MaxLength(OUT int32& value) { BindCol(value); }
		void Out_IsNullable(OUT bool& value) { BindCol(value); }
		void Out_IsIdentity(OUT bool& value) { BindCol(value); }
		void Out_SeedValue(OUT int64& value) { BindCol(value); }
		void Out_IncrementValue(OUT int64& value) { BindCol(value); }
		void Out_DefaultObjectId(OUT int32& value) { BindCol( value); }
		template<int32 N> void Out_DefaultDefinition(OUT WCHAR(&value)[N]) { BindCol( value); }
		template<int32 N> void Out_DefaultConstraintName(OUT WCHAR(&value)[N]) { BindCol( value); }
	};

	/*---------------------------------------------------------------------------------------------
	�̸�     : QIndexes
	�뵵     : Index ������ �������� Query
	������   : �̹α�
	������¥ : 2022.10.08
	----------------------------------------------------------------------------------------------*/
	const WCHAR* QIndexes =
		L"	SELECT i.object_id, i.name as indexName, i.index_id, i.type, i.is_primary_key,"
		"		i.is_unique_constraint, ic.column_id, COL_NAME(ic.object_id, ic.column_id) as columnName"
		"	FROM sys.indexes AS i"
		"	JOIN sys.index_columns AS ic"
		"		ON i.object_id = ic.object_id AND i.index_id = ic.index_id"
		"	WHERE i.type > 0 AND i.object_id IN(SELECT object_id FROM sys.tables WHERE type = 'U')"
		"	ORDER BY i.object_id ASC, i.index_id ASC;";

	/*---------------------------------------------------------------------------------------------
	�̸�     : GetDBIndexes
	�뵵     : DBbind����� �̿��Ͽ� Index�� ������ �������� ��ü
	������   : �̹α�
	������¥ : 2022.10.08
	----------------------------------------------------------------------------------------------*/
	class GetDBIndexes : public DBBind<0, 8>
	{
	public:
		GetDBIndexes(DBConnection& conn) : DBBind(conn, QIndexes) {}

		void Out_ObjectId(OUT int32& value) { BindCol( value); }
		template<int32 N> void Out_IndexName(OUT WCHAR(&value)[N]) { BindCol( value); }
		void Out_IndexId(OUT int32& value) { BindCol( value); }
		void Out_IndexType(OUT int32& value) { BindCol( value); }
		void Out_IsPrimaryKey(OUT bool& value) { BindCol(value); }
		void Out_IsUniqueConstraint(OUT bool& value) { BindCol( value); }
		void Out_ColumnId(OUT int32& value) { BindCol( value); }
		template<int32 N> void Out_ColumnName(OUT WCHAR(&value)[N]) { BindCol( value); }
	};

	/*---------------------------------------------------------------------------------------------
	�̸�     : QStoredProcedures
	�뵵     : Procedure ������ �������� Query
	������   : �̹α�
	������¥ : 2022.10.08
	----------------------------------------------------------------------------------------------*/
	const WCHAR* QStoredProcedures =
		L"	SELECT name, OBJECT_DEFINITION(object_id) AS body FROM sys.procedures;";

	/*---------------------------------------------------------------------------------------------
	�̸�     : GetDBStoredProcedures
	�뵵     : DBbind����� �̿��Ͽ� Procedure�� ������ �������� ��ü
	������   : �̹α�
	������¥ : 2022.10.08
	----------------------------------------------------------------------------------------------*/
	class GetDBStoredProcedures : public DBBind<0, 2>
	{
	public:
		GetDBStoredProcedures(DBConnection& conn) : DBBind(conn, QStoredProcedures) {}

		template<int32 N> void Out_Name(OUT WCHAR(&value)[N]) { BindCol(value); }
		void Out_Body(OUT WCHAR* value, int32 len) { BindCol( value, len); }
	};
}

/*---------------------------------------------------------------------------------------------
�̸�     : XmlDBSynchronizer::Synchronize
�뵵     : XDBSynchronizer���� Parse �� ����ȭ ���ִ� �κ�
������   : �̹α�
������¥ : 2022.10.08
----------------------------------------------------------------------------------------------*/
bool XmlDBSynchronizer::Synchronize(const WCHAR* path)
{
	ParseXmlDB(path);

	GatherDBTables();
	GatherDBIndexes();
	GatherDBStoredProcedures();

	CompareDBModel();
	ExecuteUpdateQueries();

	return true;
}

/*---------------------------------------------------------------------------------------------
�̸�     : XmlDBSynchronizer::ParseXmlDB
�뵵     : XML�� Parse �� XmlDBSynchronizer ��ü�� ��� �ִ� �Լ�
������   : �̹α�
������¥ : 2022.10.08
----------------------------------------------------------------------------------------------*/
void XmlDBSynchronizer::ParseXmlDB(const WCHAR* path)
{
	XmlNode root;
	XmlParser parser;

	// TODO : XML �ֻ�� �κ� �������� �κ�
	CRASH_IF(parser.ParseFromFile(path, OUT root) == false);

	// TODO : ���̺��� �ִ� �κ�
	Gvector<XmlNode> tables = root.FindChildren(L"Table");
	for (XmlNode& table : tables)
	{
		// TODO : ���̺��� Colum�� ���� �� �ִ� �κ�
		shared_ptr<DBModel::Table> t = GMakeShared<DBModel::Table>();
		t->_name = table.GetWStringAttr(L"name");

		Gvector<XmlNode> columns = table.FindChildren(L"Column");
		for (XmlNode& column : columns)
		{
			shared_ptr<DBModel::Column> c = GMakeShared<DBModel::Column>();
			c->_name = column.GetWStringAttr(L"name");
			c->_typeText = column.GetWStringAttr(L"type");
			c->_type = DBModel::Helpers::StringToDataType(c->_typeText.c_str(), OUT c->_maxLength);
			CRASH_IF(c->_type == DBModel::SQLDataType::None);
			c->_nullable = column.GetBoolAttr(L"notnull", false);

			const WCHAR* identityStr = column.GetWStringAttr(L"identity");
			if (::wcslen(identityStr) > 0)
			{
				std::wregex pt(L"(\\d+),(\\d+)");
				std::wcmatch match;
				CRASH_IF(std::regex_match(identityStr, OUT match, pt) == false);
				c->_identity = true;
				c->_seedValue = _wtoi(match[1].str().c_str());
				c->_incrementValue = _wtoi(match[2].str().c_str());
			}

			c->_default = column.GetWStringAttr(L"default");
			t->_columns.push_back(c);
		}

		// TODO : ���̺��� index�� ���� �� �ִ� �κ�
		Gvector<XmlNode> indexes = table.FindChildren(L"Index");
		for (XmlNode& index : indexes)
		{
			// TODO : indexd�� clustered üũ �� �ִ� �κ�
			shared_ptr<DBModel::Index> i = GMakeShared<DBModel::Index>();
			const WCHAR* typeStr = index.GetWStringAttr(L"type");
			if (::_wcsicmp(typeStr, L"clustered") == 0)
				i->_type = DBModel::IndexType::Clustered;
			else if (::_wcsicmp(typeStr, L"nonclustered") == 0)
				i->_type = DBModel::IndexType::NonClustered;
			else
				CRASH("Invalid Index Type");

			// TODO : indexd�� primaryKey ,  uniqueConstraintüũ �� �ִ� �κ�
			i->_primaryKey = index.FindChild(L"PrimaryKey").IsValid();
			i->_uniqueConstraint = index.FindChild(L"UniqueKey").IsValid();

			// TODO : indexd�� colum ���� �κ�
			Gvector<XmlNode> columns = index.FindChildren(L"Column");
			for (XmlNode& column : columns)
			{
				const WCHAR* nameStr = column.GetWStringAttr(L"name");
				shared_ptr<DBModel::Column> c = t->FindColumn(nameStr);
				CRASH_IF(c == nullptr);
				i->_columns.push_back(c);
			}

			t->_indexes.push_back(i);
		}

		_xmlTables.push_back(t);
	}

	// TODO : procedures�� ã�� �κ�
	Gvector<XmlNode> procedures = root.FindChildren(L"Procedure");
	for (XmlNode& procedure : procedures)
	{
		// TODO : procedures�� �̸��� �������� �������� �κ�
		shared_ptr<DBModel::Procedure> p = GMakeShared<DBModel::Procedure>();
		p->_name = procedure.GetWStringAttr(L"name");
		p->_body = procedure.FindChild(L"Body").GetStringValue();

		// TODO : procedures�� Param�� �����ϴ� �κ�
		Gvector<XmlNode> params = procedure.FindChildren(L"Param");
		for (XmlNode& paramNode : params)
		{
			DBModel::Param param;
			param._name = paramNode.GetWStringAttr(L"name");
			param._type = paramNode.GetWStringAttr(L"type");
			p->_parameters.push_back(param);
		}

		_xmlProcedures.push_back(p);
	}

	// TODO : RemovedTable�� �������� �κ�  (�̷��� �ʿ���� �κ��� �����ϱ� ���� �κ�)
	Gvector<XmlNode> removedTables = root.FindChildren(L"RemovedTable");
	for (XmlNode& removedTable : removedTables)
	{
		_xmlRemovedTables.insert(removedTable.GetWStringAttr(L"name"));
	}
}

/*---------------------------------------------------------------------------------------------
�̸�     : XmlDBSynchronizer::GatherDBTables
�뵵     : DataBase�� Query�� ������ Table������ �������� �Լ�
������   : �̹α�
������¥ : 2022.10.08
----------------------------------------------------------------------------------------------*/
bool XmlDBSynchronizer::GatherDBTables()
{
	int32 objectId;
	WCHAR tableName[101] = { 0 };
	WCHAR columnName[101] = { 0 };
	int32 columnId;
	int32 userTypeId;
	int32 maxLength;
	bool isNullable;
	bool isIdentity;
	int64 seedValue;
	int64 incValue;
	int32 defaultObjectId;
	WCHAR defaultDefinition[101] = { 0 };
	WCHAR defaultConstraintName[101] = { 0 };

	// TODO : DataGBase�� ���̺� ������ �������� ���� ������ DBBind�� �����ϴ� �κ�
	StoredProcedures::GetDBTables getDBTables(_dbConn);
	getDBTables.Out_ObjectId(OUT objectId);
	getDBTables.Out_TableName(OUT tableName);
	getDBTables.Out_ColumnName(OUT columnName);
	getDBTables.Out_ColumnId(OUT columnId);
	getDBTables.Out_UserType(OUT userTypeId);
	getDBTables.Out_MaxLength(OUT maxLength);
	getDBTables.Out_IsNullable(OUT isNullable);
	getDBTables.Out_IsIdentity(OUT isIdentity);
	getDBTables.Out_SeedValue(OUT seedValue);
	getDBTables.Out_IncrementValue(OUT incValue);
	getDBTables.Out_DefaultObjectId(OUT defaultObjectId);
	getDBTables.Out_DefaultDefinition(OUT defaultDefinition);
	getDBTables.Out_DefaultConstraintName(OUT defaultConstraintName);

	if (getDBTables.Execute() == false)
		return false;

	// TODO : DataBase�� ���̺��� Colum ������ ���� �� �������� �κ�
	while (getDBTables.Fetch())
	{
		shared_ptr<DBModel::Table> table;

		// TODO : ���̺��� ObjectId�� ã�� �κ�
		auto findTable = std::find_if(_dbTables.begin(), _dbTables.end(), [=](const shared_ptr<DBModel::Table>& table) { return table->_objectId == objectId; });
		if (findTable == _dbTables.end())
		{
			// TODO : ObjectId�� ���� ��� table ���� �� _dbTable�� �߰�
			table = GMakeShared<DBModel::Table>();
			table->_objectId = objectId;
			table->_name = tableName;
			_dbTables.push_back(table);
		}
		else
		{
			// TODO : ObjectId�� ���� ��� table�� �־���
			table = *findTable;
		}

		// TODO : ���̺��� Colum�� �����ϰ� �����ϴ� �κ�
		shared_ptr<DBModel::Column> column = GMakeShared<DBModel::Column>();
		{
			column->_name = columnName;
			column->_columnId = columnId;
			column->_type = static_cast<DBModel::SQLDataType>(userTypeId);
			column->_typeText = DBModel::Helpers::DataTypeToString(column->_type);
			column->_maxLength = (column->_type == DBModel::SQLDataType::NVarChar ? maxLength / 2 : maxLength);
			column->_nullable = isNullable;
			column->_identity = isIdentity;
			column->_seedValue = (isIdentity ? seedValue : 0);
			column->_incrementValue = (isIdentity ? incValue : 0);

			if (defaultObjectId > 0)
			{
				column->_default = defaultDefinition;
				uint64 p = column->_default.find_first_not_of('(');
				column->_default = column->_default.substr(p, column->_default.size() - p * 2);
				column->_defaultConstraintName = defaultConstraintName;
			}
		}

		table->_columns.push_back(column);
	}

	return true;
}

/*---------------------------------------------------------------------------------------------
�̸�     : XmlDBSynchronizer::GatherDBIndexes
�뵵     : DataBase�� Query�� ������ Index������ �������� �Լ�
������   : �̹α�
������¥ : 2022.10.08
----------------------------------------------------------------------------------------------*/
bool XmlDBSynchronizer::GatherDBIndexes()
{
	int32 objectId;
	WCHAR indexName[101] = { 0 };
	int32 indexId;
	int32 indexType;
	bool isPrimaryKey;
	bool isUniqueConstraint;
	int32 columnId;
	WCHAR columnName[101] = { 0 };

	// TODO : DataBase�� Index ������ �������� ���� ������ DBBind�� �����ϴ� �κ�
	StoredProcedures::GetDBIndexes getDBIndexes(_dbConn);
	getDBIndexes.Out_ObjectId(OUT objectId);
	getDBIndexes.Out_IndexName(OUT indexName);
	getDBIndexes.Out_IndexId(OUT indexId);
	getDBIndexes.Out_IndexType(OUT indexType);
	getDBIndexes.Out_IsPrimaryKey(OUT isPrimaryKey);
	getDBIndexes.Out_IsUniqueConstraint(OUT isUniqueConstraint);
	getDBIndexes.Out_ColumnId(OUT columnId);
	getDBIndexes.Out_ColumnName(OUT columnName);

	if (getDBIndexes.Execute() == false)
		return false;

	// TODO : DataBase�� Index ������ ���� �� �������� �κ�
	while (getDBIndexes.Fetch())
	{
		// TODO : ObjectId�� �´� ���̺��� ã�� �κ�
		auto findTable = std::find_if(_dbTables.begin(), _dbTables.end(), [=](const shared_ptr<DBModel::Table>& table) { return table->_objectId == objectId; });
		CRASH_IF(findTable == _dbTables.end());

		// TODO : ���̺��� index�� �������� �κ�
		Gvector<shared_ptr<DBModel::Index>>& indexes = (*findTable)->_indexes;

		// TODO : ���̺��� index_id�� ã�� �κ�
		auto findIndex = std::find_if(indexes.begin(), indexes.end(), [indexId](shared_ptr<DBModel::Index>& index) { return index->_indexId == indexId; });
		if (findIndex == indexes.end())
		{
			// TODO : ���̺��� index_id�� ���� ��� �ε��� ���� �� ����
			shared_ptr<DBModel::Index> index = GMakeShared<DBModel::Index>();
			{
				index->_name = indexName;
				index->_indexId = indexId;
				index->_type = static_cast<DBModel::IndexType>(indexType);
				index->_primaryKey = isPrimaryKey;
				index->_uniqueConstraint = isUniqueConstraint;
			}
			indexes.push_back(index);
			findIndex = indexes.end() - 1;
		}


		// TODO : �ε����� �ɸ� column ã�Ƽ� �������ش�
		Gvector<shared_ptr<DBModel::Column>>& columns = (*findTable)->_columns;
		auto findColumn = std::find_if(columns.begin(), columns.end(), [columnId](shared_ptr<DBModel::Column>& column) { return column->_columnId == columnId; });
		CRASH_IF(findColumn == columns.end());
		(*findIndex)->_columns.push_back(*findColumn);
	}

	return true;
}

/*---------------------------------------------------------------------------------------------
�̸�     : XmlDBSynchronizer::GatherDBStoredProcedures
�뵵     : DataBase�� Query�� ������ Procedures������ �������� �Լ�
������   : �̹α�
������¥ : 2022.10.08
----------------------------------------------------------------------------------------------*/
bool XmlDBSynchronizer::GatherDBStoredProcedures()
{
	WCHAR name[101] = { 0 };
	Gvector<WCHAR> body(PROCEDURE_MAX_LEN);

	// TODO : DataBase�� Procedures ������ �������� ���� ������ DBBind�� �����ϴ� �κ�
	StoredProcedures::GetDBStoredProcedures getDBStoredProcedures(_dbConn);
	getDBStoredProcedures.Out_Name(OUT name);
	getDBStoredProcedures.Out_Body(OUT &body[0], PROCEDURE_MAX_LEN);

	if (getDBStoredProcedures.Execute() == false)
		return false;

	// TODO : DataBase�� Procedure ������ ���� �� �������� �κ�
	while (getDBStoredProcedures.Fetch())
	{
		shared_ptr<DBModel::Procedure> proc = GMakeShared<DBModel::Procedure>();
		{
			proc->_name = name;
			proc->_fullBody = GWString(body.begin() , std::find(body.begin() ,body.end() ,0) );
		}
		_dbProcedures.push_back(proc);
	}

	return true;
}

/*---------------------------------------------------------------------------------------------
�̸�     : XmlDBSynchronizer::CompareDBModel
�뵵     : DBSynchronizer�� XML ������ DataBase�� ������ ��ġ�ϴ��� Ȯ���ϴ� �� �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.10.08
----------------------------------------------------------------------------------------------*/
void XmlDBSynchronizer::CompareDBModel()
{
	// TODO : ������Ʈ�� �ʿ��� ������ ��� �ʱ�ȭ
	_dependentIndexes.clear();
	for (Gvector<GWString>& queries : _updateQueries)
		queries.clear();

	// TODO : XML�� �ִ� ���̺� ����� �켱 ���� �´�.
	Gmap<GWString, shared_ptr<DBModel::Table>> xmlTableMap;
	for (shared_ptr<DBModel::Table>& xmlTable : _xmlTables)
		xmlTableMap[xmlTable->_name] = xmlTable;

	// TODO : DB�� �����ϴ� ���̺���� ���鼭 XML�� ���ǵ� ���̺��� �̸��� ���Ѵ�.
	for (shared_ptr<DBModel::Table>& dbTable : _dbTables)
	{
		auto findTable = xmlTableMap.find(dbTable->_name);
		// TODO : XML�̶� DB ��ο� ������ ��� ���̺� ������ Ȯ��
		if (findTable != xmlTableMap.end())
		{
			shared_ptr<DBModel::Table> xmlTable = findTable->second;
			CompareTables(dbTable, xmlTable);
			xmlTableMap.erase(findTable);
		}
		else
		{
			// TODO : ���ŵ� ���̺� ����Ǿ� �ִ��� Ȯ��
			if (_xmlRemovedTables.find(dbTable->_name) != _xmlRemovedTables.end())
			{
				// TODO : DB���� ���������� XML���� ���� ���� ���̺� ����
				GConsoleLogger->WriteStdOut(Color::YELLOW, L"Removing Table : [dbo].[%s]\n", dbTable->_name.c_str());
				_updateQueries[UpdateStep::DropTable].push_back(DBModel::Helpers::Format(L"DROP TABLE [dbo].[%s]", dbTable->_name.c_str()));
			}
		}
	}

	//TODO : XML���� ���������� DB���� ���� ���̺�
	for (auto& mapIt : xmlTableMap)
	{
		shared_ptr<DBModel::Table>& xmlTable = mapIt.second;

		//TODO : XML�� ���̺��� �÷��� �߰��ϴ� �κ�
		GWString columnsStr;
		const int32 size = static_cast<int32>(xmlTable->_columns.size());
		for (int32 i = 0; i < size; i++)
		{
			if (i != 0)
				columnsStr += L",";
			columnsStr += L"\n\t";
			columnsStr += xmlTable->_columns[i]->CreateText();
		}

		//TODO : XML�� ���̺��� DB�� ����
		GConsoleLogger->WriteStdOut(Color::YELLOW, L"Creating Table : [dbo].[%s]\n", xmlTable->_name.c_str());
		_updateQueries[UpdateStep::CreateTable].push_back(DBModel::Helpers::Format(L"CREATE TABLE [dbo].[%s] (%s)", xmlTable->_name.c_str(), columnsStr.c_str()));

		//TODO : DB�� ������ XML ���̺� �÷��� ����
		for (shared_ptr<DBModel::Column>& xmlColumn : xmlTable->_columns)
		{
			if (xmlColumn->_default.empty())
				continue;

			_updateQueries[UpdateStep::DefaultConstraint].push_back(DBModel::Helpers::Format(L"ALTER TABLE [dbo].[%s] ADD CONSTRAINT [%s] DEFAULT (%s) FOR [%s]",
				xmlTable->_name.c_str(),
				DBModel::Helpers::Format(L"DF_%s_%s", xmlTable->_name.c_str(), xmlColumn->_name.c_str()).c_str(),
				xmlColumn->_default.c_str(),
				xmlColumn->_name.c_str()));
		}

		//TODO : DB�� ������ XML ���̺� �ε����� ����
		for (shared_ptr<DBModel::Index>& xmlIndex : xmlTable->_indexes)
		{
			GConsoleLogger->WriteStdOut(Color::YELLOW, L"Creating Index : [%s] %s %s [%s]\n", xmlTable->_name.c_str(), xmlIndex->GetKeyText().c_str(), xmlIndex->GetClusteredText().c_str(), xmlIndex->GetConstraintName().c_str());
			if (xmlIndex->_primaryKey || xmlIndex->_uniqueConstraint)
			{
				_updateQueries[UpdateStep::CreateIndex].push_back(DBModel::Helpers::Format(
					L"ALTER TABLE [dbo].[%s] ADD CONSTRAINT [%s] %s %s (%s)",
					xmlTable->_name.c_str(),
					xmlIndex->CreateName(xmlTable->_name).c_str(),
					xmlIndex->GetKeyText().c_str(),
					xmlIndex->GetClusteredText().c_str(),
					xmlIndex->CreateColumnsText().c_str()));
			}
			else
			{
				_updateQueries[UpdateStep::CreateIndex].push_back(DBModel::Helpers::Format(
					L"CREATE %s INDEX [%s] ON [dbo].[%s] (%s)",
					xmlIndex->GetClusteredText().c_str(),
					xmlIndex->CreateName(xmlTable->_name).c_str(),
					xmlTable->_name.c_str(),
					xmlIndex->CreateColumnsText().c_str()));
			}
		}
	}

	CompareStoredProcedures();
}

/*---------------------------------------------------------------------------------------------
�̸�     : XmlDBSynchronizer::ExecuteUpdateQueries
�뵵     : XML�� DB�� �ٸ� �������� �����ϴ� ������ ���� _updateQueries��
           QueryStep ������ �°� DB�� ������ ������ �κ�
������   : �̹α�
������¥ : 2022.10.08
----------------------------------------------------------------------------------------------*/
void XmlDBSynchronizer::ExecuteUpdateQueries()
{
	for (int32 step = 0; step < UpdateStep::Max; step++)
	{
		for (GWString& query : _updateQueries[step])
		{
			_dbConn.Unbind();
			CRASH_IF(_dbConn.Execute(query.c_str()) == false);
		}
	}
}

/*---------------------------------------------------------------------------------------------
�̸�     : XmlDBSynchronizer::CompareTables
�뵵     : XML������ DataBase�� Table�� ������ �´��� Ȯ��
������   : �̹α�
������¥ : 2022.10.08
----------------------------------------------------------------------------------------------*/
void XmlDBSynchronizer::CompareTables(shared_ptr<DBModel::Table> dbTable, shared_ptr<DBModel::Table> xmlTable)
{
	// TODO : XML�� �ִ� �÷� ����� ���� �´�.
	Gmap<GWString, shared_ptr<DBModel::Column>> xmlColumnMap;
	for (shared_ptr<DBModel::Column>& xmlColumn : xmlTable->_columns)
		xmlColumnMap[xmlColumn->_name] = xmlColumn;

	// TODO : DB�� �����ϴ� ���̺� �÷����� ���鼭 XML�� ���ǵ� �÷���� �̸��� ���Ѵ�.
	for (shared_ptr<DBModel::Column>& dbColumn : dbTable->_columns)
	{
		auto findColumn = xmlColumnMap.find(dbColumn->_name);
		// TODO : XML�̶� DB ��ο� �̸��� ������ ��� ���̺� �÷� ������ Ȯ��
		if (findColumn != xmlColumnMap.end())
		{
			shared_ptr<DBModel::Column>& xmlColumn = findColumn->second;
			CompareColumns(dbTable, dbColumn, xmlColumn);
			xmlColumnMap.erase(findColumn);
		}
		// TODO : DB���� �����ϴµ� XML���� ���� ���
		else
		{

			// TODO : Constrain�� ���� �� CONSTRAIN�� �����Ѵ�
			GConsoleLogger->WriteStdOut(Color::YELLOW, L"Dropping Column : [%s].[%s]\n", dbTable->_name.c_str(), dbColumn->_name.c_str());
			if (dbColumn->_defaultConstraintName.empty() == false)
				_updateQueries[UpdateStep::DropColumn].push_back(DBModel::Helpers::Format(L"ALTER TABLE [dbo].[%s] DROP CONSTRAINT [%s]", dbTable->_name.c_str(), dbColumn->_defaultConstraintName.c_str()));

			// TODO : �ش� Į���� �����Ѵ�
			_updateQueries[UpdateStep::DropColumn].push_back(DBModel::Helpers::Format(L"ALTER TABLE [dbo].[%s] DROP COLUMN [%s]", dbTable->_name.c_str(), dbColumn->_name.c_str()));
		}
	}

	// TODO : XML���� �ִµ� DB���� ���� ��� 
	for (auto& mapIt : xmlColumnMap)
	{
		shared_ptr<DBModel::Column>& xmlColumn = mapIt.second;
		DBModel::Column newColumn = *xmlColumn;
		newColumn._nullable = true;

		// TODO : xml Į���� �߰��Ѵ�
		GConsoleLogger->WriteStdOut(Color::YELLOW, L"Adding Column : [%s].[%s]\n", dbTable->_name.c_str(), xmlColumn->_name.c_str());
		_updateQueries[UpdateStep::AddColumn].push_back(DBModel::Helpers::Format(L"ALTER TABLE [dbo].[%s] ADD %s %s",
			dbTable->_name.c_str(), xmlColumn->_name.c_str(), xmlColumn->_typeText.c_str()));

		// TODO : xml������ �°� �߰��� Į���� ������Ʈ �Ѵ�
		if (xmlColumn->_nullable == false && xmlColumn->_default.empty() == false)
		{
			_updateQueries[UpdateStep::AddColumn].push_back(DBModel::Helpers::Format(L"SET NOCOUNT ON; UPDATE [dbo].[%s] SET [%s] = %s WHERE [%s] IS NULL",
				dbTable->_name.c_str(), xmlColumn->_name.c_str(), xmlColumn->_default.c_str(), xmlColumn->_name.c_str()));
		}

		if (xmlColumn->_nullable == false)
		{
			_updateQueries[UpdateStep::AddColumn].push_back(DBModel::Helpers::Format(L"ALTER TABLE [dbo].[%s] ALTER COLUMN %s",
				dbTable->_name.c_str(), xmlColumn->CreateText().c_str()));
		}

		if (xmlColumn->_default.empty() == false)
		{
			_updateQueries[UpdateStep::AddColumn].push_back(DBModel::Helpers::Format(L"ALTER TABLE [dbo].[%s] ADD CONSTRAINT [DF_%s_%s] DEFAULT (%s) FOR [%s]",
				dbTable->_name.c_str(), dbTable->_name.c_str(), xmlColumn->_name.c_str(), xmlColumn->_default.c_str(), xmlColumn->_name.c_str()));
		}
	}

	// TODO : XML�� �ִ� �ε��� ����� ���� �´�.
	Gmap<GWString, shared_ptr<DBModel::Index>> xmlIndexMap;
	for (shared_ptr<DBModel::Index>& xmlIndex : xmlTable->_indexes)
		xmlIndexMap[xmlIndex->GetConstraintName()] = xmlIndex;

	// TODO : DB�� �����ϴ� ���̺� �ε������� ���鼭 XML�� ���ǵ� �ε������ ���Ѵ�.
	for (shared_ptr<DBModel::Index>& dbIndex : dbTable->_indexes)
	{
		auto findIndex = xmlIndexMap.find(dbIndex->GetConstraintName());
		// TODO : XML�� DB �ε����� �ְ� ���� �ٸ� �ε����� DB�� ���� ���
		if (findIndex != xmlIndexMap.end() && _dependentIndexes.find(dbIndex->GetConstraintName()) == _dependentIndexes.end())
		{
			shared_ptr<DBModel::Index> xmlIndex = findIndex->second;
			xmlIndexMap.erase(findIndex);
		}
		else
		{
			// TODO : �ε����� ���� �ٸ��ų� XML���� ���� DB�� �ִ� ��� �����Ѵ�
			GConsoleLogger->WriteStdOut(Color::YELLOW, L"Dropping Index : [%s] [%s] %s %s\n", dbTable->_name.c_str(), dbIndex->_name.c_str(), dbIndex->GetKeyText().c_str(), dbIndex->GetClusteredText().c_str());
			if (dbIndex->_primaryKey || dbIndex->_uniqueConstraint)
				_updateQueries[UpdateStep::DropIndex].push_back(DBModel::Helpers::Format(L"ALTER TABLE [dbo].[%s] DROP CONSTRAINT [%s]", dbTable->_name.c_str(), dbIndex->_name.c_str()));
			else
				_updateQueries[UpdateStep::DropIndex].push_back(DBModel::Helpers::Format(L"DROP INDEX [%s] ON [dbo].[%s]", dbIndex->_name.c_str(), dbTable->_name.c_str()));
		}
	}

	// TODO : XML���� �ְ� DB���� ���� ��� �ε��� �߰�
	for (auto& mapIt : xmlIndexMap)
	{
		shared_ptr<DBModel::Index> xmlIndex = mapIt.second;
		GConsoleLogger->WriteStdOut(Color::YELLOW, L"Creating Index : [%s] %s %s [%s]\n", dbTable->_name.c_str(), xmlIndex->GetKeyText().c_str(), xmlIndex->GetClusteredText().c_str(), xmlIndex->GetConstraintName().c_str());
		if (xmlIndex->_primaryKey || xmlIndex->_uniqueConstraint)
		{
			_updateQueries[UpdateStep::CreateIndex].push_back(DBModel::Helpers::Format(L"ALTER TABLE [dbo].[%s] ADD CONSTRAINT [%s] %s %s (%s)",
				dbTable->_name.c_str(), xmlIndex->CreateName(dbTable->_name).c_str(), xmlIndex->GetKeyText().c_str(), xmlIndex->GetClusteredText().c_str(), xmlIndex->CreateColumnsText().c_str()));
		}
		else
		{
			_updateQueries[UpdateStep::CreateIndex].push_back(DBModel::Helpers::Format(L"CREATE %s INDEX [%s] ON [dbo].[%s] (%s)",
				xmlIndex->GetClusteredText(), xmlIndex->CreateName(dbTable->_name).c_str(), dbTable->_name.c_str(), xmlIndex->CreateColumnsText().c_str()));
		}
	}
}

/*---------------------------------------------------------------------------------------------
�̸�     : XmlDBSynchronizer::CompareColumns
�뵵     : XML������ DataBase�� Colum�� ������ �´��� Ȯ��
������   : �̹α�
������¥ : 2022.10.08
----------------------------------------------------------------------------------------------*/
void XmlDBSynchronizer::CompareColumns(shared_ptr<DBModel::Table> dbTable, shared_ptr<DBModel::Column> dbColumn, shared_ptr<DBModel::Column> xmlColumn)
{
	uint8 flag = 0;

	// TODO : DB �� XML�� �÷��� ��� ���ڸ� �� �� �ٸ����� Flag�� ���
	if (dbColumn->_type != xmlColumn->_type)
		flag |= ColumnFlag::Type;
	if (dbColumn->_maxLength != xmlColumn->_maxLength && xmlColumn->_maxLength > 0)
		flag |= ColumnFlag::Length;
	if (dbColumn->_nullable != xmlColumn->_nullable)
		flag |= ColumnFlag::Nullable;
	if (dbColumn->_identity != xmlColumn->_identity || (dbColumn->_identity && dbColumn->_incrementValue != xmlColumn->_incrementValue))
		flag |= ColumnFlag::Identity;
	if (dbColumn->_default != xmlColumn->_default)
		flag |= ColumnFlag::Default;

	// TODO : XML �� DB �÷��� �ٸ� ���� ���� ��� ���
	if (flag)
	{
		GConsoleLogger->WriteStdOut(Color::YELLOW, L"Updating Column [%s] : (%s) -> (%s)\n", dbTable->_name.c_str(), dbColumn->CreateText().c_str(), xmlColumn->CreateText().c_str());
	}

	// TODO : ������ �ε����� ������ ���߿� �����ϱ� ���� ����Ѵ�.
	if (flag & (ColumnFlag::Type | ColumnFlag::Length | ColumnFlag::Nullable))
	{
		for (shared_ptr<DBModel::Index>& dbIndex : dbTable->_indexes)
			if (dbIndex->DependsOn(dbColumn->_name))
				_dependentIndexes.insert(dbIndex->GetConstraintName());

		flag |= ColumnFlag::Default;
	}

	// TODO :DEFALUT ���� �ٸ� ���
	if (flag & ColumnFlag::Default)
	{
		// TODO : DB�� �ش� Default�� ���� ���
		if (dbColumn->_defaultConstraintName.empty() == false)
		{
			// TODO : DB�� Defalut���� ������ �����ϱ� ���� _updateQueries[QueryStep::AlterColumn]�� CONSTRAIN�� DROP�� �߰��Ѵ�
			_updateQueries[UpdateStep::AlterColumn].push_back(DBModel::Helpers::Format(
				L"ALTER TABLE [dbo].[%s] DROP CONSTRAINT [%s]",
				dbTable->_name.c_str(),
				dbColumn->_defaultConstraintName.c_str()));
		}
	}

	DBModel::Column newColumn = *dbColumn;
	newColumn._default = L"";
	newColumn._type = xmlColumn->_type;
	newColumn._maxLength = xmlColumn->_maxLength;
	newColumn._typeText = xmlColumn->_typeText;
	newColumn._seedValue = xmlColumn->_seedValue;
	newColumn._incrementValue = xmlColumn->_incrementValue;

	// TODO :TYPE OR LENGTH OR IDENTITY ���� �ٸ� ���
	if (flag & (ColumnFlag::Type | ColumnFlag::Length | ColumnFlag::Identity))
	{
		// TODO : XML�� TYPE , LENGTH , IDENTITY �������� �÷��� ���� ���� ���� �÷��� �������� �߰��Ѵ�
		_updateQueries[UpdateStep::AlterColumn].push_back(DBModel::Helpers::Format(
			L"ALTER TABLE [dbo].[%s] ALTER COLUMN %s",
			dbTable->_name.c_str(),
			newColumn.CreateText().c_str()));
	}


	newColumn._nullable = xmlColumn->_nullable;

	// TODO :Nullable ���� �ٸ� ���
	if (flag & ColumnFlag::Nullable)
	{
		// TODO : default�� �ִ� ���
		if (xmlColumn->_default.empty() == false)
		{
			_updateQueries[UpdateStep::AlterColumn].push_back(DBModel::Helpers::Format(
				L"SET NOCOUNT ON; UPDATE [dbo].[%s] SET [%s] = %s WHERE [%s] IS NULL",
				dbTable->_name.c_str(),
				xmlColumn->_name.c_str(),
				xmlColumn->_name.c_str(),
				xmlColumn->_name.c_str()));
		}

		// TODO : XML�� Nullable�� �������� ���� �÷� ���濡 �߰� 
		_updateQueries[UpdateStep::AlterColumn].push_back(DBModel::Helpers::Format(
			L"ALTER TABLE [dbo].[%s] ALTER COLUMN %s",
			dbTable->_name.c_str(),
			newColumn.CreateText().c_str()));
	}

	// TODO :Default ���� �ٸ� ���
	if (flag & ColumnFlag::Default)
	{
		// TODO :_defaultConstraintName�� �ִ� ���
		if (dbColumn->_defaultConstraintName.empty() == false)
		{
			// TODO : XML�� Default�� �������� ���� �÷� ���濡 �߰� 
			_updateQueries[UpdateStep::AlterColumn].push_back(DBModel::Helpers::Format(
				L"ALTER TABLE [dbo].[%s] ADD CONSTRAINT [%s] DEFAULT (%s) FOR [%s]",
				dbTable->_name.c_str(),
				DBModel::Helpers::Format(L"DF_%s_%s", dbTable->_name.c_str(), dbColumn->_name.c_str()).c_str(),
				dbColumn->_default.c_str(), dbColumn->_name.c_str()));
		}
	}
}

/*---------------------------------------------------------------------------------------------
�̸�     : XmlDBSynchronizer::CompareStoredProcedures
�뵵     : XML������ DataBase�� Procedures�� ������ �´��� Ȯ��
������   : �̹α�
������¥ : 2022.10.08
----------------------------------------------------------------------------------------------*/
void XmlDBSynchronizer::CompareStoredProcedures()
{
	// TODO : XML�� �ִ� ���ν��� ����� ���� �´�.
	Gmap<GWString, shared_ptr<DBModel::Procedure>> xmlProceduresMap;
	for (shared_ptr<DBModel::Procedure>& xmlProcedure : _xmlProcedures)
		xmlProceduresMap[xmlProcedure->_name] = xmlProcedure;

	//TODO : DB�� �� XML�� ���ν����� �����ϴ��� Ȯ��.
	for (shared_ptr<DBModel::Procedure>& dbProcedure : _dbProcedures)
	{
		auto findProcedure = xmlProceduresMap.find(dbProcedure->_name);
		if (findProcedure != xmlProceduresMap.end())
		{
			shared_ptr<DBModel::Procedure> xmlProcedure = findProcedure->second;
			GWString xmlBody = xmlProcedure->GenerateCreateQuery();
			//TODO : �Ѵ� ���� �� ��� XML �� DB�� Body�� ������ Ȯ�� �� �ٸ� ��� XML �������� ����
			if (DBModel::Helpers::RemoveWhiteSpace(dbProcedure->_fullBody) != DBModel::Helpers::RemoveWhiteSpace(xmlBody))
			{
				GConsoleLogger->WriteStdOut(Color::YELLOW, L"Updating Procedure : %s\n", dbProcedure->_name.c_str());
				_updateQueries[UpdateStep::StoredProcecure].push_back(xmlProcedure->GenerateAlterQuery());
			}
			xmlProceduresMap.erase(findProcedure);
		}
	}

	// TODO : XML���� �����ϴµ� DB���� ���� ��� 
	for (auto& mapIt : xmlProceduresMap)
	{
		//TODO : XML�� ���ν����� �߰�
		GConsoleLogger->WriteStdOut(Color::YELLOW, L"Updating Procedure : %s\n", mapIt.first.c_str());
		_updateQueries[UpdateStep::StoredProcecure].push_back(mapIt.second->GenerateCreateQuery());
	}
}