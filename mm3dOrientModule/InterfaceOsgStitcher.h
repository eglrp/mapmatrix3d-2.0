#pragma once
#include "OSG_Plus.h"

#ifdef OSGSTITCHLIBDLL  
#define OSGSTITCHAPI _declspec(dllexport)  
#else  
#define OSGSTITCHAPI  _declspec(dllimport)  
#endif  


class OSGSTITCHAPI InterfaceOsgStitcher
{
public:
	virtual void Stitch(osg::ref_ptr<osg::Group> sceneGroup, std::vector<std::string> vecSceneFileName, osg::ref_ptr<osg::Group> &combineGroup) = 0; //场景拼接

	virtual bool IsPathCorrect(std::vector<std::string> vecSceneFileName) = 0;																		 //路径是否正确

	virtual void SceneSplit(osg::ref_ptr<osg::Node> sceneNode, std::string inFileName) = 0;															 //场景分离

	virtual void MoveWholeSceneXY(osg::ref_ptr<osg::Node> selectNode, double xMove, double yMove) = 0;												 //平面移动场景

	virtual void MoveWholeSceneZ(osg::ref_ptr<osg::Node> selectNode, double zMove) = 0;																 //垂直移动场景
};

class OSGSTITCHAPI InterfaceOsgStitcherFactory
{
public:
	InterfaceOsgStitcher* create();
};


