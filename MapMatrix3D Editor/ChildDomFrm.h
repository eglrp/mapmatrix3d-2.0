#pragma once


// CChildDomFrm

class CChildDomFrm : public CMDIChildWndEx
{
	DECLARE_DYNAMIC(CChildDomFrm)

public:
	CChildDomFrm();
	virtual ~CChildDomFrm();

protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};


