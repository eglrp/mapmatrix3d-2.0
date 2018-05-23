// DomView.cpp : 实现文件
//

#include "stdafx.h"
#include "MapMatrix3D Editor.h"
#include "DomView.h"
#include <osgDB/WriteFile>
#include <osgDB/ReadFile>
#include <osg/DrawPixels>
#include <osgUtil/Optimizer>
#include <osgViewer/Viewer>
#include "AbsoluteOrientationDlg.h"
#include "qcomm.h"
// CDomView
extern CAbsoluteOrientationDlg* g_mfcAbsoluteOrientationDlg;
IMPLEMENT_DYNCREATE(CDomView, CView)

CDomView::CDomView()
{
	isPaint = TRUE;
	scale = 1;
	zommImageWidth = 0;
	zoomImageHeight = 0;
	moveX = 0;
	moveY = 0;
	strFileName = "";
	isMoveWindow = false;
}

CDomView::~CDomView()
{
}

BEGIN_MESSAGE_MAP(CDomView, CView)
	ON_WM_MOUSEWHEEL()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_OPEN_IMAGE, &CDomView::OnOpenImage)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// CDomView 绘图

void CDomView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码

	//读取影像
	if(isPaint && strFileName != "")
	{
		HRESULT  hResult = m_Image.Load(strFileName);

		if (FAILED(hResult)) 
		{ 
			MessageBox(_T("无法加载影像"));	
			return;
		} 

		isPaint = FALSE;													
		CRect zcRect;
		GetClientRect(zcRect);
		double xScale = (double)zcRect.Width() /   m_Image.GetWidth();		//刚好相反
		double yScale = (double)zcRect.Height() / m_Image.GetHeight();

		if (xScale < yScale)
		{
			scale = yScale + 0.01;			//确保覆盖全框
		}
		else
		{
			scale = xScale + 0.01;			//确保覆盖全框
		}

	}

	if (!m_Image.IsNull())
	{
		zommImageWidth = m_Image.GetWidth() * scale;
		zoomImageHeight = m_Image.GetHeight() * scale; 

		//将图像显示在界面上
		HRESULT hResult = m_Image.Draw(pDC->m_hDC, moveX, moveY, zommImageWidth, zoomImageHeight);

		if (FAILED(hResult))
		{
			MessageBox(_T("调用图像文件失败"));
			return;
		}

	}

	if (vecImgPoint.size() > 0)
	{
		for (int i = 0 ; i < vecImgPoint.size(); i ++)
		{
			HDC hdc = ::GetDC(m_hWnd);
			double x = vecImgPoint[i].x * scale + moveX ;
			double y = vecImgPoint[i].y * scale + moveY;
			::MoveToEx(hdc, x - 10, y, NULL);  
			::LineTo(hdc, x + 10, y);
			::MoveToEx(hdc, x, y - 10, NULL);  
			::LineTo(hdc, x, y  + 10);
			::ReleaseDC(m_hWnd, hdc);
		}

	}
}


// CDomView 诊断

#ifdef _DEBUG
void CDomView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CDomView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CDomView 消息处理程序


BOOL CDomView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (!CView::PreCreateWindow(cs))
	{
		return FALSE;
	}

	return TRUE;
}


void CDomView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
}

BOOL CDomView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (CView::OnMouseWheel(nFlags, zDelta, pt))
	{
		return FALSE;
	}

	GetCursorPos(&pt); 
	ScreenToClient(&pt);
	
	double imagePointX = (pt.x - moveX) / scale;
	double imagePointY = (pt.y - moveY) / scale;

	CRect zcRect;
	GetClientRect(zcRect);
	double newScale = scale + 0.1 * zDelta / 120;

	//缩放位移量
	double newMoveX = moveX;
	double newMoveY = moveY;
	newMoveX += imagePointX * scale - imagePointX * newScale;
	newMoveY += imagePointY * scale - imagePointY * newScale;

	if (newMoveX > 0 || newMoveY > 0)							//不允许超出范围
	{
		return FALSE;
	}

	long right = m_Image.GetWidth() * newScale + newMoveX;
	long bottom = m_Image.GetHeight() * newScale + newMoveY; 

	if (right < zcRect.right)									//右下角不超出范围
	{
		return FALSE;
	}

	if (bottom < zcRect.bottom)
	{
		return FALSE;
	}

	moveX = newMoveX;											//获得新值
	moveY = newMoveY;
	scale = newScale;

	this->Invalidate(TRUE);
	return TRUE;
}


void CDomView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
//
//	CView::OnMButtonDown(nFlags, point);
//	ops = OPS_DRAG_IMAGE_MOVE;
//	GetCursorPos(&point); 
//	ScreenToClient(&point);
//	lastMouseX = point.x;
//	lastMouseY = point.y;
//}


void CDomView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//CView::OnMButtonUp(nFlags, point);
	//lastMouseX = 0;
	//lastMouseY = 0;
	//ops = OPS_None;
}


void CDomView::OnMouseMove(UINT nFlags, CPoint point)
{
	//// TODO: 在此添加消息处理程序代码和/或调用默认值
	//CView::OnMouseMove(nFlags, point);
	//GetCursorPos(&point); 
	//ScreenToClient(&point);

	//if (ops == OPS_DRAG_IMAGE_MOVE)
	//{
	//	CRect zcRect;
	//	GetClientRect(zcRect);										//获取客户端范围
	//	double newMoveX = moveX;
	//	double newMoveY = moveY;
	//	newMoveX += point.x - lastMouseX;
	//	newMoveY += point.y - lastMouseY;

	//	if (newMoveX > 0 || newMoveY > 0)							//左上角不允许超出范围
	//	{
	//		return;
	//	}

	//	long right = m_Image.GetWidth() * scale + newMoveX;
	//	long bottom = m_Image.GetHeight() * scale + newMoveY; 

	//	if (right < zcRect.right)									//右下角不超出范围
	//	{
	//		return;
	//	}

	//	if (bottom < zcRect.bottom)
	//	{
	//		return;
	//	}

	//	//保存值
	//	lastMouseX = point.x;
	//	lastMouseY = point.y;
	//	moveX = newMoveX;
	//	moveY = newMoveY;
	//	this->Invalidate(TRUE);
	//}
}


void CDomView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	/*CView::OnLButtonDblClk(nFlags, point);
	long imagePointX = (point.x - moveX) / scale;
	long imagePointY = (point.y - moveY) / scale;
	vecImgPoint.push_back(CoordDouble2D(imagePointX, imagePointY));
	double xCoord =  imagePointX * xResolution + xStart;
	double yCoord = imagePointY * yResolution + yStart;
	double zCoord = 10.0;
	g_mfcAbsoluteOrientationDlg->ObtainImageControlPoint(xCoord, yCoord, zCoord, false);
	this->Invalidate(TRUE);*/
}


void CDomView::OnOpenImage()
{
	// TODO: 在此添加命令处理程序代码
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, (LPCTSTR)_TEXT("图片文件 (*.tif)|*.tif|All Files (*.*)|*.*||"), NULL);

	if (dlg.DoModal() == IDOK)
	{
		strFileName = dlg.GetPathName();
		int pos = strFileName.ReverseFind('.');
		CString strFileNameWithoutExt =  strFileName.Left(pos);
		CString strCoordFileName = strFileNameWithoutExt + _T(".tfw");
		double temp1 = 0; double temp2 = 0;
		FILE* fp = fopen((LPSTR)(LPCTSTR)strCoordFileName, "r");

		if (fp ==NULL)
		{
			MessageBox("无对应的坐标文件！");
			return;
		}

		fscanf(fp, "%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n", &xResolution, &temp1, &temp2, &yResolution, &xStart, &yStart);
	}

	this->Invalidate(TRUE);
}


void CDomView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: 在此处添加消息处理程序代码
	CMenu menu;
	menu.LoadMenuA(IDR_MENU5);									//加载菜单资源
	CMenu *pPopup=menu.GetSubMenu(0);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, this);
}

