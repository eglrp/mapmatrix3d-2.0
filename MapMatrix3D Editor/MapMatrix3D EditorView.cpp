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

// MapMatrix3D EditorView.cpp : CMapMatrix3DEditorView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MapMatrix3D Editor.h"
#endif

#include "MapMatrix3D EditorDoc.h"
#include "MapMatrix3D EditorView.h"
#include "MainFrm.h"
#include "MapMatrix3D Editor.h"
#include "FileView.h"
#include "ProgressBar.h"
#include "publicDefined.h"
#include "cv_plus.hpp"
#include "DOMResSetDlg.h"
#include <gl\GL.h>
#include "OPStatusBar.h"
#include "boost/signals2/signal.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif   

//extern void CallBackStartProgress();


IMPLEMENT_DYNCREATE(CMapMatrix3DEditorView, CView)

BEGIN_MESSAGE_MAP(CMapMatrix3DEditorView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_OPEN_3DMODEL, &CMapMatrix3DEditorView::OnOpen3dmodel)
	ON_COMMAND(ID_REMOVE_SCENE, &CMapMatrix3DEditorView::OnRemoveScene)
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

#ifdef _DEBUG
void CMapMatrix3DEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CMapMatrix3DEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMapMatrix3DEditorDoc* CMapMatrix3DEditorView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMapMatrix3DEditorDoc)));
	return (CMapMatrix3DEditorDoc*)m_pDocument;
}
#endif //_DEBUG
// CMapMatrix3DEditorView 构造/析构

CMapMatrix3DEditorView::CMapMatrix3DEditorView()
{
	// TODO: 在此处添加构造代码

}

CMapMatrix3DEditorView::~CMapMatrix3DEditorView()
{
}

BOOL CMapMatrix3DEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	CRect rect;
	return CView::PreCreateWindow(cs);
}

void CMapMatrix3DEditorView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	// TODO: 在此添加专用代码和/或调用基类
	InterfaceOsgMfcViewerFactory IViewerFactory;
	theApp.IOsgMfcViewer = IViewerFactory.create(m_hWnd);
	openFileFlag = FALSE;

	theApp.InitCallBack();
}

// CMapMatrix3DEditorView 消息处理程序

void CMapMatrix3DEditorView::OnOpen3dmodel()
{
	// TODO: 在此添加命令处理程序代码
	CFileDialog dialog(TRUE, NULL, NULL, OFN_HIDEREADONLY, (LPCTSTR)_TEXT("3D model Files (*.osgb, *.osg, *.obj)|*.osgb;*.osg;*.obj;*.osgt|All Files (*.*)|*.*||"),NULL);

	if (dialog.DoModal() == IDOK)
	{
		CString fileName = dialog.GetPathName();
		ary_filename.push_back(fileName);
	}

	if (ary_filename.size() > 0)
	{
		this->Invalidate(TRUE);
	}

}

void CMapMatrix3DEditorView::OnDraw(CDC* /*pDC*/)
{	
	if (!ary_filename.empty())
	{
		std::string strModel = ary_filename[0]; 

		if (openFileFlag == TRUE)
		{
			ary_filename.clear();
			theApp.IOsgMfcViewer->ObtainISig()->EmitSigShowOpsHint("正在打开场景...");
			theApp.IOsgMfcViewer->ObtainISig()->EmitSigSetProgressFun(500);
			theApp.IOsgMfcViewer->ObtainISig()->EmitSigSetProgressFun(502);
		
			
			theApp.AddScene(strModel);
			Sleep(1000);
			theApp.IOsgMfcViewer->getViewer()->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KEY_Space);
			//theApp.IOsgMfcViewer->ObtainISig()->EmitSigSetProgressFun(900);
			theApp.IOsgMfcViewer->ObtainISig()->EmitSigSetProgressFun(999);
			theApp.IOsgMfcViewer->ObtainISig()->EmitSigEndProgressFun();
			theApp.IOsgMfcViewer->ObtainISig()->EmitSigShowOpsHint("");
		}
		else if (openFileFlag == FALSE)
		{
			openFileFlag = TRUE;
			ary_filename.clear();
			theApp.IOsgMfcViewer->ObtainISig()->EmitSigShowOpsHint("正在打开场景...");
			theApp.IOsgMfcViewer->ObtainISig()->EmitSigSetProgressFun(500);
			
			theApp.IOsgMfcViewer->ObtainISig()->EmitSigSetProgressFun(502);
			theApp.IOsgMfcViewer->InitOSG(strModel);

			//提取路径
			CString sceneFileName = strModel.c_str();
			int pos = sceneFileName.ReverseFind('\\');
			CString sceneFilePath = sceneFileName.Left(pos);
			theApp.vecSceneFileName.push_back(sceneFileName);
			CString fileTitle = sceneFileName.Right(sceneFileName.GetLength() - 1 - pos);

			//设置范围
			theApp.IOsgMfcViewer->SetSceneExtent();

			//添加路径到视图上
			CMainFrame *pMain=(CMainFrame *)AfxGetApp()->m_pMainWnd;
			pMain->insertItem(sceneFilePath, fileTitle);

			mThreadHandle = theApp.IOsgMfcViewer->StartRendering();

			
			theApp.IOsgMfcViewer->getViewer()->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KEY_L);
			Sleep(100);
			theApp.IOsgMfcViewer->getViewer()->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KEY_Space);
			theApp.IOsgMfcViewer->ObtainISig()->EmitSigSetProgressFun(999);
			theApp.IOsgMfcViewer->ObtainISig()->EmitSigEndProgressFun();
			theApp.IOsgMfcViewer->ObtainISig()->EmitSigShowOpsHint("");
		}
		
		//if (ary_filename.size() > 1)
		//{
		//	for (int i = 1; i < ary_filename.size(); i++)
		//	{
		//		std::string strTemp = ary_filename[i];
		//		osg::ref_ptr<osg::Node> tempNode1 = osgDB::readNodeFile(strTemp);
		//		osg::ref_ptr<osg::Group> group = mOSG->mRoot->getChild(0)->asGroup();
		//		group->addChild(tempNode1);

		//		//提取路径
		//		sceneFileName = strTemp.c_str();
		//		int pos = sceneFileName.ReverseFind('\\');
		//		CString sceneFilePath = sceneFileName.Left(pos);
		//		vecSceneFileName.push_back(sceneFileName);
		//		CString fileTitle = sceneFileName.Right(sceneFileName.GetLength() - 1 - pos);

		//		//添加路径到视图上
		//		CMainFrame *pMain=(CMainFrame *)AfxGetApp()->m_pMainWnd;
		//		pMain->insertItem(sceneFilePath, fileTitle);
		//	}

		//}

	}

	CMapMatrix3DEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}

void CMapMatrix3DEditorView::OnRemoveScene()
{
	// TODO: 在此添加命令处理程序代码
	if(theApp.isLoadModel() == false)
	{
		return;
	}
	
	theApp.RemoveAllScene();
}


int CMapMatrix3DEditorView::OnCreate(LPCREATESTRUCT LPCREATESTRUCT)
{
	if (CView::OnCreate(LPCREATESTRUCT) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	return 0;
}

void CMapMatrix3DEditorView::OnDestroy()
{
	CView::OnDestroy();
	// TODO: 在此处添加消息处理程序代码
}

BOOL CMapMatrix3DEditorView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE)
		{
			if (!theApp.isLoadModel())
			{
				return FALSE;
			}
			std::vector<OperatingState> vecOPS;
			vecOPS.push_back(OPS_None);
			theApp.IOsgMfcViewer->AddViewerOPS(vecOPS);
		}
	}

	return CView::PreTranslateMessage(pMsg);
}


BOOL CMapMatrix3DEditorView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect   m_rt;
	GetClientRect(&m_rt);
	CBrush   brush;
	brush.CreateSolidBrush(RGB(60, 60, 60));
	pDC->FillRect(&m_rt, &brush);
	DeleteObject(brush);

	return TRUE;
	//return CView::OnEraseBkgnd(pDC);
}
