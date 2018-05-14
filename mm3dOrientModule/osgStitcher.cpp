// osgStitcher.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgStitcher.h"
#include "SceneStitcher.h"


void COsgStitcher::Stitch(osg::ref_ptr<osg::Group> sceneGroup, std::vector<std::string> vecSceneFileName, osg::ref_ptr<osg::Group> &combineGroup)
{
	//场景拼接
	CSceneStitcher stitcher;
	stitcher.SceneStitch(sceneGroup, vecSceneFileName, combineGroup);
}

bool COsgStitcher::IsPathCorrect(std::vector<std::string> vecSceneFileName)
{
	CSceneStitcher stitcher;
	return stitcher.IsPathCorrect(vecSceneFileName);
}

void COsgStitcher::SceneSplit(osg::ref_ptr<osg::Node> sceneNode, std::string inFileName)
{
	CSceneStitcher stitcher;
	stitcher.SceneSplit(sceneNode, inFileName);
}

void COsgStitcher::MoveWholeSceneXY(osg::ref_ptr<osg::Node> selectNode, double xMove, double yMove)
{
	CSceneStitcher stitcher;
	stitcher.AdjustWholeSceneFlatPlaneXY(selectNode, xMove, yMove);
}

void COsgStitcher::MoveWholeSceneZ(osg::ref_ptr<osg::Node> selectNode, double zMove)
{
	CSceneStitcher stitcher;
	stitcher.AdjustWholeSceneHeight(selectNode, zMove);
}