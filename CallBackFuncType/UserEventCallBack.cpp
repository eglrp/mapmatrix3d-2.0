#include "StdAfx.h"
#include "UserEventCallBack.h"

pNotifyMsgFunc DllNotifyFun(pNotifyMsgFunc pFun)
{
	return pFun;
}

pGetSelIndexFunc DllGetSelModelIndexFun(pGetSelIndexFunc pFun)
{
	return pFun;
}

pGetSelIndexFunc DllGetSelAbsDlgRowFun(pGetSelIndexFunc pFun)
{
	return pFun;
}

pChangeStatus DllChangeStatusFun(pChangeStatus pFun)
{
	return pFun;
}

pObtainRelativePoint DllOtainRelativePt(pObtainRelativePoint pFun)
{
	return pFun;
}