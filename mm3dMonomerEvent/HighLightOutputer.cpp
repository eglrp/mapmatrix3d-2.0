#include "stdafx.h"
#include "HighLightOutputer.h"
#include "OSG_Plus.h"
#include "InterfaceOsgEditor.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceMFCExcute.h"

void CHighLightOutputer::outputModelWithoutTexture(std::string outModelWithoutFileName, osg::ref_ptr<osg::Node> outNode, osg::Matrix worldMatrix)
{
	setWithoutPolygonOffset(outNode);

	osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
	trans->setMatrix(worldMatrix);
	trans->addChild(outNode);

	InterfaceOsgEditorFactory iOsgEditorFactory;
	InterfaceOsgEditor* iOsgEditor = iOsgEditorFactory.create();
	iOsgEditor->SetLodFileName(trans, "");
	iOsgEditor->CancelObjectHighLight(trans);
	osgDB::writeNodeFile(*trans, outModelWithoutFileName);

	InterfaceMFCExcuteFactory iMfcExcuteFactory;
	InterfaceMFCExcute* iMfcExcute = iMfcExcuteFactory.create();
	iMfcExcute->PopUpMessage("导出白模成功");
}

void CHighLightOutputer::outputTextureModel(std::string outModelFileName, osg::ref_ptr<osg::Node> outNode, osg::Matrix worldMatrix)
{
	setWithoutPolygonOffset(outNode);

	osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
	trans->setMatrix(worldMatrix);
	trans->addChild(outNode);

	osg::ref_ptr<osgDB::Options> spOptions = new osgDB::Options;
	spOptions->setPluginStringData("WriteImageHint", "IncludeFile");

	InterfaceOsgEditorFactory iOsgEditorFactory;
	InterfaceOsgEditor* iOsgEditor = iOsgEditorFactory.create();
	iOsgEditor->SetLodFileName(trans, "");
	iOsgEditor->CancelObjectHighLight(trans);
	osgDB::writeNodeFile(*trans, outModelFileName, spOptions.get());

	//reloadScene(outModelFileName);
	InterfaceMFCExcuteFactory iMfcExcuteFactory;
	InterfaceMFCExcute* iMfcExcute = iMfcExcuteFactory.create();
	iMfcExcute->PopUpMessage("导出精模成功");
}

void CHighLightOutputer::removeSelPagedLod(osg::ref_ptr<osg::Node> selPagedLod)
{
	if (!selPagedLod)
	{
		return;
	}

	if (selPagedLod->getNumParents() == 0)
	{
		return;
	}

	selPagedLod->getParent(0)->removeChild(selPagedLod);
	selPagedLod->unref();
}

void CHighLightOutputer::reloadScene(std::string outModelFileName)
{
	iSigEventCallBack->EmitSigRemoveAllScene();
	char* reloadFileName = (char*)outModelFileName.c_str();
	iSigEventCallBack->EmitSigAddScene(reloadFileName);
}


void CHighLightOutputer::setWithoutPolygonOffset(osg::ref_ptr<osg::Node> outNode)
{
	osg::ref_ptr<osg::PolygonOffset> polyoffset = new osg::PolygonOffset;
	polyoffset->setFactor(0);
	polyoffset->setUnits(0.0f);
	outNode->getOrCreateStateSet()->setAttributeAndModes(polyoffset.get(), osg::StateAttribute::ON);
}

osg::Matrix CHighLightOutputer::obtainMatrix(osg::ref_ptr<osg::Node> outNode)
{
	osg::MatrixList matrixList = outNode->getWorldMatrices();
	int size = matrixList.size();
	osg::Matrix worldMatrix = matrixList[size - 1];
	return worldMatrix;
}