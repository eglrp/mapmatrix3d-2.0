#pragma once
#include <string>
#include "OSG_Plus.h"
#include "publicDefined.h"
#include "InterfaceOsgMfcViewer.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceOsgEditEvent.h"
#include "InterfaceOsgOrientEvent.h"
#include "InterfaceOsgMonomerEvent.h"
#include "InterfaceOsgMapperEvent.h"

#ifdef MFCVIEWERIBDLL  
#define MFCVIEWERAPI _declspec(dllexport)  
#else  
#define MFCVIEWERAPI  _declspec(dllimport)  
#endif  


class MFCVIEWERAPI InterfaceOsgMfcViewer
{
public:
	virtual void InitOSG(std::string modelname) = 0;																	//加载模型

	virtual HANDLE StartRendering() = 0;																				//开始渲染

	virtual void AddChild(osg::ref_ptr<osg::Node> parent, osg::ref_ptr<osg::Node> node) = 0;							//加节点

	virtual void RemoveChild(osg::ref_ptr<osg::Node> parent, osg::ref_ptr<osg::Node> node) = 0;							//清除节点

	virtual void RemoveAllChild() = 0;																					//移除所有节点

	virtual void SetSceneExtent() = 0;																					//设置范围

	virtual void AddViewerOPS(std::vector<OperatingState> vecOps) = 0;													//设置浏览器操作事件

	virtual void ChangeView(int num) = 0;																				//改变视角 0前 1上	2下 3后 4左 5右

	virtual void ShowFillTriangle() = 0;																				//显示带网格的三角面片

	virtual osgViewer::Viewer* getViewer() = 0;																			//获取浏览器

	virtual osg::ref_ptr<osg::Group> ObtainRoot() = 0;																	//获取root节点	

	virtual InterfaceSigEventCallBack* ObtainISig() = 0;																//获取信号的接口

	virtual InterfaceOsgOrientEvent* ObtainOrientEventSlot() = 0;														//获取定向事件槽接口

	virtual InterfaceOsgEditEvent* ObtainEditEventSlot() = 0;															//获取编辑事件槽的接口

	virtual InterfaceOsgMapperEvent* ObtainMapperEventSlot() = 0;														//获取编辑事件槽的接口

	virtual InterfaceOsgMonomerEvent* ObtainMonomerEventSlot() = 0;														//获取单体化事件槽的接口

	virtual void simulateMouseButtonEvent() = 0;																		//模拟鼠标点击事件
};


class MFCVIEWERAPI InterfaceOsgMfcViewerFactory
{
public:
	InterfaceOsgMfcViewer* create(HWND hWnd);
};

