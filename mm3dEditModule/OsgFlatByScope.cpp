#include "stdafx.h"
#include "OsgFlatByScope.h"
#include "StProgressSetter.h"

bool COsgFlatByScope::flatSceneByScope(osg::ref_ptr<osg::Node> sceneNode, std::string inFilePath, std::vector<CScope> vecClipScope, InterfaceSigEventCallBack* ISigEventCallBack)
{
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
	showHintFlating(ISigEventCallBack);

	int posStart = 0;
	int posInterval = 800;
	int tempInterval = posInterval / vecTrans.size();
	CStProgressSetter stProgressSetter(ISigEventCallBack, posStart, tempInterval);

	//裁切模型
	startFlatModelByScope(vecTrans, headOutputFilePath);

	//裁切头文件
	flatOsgbHeader(vecClipScope, sceneNode, headOutputFileName);

	ISigEventCallBack->EmitSigEndProgressFun();

	//重新加载
	fileSelector.reloadScene(ISigEventCallBack, headOutputFileName);

	showHintFinishFlat(ISigEventCallBack);
}
