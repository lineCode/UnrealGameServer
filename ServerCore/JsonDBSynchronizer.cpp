#include "pch.h"
#include "JsonDBSynchronizer.h"
#include "DBBind.h"
#include "JsonParser.h"
#include <regex>

namespace StoredProcedures
{
	/*---------------------------------------------------------------------------------------------
	이름     : QTablesAndColumns
	용도     : Table 과 Colum의 정보를 가져오는 Query
	수정자   : 이민규
	수정날짜 : 2022.10.08
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
	이름     : GetDBTables
	용도     : DBbind 기능을 이용하여 Table의 정보를 편하게 가져오는 객체
	수정자   : 이민규
	수정날짜 : 2022.10.08
	----------------------------------------------------------------------------------------------*/
	class GetDBTables : public DBBind<0, 13>
	{
	public:
		GetDBTables(DBConnection& conn) : DBBind(conn, QTablesAndColumns) {}

		void Out_ObjectId(OUT int32& value) { BindCol(0 ,value); }
		template<int32 N> void Out_TableName(OUT WCHAR(&value)[N]) { BindCol(1,value); }
		template<int32 N> void Out_ColumnName(OUT WCHAR(&value)[N]) { BindCol(2,value); }
		void Out_ColumnId(OUT int32& value) { BindCol(3,value); }
		void Out_UserType(OUT int32& value) { BindCol(4,value); }
		void Out_MaxLength(OUT int32& value) { BindCol(5,value); }
		void Out_IsNullable(OUT bool& value) { BindCol(6,value); }
		void Out_IsIdentity(OUT bool& value) { BindCol(7,value); }
		void Out_SeedValue(OUT int64& value) { BindCol(8,value); }
		void Out_IncrementValue(OUT int64& value) { BindCol(9,value); }
		void Out_DefaultObjectId(OUT int32& value) { BindCol(10,value); }
		template<int32 N> void Out_DefaultDefinition(OUT WCHAR(&value)[N]) { BindCol(11,value); }
		template<int32 N> void Out_DefaultConstraintName(OUT WCHAR(&value)[N]) { BindCol(12,value); }
	};

	/*---------------------------------------------------------------------------------------------
	이름     : QIndexes
	용도     : Index 정보를 가져오는 Query
	수정자   : 이민규
	수정날짜 : 2022.10.08
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
	이름     : GetDBIndexes
	용도     : DBbind기능을 이용하여 Index의 정보를 가져오는 객체
	수정자   : 이민규
	수정날짜 : 2022.10.08
	----------------------------------------------------------------------------------------------*/
	class GetDBIndexes : public DBBind<0, 8>
	{
	public:
		GetDBIndexes(DBConnection& conn) : DBBind(conn, QIndexes) {}

		void Out_ObjectId(OUT int32& value) { BindCol(0 ,value); }
		template<int32 N> void Out_IndexName(OUT WCHAR(&value)[N]) { BindCol(1 ,value); }
		void Out_IndexId(OUT int32& value) { BindCol(2, value); }
		void Out_IndexType(OUT int32& value) { BindCol(3,value); }
		void Out_IsPrimaryKey(OUT bool& value) { BindCol(4,value); }
		void Out_IsUniqueConstraint(OUT bool& value) { BindCol(5,value); }
		void Out_ColumnId(OUT int32& value) { BindCol(6,value); }
		template<int32 N> void Out_ColumnName(OUT WCHAR(&value)[N]) { BindCol(7,value); }
	};

	/*---------------------------------------------------------------------------------------------
	이름     : QStoredProcedures
	용도     : Procedure 정보를 가져오는 Query
	수정자   : 이민규
	수정날짜 : 2022.10.08
	----------------------------------------------------------------------------------------------*/
	const WCHAR* QStoredProcedures =
		L"	SELECT name, OBJECT_DEFINITION(object_id) AS body FROM sys.procedures;";

	/*---------------------------------------------------------------------------------------------
	이름     : GetDBStoredProcedures
	용도     : DBbind기능을 이용하여 Procedure의 정보를 가져오는 객체
	수정자   : 이민규
	수정날짜 : 2022.10.08
	----------------------------------------------------------------------------------------------*/
	class GetDBStoredProcedures : public DBBind<0, 2>
	{
	public:
		GetDBStoredProcedures(DBConnection& conn) : DBBind(conn, QStoredProcedures) {}

		template<int32 N> void Out_Name(OUT WCHAR(&value)[N]) { BindCol(0,value); }
		void Out_Body(OUT WCHAR* value, int32 len) { BindCol(1,value, len); }
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
이름     : XmlDBSynchronizer::ParseJsonDB
용도     : Json을 Parse 후 JsonDBSynchronizer 객체에 들고 있는 함수
수정자   : 이민규
수정날짜 : 2022.10.12
----------------------------------------------------------------------------------------------*/
void JsonDBSynchronizer::ParseJsonDB(const WCHAR* path)
{
	rapidjson::Document document;
	JsonParser jsonparser;

	// TODO : Json을 읽어 옴
	jsonparser.ParseJson(path, document);

	// TODO : Json 테이블을 가져옴
	JsonObject tables(document["Table"]);

	// TODO : Json의 테이블을 순차적으로 가져옴
	for(JsonObject table : tables._object.GetArray())  
	{
		shared_ptr<DBModel::Table> tabletemp = GMakeShared<DBModel::Table>();
		tabletemp->_name = table.GetWString("name");

		// TODO : 테이블의 Column을 가져옴
		Gvector<JsonObject> columns = table.GetArrayGvector("Column");
		for(JsonObject & column : columns)
		{
			shared_ptr<DBModel::Column> columntemp = GMakeShared<DBModel::Column>();
			columntemp->_name = column.GetWString("name");
			columntemp->_typeText = column.GetWString("type");
			columntemp->_type = DBModel::Helpers::StringToDataType(columntemp->_typeText.c_str(), OUT columntemp->_maxLength);
			CRASH_IF(columntemp->_type == DBModel::SQLDataType::None);
			columntemp->_nullable = column._object["notnull"].GetBool();

			// TODO : identity가 존재한다면
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

		// TODO : 테이블의 Index를 가져옴
		Gvector<JsonObject> indexes = table.GetArrayGvector("Index");
		for (JsonObject& index : indexes)
		{
			// TODO : Index의 clustered에 맞게 넣음
			shared_ptr<DBModel::Index> indextemp = GMakeShared<DBModel::Index>();
			const WCHAR* typeStr = index.GetWString("type");
			if (::_wcsicmp(typeStr, L"clustered") == 0)
				indextemp->_type = DBModel::IndexType::Clustered;
			else if (::_wcsicmp(typeStr, L"nonclustered") == 0)
				indextemp->_type = DBModel::IndexType::NonClustered;
			else
				CRASH("Invalid Index Type");

			// TODO : indexd의 primaryKey ,  uniqueConstraint체크 후 넣는 부분
			indextemp->_primaryKey = index._object["PrimaryKey"].GetBool();
			indextemp->_uniqueConstraint = index._object["UniqueKey"].GetBool();

			// TODO : Column의 Index 설정
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

	// TODO : procedures 가져오는 부분
	JsonObject procedures(document["Procedure"]);
	for (JsonObject procedure : procedures._object.GetArray())
	{
		// TODO : proceudre의 이름과 쿼리문 가져옴
		shared_ptr<DBModel::Procedure> proceduretemp = GMakeShared<DBModel::Procedure>();
		proceduretemp->_name = procedure.GetWString("name");
		proceduretemp->_body = procedure.GetWString("Body");

		// TODO : Proceudre의 Param 정보를 저장하는 부분
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

	// TODO : RemovedTable을 가져오는 부분
	if (document.HasMember("RemovedTable") == true)
	{
		// TODO : 삭제할 테이블의 이름을 가져와 저장하는 부분
		JsonObject removetables(document["RemovedTable"]);
		for (JsonObject removedTable : removetables._object.GetArray())
		{
			_jsonRemovedTables.insert(removedTable.GetWString("name"));
		}
	}
}

/*---------------------------------------------------------------------------------------------
이름     : JsonDBSynchronizer::DBTablesSave
용도     : DataBase에 Query를 보내서 Table정보를 저장하는 함수
수정자   : 이민규
수정날짜 : 2022.10.12
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

	// TODO : DataGBase의 테이블 정보를 가져오기 위한 쿼리를 DBBind로 설정하는 부분
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

	// TODO : DataBase의 테이블의 Colum 정보를 한줄 씩 가져오는 부분
	while (getDBTables.Fetch())
	{
		shared_ptr<DBModel::Table> table;

		// TODO : 테이블의 ObjectId를 찾는 부분
		auto findTable = std::find_if(_dbTables.begin(), _dbTables.end(), [=](const shared_ptr<DBModel::Table>& table) { return table->_objectId == objectId; });
		if (findTable == _dbTables.end())
		{
			// TODO : ObjectId가 없을 경우 table 생성 후 _dbTable에 추가
			table = GMakeShared<DBModel::Table>();
			table->_objectId = objectId;
			table->_name = tableName;
			_dbTables.push_back(table);
		}
		else
		{
			// TODO : ObjectId가 있을 경우 table에 넣어줌
			table = *findTable;
		}

		// TODO : 테이블의 Colum을 설정하고 저장하는 부분
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
이름     : JsonDBSynchronizer::DBIndexsSave
용도     : DataBase에 Query를 보내서 Index정보를 저장하는 함수
수정자   : 이민규
수정날짜 : 2022.10.12
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

	// TODO : DataBase의 Index 정보를 가져오기 위한 쿼리를 DBBind로 설정하는 부분
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

	// TODO : DataBase의 Index 정보를 한줄 씩 가져오는 부분
	while (getDBIndexes.Fetch())
	{
		// TODO : ObjectId에 맞는 테이블을 찾는 부분
		auto findTable = std::find_if(_dbTables.begin(), _dbTables.end(), [=](const shared_ptr<DBModel::Table>& table) { return table->_objectId == objectId; });
		CRASH_IF(findTable == _dbTables.end());

		// TODO : 테이블의 index를 가져오는 부분
		Gvector<shared_ptr<DBModel::Index>>& indexes = (*findTable)->_indexes;

		// TODO : 테이블의 index_id를 찾는 부분
		auto findIndex = std::find_if(indexes.begin(), indexes.end(), [indexId](shared_ptr<DBModel::Index>& index) { return index->_indexId == indexId; });
		if (findIndex == indexes.end())
		{
			// TODO : 테이블의 index_id가 없을 경우 인덱스 생성 후 설정
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


		// TODO : 인덱스가 걸린 column 찾아서 매핑해준다
		Gvector<shared_ptr<DBModel::Column>>& columns = (*findTable)->_columns;
		auto findColumn = std::find_if(columns.begin(), columns.end(), [columnId](shared_ptr<DBModel::Column>& column) { return column->_columnId == columnId; });
		CRASH_IF(findColumn == columns.end());
		(*findIndex)->_columns.push_back(*findColumn);
	}

	return true;
}

/*---------------------------------------------------------------------------------------------
이름     : JsonDBSynchronizer::DBProceduresSave
용도     : DataBase에 Query를 보내서 Procedure정보를 저장하는 함수
수정자   : 이민규
수정날짜 : 2022.10.12
----------------------------------------------------------------------------------------------*/
bool JsonDBSynchronizer::DBProceduresSave()
{
	WCHAR name[101] = { 0 };
	Gvector<WCHAR> body(PROCEDURE_MAX_LEN);

	// TODO : DataBase의 Procedures 정보를 가져오기 위한 쿼리를 DBBind로 설정하는 부분
	StoredProcedures::GetDBStoredProcedures getDBStoredProcedures(_dbConn);
	getDBStoredProcedures.Out_Name(OUT name);
	getDBStoredProcedures.Out_Body(OUT & body[0], PROCEDURE_MAX_LEN);

	if (getDBStoredProcedures.Execute() == false)
		return false;

	// TODO : DataBase의 Procedure 정보를 한줄 씩 가져오는 부분
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
이름     : JsonDBSynchronizer::CompareDBModel
용도     : Json 정보와 DataBase의 정보가 일치하는지 확인하는 후 수정하는 함수
수정자   : 이민규
수정날짜 : 2022.10.12
----------------------------------------------------------------------------------------------*/
void JsonDBSynchronizer::CompareDBModel()
{
	// TODO : 업데이트에 필요한 변수들 모두 초기화
	_dependentIndexes.clear();
	for (Gvector<GWString>& queries : _updateQueries)
		queries.clear();

	// TODO : JSON에 있는 테이블 목록을 우선 갖고 온다.
	Gmap<GWString, shared_ptr<DBModel::Table>> xmlTableMap;
	for (shared_ptr<DBModel::Table>& xmlTable : _jsonTables)
		xmlTableMap[xmlTable->_name] = xmlTable;

	// TODO : DB에 실존하는 테이블들을 돌면서 JSON에 정의된 테이블들과 이름을 비교한다.
	for (shared_ptr<DBModel::Table>& dbTable : _dbTables)
	{
		auto findTable = xmlTableMap.find(dbTable->_name);
		// TODO : JSON이랑 DB 모두에 존재할 경우 테이블 정보를 확인
		if (findTable != xmlTableMap.end())
		{
			shared_ptr<DBModel::Table> xmlTable = findTable->second;
			CompareTables(dbTable, xmlTable);
			xmlTableMap.erase(findTable);
		}
		else
		{
			// TODO : 제거될 테이블에 예약되어 있는지 확인
			if (_jsonRemovedTables.find(dbTable->_name) != _jsonRemovedTables.end())
			{
				// TODO : DB에는 존재하지만 JSON에는 없는 경우라서 테이블 삭제
				GConsoleLogManager->WriteStdOut(Color::YELLOW, L"Removing Table : [dbo].[%s]\n", dbTable->_name.c_str());
				_updateQueries[QueryStep::DropTable].push_back(DBModel::Helpers::Format(L"DROP TABLE [dbo].[%s]", dbTable->_name.c_str()));
			}
		}
	}

	//TODO : JSON에는 존재히지만 DB에는 없는 테이블
	for (auto& mapIt : xmlTableMap)
	{
		shared_ptr<DBModel::Table>& xmlTable = mapIt.second;

		//TODO : XML에 테이블의 컬럼을 추가하는 부분
		GWString columnsStr;
		const int32 size = static_cast<int32>(xmlTable->_columns.size());
		for (int32 i = 0; i < size; i++)
		{
			if (i != 0)
				columnsStr += L",";
			columnsStr += L"\n\t";
			columnsStr += xmlTable->_columns[i]->CreateText();
		}

		//TODO : JSON에 테이블을 DB에 생성
		GConsoleLogManager->WriteStdOut(Color::YELLOW, L"Creating Table : [dbo].[%s]\n", xmlTable->_name.c_str());
		_updateQueries[QueryStep::CreateTable].push_back(DBModel::Helpers::Format(L"CREATE TABLE [dbo].[%s] (%s)", xmlTable->_name.c_str(), columnsStr.c_str()));

		//TODO : DB에 생성된 JSON 테이블 컬럼을 수정
		for (shared_ptr<DBModel::Column>& xmlColumn : xmlTable->_columns)
		{
			if (xmlColumn->_default.empty())
				continue;

			_updateQueries[QueryStep::DefaultConstraint].push_back(DBModel::Helpers::Format(L"ALTER TABLE [dbo].[%s] ADD CONSTRAINT [%s] DEFAULT (%s) FOR [%s]",
				xmlTable->_name.c_str(),
				DBModel::Helpers::Format(L"DF_%s_%s", xmlTable->_name.c_str(), xmlColumn->_name.c_str()).c_str(),
				xmlColumn->_default.c_str(),
				xmlColumn->_name.c_str()));
		}

		//TODO : DB에 생성된 JSON 테이블 인덱스를 수정
		for (shared_ptr<DBModel::Index>& xmlIndex : xmlTable->_indexes)
		{
			GConsoleLogManager->WriteStdOut(Color::YELLOW, L"Creating Index : [%s] %s %s [%s]\n", xmlTable->_name.c_str(), xmlIndex->GetKeyText().c_str(), xmlIndex->GetClusteredText().c_str(), xmlIndex->GetConstraintName().c_str());
			if (xmlIndex->_primaryKey || xmlIndex->_uniqueConstraint)
			{
				_updateQueries[QueryStep::CreateIndex].push_back(DBModel::Helpers::Format(
					L"ALTER TABLE [dbo].[%s] ADD CONSTRAINT [%s] %s %s (%s)",
					xmlTable->_name.c_str(),
					xmlIndex->CreateName(xmlTable->_name).c_str(),
					xmlIndex->GetKeyText().c_str(),
					xmlIndex->GetClusteredText().c_str(),
					xmlIndex->CreateColumnsText().c_str()));
			}
			else
			{
				_updateQueries[QueryStep::CreateIndex].push_back(DBModel::Helpers::Format(
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
이름     : JsonDBSynchronizer::CompareDBModel
용도     : Json정보와 DataBase의 Table의 정보가 맞는지 확인
수정자   : 이민규
수정날짜 : 2022.10.12
----------------------------------------------------------------------------------------------*/
void JsonDBSynchronizer::CompareTables(shared_ptr<DBModel::Table> dbTable, shared_ptr<DBModel::Table> xmlTable)
{
	// TODO : JSON에 있는 컬럼 목록을 갖고 온다.
	Gmap<GWString, shared_ptr<DBModel::Column>> xmlColumnMap;
	for (shared_ptr<DBModel::Column>& xmlColumn : xmlTable->_columns)
		xmlColumnMap[xmlColumn->_name] = xmlColumn;

	// TODO : DB에 실존하는 테이블 컬럼들을 돌면서 XML에 정의된 컬럼들과 이름을 비교한다.
	for (shared_ptr<DBModel::Column>& dbColumn : dbTable->_columns)
	{
		auto findColumn = xmlColumnMap.find(dbColumn->_name);
		// TODO : JSON이랑 DB 모두에 이름이 존재할 경우 테이블 컬럼 정보를 확인
		if (findColumn != xmlColumnMap.end())
		{
			shared_ptr<DBModel::Column>& xmlColumn = findColumn->second;
			CompareColumns(dbTable, dbColumn, xmlColumn);
			xmlColumnMap.erase(findColumn);
		}
		// TODO : DB에는 존재하는데 XML에는 없는 경우
		else
		{
			// TODO : Constrain이 있을 시 CONSTRAIN을 삭제한다
			GConsoleLogManager->WriteStdOut(Color::YELLOW, L"Dropping Column : [%s].[%s]\n", dbTable->_name.c_str(), dbColumn->_name.c_str());
			if (dbColumn->_defaultConstraintName.empty() == false)
				_updateQueries[QueryStep::DropColumn].push_back(DBModel::Helpers::Format(L"ALTER TABLE [dbo].[%s] DROP CONSTRAINT [%s]", dbTable->_name.c_str(), dbColumn->_defaultConstraintName.c_str()));

			// TODO : 해당 칼럼을 삭제한다
			_updateQueries[QueryStep::DropColumn].push_back(DBModel::Helpers::Format(L"ALTER TABLE [dbo].[%s] DROP COLUMN [%s]", dbTable->_name.c_str(), dbColumn->_name.c_str()));
		}
	}

	// TODO : JSON에는 있는데 DB에는 없는 경우 
	for (auto& mapIt : xmlColumnMap)
	{
		shared_ptr<DBModel::Column>& xmlColumn = mapIt.second;
		DBModel::Column newColumn = *xmlColumn;
		newColumn._nullable = true;

		// TODO : JSON 컬럼을 추가한다
		GConsoleLogManager->WriteStdOut(Color::YELLOW, L"Adding Column : [%s].[%s]\n", dbTable->_name.c_str(), xmlColumn->_name.c_str());
		_updateQueries[QueryStep::AddColumn].push_back(DBModel::Helpers::Format(L"ALTER TABLE [dbo].[%s] ADD %s %s",
			dbTable->_name.c_str(), xmlColumn->_name.c_str(), xmlColumn->_typeText.c_str()));

		// TODO : JSON설정에 맞게 추가한 칼럼을 업데이트 한다
		if (xmlColumn->_nullable == false && xmlColumn->_default.empty() == false)
		{
			_updateQueries[QueryStep::AddColumn].push_back(DBModel::Helpers::Format(L"SET NOCOUNT ON; UPDATE [dbo].[%s] SET [%s] = %s WHERE [%s] IS NULL",
				dbTable->_name.c_str(), xmlColumn->_name.c_str(), xmlColumn->_default.c_str(), xmlColumn->_name.c_str()));
		}

		if (xmlColumn->_nullable == false)
		{
			_updateQueries[QueryStep::AddColumn].push_back(DBModel::Helpers::Format(L"ALTER TABLE [dbo].[%s] ALTER COLUMN %s",
				dbTable->_name.c_str(), xmlColumn->CreateText().c_str()));
		}

		if (xmlColumn->_default.empty() == false)
		{
			_updateQueries[QueryStep::AddColumn].push_back(DBModel::Helpers::Format(L"ALTER TABLE [dbo].[%s] ADD CONSTRAINT [DF_%s_%s] DEFAULT (%s) FOR [%s]",
				dbTable->_name.c_str(), dbTable->_name.c_str(), xmlColumn->_name.c_str(), xmlColumn->_default.c_str(), xmlColumn->_name.c_str()));
		}
	}

	// TODO : JSON에 있는 인덱스 목록을 갖고 온다.
	Gmap<GWString, shared_ptr<DBModel::Index>> xmlIndexMap;
	for (shared_ptr<DBModel::Index>& xmlIndex : xmlTable->_indexes)
		xmlIndexMap[xmlIndex->GetConstraintName()] = xmlIndex;

	// TODO : DB에 실존하는 테이블 인덱스들을 돌면서 XML에 정의된 인덱스들과 비교한다.
	for (shared_ptr<DBModel::Index>& dbIndex : dbTable->_indexes)
	{
		auto findIndex = xmlIndexMap.find(dbIndex->GetConstraintName());
		// TODO : JSON에 DB 인덱스가 있고 값이 다른 인덱스가 DB에 없는 경우
		if (findIndex != xmlIndexMap.end() && _dependentIndexes.find(dbIndex->GetConstraintName()) == _dependentIndexes.end())
		{
			shared_ptr<DBModel::Index> xmlIndex = findIndex->second;
			xmlIndexMap.erase(findIndex);
		}
		else
		{
			// TODO : 인덱스에 값이 다르거나 XML에는 없고 DB에 있는 경우 제거한다
			GConsoleLogManager->WriteStdOut(Color::YELLOW, L"Dropping Index : [%s] [%s] %s %s\n", dbTable->_name.c_str(), dbIndex->_name.c_str(), dbIndex->GetKeyText().c_str(), dbIndex->GetClusteredText().c_str());
			if (dbIndex->_primaryKey || dbIndex->_uniqueConstraint)
				_updateQueries[QueryStep::DropIndex].push_back(DBModel::Helpers::Format(L"ALTER TABLE [dbo].[%s] DROP CONSTRAINT [%s]", dbTable->_name.c_str(), dbIndex->_name.c_str()));
			else
				_updateQueries[QueryStep::DropIndex].push_back(DBModel::Helpers::Format(L"DROP INDEX [%s] ON [dbo].[%s]", dbIndex->_name.c_str(), dbTable->_name.c_str()));
		}
	}

	// TODO : JSON에는 있고 DB에는 없는 경우 인덱스 추가
	for (auto& mapIt : xmlIndexMap)
	{
		shared_ptr<DBModel::Index> xmlIndex = mapIt.second;
		GConsoleLogManager->WriteStdOut(Color::YELLOW, L"Creating Index : [%s] %s %s [%s]\n", dbTable->_name.c_str(), xmlIndex->GetKeyText().c_str(), xmlIndex->GetClusteredText().c_str(), xmlIndex->GetConstraintName().c_str());
		if (xmlIndex->_primaryKey || xmlIndex->_uniqueConstraint)
		{
			_updateQueries[QueryStep::CreateIndex].push_back(DBModel::Helpers::Format(L"ALTER TABLE [dbo].[%s] ADD CONSTRAINT [%s] %s %s (%s)",
				dbTable->_name.c_str(), xmlIndex->CreateName(dbTable->_name).c_str(), xmlIndex->GetKeyText().c_str(), xmlIndex->GetClusteredText().c_str(), xmlIndex->CreateColumnsText().c_str()));
		}
		else
		{
			_updateQueries[QueryStep::CreateIndex].push_back(DBModel::Helpers::Format(L"CREATE %s INDEX [%s] ON [dbo].[%s] (%s)",
				xmlIndex->GetClusteredText(), xmlIndex->CreateName(dbTable->_name).c_str(), dbTable->_name.c_str(), xmlIndex->CreateColumnsText().c_str()));
		}
	}
}

/*---------------------------------------------------------------------------------------------
이름     : JsonDBSynchronizer::CompareColumns
용도     : Json정보와 DataBase의 Colum의 정보가 맞는지 확인
수정자   : 이민규
수정날짜 : 2022.10.12
----------------------------------------------------------------------------------------------*/
void JsonDBSynchronizer::CompareColumns(shared_ptr<DBModel::Table> dbTable, shared_ptr<DBModel::Column> dbColumn,shared_ptr<DBModel::Column> xmlColumn)
{
	uint8 flag = 0;

	// TODO : DB 와 JSON의 컬럼의 모든 인자를 비교 후 다른것은 Flag에 기록
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

	// TODO : JSON 과 DB 컬럼의 다른 점이 있을 경우 출력
	if (flag)
	{
		GConsoleLogManager->WriteStdOut(Color::YELLOW, L"Updating Column [%s] : (%s) -> (%s)\n", dbTable->_name.c_str(), dbColumn->CreateText().c_str(), xmlColumn->CreateText().c_str());
	}

	// TODO : 연관된 인덱스가 있으면 나중에 삭제하기 위해 기록한다.
	if (flag & (ColumnFlag::Type | ColumnFlag::Length | ColumnFlag::Nullable))
	{
		for (shared_ptr<DBModel::Index>& dbIndex : dbTable->_indexes)
			if (dbIndex->DependsOn(dbColumn->_name))
				_dependentIndexes.insert(dbIndex->GetConstraintName());

		flag |= ColumnFlag::Default;
	}

	// TODO :DEFALUT 값이 다를 경우
	if (flag & ColumnFlag::Default)
	{
		// TODO : DB에 해당 Default가 있을 경우
		if (dbColumn->_defaultConstraintName.empty() == false)
		{
			// TODO : DB에 Defalut관한 내용을 수정하기 위해 _updateQueries[QueryStep::AlterColumn]에 CONSTRAIN에 DROP을 추가한다
			_updateQueries[QueryStep::AlterColumn].push_back(DBModel::Helpers::Format(
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

	// TODO :TYPE OR LENGTH OR IDENTITY 값이 다를 경우
	if (flag & (ColumnFlag::Type | ColumnFlag::Length | ColumnFlag::Identity))
	{
		// TODO : JSON의 TYPE , LENGTH , IDENTITY 기준으로 컬럼을 새로 만들어서 기존 컬럼에 변경으로 추가한다
		_updateQueries[QueryStep::AlterColumn].push_back(DBModel::Helpers::Format(
			L"ALTER TABLE [dbo].[%s] ALTER COLUMN %s",
			dbTable->_name.c_str(),
			newColumn.CreateText().c_str()));
	}


	newColumn._nullable = xmlColumn->_nullable;

	// TODO :Nullable 값이 다를 경우
	if (flag & ColumnFlag::Nullable)
	{
		// TODO : default가 있는 경우
		if (xmlColumn->_default.empty() == false)
		{
			_updateQueries[QueryStep::AlterColumn].push_back(DBModel::Helpers::Format(
				L"SET NOCOUNT ON; UPDATE [dbo].[%s] SET [%s] = %s WHERE [%s] IS NULL",
				dbTable->_name.c_str(),
				xmlColumn->_name.c_str(),
				xmlColumn->_name.c_str(),
				xmlColumn->_name.c_str()));
		}

		// TODO : JSON의 Nullable을 기준으로 기존 컬럼 변경에 추가 
		_updateQueries[QueryStep::AlterColumn].push_back(DBModel::Helpers::Format(
			L"ALTER TABLE [dbo].[%s] ALTER COLUMN %s",
			dbTable->_name.c_str(),
			newColumn.CreateText().c_str()));
	}

	// TODO :Default 값이 다를 경우
	if (flag & ColumnFlag::Default)
	{
		// TODO :_defaultConstraintName이 있는 경우
		if (dbColumn->_defaultConstraintName.empty() == false)
		{
			// TODO : XML의 Default을 기준으로 기존 컬럼 변경에 추가 
			_updateQueries[QueryStep::AlterColumn].push_back(DBModel::Helpers::Format(
				L"ALTER TABLE [dbo].[%s] ADD CONSTRAINT [%s] DEFAULT (%s) FOR [%s]",
				dbTable->_name.c_str(),
				DBModel::Helpers::Format(L"DF_%s_%s", dbTable->_name.c_str(), dbColumn->_name.c_str()).c_str(),
				dbColumn->_default.c_str(), dbColumn->_name.c_str()));
		}
	}
}

/*---------------------------------------------------------------------------------------------
이름     : JsonDBSynchronizer::CompareStoredProcedures
용도     : Json정보와 DataBase의 Procedure의 정보가 맞는지 확인
수정자   : 이민규
수정날짜 : 2022.10.12
----------------------------------------------------------------------------------------------*/
void JsonDBSynchronizer::CompareStoredProcedures()
{
	// TODO : XML에 있는 프로시저 목록을 갖고 온다.
	Gmap<GWString, shared_ptr<DBModel::Procedure>> xmlProceduresMap;
	for (shared_ptr<DBModel::Procedure>& xmlProcedure : _jsonProcedures)
		xmlProceduresMap[xmlProcedure->_name] = xmlProcedure;

	//TODO : DB에 와 JSON에 프로시저가 존재하는지 확인.
	for (shared_ptr<DBModel::Procedure>& dbProcedure : _dbProcedures)
	{
		auto findProcedure = xmlProceduresMap.find(dbProcedure->_name);
		if (findProcedure != xmlProceduresMap.end())
		{
			shared_ptr<DBModel::Procedure> xmlProcedure = findProcedure->second;
			GWString xmlBody = xmlProcedure->GenerateCreateQuery();
			//TODO : 둘다 존재 할 경우 XML 과 DB의 Body가 같은지 확인 후 다를 경우 XML 기준으로 변경
			if (DBModel::Helpers::RemoveWhiteSpace(dbProcedure->_fullBody) != DBModel::Helpers::RemoveWhiteSpace(xmlBody))
			{
				GConsoleLogManager->WriteStdOut(Color::YELLOW, L"Updating Procedure : %s\n", dbProcedure->_name.c_str());
				_updateQueries[QueryStep::StoredProcecure].push_back(xmlProcedure->GenerateAlterQuery());
			}
			xmlProceduresMap.erase(findProcedure);
		}
	}

	// TODO : JSON에는 존재하는데 DB에는 없는 경우 
	for (auto& mapIt : xmlProceduresMap)
	{
		//TODO : JSON의 프로시저를 추가
		GConsoleLogManager->WriteStdOut(Color::YELLOW, L"Updating Procedure : %s\n", mapIt.first.c_str());
		_updateQueries[QueryStep::StoredProcecure].push_back(mapIt.second->GenerateCreateQuery());
	}
}

/*---------------------------------------------------------------------------------------------
이름     : JsonDBSynchronizer::ExecuteUpdateQueries
용도     : Json과 DB의 다른 정보들을 수정하는 쿼리를 모은 _updateQueries를
		   QueryStep 순서에 맞게 DB에 쿼리를 보내는 부분
수정자   : 이민규
수정날짜 : 2022.10.12
----------------------------------------------------------------------------------------------*/
void JsonDBSynchronizer::ExecuteUpdateQueries()
{
	for (int32 step = 0; step < QueryStep::Max; step++)
	{
		for (GWString& query : _updateQueries[step])
		{
			_dbConn.Unbind();
			CRASH_IF(_dbConn.Execute(query.c_str()) == false);
		}
	}
}
