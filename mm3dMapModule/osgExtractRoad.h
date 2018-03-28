#pragma once
#include "InterfaceOsgExtractRoad.h"

class EXTRACTROADLIBDLL COsgExtractRoad : public InterfaceOsgExtractRoad
{
public:
	void toExtractRoad(osg::Group* mRoot, osg::Group* drawGroup, osg::NodePath vecNodePath, osg::Vec3d vecSearchPoint, std::vector<osg::Vec3d>* vecPtResult, std::vector<std::string>* vecFileNames, std::vector<CTriangleRelation>* vecTriangleRelation, std::vector<osg::Vec3d>* vecNormal, std::vector<osg::Vec4>* vecColor, osg::Matrix worldMatrix, InterfaceSigEventCallBack* ISigEventCallBack) ;

	void toExtractRoadEdge(osg::Group* mRoot_, osg::Group* drawGroup_, std::vector<osg::Vec3d>* vecPtResult, std::vector<osg::Vec3d>* vecEdge, double alpha, osg::Matrix worldMatrix_, InterfaceSigEventCallBack* ISigEventCallBack_) ;
	
	void insertNodePath(osg::Group* mRoot, osg::Vec3d p1,osg::Vec3d p2,std::vector<osg::NodePath>& vecNodePath, std::vector<osg::Vec3d>& vecSearchPoint);

protected:

	static void extractRoad(void* ptr);

	static void extractRoadEdge(void* ptr);

	void StartExtractRoad();

	void StartExtractRoadEdge();

private:
	
	void ExtractRoad(osg::Group* mRoot, osg::NodePath vecNodePath, osg::Vec3d vecSearchPoint, std::vector<osg::Vec3d>* vecPtResult, std::vector<std::string>* vecFileNames, std::vector<CTriangleRelation>* vecTriangleRelation, std::vector<osg::Vec3d>* vecNormal_, std::vector<osg::Vec4>* vecColor_, osg::Matrix worldMatrix);

	void ExtractRoadEdge( std::vector<osg::Vec3d>* vecPtResult, std::vector<osg::Vec3d>* vecEdge, double alpha);

	void getTileNames(osg::NodePath vecNodePath, std::vector<std::string>& vecNames);

	void getPointsInPolygon(std::string& vecNames, std::vector<CTriangle>& vecPoints, osg::Matrix worldMatrix);

	void getOriTriangle(std::vector<std::string> & vecNames, std::vector<std::vector<CTriangle> >& vecPoints, osg::Matrix worldMatrix);

	void searchPoints(CTriangle& triangle, CTriangle& triangleFather, std::vector<osg::Vec3d>& vecPoints, std::vector<osg::Vec3d>& vecPointstop, std::vector<CTriangleRelation>& vecTrangleRelation, std::vector<osg::Vec3d>& vecNormal, std::vector<osg::Vec4>& vecColor);
	
	bool checkNomal(osg::Vec3d nom, osg::Vec3d normal, double err);
	
	void NormalVector(osg::Vec3d& a, osg::Vec3d& b, osg::Vec3d& c, osg::Vec3d& nomal);
	
	bool PtInTriangle(osg::Vec3d pt, osg::Vec3d& t1, osg::Vec3d& t2, osg::Vec3d& t3);

	void buildRelation(std::vector<CTriangle>& vecTriangle, std::vector<CTriangleRelation>& vecRelationTemp);

	void shampPointCloud(std::vector<osg::Vec3d>&vecPt, double r);

private:

	osg::Group* mRoot;

	osg::Group* drawGroup;

	osg::NodePath vecNodePath;

	osg::Vec3d vecSearchPoint;

	std::vector<osg::Vec3d>* vecPtResult;

	std::vector<osg::Vec3d>* vecPtEdge;

	std::vector<std::string>* vecFileNames;
	
	std::vector<CTriangleRelation>* vecTriangleRelation;

	std::vector<osg::Vec3d>* vecNormal;

	std::vector<osg::Vec4>* vecColor;

	InterfaceSigEventCallBack* ISigEventCallBack;
	
	osg::Matrix worldMatrix;

	double alpha;
};

