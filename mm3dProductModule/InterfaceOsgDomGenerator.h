#pragma once
#include "OSG_Plus.h"
#include "InterfaceUserEventCallBack.h"
#include "coord.hpp"

#ifdef OSGDOMLIBDLL  
#define OSGDOMAPI _declspec(dllexport)  
#else  
#define OSGDOMAPI  _declspec(dllimport)  
#endif  


class OSGDOMAPI InterfaceOsgDomGenerator
{
public:
	virtual BOOL GenerateDOM(osg::ref_ptr<osg::Node> SceneNode, std::string SceneFileName, double RES, InterfaceSigEventCallBack* ISigCallBack) = 0;

	virtual void ExtractTexture(osg::ref_ptr<osg::Node> loadedModel, std::string outFileName, osg::Matrix matrix, double xRes, double yRes) = 0;

	virtual bool ProduceDomInRect(osg::ref_ptr<osg::MatrixTransform> mTrans, std::vector<CoordDouble3D> vecCoord, double xRes, double yRes, std::string outFileName) = 0;

	virtual bool produceNodeDOM(osg::ref_ptr<osg::Node> loadedModel, std::string &outFileName, osg::Matrix matrix, double xRes, double yRes) = 0;
};

class OSGDOMAPI InterfaceOsgDomGeneratorFactory
{
public:
	InterfaceOsgDomGenerator* create();
};