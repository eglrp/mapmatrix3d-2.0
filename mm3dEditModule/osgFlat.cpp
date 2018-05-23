// osgFlat.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "osgFlat.h"
#include "MultiScopeHeadGeodeflatter.h"
#include "MultiScopeHeadGeodeNormalizeFlatter.h"
#include "MultiScopePagedLodNormalizeFlatter.h"
#include "MultiScopeHeadGeodeFlatRecoverer.h"
#include "MultiScopePagedLodFlatRecoverer.h"
#include "InterfaceOsgAlgorithm.h"

//接口
bool COsgFlatter::flatSceneByScope(osg::ref_ptr<osg::MatrixTransform> mTrans, std::string inFilePath, std::string outputFilePath, std::vector<CScope> vecScope,
	CStProgressSetter stProgress)
{
	osg::ref_ptr<osg::Group> mGroup = mTrans->getChild(0)->asGroup();

	if (!mGroup)
	{
		return false;
	}

	osg::Matrix worldMatrix = mTrans->getMatrix();

	CObtainSelTileFileName tileFileNameGetter(vecScope);
	tileFileNameGetter.obtainFlatFileName(mTrans);
	std::vector<std::string> vecFlatPolygonFileName = tileFileNameGetter.getFlatPolygonFileName();

	CLogShower lodShower;
	lodShower.ShowLogFinishSelectScope();

	//合并组
	COsgPagedLodReader reader(inFilePath);
	osg::ref_ptr<osg::Group> combineTotalInPolygonGroup = reader.combinePagedLod(vecFlatPolygonFileName);						//将所有的tile合并																													

	lodShower.ShowLogStartOutputTotalInScopeOsgb();

	//对于在范围内进行压平
	FlatTotalInPolygonTile(combineTotalInPolygonGroup, worldMatrix);

	COsgPagedLodOutputer outputer(outputFilePath);
	outputer.replacePagedLodClipFilePath(mTrans, inFilePath, vecFlatPolygonFileName);

	lodShower.ShowLogFinishOutputTotalInScopeOsgb();
}


//接口
bool COsgFlatter::NormalizeflatSceneByScope(osg::ref_ptr<osg::MatrixTransform> mTrans, const std::string InFilePath, const std::string OutputFilePath, 
	const std::vector<CScope> VecScope, CStProgressSetter stProgress)
{
	InitOsgFlatter(InFilePath, OutputFilePath, VecScope, stProgress);

	CObtainSelTileFileName tileFileNameGetter(vecScope);
	tileFileNameGetter.obtainFlatFileName(mTrans);
	std::vector<std::string> vecFlatPolygonFileName = tileFileNameGetter.getFlatPolygonFileName();

	CLogShower lodShower;
	lodShower.ShowLogFinishSelectScope();

	//合并组
	COsgPagedLodReader reader(InFilePath);
	osg::ref_ptr<osg::Group> combineTotalInPolygonGroup = reader.combinePagedLod(vecFlatPolygonFileName);						//将所有的tile合并																													

	lodShower.ShowLogStartOutputTotalInScopeOsgb();

	osg::Matrix worldMatrix = mTrans->getMatrix();

	//对于在范围内进行压平
	NormalizeFlatTotalInPolygonTile(combineTotalInPolygonGroup, worldMatrix);

	COsgPagedLodOutputer outputer(outputFilePath);
	outputer.replacePagedLodClipFilePath(mTrans, inFilePath, vecFlatPolygonFileName);

	lodShower.ShowLogFinishOutputTotalInScopeOsgb();
}

bool COsgFlatter::RecoverFlatSceneByArea(osg::ref_ptr<osg::MatrixTransform> mTrans, std::string InFilePath, std::string OutputFilePath, std::vector<CScope> VecScope,
	std::vector<CScope> VecPreFlatScope, CStProgressSetter stProgress)
{
	InitOsgFlatter(InFilePath, OutputFilePath, VecScope, stProgress);

	osg::ref_ptr<osg::Group> mGroup = mTrans->getChild(0)->asGroup();
	CObtainSelTileFileName tileFileNameGetter(VecScope);
	tileFileNameGetter.obtainFlatFileName(mTrans);
	std::vector<std::string> vecFlatPolygonFileName = tileFileNameGetter.getFlatPolygonFileName();

	CLogShower lodShower;
	lodShower.ShowLogFinishSelectScope();

	//合并组
	COsgPagedLodReader reader(InFilePath);
	osg::ref_ptr<osg::Group> combineTotalInPolygonGroup = reader.combinePagedLod(vecFlatPolygonFileName);

	_logop("开始输出压平在范围内范围");

	osg::Matrix worldMatrix = mTrans->getMatrix();

	//对于在范围内进行压平
	RecoverPreFlatTotalInPolygonTile(mTrans, VecPreFlatScope, worldMatrix);

	//替换路径
	COsgPagedLodOutputer outputer(outputFilePath);
	outputer.replacePagedLodClipFilePath(mTrans, inFilePath, vecFlatPolygonFileName);
	lodShower.ShowLogFinishOutputTotalInScopeOsgb();
}

void COsgFlatter::FlatTotalInPolygonTile(osg::ref_ptr<osg::Node> node, osg::Matrix Matrix)
{
	//对于在范围内进行压平
	double flattenHeight = 0;
	obtainFlattenHeight(node, vecScope[0].vecCoord, Matrix, flattenHeight);

	//保存参数
	sqliteSaveer.saveFlattenHeight(flattenHeight);

	CMultiScopePagedLodFlatter iveFlatter(vecScope, inFilePath, outputFilePath, Matrix, flattenHeight, stPrgress);
	node->accept(iveFlatter);
}

void COsgFlatter::NormalizeFlatTotalInPolygonTile(osg::ref_ptr<osg::Node> node, osg::Matrix matrix)
{
	double sceneMaxHeight = 0;
	ObtainSceneMaxHeight(node, vecScope[0].vecCoord, matrix, sceneMaxHeight);

	double flattenHeight = 0;
	obtainFlattenHeight(node, vecScope[0].vecCoord, matrix, flattenHeight);

	//保存参数
	sqliteSaveer.savePreFlattenHeight(flattenHeight, sceneMaxHeight);

	//对于在范围内进行压平
	CMultiScopePagedLodNormalizeFlatter iveFlatter(vecScope, inFilePath, outputFilePath, matrix, flattenHeight, sceneMaxHeight, stPrgress);
	node->accept(iveFlatter);
}

void COsgFlatter::RecoverPreFlatTotalInPolygonTile(osg::ref_ptr<osg::Node> node, std::vector<CScope> vecPreScope, osg::Matrix matrix)
{
	sqliteSaveer.readPreFlattenHeight();
	double flattenHeight = sqliteSaveer.getPreFlattenHeight();
	double sceneMaxHeight = sqliteSaveer.getPreSceneMaxHeight();

	CMultiScopePagedLodFlatRecoverer iveFlatter(vecScope, vecPreScope, inFilePath, outputFilePath, matrix, flattenHeight, sceneMaxHeight, stPrgress);
	node->accept(iveFlatter);
}

//接口

bool COsgFlatter::flatOsgHeader(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecScope)
{
	InterfaceOsgFinderFactory IOsgFinderFactory;
	InterfaceOsgFinder* IOsgFinder = IOsgFinderFactory.create();

	std::vector<osg::MatrixTransform*> vecMatTrans;
	IOsgFinder->FindTransNode(sceneNode, vecMatTrans);


	if (vecMatTrans.size() > 0)
	{
		osg::Matrix matrix = vecMatTrans[0]->getMatrix();
		double flattenHeight = 0;
		obtainFlattenHeight(sceneNode, vecScope[0].vecCoord, matrix, flattenHeight);

		//database路径设为相对,对头结点裁切
		CMultiScopeHeadGeodeflatter ive(vecScope, matrix, flattenHeight);
		sceneNode->accept(ive);
	}

	return true;
}


bool COsgFlatter::normalizeflatOsgHeader(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecScope)
{
	InterfaceOsgFinderFactory IOsgFinderFactory;
	InterfaceOsgFinder* IOsgFinder = IOsgFinderFactory.create();

	std::vector<osg::MatrixTransform*> vecMatTrans;
	IOsgFinder->FindTransNode(sceneNode, vecMatTrans);

	if (vecMatTrans.size() > 0)
	{
		osg::Matrix matrix = vecMatTrans[0]->getMatrix();
		
		double flattenHeight = 0; double sceneMaxHeight = 0;
		ObtainSceneMaxHeight(sceneNode, vecScope[0].vecCoord, matrix, sceneMaxHeight);
		obtainFlattenHeight(sceneNode, vecScope[0].vecCoord, matrix, flattenHeight);

		//database路径设为相对,对头结点裁切
		CMultiScopeHeadGeodeNormalizeFlatter ive(vecScope, matrix, sceneMaxHeight, flattenHeight);
		sceneNode->accept(ive);
	}

	return true;
}

bool COsgFlatter::recoverFlatOsgHeader(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecScope, std::vector<CScope> preVecScope, double preSceneMaxHeight, double preFlattenHeight)
{
	InterfaceOsgFinderFactory IOsgFinderFactory;
	InterfaceOsgFinder* IOsgFinder = IOsgFinderFactory.create();

	std::vector<osg::MatrixTransform*> vecMatTrans;
	IOsgFinder->FindTransNode(sceneNode, vecMatTrans);

	if (vecMatTrans.size() > 0)
	{
		osg::Matrix matrix = vecMatTrans[0]->getMatrix();
		CMultiScopeHeadGeodeFlatRecoverer ive(vecScope, preVecScope, matrix, preSceneMaxHeight, preFlattenHeight);
		sceneNode->accept(ive);
	}

	return true;
}

void COsgFlatter::ObtainSceneMaxHeight(osg::ref_ptr<osg::Node> sceneNode, std::vector<osg::Vec3d> vecWorldPt, osg::Matrix matrix, double &sceneMaxHeight)
{
	//获取压平高度和场景最大高度
	double sceneMinHeight = 0;
	InterfaceOsgFinderFactory IOsgFinderFactory;
	InterfaceOsgFinder* IOsgFinder = IOsgFinderFactory.create();
	IOsgFinder->FindNodeMinMaxHeight(sceneNode, matrix, sceneMinHeight, sceneMaxHeight);
}

void COsgFlatter::obtainFlattenHeight(osg::ref_ptr<osg::Node> sceneNode, std::vector<osg::Vec3d> vecWorldPt, osg::Matrix matrix, double &flattenHeight)
{
	double minHeight = 0; double maxHeight = 0;
	InterfaceOsgAlgorithmFactory iOsgAlgorithmFactory;
	InterfaceOsgAlgorithm* iOsgAlgorithm = iOsgAlgorithmFactory.create();
	iOsgAlgorithm->FindMinMaxHeight(vecWorldPt, minHeight, maxHeight);
	flattenHeight = minHeight;
}

void COsgFlatter::InitOsgFlatter(const std::string inFilePath, const std::string outputFilePath, const std::vector<CScope> vecScope, 
	CStProgressSetter stPrgress)
{
	this->inFilePath = inFilePath;
	this->outputFilePath = outputFilePath;
	this->vecScope = vecScope;
	this->stPrgress = stPrgress;
}
