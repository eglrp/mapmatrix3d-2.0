#pragma once


// CDynamicEdit

class CPropertyDynamicEdit : public CEdit
{
	DECLARE_DYNAMIC(CPropertyDynamicEdit)

public:
	CPropertyDynamicEdit();
	virtual ~CPropertyDynamicEdit();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	virtual void TabKeyOperation();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);

};


