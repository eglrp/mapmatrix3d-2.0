// DomDoc.cpp : 实现文件
//

#include "stdafx.h"
#include "MapMatrix3D Editor.h"
#include "DomDoc.h"


// CDomDoc

IMPLEMENT_DYNCREATE(CDomDoc, CDocument)

CDomDoc::CDomDoc()
{
}

BOOL CDomDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CDomDoc::~CDomDoc()
{
}


BEGIN_MESSAGE_MAP(CDomDoc, CDocument)
END_MESSAGE_MAP()


// CDomDoc 诊断

#ifdef _DEBUG
void CDomDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CDomDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CDomDoc 序列化

void CDomDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}
#endif


// CDomDoc 命令
