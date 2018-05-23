#pragma once
#include "OSG_Plus.h"
#include "InterfaceUserEventCallBack.h"

class CHighLightOutputer
{
public:
	CHighLightOutputer(osgViewer::Viewer* mViewer,	InterfaceSigEventCallBack* iSigEventCallBack)
	{
		this->mViewer = mViewer;
		this->iSigEventCallBack = iSigEventCallBack;
	}

	virtual void outputModelWithoutTexture(std::string outModelWithoutFileName, osg::ref_ptr<osg::Node> outNode, osg::Matrix worldMatrix);

	virtual void outputTextureModel(std::string outModelFileName, osg::ref_ptr<osg::Node> outNode, osg::Matrix worldMatrix);

	virtual void removeSelPagedLod(osg::ref_ptr<osg::Node> selPagedLod);

	osg::Matrix obtainMatrix(osg::ref_ptr<osg::Node> outNode);

protected:
	void reloadScene(std::string outModelFileName);

	void setWithoutPolygonOffset(osg::ref_ptr<osg::Node> outNode);

private:
	osg::ref_ptr<osgViewer::Viewer> mViewer;

	InterfaceSigEventCallBack* iSigEventCallBack;
};

