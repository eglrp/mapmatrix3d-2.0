#pragma once
#include <string>
#include "InterfaceUserEventCallBack.h"
#include "OSG_Plus.h"

class CFileSelector
{
public:
	virtual bool selectDir();

	virtual bool isSameAsOutputFileName(std::string inFilePath, std::string headOutputFileName);

	virtual void reloadScene(InterfaceSigEventCallBack* iSigEventCallBack, std::string headOutputFileName);

	virtual void createClipOutputFolder(osg::ref_ptr<osg::MatrixTransform> mTrans, std::string headOutputFilePath);

	std::string getSelFileName() const;

	std::string getClipOutputFilePath() const;

private:
	std::string selFileName;

	std::string clipOutputFilePath;
};

