#pragma once
#include "OSG_Plus.h"
#include "InterfaceOsgDomGenerator.h"

class osgDomGenerator : public InterfaceOsgDomGenerator
{
public:
	virtual BOOL GenerateDOM(osg::ref_ptr<osg::Node> SceneNode, std::string SceneFileName, double RES, InterfaceSigEventCallBack* ISigCallBack);

	virtual void ExtractTexture(osg::ref_ptr<osg::Node> loadedModel, std::string outFileName, osg::Matrix matrix, double xRes, double yRes);

	virtual bool ProduceDomInRect(osg::ref_ptr<osg::MatrixTransform> mTrans, std::vector<CoordDouble3D> vecCoord, double xRes, double yRes, std::string outFileName);

	virtual bool produceNodeDOM(osg::ref_ptr<osg::Node> loadedModel, std::string &outFileName, osg::Matrix matrix, double xRes, double yRes);

protected:
	static void Produce(void* ptr);

	void StartProducingDom();

protected:
	InterfaceSigEventCallBack* iSigCallBack;
	osg::ref_ptr<osg::Node> sceneNode;
	std::string sceneFileName;
	double res;
	int xTileNum;
	int yTileNum;
	std::string outputFileName;
};