#pragma once
#include "MultiScopeHeadGeodeClipper.h"


class CMultiScopeHeadGeodeflatter : public CMultiScopeHeadGeodeClipper
{
public:
	CMultiScopeHeadGeodeflatter(std::vector<CScope> VecScope, osg::Matrix MATRIX, double FlattenHeight) : CMultiScopeHeadGeodeClipper(VecScope, MATRIX, true)
	{
		vecScope = VecScope;
		matrix = MATRIX;
		flattenHeight = FlattenHeight;
	}

protected:
	virtual bool ApplyWithTriangle(osg::Geometry* geom);

private:
	double flattenHeight;
	std::vector<CScope> vecScope;
	osg::Matrix matrix;

};

