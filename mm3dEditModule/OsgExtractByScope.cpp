#include "stdafx.h"
#include "OsgextractByScope.h"
#include "InterfaceMFCExcute.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceOsgClip.h"
#include "InterfaceOsgEditor.h"
#include "StProgressSetter.h"

bool COsgextractByScope::extractByScope(osg::ref_ptr<osg::Node> sceneNode, std::string inFilePathName, std::string outputObjectFileName, std::vector<CScope> vecClipScope, bool isInside, InterfaceSigEventCallBack* iSigEventCallBack)
{
	InterfaceOsgClipFactory iOsgClipFactory;
	InterfaceOsgClip* iOsgClip = iOsgClipFactory.create(BY_ACCURATE_POLYGON);
	bool isSuccess = iOsgClip->clipAccurateOsgHeader(sceneNode, vecClipScope, isInside);
	return isSuccess;
	//int pos = inFilePathName.rfind('\\') + 1;
	//std::string inFilePath = inFilePathName.substr(0, pos);
	//std::string headOutputFileName = inFilePathName;
	//pos = headOutputFileName.rfind('\\');
	//std::string headOutputFilePath = headOutputFileName.substr(0, pos);

	////查找trans节点
	//bool isSuccess = transFinder.findTrans(sceneNode);
	//std::vector<osg::MatrixTransform*> vecTrans = transFinder.getTrans();

	//if (!isSuccess)
	//{
	//	InterfaceMFCExcuteFactory iMFCExcuteFactory;
	//	InterfaceMFCExcute* IExcute = iMFCExcuteFactory.create();
	//	IExcute->PopUpMessage("场景未置平，请先置平");
	//	return false;
	//}

	//showHintClipping(iSigEventCallBack);

	//int posStart = 0;
	//int posInterval = 800;
	//int tempInterval = posInterval / vecTrans.size();
	//CStProgressSetter stProgressSetter(iSigEventCallBack, posStart, tempInterval);

	////裁切模型
	//startClipModelByScope(vecTrans, headOutputFilePath, inFilePath, vecClipScope, stProgressSetter, isInside);

	////裁切头文件
	//HollowOutOsgbHeader(vecClipScope, sceneNode, headOutputFileName, outputObjectFileName, isInside);

	//iSigEventCallBack->EmitSigEndProgressFun();

	//fileSelector.reloadScene(iSigEventCallBack, headOutputFileName);

	//showHintFinishClipping(iSigEventCallBack);
}
//
//void COsgextractByScope::HollowOutOsgbHeader(std::vector<CScope> vecClipScope, osg::ref_ptr<osg::Node> sceneNode, std::string headOutputFileName, 
//	std::string outputObjectFileName, bool isInside)
//{
//	//database路径设为相对,对头结点裁切
//
//	/*_logop("开始清除多余tile");*/
//	InterfaceOsgEditorFactory IEditorFactory;
//	InterfaceOsgEditor* IEditor = IEditorFactory.create();
//
//	osg::ref_ptr<osg::Node> copyNode = dynamic_cast<osg::Node*> (sceneNode->clone(osg::CopyOp::DEEP_COPY_ALL));
//	InterfaceOsgClipFactory IClipFactory;
//	InterfaceOsgClip* IClip = IClipFactory.create(BY_POLYGON);
//	IClip->clipOsgHeader(copyNode, vecClipScope, isInside);
//	IEditor->SetLodFileName(copyNode, "");
//
//	std::vector<osg::PagedLOD*> vecPagedLod;
//	InterfaceOsgFinderFactory IFinderFactory;
//	InterfaceOsgFinder* IFinder = IFinderFactory.create();
//	IFinder->FindEmptyLod(copyNode, vecPagedLod);
//
//	/*for (int i = 0; i < vecPagedLod.size(); i++)
//	{
//		std::string fileName = vecPagedLod[i]->getFileName(1);
//
//		if (std::find(vecFinalClipFileName.begin(), vecFinalClipFileName.end(), fileName) != vecFinalClipFileName.end())
//		{
//			osg::ref_ptr<osg::Group> group = vecPagedLod[i]->getParent(0);
//
//			if (!group)
//			{
//				continue;
//			}
//
//			group->removeChild(vecPagedLod[i]);
//		}
//	}*/
//
//	//重新加入单体化结果
//	//AddObjectInCopyNode(outputObjectFileName, copyNode);
//
//	osg::ref_ptr<osgDB::Options> spOptions = new osgDB::Options;
//	spOptions->setPluginStringData("WriteImageHint", "IncludeFile");
//	osgDB::writeNodeFile(*copyNode, headOutputFileName, spOptions.get());
//}
//
//bool COsgextractByScope::ClipByScope(osg::ref_ptr<osg::MatrixTransform> mTrans, std::string inFilePath, std::string outputFilePath, std::vector<CScope> vecScope,
//	CStProgressSetter stProgressSetter, bool isInside)
//{
//	InterfaceOsgClipFactory iOsgClipFactory;
//	InterfaceOsgClip* iOsgClip = iOsgClipFactory.create(BY_ACCURATE_POLYGON);
//	bool isSuccess = iOsgClip->clipSceneByScope(mTrans, inFilePath, outputFilePath, vecScope, stProgressSetter, isInside);
//	return isSuccess;
//}
//
//bool COsgextractByScope::AddObjectInCopyNode(std::string outputObjectFileName, osg::ref_ptr<osg::Node> copyNode)
//{
//	osg::ref_ptr<osg::MatrixTransform> objectTrans = dynamic_cast<osg::MatrixTransform*> (osgDB::readNodeFile(outputObjectFileName));
//
//	if (objectTrans)
//	{
//		std::vector<osg::MatrixTransform*> vecMatrixTrans;
//		InterfaceOsgFinderFactory IOsgFinderFactory;
//		InterfaceOsgFinder* iOsgFinder = IOsgFinderFactory.create();
//		iOsgFinder->FindTransNode(copyNode, vecMatrixTrans);
//
//		//设置pagedLod
//		osg::ref_ptr<osg::PagedLOD> pagedLod = new osg::PagedLOD;
//		int rPos = outputObjectFileName.rfind('\\');
//		std::string outputObjectFilePath = outputObjectFileName.substr(0, rPos - 1);
//		int rrPos = outputObjectFilePath.rfind('\\');
//		int length = outputObjectFileName.length();
//		std::string relativeOutputObjectFileName = '.' + outputObjectFileName.substr(rrPos, length - 1);
//		pagedLod->setFileName(1, relativeOutputObjectFileName);
//		pagedLod->addChild(objectTrans);
//		pagedLod->setRange(1, 1, 1e30);
//
//		//增加反向trans
//		osg::ref_ptr<osg::MatrixTransform> iObjectTrans = new osg::MatrixTransform;
//		iObjectTrans->setName("ObjectMatrix");
//		osg::Matrix worldMatrix = objectTrans->getMatrix();
//		osg::Matrix iWorldMatrix = worldMatrix.inverse(worldMatrix);
//		iObjectTrans->setMatrix(iWorldMatrix);
//		iObjectTrans->addChild(pagedLod);
//
//		vecMatrixTrans[0]->getChild(0)->asGroup()->addChild(iObjectTrans);
//
//		return true;
//	}
//
//	return false;
//}
