#pragma once
#include "sqlite3.h"
#include "InterfaceSqliteDatabase.h"

class CSqliteDatabase : public InterfaceSqliteDatabase
{
public:
	virtual sqlite3 * OpenDatabase(char* dataBaseFileName);

	virtual void InsertRecord(sqlite3 *pDB, char* insertTableRecord);

	virtual void StartBegin(sqlite3 *pDB);

	virtual void Commit(sqlite3 *pDB);

	virtual void ClearAllRecord(sqlite3 *pDB, char* deleteSql);

	virtual void create_Table(sqlite3 *pDB, char* createTableSql);	

	virtual void SelectFromTable(sqlite3 *pDB, char* selectFromTableSql, char **dbResult, int &nRow, int &nCol);

	virtual void CloseDatabase(sqlite3 *pDB);
};

