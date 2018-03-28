#pragma once
#include <string>
#include "OSG_Plus.h"
#include <direct.h>
#include "qcomm.h"
#include "publicDefined.h"
#include "Geometry.h"
#include "MultiScopeHeadGeodeClipper.h"

class CMultiChildScopeGeoClipper : public CMultiScopeHeadGeodeClipper
{
public:
	CMultiChildScopeGeoClipper(std::vector<CScope> VecScope, osg::Matrix MATRIX, bool IsInside) : CMultiScopeHeadGeodeClipper(VecScope, MATRIX, IsInside)
	{
		vecScope = VecScope;
		matrix = MATRIX;
		isInside = IsInside;
	}

protected:
	virtual osg::Matrix CalculateNodeMatrix(osg::Geode& geode) 
	{ 
		osg::Matrix matrix;
		return matrix;
	}

protected:
	std::vector<CScope> vecScope;
	osg::Matrix matrix;
	bool isInside;
};

