#include "stdafx.h"
#include "FileSelector.h"
#include "InterfaceMFCExcute.h"
#include "InterfaceOsgFinder.h"

std::string CFileSelector::getSelFileName() const
{
	return selFileName;
}


std::string CFileSelector::getClipOutputFilePath() const
{
	return clipOutputFilePath;
}

bool CFileSelector::selectDir()
{
	std::string extSelect = "osgb模型 (*.osgb)|*.osgb|osg (*.osg)|*.osg|All Files (*.*)|*.*||";

	InterfaceMFCExcuteFactory IExcuteFactory;
	InterfaceMFCExcute* IExcute = IExcuteFactory.create();

	bool isSuccess = IExcute->OpenDialog(selFileName, extSelect, false);

	if (!isSuccess)
	{
		return false;
	}

	return true;
}

void CFileSelector::createClipOutputFolder(osg::ref_ptr<osg::MatrixTransform> mTrans, std::string headOutputFilePath)
{
	std::string blockName;
	InterfaceOsgFinderFactory iFinderFactory;
	InterfaceOsgFinder* IFinder = iFinderFactory.create();
	IFinder->FindLodBlockName(mTrans, blockName);

	//创建目录
	clipOutputFilePath = headOutputFilePath + blockName + ".\\Clip";

	InterfaceMFCExcuteFactory iMFCExtueFactory;
	InterfaceMFCExcute* IExcute = iMFCExtueFactory.create();
	IExcute->createFolder(clipOutputFilePath);
}


bool CFileSelector::isSameAsOutputFileName(std::string inFilePath, std::string headOutputFileName)
{
	int pos = headOutputFileName.rfind("\\");
	std::string headOutputFilePath = headOutputFileName.substr(0, pos + 1);

	if (inFilePath == headOutputFilePath)
	{
		InterfaceMFCExcuteFactory iMfcExcuteFactory;
		InterfaceMFCExcute* IExcute = iMfcExcuteFactory.create();
		IExcute->PopUpMessage("输出路径不能和模型索引位置一样");
		return true;
	}

	return false;
}

void CFileSelector::reloadScene(InterfaceSigEventCallBack* iSigEventCallBack, std::string headOutputFileName)
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