

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

// MapMatrix3D Editor.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "OPStatusBar.h"
#include "MapMatrix3D Editor.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "MapMatrix3D EditorDoc.h"
#include "MapMatrix3D EditorView.h"
#include "DomDoc.h"
#include "qcomm.h"
#include "SplashWnd.h"
#include "AbsoluteOrientationDlg.h"
#include "InterfaceOsgEditor.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceOsgHorizon.h"
#include "InterfaceOsgOrient.h"
#include "InterfaceOsgStitcher.h"
#include "InterfaceOsgDsmGene.h"
#include "InterfaceOsgDomGenerator.h"
#include "InterfaceMFCExcute.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceViewerSlot.h"
#include "osgEarthAppView.h"
#include "FileView.h"
#include "DOMResSetDlg.h"
#include "AbsoluteOrientationDlg.h"
#include "MeasureResultDlg.h"
#include "HeightSetDlg.h"
#include "SetPtCloudDlg.h"
#include "resource.h"
#include "compass.h"
#include "CallBackFunc.h"
#include "osgExtractHatchingEvent.h"
#include "InterfaceOsgMultiFileLinkCreator.h"
#include "ThreadSafeUpdateCallback.h"
#include "PropertyListDlg.h"
#include "publicDefined.h"
#include "InterfaceCEncrypt.h"
#include "SetPTCloudDLG.h"
#include "LookUpObjectPropertyDlg.h"
#include "InterfaceOsgFinder.h"
#include <osg/ComputeBoundsVisitor>
#include "DlgSetContour.h"
#include "DLGHatchSetting.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CMeasureResultDlg* g_mfcMeasureResultDlg;
extern CAbsoluteOrientationDlg* g_mfcAbsoluteOrientationDlg;
extern OPStatusBar* g_mfcOpsStatusBar; 

#define RET_LEN sizeof(DWORD)+sizeof(LONG)+sizeof(LONG)+sizeof(int)+sizeof(float)

typedef LANGID(WINAPI *FP_SetThreadUILanguage)(LANGID LangId);

UINT ChangeProgressBar(LPVOID pParam)
{
	int elapseTime = 50;
	g_mfcOpsStatusBar->StartProgress(elapseTime);
	return 0;
}

UINT ListenBoostSignal(LPVOID pParam)
{
	//函数体
	while (true)
	{
		MSG msg = { 0 };
		PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

		switch (msg.message)
		{
			case WM_POP_UP_PROPERTY_LIST:
			{
				PostMessage(AfxGetMainWnd()->m_hWnd, WM_SHOW_PROPERTY_LIST_DLG, 0, msg.lParam);
				break;
			}
			case WM_SHOW_HIGHLIGHT_LIST:
			{
				PostMessage(AfxGetMainWnd()->m_hWnd, WM_POP_UP_HIGHLIGHT_LIST, msg.wParam, msg.lParam);
				break;
			}

		}

		Sleep(100);
	}

	return 0;
}


class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
} 

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

// CMapMatrix3DEditorApp

extern CFileView* g_mfcFileView;

BOOL LaunchProc(CString strCmd11, int nCmdSHow)
{
	std::string strCmdLine = strCmd11;
	STARTUPINFO stStartUpInfo;
	memset(&stStartUpInfo, 0, sizeof(STARTUPINFO));	
	stStartUpInfo.cb = sizeof(STARTUPINFO);
	stStartUpInfo.dwFlags = STARTF_USESHOWWINDOW;
	stStartUpInfo.wShowWindow = nCmdSHow;
	PROCESS_INFORMATION infoProcess;
	CString strCmd = strCmdLine.c_str();
	LPSTR lpCmdline = (LPSTR)(LPCSTR)strCmd;
	CreateProcess(NULL, lpCmdline, NULL, NULL, false, NORMAL_PRIORITY_CLASS, NULL, NULL, &stStartUpInfo, &infoProcess);

	bool bTerminate = false;

	while(true)
	{
		DWORD dwResult = WaitForSingleObject(infoProcess.hProcess, INFINITE);

		//根据返回值判断
		switch(dwResult)
		{
		case WAIT_OBJECT_0:
			{
				GetExitCodeProcess(infoProcess.hProcess, &dwResult);
				return dwResult == 0;
			}	
		case  WAIT_FAILED:
			{
				return FALSE;
			}
		default:
			{
				long bCanceled = FALSE;
				//获取是否任务被取消
				if (bCanceled)
				{
					if (!bTerminate)
					{
						TerminateProcess(infoProcess.hProcess, 1);
						bTerminate = true;
					}
				}
			}

			break;
		}
	}
	return FALSE;
}

BOOL CheckWinLicense()
{
#ifndef _DEBUG
	CFile file;
	BOOL bRet = FALSE;
	//检查 DATLincenseCheck - DATPlatform1.exe
	//保存当前文件目录
	TCHAR currentPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, currentPath);

	//获得当前程序目录
	char chExePath[_MAX_PATH];
	::GetModuleFileName(NULL, chExePath, _MAX_PATH);

	//DATMatrix.exe目录
	CString szDATMatrixExePath(chExePath);
	szDATMatrixExePath = szDATMatrixExePath.Left(szDATMatrixExePath.ReverseFind(_T('\\'))+1);

	DWORD dwRet = 0x19880826;
	LONG lCx = 256;
	LONG lCy = 256;
	int iLev = 0;
	float fZoom = 1.0f;

	//运行DATPlatform1.exe
	if(!LaunchProc(szDATMatrixExePath + _T("DATPlatform1.exe -SHOWHIDE"), SW_SHOW))
	{
		goto LABLE_CheckWinLicense_END;
	}

	//写入DATPlatform1.dll文件
	BYTE buf[RET_LEN];
	memcpy_s(buf+0,														sizeof(DWORD),	&dwRet,	sizeof(DWORD));
	memcpy_s(buf+sizeof(DWORD),											sizeof(LONG),	&lCx,	sizeof(LONG));
	memcpy_s(buf+sizeof(DWORD)+sizeof(LONG),							sizeof(LONG),	&lCy,	sizeof(LONG));
	memcpy_s(buf+sizeof(DWORD)+sizeof(LONG)+sizeof(LONG),				sizeof(int),	&iLev,	sizeof(int));
	memcpy_s(buf+sizeof(DWORD)+sizeof(LONG)+sizeof(LONG)+sizeof(int),	sizeof(float),	&fZoom,	sizeof(float));

	if(!file.Open(szDATMatrixExePath+_T("DATPlatform1.dll"),CFile::modeReadWrite|CFile::typeBinary))
	{
		goto LABLE_CheckWinLicense_END;
	}

	file.Seek(62912,CFile::begin);
	file.Write(buf,RET_LEN);
	file.Close();

	//检查结果
	if(!file.Open(szDATMatrixExePath+_T("DATPlatform1.dll"),CFile::modeReadWrite|CFile::typeBinary))
	{
		goto LABLE_CheckWinLicense_END;
	}

	file.Seek(62912, CFile::begin);
	file.Read(buf, RET_LEN);
	file.Close();

	memcpy_s(&dwRet,sizeof(DWORD),buf,sizeof(DWORD));

	if(dwRet!=0x19841101)
	{
		BYTE src[RET_LEN]= {0x10,0x5F,0x5E,0x5B,  0x8B,0xE5,0x5D,0xC2, 0x08,0x00,0xCC,0xCC, 0xCC,0xCC,0xCC,0xCC, 0x55,0x8B,0xEC,0x6A};

		if (memcmp(buf,src,RET_LEN)!=0)
		{
			if(file.Open(szDATMatrixExePath+_T("DATPlatform1.dll"),CFile::modeReadWrite|CFile::typeBinary))
			{
				file.Seek(62912,CFile::begin);
				file.Write(src,RET_LEN);
				file.Close();
			}
		}

		goto LABLE_CheckWinLicense_END;
	}

LABLE_CheckWinLicense_END:
	LONG iNeedWidth = 0;
	LONG iNeedHeight = 0;
	memcpy_s(&iNeedWidth,sizeof(LONG),buf + sizeof(DWORD),sizeof(LONG));
	memcpy_s(&iNeedHeight,sizeof(LONG),buf + sizeof(DWORD) +sizeof(LONG),sizeof(LONG));
	SetCurrentDirectory(currentPath);

	return (iNeedWidth==256 && iNeedHeight==256);

#endif
	return TRUE;
}

void SetLanguage(HMODULE hModule)
{
	TCHAR szFileName[1000];
	GetModuleFileName(hModule, szFileName, MAX_PATH);
	TCHAR *pStr = _tcsrchr(szFileName, '\\');
	if (NULL != pStr)*pStr = '\0';
	_tcscat(szFileName, _T("\\Resource.dat"));
	FILE *fp = _tfopen(szFileName, _T("rt"));

	char line[1000] = { 0 };
	if (fp)
	{
		fgets(line, sizeof(line)-1, fp);
		fclose(fp);
	}

	strupr(line);

	DWORD dwID;
	if (NULL != strstr(line, "ENGLISH"))
	{
		WORD id = MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT);
		dwID = MAKELCID(id, SORT_DEFAULT);
	}
	else if (NULL != strstr(line, "JAPANESE"))
	{
		WORD id = MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT);
		dwID = MAKELCID(id, SORT_DEFAULT);
	}
	else // CHINESE
	{
		WORD id = MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED);
		dwID = MAKELCID(id, SORT_CHINESE_PRC);
	}

	HMODULE hKernel32 = GetModuleHandle(_T("Kernel32.dll"));
	FARPROC pFn = GetProcAddress(hKernel32, "SetThreadUILanguage");

	FP_SetThreadUILanguage pSetThreadUILanguage = (FP_SetThreadUILanguage)pFn;
	if (pSetThreadUILanguage)pSetThreadUILanguage(dwID);

	SetThreadLocale(dwID);
} 

BEGIN_MESSAGE_MAP(CMapMatrix3DEditorApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CMapMatrix3DEditorApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	ON_COMMAND(ID_FILE_NEW, &CMapMatrix3DEditorApp::OnFileNew)
	ON_COMMAND(ID_BUTTON_MULTIFILE, &CMapMatrix3DEditorApp::OnButtonMultifile)
	ON_COMMAND(ID_BUTTON_PT_MEASURE, &CMapMatrix3DEditorApp::OnButtonPtMeasure)
	ON_COMMAND(ID_BUTTON_MEASURE_LINE, &CMapMatrix3DEditorApp::OnButtonMeasureLine)
	ON_COMMAND(ID_BUTTON_MULTILINE, &CMapMatrix3DEditorApp::OnButtonMultiline)
	ON_COMMAND(ID_BUTTON_MEASURE_AREA, &CMapMatrix3DEditorApp::OnButtonMeasureArea)
	ON_COMMAND(ID_BUTTON_Meshing, &CMapMatrix3DEditorApp::OnButtonMeshing)
	ON_COMMAND(ID_BUTTON_HOME, &CMapMatrix3DEditorApp::OnButtonHome)
	ON_COMMAND(ID_BUTTON_LIGHTING, &CMapMatrix3DEditorApp::OnButtonLighting)
	ON_COMMAND(ID_OUTPUT_SINGLE_SCENE, &CMapMatrix3DEditorApp::OnOutputSingleScene)
	ON_COMMAND(ID_BUTTON_SHOW_FILL_TRIANGLE, &CMapMatrix3DEditorApp::OnButtonShowFillTriangle)
	ON_COMMAND(ID_BUTTON_MODEL_MATCHING, &CMapMatrix3DEditorApp::OnButtonModelMatching)
	ON_COMMAND(ID_BUTTON_HOLLOW_OUT, &CMapMatrix3DEditorApp::OnButtonHollowOut)
	ON_COMMAND(ID_BUTTON_START_HOLLOW_OUT, &CMapMatrix3DEditorApp::OnButtonStartHollowOut)
	ON_COMMAND(ID_BUTTON_HOLLOW_OUT_SUSPENDED, &CMapMatrix3DEditorApp::OnButtonHollowOutSuspended)
	ON_COMMAND(ID_BUTTON_HOLLOW_OUT_SUSPENDED_UP, &CMapMatrix3DEditorApp::OnButtonHollowOutSuspendedUp)
	ON_COMMAND(ID_BUTTON_START_HOLLOW_OUT_SUSPEND, &CMapMatrix3DEditorApp::OnButtonStartHollowOutSuspended)
	ON_COMMAND(ID_BUTTON_CLIP_BY_POLYGON, &CMapMatrix3DEditorApp::OnButtonClipByPolygon)
	ON_COMMAND(ID_BUTTON_START_CLIP, &CMapMatrix3DEditorApp::OnButtonStartClipPolygon)
	ON_COMMAND(ID_BUTTON_REPAIRHOLE, &CMapMatrix3DEditorApp::OnButtonRepairhole)
	ON_COMMAND(ID_BUTTON_START_REPAIR_HOLE, &CMapMatrix3DEditorApp::OnButtonStartRepairHole)
	ON_COMMAND(ID_BUTTON_FRONTVIEW, &CMapMatrix3DEditorApp::OnButtonFrontview)
	ON_COMMAND(ID_BUTTON_LEFT_VIEW, &CMapMatrix3DEditorApp::OnButtonLeftView)
	ON_COMMAND(ID_BUTTON_BACK_VIEW, &CMapMatrix3DEditorApp::OnButtonBackView)
	ON_COMMAND(ID_BUTTON_RIGHT_VIEW, &CMapMatrix3DEditorApp::OnButtonRightView)
	ON_COMMAND(ID_BUTTON_LOOK_DOWN, &CMapMatrix3DEditorApp::OnButtonLookDown)
	ON_COMMAND(ID_BUTTON_LOOKUP, &CMapMatrix3DEditorApp::OnButtonLookup) 
	ON_COMMAND(ID_BUTTON_MIRROR, &CMapMatrix3DEditorApp::OnButtonMirror)
	ON_COMMAND(ID_BUTTON_UPSIDE_DOWN, &CMapMatrix3DEditorApp::OnButtonUpsideDown)
	ON_COMMAND(ID_BUTTON_ABSOLUTE_ORIENTATION, &CMapMatrix3DEditorApp::OnButtonAbsoluteOrientation)
	ON_COMMAND(ID_BUTTON_CLIP_BY_HEIGHT, &CMapMatrix3DEditorApp::OnButtonClipByHeight)
	ON_COMMAND(ID_BUTTON_InvertSelect, &CMapMatrix3DEditorApp::OnButtonInvertselect)
	ON_COMMAND(ID_BUTTON_AUTOHORIZON, &CMapMatrix3DEditorApp::OnButtonAutohorizon)
	ON_COMMAND(ID_BUTTON_SCENE_STITCHING, &CMapMatrix3DEditorApp::OnButtonSceneStitching)
	ON_COMMAND(ID_SCENE_ADJUST_FLAT_PLANE, &CMapMatrix3DEditorApp::OnSceneAdjustFlatPlane)
	ON_COMMAND(ID_ADJUST_HEIGHT, &CMapMatrix3DEditorApp::OnAdjustHeight)
	ON_COMMAND(ID_BUTTON_DOM_GENERATE, &CMapMatrix3DEditorApp::OnButtonDomGenerate)
	ON_COMMAND(ID_BUTTON_DSM_GENE, &CMapMatrix3DEditorApp::OnButtonDsmGene)
	ON_COMMAND(ID_BUTTON_OUTPUT_SCENE, &CMapMatrix3DEditorApp::OnButtonOutputScene)
	ON_COMMAND(ID_BUTTON_REVERSE, &CMapMatrix3DEditorApp::OnButtonReverse)
	ON_COMMAND(ID_BUTTON_EXTRACT_HATCH, &CMapMatrix3DEditorApp::OnButtonExtractHatch)
	ON_COMMAND(ID_MATCH_SINGLE_MODEL, &CMapMatrix3DEditorApp::OnMatchSingleModel)
	ON_COMMAND(ID_BUTTON_INTERNET_ABS_ORIENTATION, &CMapMatrix3DEditorApp::OnButtonInternetAbsOrientation)
	ON_COMMAND(ID_BUTTON_SPLIT_MODEL, &CMapMatrix3DEditorApp::OnButtonSplitModel)
	ON_COMMAND(ID_BUTTON_SEL_TILE_REMOVE, &CMapMatrix3DEditorApp::OnButtonSelTileRemove)
	ON_COMMAND(ID_BUTTON_INTERNET, &CMapMatrix3DEditorApp::OnButtonStartInternetAbsOri)
	ON_COMMAND(ID_BUTTON_INTENET_FINISH, &CMapMatrix3DEditorApp::OnButtonIntenetFinish)
	ON_COMMAND(ID_BUTTON_manualHORIZON, &CMapMatrix3DEditorApp::OnButtonManualhorizon)
	ON_COMMAND(ID_BUTTON_StartManualHorizon, &CMapMatrix3DEditorApp::OnButtonStartManualHorizon)
	ON_COMMAND(ID_BUTTON_EXPORT_HATCHING, &CMapMatrix3DEditorApp::OnButtonExportHatching)
	ON_COMMAND(ID_BUTTON_HANDPAINT_LINE, &CMapMatrix3DEditorApp::OnButtonHandPaintLine)
	ON_COMMAND(ID_Clear_Single_Scene, &CMapMatrix3DEditorApp::OnClearSingleScene)
	ON_COMMAND(ID_BUTTON_EXTRACT_FINE_OUTLINE, &CMapMatrix3DEditorApp::OnButtonExtractFineOutline)
	ON_COMMAND(ID_BUTTON_CHANGE_HEIGHT, &CMapMatrix3DEditorApp::OnButtonExtractFineOutlineNewHeight)
	ON_COMMAND(ID_BUTTON_SELECT_BY_RECTANGLE, &CMapMatrix3DEditorApp::OnButtonSelectByRectangle)
	ON_COMMAND(ID_BUTTON_SELECT_BY_POLYGON, &CMapMatrix3DEditorApp::OnButtonSelectByPolygon)
	ON_COMMAND(ID_BUTTON_SELECT_BY_CLICK,&CMapMatrix3DEditorApp::OnButtonSelectByClick)

	ON_UPDATE_COMMAND_UI(ID_BUTTON_PT_MEASURE, &CMapMatrix3DEditorApp::OnUpdateButtonPtMeasure)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_MEASURE_AREA, &CMapMatrix3DEditorApp::OnUpdateButtonMeasureArea)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_MEASURE_LINE, &CMapMatrix3DEditorApp::OnUpdateButtonMeasureLine)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_MULTILINE, &CMapMatrix3DEditorApp::OnUpdateButtonMultiline)

	ON_COMMAND(ID_BUTTON_EXTRACT_CONTOUR, &CMapMatrix3DEditorApp::OnButtonExtractContour)
	ON_COMMAND(ID_BUTTON_SELECT_WALL_AUTO, &CMapMatrix3DEditorApp::OnButtonAutoSelectWallspace)
	ON_COMMAND(ID_BUTTON_SELECT_WALL_HAND,&CMapMatrix3DEditorApp::OnButtonHandSelectWallspace)
	ON_COMMAND(ID_BUTTON_DRAW_HORIZLINE,&CMapMatrix3DEditorApp::OnButtonDrawHorizLine)	
	ON_COMMAND(ID_BUTTON_Manual_Extract_Texture, &CMapMatrix3DEditorApp::OnButtonManualExtractTexture)
	ON_COMMAND(ID_BUTTON_COMBINE, &CMapMatrix3DEditorApp::OnButtonCombineGroup)
	ON_COMMAND(ID_BUTTON_WALLINSECT,&CMapMatrix3DEditorApp::OnButtonWallInsect)
	ON_COMMAND(ID_BUTTON_FITTING_WALLSPACE,&CMapMatrix3DEditorApp::OnButtonFittingWallspace)
	ON_COMMAND(ID_BUTTON_Manual_Start_Extract_Texture, &CMapMatrix3DEditorApp::OnButtonStartExtractTexture)
	ON_COMMAND(ID_BUTTON_WHOLE_MODEL_MATCHING, &CMapMatrix3DEditorApp::OnButtonWholeModelMatching)
	ON_COMMAND(ID_BUTTON_START_MATCH_WHOLE_MODEL, &CMapMatrix3DEditorApp::OnButtonStartWholeModelMatching)
	ON_COMMAND(ID_BUTTON_LINE_INSECT,&CMapMatrix3DEditorApp::OnButtonLineInsect)
	ON_COMMAND(ID_BUTTON_EXTRACT_OBJECT_BY_POLYGON, &CMapMatrix3DEditorApp::OnButtonExtractObjectByPolygon)
	ON_COMMAND(ID_BUTTON_START_EXTRACT_OBJECT, &CMapMatrix3DEditorApp::OnButtonStartExtract)
	ON_COMMAND(ID_BUTTON_START_QUICK_CLIP_MODEL, &CMapMatrix3DEditorApp::OnButtonStartQuickClipModel)
	ON_COMMAND(ID_BUTTON_QUICK_CLIP_MODEL_BY_TRIANGLE, &CMapMatrix3DEditorApp::OnButtonQuickClipModelByTriangle)
	ON_COMMAND(ID_BUTTON_QUICK_DELETE_SINGLE_TRIANGLE, &CMapMatrix3DEditorApp::OnButtonQuickDeleteSingleTriangle)
	ON_COMMAND(ID_BUTTON_QUICK_DELETE_TRIANGLE_BY_LINE, &CMapMatrix3DEditorApp::OnButtonQuickDeleteTriangleByLine)
	ON_COMMAND(ID_BUTTON_QUICK_DELETE_TRIANGLE_BY_SOLID, &CMapMatrix3DEditorApp::OnButtonQuickDeleteTriangleBySolid)
	ON_COMMAND(ID_BUTTON_START_QUICK_DELETE_TRIANGLE_BY_SOLID, &CMapMatrix3DEditorApp::OnButtonStartQuickDeleteTriangleBySolid)
	ON_COMMAND(ID_BUTTON_ABSOLUTE_CLIP_SCENE_BY_POLYGON, &CMapMatrix3DEditorApp::OnButtonAbsoluteClipSceneByPolygon)
	ON_COMMAND(ID_BUTTON_START_ABSOLUTE_CLIP_SCENE_BY_POLYGON, &CMapMatrix3DEditorApp::OnButtonStartAbsoluteClipSceneByPolygon)
	ON_COMMAND(ID_BUTTON_START_FLAT_SCENE_BY_POLYGON, &CMapMatrix3DEditorApp::OnButtonStartFlatSceneByPolygon)
	ON_COMMAND(ID_BUTTON_FLAT_SCENE_BY_POLYGON, &CMapMatrix3DEditorApp::OnButtonFlatSceneByPolygon)
	ON_COMMAND(ID_BUTTON_EXTRACT_OUTLINE_DRAW_AREA, &CMapMatrix3DEditorApp::OnButtonExtractOutlineDrawArea)
	ON_COMMAND(ID_BUTTON_EXTRACT_FINE_OUTLINE_IN_AREA, &CMapMatrix3DEditorApp::OnButtonExtractOutlineArea)
	ON_COMMAND(ID_BUTTON_CHANGE_HEIGHT_IN_AREA, &CMapMatrix3DEditorApp::OnButtonExtractOutlineAreaNewHeight)
	ON_COMMAND(ID_BUTTON_HIGHLIGHT_OBJECT, &CMapMatrix3DEditorApp::OnButtonHighlightObject)
	ON_COMMAND(ID_BUTTON_LINK_OBJECT_PROPERTY, &CMapMatrix3DEditorApp::OnButtonLinkObjectProperty)
	ON_COMMAND(ID_BUTTON_EXTRACT_PTCLOUD_EDGE, &CMapMatrix3DEditorApp::OnButtonExtractPtCloudEdge)
	ON_COMMAND(ID_BUTTON_EXTRACT_PTCLOUD, &CMapMatrix3DEditorApp::OnButtonExtractPtCloud)
	ON_COMMAND(ID_BUTTON_SELECT_PTCLOUD_AREA, &CMapMatrix3DEditorApp::OnButtonSelectPtCloudByArea)
	ON_COMMAND(ID_BUTTON_SET_PTCLOUD_DLG, &CMapMatrix3DEditorApp::OnButtonSetPtCloudDlg)
	ON_COMMAND(ID_BUTTON_LOOKUP_OBJECT_PROPERTY, &CMapMatrix3DEditorApp::OnButtonLookupObjectProperty)
	ON_COMMAND(ID_BUTTON_OUTPUT_MODEL_WITHOUT_TEXTURE, &CMapMatrix3DEditorApp::OnButtonOutputModelWithoutTexture)
	ON_COMMAND(ID_BUTTON_SIMPLIFY_OBJECT_BY_POLYGON, &CMapMatrix3DEditorApp::OnButtonSimplifyObjectByPolygon)
	ON_COMMAND(ID_BUTTON_START_SIMPLIFY_OBJECT, &CMapMatrix3DEditorApp::OnButtonStartSimplifyObject)
	ON_COMMAND(ID_BUTTON_OUTPUT_3DS_MODEL, &CMapMatrix3DEditorApp::OnButtonOutput3dsModel)
	ON_COMMAND(ID_BUTTON_EXTRACT_AREA, &CMapMatrix3DEditorApp::OnButtonExtractArea)
	ON_COMMAND(ID_BUTTON_EXTRACT_ROAD, &CMapMatrix3DEditorApp::OnButtonExtractRoad)
	ON_COMMAND(ID_BUTTON_DRAW_EDGE_HAND, &CMapMatrix3DEditorApp::OnButtonDrawEdgeHand)
	ON_COMMAND(ID_BUTTON_DRAW_EDGE_HAND_1, &CMapMatrix3DEditorApp::OnButtonDrawEdgeHand1)
	ON_COMMAND(ID_BUTTON_HORIZON_ACCURATE, &CMapMatrix3DEditorApp::OnButtonHorizonAccurate)
	ON_COMMAND(ID_OUT_TEXTURE_OBJ, &CMapMatrix3DEditorApp::OnOutTextureObj)
	ON_COMMAND(ID_OUT_NO_TEXTURE_OBJ, &CMapMatrix3DEditorApp::OnOutNoTextureObj)
	ON_COMMAND(ID_DELETE_OBJ, &CMapMatrix3DEditorApp::OnDeleteObj)
	ON_COMMAND(ID_BUTTON_EXTRACT_ROAD_BY_DOM,&CMapMatrix3DEditorApp::OnButtonExtractRoadByDom)
	ON_COMMAND(IDC_BUTTON_EXTRACTCONTOUR, &CMapMatrix3DEditorApp::OnButtonExtractcontour)
	ON_COMMAND(ID_BUTTON_HATCH_SETTING, &CMapMatrix3DEditorApp::OnButtonHatchSetting)
	ON_COMMAND(ID_BUTTON_EXTRACT_OUTLINE_PG, &CMapMatrix3DEditorApp::OnButtonExtractOutlinePg)
	ON_COMMAND(ID_BUTTON_IMPORT_DATA, &CMapMatrix3DEditorApp::OnButtonImportData)
	END_MESSAGE_MAP()

// CMapMatrix3DEditorApp 构造

CMapMatrix3DEditorApp::CMapMatrix3DEditorApp()
{
	m_bHiColorIcons = TRUE;
	isInternetPick = FALSE;
	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("MapMatrix3D Editor.AppID.NoVersion"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	isPtMeasure = FALSE;
	isAreaMeasure = FALSE;
	fileFolderPath = _T("");
	selectCnt = 0;
	selectRowAbs = -1;
	reFactor = 1;
	listenBoostSignalHandle = NULL;

	//开启一个线程， 循坏的进行弹出对话框的操作
	
}

// 唯一的一个 CMapMatrix3DEditorApp 对象

CMapMatrix3DEditorApp theApp;

// CMapMatrix3DEditorApp 初始化

BOOL CMapMatrix3DEditorApp::InitInstance()
{
	CWinAppEx::InitInstance();

	int width = GetSystemMetrics ( SM_CXSCREEN ); 
	int height= GetSystemMetrics ( SM_CYSCREEN ); 
	if (width < 1152 )
	{
		AfxMessageBox(_T("屏幕分辨率过低，请调整分辨率重新启动软件"),MB_ICONSTOP|MB_OK );
		exit(0);
	}

	SetLanguage(NULL);

	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	EnableTaskbarInteraction();

	// 使用 RichEdit 控件需要  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)
	m_bSaveState=FALSE;
	CleanState();

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();

	LOGFONT logfont = {0};
	:: SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &logfont, 0);
	/*logfont.lfHeight = logfont.lfHeight * 1.1;
	logfont.lfWidth = logfont.lfWidth * 1.1;*/
	

	strcpy(logfont.lfFaceName , "微软雅黑");
	afxGlobalData.SetMenuFont(&logfont,true);


	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);


	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	
	pDocSceneTemplate = new CMultiDocTemplate(IDR_MapMatrix3DEditTYPE,
		RUNTIME_CLASS(CMapMatrix3DEditorDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CMapMatrix3DEditorView));
	if (!pDocSceneTemplate)
		return FALSE;
	
	AddDocTemplate(pDocSceneTemplate);

	//pDocDOMTemplate = new CMultiDocTemplate(IDR_MapMatrix3DEditTYPE,
	//	RUNTIME_CLASS(CMapMatrix3DEditorDoc),
	//	RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
	//	RUNTIME_CLASS(CosgEarthAppView));
	//
	//if (!pDocDOMTemplate)
	//{
	//	return FALSE;
	//}

	//AddDocTemplate(pDocDOMTemplate);
	
	//启动时最大化
	m_nCmdShow = SW_SHOWMAXIMIZED;

	// 创建主 MDI 框架窗口
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	m_pMainWnd->ShowWindow(SW_HIDE);
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 MDI 应用程序中，这应在设置 m_pMainWnd 之后立即发生

	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// 调度在命令行中指定的命令。如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// 主窗口已初始化，因此显示它并对其进行更新
	

	m_pMainWnd->SetWindowText(_T("航天远景MapMatrix3D图阵三维智能测图系统"));

	CSplashWnd *pSplashWindow = new CSplashWnd;//创建对象
	pSplashWindow->create();
	pSplashWindow->CenterWindow();
	pSplashWindow->ShowWindow(SW_SHOW);  //显示窗口
	pSplashWindow->UpdateWindow();
	Sleep(2000);  //表示启动画面持续时间
	pSplashWindow->DestroyWindow(); //销毁启动画面
	delete pSplashWindow; //删除
	//m_pMainWnd->ShowWindow(SW_SHOW);
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	listenBoostSignalHandle = AfxBeginThread(ListenBoostSignal, this);
	return TRUE;
}

int CMapMatrix3DEditorApp::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CMapMatrix3DEditorApp 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)

//	ON_WM_CONTEXTMENU()
//	ON_COMMAND(ID_BUTTON_OUTPUT_SCENE, &CAboutDlg::OnButtonOutputScene)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CMapMatrix3DEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CMapMatrix3DEditorApp 自定义加载/保存方法

void CMapMatrix3DEditorApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CMapMatrix3DEditorApp::LoadCustomState()
{
}

void CMapMatrix3DEditorApp::SaveCustomState()
{
}

// CMapMatrix3DEditorApp 消息处理程序

void CMapMatrix3DEditorApp::OnFileNew()
{
	// TODO: 在此添加命令处理程序代码
	pDocSceneTemplate->OpenDocumentFile(NULL);
	
}

void CMapMatrix3DEditorApp::createNewDoc()
{
	pDocDOMTemplate->OpenDocumentFile(NULL);

	//获取第二个文件模板，并放置在屏幕右边
	POSITION POS; 
	POS = theApp.GetFirstDocTemplatePosition();  
	CDocTemplate* pDocTemplate = theApp.GetNextDocTemplate(POS);  
	pDocTemplate = theApp.GetNextDocTemplate(POS);
	POS = pDocTemplate->GetFirstDocPosition();
	CDocument* pDoc = pDocTemplate->GetNextDoc(POS);   
	POS = pDoc->GetFirstViewPosition();
	CView* pView = (CView*)pDoc->GetNextView(POS);
	CFrameWnd* pChildFrame = pView->GetParentFrame();

	//移动
	CRect rect;
	pChildFrame->GetClientRect(rect); 
	AfxGetMainWnd()->GetClientRect(rect);
	pChildFrame->MoveWindow(rect.Width() / 2-  3*GetSystemMetrics(SM_CXDLGFRAME), 
														-GetSystemMetrics(SM_CYCAPTION)- 3*GetSystemMetrics(SM_CXDLGFRAME), 
														rect.Width() / 2, 
														rect.Height() - GetSystemMetrics(SM_CYCAPTION) - GetSystemMetrics(SM_CXDLGFRAME)); 

};

void CMapMatrix3DEditorApp::ChangeModelViewWindowSizeToHalf()
{
	//获取第一个文件模板，并放置在屏幕左边
	POSITION POS;   
	POS = AfxGetApp()->GetFirstDocTemplatePosition();   
	CDocTemplate* pDocTemplate = AfxGetApp()->GetNextDocTemplate(POS); 
	POS = pDocTemplate->GetFirstDocPosition();   
	CDocument* pDoc=pDocTemplate->GetNextDoc(POS);   
	POS = pDoc->GetFirstViewPosition();
	CView* pView = (CView*)pDoc->GetNextView(POS);
	CFrameWnd* pChildFrame = pView->GetParentFrame();

	//移动
	CRect rect;
	
	AfxGetMainWnd()->GetClientRect(rect);
	pChildFrame->MoveWindow(-  3*GetSystemMetrics(SM_CXDLGFRAME),
														-GetSystemMetrics(SM_CYCAPTION)- 3*GetSystemMetrics(SM_CXDLGFRAME), 
														rect.Width() / 2,
														rect.Height() - GetSystemMetrics(SM_CYCAPTION) - GetSystemMetrics(SM_CXDLGFRAME));   

	//左上角添加指北针
	osg::ref_ptr<osg::Group> group;
	int numChild  = (theApp.IOsgMfcViewer->ObtainRoot())->getNumChildren();
	for (int i = 0; i < numChild; i++)
	{
		group = (theApp.IOsgMfcViewer->ObtainRoot())->getChild(i)->asGroup();

		if (group)
		{	
			if (group->getName() == "viewerGroup")
			{
				pChildFrame->GetClientRect(rect);
				createCompass(theApp.IOsgMfcViewer->getViewer(),group,rect.left+20,rect.bottom-120,100,100);
				break;
			}
		}
	}
}

void CMapMatrix3DEditorApp::ChangeModelViewWindowSizeToFull()
{
	//获取第一个文件模板，并全屏显示
	POSITION POS;   
	POS = AfxGetApp()->GetFirstDocTemplatePosition();   
	CDocTemplate* pDocTemplate = AfxGetApp()->GetNextDocTemplate(POS);  
	CDocTemplate* pSecondDocTemplate = AfxGetApp()->GetNextDocTemplate(POS);
	POS = pDocTemplate->GetFirstDocPosition();   
	CDocument* pDoc=pDocTemplate->GetNextDoc(POS);   
	POS = pDoc->GetFirstViewPosition();
	CView* pView = (CView*)pDoc->GetNextView(POS);
	CFrameWnd* pChildFrame = pView->GetParentFrame();
	CRect rect;
	//pChildFrame->GetClientRect(rect);
	AfxGetMainWnd()->GetClientRect(rect);
	pChildFrame->MoveWindow(rect.left -  3*GetSystemMetrics(SM_CXDLGFRAME), 
														rect.top - GetSystemMetrics(SM_CYCAPTION) - 3*GetSystemMetrics(SM_CXDLGFRAME), 
														2*(rect.Width()+6*GetSystemMetrics(SM_CXDLGFRAME)), 
														rect.Height() - GetSystemMetrics(SM_CYCAPTION) - GetSystemMetrics(SM_CXDLGFRAME));   

	//关闭第二个模板
	pSecondDocTemplate->CloseAllDocuments(TRUE);
	//左上角删除指北针
	osg::ref_ptr<osg::Group> group;
	int numChild  = (theApp.IOsgMfcViewer->ObtainRoot())->getNumChildren();
	for (int i = 0; i < numChild; i++)
	{
		group =  (theApp.IOsgMfcViewer->ObtainRoot())->getChild(i)->asGroup();

		if (group)
		{	
			if (group->getName() == "viewerGroup")
			{
				int n = group->getNumChildren();
				group->removeChild(n-1);
				break;
			}
		}
	}
}

void CMapMatrix3DEditorApp::OnButtonMultifile()
{
	//if (CheckPrerequisite() == false)			//检测
	//{
	//	return;
	//}

	InterfaceOsgMultiFileLinkCreatorFactory ICreatorFactory;
	InterfaceOsgMultiFileLinkCreator* ICreator = ICreatorFactory.create();
	InterfaceSigEventCallBack* ICallBack = theApp.IOsgMfcViewer->ObtainISig();
	ICreator->createLinkForMultiFile(ICallBack);
}

void CMapMatrix3DEditorApp::OnButtonReverse()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	IOsgMfcViewer->getViewer()->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KEY_Z);
}

void CMapMatrix3DEditorApp::OnButtonPtMeasure()
{
	CMainFrame* mainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	HWND hViewWnd = mainFrame->GetActiveView()->GetSafeHwnd();
	//显示测量窗口
	if (!isLoadModel())
	{
		return;
	}
	dlg = NULL;
	SetOpsNone();
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_SINGLE_PT);
	vecOPS.push_back(OPS_MEASURE_POINT);

	//开始测量点事件
	IOsgMfcViewer->AddViewerOPS(vecOPS);

	if (dlg == NULL)
	{
		dlg = new CMeasureResultDlg;
		dlg->Create(IDD_DIALOG_Measure_Result);
	}

	if (IsWindow((dlg->m_hWnd)))
	{
		dlg->ShowWindow(SW_SHOW);
	}

	isPtMeasure = TRUE;
	isLineMeasure = FALSE;
	isMultiLineMeasure = FALSE;
	isAreaMeasure = FALSE;
}

void CMapMatrix3DEditorApp::OnButtonMeasureLine()
{
	// TODO: 在此添加命令处理程序代码
	if (!isLoadModel())
	{
		return;
	}
	dlg = NULL;
	SetOpsNone();
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_SINGLE_LINE);
	vecOPS.push_back(OPS_MEASURE_LINE);

	//开始测量线事件
	IOsgMfcViewer->AddViewerOPS(vecOPS);

	if (dlg == NULL)
	{
		dlg = new CMeasureResultDlg;
		dlg->Create(IDD_DIALOG_Measure_Result);
	}

	if (IsWindow((dlg->m_hWnd)))
	{
		dlg->ShowWindow(SW_SHOW);
	}


	isPtMeasure = FALSE;
	isLineMeasure = TRUE;
	isMultiLineMeasure = FALSE;
	isAreaMeasure = FALSE;
}

void CMapMatrix3DEditorApp::OnButtonMultiline()
{
	// TODO: 在此添加命令处理程序代码
	if (!isLoadModel())
	{
		return;
	}
	dlg = NULL;
	SetOpsNone();
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_MULTI_LINE_FOR_MULTI_LINE);
	vecOPS.push_back(OPS_MEASURE_MULTIlINE);

	//设置多线测量
	IOsgMfcViewer->AddViewerOPS(vecOPS);

	if (dlg == NULL)
	{
		dlg = new CMeasureResultDlg;
		dlg->Create(IDD_DIALOG_Measure_Result);
	}

	if (IsWindow((dlg->m_hWnd)))
	{
		dlg->ShowWindow(SW_SHOW);
	}

	isPtMeasure = FALSE;
	isLineMeasure = FALSE;
	isMultiLineMeasure = TRUE;
	isAreaMeasure = FALSE;
}

void CMapMatrix3DEditorApp::OnButtonMeasureArea()
{
	if (!isLoadModel())
	{
		return;
	}
	dlg = NULL;
	SetOpsNone();
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_MULTI_CLOSED_LINE_FOR_AREA);
	vecOPS.push_back(OPS_MEASURE_AREA);

	// TODO: 在此添加命令处理程序代码
	IOsgMfcViewer->AddViewerOPS(vecOPS);

	if (dlg == NULL)
	{
		dlg = new CMeasureResultDlg;
		dlg->Create(IDD_DIALOG_Measure_Result);
	}

	if (IsWindow((dlg->m_hWnd)))
	{
		dlg->ShowWindow(SW_SHOW);
	}

	isPtMeasure = FALSE;
	isLineMeasure = FALSE;
	isMultiLineMeasure = FALSE;
	isAreaMeasure = TRUE;
}

void CMapMatrix3DEditorApp::OnButtonMeshing()
{
	// TODO: 在此添加命令处理程序代码
	if (!isLoadModel())
	{
		return;
	}
	IOsgMfcViewer->getViewer()->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KEY_W);
}

void CMapMatrix3DEditorApp::OnButtonShowFillTriangle()
{
	if (CheckPrerequisite() == false || CheckIsSingleModel() == false)			//检测
	{
		return;
	}
	
	IOsgMfcViewer->ShowFillTriangle();

}

void CMapMatrix3DEditorApp::OnButtonHome()
{
	// TODO: 在此添加命令处理程序代码
	if (!isLoadModel())
	{
		return;
	}
	IOsgMfcViewer->getViewer()->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KEY_Space);
}

void CMapMatrix3DEditorApp::OnButtonLighting()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false || CheckIsSingleModel() == false)			//检测
	{
		return;
	}
	IOsgMfcViewer->getViewer()->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KEY_L);
}

void CMapMatrix3DEditorApp::OnOutputSingleScene()
{
	if (CheckPrerequisite() == false)									//检测
	{
		return;
	}

	// TODO: 在此添加命令处理程序代码
	selectCnt = g_mfcFileView->getSelectedItemCount();		//获取选择的场景

	if (selectCnt < 0)
	{
		return;
	}

	CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, (LPCTSTR)_TEXT("osgb模型 (*.osgb)|*.osgb|All Files (*.*)|*.*||"), NULL);

	if (dlg.DoModal() == IDOK)
	{
		CString filePath = dlg.GetPathName();
		CString fileExt = dlg.GetFileExt();

		if (fileExt != _T("osgb"))
		{
			filePath += _T(".osgb");
		}

		osg::ref_ptr<osg::Node> node = theApp.IOsgMfcViewer->ObtainRoot()->getChild(0)->asGroup()->getChild(selectCnt);

		InterfaceOsgEditorFactory IEditorFactory;
		InterfaceOsgEditor* IEditor = IEditorFactory.create();
		IEditor->SetLodFileName(node, "");

		osg::ref_ptr<osgDB::Options> spOptions = new osgDB::Options;
		spOptions->setPluginStringData("WriteImageHint", "IncludeFile");
		osgDB::writeNodeFile(*node, (LPSTR)(LPCTSTR)filePath, spOptions);

		InterfaceMFCExcuteFactory iExecuteFactory;
		InterfaceMFCExcute* iExecute = iExecuteFactory.create();
		iExecute->PopUpMessage("场景输出成功");
	}
	
}

void CMapMatrix3DEditorApp::OnButtonSelTileRemove()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false || CheckIsNotHorizon() == true || CheckIsSingleModel() == false)									//检测
	{
		return;
	}

	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_SEL_TILE_REMOVE);

	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonHollowOut()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false || CheckIsNotHorizon() == true || CheckIsSingleModel() == false)									//检测
	{
		return;
	}

	SetOpsNone();
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_MULTI_CLOSED_LINE);
	vecOPS.push_back(OPS_HOLLOW_OUT);

	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonStartHollowOut()
{
	if (CheckPrerequisite() == false || CheckIsNotHorizon() == true || CheckIsSingleModel() == false)									//检测
	{
		return;
	}

	std::string inFileName = vecSceneFileName[0];
	int pos = inFileName.rfind('\\') + 1;
	std::string inFilePath = inFileName.substr(0, pos);
	IOsgMfcViewer->ObtainEditEventSlot()->HollowOutByPolygon(inFilePath);

	SetOpsNone();
}

void CMapMatrix3DEditorApp::OnButtonHollowOutSuspended()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false || CheckIsNotHorizon() == true || CheckIsSingleModel() == false)									//检测
	{
		return;
	}

	SetOpsNone();
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_SUSPEND_OBJ_SCOPE);
	vecOPS.push_back(OPS_HOLLOW_OUT_SUSPENDED);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonHollowOutSuspendedUp()
{
	if (CheckPrerequisite() == false || CheckIsNotHorizon() == true || CheckIsSingleModel() == false)									//检测
	{
		return;
	}

	SetOpsNone();
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_SUSPEND_OBJ_SCOPE);
	vecOPS.push_back(OPS_HOLLOW_OUT_SUSPENDED_UP);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonStartHollowOutSuspended()
{
	if (CheckPrerequisite() == false || CheckIsNotHorizon() == true || CheckIsSingleModel() == false)									//检测
	{
		return;
	}

	std::string inFileName = vecSceneFileName[0];
	int pos = inFileName.rfind('\\') + 1;
	std::string inFilePath = inFileName.substr(0, pos);
	IOsgMfcViewer->ObtainEditEventSlot()->HollowOutSuspend(inFilePath);

	SetOpsNone();
}

void CMapMatrix3DEditorApp::OnButtonClipByPolygon()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false || CheckIsNotHorizon() == true || isSingleCheckModel() == false)					//检测
	{
		return;
	}
	
	SetOpsNone();
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_MULTI_CLOSED_LINE);
	vecOPS.push_back(OPS_CLIP_BY_POLYGON);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonStartClipPolygon()
{
	if (CheckPrerequisite() == false || CheckIsNotHorizon() == true || isSingleCheckModel() == false)					//检测
	{
		return;
	}

	std::string inFileName = vecSceneFileName[0];
	int pos = inFileName.rfind('\\') + 1;
	std::string inFilePath = inFileName.substr(0, pos);
	IOsgMfcViewer->ObtainEditEventSlot()->ClipByPolygon(inFilePath);

	SetOpsNone();
}

void CMapMatrix3DEditorApp::OnButtonClipByHeight()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false || CheckIsNotHorizon() == true)									//检测
	{
		return;
	}

	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_FLAT_PLANE);
	vecOPS.push_back(OPS_CLIP_BY_HEIGHT);
	
	IOsgMfcViewer->AddViewerOPS(vecOPS);
	theApp.IOsgMfcViewer->ObtainEditEventSlot()->ClipHeight();
}

void CMapMatrix3DEditorApp::OnButtonInvertselect()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false || CheckIsNotHorizon() == true)									//检测
	{
		return;
	}

	theApp.IOsgMfcViewer->ObtainEditEventSlot()->InvertClipByHeight();
}

void CMapMatrix3DEditorApp::OnButtonRepairhole()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false || CheckIsNotHorizon() == true || CheckIsSingleModel() == false)							//检测
	{
		return;
	}

	SetOpsNone();
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_CLOSED_LINE);
	vecOPS.push_back(OPS_REPAIR_HOLE);

	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonStartRepairHole()
{
	if (CheckPrerequisite() == false || CheckIsNotHorizon() == true || CheckIsSingleModel() == false)							//检测
	{
		return;
	}

	//IOsgMfcViewer->ObtainEditEventSlot()->();
	
}

void CMapMatrix3DEditorApp::OnButtonFrontview()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)																		 //检测
	{
		return;
	}

	IOsgMfcViewer->ChangeView(0);
}

void CMapMatrix3DEditorApp::OnButtonLeftView()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)																		 //检测
	{
		return;
	}

	IOsgMfcViewer->ChangeView(4);
}

void CMapMatrix3DEditorApp::OnButtonRightView()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)																		 //检测
	{
		return;
	}

	IOsgMfcViewer->ChangeView(5);
}

void CMapMatrix3DEditorApp::OnButtonBackView()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)																		 //检测
	{
		return;
	}

	IOsgMfcViewer->ChangeView(3);
}

void CMapMatrix3DEditorApp::OnButtonLookDown()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)																		 //检测
	{
		return;
	}

	IOsgMfcViewer->ChangeView(2);
}

void CMapMatrix3DEditorApp::OnButtonLookup()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)																		 //检测
	{
		return;
	}

	IOsgMfcViewer->ChangeView(1);
}

void CMapMatrix3DEditorApp::OnClearSingleScene()
{
	// TODO: 在此添加命令处理程序代码
	osg::ref_ptr<osg::Group> root = IOsgMfcViewer->ObtainRoot();
	osg::ref_ptr<osg::Group> sceneGroup = root->getChild(0)->asGroup();

	selectCnt = g_mfcFileView->getSelectedItemCount();		//获取选择的场景

	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	pMain->RemoveItem(selectCnt);

	osg::ref_ptr<osg::Node> sceneNode = sceneGroup->getChild(selectCnt);
	sceneGroup->removeChild(sceneNode);

	std::vector<CString>::iterator iter = vecSceneFileName.begin() + selectCnt;
	vecSceneFileName.erase(iter);
}

void CMapMatrix3DEditorApp::OnButtonOutputScene()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false || CheckIsSingleModel() == false)																		 //检测
	{
		return;
	}

	//创建文件夹并设置输出路径
	CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, (LPCTSTR)_TEXT("osgb模型 (*.osgb)|*.osgb|All Files (*.*)|*.*||"), NULL);

	if (dlg.DoModal() == IDOK)
	{
		CString filePath = dlg.GetPathName();
		CString fileExt = dlg.GetFileExt();

		if (fileExt != _T("osgb") && fileExt != _T("osg"))									//防止忘了输出后缀名
		{
			filePath += _T(".osgb");
		}

		int pos = filePath.ReverseFind('\\');
		int nLength = filePath.GetLength();
		CString outputFilePath = filePath.Left(pos);

		//输出照片
		std::string outFileName = filePath;

		osg::ref_ptr<osg::Group> root = IOsgMfcViewer->ObtainRoot();
		osg::ref_ptr<osg::Node> copyNode = dynamic_cast<osg::Node*> (root->getChild(0)->asGroup()->getChild(0)->clone(osg::CopyOp::DEEP_COPY_ALL));

		//database路径设为相对
		InterfaceOsgEditorFactory IEditorFactory;
		InterfaceOsgEditor* IEditor = IEditorFactory.create();
		IEditor->SetLodFileName(copyNode, "");
		osg::ref_ptr<osgDB::Options> spOptions = new osgDB::Options;
		spOptions->setPluginStringData("WriteImageHint", "IncludeFile");
		osgDB::writeNodeFile(*copyNode, outFileName, spOptions.get());

		//让用户选择是否重新加载
		InterfaceMFCExcuteFactory IMFCExecuteFactory;
		InterfaceMFCExcute* IMFCExecute = IMFCExecuteFactory.create();
		bool isReload = IMFCExecute->MessageBoxIsReload();

		if (isReload)
		{
			InterfaceSigEventCallBack* iSigEventCallBack = theApp.IOsgMfcViewer->ObtainISig();
			iSigEventCallBack->EmitSigRemoveAllScene();
			char* reloadFileName = (char*) outFileName.c_str();
			iSigEventCallBack->EmitSigAddScene(reloadFileName);
		}
	}

}

void CMapMatrix3DEditorApp::OnButtonHorizonAccurate()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false || CheckIsSingleModel() == false || CheckIsNotHorizon() == true)			//检测
	{
		return;
	}

	SetOpsNone();
	std::vector<OperatingState> vecOPS;

	vecOPS.push_back(OPS_HORIZON_ACCURATE);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonAutohorizon()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false || CheckIsSingleModel() == false || CheckIsHorizon() == true)			//检测
	{
		return;
	}

	//开始进度条 

	osg::ref_ptr<osg::Group> root = IOsgMfcViewer->ObtainRoot();
	osg::ref_ptr<osg::Group> sceneGroup = root->getChild(0)->asGroup();

	InterfaceOsgHorizonFactory IHorizonFactory;
	InterfaceOsgHorizon* IHoriozn = IHorizonFactory.create();
	IHoriozn->ModelHorizontalization(sceneGroup);

	//g_mfcOpsStatusBar->EndProgress();
	IOsgMfcViewer->getViewer()->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KEY_Space);
}

void CMapMatrix3DEditorApp::OnButtonManualhorizon()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false || CheckIsSingleModel() == false || CheckIsNotHorizon() == true)			//检测
	{
		return;
	}

	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_MANUAL_ORIENTATION);
	vecOPS.push_back(OPS_DRAW_MULTI_PT);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonStartManualHorizon()
{
	if (CheckPrerequisite() == false || CheckIsSingleModel() == false || CheckIsNotHorizon() == true)			//检测
	{
		return;
	}

	//IOsgMfcViewer->ObtainOrientEventSlot()->ManualHorizon();

	SetOpsNone();
}

void CMapMatrix3DEditorApp::OnButtonMirror()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false || CheckIsSingleModel() == false || CheckIsNotHorizon() == true)			//检测
	{
		return;
	}

	//开始进度条 
	osg::ref_ptr<osg::Group> root = IOsgMfcViewer->ObtainRoot();
	osg::ref_ptr<osg::Group> sceneGroup = root->getChild(0)->asGroup();
	InterfaceOsgHorizonFactory IHorizonFactory;
	InterfaceOsgHorizon* IHoriozn = IHorizonFactory.create();
	IHoriozn->ModelMirror(sceneGroup);

	//g_mfcOpsStatusBar->EndProgress();
	IOsgMfcViewer->getViewer()->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KEY_Space);
}

void CMapMatrix3DEditorApp::OnButtonUpsideDown()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false || CheckIsSingleModel() == false || CheckIsNotHorizon() == true)			//检测
	{
		return;
	}

	//开始进度条 

	osg::ref_ptr<osg::Group> root = IOsgMfcViewer->ObtainRoot();
	osg::ref_ptr<osg::Group> sceneGroup = root->getChild(0)->asGroup();
	InterfaceOsgHorizonFactory IHorizonFactory;
	InterfaceOsgHorizon* IHoriozn = IHorizonFactory.create();
	IHoriozn->ModelUpsideDown(sceneGroup);

	IOsgMfcViewer->getViewer()->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KEY_Space);
}

void CMapMatrix3DEditorApp::OnButtonAbsoluteOrientation()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false || CheckIsSingleModel() == false || CheckIsNotHorizon() == true)			//检测
	{
		return;
	}

	SetOpsNone();

	std::vector<OperatingState> vecOPS;
	//vecOPS.push_back(OPS_DRAW_MULTI_PT);
	vecOPS.push_back(OPS_ABSOLUTE_ORIENTATION);

	theApp.IOsgMfcViewer->AddViewerOPS(vecOPS);

	CAbsoluteOrientationDlg* dlg = new CAbsoluteOrientationDlg(NULL, vecSceneFileName);
	dlg->Create(IDD_DIALOG_ABSOLUTE_ORIENTATION);
	dlg->ShowWindow(SW_SHOW);
}

void CMapMatrix3DEditorApp::OnButtonInternetAbsOrientation()
{
	// TODO: 在此添加命令处理程序代码
	if(isInternetPick == TRUE)
		return;
	isInternetPick = TRUE;
	if (CheckPrerequisite() == false || CheckIsSingleModel() == false || CheckIsNotHorizon() == true)			//检测
	{
		return;
	}

	ChangeModelViewWindowSizeToHalf();
	createNewDoc();

	//设置两个浏览器刺点状态
	SetOpsNone();

	//设置两个浏览器刺点状态
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_MULTI_PT);
	vecOPS.push_back(OPS_INTERNET_ORIENTATION);

	theApp.IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonIntenetFinish()
{
	// TODO: 在此添加命令处理程序代码
	if (isInternetPick == FALSE)
	{
		return;
	}
	isInternetPick = FALSE;
	ChangeModelViewWindowSizeToFull();
}

void CMapMatrix3DEditorApp::OnButtonStartInternetAbsOri()
{
	if (CheckPrerequisite() == false || CheckIsSingleModel() == false || CheckIsNotHorizon() == true)			//检测
	{
		return;
	}

	//theApp.IOsgMfcViewer->ObtainOrientEventSlot()->InternetAbsOrient();
}

void CMapMatrix3DEditorApp::OnButtonModelMatching()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	SetOpsNone();
	//设置浏览器状态
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_MULTI_SINGLE_LINE);
	vecOPS.push_back(OPS_MODEL_MATCHING);

	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnMatchSingleModel()
{
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}
	
	selectCnt = g_mfcFileView->getSelectedItemCount();		//获取选择的场景
	theApp.IOsgMfcViewer->ObtainOrientEventSlot()->matchSingleModel();
}

void CMapMatrix3DEditorApp::OnButtonSceneStitching()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}
	
	std::vector<osg::MatrixTransform*> vecTrans;
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();

	osg::ref_ptr<osg::Group> root = IOsgMfcViewer->ObtainRoot();
	IFinder->FindTransNode(root, vecTrans);

	//if (vecTrans.size() <= 1)
	if (vecSceneFileName.size()<=1)
	{
		//AfxMessageBox(_T("必须加装两个以上场景"));
		InterfaceMFCExcuteFactory iExecuteFactory;
		InterfaceMFCExcute* iExecute = iExecuteFactory.create();
		iExecute->PopUpMessage("必须加载两个以上场景");
		return;
	}

	std::vector<std::string> vecStrSceneFileName;

	for (int i = 0; i < vecSceneFileName.size(); i++)
	{
		std::string temp = vecSceneFileName[i];
		vecStrSceneFileName.push_back(temp);
	}

	InterfaceOsgStitcherFactory IStitcherFactory;
	InterfaceOsgStitcher* IStitcher = IStitcherFactory.create();

	//是否满足拼接条件 在同一个目录下
	if (!IStitcher->IsPathCorrect(vecStrSceneFileName))
	{
		//AfxMessageBox(_T("所有场景需具有共同的上级目录"));
		InterfaceMFCExcuteFactory iExecuteFactory;
		InterfaceMFCExcute* iExecute = iExecuteFactory.create();
		iExecute->PopUpMessage("所有场景需具有共同的上级目录");
		return;
	}

	osg::ref_ptr<osg::Group> combineGroup = new osg::Group;
	//场景合并
	osg::ref_ptr<osg::Group> sceneGroup = root->getChild(0)->asGroup();

	//场景拼接
	IStitcher->Stitch(sceneGroup, vecStrSceneFileName, combineGroup);

	int posL = vecSceneFileName[0].ReverseFind('\\');
	int Length = vecSceneFileName[0].GetLength();
	std::string strFilePath = vecSceneFileName[0].Left(posL);	
	int posEnd = strFilePath.rfind('\\');

	//输出osgb
	std::string strOutFilePath = strFilePath.substr(0, posEnd + 1) + "combine.osgb";
	InterfaceOsgEditorFactory IEditorFactory;
	InterfaceOsgEditor* IEditor = IEditorFactory.create();
	IEditor->SetLodFileName(combineGroup, "");

	osgDB::writeNodeFile(*combineGroup, strOutFilePath);

	//让用户选择是否重新加载
	char strMsg[99];
	sprintf_s(strMsg, "合并场景成功，输出到%s\n", strOutFilePath.c_str());
	InterfaceMFCExcuteFactory IMFCExecuteFactory;
	InterfaceMFCExcute* IMFCExecute = IMFCExecuteFactory.create();
	bool isReload = IMFCExecute->MessageBoxIsReloadWithFileName(strMsg);

	if (isReload)
	{
		InterfaceSigEventCallBack* iSigEventCallBack = theApp.IOsgMfcViewer->ObtainISig();
		iSigEventCallBack->EmitSigRemoveAllScene();
		char* reloadFileName = (char*) strOutFilePath.c_str();
		iSigEventCallBack->EmitSigAddScene(reloadFileName);
	}
}

void CMapMatrix3DEditorApp::OnButtonSplitModel()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false || CheckIsSingleModel() == false)			//检测
	{
		return;
	}

	osg::ref_ptr<osg::Group> root = IOsgMfcViewer->ObtainRoot();
	osg::ref_ptr<osg::Node> sceneNode = root->getChild(0)->asGroup()->getChild(0);
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();
	std::vector<osg::MatrixTransform*> vecTrans;
	IFinder->FindTransNode(sceneNode, vecTrans);

	if (vecTrans.size() < 2)
	{
		//AfxMessageBox(_T("必须加装两个以上场景"));
		InterfaceMFCExcuteFactory iExecuteFactory;
		InterfaceMFCExcute* iExecute = iExecuteFactory.create();
		iExecute->PopUpMessage("单个场景不能拆分");
		return;
	}

	if (sceneNode)
	{
		std::string sceneFileName = vecSceneFileName[0];
		InterfaceOsgStitcherFactory IStitcherFactory;
		InterfaceOsgStitcher* IStitcher = IStitcherFactory.create();
		IStitcher->SceneSplit(sceneNode, sceneFileName);

		//AfxMessageBox(_T("场景分离成功"));
		InterfaceMFCExcuteFactory iExecuteFactory;
		InterfaceMFCExcute* iExecute = iExecuteFactory.create();
		iExecute->PopUpMessage("场景分离成功");
	
	}
}

void CMapMatrix3DEditorApp::OnSceneAdjustFlatPlane()
{
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_EDIT_WHOLE_SCENE_XY);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnAdjustHeight()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_EDIT_WHOLE_SCENE_Z);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonDomGenerate()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false || CheckIsSingleModel() == false)			//检测
	{
		return;
	}

	SetOpsNone();

	//弹出输入分辨率界面
	CDOMResSetDlg dlg;
	double res = 0;

	if (dlg.DoModal() == IDOK)
	{
		res = dlg.res;
		std::string strSceneFileName = vecSceneFileName[0];
		InterfaceOsgDomGeneratorFactory IDomGeneFactory;
		InterfaceOsgDomGenerator* IDomGene = IDomGeneFactory.create();

		osg::ref_ptr<osg::Group> root = IOsgMfcViewer->ObtainRoot();
		osg::ref_ptr<osg::Node> sceneNode = root->getChild(0)->asGroup()->getChild(0);
		InterfaceSigEventCallBack* iSigCallBack = IOsgMfcViewer->ObtainISig();
		IDomGene->GenerateDOM(sceneNode, strSceneFileName, res, iSigCallBack);
	}
	else
	{
		return;
	}
	
}

void CMapMatrix3DEditorApp::OnButtonDsmGene()
{
	 //TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false || CheckIsSingleModel() == false)			//检测
	{
		return;
	}

	SetOpsNone();
	CDOMResSetDlg dlg;
	
	if (dlg.DoModal() == IDOK)
	{
		//创建保存文件对话框
		double res = dlg.res;
		std::string strSceneFileName = vecSceneFileName[0];
		
		osg::ref_ptr<osg::Group> root = IOsgMfcViewer->ObtainRoot();
		osg::ref_ptr<osg::Node> sceneNode = root->getChild(0)->asGroup()->getChild(0);
		InterfaceSigEventCallBack* iSigCallBack = IOsgMfcViewer->ObtainISig();

		InterfaceOsgDsmGeneFactory iDSMGeneFactory;
		InterfaceOsgDsmGene* iDSMGene = iDSMGeneFactory.create();
		iDSMGene->GenerateDSM(sceneNode, strSceneFileName, res, res, iSigCallBack);

	/*	theApp.IOsgMfcViewer->ObtainMapperEventSlot()->ProduceDsm(strSceneFileName, res);*/
	}
}

void CMapMatrix3DEditorApp::OnButtonExtractHatch()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	SetOpsNone();

	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_SINGLE_PT);
	vecOPS.push_back(OPS_EXTRACT_HATCH);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::SetOpsNone()
{
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_None);
	theApp.IOsgMfcViewer->AddViewerOPS(vecOPS);
}

bool CMapMatrix3DEditorApp::CheckIsHorizon()
{
	std::vector<osg::MatrixTransform*> vecTrans;
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();

	osg::ref_ptr<osg::Group> root = IOsgMfcViewer->ObtainRoot();
	osg::ref_ptr<osg::Node> sceneNode = root->getChild(0);
	IFinder->FindTransNode(sceneNode, vecTrans);

	if (vecTrans.size() != 0)
	{
		//AfxMessageBox(_T("场景已经做过置平"));
		InterfaceMFCExcuteFactory IExcuteFactory;
		InterfaceMFCExcute* IExcute = IExcuteFactory.create();
		IExcute->PopUpMessage("场景已经做过置平");
		return true;
	}

	return false;
}

bool CMapMatrix3DEditorApp::CheckIsNotHorizon()
{
	std::vector<osg::MatrixTransform*> vecTrans;
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();
	osg::ref_ptr<osg::Group> root = IOsgMfcViewer->ObtainRoot();
	osg::ref_ptr<osg::Node> sceneNode = root->getChild(0);
	IFinder->FindTransNode(sceneNode, vecTrans);

	if (vecTrans.size() == 0)
	{
		//AfxMessageBox(_T("场景还没有做置平"));
		InterfaceMFCExcuteFactory IExcuteFactory;
		InterfaceMFCExcute* IExcute = IExcuteFactory.create();
		IExcute->PopUpMessage("场景还没有做置平");
		return true;
	}

	return false;
}

bool CMapMatrix3DEditorApp::CheckIsSingleModel()
{
	if (vecSceneFileName.size() > 1)
	{
		//AfxMessageBox(_T("加载超过一个场景，不支持超过一个的场景"));
		InterfaceMFCExcuteFactory IExcuteFactory;
		InterfaceMFCExcute* IExcute = IExcuteFactory.create();
		IExcute->PopUpMessage("加载超过一个场景,不支持超过一个的场景");
		return false;
	}

	return true;
}

bool CMapMatrix3DEditorApp::CheckPrerequisite()
{
	//IsHasLicense();

	if (!isLoadModel())
	{
		return false;
	}

	return true;
};

bool CMapMatrix3DEditorApp::isLoadModel()
{
	osg::ref_ptr<osg::Group> mRoot = IOsgMfcViewer->ObtainRoot();

	if (!mRoot)
	{
		return false;
	}
	
	if (mRoot->getNumChildren() == 0)
	{
		return false;
	}

	int childNum = mRoot->getChild(0)->asGroup()->getNumChildren();

	if (childNum > 0)
	{
		return true;
	}

	return false;
}

bool CMapMatrix3DEditorApp::isSingleCheckModel()
{
	int modelNum = 0;
	osg::ref_ptr<osg::Group> root = IOsgMfcViewer->ObtainRoot();
	osg::ref_ptr<osg::Group> sceneGroup = root->getChild(0)->asGroup();

	for (int i = 0; i < sceneGroup->getNumChildren(); i++)
	{
		if (sceneGroup->getChild(i)->getNodeMask() != 0x0)
		{
			modelNum++;
		}

	}

	InterfaceMFCExcuteFactory IExcuteFactory;
	InterfaceMFCExcute* IExcute = IExcuteFactory.create();

	if (modelNum != 1)
	{
		IExcute->PopUpMessage("加载超过一个场景,不支持超过一个的场景,请把单个场景清除，或隐藏");
		return false;
	}

	if (sceneGroup->getChild(0)->getNodeMask() == 0x0)
	{
		IExcute->PopUpMessage("第一个场景不能是隐藏");
		return false;
	}

	
	return true;
};

void CMapMatrix3DEditorApp::ShowOrHideSceneGroup(int selCnt, bool bCheck)
{
	osg::ref_ptr<osg::Group> root = IOsgMfcViewer->ObtainRoot();
	osg::ref_ptr<osg::Group> sceneGroup = root->getChild(0)->asGroup();

	if (bCheck == true)
	{
		sceneGroup->getChild(selCnt)->setNodeMask(1);
	}
	else
	{
		sceneGroup->getChild(selCnt)->setNodeMask(0x0);
	}
}

void CMapMatrix3DEditorApp::IsHasLicense()
{
	InterfaceCEncryptFactory iCEncryptFactory;
	InterfaceCEncrypt* iCEncrypt = iCEncryptFactory.Create();
	bool isSuccess = iCEncrypt->CheckModuleNum(71, FALSE);

	if (!isSuccess)
	{
		InterfaceMFCExcuteFactory IMFCExecuteFactory;
		InterfaceMFCExcute* IMFCExecute = IMFCExecuteFactory.create();
		IMFCExecute->PopUpMessage("无法找到加密狗，请插入加密狗，重新运行程序");

		exit(0);
	}
}
	
void CMapMatrix3DEditorApp::StartProgressThread()
{
	AfxBeginThread(ChangeProgressBar, this);
}

void CMapMatrix3DEditorApp::RemoveAllScene()
{
	SetOpsNone();
	IOsgMfcViewer->RemoveAllChild();

	CMainFrame *pMain=(CMainFrame *)AfxGetApp()->m_pMainWnd;
	pMain->removeAllItem();
	vecSceneFileName.clear();
}

void CMapMatrix3DEditorApp::AddScene(std::string strModel)
{
	//读取文件
	int num = theApp.vecSceneFileName.size() + 1;
	// int 转 string
	stringstream ss;
	string str;
	ss<<num;
	ss>>str;
	std::string strName = "model" + str;

	osg::ref_ptr<osg::Node> tempNodeStart = osgDB::readNodeFile(strModel);
	tempNodeStart->setName(strName.c_str());

	InterfaceOsgEditorFactory iOsgEditorFactory;
	InterfaceOsgEditor* iOsgEditor = iOsgEditorFactory.create();
	iOsgEditor->SetImageDataVariance(tempNodeStart);

	//加载节点
	osg::ref_ptr<osg::Group> root = IOsgMfcViewer->ObtainRoot();
	osg::ref_ptr<osg::Group> group = root->getChild(0)->asGroup();
	IOsgMfcViewer->AddChild(group, tempNodeStart);

	//设置范围
	IOsgMfcViewer->SetSceneExtent();

	CString sceneFileName = strModel.c_str();
	theApp.vecSceneFileName.push_back(sceneFileName);
	//提取路径
	int pos = sceneFileName.ReverseFind('\\');
	CString sceneFilePath = sceneFileName.Left(pos);			
	CString fileTitle = sceneFileName.Right(sceneFileName.GetLength() - 1 - pos);

	//添加路径到视图上
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	pMain->insertItem(sceneFilePath, fileTitle);
}

void CMapMatrix3DEditorApp::InitCallBack()
{
	InterfaceSigEventCallBack* ISigEvent = theApp.IOsgMfcViewer->ObtainISig();
	boost::signals2::signal<void ()>* sigVoid = ISigEvent->ObtainSigStartProgress();

	CallBackStartProgress callBackStartProgress;
	sigVoid->connect(callBackStartProgress);

	sigVoid = ISigEvent->ObtainSigEndProgress();
	CallBackEndProgress callBackEndProgress;
	sigVoid->connect(callBackEndProgress);

	boost::signals2::signal<void (int)>* sigInt = ISigEvent->ObtainSigSetProgress();
	CallBackSetProgressBarPos callBackSetProgress;
	sigInt->connect(callBackSetProgress);

	boost::signals2::signal<int (), result_handler<std::vector<int>>>* sigReturnInt = ISigEvent->ObtainSigSelModelIndex();
	CallBackSelModelIndex callBackSelModelIndex;
	sigReturnInt->connect(callBackSelModelIndex);

	sigReturnInt = ISigEvent->ObtainSigGetSelPtNumFromAbsDlg();
	CallBackGetSelPtNumFromAbsDlg callBackSelPtNum;
	sigReturnInt->connect(callBackSelPtNum);

	sigReturnInt = ISigEvent->ObtainSigGetSelRowFromAbsDlg();
	CallBackGetSelRowFromAbsDlg callBackSelRow;
	sigReturnInt->connect(callBackSelRow);

	boost::signals2::signal<void  (char*)>* sigChar = ISigEvent->ObtainSigNotifyMeasureMsg();
	CallBackNotifyMeasureMsg callBackNotifyMeasureMsg;
	sigChar->connect(callBackNotifyMeasureMsg);

	boost::signals2::signal<void  (void)>* sigCloseM = ISigEvent->ObtainSigCloseMeasureDlg();
	CallBackCloseMeasureDlg callBackcloseMeasureDlg;
	sigCloseM->connect(callBackcloseMeasureDlg);

	boost::signals2::signal<void  (void)>* sigCloseA = ISigEvent->ObtainSigCloseAbsDlg();
	CallBackCloseAbsDlg callBackcloseAbsDlg;
	sigCloseA->connect(callBackcloseAbsDlg);

	boost::signals2::signal<void (int, double, double, double, bool)>* sigMul = ISigEvent->ObtainSigRelativePt();
	CallBackObtainRelativePoint callBackObtainRelaPt;
	sigMul->connect(callBackObtainRelaPt);

	boost::signals2::signal<bool (), result_handler<std::vector<bool>>>* sigChangeCtrlList = ISigEvent->ObtainSigChangeCtrlList();
	CallBackChangeCtrlList callBackChangeCtrlList;
	sigChangeCtrlList->connect(callBackChangeCtrlList);

	boost::signals2::signal<void ()>* sigRemoveAllScene = ISigEvent->ObtainSigRemoveAllScene();
	CallBackRemoveAllScene callBackRemoveAllScene;
	sigRemoveAllScene->connect(callBackRemoveAllScene);

	boost::signals2::signal<void (char*)>* sigAddScene = ISigEvent->ObtainSigAddScene();
	CallBackAddScene callBackAddScene;
	sigAddScene->connect(callBackAddScene);

	boost::signals2::signal<void (int, double, double, double)>* sigInsertCtrlListRecord = ISigEvent->ObtainSigInsertCtrlListRecord();
	CallBackInsertControlListRecord callBackInsertCtrlListRecord;
	sigInsertCtrlListRecord->connect(callBackInsertCtrlListRecord);

	boost::signals2::signal<void (char*)>* sigShowOpsHint = ISigEvent->ObtainSigShowOpsHint();
	CallBackShowOpsBarHint callBackShowOpsBarHint;
	sigShowOpsHint->connect(callBackShowOpsBarHint);

	boost::signals2::signal<void(char*)>* sigLinkPropertyListDlg = ISigEvent->ObtainSigLinkPropertyListDlg();
	CallBackPopUpPropertyListDlg callBackPopUpPropertyListDlg;
	sigLinkPropertyListDlg->connect(callBackPopUpPropertyListDlg);

	boost::signals2::signal<void(int, int)>* sigObjectMenuDlg = ISigEvent->obtainSigObjectMenuDlg();
	CallBackObjectMenuDlg callBackObjectMenuDlg;
	sigObjectMenuDlg->connect(callBackObjectMenuDlg);
}

void CMapMatrix3DEditorApp::OnUpdateButtonPtMeasure(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if (isPtMeasure == TRUE)
	{
		pCmdUI->SetCheck(TRUE);
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
	}
}

void CMapMatrix3DEditorApp::OnUpdateButtonMeasureLine(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if (isLineMeasure == TRUE)
	{
		pCmdUI->SetCheck(TRUE);
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
	}
}

void CMapMatrix3DEditorApp::OnUpdateButtonMultiline(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if(isMultiLineMeasure == TRUE)
	{
		pCmdUI->SetCheck(TRUE);
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
	}
}

void CMapMatrix3DEditorApp::OnUpdateButtonMeasureArea(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if (isAreaMeasure == TRUE)
	{
		pCmdUI->SetCheck(TRUE);
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
	}

}

void CMapMatrix3DEditorApp::OnButtonExportHatching()//输出剖面线
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	osg::ref_ptr<osg::Group> root = theApp.IOsgMfcViewer->ObtainRoot();
	theApp.IOsgMfcViewer->ObtainMapperEventSlot()->outForDxfAndShp(root);
}

void CMapMatrix3DEditorApp::OnButtonHandPaintLine()
{
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	SetOpsNone();
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_EXTRACT_HAND_PAINT_LINE);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonExtractFineOutline()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	SetOpsNone();
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_EXTRACT_TRIANGLE_OUTLINE);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonExtractFineOutlineNewHeight()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	SetOpsNone();
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_EXTRACT_TRIANGLE_OUTLINE_CHANGE_HEIGHT);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonSelectByRectangle()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_RECTANGLE);
	vecOPS.push_back(OPS_SELECT_LINE_BY_RECTANGLE);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonSelectByClick()
{
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	std::vector<OperatingState> vecOPS;
	
	vecOPS.push_back(OPS_SELECT_LINE_BY_CLICK);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonSelectByPolygon()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_MULTI_CLOSED_LINE_DBCLICK);
	vecOPS.push_back(OPS_SELECT_LINE_BY_Area);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonExtractContour()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	SetOpsNone();

	//弹出输入分辨率界面
	CHeightSetDlg dlg;
	double res = 0;

	if (dlg.DoModal() == IDOK)
	{
		res = dlg.res;
		theApp.IOsgMfcViewer->ObtainMapperEventSlot()->extractContour(res);
	}
	else
	{
		return;
	}
}

void CMapMatrix3DEditorApp::OnButtonAutoSelectWallspace()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}
	SetOpsNone();
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_SELECT_WALLSPACE_AUTO);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonHandSelectWallspace()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}
	SetOpsNone();
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_SELECT_WALLSPACE_HAND);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonDrawHorizLine()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}
	SetOpsNone();
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_HORIZ_LINE);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonWallInsect()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}
	SetOpsNone();
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_WALLSPACE_INSECT);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonFittingWallspace()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}
	SetOpsNone();
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_FITTING_WALLSPACE);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonManualExtractTexture()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	std::vector<OperatingState> vecOPS;

	vecOPS.push_back(OPS_DRAW_CLOSED_LINE);
	vecOPS.push_back(OPS_MANUAL_EXTRACT_TEXTURE);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonCombineGroup()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}
	SetOpsNone();
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_MULTI_CLOSED_LINE_DBCLICK);
	vecOPS.push_back(OPS_COMBINE);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonStartExtractTexture()
{
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	double deltaZ = 0.1;
	double res = 0.005;
	//IOsgMfcViewer->ObtainMonomerEventSlot()->ExtractTexture(res, deltaZ);
}

void CMapMatrix3DEditorApp::OnButtonWholeModelMatching()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	SetOpsNone();
	//设置浏览器状态
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_MULTI_SINGLE_LINE);
	vecOPS.push_back(OPS_WHOLE_MATCH_SCENE);

	IOsgMfcViewer->AddViewerOPS(vecOPS);

}

void CMapMatrix3DEditorApp::OnButtonStartWholeModelMatching()
{
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	//IOsgMfcViewer->ObtainOrientEventSlot()->matchWholeModel();
}

void CMapMatrix3DEditorApp::OnButtonLineInsect()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}
	SetOpsNone();
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_LINE_INSECT);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonExtractObjectByPolygon()
{
	// TODO: Add your command handler code here
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_CLOSED_LINE);
	vecOPS.push_back(OPS_EXTRACT_OBJECT_BY_POLYGON);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}


void CMapMatrix3DEditorApp::OnButtonStartExtract()
{
	// TODO: Add your command handler code here
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	std::string inFileName = vecSceneFileName[0];
	IOsgMfcViewer->ObtainMonomerEventSlot()->extractObject(inFileName);
	SetOpsNone();
}


void CMapMatrix3DEditorApp::OnButtonStartQuickClipModel()
{
	// TODO: Add your command handler code here
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	/*IOsgMfcViewer->ObtainMonomerEventSlot()->QuickClipByPolygon();*/

	SetOpsNone();
}


void CMapMatrix3DEditorApp::OnButtonQuickClipModelByTriangle()
{
	// TODO: Add your command handler code here
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_MULTI_CLOSED_LINE_DBCLICK);
	vecOPS.push_back(OPS_QUICK_CLIP_MODEL_BY_TRIANGLE);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}


void CMapMatrix3DEditorApp::OnButtonQuickDeleteSingleTriangle()
{
	// TODO: Add your command handler code here
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_QUICK_DELETE_SINGLE_TRIANGLE);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}


void CMapMatrix3DEditorApp::OnButtonQuickDeleteTriangleByLine()
{
	// TODO: Add your command handler code here
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_SINGLE_LINE);
	vecOPS.push_back(OPS_QUICK_DELETE_SINGLE_TRIANGLE_BY_LINE);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}


void CMapMatrix3DEditorApp::OnButtonQuickDeleteTriangleBySolid()
{
	// TODO: Add your command handler code here
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_QUICK_DELETE_SINGLE_TRIANGLE_BY_SOLID);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}


void CMapMatrix3DEditorApp::OnButtonStartQuickDeleteTriangleBySolid()
{
	// TODO: Add your command handler code here
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	/*IOsgMfcViewer->ObtainMonomerEventSlot()->QuickClipBySolid();*/

	SetOpsNone();
}


void CMapMatrix3DEditorApp::OnButtonAbsoluteClipSceneByPolygon()
{
	// TODO: Add your command handler code here
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_CLOSED_LINE);
	vecOPS.push_back(OPS_ABS_HOLLOW_OUT_BY_POLYGON);
	IOsgMfcViewer->AddViewerOPS(vecOPS);

}


void CMapMatrix3DEditorApp::OnButtonStartAbsoluteClipSceneByPolygon()
{
	// TODO: Add your command handler code here
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	/*std::string inFileName = vecSceneFileName[0];
	int pos = inFileName.rfind('\\') + 1;
	std::string inFilePath = inFileName.substr(0, pos);
	IOsgMfcViewer->ObtainEditEventSlot()->bsoluteHollowOutByPolygon(inFilePath);*/

}


void CMapMatrix3DEditorApp::OnButtonStartFlatSceneByPolygon()
{
	// TODO: Add your command handler code here
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	std::string inFileName = vecSceneFileName[0];
	int pos = inFileName.rfind('\\') + 1;
	std::string inFilePath = inFileName.substr(0, pos);
	IOsgMfcViewer->ObtainMonomerEventSlot()->flatByPolygon(inFilePath);
}


void CMapMatrix3DEditorApp::OnButtonFlatSceneByPolygon()
{
	// TODO: Add your command handler code here
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_CLOSED_LINE);
	vecOPS.push_back(OPS_FLAT_SCENE_BY_POLYGON);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}



void CMapMatrix3DEditorApp::OnButtonExtractOutlineDrawArea()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}
	SetOpsNone();
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_MULTI_CLOSED_LINE_DBCLICK);
	vecOPS.push_back(OPS_EXTRACT_TRIANGLE_OUTLINE_DRAW_AREA);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonExtractOutlineArea()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}
	SetOpsNone();
	std::vector<OperatingState> vecOPS;

	vecOPS.push_back(OPS_EXTRACT_TRIANGLE_OUTLINE_IN_AREA);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonExtractOutlineAreaNewHeight()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	SetOpsNone();
	std::vector<OperatingState> vecOPS;

	vecOPS.push_back(OPS_EXTRACT_TRIANGLE_OUTLINE_IN_AREA_CHANGE_HEIGHT);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonHighlightObject()
{
	// TODO: Add your command handler code here
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_HIGHLIGHT_OBJECT_BY_CLICK);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}


void CMapMatrix3DEditorApp::OnButtonLinkObjectProperty()
{
	// TODO: Add your command handler code here
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_LINK_PROPERTY_WITH_OBJECT);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::PopUpLinkObjectPropertyListDlg(std::string strPropertyListText)
{
	CPropertyListDlg *dlg = new CPropertyListDlg;
	dlg->Create(ID_DIALOG_PROPERTY_LIST);
	dlg->ShowWindow(SW_SHOWNORMAL);
	dlg->SetPropertyListName(strPropertyListText);
}

void CMapMatrix3DEditorApp::PopUpHighLightMenu()
{

}

void CMapMatrix3DEditorApp::OnButtonExtractPtCloudEdge()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}
	SetOpsNone();
	std::vector<OperatingState> vecOPS;

	vecOPS.push_back(OPS_EXTRCT_PT_CLOUD_EDGE);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonExtractPtCloud()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}
	SetOpsNone();
	std::vector<OperatingState> vecOPS;

	vecOPS.push_back(OPS_EXTRCT_PT_CLOUD);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonSelectPtCloudByArea()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}
	SetOpsNone();
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_MULTI_CLOSED_LINE_DBCLICK);
	vecOPS.push_back(OPS_SELECT_PTCLOUD_BY_Area);
	IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonSetPtCloudDlg()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	double angle, alpha, color;
	theApp.IOsgMfcViewer->ObtainMapperEventSlot()->getPtCloudParameter(angle, alpha, color);
	CSetPtCloudDlg dlg(angle, alpha, color);
	if (dlg.DoModal() == IDOK)
	{
		double angle = dlg.m_angle;
		double alpha = dlg.m_alpha;
		double color = dlg.m_color;
		theApp.IOsgMfcViewer->ObtainMapperEventSlot()->setPtCloudParameter(angle, alpha, color);
	}
}

void CMapMatrix3DEditorApp::OnButtonLookupObjectProperty()
{
	// TODO: Add your command handler code here
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	//弹出对话框
	CLookUpObjectPropertyDlg* dlg = new CLookUpObjectPropertyDlg();
	dlg->Create(IDD_DIALOG_LOOKUP_PROPERTY);
	dlg->ShowWindow(SW_SHOWNORMAL);
}


void CMapMatrix3DEditorApp::OnButtonOutputModelWithoutTexture()
{
	// TODO: Add your command handler code here
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, (LPCTSTR)_TEXT("osgb模型 (*.osgb)|*.osgb|All Files (*.*)|*.*||"), NULL);

	if (dlg.DoModal() == IDOK)
	{
		CString fileExt = dlg.GetFileExt();
		CString filePath = dlg.GetPathName();

		if (fileExt != _T("osgb"))
		{
			filePath += _T(".osgb");
		}

		std::string outputModelWithoutTexture = filePath;
		//theApp.IOsgMfcViewer->ObtainMonomerEventSlot()->utputObjectModelWithoutTexture(outputModelWithoutTexture);
	}
}


void CMapMatrix3DEditorApp::OnButtonSimplifyObjectByPolygon()
{
	// TODO: Add your command handler code here
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_CLOSED_LINE);
	vecOPS.push_back(OPS_SIMPLIFY_OBJECT);
	theApp.IOsgMfcViewer->AddViewerOPS(vecOPS);
}


void CMapMatrix3DEditorApp::OnButtonStartSimplifyObject()
{
	// TODO: Add your command handler code here
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	//theApp.IOsgMfcViewer->

}


void CMapMatrix3DEditorApp::OnButtonOutput3dsModel()
{
	// TODO: Add your command handler code here
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	osg::ref_ptr<osg::Group> root = IOsgMfcViewer->ObtainRoot();
	osg::ref_ptr<osg::Node> sceneNode = root->getChild(0)->asGroup()->getChild(0);

	if (sceneNode)
	{
		CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, (LPCTSTR)_TEXT("3ds模型 (*.3ds)|*.3ds|All Files (*.*)|*.*||"), NULL);

		if (dlg.DoModal() == IDOK)
		{
			CString fileExt = dlg.GetFileExt();
			CString strFileName = dlg.GetPathName();

			if (fileExt != _T("3ds"))
			{
				strFileName += _T(".3ds");
			}

			InterfaceOsgFinderFactory iOsgFinderFactory;
			InterfaceOsgFinder* iOsgFinder = iOsgFinderFactory.create();
			std::vector<osg::Geode*> vecGeode = iOsgFinder->FindAndGetGeode(sceneNode);
			std::vector<osg::MatrixTransform*> vecMatrixTransform;
			iOsgFinder->FindTransNode(sceneNode, vecMatrixTransform);
			osg::Matrix matrix;

			if (vecMatrixTransform.size() > 0)
			{
				matrix = vecMatrixTransform[0]->getMatrix();
			}

			std::string filePathName = strFileName;
			int pos = filePathName.rfind('.');
			std::string filePathNameWithoutExt = filePathName.substr(0, pos);
			int posPath = filePathName.rfind('\\');
			std::string filePath = filePathName.substr(0, posPath + 1);

			for (int i  = 0; i < vecGeode.size(); i++)
			{
				std::stringstream sstr;
				sstr << i;
				std::string str2;
				sstr >> str2;

				std::string geode3dsFileName = filePathNameWithoutExt + str2 + ".3ds";
				osg::ref_ptr<osg::MatrixTransform> mTrans = new osg::MatrixTransform;
				mTrans->setMatrix(matrix);
				mTrans->addChild(vecGeode[i]);

				osgDB::writeNodeFile(*mTrans, geode3dsFileName);

				std::string oldImageFileName = filePath + "Tile_+.jpg";
				std::string newImageFileName = filePathNameWithoutExt + str2 + ".jpg";
				rename(oldImageFileName.c_str(), newImageFileName.c_str());
			}
			
		
		}
	}
}


void CMapMatrix3DEditorApp::OnButtonExtractArea()
{
	// TODO: Add your command handler code here
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	std::string inFileName = vecSceneFileName[0];
	IOsgMfcViewer->ObtainMonomerEventSlot()->extractArea(inFileName);

	SetOpsNone();
}

void CMapMatrix3DEditorApp::OnButtonDrawEdgeHand()
{
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}
	SetOpsNone();
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_EDGE_BY_HAND);

	theApp.IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonExtractRoad()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}
	SetOpsNone();
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_ROAD_PATH);
	vecOPS.push_back(OPS_EXTRACT_ROAD);

	IOsgMfcViewer->AddViewerOPS(vecOPS);

}

void CMapMatrix3DEditorApp::OnButtonDrawEdgeHand1()
{
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}
	SetOpsNone();
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_DRAW_EDGE_BY_HAND_1);
	
	theApp.IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnButtonExtractRoadByDom()
{
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}
	SetOpsNone();
	std::vector<OperatingState> vecOPS;
	
	vecOPS.push_back(OPS_EXTRACT_ROAD_BY_DOM);
	theApp.IOsgMfcViewer->AddViewerOPS(vecOPS);
}

void CMapMatrix3DEditorApp::OnOutTextureObj()
{
	// TODO: Add your command handler code here
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}

	CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, (LPCTSTR)_TEXT("osgb模型 (*.osgb)|*.osgb|All Files (*.*)|*.*||"), NULL);
	
	if (dlg.DoModal() == IDOK)
	{
		CString fileExt = dlg.GetFileExt();
		CString filePath = dlg.GetPathName();

		if (fileExt != _T("osgb") && fileExt != _T("osg"))									//防止忘了输出后缀名
		{
			filePath += _T(".osgb");
		}

		std::string outTextureObjFileName = filePath;
		IOsgMfcViewer->ObtainMonomerEventSlot()->outputTextureModel(outTextureObjFileName);
	}
}


void CMapMatrix3DEditorApp::OnOutNoTextureObj()
{
	// TODO: Add your command handler code here
	CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, (LPCTSTR)_TEXT("osgb模型 (*.osgb)|*.osgb|All Files (*.*)|*.*||"), NULL);

	if (dlg.DoModal() == IDOK)
	{
		CString fileExt = dlg.GetFileExt();
		CString filePath = dlg.GetPathName();

		if (fileExt != _T("osgb") && fileExt != _T("osg"))									//防止忘了输出后缀名
		{
			filePath += _T(".osgb");
		}

		std::string outNoTextureObjFileName = filePath;
		IOsgMfcViewer->ObtainMonomerEventSlot()->outputModelWithoutTexture(outNoTextureObjFileName);
	}
}


void CMapMatrix3DEditorApp::OnDeleteObj()
{
	// TODO: Add your command handler code here
	IOsgMfcViewer->ObtainMonomerEventSlot()->removeSelPagedLod();

}


void CMapMatrix3DEditorApp::OnButtonExtractcontour()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false || CheckIsSingleModel() == false)			//检测
	{
		return;
	}

	//计算场景最高点和最低点
	osg::ref_ptr<osg::Node> sceneNode = IOsgMfcViewer->ObtainRoot()->getChild(0);
	osg::ComputeBoundsVisitor cbbv;
	sceneNode->accept(cbbv);
	osg::BoundingBox bb = cbbv.getBoundingBox();
	double z1 = bb.zMin();
	double z2 = bb.zMax();

	DlgSetContour dlg(z2, z1);
	if (dlg.DoModal() == IDOK)
	{
		double zs = dlg.m_zstart;
		double ze = dlg.m_zend;
		double zspan = dlg.m_zspan;
		theApp.IOsgMfcViewer->ObtainMapperEventSlot()->extractContour(zs, ze, zspan);
	}
	
}


void CMapMatrix3DEditorApp::OnButtonExtractOutlinePg()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}
	SetOpsNone();
	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_EXTRACT_OUTLINE_BY_POSTGIS);
	/*vecOPS.push_back(OPS_SET_NAME);*/
	theApp.IOsgMfcViewer->AddViewerOPS(vecOPS);
}


void CMapMatrix3DEditorApp::OnButtonImportData()
{
	// TODO: 在此添加命令处理程序代码
	//获得exe路径
	char szapipath[MAX_PATH];
	memset(szapipath, 0, MAX_PATH);
	GetModuleFileNameA(NULL, szapipath, MAX_PATH);
	std::string mEXEPath = string(szapipath);
	int pos = mEXEPath.rfind('\\');
	mEXEPath = mEXEPath.substr(0, pos);
	WinExec(CString(mEXEPath.c_str()) + "\\dataImportTool.exe", SW_SHOWNORMAL);
}

void CMapMatrix3DEditorApp::OnButtonHatchSetting()
{
	// TODO: 在此添加命令处理程序代码
	if (CheckPrerequisite() == false)			//检测
	{
		return;
	}
	double step;
	theApp.IOsgMfcViewer->ObtainMapperEventSlot()->getHatchParameter(step);
	DLGHatchSetting dlg(step);
	if (dlg.DoModal() == IDOK)
	{
		step = dlg.mStep;

		theApp.IOsgMfcViewer->ObtainMapperEventSlot()->setHatchParameter(step);
	}
}
