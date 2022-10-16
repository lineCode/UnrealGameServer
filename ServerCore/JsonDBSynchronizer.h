#pragma once
#include "DBConnection.h"
#include "DBModel.h"
#include "rapidjson/document.h"

/*---------------------------------------------------------------------------------------------
�̸�     : JsonDBSynchronizer
�뵵     : ���� Json�� DataBase�� ��ġ�ϵ��� �����ϴ� ��ü
������   : �̹α�
������¥ : 2022.10.12
----------------------------------------------------------------------------------------------*/
class JsonDBSynchronizer
{
	enum
	{
		PROCEDURE_MAX_LEN = 10000
	};

	enum QueryStep : uint8
	{
		DropIndex,
		AlterColumn,
		AddColumn,
		CreateTable,
		DefaultConstraint,
		CreateIndex,
		DropColumn,
		DropTable,
		StoredProcecure,

		Max
	};

	enum ColumnFlag : uint8
	{
		Type = 1 << 0,
		Nullable = 1 << 1,
		Identity = 1 << 2,
		Default = 1 << 3,
		Length = 1 << 4,
	};

public:
	JsonDBSynchronizer(DBConnection& connection) : _dbConn(connection) {}
	~JsonDBSynchronizer() = default;

	bool Synchronize(const WCHAR* path);

private:
	void ParseJsonDB(const WCHAR* path);
	bool DBTablesSave();
	bool DBIndexsSave();
	bool DBProceduresSave();

	void CompareDBModel();
	void CompareTables(shared_ptr<DBModel::Table> dbTable, shared_ptr<DBModel::Table> xmlTable);
	void CompareColumns(shared_ptr<DBModel::Table> dbTable, shared_ptr<DBModel::Column> dbColumn, shared_ptr<DBModel::Column> xmlColumn);
	void CompareStoredProcedures();
		 
	void ExecuteUpdateQueries();
private:
	DBConnection& _dbConn;

	// TODO : JSON ������
	Gvector<shared_ptr<DBModel::Table>>			_jsonTables;
	Gvector<shared_ptr<DBModel::Procedure>>		_jsonProcedures;
	Gset<GWString>							_jsonRemovedTables;

	// TODO : DB ������
	Gvector<shared_ptr<DBModel::Table>> _dbTables;
	Gvector<shared_ptr<DBModel::Procedure>> _dbProcedures;

private:
	Gset<GWString> _dependentIndexes;
	Gvector<GWString> _updateQueries[QueryStep::Max];
};

