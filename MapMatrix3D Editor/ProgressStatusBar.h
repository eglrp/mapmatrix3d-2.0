#pragma once


// CProgressStatusBar

class CProgressStatusBar : public CStatusBar
{
	DECLARE_DYNAMIC(CProgressStatusBar)

public:
	CProgressStatusBar();
	virtual ~CProgressStatusBar();
	CProgressCtrl& GetProgressCtrl()
	{
		return m_Progress;
	}

	int OnCreate(LPCREATESTRUCT lpCreateStruct); 
	void OnSize(UINT nType, int cx, int cy);

private:
	CProgressCtrl m_Progress;

protected:
	DECLARE_MESSAGE_MAP()
};


