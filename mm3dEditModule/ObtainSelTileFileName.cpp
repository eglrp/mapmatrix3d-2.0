#include "stdafx.h"
#include "ObtainSelTileFileName.h"
#include "InterfaceOsgFinder.h"
#include "Geometry.h"
#include "qcomm.h"

void CObtainSelTileFileName::obtainSelTileName(osg::ref_ptr<osg::Group> mGroup, osg::Matrix matrix)
{
	//根据范围获取所有的tile目录
	int groupNum = mGroup->getNumChildren();

	//根据多边形范围筛选

	for (int i = groupNum - 1; i >= 0; i--)
	{
		osg::ref_ptr<osg::PagedLOD> pagedLod = dynamic_cast<osg::PagedLOD*>(mGroup->getChild(i));

		if (!pagedLod)
		{
			continue;
		}

		//获取所有的节点
		osg::ref_ptr<osg::Vec3Array> extracted_verts = new osg::Vec3Array;
		InterfaceOsgFinderFactory IFinderFactory;
		InterfaceOsgFinder* IFinder = IFinderFactory.create();
		IFinder->ExtractVerts(pagedLod, extracted_verts);

		if (extracted_verts->size() == 0)
		{
			continue;
		}

		std::string fileName = pagedLod->getFileName(1);
		int innerCnt = 0;

		for (int j = 0; j < extracted_verts->size(); j++)
		{
			osg::Vec3d pt(extracted_verts->at(j).x(), extracted_verts->at(j).y(), extracted_verts->at(j).z());
			osg::Vec3d pt1 = pt * matrix;

			bool bIsInPolygon = isInAreaJudger.isPtInArea(pt1);

			if (bIsInPolygon)
			{
				innerCnt++;
			}
		}

		if (innerCnt > 0 && innerCnt < extracted_verts->size())
		{
			vecHalfInFileName.push_back(fileName);
		}
		else if (innerCnt == extracted_verts->size())
		{
			vecTotalInFileName.push_back(fileName);
		}
	}

	return;
}

void CObtainSelTileFileName::obtainSelTileNamebyScope(osg::ref_ptr<osg::Group> mGroup, osg::Matrix matrix)
{
	//根据范围获取所有的tile目录
	int groupNum = mGroup->getNumChildren();

	//根据多边形范围筛选

	for (int i = groupNum - 1; i >= 0; i--)
	{
		osg::ref_ptr<osg::PagedLOD> pagedLod = dynamic_cast<osg::PagedLOD*>(mGroup->getChild(i));

		if (!pagedLod)
		{
			continue;
		}

		//获取所有的节点
		osg::ref_ptr<osg::Vec3Array> extracted_verts = new osg::Vec3Array;

		InterfaceOsgFinderFactory IFinderFactory;
		InterfaceOsgFinder* IFinder = IFinderFactory.create();
		IFinder->ExtractVerts(pagedLod, extracted_verts);

		std::vector<double> vecDbX, vecDbY;

		if (extracted_verts->size() == 0)
		{
			continue;
		}

		std::string fileName = pagedLod->getFileName(1);
		int innerCnt = 0;

		for (int j = 0; j < extracted_verts->size(); j++)
		{
			osg::Vec3d pt(extracted_verts->at(j).x(), extracted_verts->at(j).y(), extracted_verts->at(j).z());
			osg::Vec3d pt1 = pt * matrix;

			bool bIsInScope = isInAreaJudger.isPtInScope(pt1);

			if (bIsInScope)
			{
				innerCnt++;
			}
		}

		if (innerCnt > 0 && innerCnt < extracted_verts->size())
		{
			vecHalfInFileName.push_back(fileName);
		}
		else if (innerCnt == extracted_verts->size())
		{
			vecTotalInFileName.push_back(fileName);
		}
	}

	return;
}


bool CObtainSelTileFileName::obtainFlatFileName(osg::ref_ptr<osg::MatrixTransform> mTrans)
{
	osg::ref_ptr<osg::Group> mGroup = mTrans->getChild(0)->asGroup();

	if (!mGroup)
	{
		return false;
	}

	osg::Matrix worldMatrix = mTrans->getMatrix();
	obtainSelTileName(mGroup, worldMatrix);							//获得tile名
	vecFlatPolygonFileName = vecTotalInFileName;

	for (int j = 0; j < vecHalfInFileName.size(); j++)
	{
		if (std::find(vecFlatPolygonFileName.begin(), vecFlatPolygonFileName.end(), vecHalfInFileName[j]) == vecFlatPolygonFileName.end())
		{
			vecFlatPolygonFileName.push_back(vecHalfInFileName[j]);
		}
	}
}