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
    			{proc.name} PROCEUDRE (Param : 1 Column : 0)
    ----------------------------------------------------------------------------------------------*/
    class CreateAccount : public DBBind<1,0>
    {
    public:
    	CreateAccount(DBConnection& conn) : DBBind(conn, L"{CALL dbo.ProcCreateAccount(?)}") { }
    	template<int32 N> void Param_Name(WCHAR(&v)[N]) { BindParam(v); };
    	template<int32 N> void Param_Name(const WCHAR(&v)[N]) { BindParam(v); };
    	void Param_Name(WCHAR* v, int32 count) { BindParam(v, count); };
    	void Param_Name(const WCHAR* v, int32 count) { BindParam(v, count); };

    private:
    };

    /*---------------------------------------------------------------------------------------------
    			{proc.name} PROCEUDRE (Param : 1 Column : 1)
    ----------------------------------------------------------------------------------------------*/
    class FindAccountid : public DBBind<1,1>
    {
    public:
    	FindAccountid(DBConnection& conn) : DBBind(conn, L"{CALL dbo.ProcFindAccountid(?)}") { }
    	template<int32 N> void Param_Name(WCHAR(&v)[N]) { BindParam(v); };
    	template<int32 N> void Param_Name(const WCHAR(&v)[N]) { BindParam(v); };
    	void Param_Name(WCHAR* v, int32 count) { BindParam(v, count); };
    	void Param_Name(const WCHAR* v, int32 count) { BindParam(v, count); };
    	void Column_Accountid(OUT int32& v) { BindCol(v); };

    private:
    };

    /*---------------------------------------------------------------------------------------------
    			{proc.name} PROCEUDRE (Param : 1 Column : 10)
    ----------------------------------------------------------------------------------------------*/
    class FindAccountPlayer : public DBBind<1,10>
    {
    public:
    	FindAccountPlayer(DBConnection& conn) : DBBind(conn, L"{CALL dbo.ProcFindAccountPlayer(?)}") { }
    	template<int32 N> void Param_Name(WCHAR(&v)[N]) { BindParam(v); };
    	template<int32 N> void Param_Name(const WCHAR(&v)[N]) { BindParam(v); };
    	void Param_Name(WCHAR* v, int32 count) { BindParam(v, count); };
    	void Param_Name(const WCHAR* v, int32 count) { BindParam(v, count); };
    	void Column_Playerid(OUT int32& v) { BindCol(v); };
    	void Column_AccountId(OUT int32& v) { BindCol(v); };
    	template<int32 N> void Column_Playername(OUT WCHAR(&v)[N]) { BindCol(v); };
    	void Column_Objectid(OUT int32& v) { BindCol(v); };
    	void Column_Attack(OUT int32& v) { BindCol(v); };
    	void Column_Hp(OUT int32& v) { BindCol(v); };
    	void Column_Level(OUT int32& v) { BindCol(v); };
    	void Column_MaxHp(OUT int32& v) { BindCol(v); };
    	void Column_Speed(OUT float& v) { BindCol(v); };
    	void Column_TotalExp(OUT int32& v) { BindCol(v); };

    private:
    };

    /*---------------------------------------------------------------------------------------------
    			{proc.name} PROCEUDRE (Param : 9 Column : 0)
    ----------------------------------------------------------------------------------------------*/
    class PlayerCreate : public DBBind<9,0>
    {
    public:
    	PlayerCreate(DBConnection& conn) : DBBind(conn, L"{CALL dbo.ProcPlayerCreate(?,?,?,?,?,?,?,?,?)}") { }
    	template<int32 N> void Param_Name(WCHAR(&v)[N]) { BindParam(v); };
    	template<int32 N> void Param_Name(const WCHAR(&v)[N]) { BindParam(v); };
    	void Param_Name(WCHAR* v, int32 count) { BindParam(v, count); };
    	void Param_Name(const WCHAR* v, int32 count) { BindParam(v, count); };
    	void Param_Accountid(int32& v) { BindParam(v); };
    	void Param_Accountid(int32&& v) { _accountid = std::move(v); BindParam(_accountid); };
    	void Param_Attack(int32& v) { BindParam(v); };
    	void Param_Attack(int32&& v) { _attack = std::move(v); BindParam(_attack); };
    	void Param_Hp(int32& v) { BindParam(v); };
    	void Param_Hp(int32&& v) { _hp = std::move(v); BindParam(_hp); };
    	void Param_Level(int32& v) { BindParam(v); };
    	void Param_Level(int32&& v) { _level = std::move(v); BindParam(_level); };
    	void Param_Maxhp(int32& v) { BindParam(v); };
    	void Param_Maxhp(int32&& v) { _maxhp = std::move(v); BindParam(_maxhp); };
    	void Param_Speed(float& v) { BindParam(v); };
    	void Param_Speed(float&& v) { _speed = std::move(v); BindParam(_speed); };
    	void Param_Totalexp(int32& v) { BindParam(v); };
    	void Param_Totalexp(int32&& v) { _totalexp = std::move(v); BindParam(_totalexp); };
    	void Param_Objectid(int32& v) { BindParam(v); };
    	void Param_Objectid(int32&& v) { _objectid = std::move(v); BindParam(_objectid); };

    private:
    	int32 _accountid = {};
    	int32 _attack = {};
    	int32 _hp = {};
    	int32 _level = {};
    	int32 _maxhp = {};
    	float _speed = {};
    	int32 _totalexp = {};
    	int32 _objectid = {};
    };

    /*---------------------------------------------------------------------------------------------
    			{proc.name} PROCEUDRE (Param : 1 Column : 1)
    ----------------------------------------------------------------------------------------------*/
    class FindPlayer : public DBBind<1,1>
    {
    public:
    	FindPlayer(DBConnection& conn) : DBBind(conn, L"{CALL dbo.ProcFindPlayer(?)}") { }
    	template<int32 N> void Param_Name(WCHAR(&v)[N]) { BindParam(v); };
    	template<int32 N> void Param_Name(const WCHAR(&v)[N]) { BindParam(v); };
    	void Param_Name(WCHAR* v, int32 count) { BindParam(v, count); };
    	void Param_Name(const WCHAR* v, int32 count) { BindParam(v, count); };
    	template<int32 N> void Column_Playername(OUT WCHAR(&v)[N]) { BindCol(v); };

    private:
    };


     
};