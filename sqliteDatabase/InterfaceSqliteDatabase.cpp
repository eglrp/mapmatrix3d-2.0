#include "stdafx.h"
#include "InterfaceSqliteDatabase.h"
#include "SqliteDatabase.h"

InterfaceSqliteDatabase* InterfaceSqliteDatabaseFactory::create()
{
	return new CSqliteDatabase();
}