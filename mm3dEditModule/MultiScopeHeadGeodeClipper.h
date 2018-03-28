#pragma once
#include <string>
#include "OSG_Plus.h"
#include <direct.h>
#include "qcomm.h"
#include "publicDefined.h"
#include "Geometry.h"
#include "InterfaceOsgClip.h"
#include "IsInAreaJudger.h"

class CMultiScopeHeadGeodeClipper : public osg::NodeVisitor
{
public:
	CMultiScopeHeadGeodeClipper(std::vector<CScope> VecScope, osg::Matrix MATRIX, bool IsInside) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN),
		isInAreaJudger(VecScope)
	{
		vecScope = VecScope;
		matrix = MATRIX;
		isInside = IsInside;
	}


protected:
	virtual void apply(osg::Geode& geode);

	virtual bool ApplyWithTriangle(osg::Geometry* geom);

	virtual bool ApplyWithGeode(osg::Geode* geode, int j);

private:
	std::vector<CScope> vecScope;
	osg::Matrix matrix;	
	bool isInside;

protected:
	CIsInAreaJudger isInAreaJudger;

};

