#pragma once
#include "OSG_Plus.h"
#include "publicDefined.h"
#include "InterfaceUserEventCallBack.h"
#include "StProgressSetter.h"

#ifdef CLIPLIBDLL  
#define CLIPAPI _declspec(dllexport)  
#else  
#define CLIPAPI  _declspec(dllimport)  
#endif  

class CLIPAPI InterfaceOsgClip
{
public:
	virtual bool clipSceneByScope(osg::ref_ptr<osg::MatrixTransform> mTrans, std::string inFilePath, std::string outputFilePath, std::vector<CScope> vecScope, 
		 CStProgressSetter stProgressSetter, bool isInside) = 0;																										//按范围删除

	virtual bool clipOsgHeader(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecScope, bool isInside) = 0;																//裁剪头结点

	virtual bool clipAccurateOsgHeader(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecScope, bool isInside ,bool bAccurate = true) = 0;														//精确裁切头节点

	virtual void obtainSelTileName(std::vector<CScope> vecScope, osg::ref_ptr<osg::Group> mGroup, osg::Matrix matrix, std::vector<std::string> &vecHalfInPagedLodFileName, 
		std::vector<std::string> &vecTotalInPagedLodFileName) = 0;

	virtual void obtainSelTileNamebyScope(std::vector<CScope> vecScope, osg::ref_ptr<osg::Group> mGroup, osg::Matrix matrix, std::vector<std::string> &vecHalfInPagedLodFileName, 
		std::vector<std::string> &vecTotalInPagedLodFileName) = 0;

	virtual std::vector<osg::Vec3d> getInsideVecVerts() = 0;
};


enum MODE {BY_POLYGON, BY_SUSPEND_SCOPE, BY_ABSOLUTE_POLYGON, BY_ACCURATE_POLYGON, BY_ACCURATE_SUSPEND_SCOPE}; //标签  

class CLIPAPI InterfaceOsgClipFactory
{
public:
	InterfaceOsgClip* create(MODE mode);
};

