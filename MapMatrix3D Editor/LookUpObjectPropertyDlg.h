#pragma once


// CLookUpObjectPropertyDlg dialog

class CLookUpObjectPropertyDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLookUpObjectPropertyDlg)

public:
	CLookUpObjectPropertyDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLookUpObjectPropertyDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LOOKUP_PROPERTY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	afx_msg void OnBnClickedLookup();

protected:


protected:
	virtual void CenterScreen();
public:
	virtual BOOL OnInitDialog();
};
