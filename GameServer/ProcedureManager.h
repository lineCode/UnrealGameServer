#pragma once
#include "Types.h"
#include <windows.h>
#include "DBBind.h"

/*---------------------------------------------------------------------------------------------
								PROCEUDRE AUTO CREATEA
----------------------------------------------------------------------------------------------*/
namespace ProcedureManager
{
	
    /*---------------------------------------------------------------------------------------------
                                        InsertGold(Param : 3 , Column : 0)
    ----------------------------------------------------------------------------------------------*/
    class InsertGold : public DBBind<3,0>
    {
    public:
    	InsertGold(DBConnection& conn) : DBBind(conn, L"{CALL dbo.ProcInsertGold(?,?,?)}") { }
    	void Param_Gold(int32& v) { BindParam(v); };
    	void Param_Gold(int32&& v) { _gold = std::move(v); BindParam(_gold); };
    	template<int32 N> void Param_Name(WCHAR(&v)[N]) { BindParam(v); };
    	template<int32 N> void Param_Name(const WCHAR(&v)[N]) { BindParam(v); };
    	void Param_Name(WCHAR* v, int32 count) { BindParam(v, count); };
    	void Param_Name(const WCHAR* v, int32 count) { BindParam(v, count); };
    	void Param_CreateDate(TIMESTAMP_STRUCT& v) { BindParam(v); };
    	void Param_CreateDate(TIMESTAMP_STRUCT&& v) { _createDate = std::move(v); BindParam(_createDate); };

    private:
    	int32 _gold = {};
    	TIMESTAMP_STRUCT _createDate = {};
    };

    /*---------------------------------------------------------------------------------------------
                                        GetGold(Param : 1 , Column : 4)
    ----------------------------------------------------------------------------------------------*/
    class GetGold : public DBBind<1,4>
    {
    public:
    	GetGold(DBConnection& conn) : DBBind(conn, L"{CALL dbo.ProcGetGold(?)}") { }
    	void Param_Gold(int32& v) { BindParam(v); };
    	void Param_Gold(int32&& v) { _gold = std::move(v); BindParam(_gold); };
    	void Column_Id(OUT int32& v) { BindCol(v); };
    	void Column_Gold(OUT int32& v) { BindCol(v); };
    	template<int32 N> void Column_Name(OUT WCHAR(&v)[N]) { BindCol(v); };
    	void Column_CreateDate(OUT TIMESTAMP_STRUCT& v) { BindCol(v); };

    private:
    	int32 _gold = {};
    };


     
};