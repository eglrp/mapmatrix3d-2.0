#include "StdAfx.h"
#include "CallBackFunc.h"
#include "MapMatrix3D Editor.h"
#include "AbsoluteOrientationDlg.h"
#include "FileView.h"
#include "OPStatusBar.h"
#include "MeasureResultDlg.h"
#include "PropertyListDlg.h"
#include "publicDefined.h"
#include "qcomm.h"

extern CMeasureResultDlg* g_mfcMeasureResultDlg;
extern CAbsoluteOrientationDlg* g_mfcAbsoluteOrientationDlg;
extern OPStatusBar* g_mfcOpsStatusBar;
extern CPropertyListDlg* g_mfcPropertyListDlg;


void CallBackNotifyMeasureMsg::operator() (char* msg) const
{
	g_mfcMeasureResultDlg->notifyMeasureResult(msg);
}

void CallBackCloseMeasureDlg::operator() () const
{
	g_mfcMeasureResultDlg->closeDlg();
}

void CallBackCloseAbsDlg::operator() () const
{
	g_mfcAbsoluteOrientationDlg->closeDlg();
}

int CallBackSelModelIndex::operator() () const
{
	return theApp.selectCnt;
}

int CallBackGetSelRowFromAbsDlg::operator() () const
{
	int row = g_mfcAbsoluteOrientationDlg->ObtainSelectedRow();
	return row;
}

bool CallBackChangeCtrlList::operator() () const
{
	return g_mfcAbsoluteOrientationDlg->InsertControlListCompleteStatus();
}

int CallBackGetSelPtNumFromAbsDlg::operator() () const
{
	CString ptNumStr = g_mfcAbsoluteOrientationDlg->ObtainSelectedPtNum();
	int num = atoi(ptNumStr);

	return num;
}

void CallBackObtainRelativePoint::operator() (int ptNum, double xRelaPt, double yRelaPt, double zRelaPt, bool isCheckPt) const
{
	g_mfcAbsoluteOrientationDlg->ObtainModelRelaPoint(ptNum, xRelaPt, yRelaPt, zRelaPt, isCheckPt);
}

void CallBackSetProgressBarPos::operator() (int pos) const
{
	g_mfcOpsStatusBar->SetProgressBarPos(pos);
}

void CallBackShowOpsBarHint::operator()(char* strHint) const
{
	g_mfcOpsStatusBar->ShowHint(strHint);
}

void CallBackStartProgress::operator() () const
{
	theApp.StartProgressThread();
}

void CallBackEndProgress::operator() () const
{
	g_mfcOpsStatusBar->EndProgress();
}

void CallBackRemoveAllScene::operator() () const
{
	theApp.RemoveAllScene();
}

void CallBackAddScene::operator() (char* strName) const
{
	std::string strModel = strName;
	theApp.AddScene(strModel);
}

void CallBackInsertControlListRecord::operator() (int ptNum, double ptX, double ptY, double ptZ) const
{
	g_mfcAbsoluteOrientationDlg->InsertControlListRecord(ptNum, ptX, ptY, ptZ);

}


void CallBackPopUpPropertyListDlg::operator() (char* propertyListText) const
{
	int threadId = theApp.listenBoostSignalHandle->m_nThreadID;

	PostThreadMessageA(threadId, WM_POP_UP_PROPERTY_LIST, 0, (LPARAM)propertyListText);
}

void CallBackObjectMenuDlg::operator() (int mouseX, int mouseY) const
{
	int threadId = theApp.listenBoostSignalHandle->m_nThreadID;

	PostThreadMessageA(threadId, WM_SHOW_HIGHLIGHT_LIST, mouseX, mouseY);
}


