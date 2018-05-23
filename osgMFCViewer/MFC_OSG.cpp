// MFC_OSG.cpp : implementation of the cOSG class
//
#include "stdafx.h"
#include "osgShowCoordEvent.h"
#include "BaseUserEvent.h"
#include "SelfDefTerrainManipulator.h"
#include "SelfDefTrackBallManipulator.h"
#include "InterfaceOsgEditor.h"
#include "InterfaceMFCExcute.h"
#include <osg/ComputeBoundsVisitor>
#include "qcomm.h"
#include "MFC_OSG.h"
#include "ThreadSafeUpdateCallback.h"
#include "compass.h"
//#include "OsgStateDynamicSetter.h"

//#include "skyBox.h"
osg::ref_ptr<osg::Group> cOSG::ObtainRoot()
{
	return mRoot.get();
}

cOSG::cOSG(HWND hWnd) :
   m_hWnd(hWnd)
{
	InterfaceSigEventCallBackFactory iSigEventCallBackFactory;
	iSigEventCallBack = iSigEventCallBackFactory.create();

	isShowFillWireTriangle = FALSE;
}

cOSG::~cOSG()
{
    mViewer->setDone(true);
    Sleep(1000);
    mViewer->stopThreading();

    delete mViewer;
}

void cOSG::InitOSG(std::string modelname)
{
	// Store the name of the model to load

    m_ModelName = modelname;

    // Init different parts of OSG
    InitManipulators();
    InitSceneGraph();
    InitCameraConfig();
}

void cOSG::InitManipulators(void)
{
    // create a trackball manipulator
    trackball = new CSelfDefTrackBallManipulator();
	terrain = new CSelfDefTerrainManipulator();

    // create a Manipulator Switcher
    keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;

    // Add our trackball manipulator to the switcher
    keyswitchManipulator->addMatrixManipulator( '1', "Trackball", trackball.get());
	keyswitchManipulator->addMatrixManipulator( '2', "Terrain", terrain.get());

    // Init the switcher to the first manipulator (in this case the only manipulator)
    keyswitchManipulator->selectMatrixManipulator(1);  // Zero based index Value
}

void cOSG::InitSceneGraph(void)
{
    // Init the main Root Node/Group
    mRoot  = new osg::Group;
	mRoot->addUpdateCallback(new CThreadSafeUpdateCallback());

	mSceneGroup = new osg::Group;
	mSceneGroup->setName("sceneGroup");
	mEditSceneGroup = new osg::Group;
	mEditSceneGroup->setName("editGroup");
	mViewerGroup = new osg::Group;
	mViewerGroup->setName("viewerGroup");
	mMapperGroup = new osg::Group;
	mMapperGroup->setName("mapperGroup");
	mSkyBoxGroup = new osg::Group;
	mSkyBoxGroup->setName("SkyBoxGroup");
	mTempGroup = new osg::Group;
	mTempGroup->setName("tempGroup");
	mTempGroup1 = new osg::Group;
	mTempGroup1->setName("tempGroup1");
	mScopeGroup = new osg::Group;
	mScopeGroup->setName("ScopeGroup");
    // Load the Model from the model name	

    mModel = osgDB::readNodeFile(m_ModelName);

	if (!mModel)
	{
	/*	AfxMessageBox("无法正常读取数据，请检测数据");*/
		return;
	}

	mModel->setName("model1");
	
    if (!mModel) 
	{
		return;
	}	

	InterfaceOsgEditorFactory iOsgEditorFactory;
	InterfaceOsgEditor* iOsgEditor = iOsgEditorFactory.create();
	iOsgEditor->SetImageDataVariance(mModel);

    // Optimize the model
    osgUtil::Optimizer optimizer;
    optimizer.optimize(mSceneGroup.get());
    optimizer.reset();
	 
	osg::ref_ptr<CThreadSafeUpdateCallback> spCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());
    // Add the model to the scene
	if (spCallback != NULL)
	{
		mRoot->addChild(mSceneGroup.get());
		mRoot->addChild(mEditSceneGroup.get());
		mRoot->addChild(mViewerGroup.get());
		mRoot->addChild(mMapperGroup.get());
		mRoot->addChild(mSkyBoxGroup.get());
		mRoot->addChild(mTempGroup.get());
		//mRoot->addChild(mScopeGroup.get());

		InitialViewerGroup(mViewerGroup);
		//InitialSkyBox(mSkyBoxGroup);//创建天空盒
	}	

	if (spCallback != NULL)
	{
		mSceneGroup->addChild(mModel.get());
	}
}

void cOSG::InitialSkyBox(osg::ref_ptr<osg::Group> mSkyBoxGroup)
{
	
}

void cOSG::InitialViewerGroup(osg::ref_ptr<osg::Group> mViewerGroup)
{
	//初始化坐标文字
	osgText::Text* posText = new osgText::Text;
	osgText::Text* posText1 = new osgText::Text;
	osgText::Text* posText2 = new osgText::Text;
	osgText::Text* posText3 = new osgText::Text;
	std::string caiyun("fonts/simhei.ttf");//此处设置的是汉字字体
	posText->setFont(caiyun); 
	posText1->setFont(caiyun); 
	posText2->setFont(caiyun); 
	posText3->setFont(caiyun); 
	//设置文字显示的位置
	osg::Vec3 position(10.0f,50.0f,0.0f);
	posText->setPosition(position); 
	position.y()-=12;
	posText1->setPosition(position);
	position.y()-=12;
	posText2->setPosition(position);
	position.y()-=12;
	posText3->setPosition(position);

	posText->setCharacterSize(8.0);//字体大小
	posText1->setCharacterSize(8.0);//字体大小
	posText2->setCharacterSize(8.0);//字体大小
	posText3->setCharacterSize(8.0);//字体大小
	posText->setColor( osg::Vec4( 1, 1, 1, 1));
	posText1->setColor( osg::Vec4( 1, 1, 1, 1));
	posText2->setColor( osg::Vec4( 1, 1, 1, 1));
	posText3->setColor( osg::Vec4( 1, 1, 1, 1));
	posText->setDataVariance(osg::Object::DYNAMIC);
	posText1->setDataVariance(osg::Object::DYNAMIC);
	posText2->setDataVariance(osg::Object::DYNAMIC);
	posText3->setDataVariance(osg::Object::DYNAMIC);

	posText->setText("Row:0 Col:0");//设置显示的文字
	posText1->setText("[X]:0.0");
	posText2->setText("[Y]:0.0");
	posText3->setText("[Z]:0.0");
	osg::Geode* textGeode = new osg::Geode();
	textGeode->addDrawable(posText);
	textGeode->addDrawable(posText1);
	textGeode->addDrawable(posText2);
	textGeode->addDrawable(posText3);
	//设置状态
	osg::StateSet* stateset = textGeode->getOrCreateStateSet(); 
	stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);//关闭灯光
	stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);//关闭深度测试
	//打开GL_BLEND混合模式（以保证Alpha纹理正确）
	stateset->setMode(GL_BLEND,osg::StateAttribute::ON);

	osg::Camera* textCamera = new osg::Camera;
	//设置透视矩阵
	textCamera->setProjectionMatrix(osg::Matrix::ortho2D(0,600,0,600));//正交投影   
	//设置绝对参考坐标系，确保视图矩阵不会被上级节点的变换矩阵影响
	textCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	//视图矩阵为默认的
	textCamera->setViewMatrix(osg::Matrix::identity());
	//设置背景为透明，否则的话可以设置ClearColor 
	textCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
	textCamera->setAllowEventFocus( false);//不响应事件，始终得不到焦点
	//设置渲染顺序，必须在最后渲染
	textCamera->setRenderOrder(osg::Camera::RenderOrder::POST_RENDER); 

	osg::ref_ptr<CThreadSafeUpdateCallback> spCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());

	if (spCallback != NULL)
	{
		mViewerGroup->addChild(textCamera);
		textCamera->addChild(textGeode);
	}
	
}

void cOSG::InitCameraConfig(void)
{
    // Local Variable to hold window size data
    CRect rect;

    // create the viewer for this window
    mViewer = new osgViewer::Viewer();

    // Add a Stats Handler to the viewer
    mViewer->addEventHandler(new osgViewer::StatsHandler);

    // Get the current window size
    ::GetClientRect(m_hWnd, &rect);
	POINT pt;
	pt.x = 0;
	pt.y = 0;
	::ClientToScreen(m_hWnd, &pt);
    // Init the GraphicsContext Traits

    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    // Init the Windata Variable that holds the handle for the Window to display OSG in.
    osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData(m_hWnd);

    // Setup the traits parameters
    traits->x = 0;
    traits->y = 0;
    traits->width = rect.right - rect.left;
    traits->height = rect.bottom - rect.top;

    traits->windowDecoration = false;
    traits->doubleBuffer = true;
    traits->sharedContext = 0;
    traits->setInheritedWindowPixelFormat = true;
    traits->inheritedWindowData = windata;

    // create the Graphics Context
    osg::GraphicsContext* gc = osg::GraphicsContext::createGraphicsContext(traits.get());

    // Init Master Camera for this View
    osg::ref_ptr<osg::Camera> camera = mViewer->getCamera();

    // Assign Graphics Context to the Camera
    camera->setGraphicsContext(gc);

    // Set the viewport for the Camera
    camera->setViewport(new osg::Viewport(rect.left, rect.top, traits->width, traits->height));

    // Set projection matrix and camera attribtues
    camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    /*camera->setClearColor(osg::Vec4f(0.2f, 0.2f, 0.4f, 1.0f));*/
	camera->setClearColor(osg::Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
    camera->setProjectionMatrixAsPerspective(
        30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0, 1000.0);

    // Add the Camera to the Viewer
    //mViewer->addSlave(camera.get());
    mViewer->setCamera(camera.get());

    // Add the Camera Manipulator to the Viewer
    mViewer->setCameraManipulator(keyswitchManipulator.get());
	createCompass(mViewer, mViewerGroup, rect.right - 1.5 * rect.Height() / 8, rect.bottom - 1.2 * rect.Height() / 8 ,rect.Height() / 8, rect.Height() / 8);
    // Set the Scene Data
	mViewer->setSceneData(mRoot.get());
	
	InitViewerEvent();
	mViewer->addEventHandler(new osgGA::StateSetManipulator(mViewer->getCamera()->getOrCreateStateSet()));
	/*mViewer->addEventHandler(new osgViewer::WindowSizeHandler); */ 
	mViewer->addEventHandler(new osgViewer::HelpHandler);
	//mViewer->addEventHandler(new osgViewer::ThreadingHandler);
	//mViewer->addEventHandler(new osgViewer::RecordCameraPathHandler);
	//mViewer->addEventHandler(new osgViewer::LODScaleHandler);
	//mViewer->addEventHandler(new osgViewer::ScreenCaptureHandler);

    // Realize the Viewer
    mViewer->realize();

	mViewer->setKeyEventSetsDone(0);
    // Correct aspect ratio
    /*double fovy,aspectRatio,z1,z2;
    mViewer->getCamera()->getProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);
    aspectRatio=double(traits->width)/double(traits->height);
    mViewer->getCamera()->setProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);*/
}

void cOSG::InitViewerEvent()
{
	InterfaceOsgDrawEventFactory iOsgDrawEventFactory;
	iOsgDrawEvent = iOsgDrawEventFactory.create(mViewer, mRoot, mEditSceneGroup, mTempGroup, mMapperGroup);
	iOsgDrawEvent->addEventToViewer(mViewer);

	InterfaceOsgMeasureEventFactory iOsgMeasureEventFactory;
	iOsgMeasureEvent = iOsgMeasureEventFactory.create(mViewer, mRoot, iSigEventCallBack);
	iOsgMeasureEvent->addEventToViewer(mViewer);

	InterfaceOsgOrientEventFactory iOsgOrientEventFactory;
	iOsgOrientEvent = iOsgOrientEventFactory.create(mViewer, mRoot, mEditSceneGroup, iSigEventCallBack);
	iOsgOrientEvent->addEventToViewer(mViewer);

	InterfaceOsgEditEventFactory iOsgEditEventFactory;
	iOsgEditEvent = iOsgEditEventFactory.create(mViewer, mRoot, mEditSceneGroup, mTempGroup, mScopeGroup,mTempGroup1, iSigEventCallBack);
	iOsgEditEvent->addEventToViewer(mViewer);

	InterfaceOsgMapperEventFactory iOsgMapperEventFactory;
	iOsgMapperEvent = iOsgMapperEventFactory.create(mViewer, mRoot,  mTempGroup1,mMapperGroup, mTempGroup,mScopeGroup,mSkyBoxGroup, iSigEventCallBack);
	iOsgMapperEvent->addEventToViewer(mViewer);

	InterfaceOsgMonomerEventFactory iOsgMonomerEventFactory;
	iOsgMonomerEvent = iOsgMonomerEventFactory.create(mViewer, mRoot, mEditSceneGroup, mTempGroup, iSigEventCallBack);
	iOsgMonomerEvent->addEventToViewer(mViewer);

	mViewer->addEventHandler(new COsgShowCoordEvent(mViewer, mRoot, mViewerGroup, iSigEventCallBack));
	
}

void cOSG::PreFrameUpdate()
{
    // Due any preframe updates in this routine
}

void cOSG::PostFrameUpdate()
{
    // Due any postframe updates in this routine
}

void cOSG::Render(void* ptr)
{
    cOSG* osg = (cOSG*)ptr;
    osgViewer::Viewer* viewer = osg->getViewer();

    // You have two options for the main viewer loop
    //      viewer->run()   or
    //      while(!viewer->done()) { viewer->frame(); }

    viewer->run();
     //while(!viewer->done())
     //{
     //    osg->PreFrameUpdate();
     //    viewer->frame();
     //    osg->PostFrameUpdate();
     //   /* Sleep(1000);*/         // Use this command if you need to allow other processes to have cpu time
     //}

    // For some reason this has to be here to avoid issue:
    // if you have multiple OSG windows up
    // and you exit one then all stop rendering
    //AfxMessageBox("Exit Rendering Thread");

    _endthread();
}

void cOSG::AddChild(osg::ref_ptr<osg::Node> parent, osg::ref_ptr<osg::Node> node)
{
	osg::ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());

	if (spChildCallback != NULL)
	{
		spChildCallback->AddChild(parent, node);
	}
}

void cOSG::RemoveChild(osg::ref_ptr<osg::Node> parent, osg::ref_ptr<osg::Node> node)
{
	osg::ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());

	if (spChildCallback != NULL)
	{
		spChildCallback->RemoveChild(parent, node);
	}
}

void cOSG::RemoveAllChild()
{
	int numChild = mRoot->getNumChildren();

	if (numChild == 0)
	{
		return;
	}

	for (int i = 0; i < numChild; i++)
	{
		osg::ref_ptr<osg::Group> group = mRoot->getChild(i)->asGroup();

		if (group)
		{	
			if (group->getName() == "viewerGroup")
			{
				continue;
			}

			osg::ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());

			if (spChildCallback != NULL)
			{
				int numGroupChild = group->getNumChildren();

				for (int j = 0; j < numGroupChild; j++)
				{
					osg::ref_ptr<osg::Node> childNode = group->getChild(j);
					spChildCallback->RemoveChild(group, childNode);
				}
			}
		}
	}	
}

void cOSG::ChangeView(int num)
{
	switch(num)
	{
		case 0:
		{
			FrontView();
			break;
		}
		case 1:
		{
			LookUp();
			break;
		}
		case 2:
		{
			LookDown();
			break;
		}
		case 3:
		{
			RearView();
			break;
		}
		case 4:
		{
			LeftView();
			break;
		}		
		case 5:
		{
			RightView();
			break;
		}
	}
}

void cOSG::LookDown()
{
	osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0);

	if (!sceneNode)
	{
		return;
	}

	osg::ComputeBoundsVisitor cbbv;
	sceneNode->accept(cbbv);
	osg::BoundingBox bb = cbbv.getBoundingBox();

	osg::Vec3f bp = bb.center(); 
	double radius = bb.radius();

	osg::Vec3d start(bp.x(), bp.y(), bp.z() - radius);
	osg::Vec3d end(bp.x(), bp.y(), bp.z() + radius);
	osg::Vec3d worldPt(0, 0, 0);
	bool isSuccess = LinePickUpPoint(start, end, worldPt, sceneNode);

	if (!isSuccess)
	{
		InterfaceMFCExcuteFactory iMFCExcuteFactory;
		InterfaceMFCExcute* iMFCExcute = iMFCExcuteFactory.create();
		iMFCExcute->PopUpMessage("场景正中间存在空洞，所以无法切换视角");
		return;
	}

	// 创建矩阵，指定到视点的距离。
	osg::Matrix trans;
	trans.makeTranslate(worldPt.x(), worldPt.y(), worldPt.z() + radius * 2);
	// 旋转一定角度（弧度值）。
	double angle(0);
	// 创建旋转矩阵。
	osg::Matrix rot;
	rot.makeRotate(osg::DegreesToRadians(angle), osg::Vec3(1., 0., 0.));

	// 设置视口矩阵（旋转矩阵和平移矩阵连乘）。
	mViewer->getCameraManipulator()->setByMatrix(rot * trans);
};

void cOSG::LookUp()
{
	osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0);

	if (!sceneNode)
	{
		return;
	}

	osg::ComputeBoundsVisitor cbbv;
	sceneNode->accept(cbbv);
	osg::BoundingBox bb = cbbv.getBoundingBox();

	osg::Vec3f bp = bb.center(); 
	double radius = bb.radius();

	osg::Vec3d start(bp.x(), bp.y(), bp.z() - radius);
	osg::Vec3d end(bp.x(), bp.y(), bp.z() + radius);
	osg::Vec3d worldPt(0, 0, 0);
	bool isSuccess = LinePickUpPoint(start, end, worldPt, sceneNode);

	if (!isSuccess)
	{
		InterfaceMFCExcuteFactory iMFCExcuteFactory;
		InterfaceMFCExcute* iMFCExcute = iMFCExcuteFactory.create();
		iMFCExcute->PopUpMessage("场景正中间存在空洞，所以无法切换视角");
		return;
	}

	// 创建矩阵，指定到视点的距离。
	osg::Matrix trans;
	trans.makeTranslate(worldPt.x(), worldPt.y(), worldPt.z() - radius * 2);
	// 旋转一定角度（弧度值）。

	double angle(180);
	// 创建旋转矩阵。
	osg::Matrix rot;
	rot.makeRotate(osg::DegreesToRadians(angle), osg::Vec3(1., 0., 0.));

	// 设置视口矩阵（旋转矩阵和平移矩阵连乘）。
	mViewer->getCameraManipulator()->setByMatrix(rot * trans);
}

void cOSG::FrontView()
{
	osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0);

	if (!sceneNode)
	{
		return;
	}

	mViewer->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KEY_Space);

	//osg::ref_ptr<osgGA::TrackballManipulator> trackball = new osgGA::TrackballManipulator();

	//// create a Manipulator Switcher
	//osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;

	//// Add our trackball manipulator to the switcher
	//keyswitchManipulator->addMatrixManipulator('1', "Trackball", trackball.get());

	//// Init the switcher to the first manipulator (in this case the only manipulator)
	//keyswitchManipulator->selectMatrixManipulator(0);  // Zero based index Value

	//mViewer->setCameraManipulator(keyswitchManipulator);
}

void cOSG::RearView()
{
	osg::ref_ptr<osg::Node> node = mRoot->getChild(0);

	if (!node)
	{
		return;
	}

	osg::ComputeBoundsVisitor cbbv;
	node->accept(cbbv);
	osg::BoundingBox bb = cbbv.getBoundingBox();

	osg::Vec3f bp = bb.center(); 
	double radius = bb.radius();

	osg::Vec3d start(bp.x(), bp.y(), bp.z() - radius);
	osg::Vec3d end(bp.x(), bp.y(), bp.z() + radius);
	osg::Vec3d worldPt(0, 0, 0);
	bool isSuccess = LinePickUpPoint(start, end, worldPt, node);

	if (!isSuccess)
	{
		InterfaceMFCExcuteFactory iMFCExcuteFactory;
		InterfaceMFCExcute* iMFCExcute = iMFCExcuteFactory.create();
		iMFCExcute->PopUpMessage("场景正中间存在空洞，所以无法切换视角");
		return;
	}

	// 创建矩阵，指定到视点的距离。
	osg::Matrix trans;
	trans.makeTranslate(worldPt.x(), worldPt.y() + radius * 2, worldPt.z());
	// 旋转一定角度（弧度值）。
	double angle(90);
	// 创建旋转矩阵。
	osg::Matrix rot;
	rot.makeRotate(osg::DegreesToRadians(angle), osg::Vec3(1., 0., 0.));
	// 创建旋转矩阵。
	double angle1(180);
	osg::Matrix rot1;
	rot1.makeRotate(osg::DegreesToRadians(angle1), osg::Vec3(0, 0., 1.));

	// 设置视口矩阵（旋转矩阵和平移矩阵连乘）。
	mViewer->getCameraManipulator()->setByMatrix(rot * rot1 * trans);
}

void cOSG::LeftView()
{
	osg::ref_ptr<osg::Node> node = mRoot->getChild(0);

	if (!node)
	{
		return;
	}

	osg::ComputeBoundsVisitor cbbv;
	node->accept(cbbv);
	osg::BoundingBox bb = cbbv.getBoundingBox();

	osg::Vec3f bp = bb.center(); 
	double radius = bb.radius();

	osg::Vec3d start(bp.x(), bp.y(), bp.z() - radius);
	osg::Vec3d end(bp.x(), bp.y(), bp.z() + radius);
	osg::Vec3d worldPt(0, 0, 0);
	bool isSuccess = LinePickUpPoint(start, end, worldPt, node);

	if (!isSuccess)
	{
		InterfaceMFCExcuteFactory iMFCExcuteFactory;
		InterfaceMFCExcute* iMFCExcute = iMFCExcuteFactory.create();
		iMFCExcute->PopUpMessage("场景正中间存在空洞，所以无法切换视角");
		return;
	}

	// 创建矩阵，指定到视点的距离。
	osg::Matrix trans;
	trans.makeTranslate(worldPt.x() - radius * 2, worldPt.y(), worldPt.z());
	// 旋转一定角度（弧度值）。
	double angle(90);
	// 创建旋转矩阵。
	osg::Matrix rot;
	rot.makeRotate(osg::DegreesToRadians(angle), osg::Vec3(0., 0., -1.));
	// 创建旋转矩阵。
	osg::Matrix rot1;
	rot1.makeRotate(osg::DegreesToRadians(angle), osg::Vec3(0., -1., 0.));
	
	// 设置视口矩阵（旋转矩阵和平移矩阵连乘）。
	mViewer->getCameraManipulator()->setByMatrix(rot * rot1 * trans);
}

void cOSG::RightView()
{
	osg::ref_ptr<osg::Node> node = mRoot->getChild(0);

	if (!node)
	{
		return;
	}

	osg::ComputeBoundsVisitor cbbv;
	node->accept(cbbv);
	osg::BoundingBox bb = cbbv.getBoundingBox();

	osg::Vec3f bp = bb.center(); 
	double radius = bb.radius();

	osg::Vec3d start(bp.x(), bp.y(), bp.z() - radius);
	osg::Vec3d end(bp.x(), bp.y(), bp.z() + radius);
	osg::Vec3d worldPt(0, 0, 0);
	bool isSuccess = LinePickUpPoint(start, end, worldPt, node);

	if (!isSuccess)
	{
		InterfaceMFCExcuteFactory iMFCExcuteFactory;
		InterfaceMFCExcute* iMFCExcute = iMFCExcuteFactory.create();
		iMFCExcute->PopUpMessage("场景正中间存在空洞，所以无法切换视角");
		return;
	}

	// 创建矩阵，指定到视点的距离。
	osg::Matrix trans;
	trans.makeTranslate(worldPt.x() + radius * 2, worldPt.y(), worldPt.z());
	// 旋转一定角度（弧度值）。
	double angle(90);
	// 创建旋转矩阵。
	osg::Matrix rot;
	rot.makeRotate(osg::DegreesToRadians(angle), osg::Vec3(1., 0., 0.));
	// 创建旋转矩阵
	osg::Matrix rot1;
	rot1.makeRotate(osg::DegreesToRadians(angle), osg::Vec3(0., 0., 1.));

	// 设置视口矩阵（旋转矩阵和平移矩阵连乘）。
	mViewer->getCameraManipulator()->setByMatrix(rot * rot1 * trans);
}

bool cOSG::LinePickUpPoint(osg::Vec3d start, osg::Vec3d end, osg::Vec3d &worldPt, osg::ref_ptr<osg::Node> pickUpNode)
{
	osg::ref_ptr<osgUtil::LineSegmentIntersector> ls = new osgUtil::LineSegmentIntersector(start, end);
	// 创建一个IV
	osgUtil::IntersectionVisitor iv(ls);
	// 把线段添加到IV中
	pickUpNode->accept(iv);

	if (ls.valid() && ls->containsIntersections())
	{
		worldPt = ls->getFirstIntersection().getWorldIntersectPoint();
		return true;
	}
	else
	{
		return false;
	}
}

void cOSG::ShowFillTriangle()
{
	// TODO: 在此添加命令处理程序代码
	if(isShowFillWireTriangle == FALSE)				//显示带网格纹理的模型
	{
		osg::ref_ptr<osg::Group> oriSceneGroup = mRoot->getChild(0)->asGroup();

		if (!oriSceneGroup)
		{
			return;
		}

		osg::ref_ptr<osg::Node> oriSceneNode = oriSceneGroup->getChild(0);

		if (oriSceneNode)
		{
			//创建带有wireframe的节点
			osg::Group* rootnode = new osg::Group;
			osg::Group* decorator = new osg::Group;
			rootnode->addChild(oriSceneNode);
			rootnode->addChild(decorator);
			decorator->addChild(oriSceneNode);  
			osg::StateSet* stateset = new osg::StateSet;
			osg::PolygonOffset* polyoffset = new osg::PolygonOffset;
			polyoffset->setFactor(-1.0f);
			polyoffset->setUnits(-1.0f);
			osg::PolygonMode* polymode = new osg::PolygonMode;
			polymode->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
			stateset->setAttributeAndModes(polyoffset,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
			stateset->setAttributeAndModes(polymode,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
			stateset->setTextureMode(0,GL_TEXTURE_2D,osg::StateAttribute::OVERRIDE|osg::StateAttribute::OFF);
			decorator->setStateSet(stateset);

			osg::ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());

			if (spChildCallback != NULL)
			{
				spChildCallback->RemoveChild(oriSceneGroup, oriSceneNode);
				spChildCallback->AddChild(oriSceneGroup, rootnode);
				isShowFillWireTriangle = TRUE;
			}
		}
	}
	else											//显示原来模型
	{
		osg::ref_ptr<osg::Group> oriSceneGroup = mRoot->getChild(0)->asGroup();

		if (!oriSceneGroup)
		{
			return;
		}

		osg::ref_ptr<osg::Group> wiredSceneGroup = oriSceneGroup->getChild(0)->asGroup();

		if (wiredSceneGroup)
		{
			osg::ref_ptr<osg::Node> oriSceneNode = wiredSceneGroup->getChild(0);

			if (oriSceneNode)
			{
				osg::ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());

				if (spChildCallback != NULL)
				{
					spChildCallback->RemoveChild(oriSceneGroup, wiredSceneGroup);
					spChildCallback->AddChild(oriSceneGroup, oriSceneNode);
					isShowFillWireTriangle = FALSE;
				}
			}
		}
	}
}

void cOSG::SetSceneExtent()
{
	//设置范围
	osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator = dynamic_cast<osgGA::KeySwitchMatrixManipulator*> (mViewer->getCameraManipulator());
	osg::ref_ptr<osgGA::TerrainManipulator> terrainManipulator = dynamic_cast<osgGA::TerrainManipulator*> (keyswitchManipulator->getMatrixManipulatorWithIndex(1));
	terrainManipulator->setNode(mRoot);
}

osgViewer::Viewer* cOSG::getViewer() 
{ 
	return mViewer; 
}

void cOSG::AddViewerOPS(std::vector<OperatingState> vecOps)
{
	for(osgViewer::View::EventHandlers::iterator hitr = mViewer->getEventHandlers().begin(); hitr != mViewer->getEventHandlers().end(); ++hitr)
	{
		osgGA::EventHandler* pEvent = *hitr;

		osg::ref_ptr<CBaseUserEvent> ph = dynamic_cast<CBaseUserEvent*> (pEvent);

		if (!ph)
		{
			continue;
		}

		ph->UpdateActivatedStatus(vecOps);
	}
}

HANDLE cOSG::StartRendering()
{
	HANDLE mThreadHandle = (HANDLE)_beginthread(&Render, 0, this);
	return mThreadHandle;
}

 InterfaceSigEventCallBack* cOSG::ObtainISig()
 {
	 return iSigEventCallBack;
 }

InterfaceOsgOrientEvent* cOSG::ObtainOrientEventSlot()
 {
	 return iOsgOrientEvent;
 }

InterfaceOsgEditEvent* cOSG::ObtainEditEventSlot()
 {
	 return iOsgEditEvent;
 }

 InterfaceOsgMapperEvent* cOSG::ObtainMapperEventSlot()
 {
	 return iOsgMapperEvent;
 }

 InterfaceOsgMonomerEvent* cOSG::ObtainMonomerEventSlot()
 {
	 return iOsgMonomerEvent;
 }

 void cOSG::simulateMouseButtonEvent()
 {
	 mViewer->getEventQueue()->mouseButtonPress(100, 100, 3);
	 mViewer->getEventQueue()->mouseButtonRelease(100, 100, 3);
 }


CRenderingThread::CRenderingThread( cOSG* ptr )
:   OpenThreads::Thread(), _ptr(ptr), _done(false)
{
}

CRenderingThread::~CRenderingThread()
{
    _done = true;
    if (isRunning())
    {
        cancel();
        join();
    }
}

void CRenderingThread::run()
{
    if ( !_ptr )
    {
        _done = true;
        return;
    }

    osgViewer::Viewer* viewer = _ptr->getViewer();
    do
    {
        _ptr->PreFrameUpdate();
        viewer->frame();
        _ptr->PostFrameUpdate();
    } while ( !testCancel() && !viewer->done() && !_done );
}

