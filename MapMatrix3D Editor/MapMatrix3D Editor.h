// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问  
// http://msdn.microsoft.com/officeui。
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// MapMatrix3D Editor.h : MapMatrix3D Editor 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号
#include "OSG_Plus.h"
#include <vector>
#include "MeasureResultDlg.h"
#include "InterfaceOsgMfcViewer.h"
#include "SevenParameter.h" 

// CMapMatrix3DEditorApp:
// 有关此类的实现，请参阅 MapMatrix3D Editor.cpp

class CMapMatrix3DEditorApp : public CWinAppEx
{
public:
	CMapMatrix3DEditorApp(); 

// 重写
public:
// 实现
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;
	std::vector<CString> vecStrFileName;
	std::vector<CString> vecSceneFileName;
	CString fileFolderPath;
	int clipCount;
	double reFactor;

	int selectCnt;

	int selectRowAbs;

	CWinThread* listenBoostSignalHandle;

	CSevenParameter sevenParamCalulator;

	InterfaceOsgMfcViewer* IOsgMfcViewer;

protected:
	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	DECLARE_MESSAGE_MAP()	
public:
	void createNewDoc();
	void ChangeModelViewWindowSizeToHalf();
	void ChangeModelViewWindowSizeToFull();	

	//void SearchFile(CString strDir);

	void ShowOrHideSceneGroup(int selCnt, bool bCheck);

	void StartProgressThread();

	void OnButtonStartInternetAbsOri();

	void RemoveAllScene();

	void AddScene(std::string strModel);

	void InitCallBack();

	bool isLoadModel();

	bool isSingleCheckModel();

	void PopUpLinkObjectPropertyListDlg(std::string strPropertyListText);

	void PopUpHighLightMenu();

public:  
	CMultiDocTemplate* pDocSceneTemplate;
	CMultiDocTemplate* pDocDOMTemplate;
private:
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	afx_msg void OnButtonMultifile();
	afx_msg void OnButtonPtMeasure();
	afx_msg void OnButtonMeasureLine();
	afx_msg void OnButtonMultiline();
	afx_msg void OnButtonMeasureArea();
	afx_msg void OnButtonMeshing();
	afx_msg void OnButtonHome();
	afx_msg void OnButtonLighting();
	afx_msg void OnOutputSingleScene();
	afx_msg void OnButtonShowFillTriangle();
	afx_msg void OnButtonModelMatching();
	afx_msg void OnButtonHollowOut();
	afx_msg void OnButtonStartHollowOut();
	afx_msg void OnButtonHollowOutSuspendedUp();
	afx_msg void OnButtonHollowOutSuspended();
	afx_msg void OnButtonStartHollowOutSuspended();
	afx_msg void OnButtonClipByPolygon();
	afx_msg void OnButtonStartClipPolygon();
	afx_msg void OnButtonRepairhole();
	afx_msg void OnButtonStartRepairHole();
	afx_msg void OnButtonFrontview();
	afx_msg void OnButtonLeftView();
	afx_msg void OnButtonBackView();
	afx_msg void OnButtonRightView();
	afx_msg void OnButtonLookDown();
	afx_msg void OnButtonLookup();
	afx_msg void OnButtonMirror();
	afx_msg void OnButtonUpsideDown();
	afx_msg void OnButtonAbsoluteOrientation();
	afx_msg void OnButtonClipByHeight();
	afx_msg void OnButtonInvertselect();
	afx_msg void OnButtonAutohorizon();
	afx_msg void OnButtonSceneStitching();
	afx_msg void OnSceneAdjustFlatPlane();
	afx_msg void OnAdjustHeight();
	afx_msg void OnButtonDomGenerate();
	afx_msg void OnUpdateButtonPtMeasure(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonMeasureArea(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonMeasureLine(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonMultiline(CCmdUI *pCmdUI);
	afx_msg void OnButtonExportHatching();
	afx_msg void OnButtonHandPaintLine();
	afx_msg void OnButtonExtractFineOutline();
	afx_msg void OnButtonExtractOutlineDrawArea();
	afx_msg void OnButtonExtractOutlineArea();
	afx_msg void OnButtonExtractOutlineAreaNewHeight();
	afx_msg void OnButtonExtractRoad();
	afx_msg void OnButtonDrawEdgeHand();
	afx_msg void OnButtonDrawEdgeHand1();
	afx_msg void OnButtonExtractRoadByDom();
	afx_msg void OnButtonHorizonAccurate();

private:
	void IsHasLicense();
	bool CheckPrerequisite();
	bool CheckIsSingleModel();
	bool CheckIsHorizon();
	bool CheckIsNotHorizon();
	void SetOpsNone();

private:
	CMeasureResultDlg *dlg;

	BOOL isInternetPick;//是否互联网刺点
public:
	BOOL isPtMeasure;
	BOOL isLineMeasure;
	BOOL isMultiLineMeasure;
	BOOL isAreaMeasure;
private:
	afx_msg void OnButtonDsmGene();
	afx_msg void OnButtonOutputScene();
	afx_msg void OnButtonReverse();
	afx_msg void OnButtonExtractHatch();
	afx_msg void OnMatchSingleModel();
	afx_msg void OnButtonSplitModel();
	afx_msg void OnButtonSelTileRemove();
	afx_msg void OnButtonInternetAbsOrientation();
	afx_msg void OnButtonIntenetFinish();
	afx_msg void OnButtonManualhorizon();
	afx_msg void OnButtonStartManualHorizon();
public:
	afx_msg void OnClearSingleScene();
	afx_msg void OnButtonExtractFineOutlineNewHeight();
	afx_msg void OnButtonSelectByRectangle();
	afx_msg void OnButtonSelectByPolygon();
	afx_msg void OnButtonSelectByClick();
	afx_msg void OnButtonExtractContour();
	afx_msg void OnButtonFittingWallspace();
	afx_msg void OnButtonWallInsect();
	afx_msg void OnButtonAutoSelectWallspace();
	afx_msg void OnButtonHandSelectWallspace();
	afx_msg void OnButtonDrawHorizLine();
	afx_msg void OnButtonManualExtractTexture();
	afx_msg void OnButtonCombineGroup();
	afx_msg void OnButtonStartExtractTexture();
	afx_msg void OnButtonWholeModelMatching();
	afx_msg void OnButtonStartWholeModelMatching();
	afx_msg void OnButtonLineInsect();
	afx_msg void OnButtonExtractObjectByPolygon();
	afx_msg void OnButtonStartExtract();
	afx_msg void OnButtonStartQuickClipModel();
	afx_msg void OnButtonQuickClipModelByTriangle();
	afx_msg void OnButtonQuickDeleteSingleTriangle();
	afx_msg void OnButtonQuickDeleteTriangleByLine();
	afx_msg void OnButtonQuickDeleteTriangleBySolid();
	afx_msg void OnButtonStartQuickDeleteTriangleBySolid();
	afx_msg void OnButtonAbsoluteClipSceneByPolygon();
	afx_msg void OnButtonStartAbsoluteClipSceneByPolygon();
	afx_msg void OnButtonStartFlatSceneByPolygon();
	afx_msg void OnButtonFlatSceneByPolygon();
	afx_msg void OnButtonHighlightObject();
	afx_msg void OnButtonLinkObjectProperty();
	afx_msg void OnButtonExtractPtCloudEdge();
	afx_msg void OnButtonExtractPtCloud();
	afx_msg void OnButtonSelectPtCloudByArea();
	afx_msg void OnButtonSetPtCloudDlg();
	afx_msg void OnButtonLookupObjectProperty();
	afx_msg void OnButtonOutputModelWithoutTexture();
	afx_msg void OnButtonSimplifyObjectByPolygon();
	afx_msg void OnButtonStartSimplifyObject();
	afx_msg void OnButtonOutput3dsModel();
	afx_msg void OnButtonExtractArea();
	afx_msg void OnOutTextureObj();
	afx_msg void OnOutNoTextureObj();
	afx_msg void OnDeleteObj();
	afx_msg void OnButtonExtractcontour();
	afx_msg void OnButtonExtractOutlinePg();
	afx_msg void OnButtonImportData();
	afx_msg void OnButtonHatchSetting();
};

extern CMapMatrix3DEditorApp theApp;
