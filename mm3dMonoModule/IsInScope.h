#pragma once
#include "OSG_Plus.h"
#include "TriangleRelation.h"

class CIsInScope
{
public:
	bool PtInTriangle(osg::Vec3d pt, osg::Vec3d& t1, osg::Vec3d& t2, osg::Vec3d& t3);

	bool PtInVector(osg::Vec3d& pt1, osg::Vec3d& pt2, osg::Vec3d& pt3, std::vector<osg::Vec3d> &vecPoints);

	bool higherThan(CTriangle& triangle, double h);																			//检查高度

	bool checkNomal(osg::Vec3d nom, osg::Vec3d normal, double err);															//检查法向量

	bool checkNormalForMono(osg::Vec3d nom, osg::Vec3d normal, double err);
};

