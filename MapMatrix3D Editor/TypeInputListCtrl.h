#pragma once


// CTypeInputListCtrl

class CTypeInputListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CTypeInputListCtrl)

public:
	CTypeInputListCtrl();
	virtual ~CTypeInputListCtrl();

	int obtainSaveiTtem() { return iSaveTtem; }

	int obtainSaveSubiTtem() { return iSaveSubTtem; }

	void ShowEditControl();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	afx_msg LRESULT OnChangeListText(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnTabCtrlList(WPARAM wParam, LPARAM lParam);

protected:
	virtual BOOL EditLabel(int iItem, int iSubItem);

public:
	int iSaveTtem;
	int iSaveSubTtem;	
};


