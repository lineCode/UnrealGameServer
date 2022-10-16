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
    			     	          CreateAccount(1,0)
    ----------------------------------------------------------------------------------------------*/
    class CreateAccount : public DBBind<1,0>
    {
    public:
    	CreateAccount(DBConnection& conn) : DBBind(conn, L"{CALL dbo.ProcCreateAccount(?)}") { }
    	template<int32 N> void Param_Name1(WCHAR(&v)[N]) { BindParam(v); };
    	template<int32 N> void Param_Name1(const WCHAR(&v)[N]) { BindParam(v); };
    	void Param_Name1(WCHAR* v, int32 count) { BindParam(v, count); };
    	void Param_Name1(const WCHAR* v, int32 count) { BindParam(v, count); };

    private:
    };

    /*---------------------------------------------------------------------------------------------
    			     	          FindAccountid(1,1)
    ----------------------------------------------------------------------------------------------*/
    class FindAccountid : public DBBind<1,1>
    {
    public:
    	FindAccountid(DBConnection& conn) : DBBind(conn, L"{CALL dbo.ProcFindAccountid(?)}") { }
    	template<int32 N> void Param_Name1(WCHAR(&v)[N]) { BindParam(v); };
    	template<int32 N> void Param_Name1(const WCHAR(&v)[N]) { BindParam(v); };
    	void Param_Name1(WCHAR* v, int32 count) { BindParam(v, count); };
    	void Param_Name1(const WCHAR* v, int32 count) { BindParam(v, count); };
    	void Column_Accountid(OUT int32& v) { BindCol(v); };

    private:
    };

    /*---------------------------------------------------------------------------------------------
    			     	          FindAccountPlayer(1,10)
    ----------------------------------------------------------------------------------------------*/
    class FindAccountPlayer : public DBBind<1,10>
    {
    public:
    	FindAccountPlayer(DBConnection& conn) : DBBind(conn, L"{CALL dbo.ProcFindAccountPlayer(?)}") { }
    	template<int32 N> void Param_Name1(WCHAR(&v)[N]) { BindParam(v); };
    	template<int32 N> void Param_Name1(const WCHAR(&v)[N]) { BindParam(v); };
    	void Param_Name1(WCHAR* v, int32 count) { BindParam(v, count); };
    	void Param_Name1(const WCHAR* v, int32 count) { BindParam(v, count); };
    	void Column_Playerid(OUT int32& v) { BindCol(v); };
    	void Column_AccountId(OUT int32& v) { BindCol(v); };
    	template<int32 N> void Column_Playername(OUT WCHAR(&v)[N]) { BindCol(v); };
    	void Column_Objectid(OUT int32& v) { BindCol(v); };
    	void Column_Attack(OUT int32& v) { BindCol(v); };
    	void Column_Hp(OUT int32& v) { BindCol(v); };
    	void Column_Level(OUT int32& v) { BindCol(v); };
    	void Column_MaxHp(OUT int32& v) { BindCol(v); };
    	void Column_Speed(OUT int32& v) { BindCol(v); };
    	void Column_TotalExp(OUT int32& v) { BindCol(v); };

    private:
    };

    /*---------------------------------------------------------------------------------------------
    			     	          PlayerCreate(9,0)
    ----------------------------------------------------------------------------------------------*/
    class PlayerCreate : public DBBind<9,0>
    {
    public:
    	PlayerCreate(DBConnection& conn) : DBBind(conn, L"{CALL dbo.ProcPlayerCreate(?,?,?,?,?,?,?,?,?)}") { }
    	template<int32 N> void Param_Name1(WCHAR(&v)[N]) { BindParam(v); };
    	template<int32 N> void Param_Name1(const WCHAR(&v)[N]) { BindParam(v); };
    	void Param_Name1(WCHAR* v, int32 count) { BindParam(v, count); };
    	void Param_Name1(const WCHAR* v, int32 count) { BindParam(v, count); };
    	void Param_Accountid2(int32& v) { BindParam(v); };
    	void Param_Accountid2(int32&& v) { _accountid2 = std::move(v); BindParam(_accountid2); };
    	void Param_Attack3(int32& v) { BindParam(v); };
    	void Param_Attack3(int32&& v) { _attack3 = std::move(v); BindParam(_attack3); };
    	void Param_Hp4(int32& v) { BindParam(v); };
    	void Param_Hp4(int32&& v) { _hp4 = std::move(v); BindParam(_hp4); };
    	void Param_Level5(int32& v) { BindParam(v); };
    	void Param_Level5(int32&& v) { _level5 = std::move(v); BindParam(_level5); };
    	void Param_Maxhp6(int32& v) { BindParam(v); };
    	void Param_Maxhp6(int32&& v) { _maxhp6 = std::move(v); BindParam(_maxhp6); };
    	void Param_Speed7(float& v) { BindParam(v); };
    	void Param_Speed7(float&& v) { _speed7 = std::move(v); BindParam(_speed7); };
    	void Param_Totalexp8(int32& v) { BindParam(v); };
    	void Param_Totalexp8(int32&& v) { _totalexp8 = std::move(v); BindParam(_totalexp8); };
    	void Param_Objectid9(int32& v) { BindParam(v); };
    	void Param_Objectid9(int32&& v) { _objectid9 = std::move(v); BindParam(_objectid9); };

    private:
    	int32 _accountid2 = {};
    	int32 _attack3 = {};
    	int32 _hp4 = {};
    	int32 _level5 = {};
    	int32 _maxhp6 = {};
    	float _speed7 = {};
    	int32 _totalexp8 = {};
    	int32 _objectid9 = {};
    };

    /*---------------------------------------------------------------------------------------------
    			     	          FindPlayer(1,1)
    ----------------------------------------------------------------------------------------------*/
    class FindPlayer : public DBBind<1,1>
    {
    public:
    	FindPlayer(DBConnection& conn) : DBBind(conn, L"{CALL dbo.ProcFindPlayer(?)}") { }
    	template<int32 N> void Param_Name1(WCHAR(&v)[N]) { BindParam(v); };
    	template<int32 N> void Param_Name1(const WCHAR(&v)[N]) { BindParam(v); };
    	void Param_Name1(WCHAR* v, int32 count) { BindParam(v, count); };
    	void Param_Name1(const WCHAR* v, int32 count) { BindParam(v, count); };
    	template<int32 N> void Column_Playername(OUT WCHAR(&v)[N]) { BindCol(v); };

    private:
    };


     
};