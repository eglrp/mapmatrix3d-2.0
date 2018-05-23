#pragma once
#include "osgAbsOrientEvent.h"
#include "osgSceneMatchEvent.h"
#include "osgInternetOrientEvent.h"
#include "osgManualOrientEvent.h"
#include "osgClipByPolygonEvent.h"
#include "osgHollowOutByPolygonEvent.h"
#include "osgHollowOutSuspendEvent.h"
#include "osgSelTileRemoveEvent.h"
#include "osgClipByHeightEvent.h"
#include "osgRepairHoleEvent.h"
#include "osgDrawFlatPlaneEvent.h"
#include "InterfaceViewerSlot.h"
#include "osgHollowOutSuspendEvent.h"
#include "osgExtractHatchingEvent.h"
#include "osgExtractOutlineEvent.h"
#include "osgExtractAccuraOutlineEvent.h"
#include "osgExtractHandPaintOutLineEvent.h"
#include "osgExtractHandPaintLineEvent.h"
#include "InterfaceMFCExcute.h"
#include "toShpAndDxf.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceOsgFinder.h"
#include "osgExtractTriangleBaseOutline.h"
#include "osgManualExtractTextureEvent.h"
#include "osgWholeSceneMatchEvent.h"
#include "osgExtractObjectByPolygonEvent.h"
#include "osgQuickClipModelByTriangleEvent.h"
#include "osgQuickDeleteTriangleBySolidEvent.h"
#include "osgLinkPropertyWithObjectEvent.h"
#include "osgExtractPtCloudEvent.h"
#include "osgExtractPtCloudEdgeEvent.h"
#include "osgHighLightObjectByClickEvent.h"
#include "osgSimplifyObjectByPolygonEvent.h"
#include "osgFlatSceneByPolygonEvent.h"

class COrientSlot : public InterfaceOrientSlot
{
public:			
	COrientSlot(COsgAbsOrientEvent* OsgAbsOriEvent, COsgSceneMatchEvent* OsgSceneMatchEvent, COsgInternetOrientEvent* OsgInternetOrientEvent, COsgManualOrientEvent* OsgManualOrientEvent, COsgWholeSceneMatchEvent* OsgWholeSceneMatchEvent)
	{
		osgAbsOriEvent = OsgAbsOriEvent;
		osgSceneMatchEvent = OsgSceneMatchEvent;
		osgInternetOrientEvent = OsgInternetOrientEvent;
		osgManualOrientEvent = OsgManualOrientEvent;
		osgWholeSceneMatchEvent = OsgWholeSceneMatchEvent;
	}

	BOOL AbsOrient(std::string inFilePath)															//绝对定向
	{
		return(osgAbsOriEvent->AbsOrient(inFilePath));
	};

	void ReClickCtrlPt(int ptNum)																	//重新刺点
	{
		osgAbsOriEvent->ReclickCtrlPt(ptNum);
	}

	void LoadCtrlPt()																				//加载控制点
	{
		osgAbsOriEvent->LoadControlPt();
	};

	void MatchSingleModel()																			//匹配单个模型
	{
		osgSceneMatchEvent->MatchSingleModel();
	};

	void InternetAbsOrient()																		//互联网绝对定向
	{
		osgInternetOrientEvent->InternetAbsOrient();	
	}

	void ManualHorizon()
	{
		osgManualOrientEvent->StartManualHorizon();
	}

	void ResetControlPtStatus(int ptNum, bool isCheckPt)											//重新设置控制点状态
	{
		osgAbsOriEvent->ReSetCtrlPtCheckStatus(ptNum, isCheckPt);
	}

	void MatchWholeModel()
	{
		osgWholeSceneMatchEvent->StartWholeSceneMatch();
	}

protected:
	COsgAbsOrientEvent* osgAbsOriEvent;

	COsgSceneMatchEvent* osgSceneMatchEvent;

	COsgInternetOrientEvent* osgInternetOrientEvent;

	COsgManualOrientEvent* osgManualOrientEvent;

	COsgWholeSceneMatchEvent* osgWholeSceneMatchEvent;

};

class CEditSlot : public InterfaceEditSlot
{
public:
	CEditSlot(COsgClipByPolygonEvent* OsgClipByPolygonEvent, COsgHollowOutByPolygonEvent* OsgHollowOutByPolygonEvent, COsgClipByHeightEvent* OsgClipByHeightEvent, COsgSelTileRemoveEvent* OsgSelTileRemoveEvent, COsgRepairHoleEvent* OsgRepairHoleEvent, COsgDrawFlatPlaneEvent* OsgDrawFlatPlaneEvent, COsgHollowOutSuspendEvent* OsgHollowOutSuspendEvent, COsgFlatSceneByPolygonEvent* OsgFlatSceneByPolygonEvent)
	{
		osgClipByPolygonEvent = OsgClipByPolygonEvent;
		osgHollowOutByPolygonEvent = OsgHollowOutByPolygonEvent;
		osgClipByHeightEvent = OsgClipByHeightEvent;
		osgSelTileRemoveEvent = OsgSelTileRemoveEvent;
		osgRepairHoleEvent = OsgRepairHoleEvent;
		osgDrawFlatPlaneEvent = OsgDrawFlatPlaneEvent;
		osgHollowOutSuspendEvent = OsgHollowOutSuspendEvent;
		osgFlatSceneByPolygonEvent = OsgFlatSceneByPolygonEvent;
	}

	virtual void ClipByPolyon(std::string inFilePath)
	{
		osgClipByPolygonEvent->ClipByPolygon(inFilePath);
	}

	virtual void HollowOut(std::string inFilePath)
	{
		osgHollowOutByPolygonEvent->HollowOutByPolygon(inFilePath);
	}

	virtual void RepairHole()
	{
		osgRepairHoleEvent->StartRepairHole();
	}

	virtual void ClipByHeight()
	{
		osgClipByHeightEvent->ClipHeight();
		osgDrawFlatPlaneEvent->DrawFlatPlaneByHeight();
	}

	virtual void InvertClipHeight()
	{
		osgClipByHeightEvent->InvertClipByHeight();
	}

	virtual void HollowOutSuspend(std::string inFilePath)
	{
		osgHollowOutSuspendEvent->HollowOutSuspend(inFilePath);
	}

	virtual void AbsoluteHollowOutByPolygon(std::string inFilePath)
	{
		
	}

	virtual void FlatByPolygon(std::string inFilePath)
	{
		osgFlatSceneByPolygonEvent->FlatByPolygon(inFilePath);
	}

protected:
	COsgClipByPolygonEvent* osgClipByPolygonEvent;

	COsgHollowOutByPolygonEvent* osgHollowOutByPolygonEvent;

	COsgClipByHeightEvent* osgClipByHeightEvent;

	COsgSelTileRemoveEvent* osgSelTileRemoveEvent;

	COsgRepairHoleEvent* osgRepairHoleEvent;

	COsgDrawFlatPlaneEvent* osgDrawFlatPlaneEvent;

	COsgHollowOutSuspendEvent* osgHollowOutSuspendEvent;

	COsgFlatSceneByPolygonEvent* osgFlatSceneByPolygonEvent;

};

class CMapperSlot : public InterfaceMapperSlot
{
public:
	CMapperSlot(COsgExtractHatchingEvent* OsgExtractHatchingEvent, COsgExtractOutLineEvent* OsgExtractOutLineEvent, COsgExtractTriangleBaseOutLineEvent* OsgExtractTriangleBaseOutLineEvent,
		COsgExtractHandPaintOutLineEvent* OsgExtractHandPaintOutlineEvent, COsgExtractHandPaintLineEvent* OsgExtractHandPaintLineEvent, COsgExtractPtCloudEdgeEvent* OsgExtractPtCloudEdgeEvent,
		COsgExtractPtCloudEvent* OsgExtractPtCloudEvent, osg::ref_ptr<osg::Group> mRoot, InterfaceSigEventCallBack* iSigEventCallBack)
	{
		osgExtractHatchingEvent = OsgExtractHatchingEvent;

		osgExtractOutLineEvent = OsgExtractOutLineEvent;
		osgExtractTriangleBaseOutLineEvent = OsgExtractTriangleBaseOutLineEvent;
		osgExtractHandPaintOutlineEvent = OsgExtractHandPaintOutlineEvent;
		osgExtractHandPaintLineEvent = OsgExtractHandPaintLineEvent;
		osgExtractPtCloudEdgeEvent = OsgExtractPtCloudEdgeEvent;
		osgExtractPtCloudEvent = OsgExtractPtCloudEvent;

		root = mRoot;
		iSigEventCallBack = iSigEventCallBack;
	}

	void outForDxfAndShp();

	void ProduceDom(std::string strSceneFileName, double res);

	void ProduceDsm(std::string strSceneFileName, double res);

	bool ExtractContour(double high);

	void setPtCloudParameter(double angle, double alpha,double color);

	void getPtCloudParameter(double& angle, double& alpha,double& color);
protected:
	COsgExtractHatchingEvent* osgExtractHatchingEvent;

	COsgExtractOutLineEvent* osgExtractOutLineEvent;
	
	COsgExtractTriangleBaseOutLineEvent* osgExtractTriangleBaseOutLineEvent;
	
	COsgExtractHandPaintOutLineEvent* osgExtractHandPaintOutlineEvent;
	
	COsgExtractHandPaintLineEvent* osgExtractHandPaintLineEvent;
	
	COsgExtractPtCloudEdgeEvent* osgExtractPtCloudEdgeEvent;
	
	COsgExtractPtCloudEvent* osgExtractPtCloudEvent;
	
	osg::ref_ptr<osg::Group> root;
	
	InterfaceSigEventCallBack* iSigEventCallBack;
};


class CMonomerSlot : public InterfaceMonomerSlot
{
public:
	CMonomerSlot(COsgManualExtractTextureEvent* OsgManualExtractTextureEvent, COsgExtractObjectByPolygonEvent* OsgExtractObjectByPolygonEvent, COsgQuickClipModelByTriangleEvent* OsgQuickClipModelByTriangleEvent, COsgQuickDeleteSingleTriangleBySolidEvent* OsgQuickDeleteSingleTriangleBySolidEvent, COsgHighLightObjectByClickEvent* OsgHighLightObjectByClickEvent, COsgLinkPropertyWithObjectEvent* OsgLinkPropertyWithObjectEvent, COsgSimplifyObjectByPolygonEvent* OsgSimplifyObjectByPolygonEvent)
	{
		osgManualExtractTextureEvent = OsgManualExtractTextureEvent;
		osgExtractObjectByPolygonEvent = OsgExtractObjectByPolygonEvent;
		osgQuickClipModelByTriangleEvent = OsgQuickClipModelByTriangleEvent;
		osgQuickDeleteSingleTriangleBySolidEvent = OsgQuickDeleteSingleTriangleBySolidEvent;
		osgLinkPropertyWithObjectEvent = OsgLinkPropertyWithObjectEvent;
		osgHighLightObjectByClickEvent = OsgHighLightObjectByClickEvent;
		osgSimplifyObjectByPolygonEvent = OsgSimplifyObjectByPolygonEvent;
	}

	virtual void StartExtractTexture(double res, double deltaZ)
	{
		osgManualExtractTextureEvent->StartExtractTexture(res, deltaZ);
	}

	virtual void ExtractObjectByPolygon(std::string inFilePathName)
	{
		osgExtractObjectByPolygonEvent->ExtractObject(inFilePathName);
	}

	virtual void QuickClipByPolygon()
	{
		osgQuickClipModelByTriangleEvent->StartQuickClipSceneByTriangle();
	}


	virtual void QuickClipBySolid()
	{
		osgQuickDeleteSingleTriangleBySolidEvent->StartQuickClipSceneBySolid();
	}

	virtual void SetObjectProperty(std::string strObjectProperty) 
	{
		osgLinkPropertyWithObjectEvent->SetObjectProperty(strObjectProperty);
	}

	virtual void LookUpOjbectProperty(std::string objectPropertyName, std::string objectPropertyValue)
	{
		osgLinkPropertyWithObjectEvent->LookUpOjbectProperty(objectPropertyName, objectPropertyValue);
	}

	virtual void OutputObjectModelWithoutTexture(std::string outModelWithoutFileName)
	{
		osgHighLightObjectByClickEvent->OutputModelWithoutTexture(outModelWithoutFileName);
	}

	virtual void SelectHousePt()
	{
		osgExtractObjectByPolygonEvent->SelectObjectPt();
	}

protected:
	COsgManualExtractTextureEvent* osgManualExtractTextureEvent;

	COsgExtractObjectByPolygonEvent* osgExtractObjectByPolygonEvent;

	COsgQuickClipModelByTriangleEvent* osgQuickClipModelByTriangleEvent;

	COsgQuickDeleteSingleTriangleBySolidEvent* osgQuickDeleteSingleTriangleBySolidEvent;

	COsgLinkPropertyWithObjectEvent* osgLinkPropertyWithObjectEvent;

	COsgHighLightObjectByClickEvent* osgHighLightObjectByClickEvent;

	COsgSimplifyObjectByPolygonEvent* osgSimplifyObjectByPolygonEvent;

};