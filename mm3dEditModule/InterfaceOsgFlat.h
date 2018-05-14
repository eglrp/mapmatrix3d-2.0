#pragma once
#include "OSG_Plus.h"
#include "publicDefined.h"
#include "InterfaceUserEventCallBack.h"
#include "StProgressSetter.h"

#ifdef OSGFLATLIBDLL  
#define OSGFLATAPI _declspec(dllexport)  
#else  
#define OSGFLATAPI  _declspec(dllimport)  
#endif  

class OSGFLATAPI InterfaceOsgFlat
{
public:
	virtual bool flatSceneByScope(osg::ref_ptr<osg::MatrixTransform> mTrans, std::string inFilePath, std::string outputFilePath, std::vector<CScope> vecScope,
		CStProgressSetter stProgress) = 0;

	virtual bool NormalizeflatSceneByScope(osg::ref_ptr<osg::MatrixTransform> mTrans, std::string inFilePath, std::string outputFilePath, std::vector<CScope> vecScope,
		CStProgressSetter stProgress) = 0;

	virtual bool RecoverFlatSceneByArea(osg::ref_ptr<osg::MatrixTransform> mTrans, std::string InFilePath, std::string OutputFilePath, std::vector<CScope> VecScope,
		std::vector<CScope> VecPreFlatScope, CStProgressSetter stProgress) = 0;

	virtual bool flatOsgHeader(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecScope) = 0;

	virtual bool normalizeflatOsgHeader(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecScope) = 0;

	virtual bool recoverFlatOsgHeader(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecScope, std::vector<CScope> preVecScope, double preSceneMaxHeight, double preFlattenHeight) = 0;
};


class OSGFLATAPI InterfaceOsgFlatFactory
{
public:
	InterfaceOsgFlat* create();
};