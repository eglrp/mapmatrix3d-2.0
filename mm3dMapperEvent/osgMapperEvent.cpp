// mm3dMapperEvent.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "osgMapperEvent.h"
#include "osgExtractRoadByDomEvent.h"
#include "osgAutoExtractWallPointEvent.h"
#include "osgCombineGroupEvent.h"
#include "osgDrawHorizLineEvent.h"
#include "osgExtractAccuraOutlineEvent.h"
#include "osgExtractHandPaintLineEvent.h"
#include "osgExtractHandPaintOutLineEvent.h"
#include "osgExtractHatchingEvent.h"
#include "osgExtractLineInsectEvent.h"
#include "osgExtractOutlineEvent.h"
#include "osgExtractPtCloudEdgeEvent.h"
#include "osgExtractPtCloudEvent.h"
#include "osgExtractTriangleBaseOutline.h"
#include "osgExtractTriangleBaseOutlineInAreaEvent.h"
#include "osgExtractWallInsectEvent.h"
#include "osgFittingWallSpaceEvent.h"
#include "osgHandExtractWallSpaceEvent.h"
#include "osgSelectLineByAreaEvent.h"
#include "osgSelectLineByClickEvent.h"
#include "osgSelectLineByRectangleEvent.h"
#include "osgSelectPtCloudByAreaEvent.h"
#include "osgExtractPtCloudinMultiTilesEvent.h"
#include "osgHandDrawEdgeEvent.h"
#include "osgHandDrawEdge_1_Event.h"
//#include "osgBreakoutLineEvent.h"
//#include "osgCrossLineEvent.h"
//#include "osgExtractFarmLandByDsmEvent.h"
#include "InterfaceMFCExcute.h"
#include "toShpAndDxf.h"
#include "osgExtractContourEvent.h"
#include "osgExtractLineBasePG.h"

void COsgMapperEvent::addEventToViewer(osg::ref_ptr<osgViewer::Viewer> viewer)
{
	for (int i = 0; i < vecMapperEvent.size(); i++)
	{
		viewer->addEventHandler(vecMapperEvent[i]);
	}
}

COsgMapperEvent::COsgMapperEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> editGroup, osg::ref_ptr<osg::Group> mapperGroup, osg::ref_ptr<osg::Group> TempGroup, 
	osg::ref_ptr<osg::Group> linkGroup, osg::ref_ptr<osg::Group> DataGroup, InterfaceSigEventCallBack* iSigEventCallBack)
{
	CBaseUserEvent* ExtractHatching = new COsgExtractHatchingEvent(viewer, root, mapperGroup,TempGroup, iSigEventCallBack);
	CBaseUserEvent* ExtractOutLine = new COsgExtractOutLineEvent(viewer, root, mapperGroup, iSigEventCallBack);
	CBaseUserEvent* ExtractAccuraOutLine = new COsgExtractAccuraOutLineEvent(viewer, root, mapperGroup, TempGroup, iSigEventCallBack);
	CBaseUserEvent* ExtractHandPaintOutline = new COsgExtractHandPaintOutLineEvent(viewer, root, mapperGroup, iSigEventCallBack);
	CBaseUserEvent* AutoExtractWallSpaceEvent = new COsgAutoExtractWallPointEvent(viewer, root, editGroup, linkGroup, TempGroup, iSigEventCallBack);
	CBaseUserEvent* ExtractHandPaintLine = new COsgExtractHandPaintLineEvent(viewer, root, mapperGroup, TempGroup, iSigEventCallBack);
	CBaseUserEvent* ExtractTriBaseOutline = new COsgExtractTriangleBaseOutLineEvent(viewer, root, mapperGroup,TempGroup, iSigEventCallBack);
	CBaseUserEvent* ExtractTriBaseOutLineInArea = new COsgExtractTriangleBaseOutLineInAreaEvent(viewer, root, mapperGroup, TempGroup, iSigEventCallBack);
	CBaseUserEvent* SelectLineByRectangle = new COsgSelectLineByRectangleEvent(viewer, root, mapperGroup, iSigEventCallBack);
	CBaseUserEvent* SelectLineByArea = new COsgSelectLineByAreaEvent(viewer, root, mapperGroup, iSigEventCallBack);
	CBaseUserEvent* SelectLineByClick = new COsgSelectLineByClickEvent(viewer, root, mapperGroup, iSigEventCallBack);
	CBaseUserEvent* ExtractPtCloudEdge = new COsgExtractPtCloudEdgeEvent(viewer, root, mapperGroup, TempGroup, iSigEventCallBack);
	CBaseUserEvent* ExtractPtCloud = new COsgExtractPtCloudEvent(viewer, root, TempGroup, iSigEventCallBack);
	CBaseUserEvent* SelectPtCloudByArea = new COsgSelectPtCloudByAreaEvent(viewer, root, mapperGroup, iSigEventCallBack);
	CBaseUserEvent* FittingWallSpace = new COsgFittingWallSpaceEvent(viewer, root, editGroup, linkGroup, TempGroup, iSigEventCallBack);
	CBaseUserEvent* ExtractWallInsect = new COsgExtractWallInsectEvent(viewer, root, editGroup, mapperGroup,  TempGroup,DataGroup, iSigEventCallBack);
	CBaseUserEvent* DrawHorizLine = new COsgDrawHorizLineEvent(viewer, root, mapperGroup, TempGroup, iSigEventCallBack);
	CBaseUserEvent* CombineGroup = new COsgCombineGroupEvent(viewer, root, mapperGroup, iSigEventCallBack);
	CBaseUserEvent* HandExtractWallSpace = new COsgHandExtractWallSpaceEvent(viewer, root, TempGroup, editGroup, linkGroup, iSigEventCallBack);
	CBaseUserEvent* ExtractLineInsect = new COsgExtractLineInsectEvent(viewer, root, mapperGroup,  TempGroup,  iSigEventCallBack);
	CBaseUserEvent* ExtractPtCloudinMultiTiles = new COsgExtractPtCloudinMultiTilesEvent(viewer, root, mapperGroup, TempGroup,  iSigEventCallBack);
	CBaseUserEvent* HandDrawEdge = new COsgHandDrawEdgeEvent(viewer, root, mapperGroup, TempGroup,  DataGroup,iSigEventCallBack);
	CBaseUserEvent* HandDrawEdge_1 = new COsgHandDrawEdge_1_Event(viewer, root, mapperGroup,  TempGroup, DataGroup,  iSigEventCallBack);
	//CBaseUserEvent* ExtractRoadByDom = new COsgExtractRoadByDomEvent(viewer, root, mapperGroup, iSigEventCallBack);
	//CBaseUserEvent* BreakoutLine = new COsgBreakoutLineEvent(viewer, root, mapperGroup, TempGroup, iSigEventCallBack);
	//CBaseUserEvent* CrossLine = new COsgCrossLineEvent(viewer, root, mapperGroup, TempGroup, iSigEventCallBack);
	//CBaseUserEvent* ExtractFarmLandByDsm = new COsgExtractFarmLandByDsmEvent(viewer, root, mapperGroup, iSigEventCallBack);
	CBaseUserEvent* ExtractContour = new COsgExtractContourEvent(viewer,root,mapperGroup,iSigEventCallBack);
	CBaseUserEvent* ExtractLineBasePG = new COsgExtractLineBasePGEvent(viewer, root, mapperGroup, iSigEventCallBack);

	vecMapperEvent.push_back(ExtractHatching);
	vecMapperEvent.push_back(ExtractOutLine);
	vecMapperEvent.push_back(ExtractAccuraOutLine);
	vecMapperEvent.push_back(ExtractHandPaintOutline);
	vecMapperEvent.push_back(AutoExtractWallSpaceEvent);
	vecMapperEvent.push_back(HandExtractWallSpace);
	vecMapperEvent.push_back(FittingWallSpace);
	vecMapperEvent.push_back(ExtractWallInsect);
	vecMapperEvent.push_back(DrawHorizLine);
	vecMapperEvent.push_back(CombineGroup);
	vecMapperEvent.push_back(ExtractLineInsect);
	vecMapperEvent.push_back(ExtractHandPaintLine);
	vecMapperEvent.push_back(ExtractTriBaseOutline);
	vecMapperEvent.push_back(ExtractTriBaseOutLineInArea);
	vecMapperEvent.push_back(SelectLineByRectangle);
	vecMapperEvent.push_back(SelectLineByArea);
	vecMapperEvent.push_back(SelectLineByClick);
	vecMapperEvent.push_back(ExtractPtCloudEdge);
	vecMapperEvent.push_back(ExtractPtCloud);
	vecMapperEvent.push_back(SelectPtCloudByArea);
	vecMapperEvent.push_back(ExtractPtCloudinMultiTiles);
	vecMapperEvent.push_back(HandDrawEdge);
	vecMapperEvent.push_back(HandDrawEdge_1);
	//vecMapperEvent.push_back(ExtractRoadByDom);
	//vecMapperEvent.push_back(BreakoutLine);
	//vecMapperEvent.push_back(CrossLine);
	//vecMapperEvent.push_back(ExtractFarmLandByDsm);
	vecMapperEvent.push_back(ExtractContour);
	vecMapperEvent.push_back(ExtractLineBasePG);
}

void COsgMapperEvent::outForDxfAndShp(osg::Group *root)
{
	osg::ref_ptr<osg::Group> mapperGroup = root->getChild(3)->asGroup();

	vector<vector<osg::Vec3d> > vecVec;
	vector<osg::Vec3d> vecLine;
	vecLine.clear();
	vecVec.clear();
	InterfaceMFCExcuteFactory IExcuteFactory;
	InterfaceMFCExcute* IExcute = IExcuteFactory.create();
	std::vector<osg::MatrixTransform*> vecTrans;
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();

	osg::ref_ptr<osg::Vec3Array> extracted_verts;
	map<string, std::vector<osg::ref_ptr<osg::Group> > > mymap;
	std::vector<osg::ref_ptr<osg::Group> > vecGroup;
	std::vector<string> vecName;

	osg::ref_ptr<osg::Group> group;
	for (int i = 0; i<mapperGroup->getNumChildren(); ++i)
	{
		vecName.push_back(mapperGroup->getChild(i)->getName());
	}
	sort(vecName.begin(), vecName.end());
	std::vector<string>::iterator pos;
	pos = unique(vecName.begin(), vecName.end());
	vecName.erase(pos, vecName.end());

	for (int i = 0; i<vecName.size(); ++i)
	{
		vecGroup.clear();
		for (int j = 0; j<mapperGroup->getNumChildren(); ++j)
		{
			if (mapperGroup->getChild(j)->getName() == vecName[i])
			{
				vecGroup.push_back(mapperGroup->getChild(j)->asGroup());
			}
		}
		mymap[vecName[i]] = vecGroup;
	}

	map<string, vector< vector<osg::Vec3d> > > dataMap;

	for (map<string, std::vector<osg::ref_ptr<osg::Group> > >::iterator ite = mymap.begin(); ite != mymap.end(); ++ite)
	{
		vecVec.clear();
		vecTrans.clear();
		for (int k = 0; k<(*ite).second.size(); ++k)
		{
			IFinder->FindTransNode(((*ite).second)[k], vecTrans);
			if (vecTrans.size() == 0)
			{
				continue;
			}
			for (int i = 0; i < vecTrans.size(); ++i)
			{
				osg::Geode* geode = vecTrans[i]->getChild(0)->asGeode();
				osg::Matrix matrix = vecTrans[i]->getMatrix();
				IFinder->ExtractVerts(geode, extracted_verts);
				for (int j = 0; j < extracted_verts->size(); ++j)
				{
					osg::Vec3d pt(extracted_verts->at(j).x(), extracted_verts->at(j).y(), extracted_verts->at(j).z());
					pt = pt*matrix;
					vecLine.push_back(pt);
				}
				vecVec.push_back(vecLine);
				vecLine.clear();
			}
		}
		dataMap[(*ite).first] = vecVec;
	}

	//创建文件夹并设置输出路径
	std::string extSelect = "DXF FILE (*.dxf)|*.dxf|SHP FILE (*.shp)|*.shp||";
	std::string outputFileName;

	bool isSuccess = IExcute->OpenDialogEx(outputFileName, extSelect, false, ".dxf", ".shp");
	//IExcute->PopUpMessage(outputFileName);
	if (!isSuccess)
	{
		//IExcute->PopUpMessage("文件保存路径错误");
		return;
	}


	if (int(outputFileName.find(".shp")) != -1)//如果输出shp
	{
		toShpAndDxf::convertToShp(vecVec, outputFileName, "hatching");
		IExcute->PopUpMessage("SHP文件输出完成！");
		return;
	}
	else
	{
		toShpAndDxf::convertToDxf(dataMap, outputFileName, "hatching");
		IExcute->PopUpMessage("DXF文件输出完成！");
		return;
	}
}

bool COsgMapperEvent::extractContour(double high)
{
	for (int i = 0; i < vecMapperEvent.size(); i++)
	{
		COsgExtractTriangleBaseOutLineEvent* extractTriangleBaseOutlineEvent = dynamic_cast<COsgExtractTriangleBaseOutLineEvent*> (vecMapperEvent[i]);

		if (extractTriangleBaseOutlineEvent)
		{
			//extractTriangleBaseOutlineEvent->extractContour(high);
			break;
		}
	}

	return true;
}

void COsgMapperEvent::extractContour(double zs, double ze, double zspan)
{
	for (int i = 0; i < vecMapperEvent.size(); i++)
	{
		COsgExtractContourEvent* extractTriangleBaseOutlineEvent = dynamic_cast<COsgExtractContourEvent*> (vecMapperEvent[i]);

		if (extractTriangleBaseOutlineEvent)
		{
			extractTriangleBaseOutlineEvent->extractContour(zs,ze,zspan);
			break;
		}
	}
}

void COsgMapperEvent::setPtCloudParameter(double angle, double alpha, double color)
{
	for (int i = 0; i < vecMapperEvent.size(); i++)
	{
		COsgExtractPtCloudEvent* ptCloudEvent = dynamic_cast<COsgExtractPtCloudEvent*> (vecMapperEvent[i]);

		if (ptCloudEvent)
		{
			ptCloudEvent->ANGLE = angle;
			ptCloudEvent->similarity = color;
		}
	}

	for (int i = 0; i < vecMapperEvent.size(); i++)
	{
		COsgExtractPtCloudEdgeEvent* ptCloudEdgeEvent = dynamic_cast<COsgExtractPtCloudEdgeEvent*> (vecMapperEvent[i]);

		if (ptCloudEdgeEvent)
		{
			ptCloudEdgeEvent->alpha = alpha;
		}
	}
	
}

void COsgMapperEvent::getPtCloudParameter(double& angle, double& alpha, double& color)
{
	for (int i = 0; i < vecMapperEvent.size(); i++)
	{
		COsgExtractPtCloudEvent* ptCloudEvent = dynamic_cast<COsgExtractPtCloudEvent*> (vecMapperEvent[i]);

		if (ptCloudEvent)
		{
			angle = ptCloudEvent->ANGLE;
			color = ptCloudEvent->similarity;
		}
	}

	for (int i = 0; i < vecMapperEvent.size(); i++)
	{
		COsgExtractPtCloudEdgeEvent* ptCloudEdgeEvent = dynamic_cast<COsgExtractPtCloudEdgeEvent*> (vecMapperEvent[i]);

		if (ptCloudEdgeEvent)
		{
			alpha = ptCloudEdgeEvent->alpha;
		}
	}
}

void COsgMapperEvent::getHatchParameter(double& step)
{
	for (int i = 0; i < vecMapperEvent.size(); i++)
	{
		COsgExtractHatchingEvent* HatchingEvent = dynamic_cast<COsgExtractHatchingEvent*> (vecMapperEvent[i]);
		if (HatchingEvent)
		{
			step = HatchingEvent->mStep;
			break;
		}
	}
}

void COsgMapperEvent::setHatchParameter(double step)
{
	for (int i = 0; i < vecMapperEvent.size(); i++)
	{
		COsgExtractHatchingEvent* HatchingEvent = dynamic_cast<COsgExtractHatchingEvent*> (vecMapperEvent[i]);
		if (HatchingEvent)
		{
			HatchingEvent->mStep = step;
			break;
		}
	}
}