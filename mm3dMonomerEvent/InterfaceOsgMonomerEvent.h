#pragma once
#include "OSG_Plus.h"
#include "InterfaceUserEventCallBack.h"


#ifdef MONOMEREVENTLIBDLL  
#define MONOMEREVENTAPI _declspec(dllexport)  
#else  
#define MONOMEREVENTAPI  _declspec(dllimport)  
#endif 


class MONOMEREVENTAPI InterfaceOsgMonomerEvent
{
public:
	virtual void addEventToViewer(osg::ref_ptr<osgViewer::Viewer> viewer) = 0;

	virtual void extractObject(std::string inFilePathName) = 0;																					//接口， 单体化物体

	virtual void extractArea(std::string inFilePathName) = 0;																												//接口，单体化区域

	virtual void flatByPolygon(std::string inFilePath) = 0;																						//接口， 压平场景

	virtual void setObjectProperty(std::string strObjectProperty) = 0;																			//接口， 设置属性

	virtual void lookUpOjbectProperty(std::string objectPropertyName, std::string objectPropertyValue) = 0;										//接口， 查询属性

	virtual void outputModelWithoutTexture(std::string outModelWithoutFileName) = 0;

	virtual void outputTextureModel(std::string outModelWithoutFileName) = 0;

	virtual	void removeSelPagedLod() = 0;
};


class MONOMEREVENTAPI InterfaceOsgMonomerEventFactory
{
public:
	InterfaceOsgMonomerEvent* create(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> EditGroup, osg::ref_ptr<osg::Group> ObjectGroup,
		InterfaceSigEventCallBack* iSigEventCallBack);
};
