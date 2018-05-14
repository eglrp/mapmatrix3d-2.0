#pragma once
#include "cv_plus.hpp"
#include "operationState.h"

#ifdef OPENCVLIBDLL  
#define OPENCVAPI _declspec(dllexport)  
#else  
#define OPENCVAPI  _declspec(dllimport)  
#endif  


class OPENCVAPI InterfaceOpencvDependeny 
{
public:
	virtual void OpencvCvmPrint(CvMat* M) = 0;
};

class OPENCVAPI InterfaceOpencvDependenyFactory 
{
public:
	InterfaceOpencvDependeny* create();
};


