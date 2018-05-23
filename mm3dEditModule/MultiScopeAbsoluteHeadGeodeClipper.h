#pragma once
#include <string>
#include "OSG_Plus.h"
#include <direct.h>
#include "qcomm.h"
#include "publicDefined.h"
#include "Geometry.h"
#include "MultiScopeHeadGeodeClipper.h"

class CMultiScopeAbsoluteHeadGeodeClipper : public CMultiScopeHeadGeodeClipper
{
public:
	CMultiScopeAbsoluteHeadGeodeClipper(std::vector<CScope> VecScope, osg::Matrix MATRIX, bool IsInside) : CMultiScopeHeadGeodeClipper(VecScope, MATRIX, IsInside)
	{
		vecScope = VecScope;
		matrix = MATRIX;
		isInside = IsInside;
	}

protected:
	virtual bool ApplyWithTriangle(osg::Geometry* geom);

private:
	std::vector<CScope> vecScope;
	osg::Matrix matrix;
	bool isInside;
};

