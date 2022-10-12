#pragma once
#include "DBConnection.h"
#include "DBModel.h"

/*---------------------------------------------------------------------------------------------
�̸�     : XmlDBSynchronizer
�뵵     : ���� XML�� DataBase�� ��ġ�ϵ��� �����ϴ� ��ü
������   : �̹α�
������¥ : 2022.10.08
----------------------------------------------------------------------------------------------*/
class XmlDBSynchronizer
{
	enum
	{
		PROCEDURE_MAX_LEN = 10000
	};

	enum UpdateStep : uint8
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
	XmlDBSynchronizer(DBConnection& conn) : _dbConn(conn) { }
	~XmlDBSynchronizer() {};

	bool		Synchronize(const WCHAR* path);

private:
	void		ParseXmlDB(const WCHAR* path);
	bool		GatherDBTables();
	bool		GatherDBIndexes();
	bool		GatherDBStoredProcedures();

	void		CompareDBModel();
	void		CompareTables(shared_ptr<DBModel::Table> dbTable, shared_ptr<DBModel::Table> xmlTable);
	void		CompareColumns(shared_ptr<DBModel::Table> dbTable, shared_ptr<DBModel::Column> dbColumn, shared_ptr<DBModel::Column> xmlColumn);
	void		CompareStoredProcedures();

	void		ExecuteUpdateQueries();

private:
	DBConnection& _dbConn;

	// XML ������
	Gvector<shared_ptr<DBModel::Table>>			_xmlTables;
	Gvector<shared_ptr<DBModel::Procedure>>		_xmlProcedures;
	Gset<GWString>							_xmlRemovedTables;

	// DB ������
	Gvector<shared_ptr<DBModel::Table>>			_dbTables;
	Gvector<shared_ptr<DBModel::Procedure>>		_dbProcedures;

private:
	Gset<GWString>							_dependentIndexes;
	Gvector<GWString>						_updateQueries[UpdateStep::Max];
};