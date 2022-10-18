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
    			  CreateAccount (Param : 1 Column : 0)
    ----------------------------------------------------------------------------------------------*/
    class CreateAccount : public DBBind<1,0>
    {
    public:
    	CreateAccount(DBConnection& conn) : DBBind(conn, L"{CALL dbo.ProcCreateAccount(?)}") { }
    	template<int32 N> void In_Name(WCHAR(&v)[N]) { BindParam(0, v); };
    	template<int32 N> void In_Name(const WCHAR(&v)[N]) { BindParam(0, v); };
    	void Param_Name(WCHAR* v, int32 count) { BindParam(0, v, count); };
    	void Param_Name(const WCHAR* v, int32 count) { BindParam(0, v, count); };

    private:
    };

    /*---------------------------------------------------------------------------------------------
    			  FindAccountid (Param : 1 Column : 1)
    ----------------------------------------------------------------------------------------------*/
    class FindAccountid : public DBBind<1,1>
    {
    public:
    	FindAccountid(DBConnection& conn) : DBBind(conn, L"{CALL dbo.ProcFindAccountid(?)}") { }
    	template<int32 N> void In_Name(WCHAR(&v)[N]) { BindParam(0, v); };
    	template<int32 N> void In_Name(const WCHAR(&v)[N]) { BindParam(0, v); };
    	void Param_Name(WCHAR* v, int32 count) { BindParam(0, v, count); };
    	void Param_Name(const WCHAR* v, int32 count) { BindParam(0, v, count); };
    	void Column_Accountid(OUT int32& v) { BindCol(0, v); };

    private:
    };

    /*---------------------------------------------------------------------------------------------
    			  FindAccountPlayer (Param : 1 Column : 10)
    ----------------------------------------------------------------------------------------------*/
    class FindAccountPlayer : public DBBind<1,10>
    {
    public:
    	FindAccountPlayer(DBConnection& conn) : DBBind(conn, L"{CALL dbo.ProcFindAccountPlayer(?)}") { }
    	template<int32 N> void In_Name(WCHAR(&v)[N]) { BindParam(0, v); };
    	template<int32 N> void In_Name(const WCHAR(&v)[N]) { BindParam(0, v); };
    	void Param_Name(WCHAR* v, int32 count) { BindParam(0, v, count); };
    	void Param_Name(const WCHAR* v, int32 count) { BindParam(0, v, count); };
    	void Column_Playerid(OUT int32& v) { BindCol(0, v); };
    	void Column_AccountId(OUT int32& v) { BindCol(1, v); };
    	template<int32 N> void Column_Playername(OUT WCHAR(&v)[N]) { BindCol(2, v); };
    	void Column_Objectid(OUT int32& v) { BindCol(3, v); };
    	void Column_Attack(OUT int32& v) { BindCol(4, v); };
    	void Column_Hp(OUT int32& v) { BindCol(5, v); };
    	void Column_Level(OUT int32& v) { BindCol(6, v); };
    	void Column_MaxHp(OUT int32& v) { BindCol(7, v); };
    	void Column_Speed(OUT int32& v) { BindCol(8, v); };
    	void Column_TotalExp(OUT int32& v) { BindCol(9, v); };

    private:
    };

    /*---------------------------------------------------------------------------------------------
    			  PlayerCreate (Param : 9 Column : 0)
    ----------------------------------------------------------------------------------------------*/
    class PlayerCreate : public DBBind<9,0>
    {
    public:
    	PlayerCreate(DBConnection& conn) : DBBind(conn, L"{CALL dbo.ProcPlayerCreate(?,?,?,?,?,?,?,?,?)}") { }
    	template<int32 N> void In_Name(WCHAR(&v)[N]) { BindParam(0, v); };
    	template<int32 N> void In_Name(const WCHAR(&v)[N]) { BindParam(0, v); };
    	void Param_Name(WCHAR* v, int32 count) { BindParam(0, v, count); };
    	void Param_Name(const WCHAR* v, int32 count) { BindParam(0, v, count); };
    	void Param_Accountid(int32& v) { BindParam(1, v); };
    	void Param_Accountid(int32&& v) { _accountid = std::move(v); BindParam(1, _accountid); };
    	void Param_Attack(int32& v) { BindParam(2, v); };
    	void Param_Attack(int32&& v) { _attack = std::move(v); BindParam(2, _attack); };
    	void Param_Hp(int32& v) { BindParam(3, v); };
    	void Param_Hp(int32&& v) { _hp = std::move(v); BindParam(3, _hp); };
    	void Param_Level(int32& v) { BindParam(4, v); };
    	void Param_Level(int32&& v) { _level = std::move(v); BindParam(4, _level); };
    	void Param_Maxhp(int32& v) { BindParam(5, v); };
    	void Param_Maxhp(int32&& v) { _maxhp = std::move(v); BindParam(5, _maxhp); };
    	void Param_Speed(int32& v) { BindParam(6, v); };
    	void Param_Speed(int32&& v) { _speed = std::move(v); BindParam(6, _speed); };
    	void Param_Totalexp(int32& v) { BindParam(7, v); };
    	void Param_Totalexp(int32&& v) { _totalexp = std::move(v); BindParam(7, _totalexp); };
    	void Param_Objectid(int32& v) { BindParam(8, v); };
    	void Param_Objectid(int32&& v) { _objectid = std::move(v); BindParam(8, _objectid); };

    private:
    	int32 _accountid = {};
    	int32 _attack = {};
    	int32 _hp = {};
    	int32 _level = {};
    	int32 _maxhp = {};
    	int32 _speed = {};
    	int32 _totalexp = {};
    	int32 _objectid = {};
    };

    /*---------------------------------------------------------------------------------------------
    			  FindPlayerid (Param : 1 Column : 1)
    ----------------------------------------------------------------------------------------------*/
    class FindPlayerid : public DBBind<1,1>
    {
    public:
    	FindPlayerid(DBConnection& conn) : DBBind(conn, L"{CALL dbo.ProcFindPlayerid(?)}") { }
    	template<int32 N> void In_Name(WCHAR(&v)[N]) { BindParam(0, v); };
    	template<int32 N> void In_Name(const WCHAR(&v)[N]) { BindParam(0, v); };
    	void Param_Name(WCHAR* v, int32 count) { BindParam(0, v, count); };
    	void Param_Name(const WCHAR* v, int32 count) { BindParam(0, v, count); };
    	void Column_Playerid(OUT int32& v) { BindCol(0, v); };

    private:
    };

    /*---------------------------------------------------------------------------------------------
    			  PlayerChangeHp (Param : 2 Column : 0)
    ----------------------------------------------------------------------------------------------*/
    class PlayerChangeHp : public DBBind<2,0>
    {
    public:
    	PlayerChangeHp(DBConnection& conn) : DBBind(conn, L"{CALL dbo.ProcPlayerChangeHp(?,?)}") { }
    	void Param_Hp(int32& v) { BindParam(0, v); };
    	void Param_Hp(int32&& v) { _hp = std::move(v); BindParam(0, _hp); };
    	void Param_Playerid(int32& v) { BindParam(1, v); };
    	void Param_Playerid(int32&& v) { _playerid = std::move(v); BindParam(1, _playerid); };

    private:
    	int32 _hp = {};
    	int32 _playerid = {};
    };


     
};