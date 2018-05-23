#include "StdAfx.h"
#include "MFC_OSG.h"


InterfaceOsgMfcViewer* InterfaceOsgMfcViewerFactory::create(HWND hWnd)
{
	return new cOSG(hWnd);
}