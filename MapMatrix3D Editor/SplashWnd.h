#pragma once
#include "atlimage.h"

// CSplashWnd

class CSplashWnd : public CWnd
{
	DECLARE_DYNAMIC(CSplashWnd)

public:
	CSplashWnd();
	virtual BOOL create();
	afx_msg void OnPaint();
	virtual ~CSplashWnd();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	CBitmap m_bitmap;
	
};


