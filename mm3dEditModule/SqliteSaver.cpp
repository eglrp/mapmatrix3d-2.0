#include "stdafx.h"
#include "SqliteSaver.h"

void CSqliteSaver::saveFlattenHeight(double flattenHeight)
{
	//创建database和相应的table
	InterfaceSqliteDatabaseFactory iSqliteDatabaseFactory;
	InterfaceSqliteDatabase* iSqliteDatabase = iSqliteDatabaseFactory.create();
	//保存采样点信息
	sqlite3* db = iSqliteDatabase->OpenDatabase("flatDatabase.db");
	iSqliteDatabase->create_Table(db, "create Table flattenHeight(fHeight);");
	iSqliteDatabase->ClearAllRecord(db, "delete from flattenHeight");
	iSqliteDatabase->StartBegin(db);

	char insertSql[99];
	strcpy_s(insertSql, "insert into flattenHeight(fHeight)values('%lf')");
	char* InsertSql = sqlite3_mprintf(insertSql, flattenHeight);
	iSqliteDatabase->InsertRecord(db, InsertSql);
	iSqliteDatabase->Commit(db);
	iSqliteDatabase->CloseDatabase(db);
}

void CSqliteSaver::savePreFlattenHeight(double preFlattenHeight, double preSceneMaxHeight)
{
	//创建database和相应的table
	InterfaceSqliteDatabaseFactory iSqliteDatabaseFactory;
	InterfaceSqliteDatabase* iSqliteDatabase = iSqliteDatabaseFactory.create();
	//保存采样点信息
	sqlite3* db = iSqliteDatabase->OpenDatabase("flatDatabase.db");
	iSqliteDatabase->create_Table(db, "create Table preFlattenHeight(fH, mH);");
	iSqliteDatabase->ClearAllRecord(db, "delete from preFlattenHeight");
	iSqliteDatabase->StartBegin(db);

	char insertSql[99];
	strcpy_s(insertSql, "insert into preFlattenHeight(fH, mH)values('%lf', '%lf')");
	char* InsertSql = sqlite3_mprintf(insertSql, preFlattenHeight, preSceneMaxHeight);
	iSqliteDatabase->InsertRecord(db, InsertSql);
	iSqliteDatabase->Commit(db);
	iSqliteDatabase->CloseDatabase(db);
}

void CSqliteSaver::readPreFlattenHeight()
{
	InterfaceSqliteDatabaseFactory iSqliteDatabaseFactory;
	InterfaceSqliteDatabase* iSqliteDatabase = iSqliteDatabaseFactory.create();
	sqlite3* db = iSqliteDatabase->OpenDatabase("flatDatabase.db");
	char *errmsg = NULL;
	const char* strSql1 = "select * from preFlattenHeight";
	char **dbResult = NULL;
	int nRow;
	int nCol;
	int result = sqlite3_get_table(db, strSql1, &dbResult, &nRow, &nCol, &errmsg);

	if (nRow < 1)
	{
		return;
	}

	preFlattenHeight = atof(dbResult[0]);
	preSceneMaxHeight = atof(dbResult[1]);

}

double CSqliteSaver::getPreFlattenHeight() const
{ 
	return preFlattenHeight; 
}

double CSqliteSaver::getPreSceneMaxHeight() const
{
	return preFlattenHeight;
}

