#pragma once
#include "cv_plus.hpp"
#include "InterfaceOpencvDependency.h"

class OPENCVAPI OpencvDependency : public InterfaceOpencvDependeny
{
public:
	void OpencvCvmPrint(CvMat* M);
};

