#pragma once
#include "publicDefined.h"
#include <string>
#include "coord.hpp"
#include <vector>

// CDomView 视图

class CDomView : public CView
{
	DECLARE_DYNCREATE(CDomView)
public:
		
protected:
	CDomView();           // 动态创建所使用的受保护的构造函数
	virtual ~CDomView();
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	DECLARE_MESSAGE_MAP()
	
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

private:	
	BOOL isPaint;													//是否绘制
	double scale;													//缩放比例
	int zoomImageHeight;											//缩放后的影像高
	int zommImageWidth;												//缩放后的影像宽
	long moveX;														//X方向影像移动
	long moveY;														//Y方向影像移动
	CImage m_Image;													//影像底图
	OperatingState ops;												//操作标识
	long lastMouseX;												//上次的x方向影像移动
	long lastMouseY;												//上次的y方向影像移动
	CString strFileName;											//读取影像路径文件名
	std::vector<CoordDouble2D> vecImgPoint;							//刺点影像坐标
	double xResolution;												//x方向分辨率
	double yResolution;												//y方向分辨率
	double xStart;													//x方向开始坐标
	double yStart;													//y方向开始坐标
	bool isMoveWindow;												//是否移动窗口
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnOpenImage();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};


