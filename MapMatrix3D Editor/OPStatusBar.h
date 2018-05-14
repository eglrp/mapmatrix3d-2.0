#pragma once
#include <string>
#include "OSG_Plus.h"
// OPStatusBar

class OPStatusBar : public CMFCRibbonStatusBar
{
	DECLARE_DYNAMIC(OPStatusBar)

public:
	OPStatusBar();
	virtual ~OPStatusBar();
public:
	void ShowCoordinate(double gx, double gy, float ele);			//设置坐标
	void ShowDeviceCoordinate(double x, double y, double z);
	void ShowHint(char* strHint);
	void ShowDeviceCoordinate(LPCTSTR strDevciecoordinate);
	CWnd* GetProgressBarWindow();
	void SetProgressBarPos(int POS);								//设置位置
	void StartProgress(int elapseTime);								//开始进度条
	void EndProgress();												//结束进度条

private:
	CStatic progress_bar;
	CMFCRibbonProgressBar* m_wndProgressBar;
	//OpenThreads::Mutex  _mutex;

	enum  
	{  
		ID_TIMER_1 = 100,  
		ID_TIMER_2 = 101,  
	};  

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT LPCREATESTRUCT);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void SetTimer1(int elapseTime);
	void ShowPercentage(int percentage);
	void EndTimer();

public:
	int pos;
	afx_msg void OnPaint();
	void OnFillBackground(CDC* pDC, CRect rectClient);
};


