pushd %~dp0

ProcedureAutoCreate.exe --path=../../Resource/DB/GameDB.json --output=ProcedureManager.h
IF ERRORLEVEL 1 PAUSE

XCOPY /Y ProcedureManager.h "../../GameServer"

DEL /Q /F *.h

PAUSE