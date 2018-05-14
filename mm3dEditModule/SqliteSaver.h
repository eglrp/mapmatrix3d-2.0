#pragma once
#include "InterfaceSqliteDatabase.h"
#include "sqlite3.h"
#include <iostream>

class CSqliteSaver
{
public:
	virtual void saveFlattenHeight(double flattenHeight);

	virtual void savePreFlattenHeight(double preFlattenHeight, double preSceneMaxHeight);

	virtual	void readPreFlattenHeight();

	virtual double getPreFlattenHeight() const;

	virtual double getPreSceneMaxHeight() const;

private:
	double preFlattenHeight;
	double preSceneMaxHeight;
};

