#pragma once
#include "OSG_Plus.h"
#include "InterfaceOsgMultiFileLinkCreator.h"

class OSGLINKCREATORAPI COsgMultiFileLinkCreator : public InterfaceOsgMultiFileLinkCreator
{
public:
	void createLinkForMultiFile(InterfaceSigEventCallBack* iSigEventCallBack);				

	void createLink(InterfaceSigEventCallBack* iSigEventCallBack);
protected:
	static void createMultiFileLink(void* ptr);
	void StartcreateLinkForMultiFile();
protected:
	InterfaceSigEventCallBack* iSigEventCallBack;
	std::string strFilePath;
};