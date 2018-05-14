#pragma once
#include "MultiScopeHeadGeodeClipper.h"

class CMultiScopeHeadGeodeNormalizeFlatter : public CMultiScopeHeadGeodeClipper
{
public:
	CMultiScopeHeadGeodeNormalizeFlatter(std::vector<CScope> VecScope, osg::Matrix MATRIX, double SceneMaxHeight, double FlattenHeight) 
	: CMultiScopeHeadGeodeClipper(VecScope, MATRIX, true)
	{
		vecScope = VecScope;
		matrix = MATRIX;
		sceneMaxHeight = SceneMaxHeight;
		flattenHeight = FlattenHeight;
	}

protected:
	virtual bool ApplyWithTriangle(osg::Geometry* geom);

private:
	std::vector<CScope> vecScope;
	osg::Matrix matrix;
	double sceneMaxHeight;
	double flattenHeight;

};

