#pragma once
#include "OSG_Plus.h"
#include "InterfaceOsgDsmGene.h"

class osgDSMGene : public InterfaceOsgDsmGene
{
public:
	void GenerateDSM(osg::ref_ptr<osg::Node> node, std::string strSceneFileName, double xRes, double yRes, InterfaceSigEventCallBack* ISigCallBack);

protected:
	static void Produce(void* ptr);

	void StartProducingDSM();

protected:
	InterfaceSigEventCallBack* iSigCallBack;
	osg::ref_ptr<osg::Node> SceneNode;
	std::string RootModelPath;
	double XRES;
	double YRES;
	std::string OutputFileName;
};

