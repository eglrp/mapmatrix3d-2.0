#include "StdAfx.h"
#include "osgDSMGene.h"
#include "SceneDemGenerator.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceMFCExcute.h"
#include "qcomm.h"
#include <process.h>

void osgDSMGene::GenerateDSM(osg::ref_ptr<osg::Node> node, std::string strSceneFileName, double xRes, double yRes, InterfaceSigEventCallBack* ISigCallBack)
{
	std::string extSelect = "TIF文件 (*.tif)|*.tif||";
	std::string outputFileName;
	InterfaceMFCExcuteFactory IExcuteFactory;
	InterfaceMFCExcute* IExcute = IExcuteFactory.create();
	bool isSuccess = IExcute->OpenDialog(outputFileName, extSelect, false);

	if (!isSuccess)
	{
		return;
	}

	std::vector<osg::MatrixTransform*> vecTransNode;
	
	
	iSigCallBack = ISigCallBack;
	SceneNode = node;

	int pos = strSceneFileName.rfind('\\') + 1;
	std::string sceneFilePath = strSceneFileName.substr(0, pos);
	RootModelPath = sceneFilePath;
	XRES = xRes;
	YRES = yRes;
	OutputFileName = outputFileName;

	StartProducingDSM();
}

void osgDSMGene::Produce(void* ptr)
{
	osgDSMGene* producer = (osgDSMGene*)ptr;
	InterfaceSigEventCallBack* ISigCallBack = producer->iSigCallBack;
	double xRes = producer->XRES;
	double yRes = producer->YRES;
	std::string rootModelPath = producer->RootModelPath;
	std::string outfileName = producer->OutputFileName;
	osg::ref_ptr<osg::Node> sceneNode = producer->SceneNode;
	std::string outputFileName = producer->OutputFileName;

	_logop("start producing DSM");
	std::vector<osg::MatrixTransform*> vecTransNode;
	InterfaceOsgFinderFactory IFactory;
	InterfaceOsgFinder* IOsgFinder = IFactory.create();
	IOsgFinder->FindTransNode(sceneNode, vecTransNode);
	CSceneDemGenerator DemGene;
	DemGene.StartGeneratingDem(vecTransNode, rootModelPath, outfileName, xRes, yRes, ISigCallBack);

	InterfaceMFCExcuteFactory IMFCExcuteFactory;
	InterfaceMFCExcute* IMFCExcute = IMFCExcuteFactory.create();

	IMFCExcute->PopUpMessage("DSM生成完成");
}

void osgDSMGene::StartProducingDSM()
{
	_beginthread(&Produce, 0, this);
}