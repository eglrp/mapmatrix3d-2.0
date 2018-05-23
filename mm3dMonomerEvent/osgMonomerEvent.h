#pragma once
#include "InterfaceOsgMonomerEvent.h"
#include "BaseUserEvent.h"

class COsgMonomerEvent : public InterfaceOsgMonomerEvent
{
public:
	COsgMonomerEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> EditGroup, osg::ref_ptr<osg::Group> ObjectGroup, InterfaceSigEventCallBack* iSigEventCallBack);
	
	virtual void extractObject(std::string inFilePathName);																					//接口， 单体化物体

	virtual void extractArea(std::string inFilePathName);

	virtual void flatByPolygon(std::string inFilePath);																						//接口， 压平场景

	virtual void addEventToViewer(osg::ref_ptr<osgViewer::Viewer> viewer);		

	virtual void setObjectProperty(std::string strObjectProperty);

	virtual void lookUpOjbectProperty(std::string objectPropertyName, std::string objectPropertyValue);

	virtual void outputModelWithoutTexture(std::string outModelWithoutFileName);

	virtual void outputTextureModel(std::string outModelWithoutFileName);

	virtual	void removeSelPagedLod();

private:
	std::vector<CBaseUserEvent*> vecMonomerEvent;
};
