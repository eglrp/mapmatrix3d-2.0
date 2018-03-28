#pragma once
#include "OSG_Plus.h"
#include "publicDefined.h"
#include "Geometry.h"

class COsgSelectTriangleByPolygonFinder : public osg::NodeVisitor
{
public:
	COsgSelectTriangleByPolygonFinder(std::vector<CScope> VecScope, bool IsInside) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{
		vecScope = VecScope;
		isInside = IsInside;
	}

public:
	typedef std::map<double, osg::Vec3d> MAP_VERTS;

	MAP_VERTS m_MapInsideVerts;

protected:
	virtual void apply(osg::Geode &geode);

	virtual bool ApplyWithTriangle(osg::Geometry* geom, osg::Matrix worldMatrix, std::vector<CScope> vecScope);

	virtual bool IsTriangleInPolygon(osg::Vec3d pt1, osg::Vec3d pt2, osg::Vec3d pt3, double minH, double maxH, std::vector<CL::Vec2d> pol);

	virtual bool FindTriangleInPolygon(osg::Geometry* geom, osg::Vec3Array* verts, osg::ref_ptr<osg::DrawElements> drawElementTriangle, osg::Matrix worldMatrix, std::vector<CScope> vecScope);

	virtual osg::Matrix CalculateNodeMatrix(osg::Geode* geode);

protected:
	std::vector<CScope> vecScope;

	bool isInside;
};

