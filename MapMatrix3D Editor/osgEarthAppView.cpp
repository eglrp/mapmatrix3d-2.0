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

// osgEarthAppView.cpp : CosgEarthAppView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MapMatrix3D Editor.h"
#endif

//#include "osgEarthAppDoc.h"
#include "osgEarthAppView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CosgEarthAppView

IMPLEMENT_DYNCREATE(CosgEarthAppView, CView)

BEGIN_MESSAGE_MAP(CosgEarthAppView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	//ON_COMMAND(ID_BUTTON2, &CosgEarthAppView::OnButton2)
END_MESSAGE_MAP()

// CosgEarthAppView 构造/析构

CosgEarthAppView::CosgEarthAppView()
{
	// TODO: 在此处添加构造代码
	openFileFlag = FALSE;
}

CosgEarthAppView::~CosgEarthAppView()
{
}

BOOL CosgEarthAppView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CosgEarthAppView 绘制

void CosgEarthAppView::OnDraw(CDC* /*pDC*/)
{
	CDocument* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	//读取文件
	if (openFileFlag == FALSE)
	{
		openFileFlag = TRUE;

	/*	theApp.IOsgEarth->InitOSGearth("");*/
		/*mThreadHandle = theApp.IOsgEarth->StartRendering();
		std::vector<OperatingState> vecOps;
		vecOps.push_back(OPS_INTERNET_ORIENTATION);
		theApp.IOsgEarth->AddViewerOPS(vecOps);*/
	}

	// TODO: 在此处为本机数据添加绘制代码
}

void CosgEarthAppView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CosgEarthAppView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CosgEarthAppView 诊断

#ifdef _DEBUG
void CosgEarthAppView::AssertValid() const
{
	CView::AssertValid();
}

void CosgEarthAppView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CosgEarthAppDoc* CosgEarthAppView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CosgEarthAppDoc)));
	return (CosgEarthAppDoc*)m_pDocument;
}
#endif //_DEBUG


// CosgEarthAppView 消息处理程序


void CosgEarthAppView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	//InterfaceOsgEarthFactory IOsgEarthFactory;
	//theApp.IOsgEarth = IOsgEarthFactory.Create(m_hWnd);

	////创建按键
	//CRect rect;
	//AfxGetMainWnd()->GetClientRect(rect);

	//myBtn1.Create(_T("定位"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,CRect(rect.right/2-100,rect.bottom/2,rect.right/2-50, rect.bottom/2+50),
	//	this,1);
	//myBtn1.ShowWindow(SW_SHOW);
}


//void CosgEarthAppView::OnButton2()
//{
//	// TODO: 在此添加命令处理程序代码
//	openFileFlag = FALSE;
//
//	this->Invalidate(TRUE);
//}
