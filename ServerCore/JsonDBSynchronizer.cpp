#include "pch.h"
#include "JsonDBSynchronizer.h"
#include "DBBind.h"
#include "JsonParser.h"
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

		void Out_ObjectId(OUT int32& value) { BindCol(value); }
		template<int32 N> void Out_TableName(OUT WCHAR(&value)[N]) { BindCol(value); }
		template<int32 N> void Out_ColumnName(OUT WCHAR(&value)[N]) { BindCol(value); }
		void Out_ColumnId(OUT int32& value) { BindCol(value); }
		void Out_UserType(OUT int32& value) { BindCol(value); }
		void Out_MaxLength(OUT int32& value) { BindCol(value); }
		void Out_IsNullable(OUT bool& value) { BindCol(value); }
		void Out_IsIdentity(OUT bool& value) { BindCol(value); }
		void Out_SeedValue(OUT int64& value) { BindCol(value); }
		void Out_IncrementValue(OUT int64& value) { BindCol(value); }
		void Out_DefaultObjectId(OUT int32& value) { BindCol(value); }
		template<int32 N> void Out_DefaultDefinition(OUT WCHAR(&value)[N]) { BindCol(value); }
		template<int32 N> void Out_DefaultConstraintName(OUT WCHAR(&value)[N]) { BindCol(value); }
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

		void Out_ObjectId(OUT int32& value) { BindCol(value); }
		template<int32 N> void Out_IndexName(OUT WCHAR(&value)[N]) { BindCol(value); }
		void Out_IndexId(OUT int32& value) { BindCol(value); }
		void Out_IndexType(OUT int32& value) { BindCol(value); }
		void Out_IsPrimaryKey(OUT bool& value) { BindCol(value); }
		void Out_IsUniqueConstraint(OUT bool& value) { BindCol(value); }
		void Out_ColumnId(OUT int32& value) { BindCol(value); }
		template<int32 N> void Out_ColumnName(OUT WCHAR(&value)[N]) { BindCol(value); }
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
		void Out_Body(OUT WCHAR* value, int32 len) { BindCol(value, len); }
	};
}

bool JsonDBSynchronizer::Synchronize(const WCHAR* path)
{
	ParseJsonDB(path);

	DBTablesSave();
	DBIndexsSave();
	DBProceduresSave();

	CompareDBModel();
	ExecuteUpdateQueries();
	return true;
}

/*---------------------------------------------------------------------------------------------
�̸�     : XmlDBSynchronizer::ParseJsonDB
�뵵     : Json�� Parse �� JsonDBSynchronizer ��ü�� ��� �ִ� �Լ�
������   : �̹α�
������¥ : 2022.10.12
----------------------------------------------------------------------------------------------*/
void JsonDBSynchronizer::ParseJsonDB(const WCHAR* path)
{
	rapidjson::Document document;
	JsonParser jsonparser;

	// TODO : Json�� �о� ��
	jsonparser.ParseJson(path, document);

	// TODO : Json ���̺��� ������
	JsonObject tables(document["Table"]);

	// TODO : Json�� ���̺��� ���������� ������
	for(JsonObject table : tables._object.GetArray())  
	{
		shared_ptr<DBModel::Table> tabletemp = GMakeShared<DBModel::Table>();
		tabletemp->_name = table.GetWString("name");

		// TODO : ���̺��� Column�� ������
		Gvector<JsonObject> columns = table.GetArrayGvector("Column");
		for(JsonObject & column : columns)
		{
			shared_ptr<DBModel::Column> columntemp = GMakeShared<DBModel::Column>();
			columntemp->_name = column.GetWString("name");
			columntemp->_typeText = column.GetWString("type");
			columntemp->_type = DBModel::Helpers::StringToDataType(columntemp->_typeText.c_str(), OUT columntemp->_maxLength);
			CRASH_IF(columntemp->_type == DBModel::SQLDataType::None);
			columntemp->_nullable = column._object["notnull"].GetBool();

			// TODO : identity�� �����Ѵٸ�
			bool identity = column._object.HasMember("identity");
			if (identity)
			{
				const WCHAR* identityStr = column.GetWString("identity");
				std::wregex pt(L"(\\d+),(\\d+)");
				std::wcmatch match;
				CRASH_IF(std::regex_match(identityStr, OUT match, pt) == false);
				columntemp->_identity = true;
				columntemp->_seedValue = _wtoi(match[1].str().c_str());
				columntemp->_incrementValue = _wtoi(match[2].str().c_str());
			}

			columntemp->_default = column.GetWString("default");
			tabletemp->_columns.push_back(columntemp);
		}

		// TODO : ���̺��� Index�� ������
		Gvector<JsonObject> indexes = table.GetArrayGvector("Index");
		for (JsonObject& index : indexes)
		{
			// TODO : Index�� clustered�� �°� ����
			shared_ptr<DBModel::Index> indextemp = GMakeShared<DBModel::Index>();
			const WCHAR* typeStr = index.GetWString("type");
			if (::_wcsicmp(typeStr, L"clustered") == 0)
				indextemp->_type = DBModel::IndexType::Clustered;
			else if (::_wcsicmp(typeStr, L"nonclustered") == 0)
				indextemp->_type = DBModel::IndexType::NonClustered;
			else
				CRASH("Invalid Index Type");

			// TODO : indexd�� primaryKey ,  uniqueConstraintüũ �� �ִ� �κ�
			indextemp->_primaryKey = index._object["PrimaryKey"].GetBool();
			indextemp->_uniqueConstraint = index._object["UniqueKey"].GetBool();

			// TODO : Column�� Index ����
			Gvector<JsonObject> columns = index.GetArrayGvector("Column");
			for (JsonObject& column : columns)
			{
				const WCHAR* nameStr = column.GetWString("name");
				shared_ptr<DBModel::Column> c = tabletemp->FindColumn(nameStr);
				CRASH_IF(c == nullptr);
				indextemp->_columns.push_back(c);
			}

			tabletemp->_indexes.push_back(indextemp);
		}

		_jsonTables.push_back(tabletemp);
	}

	// TODO : procedures �������� �κ�
	JsonObject procedures(document["Procedure"]);
	for (JsonObject procedure : procedures._object.GetArray())
	{
		// TODO : proceudre�� �̸��� ������ ������
		shared_ptr<DBModel::Procedure> proceduretemp = GMakeShared<DBModel::Procedure>();
		proceduretemp->_name = procedure.GetWString("name");
		proceduretemp->_body = procedure.GetWString("Body");

		// TODO : Proceudre�� Param ������ �����ϴ� �κ�
		Gvector<JsonObject> params = procedure.GetArrayGvector("Param");
		for (JsonObject& paramNode : params)
		{
			DBModel::Param param;
			param._name = paramNode.GetWString("name");
			param._type = paramNode.GetWString("type");
			proceduretemp->_parameters.push_back(param);
		}

		_jsonProcedures.push_back(proceduretemp);
	}

	// TODO : RemovedTable�� �������� �κ�
	if (document.HasMember("RemovedTable") == true)
	{
		// TODO : ������ ���̺��� �̸��� ������ �����ϴ� �κ�
		JsonObject removetables(document["RemovedTable"]);
		for (JsonObject removedTable : removetables._object.GetArray())
		{
			_jsonRemovedTables.insert(removedTable.GetWString("name"));
		}
	}
}

/*---------------------------------------------------------------------------------------------
�̸�     : JsonDBSynchronizer::DBTablesSave
�뵵     : DataBase�� Query�� ������ Table������ �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.10.12
----------------------------------------------------------------------------------------------*/
bool JsonDBSynchronizer::DBTablesSave()
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
�̸�     : JsonDBSynchronizer::DBIndexsSave
�뵵     : DataBase�� Query�� ������ Index������ �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.10.12
----------------------------------------------------------------------------------------------*/
bool JsonDBSynchronizer::DBIndexsSave()
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
�̸�     : JsonDBSynchronizer::DBProceduresSave
�뵵     : DataBase�� Query�� ������ Procedure������ �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.10.12
----------------------------------------------------------------------------------------------*/
bool JsonDBSynchronizer::DBProceduresSave()
{
	WCHAR name[101] = { 0 };
	Gvector<WCHAR> body(PROCEDURE_MAX_LEN);

	// TODO : DataBase�� Procedures ������ �������� ���� ������ DBBind�� �����ϴ� �κ�
	StoredProcedures::GetDBStoredProcedures getDBStoredProcedures(_dbConn);
	getDBStoredProcedures.Out_Name(OUT name);
	getDBStoredProcedures.Out_Body(OUT & body[0], PROCEDURE_MAX_LEN);

	if (getDBStoredProcedures.Execute() == false)
		return false;

	// TODO : DataBase�� Procedure ������ ���� �� �������� �κ�
	while (getDBStoredProcedures.Fetch())
	{
		shared_ptr<DBModel::Procedure> proc = GMakeShared<DBModel::Procedure>();
		{
			proc->_name = name;
			proc->_fullBody = GWString(body.begin(), std::find(body.begin(), body.end(), 0));
		}
		_dbProcedures.push_back(proc);
	}

	return true;
}

/*---------------------------------------------------------------------------------------------
�̸�     : JsonDBSynchronizer::CompareDBModel
�뵵     : Json ������ DataBase�� ������ ��ġ�ϴ��� Ȯ���ϴ� �� �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.10.12
----------------------------------------------------------------------------------------------*/
void JsonDBSynchronizer::CompareDBModel()
{
	// TODO : ������Ʈ�� �ʿ��� ������ ��� �ʱ�ȭ
	_dependentIndexes.clear();
	for (Gvector<GWString>& queries : _updateQueries)
		queries.clear();

	// TODO : XML�� �ִ� ���̺� ����� �켱 ���� �´�.
	Gmap<GWString, shared_ptr<DBModel::Table>> xmlTableMap;
	for (shared_ptr<DBModel::Table>& xmlTable : _jsonTables)
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
			if (_jsonRemovedTables.find(dbTable->_name) != _jsonRemovedTables.end())
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
			GConsoleLogger->WriteStdOut(Color::YELLOW, L"Creating Index : [%s] %s %s [%s]\n", xmlTable->_name.c_str(), xmlIndex->GetKeyText().c_str(), xmlIndex->GetTypeText().c_str(), xmlIndex->GetUniqueName().c_str());
			if (xmlIndex->_primaryKey || xmlIndex->_uniqueConstraint)
			{
				_updateQueries[UpdateStep::CreateIndex].push_back(DBModel::Helpers::Format(
					L"ALTER TABLE [dbo].[%s] ADD CONSTRAINT [%s] %s %s (%s)",
					xmlTable->_name.c_str(),
					xmlIndex->CreateName(xmlTable->_name).c_str(),
					xmlIndex->GetKeyText().c_str(),
					xmlIndex->GetTypeText().c_str(),
					xmlIndex->CreateColumnsText().c_str()));
			}
			else
			{
				_updateQueries[UpdateStep::CreateIndex].push_back(DBModel::Helpers::Format(
					L"CREATE %s INDEX [%s] ON [dbo].[%s] (%s)",
					xmlIndex->GetTypeText().c_str(),
					xmlIndex->CreateName(xmlTable->_name).c_str(),
					xmlTable->_name.c_str(),
					xmlIndex->CreateColumnsText().c_str()));
			}
		}
	}

	CompareStoredProcedures();
}

/*---------------------------------------------------------------------------------------------
�̸�     : JsonDBSynchronizer::CompareDBModel
�뵵     : Json������ DataBase�� Table�� ������ �´��� Ȯ��
������   : �̹α�
������¥ : 2022.10.12
----------------------------------------------------------------------------------------------*/
void JsonDBSynchronizer::CompareTables(shared_ptr<DBModel::Table> dbTable, shared_ptr<DBModel::Table> xmlTable)
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
		xmlIndexMap[xmlIndex->GetUniqueName()] = xmlIndex;

	// TODO : DB�� �����ϴ� ���̺� �ε������� ���鼭 XML�� ���ǵ� �ε������ ���Ѵ�.
	for (shared_ptr<DBModel::Index>& dbIndex : dbTable->_indexes)
	{
		auto findIndex = xmlIndexMap.find(dbIndex->GetUniqueName());
		// TODO : XML�� DB �ε����� �ְ� ���� �ٸ� �ε����� DB�� ���� ���
		if (findIndex != xmlIndexMap.end() && _dependentIndexes.find(dbIndex->GetUniqueName()) == _dependentIndexes.end())
		{
			shared_ptr<DBModel::Index> xmlIndex = findIndex->second;
			xmlIndexMap.erase(findIndex);
		}
		else
		{
			// TODO : �ε����� ���� �ٸ��ų� XML���� ���� DB�� �ִ� ��� �����Ѵ�
			GConsoleLogger->WriteStdOut(Color::YELLOW, L"Dropping Index : [%s] [%s] %s %s\n", dbTable->_name.c_str(), dbIndex->_name.c_str(), dbIndex->GetKeyText().c_str(), dbIndex->GetTypeText().c_str());
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
		GConsoleLogger->WriteStdOut(Color::YELLOW, L"Creating Index : [%s] %s %s [%s]\n", dbTable->_name.c_str(), xmlIndex->GetKeyText().c_str(), xmlIndex->GetTypeText().c_str(), xmlIndex->GetUniqueName().c_str());
		if (xmlIndex->_primaryKey || xmlIndex->_uniqueConstraint)
		{
			_updateQueries[UpdateStep::CreateIndex].push_back(DBModel::Helpers::Format(L"ALTER TABLE [dbo].[%s] ADD CONSTRAINT [%s] %s %s (%s)",
				dbTable->_name.c_str(), xmlIndex->CreateName(dbTable->_name).c_str(), xmlIndex->GetKeyText().c_str(), xmlIndex->GetTypeText().c_str(), xmlIndex->CreateColumnsText().c_str()));
		}
		else
		{
			_updateQueries[UpdateStep::CreateIndex].push_back(DBModel::Helpers::Format(L"CREATE %s INDEX [%s] ON [dbo].[%s] (%s)",
				xmlIndex->GetTypeText(), xmlIndex->CreateName(dbTable->_name).c_str(), dbTable->_name.c_str(), xmlIndex->CreateColumnsText().c_str()));
		}
	}
}

/*---------------------------------------------------------------------------------------------
�̸�     : JsonDBSynchronizer::CompareColumns
�뵵     : Json������ DataBase�� Colum�� ������ �´��� Ȯ��
������   : �̹α�
������¥ : 2022.10.12
----------------------------------------------------------------------------------------------*/
void JsonDBSynchronizer::CompareColumns(shared_ptr<DBModel::Table> dbTable, shared_ptr<DBModel::Column> dbColumn,shared_ptr<DBModel::Column> xmlColumn)
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
				_dependentIndexes.insert(dbIndex->GetUniqueName());

		flag |= ColumnFlag::Default;
	}

	// TODO :DEFALUT ���� �ٸ� ���
	if (flag & ColumnFlag::Default)
	{
		// TODO : DB�� �ش� Default�� ���� ���
		if (dbColumn->_defaultConstraintName.empty() == false)
		{
			// TODO : DB�� Defalut���� ������ �����ϱ� ���� _updateQueries[UpdateStep::AlterColumn]�� CONSTRAIN�� DROP�� �߰��Ѵ�
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
�̸�     : JsonDBSynchronizer::CompareStoredProcedures
�뵵     : Json������ DataBase�� Procedure�� ������ �´��� Ȯ��
������   : �̹α�
������¥ : 2022.10.12
----------------------------------------------------------------------------------------------*/
void JsonDBSynchronizer::CompareStoredProcedures()
{
	// TODO : XML�� �ִ� ���ν��� ����� ���� �´�.
	Gmap<GWString, shared_ptr<DBModel::Procedure>> xmlProceduresMap;
	for (shared_ptr<DBModel::Procedure>& xmlProcedure : _jsonProcedures)
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

/*---------------------------------------------------------------------------------------------
�̸�     : JsonDBSynchronizer::ExecuteUpdateQueries
�뵵     : Json�� DB�� �ٸ� �������� �����ϴ� ������ ���� _updateQueries��
		   UpdateStep ������ �°� DB�� ������ ������ �κ�
������   : �̹α�
������¥ : 2022.10.12
----------------------------------------------------------------------------------------------*/
void JsonDBSynchronizer::ExecuteUpdateQueries()
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
