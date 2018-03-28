#include "StdAfx.h"
#include "ControlPoint.h"


CControlPoint::CControlPoint(int serialNum, double pX, double pY, double pZ, bool ischeckpoint)
{
	ptSerialNum = serialNum;
	ptX = pX;
	ptY = pY;
	ptZ = pZ;
	isCheckPoint = ischeckpoint;
}

