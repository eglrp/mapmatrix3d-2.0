#pragma once
#include "sqlite3.h"

#ifdef SQLITEDATABASELIBDLL  
#define SQLITEDATABASEAPI _declspec(dllexport)  
#else  
#define SQLITEDATABASEAPI  _declspec(dllimport)  
#endif  

class SQLITEDATABASEAPI InterfaceSqliteDatabase
{
public:
	virtual sqlite3* OpenDatabase(char* dataBaseFileName) = 0;

	virtual void InsertRecord(sqlite3 *pDB, char* insertTableRecord) = 0;

	virtual void StartBegin(sqlite3 *pDB) = 0;

	virtual void Commit(sqlite3 *pDB) = 0;

	virtual void ClearAllRecord(sqlite3 *pDB, char* deleteSql) = 0;

	virtual void create_Table(sqlite3 *pDB, char* createTableSql) = 0;

	virtual void SelectFromTable(sqlite3 *pDB, char* selectFromTableSql, char **dbResult, int &nRow, int &nCol) = 0; 

	virtual void CloseDatabase(sqlite3 *pDB) = 0;
};

class SQLITEDATABASEAPI InterfaceSqliteDatabaseFactory
{
public:
	InterfaceSqliteDatabase* create();
};

