// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问  
// http://msdn.microsoft.com/officeui。
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// MapMatrix3D EditorView.h : CMapMatrix3DEditorView 类的接口
//

#pragma once
#include "publicDefined.h"
#include "coord.hpp"
#include <vector>
//#include "MFC_OSG.h"
#include "InterfaceOsgMfcViewer.h"


class CMapMatrix3DEditorView : public CView
{
protected: // 仅从序列化创建
	CMapMatrix3DEditorView();
	DECLARE_DYNCREATE(CMapMatrix3DEditorView)

// 特性
public:
	CMapMatrix3DEditorDoc* GetDocument() const;

// 操作
public:
	//用于显示osg
	/*cOSG* mOSG;*/
	//用于封装view类

	//用CString数组存放文件的路径  
	std::vector<CString> ary_filename;  
	//判断是否已经打开过文件
	BOOL openFileFlag;
	HANDLE mThreadHandle;
	////场景路径
	//CString sceneFileName;
private:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();

public:

protected:

// 实现
public:
	virtual ~CMapMatrix3DEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnOpen3dmodel();
	afx_msg void OnRemoveScene();
	afx_msg int OnCreate(LPCREATESTRUCT LPCREATESTRUCT);
	afx_msg void OnDestroy();

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

};

#ifndef _DEBUG  // MapMatrix3D EditorView.cpp 中的调试版本
inline CMapMatrix3DEditorDoc* CMapMatrix3DEditorView::GetDocument() const
   { return reinterpret_cast<CMapMatrix3DEditorDoc*>(m_pDocument); }
#endif

