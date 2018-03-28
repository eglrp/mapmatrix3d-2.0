#pragma once
#include "InterfaceOsgStitcher.h"


class COsgStitcher : public InterfaceOsgStitcher
{
public:
	void Stitch(osg::ref_ptr<osg::Group> sceneGroup, std::vector<std::string> vecSceneFileName, osg::ref_ptr<osg::Group> &combineGroup); //场景拼接

	bool IsPathCorrect(std::vector<std::string> vecSceneFileName);																		 //路径是否正确

	void SceneSplit(osg::ref_ptr<osg::Node> sceneNode, std::string inFileName);															 //场景分离

	void MoveWholeSceneXY(osg::ref_ptr<osg::Node> selectNode, double xMove, double yMove);												 //平面移动场景

	void MoveWholeSceneZ(osg::ref_ptr<osg::Node> selectNode, double zMove);																 //垂直移动场景
};