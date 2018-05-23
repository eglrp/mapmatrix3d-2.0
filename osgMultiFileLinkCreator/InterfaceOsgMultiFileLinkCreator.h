#pragma once
#include "InterfaceUserEventCallBack.h"

#ifdef OSGLINKCREATORLIBDLL  
#define OSGLINKCREATORAPI _declspec(dllexport)  
#else  
#define OSGLINKCREATORAPI  _declspec(dllimport)  
#endif  

class OSGLINKCREATORAPI InterfaceOsgMultiFileLinkCreator
{
public:
	virtual void createLinkForMultiFile(InterfaceSigEventCallBack* iSigEventCallBack) = 0;									//对多目录生成索引

};

class OSGLINKCREATORAPI InterfaceOsgMultiFileLinkCreatorFactory
{
public:
	InterfaceOsgMultiFileLinkCreator* create();
};



