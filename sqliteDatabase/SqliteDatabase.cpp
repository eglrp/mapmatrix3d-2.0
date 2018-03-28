#include "stdafx.h"
#include "SqliteDatabase.h"


sqlite3 * CSqliteDatabase::OpenDatabase(char* dataBaseFileName)
{
	sqlite3 *pDB = NULL;
	int result = sqlite3_open(dataBaseFileName, &pDB);
	return pDB;
}

void CSqliteDatabase::ClearAllRecord(sqlite3 *pDB, char* deleteSql)
{
	sqlite3_exec(pDB, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	sqlite3_exec(pDB, deleteSql, NULL, NULL, NULL);
	sqlite3_exec(pDB, "COMMIT TRANSACTION;", NULL, NULL, NULL);
}

void CSqliteDatabase::create_Table(sqlite3 *pDB, char* createTableSql)
{
	int result = sqlite3_exec(pDB, createTableSql, 0, 0, NULL);
}

void CSqliteDatabase::SelectFromTable(sqlite3 *pDB, char* selectFromTableSql, char **dbResult, int &nRow, int &nCol)
{
	char *errmsg = NULL;
	int result = sqlite3_get_table(pDB, selectFromTableSql, &dbResult, &nRow, &nCol, &errmsg);
}

void CSqliteDatabase::InsertRecord(sqlite3 *pDB, char* insertTableRecord)
{
	int result = sqlite3_exec(pDB, insertTableRecord, 0, 0, NULL);
}

void CSqliteDatabase::StartBegin(sqlite3 *pDB)
{
	sqlite3_exec(pDB, "begin;", 0, 0, 0);
}

void CSqliteDatabase::Commit(sqlite3 *pDB)
{
	sqlite3_exec(pDB, "commit;", 0, 0, 0);
}

void CSqliteDatabase::CloseDatabase(sqlite3 *pDB)
{
	sqlite3_close(pDB);
}