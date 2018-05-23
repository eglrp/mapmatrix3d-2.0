#pragma once
#include "MultiScopeHeadGeodeClipper.h"

class CMultiScopeHeadGeodeFlatRecoverer : public CMultiScopeHeadGeodeClipper

{
public:
	CMultiScopeHeadGeodeFlatRecoverer(std::vector<CScope> VecScope, std::vector<CScope> VecPreFlatArea, osg::Matrix MATRIX, double SceneMaxHeight, double FlattenHeight) 
		: CMultiScopeHeadGeodeClipper(VecScope, MATRIX, true)
	{
		vecScope = VecScope;
		matrix = MATRIX;
		flattenHeight = FlattenHeight;
		sceneMaxHeight = SceneMaxHeight;
		vecPreFlatArea = VecPreFlatArea;
	}

protected:
	virtual bool ApplyWithTriangle(osg::Geometry* geom);

private:
	double sceneMaxHeight;
	double flattenHeight;
	std::vector<CScope> vecScope;
	std::vector<CScope> vecPreFlatArea;
	osg::Matrix matrix;

};

