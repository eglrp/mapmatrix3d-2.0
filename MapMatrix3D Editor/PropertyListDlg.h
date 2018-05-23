#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "TypeInputListCtrl.h"
#include "DynamicEdit.h"
#include <string>
#include <map>
#include <vector>
#include "publicDefined.h"

// CPropertyListDlg dialog

class CPropertyListDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPropertyListDlg)

public:
	CPropertyListDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPropertyListDlg();

	virtual void SetPropertyListName(std::string strPropertyList);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = ID_DIALOG_PROPERTY_LIST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual void CenterScreen();

	virtual bool ExtractSubPropertyListText(std::string propertyList, std::string &subPropertyList);

	virtual void ExtractPropertyListName(std::string propertyListName, std::vector<CPropertyListText> &vecPropertyName);

	virtual void FindSpacePos(std::string propertyName, std::vector<int> &vecSpacePos);

	virtual void ExtractWord(std::string propertyName, std::vector<CPropertyListText> &vecPropertyName, std::vector<int> vecPos);

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

protected:
	afx_msg void OnBnClickedOk();

public:
	CTypeInputListCtrl dynamicCtrlList;

	CPropertyDynamicEdit dynamicEditContrl;
};
