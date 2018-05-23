#include "stdafx.h"
#include "OsgNormalizeFlatSceneByPolygon.h"
#include "InterfaceOsgFlat.h"
#include "InterfaceMFCExcute.h"
#include "InterfaceOsgEditor.h"
#include "StProgressSetter.h"

bool COsgNormalizeFlatSceneByPolygon::flatSceneByScope(osg::ref_ptr<osg::Node> sceneNode, std::string inFilePath, std::vector<CScope> vecClipScope, InterfaceSigEventCallBack* iSigEventCallBack)
{
	InitFlatPara(inFilePath, vecClipScope, iSigEventCallBack);
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

	int posStart = 0;
	int posInterval = 800;
	int tempInterval = posInterval / vecTrans.size();
	CStProgressSetter stProgressSetter(iSigEventCallBack, posStart, tempInterval);

	//压平模型
	startFlatModelByScope(vecTrans, headOutputFilePath);

	//压平头文件
	flatOsgbHeader(vecClipScope, sceneNode, headOutputFileName);

	iSigEventCallBack->EmitSigEndProgressFun();

	//重新加载
	fileSelector.reloadScene(iSigEventCallBack, headOutputFileName);

	showHintFinishFlat(iSigEventCallBack);
}

bool COsgNormalizeFlatSceneByPolygon::flatSingleObjectByScope(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecClipScope)
{
	InterfaceOsgFlatFactory IOsgFlatFactory;
	InterfaceOsgFlat* IFlat = IOsgFlatFactory.create();
	IFlat->normalizeflatOsgHeader(sceneNode, vecClipScope);

	return true;
}

void COsgNormalizeFlatSceneByPolygon::startFlatModelByScope(std::vector<osg::MatrixTransform*> vecTrans, std::string headOutputFilePath)
{
	int posStart = 0;
	int posInterval = 800;
	int tempInterval = posInterval / vecTrans.size();

	InterfaceOsgFlatFactory iOsgFlatFactory;
	InterfaceOsgFlat* iOsgFlat = iOsgFlatFactory.create();
	//裁切
	for (int i = 0; i < vecTrans.size(); i++)
	{
		osg::ref_ptr<osg::MatrixTransform> mTrans = vecTrans[i];

		fileSelector.createClipOutputFolder(mTrans, headOutputFilePath);
		std::string outputFilePath = fileSelector.getClipOutputFilePath();

		CStProgressSetter stProgress(iSigEventCallBack, posStart + tempInterval * i, tempInterval);

		//按归一化的方法进行裁切
		iOsgFlat->NormalizeflatSceneByScope(mTrans, inFilePath, outputFilePath, vecClipScope, stProgress);
	}
}

void COsgNormalizeFlatSceneByPolygon::flatOsgbHeader(std::vector<CScope> vecClipScope, osg::ref_ptr<osg::Node> sceneNode, std::string headOutputFileName)
{
	osg::ref_ptr<osg::Node> copyNode = dynamic_cast<osg::Node*> (sceneNode->clone(osg::CopyOp::DEEP_COPY_ALL));
	InterfaceOsgFlatFactory IOsgFlatFactory;
	InterfaceOsgFlat* IFlat = IOsgFlatFactory.create();
	IFlat->normalizeflatOsgHeader(sceneNode, vecClipScope);

	//database路径设为相对,对头结点裁切
	InterfaceOsgEditorFactory IEditorFactory;
	InterfaceOsgEditor* IEditor = IEditorFactory.create();
	IEditor->SetLodFileName(copyNode, "");

	osg::ref_ptr<osgDB::Options> spOptions = new osgDB::Options;
	spOptions->setPluginStringData("WriteImageHint", "IncludeFile");
	osgDB::writeNodeFile(*copyNode, headOutputFileName, spOptions.get());
}

void COsgNormalizeFlatSceneByPolygon::InitFlatPara(std::string InFilePath, std::vector<CScope> VecClipScope, InterfaceSigEventCallBack* iSigEventCallBack)
{
	this->inFilePath = InFilePath;
	this->vecClipScope = VecClipScope;
	this->iSigEventCallBack = iSigEventCallBack;
}

void COsgNormalizeFlatSceneByPolygon::showHintFlating(InterfaceSigEventCallBack* iSigEventCallBack)
{
	iSigEventCallBack->EmitSigShowOpsHint("正在归一化压平");
}

void COsgNormalizeFlatSceneByPolygon::showHintFinishFlat(InterfaceSigEventCallBack* iSigEventCallBack)
{
	iSigEventCallBack->EmitSigShowOpsHint("结束归一化压平");
}