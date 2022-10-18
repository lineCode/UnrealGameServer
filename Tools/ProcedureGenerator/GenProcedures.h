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
    	CreateAccount(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spCreateAccount(?)}") { }
    	template<int32 N> void In_Name1(WCHAR(&v)[N]) { BindParam(0, v); };
    	template<int32 N> void In_Name1(const WCHAR(&v)[N]) { BindParam(0, v); };
    	void In_Name1(WCHAR* v, int32 count) { BindParam(0, v, count); };
    	void In_Name1(const WCHAR* v, int32 count) { BindParam(0, v, count); };

    private:
    };

    /*---------------------------------------------------------------------------------------------
    			{proc.name} PROCEUDRE (Param : 1 Column : 1)
    ----------------------------------------------------------------------------------------------*/
    class FindAccountid : public DBBind<1,1>
    {
    public:
    	FindAccountid(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spFindAccountid(?)}") { }
    	template<int32 N> void In_Name1(WCHAR(&v)[N]) { BindParam(0, v); };
    	template<int32 N> void In_Name1(const WCHAR(&v)[N]) { BindParam(0, v); };
    	void In_Name1(WCHAR* v, int32 count) { BindParam(0, v, count); };
    	void In_Name1(const WCHAR* v, int32 count) { BindParam(0, v, count); };
    	void Out_Accountid(OUT int32& v) { BindCol(0, v); };

    private:
    };

    /*---------------------------------------------------------------------------------------------
    			{proc.name} PROCEUDRE (Param : 1 Column : 10)
    ----------------------------------------------------------------------------------------------*/
    class FindAccountPlayer : public DBBind<1,10>
    {
    public:
    	FindAccountPlayer(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spFindAccountPlayer(?)}") { }
    	template<int32 N> void In_Name1(WCHAR(&v)[N]) { BindParam(0, v); };
    	template<int32 N> void In_Name1(const WCHAR(&v)[N]) { BindParam(0, v); };
    	void In_Name1(WCHAR* v, int32 count) { BindParam(0, v, count); };
    	void In_Name1(const WCHAR* v, int32 count) { BindParam(0, v, count); };
    	void Out_Playerid(OUT int32& v) { BindCol(0, v); };
    	void Out_AccountId(OUT int32& v) { BindCol(1, v); };
    	template<int32 N> void Out_Playername(OUT WCHAR(&v)[N]) { BindCol(2, v); };
    	void Out_Objectid(OUT int32& v) { BindCol(3, v); };
    	void Out_Attack(OUT int32& v) { BindCol(4, v); };
    	void Out_Hp(OUT int32& v) { BindCol(5, v); };
    	void Out_Level(OUT int32& v) { BindCol(6, v); };
    	void Out_MaxHp(OUT int32& v) { BindCol(7, v); };
    	void Out_Speed(OUT int32& v) { BindCol(8, v); };
    	void Out_TotalExp(OUT int32& v) { BindCol(9, v); };

    private:
    };

    /*---------------------------------------------------------------------------------------------
    			{proc.name} PROCEUDRE (Param : 9 Column : 0)
    ----------------------------------------------------------------------------------------------*/
    class PlayerCreate : public DBBind<9,0>
    {
    public:
    	PlayerCreate(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spPlayerCreate(?,?,?,?,?,?,?,?,?)}") { }
    	template<int32 N> void In_Name1(WCHAR(&v)[N]) { BindParam(0, v); };
    	template<int32 N> void In_Name1(const WCHAR(&v)[N]) { BindParam(0, v); };
    	void In_Name1(WCHAR* v, int32 count) { BindParam(0, v, count); };
    	void In_Name1(const WCHAR* v, int32 count) { BindParam(0, v, count); };
    	void In_Accountid2(int32& v) { BindParam(1, v); };
    	void In_Accountid2(int32&& v) { _accountid2 = std::move(v); BindParam(1, _accountid2); };
    	void In_Attack3(int32& v) { BindParam(2, v); };
    	void In_Attack3(int32&& v) { _attack3 = std::move(v); BindParam(2, _attack3); };
    	void In_Hp4(int32& v) { BindParam(3, v); };
    	void In_Hp4(int32&& v) { _hp4 = std::move(v); BindParam(3, _hp4); };
    	void In_Level5(int32& v) { BindParam(4, v); };
    	void In_Level5(int32&& v) { _level5 = std::move(v); BindParam(4, _level5); };
    	void In_Maxhp6(int32& v) { BindParam(5, v); };
    	void In_Maxhp6(int32&& v) { _maxhp6 = std::move(v); BindParam(5, _maxhp6); };
    	void In_Speed7(int32& v) { BindParam(6, v); };
    	void In_Speed7(int32&& v) { _speed7 = std::move(v); BindParam(6, _speed7); };
    	void In_Totalexp8(int32& v) { BindParam(7, v); };
    	void In_Totalexp8(int32&& v) { _totalexp8 = std::move(v); BindParam(7, _totalexp8); };
    	void In_Objectid9(int32& v) { BindParam(8, v); };
    	void In_Objectid9(int32&& v) { _objectid9 = std::move(v); BindParam(8, _objectid9); };

    private:
    	int32 _accountid2 = {};
    	int32 _attack3 = {};
    	int32 _hp4 = {};
    	int32 _level5 = {};
    	int32 _maxhp6 = {};
    	int32 _speed7 = {};
    	int32 _totalexp8 = {};
    	int32 _objectid9 = {};
    };

    /*---------------------------------------------------------------------------------------------
    			{proc.name} PROCEUDRE (Param : 1 Column : 1)
    ----------------------------------------------------------------------------------------------*/
    class FindPlayerid : public DBBind<1,1>
    {
    public:
    	FindPlayerid(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spFindPlayerid(?)}") { }
    	template<int32 N> void In_Name1(WCHAR(&v)[N]) { BindParam(0, v); };
    	template<int32 N> void In_Name1(const WCHAR(&v)[N]) { BindParam(0, v); };
    	void In_Name1(WCHAR* v, int32 count) { BindParam(0, v, count); };
    	void In_Name1(const WCHAR* v, int32 count) { BindParam(0, v, count); };
    	void Out_Playerid(OUT int32& v) { BindCol(0, v); };

    private:
    };

    /*---------------------------------------------------------------------------------------------
    			{proc.name} PROCEUDRE (Param : 2 Column : 0)
    ----------------------------------------------------------------------------------------------*/
    class PlayerChangeHp : public DBBind<2,0>
    {
    public:
    	PlayerChangeHp(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spPlayerChangeHp(?,?)}") { }
    	void In_Hp1(int32& v) { BindParam(0, v); };
    	void In_Hp1(int32&& v) { _hp1 = std::move(v); BindParam(0, _hp1); };
    	void In_Playerid2(int32& v) { BindParam(1, v); };
    	void In_Playerid2(int32&& v) { _playerid2 = std::move(v); BindParam(1, _playerid2); };

    private:
    	int32 _hp1 = {};
    	int32 _playerid2 = {};
    };


     
};