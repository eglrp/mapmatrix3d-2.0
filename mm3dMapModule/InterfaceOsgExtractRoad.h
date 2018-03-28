#pragma once
#include <vector>
#include <osg/Node>
#include <osg/Group>

#include "publicDefined.h"
#include "InterfaceUserEventCallBack.h"
#include "TriangleRelation.h"


#define EXTRACTROADLIBDLLAPI

#ifdef EXTRACTROADLIBDLLAPI  
#define EXTRACTROADLIBDLL _declspec(dllexport)  
#else  
#define EXTRACTROADLIBDLL  _declspec(dllimport)  
#endif  



class EXTRACTROADLIBDLL InterfaceOsgExtractRoad
{
public:
	virtual void toExtractRoad(osg::Group* mRoot, osg::Group* drawGroup, osg::NodePath vecNodePath, osg::Vec3d vecSearchPoint, std::vector<osg::Vec3d>* vecPtResult, std::vector<std::string>* vecFileNames, std::vector<CTriangleRelation>* vecTriangleRelation, std::vector<osg::Vec3d>* vecNormal, std::vector<osg::Vec4>* vecColor, osg::Matrix worldMatrix, InterfaceSigEventCallBack* ISigEventCallBack) = 0;

	virtual void toExtractRoadEdge(osg::Group* mRoot_, osg::Group* drawGroup_,  std::vector<osg::Vec3d>* vecPtResult, std::vector<osg::Vec3d>* vecEdge, double alpha, osg::Matrix worldMatrix_, InterfaceSigEventCallBack* ISigEventCallBack_) = 0;

	virtual void insertNodePath(osg::Group* mRoot, osg::Vec3d p1, osg::Vec3d p2, std::vector<osg::NodePath>& vecNodePath, std::vector<osg::Vec3d>& vecSearchPoint) = 0;
};

class EXTRACTROADLIBDLL InterfaceOsgExtractRoadFactory
{
public:
	InterfaceOsgExtractRoad* create();
};