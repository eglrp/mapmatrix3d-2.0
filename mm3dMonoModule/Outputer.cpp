#include "stdafx.h"
#include "Outputer.h"
#include "InterfaceMFCExcute.h"


void COsgOutputer::OutputOsgb(osg::ref_ptr<osg::MatrixTransform> mTrans, std::string outputFileName)
{
	int pos = outputFileName.rfind('\\');
	int length = outputFileName.length() - 1;
	std::string outputFileTitle = outputFileName.substr(pos + 1, length - 1);
	mTrans->setName(outputFileTitle);

	mTrans->getChild(0)->setName("Property:");

	osg::ref_ptr<osgDB::Options> spOptions = new osgDB::Options;
	spOptions->setPluginStringData("WriteImageHint", "IncludeFile");
	osgDB::writeNodeFile(*mTrans, outputFileName, spOptions.get());
}


void COsgOutputer::ReloadScene(InterfaceSigEventCallBack* iSigEventCallBack, std::string headOutputFileName)
{
	//让用户选择是否重新加载
	InterfaceMFCExcuteFactory iMFCExcuteFactory;
	InterfaceMFCExcute* IExcute = iMFCExcuteFactory.create();
	bool isReload = IExcute->MessageBoxIsReload();

	if (isReload)
	{
		iSigEventCallBack->EmitSigRemoveAllScene();
		char* reloadFileName = (char*)headOutputFileName.c_str();
		iSigEventCallBack->EmitSigAddScene(reloadFileName);
	}
}
