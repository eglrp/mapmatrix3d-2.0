#pragma once
#include "publicDefined.h"

class OPSAPI CControlPoint
{
public:
	int ptSerialNum;
	double ptX;
	double ptY;
	double ptZ;
	bool isCheckPoint;
public:
	CControlPoint(int serialNum, double pX, double pY, double pZ, bool ischeckpoint);
};

