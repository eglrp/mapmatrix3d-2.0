#pragma once
#include <string>
#include "OSG_Plus.h"

class CSceneStitcher
{
public:
	void SceneStitch(osg::ref_ptr<osg::Group> sceneGroup, std::vector<std::string> vecSceneFileName, 
		osg::ref_ptr<osg::Group> &combineGroup);															//场景拼接

	void AdjustWholeSceneFlatPlaneXY(osg::ref_ptr<osg::Node> selectNode, double xMove, double yMove);	//平面场景调整							

	void AdjustWholeSceneHeight(osg::ref_ptr<osg::Node> selectNode, double zMove);						//高度场景调整

	bool IsPathCorrect(std::vector<std::string> vecSceneFileName);										//是否满足场景拼接路径要求

	void SceneSplit(osg::ref_ptr<osg::Node> sceneNode, std::string inFileName);							//场景拆分

private:
	std::string ObtainFolderName(std::string inFileName);												//获取目录名

	void ObtainDatabasePath(std::string inFileName, std::string &outFileName);							//获取数据路径
};

