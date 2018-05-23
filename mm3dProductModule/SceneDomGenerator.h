#pragma once
#include <vector>
#include <string>
#include <osgViewer/Renderer>
#include "cv_plus.hpp"
#include "coord.hpp"
#include "GDAL_PLUS.h"
#include "OSG_Plus.h"
#include "qcomm.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceOsgEditor.h"
#include "InterfaceUserEventCallBack.h"
#include "osgOffScreenViewer.h"
#include "Executor.h"
/** Capture the frame buffer and write image to disk*/
class WindowCaptureCallback : public osg::Camera::DrawCallback
{
public:
	bool isOutputImage;
public:    
	WindowCaptureCallback(GLenum readBuffer, const std::string& name, int frameNum, int Width, int Height):
	  _readBuffer(readBuffer),
		  _fileName(name)
	  {
		  _image = new osg::Image;
		  isOutputImage = false;
		  _frameNum = frameNum;
		  width = Width;
		  height = Height;
		  outputFileName = name;
	  }

	  virtual void operator () (osg::RenderInfo& renderInfo) const
	  {
		  glReadBuffer(_readBuffer);
		  osg::GraphicsContext* gc = renderInfo.getState()->getGraphicsContext();

		  if (gc->getTraits())
		  {
			  GLenum pixelFormat;

			  if (gc->getTraits()->alpha)

				  pixelFormat = GL_RGBA;
			  else 
				  pixelFormat = GL_RGB;

			  _image->readPixels(0, 0, width, height, pixelFormat, GL_UNSIGNED_BYTE); 
		  }

		  --_frameNum;

		  if (!outputFileName.empty())
		  {
			  InterfaceMFCExcuteFactory IMFCExecuteFactory;
			  InterfaceMFCExcute* IMFCExecute = IMFCExecuteFactory.create();
			  
			  bool isFileExist = IMFCExecute->IsFileExists(outputFileName);

			  if (isFileExist)
			  {
				  IMFCExecute->RemoveFile(outputFileName);
			  }

			  bool isSuccess = osgDB::writeImageFile(*_image, outputFileName);

			  if (!isSuccess)
			  {
				  char strMsg[MAX_PATH];
				  sprintf_s(strMsg, "无法输出%s\n", outputFileName.c_str());
				  _logop(strMsg);
			  }
		  }
	  }

protected:    
	GLenum                      _readBuffer;
	std::string                 _fileName;
	osg::ref_ptr<osg::Image>    _image;
	mutable OpenThreads::Mutex  _mutex;
	mutable int _frameNum;
	int width;
	int height;
	std::string outputFileName;
};



/** Do Culling only while loading PagedLODs*/
class CustomRenderer : public osgViewer::Renderer
{
public:
	CustomRenderer(osg::Camera* camera) 
		: osgViewer::Renderer(camera),
		_cullOnly(true)
	{
		/*setTargetFrameRate(1);
		setMinimumTimeAvailableForGLCompileAndDeletePerFrame(1);*/
	}

	/** Set flag to omit drawing in renderingTraversals */
	void setCullOnly(bool on) { _cullOnly = on; }

	virtual void operator () (osg::GraphicsContext* /*context*/)
	{
		if (_graphicsThreadDoesCull)
		{
			if (_cullOnly)
				cull();
			else
				cull_draw();
		}
	}

	virtual void cull()
	{
		osgUtil::SceneView* sceneView = _sceneView[0].get();
		if (!sceneView || _done ) return;

		updateSceneView(sceneView);

		osgViewer::View* view = dynamic_cast<osgViewer::View*>(_camera->getView());
		if (view) sceneView->setFusionDistance(view->getFusionDistanceMode(), view->getFusionDistanceValue());

		sceneView->inheritCullSettings(*(sceneView->getCamera()));
		sceneView->cull();
	}

	bool _cullOnly;
};

class CSceneDomGenerator
{
public:
	//接口
	virtual bool StartProducingDOM(osg::ref_ptr<osg::Node> sceneNode, std::string SceneFileName, std::string OutputFileName, double Res, int XTileNum, int YTileNum, 
		InterfaceSigEventCallBack* ISigCallBack);

	virtual bool ProduceRectDom(osg::ref_ptr<osg::MatrixTransform> mTrans, std::vector<CoordDouble3D> vecCoord, std::string outRectDomFileName);

	virtual bool produceNodeDOM(osg::ref_ptr<osg::Node> loadedModel, std::string &outRectDomFileName, osg::Matrix matrix);

	void SetResolution(double RES)
	{
		res = RES;
	}

protected:
	std::vector<std::string> vecFileName;
	std::vector<std::string> vecSplitImgFileName;
	std::vector<std::string> vecSplitCoordFileName;

	bool produceSplitedDOM(int i, int j, osg::ref_ptr<osg::MatrixTransform> mTrans, int newWidth, int newHeight, double newRectLeft,
		double newRectRight, double newRectBottom, double newRectTop, double newRectZnear, double newRectZfar, std::string outRectDomFileName);

	bool produceScreenDOM(osg::ref_ptr<osg::MatrixTransform> node, int newWidth, int newHeight, double newRectLeft, double newRectRight, double newRectBottom, 
		double newRectTop, double newRectZnear, double newRectZfar, std::string newOutFileName);

	bool produceSpecificScreenDOM(osg::ref_ptr<osg::MatrixTransform> node, int newWidth, int newHeight, double newRectLeft, double newRectRight, double newRectBottom,
		double newRectTop, double newRectZnear, double newRectZfar, osg::Matrix viewMatrix, std::string newOutFileName);

	bool produceSpecificSplitedDOM(int i, int j, osg::ref_ptr<osg::MatrixTransform> mTrans, int newWidth, int newHeight, double newRectLeft,
		double newRectRight, double newRectBottom, double newRectTop, double newRectZnear, double newRectZfar, osg::Matrix viewMatrix, std::string outFileName);

	bool StartGeneratingDOM(std::string outputDomFileName, std::string outputCoordFileName, osg::ref_ptr<osg::MatrixTransform> mTrans, bool isCombine);

	std::vector<osg::Vec4d> findImageRect(std::vector<std::string> vecCoordTxtFileName);

	std::vector<osg::Vec4d> findDOMRect(std::vector<std::string> vecCoordFileName, std::vector<std::string> vecImageFileName);

	void combineImage(CvMat* imgMat, std::string imgFileName, osg::Vec4d rect, osg::Vec4d maxRect);

	void combineImage(GDALDataset* poDataset, std::string imgFileName, osg::Vec4d rect, osg::Vec4d maxRect);

	void combineBigImage(GDALDataset* poDataset, GDALDataset* poBigImageDataset, osg::Vec4d rect, osg::Vec4d bigImageMaxRect, osg::Vec4d maxRect);

	void combineRepairImage(GDALDataset* poDataset, std::string repairImgFileName, osg::Vec4d rect, osg::Vec4d maxRect);

	osg::Vec4d findMaxRect(std::vector<osg::Vec4d> vecRect);

protected:
	bool FindTileImageRect(std::string txtFileName, osg::Vec4d &rect);

	bool FindMinMaxNode(osg::ref_ptr<osg::Node> node, osg::Matrix matrix, std::vector<CoordDouble3D> &minMaxCoord);

	bool findMinMax(std::vector<CoordDouble3D> vecMinMax, std::vector<CoordDouble3D> &coord);

	std::vector<std::vector<std::string>> SeparateSceneTile(std::vector<std::string> vecChildFilePath);

	bool GenerateDOMForSingleModel(osg::ref_ptr<osg::MatrixTransform> mTrans, std::string outputDomFileName, bool isCombine);

	bool GenerateMultiDomForSingleModel(std::vector<osg::MatrixTransform*> vecTransNode);

	bool OutputImageAndCoords(std::vector<osg::MatrixTransform*> vecTransNode, std::vector<std::string> vecSceneFilePath, std::vector<std::string> vecImageFileName, 
		std::vector<std::string> vecCoordFileName);

	void combineBigRepairImage(GDALDataset* poDataset, GDALDataset* poBigRepairImageDataset, osg::Vec4d rect, osg::Vec4d bigImageMaxRect, osg::Vec4d maxRect);

	bool StartRepairDom(osg::ref_ptr<osg::MatrixTransform> mTrans, GDALDataset* poDataset, std::string sceneFilepath, osg::Vec4d maxRect);

	void InitDomGeneratorPara(std::string SceneFileName, std::string OutputFileName, double Res, int XTileNum, int YTileNum,
		InterfaceSigEventCallBack* iSigCallBack);

protected:
	

	std::string sceneFileName;
	
	std::string outputFileName;

	double res;

	int xTileNum;

	int yTileNum;
	
	InterfaceSigEventCallBack* ISigCallBack;
};
