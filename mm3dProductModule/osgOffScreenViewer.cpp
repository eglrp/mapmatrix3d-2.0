#include "StdAfx.h"
#include "osg/GraphicsContext"
#include "osgOffScreenViewer.h"
#include <sstream>



void osgOffScreenViewer::realize()
{
	//OSG_INFO<<"Viewer::realize()"<<std::endl;

	Contexts contexts;
	getContexts(contexts);

	if (contexts.empty())
	{
		OSG_INFO<<"Viewer::realize() - No valid contexts found, setting up view across all screens."<<std::endl;

		// no windows are already set up so set up a default view

		const char* ptr = 0;
		if ((ptr = getenv("OSG_CONFIG_FILE")) != 0)
		{
			readConfiguration(ptr);
		}
		else
		{
			int screenNum = -1;
			if ((ptr = getenv("OSG_SCREEN")) != 0)
			{
				if (strlen(ptr)!=0) screenNum = atoi(ptr);
				else screenNum = -1;
			}

			int x = -1, y = -1, width = -1, height = -1;
			if ((ptr = getenv("OSG_WINDOW")) != 0)
			{
				std::istringstream iss(ptr);
				iss >> x >> y >> width >> height;
			}

			if (width>0 && height>0)
			{
				if (screenNum>=0) setUpViewInWindow(x, y, width, height, screenNum);
				else setUpViewInWindow(x,y,width,height);
			}
			else if (screenNum>=0)
			{
				setUpViewOnSingleScreen(screenNum);
			}
			else
			{
				setUpViewAcrossAllScreens();
			}
		}

		getContexts(contexts);
	}

	if (contexts.empty())
	{
		OSG_NOTICE<<"Viewer::realize() - failed to set up any windows"<<std::endl;
		_done = true;
		return;
	}

	// get the display settings that will be active for this viewer
	osg::DisplaySettings* ds = _displaySettings.valid() ? _displaySettings.get() : osg::DisplaySettings::instance().get();
	osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();

	// pass on the display settings to the WindowSystemInterface.
	if (wsi && wsi->getDisplaySettings()==0) wsi->setDisplaySettings(ds);

	unsigned int maxTexturePoolSize = ds->getMaxTexturePoolSize();
	unsigned int maxBufferObjectPoolSize = ds->getMaxBufferObjectPoolSize();

	for(Contexts::iterator citr = contexts.begin();
		citr != contexts.end();
		++citr)
	{
		osg::GraphicsContext* gc = *citr;

		if (ds->getSyncSwapBuffers()) gc->setSwapCallback(new osg::SyncSwapBuffersCallback);

		// set the pool sizes, 0 the default will result in no GL object pools.
		gc->getState()->setMaxTexturePoolSize(maxTexturePoolSize);
		gc->getState()->setMaxBufferObjectPoolSize(maxBufferObjectPoolSize);
		gc->SetOffScreen(true);
		gc->realize();

		if (_realizeOperation.valid() && gc->valid())
		{
			gc->makeCurrent();

			(*_realizeOperation)(gc);

			gc->releaseContext();
		}

	}

	// attach contexts to _incrementalCompileOperation if attached.
	if (_incrementalCompileOperation) _incrementalCompileOperation->assignContexts(contexts);

	bool grabFocus = false;
	if (grabFocus)
	{
		for(Contexts::iterator citr = contexts.begin();
			citr != contexts.end();
			++citr)
		{
			osgViewer::GraphicsWindow* gw = dynamic_cast<osgViewer::GraphicsWindow*>(*citr);
			if (gw)
			{
				gw->grabFocusIfPointerInWindow();
			}
		}
	}

	// initialize the global timer to be relative to the current time.
	osg::Timer::instance()->setStartTick();

	// pass on the start tick to all the associated event queues
	setStartTick(osg::Timer::instance()->getStartTick());

	// configure threading.
	setUpThreading();

	if (osg::DisplaySettings::instance()->getCompileContextsHint())
	{
		int numProcessors = OpenThreads::GetNumberOfProcessors();
		int processNum = 0;

		for(unsigned int i=0; i<= osg::GraphicsContext::getMaxContextID(); ++i)
		{
			osg::GraphicsContext* gc = osg::GraphicsContext::getOrCreateCompileContext(i);

			if (gc)
			{
				gc->createGraphicsThread();
				gc->getGraphicsThread()->setProcessorAffinity(processNum % numProcessors);
				gc->getGraphicsThread()->startThread();

				++processNum;
			}
		}
	}
#if 0
	osgGA::GUIEventAdapter* eventState = getEventQueue()->getCurrentEventState();
	if (getCamera()->getViewport())
	{
		osg::Viewport* viewport = getCamera()->getViewport();
		eventState->setInputRange( viewport->x(), viewport->y(), viewport->x() + viewport->width(), viewport->y() + viewport->height());
	}
	else
	{
		eventState->setInputRange(-1.0, -1.0, 1.0, 1.0);
	}
#endif
}