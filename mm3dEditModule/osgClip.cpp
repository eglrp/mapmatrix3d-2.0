// osgClip.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgClip.h"
#include "qcomm.h"
#include "MultiScopeHeadGeodeClipper.h"
#include "MultiScopeAccurateHeadGeodeClippper.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceMFCExcute.h"
#include "Geometry.h"
#include "OsgPagedLodOutputer.h"
#include "OsgPagedLodReader.h"

bool COsgClipper::clipSceneByScope(osg::ref_ptr<osg::MatrixTransform> mTrans, std::string inFilePath, std::string outputFilePath, std::vector<CScope> vecScope,
	 CStProgressSetter stProgressSetter, bool isInside)
{
	osg::ref_ptr<osg::Group> mGroup = mTrans->getChild(0)->asGroup();

	if (!mGroup)
	{
		return false;
	}

	osg::Matrix worldMatrix = mTrans->getMatrix();
	CObtainSelTileFileName tileFileNameGetter(vecScope); 
	tileFileNameGetter.obtainSelTileName(mGroup, worldMatrix);							//获得tile名
	std::vector<std::string> vecHalfInPolygonFileName = tileFileNameGetter.getHalfInFileName();
	std::vector<std::string> vecTotalInPolygonFileName = tileFileNameGetter.getTotalInFileName();

	lodShower.ShowLogFinishSelectScope();
	//合并组
	COsgPagedLodReader reader(inFilePath);
	osg::ref_ptr<osg::Group> combineHalfInPolygonGroup = reader.combinePagedLod(vecHalfInPolygonFileName);						//将所有的tile合并
																									
	lodShower.ShowLogStartOutputHalfInScopeOsgb();
	
	int halfPercentInterval = 0;
	changePercentageBarInterval(halfPercentInterval, stProgressSetter.percentInterval, isInside);

	//对于不完全在范围内进行裁切
	clipHalfInPolygonTile(combineHalfInPolygonGroup, vecScope, inFilePath, outputFilePath, worldMatrix, stProgressSetter, isInside);

	lodShower.ShowLogFinishOutputHalfInScopeOsgb();

	if (isInside)
	{
		stProgressSetter.percentStart = stProgressSetter.percentStart + halfPercentInterval;
		stProgressSetter.percentInterval = stProgressSetter.percentInterval - halfPercentInterval;
		//对于完全在范围内的直接输出
		COsgPagedLodOutputer pagedLodOutput(outputFilePath);
		pagedLodOutput.OutputTotalInScopeOsgb(inFilePath, vecTotalInPolygonFileName, stProgressSetter);
	}
	else
	{
		COsgPagedLodOutputer pagedLodOutput(outputFilePath);
		pagedLodOutput.replacePagedLodClipFilePath(mTrans, inFilePath, vecHalfInPolygonFileName);
	}

	return true;
}


void COsgClipper::obtainSelTileName(std::vector<CScope> vecScope, osg::ref_ptr<osg::Group> mGroup, osg::Matrix matrix, std::vector<std::string> &vecHalfInPagedLodFileName,
	std::vector<std::string> &vecTotalInPagedLodFileName)
{
	CObtainSelTileFileName selTileFileNameGetter(vecScope);
	selTileFileNameGetter.obtainSelTileName(mGroup, matrix);
	vecHalfInPagedLodFileName = selTileFileNameGetter.getHalfInFileName();
	vecTotalInPagedLodFileName = selTileFileNameGetter.getTotalInFileName();
}

void COsgClipper::obtainSelTileNamebyScope(std::vector<CScope> vecScope, osg::ref_ptr<osg::Group> mGroup, osg::Matrix matrix, std::vector<std::string> &vecHalfInPagedLodFileName,
	std::vector<std::string> &vecTotalInPagedLodFileName)
{
	CObtainSelTileFileName selTileFileNameGetter(vecScope);
	selTileFileNameGetter.obtainSelTileNamebyScope(mGroup, matrix);
	vecHalfInPagedLodFileName = selTileFileNameGetter.getHalfInFileName();
	vecTotalInPagedLodFileName = selTileFileNameGetter.getTotalInFileName();
}


void COsgClipper::changePercentageBarInterval(int &halfPercentInterval, int &PercentInterval, bool isInside)
{
	if (isInside)
	{
		halfPercentInterval = PercentInterval * 0.9;
	}
	else
	{
		halfPercentInterval = PercentInterval;
	}
}


bool COsgClipper::clipOsgHeader(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecScope, bool isInside)
{
	InterfaceOsgFinderFactory IOsgFinderFactory;
	InterfaceOsgFinder* IOsgFinder = IOsgFinderFactory.create();

	std::vector<osg::MatrixTransform*> vecMatTrans; 
	IOsgFinder->FindTransNode(sceneNode, vecMatTrans);

	if (vecMatTrans.size() > 0)
	{
		osg::Matrix matrix = vecMatTrans[0]->getMatrix();

		//database路径设为相对,对头结点裁切
		CMultiScopeHeadGeodeClipper ive(vecScope, matrix, isInside);
		sceneNode->accept(ive);
	}
	
	return true;
}

bool COsgClipper::clipAccurateOsgHeader(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecScope, bool isInside, bool bAccurate)
{
	InterfaceOsgFinderFactory IOsgFinderFactory;
	InterfaceOsgFinder* IOsgFinder = IOsgFinderFactory.create();

	std::vector<osg::MatrixTransform*> vecMatTrans;
	IOsgFinder->FindTransNode(sceneNode, vecMatTrans);

	if (vecMatTrans.size() > 0)
	{
		osg::Matrix matrix = vecMatTrans[0]->getMatrix();

		//database路径设为相对,对头结点裁切
		if (bAccurate)
		{
			CMultiScopeAccurateHeadGeodeClippper ive(vecScope, matrix, isInside);//边缘精确裁切
			sceneNode->accept(ive);
			vecInsideVerts = ive.vecInsideVerts;
		}
		else
		{
			CMultiScopeNotAccurateHeadGeodeClippper ive(vecScope, matrix, isInside);//边缘非精确裁切
			sceneNode->accept(ive);
			vecInsideVerts = ive.vecInsideVerts;
		}	
	}

	return true;
}

std::vector<osg::Vec3d> COsgClipper::getInsideVecVerts()
{
	return vecInsideVerts;
}
