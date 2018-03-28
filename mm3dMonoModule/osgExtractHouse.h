#pragma once
#include "OSG_Plus.h"
#include "InterfaceUserEventCallBack.h"
#include "TriangleRelation.h"
#include "InterfaceOsgExtractHouse.h"
#include "IsInScope.h"
#include "ObjectAdder.h"

class COsgExtractHouse : public InterfaceOsgExtractHouse
{
public:
	virtual void extractHouse(osg::ref_ptr<osg::Node> sceneNode, std::vector<osg::Vec3d> vecWorldPt);

protected:
	virtual void StartExtractHouse(osg::ref_ptr<osg::Node> node, osg::Matrix worldMatrix, double userDefineGroundHeight);

	virtual void searchPoints(CTriangle& triangle, std::vector<osg::Vec3d>& vecPoints, std::vector<CTriangleRelation>& vecTrangleRelation, double userDefineGroundHeight);

	virtual void searchGreen(CTriangle& triangle, std::vector<osg::Vec3d>& vecPoints, std::vector<CTriangleRelation>& vecTrangleRelation);

	virtual void clipObject(osg::ref_ptr<osg::Node> node, osg::Matrix worldMatrix, std::vector<osg::Vec3d>& vecPoints);

	virtual void clipObjectByHeight(osg::ref_ptr<osg::Node> node, osg::Matrix worldMatrix, double height);

protected:
	inline double MinZ(osg::Vec3d& pt1, osg::Vec3d& pt2, osg::Vec3d& pt3)
	{
		double v = pt1.z() < pt2.z() ? pt1.z() : pt2.z();
		return v < pt3.z() ? v : pt3.z();
	}

	osg::Vec3d normalVector(osg::Vec3d a, osg::Vec3d b, osg::Vec3d c);

protected:
	CIsInScope isInScope;

	CObjectAdder objectAdder;
};