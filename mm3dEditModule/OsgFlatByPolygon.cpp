#include "stdafx.h"
#include "OsgFlatByPolygon.h"
#include "InterfaceMFCExcute.h"
#include "InterfaceOsgEditor.h"
#include "InterfaceOsgFlat.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceOsgAlgorithm.h"

bool COsgFlatByPolygon::flatSceneByScope(osg::ref_ptr<osg::Node> sceneNode, std::string InFilePath, std::vector<CScope> VecClipScope,
	InterfaceSigEventCallBack* iSigEventCallBack)
{
	InitFlatPara(InFilePath, VecClipScope, iSigEventCallBack);
	//选择路径
	bool isSuccess = fileSelector.selectDir();

	if (!isSuccess)
	{
		return false;
	}

	std::string headOutputFileName = fileSelector.getSelFileName();
	int pos = headOutputFileName.rfind('\\');
	std::string headOutputFilePath = headOutputFileName.substr(0, pos);

	//查找trans节点
	isSuccess = transFinder.findTrans(sceneNode);

	if (!isSuccess)
	{
		InterfaceMFCExcuteFactory iMFCExcuteFactory;
		InterfaceMFCExcute* IExcute = iMFCExcuteFactory.create();
		IExcute->PopUpMessage("场景未置平，请先置平");
		return false;
	}

	std::vector<osg::MatrixTransform*> vecTrans = transFinder.getTrans();
	showHintFlating(iSigEventCallBack);

	//裁切模型
	startFlatModelByScope(vecTrans, headOutputFilePath);

	//裁切头文件
	flatOsgbHeader(vecClipScope, sceneNode, headOutputFileName);

	iSigEventCallBack->EmitSigEndProgressFun();

	fileSelector.reloadScene(iSigEventCallBack, headOutputFileName);
	showHintFinishFlat(iSigEventCallBack);
}

bool COsgFlatByPolygon::flatSingleObjectByScope(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecClipScope)
{
	InterfaceOsgFlatFactory IOsgFlatFactory;
	InterfaceOsgFlat* IFlat = IOsgFlatFactory.create();
	IFlat->flatOsgHeader(sceneNode, vecClipScope);
	return true;
}

void COsgFlatByPolygon::showHintFlating(InterfaceSigEventCallBack* iSigEventCallBack)
{
	iSigEventCallBack->EmitSigShowOpsHint("正在压平");
}

void COsgFlatByPolygon::showHintFinishFlat(InterfaceSigEventCallBack* iSigEventCallBack)
{
	iSigEventCallBack->EmitSigShowOpsHint("结束压平");
}

void COsgFlatByPolygon::startFlatModelByScope(std::vector<osg::MatrixTransform*> vecTrans, std::string headOutputFilePath)
{
	int posStart = 0;
	int posInterval = 800;
	int tempInterval = posInterval / vecTrans.size();

	InterfaceOsgFlatFactory iOsgFlatFactory;
	InterfaceOsgFlat* iOsgFlat = iOsgFlatFactory.create();

	double flattenHeight = 0;
	obtainFlattenHeight(flattenHeight, vecClipScope);

	
	
	//压平
	for (int i = 0; i < vecTrans.size(); i++)
	{
		osg::ref_ptr<osg::MatrixTransform> mTrans = vecTrans[i];

		fileSelector.createClipOutputFolder(mTrans, headOutputFilePath);
		std::string outputFilePath = fileSelector.getClipOutputFilePath();
		
		CStProgressSetter stProgress(iSigEventCallBack, posStart + tempInterval * i, tempInterval);
		iOsgFlat->flatSceneByScope(mTrans, inFilePath, headOutputFilePath, vecClipScope, stProgress);

	}
}


void COsgFlatByPolygon::flatOsgbHeader(std::vector<CScope> vecClipScope, osg::ref_ptr<osg::Node> sceneNode, std::string headOutputFileName)
{
	//对获取压平高度
	double flattenHeight = 0;
	obtainFlattenHeight(flattenHeight, vecClipScope);

	//头结点压平
	osg::ref_ptr<osg::Node> copyNode = dynamic_cast<osg::Node*> (sceneNode->clone(osg::CopyOp::DEEP_COPY_ALL));
	InterfaceOsgFlatFactory IOsgFlatFactory;
	InterfaceOsgFlat* IFlat = IOsgFlatFactory.create();
	IFlat->flatOsgHeader(copyNode, vecClipScope);

	//头结点输出
	InterfaceOsgEditorFactory IEditorFactory;
	InterfaceOsgEditor* IEditor = IEditorFactory.create();
	IEditor->SetLodFileName(copyNode, "");

	osg::ref_ptr<osgDB::Options> spOptions = new osgDB::Options;
	spOptions->setPluginStringData("WriteImageHint", "IncludeFile");
	osgDB::writeNodeFile(*copyNode, headOutputFileName, spOptions.get());
}

void COsgFlatByPolygon::obtainFlattenHeight(double &flattenHeight, std::vector<CScope> vecClipScope)
{
	std::vector<osg::Vec3d> vecWorldPt = vecClipScope[0].vecCoord;
	InterfaceOsgAlgorithmFactory iOsgAlgorithmFactory;
	InterfaceOsgAlgorithm* iOsgAlgorithm = iOsgAlgorithmFactory.create();
	double minHeight = 0; double maxHeight = 0;
	iOsgAlgorithm->FindMinMaxHeight(vecWorldPt, minHeight, maxHeight);
	flattenHeight = minHeight;
}

void COsgFlatByPolygon::InitFlatPara(std::string InFilePath, std::vector<CScope> VecClipScope, InterfaceSigEventCallBack* iSigEventCallBack)
{
	inFilePath = InFilePath;
	vecClipScope = VecClipScope;
	iSigEventCallBack = iSigEventCallBack;
}

