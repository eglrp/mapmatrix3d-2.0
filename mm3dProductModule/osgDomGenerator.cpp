// osgDomGenerator.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgDomGenerator.h"
#include "SceneDomGenerator.h"
#include <process.h>

BOOL osgDomGenerator::GenerateDOM(osg::ref_ptr<osg::Node> SceneNode, std::string SceneFileName, double RES, InterfaceSigEventCallBack* ISigCallBack)
{
	_logop("开始生产DOM");
	xTileNum = 1;
	yTileNum = 1;

	//创建文件夹并设置输出路径
	std::string extSelect = "tiff文件 (*.tif)|*.tif||";

	InterfaceMFCExcuteFactory IExcuteFactory;
	InterfaceMFCExcute* IExcute = IExcuteFactory.create();
	bool isSuccess = IExcute->OpenDialog(outputFileName, extSelect, false);

	if (!isSuccess)
	{
		return FALSE;
	}

	iSigCallBack = ISigCallBack;
	res = RES;
	sceneFileName = SceneFileName;
	sceneNode = SceneNode;

	int pos = outputFileName.rfind('.');
	int length = outputFileName.length() - 1;
	std::string extFile = outputFileName.substr(pos, length);

	if (extFile != ".tif")
	{
		outputFileName += ".tif";
	}

	char strMsg[MAX_PATH];
	sprintf_s(strMsg, "%s\n", outputFileName.c_str());
	_logop(strMsg);
	
	StartProducingDom();

	return TRUE;
}

void osgDomGenerator::ExtractTexture(osg::ref_ptr<osg::Node> loadedModel, std::string outFileName, osg::Matrix matrix, double xRes, double yRes)
{
	CSceneDomGenerator geneDom;
	geneDom.SetResolution(xRes);
	geneDom.produceNodeDOM(loadedModel, outFileName, matrix);
}

bool osgDomGenerator::ProduceDomInRect(osg::ref_ptr<osg::MatrixTransform> mTrans, std::vector<CoordDouble3D> vecCoord, double xRes, double yRes, std::string outFileName)
{
	CSceneDomGenerator geneDom;
	geneDom.SetResolution(xRes);
	return geneDom.ProduceRectDom(mTrans, vecCoord, outFileName);

}

bool osgDomGenerator::produceNodeDOM(osg::ref_ptr<osg::Node> loadedModel, std::string &outFileName, osg::Matrix matrix, double xRes, double yRes)
{
	CSceneDomGenerator geneDom;
	geneDom.SetResolution(xRes);
	return geneDom.produceNodeDOM(loadedModel, outFileName, matrix);
}


void osgDomGenerator::Produce(void* ptr)
{
	osgDomGenerator* producer = (osgDomGenerator*)ptr;
	InterfaceSigEventCallBack* ISigCallBack = producer->iSigCallBack;
	double RES = producer->res;
	int XTileNum = producer->xTileNum;
	int YTileNum = producer->yTileNum;
	std::string SceneFileName = producer->sceneFileName;
	osg::ref_ptr<osg::Node> SceneNode = producer->sceneNode;
	std::string OutputFileName = producer->outputFileName;

	CSceneDomGenerator geneDom;
	bool isSuccess = geneDom.StartProducingDOM(SceneNode, SceneFileName, OutputFileName, RES, XTileNum, YTileNum, ISigCallBack);

	if (isSuccess)
	{
		InterfaceMFCExcuteFactory IMFCExcuteFactory;
		InterfaceMFCExcute* IMFCExcute = IMFCExcuteFactory.create();
		IMFCExcute->PopUpMessage("DOM生成完成");
	}
}

void osgDomGenerator::StartProducingDom()
{
	_beginthread(&Produce, 0, this);
}