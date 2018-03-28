#pragma once
#include "operationState.h"
#include "OSG_Plus.h"
#include "publicDefined.h"
#include "InterfaceOsgMfcViewer.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceOsgEditEvent.h"
#include "InterfaceOsgMeasureEvent.h"
#include "InterfaceOsgOrientEvent.h"
#include "InterfaceOsgMonomerEvent.h"
#include "InterfaceOsgMapperEvent.h"
#include "InterfaceOsgDrawEvent.h"

#include <string>

class MFCVIEWERAPI cOSG : public InterfaceOsgMfcViewer
{
public:
    cOSG(HWND hWnd);
    ~cOSG();

    void InitOSG(std::string modelname);

	HANDLE StartRendering();																						//开始渲染				

	void AddChild(osg::ref_ptr<osg::Node> parent, osg::ref_ptr<osg::Node> node);									//加节点

	void RemoveChild(osg::ref_ptr<osg::Node> parent, osg::ref_ptr<osg::Node> node);									//清除节点

	void SetSceneExtent();																							//设置场景范围

	osgViewer::Viewer* getViewer();																					//获取浏览器指针

	void RemoveAllChild();																							//清除所有节点

	void AddViewerOPS(std::vector<OperatingState> vecOps);															//设置浏览器操作世界

	void ChangeView(int num);																						//改变视角

	InterfaceSigEventCallBack* ObtainISig();																		//获取信号的接口

	InterfaceOsgOrientEvent* ObtainOrientEventSlot();																//获取定向事件槽的接口

	InterfaceOsgEditEvent* ObtainEditEventSlot();																	//获取编辑事件槽的接口

	InterfaceOsgMapperEvent* ObtainMapperEventSlot();																//获取测图事件槽的接口

	InterfaceOsgMonomerEvent* ObtainMonomerEventSlot();																//获取单体化事件槽的接口

	osg::ref_ptr<osg::Group> ObtainRoot();																			//获取跟节点

	void simulateMouseButtonEvent();																				//模拟鼠标点击事件
	

public:
	void PreFrameUpdate(void);
	
	void PostFrameUpdate(void);
	
protected:
    void InitManipulators(void);
    void InitSceneGraph(void);
    void InitCameraConfig(void);

    void Done(bool value) { mDone = value; }
    bool Done(void) { return mDone; }
    static void Render(void* ptr);

	void LookDown();																								//俯视

	void LookUp();																									//仰视

	void LeftView();																								//左视

	void RightView();																								//右视

	void FrontView();																								//前视

	void RearView();																								//后视	

	void ShowFillTriangle();																						//显示带网格的三角面

	bool LinePickUpPoint(osg::Vec3d start, osg::Vec3d end, osg::Vec3d &worldPt, osg::ref_ptr<osg::Node> pickUpNode);//垂直碰撞检测


protected:
	InterfaceSigEventCallBack* iSigEventCallBack;

	InterfaceOsgEditEvent* iOsgEditEvent;

	InterfaceOsgOrientEvent* iOsgOrientEvent;
	
	InterfaceOsgMeasureEvent* iOsgMeasureEvent;

	InterfaceOsgMonomerEvent* iOsgMonomerEvent;

	InterfaceOsgDrawEvent* iOsgDrawEvent;

	InterfaceOsgMapperEvent* iOsgMapperEvent;

    bool mDone;
    std::string m_ModelName;
    HWND m_hWnd;
	osg::ref_ptr<osgViewer::Viewer> mViewer;
    osg::ref_ptr<osg::Group> mRoot;
    osg::ref_ptr<osg::Node> mModel;
    osg::ref_ptr<osgGA::TerrainManipulator> trackball;
	osg::ref_ptr<osgGA::TerrainManipulator> terrain;
    osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator;

	//保存场景数据节点组
	osg::ref_ptr<osg::Group> mSceneGroup;
	//保存场景编辑组
	osg::ref_ptr<osg::Group> mEditSceneGroup;
	//保存鼠标坐标
	osg::ref_ptr<osg::Group> mViewerGroup;
	//保存测图结果
	osg::ref_ptr<osg::Group> mMapperGroup;
	//保存天空盒节点
	osg::ref_ptr<osg::Group> mSkyBoxGroup;
	//临时组
	osg::ref_ptr<osg::Group> mTempGroup, mTempGroup1, mScopeGroup;

	BOOL isShowFillWireTriangle;

protected:
	void InitialViewerGroup(osg::ref_ptr<osg::Group> mViewerGroup);
	void InitialSkyBox(osg::ref_ptr<osg::Group> mSkyBoxGroup);
	void InitViewerEvent();
};

class CRenderingThread : public OpenThreads::Thread
{
public:
    CRenderingThread( cOSG* ptr );
    virtual ~CRenderingThread();

    virtual void run();

protected:
    cOSG* _ptr;
    bool _done;
};

