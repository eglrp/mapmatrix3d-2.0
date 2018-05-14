#include "stdafx.h"
#include "AccurateClipper.h"
#include "InterfaceOsgClip.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceOsgEditor.h"

bool CAccurateClipper::AccurateClipBottomScene(osg::ref_ptr<osg::MatrixTransform> &mTrans, bool bAccurate)
{
	//查找变换节点
	std::vector<osg::MatrixTransform*> vecMatrixTrans;
	bool isSuccess = FindTransNode(vecMatrixTrans);

	if (vecMatrixTrans.size() == 0)
	{
		return false;
	}

	//按范围查找底层并重新合并
	osg::ref_ptr<osg::Group> group = new osg::Group;
	FindBottomLevelOsgbModelInRect(vecMatrixTrans[0], group);

	osg::Matrix transMatrix = vecMatrixTrans[0]->getMatrix();
	mTrans->setMatrix(transMatrix);
	mTrans->addChild(group);

	//进行裁切
	StartAccurateClipBottomScene(mTrans,bAccurate);

	InterfaceOsgEditorFactory iOsgEditorFactory;
	InterfaceOsgEditor* iOsgEditor = iOsgEditorFactory.create();
	iOsgEditor->SetImageDataVariance(mTrans);

	return true;
}

bool CAccurateClipper::clipBottomScene(osg::ref_ptr<osg::MatrixTransform> &mTrans)
{
	//查找变换节点
	std::vector<osg::MatrixTransform*> vecMatrixTrans;
	bool isSuccess = FindTransNode(vecMatrixTrans);

	if (vecMatrixTrans.size() == 0)
	{
		return false;
	}

	//按范围查找底层并重新合并
	osg::ref_ptr<osg::Group> group = new osg::Group;
	FindBottomLevelOsgbModelInRect(vecMatrixTrans[0], group);

	osg::Matrix transMatrix = vecMatrixTrans[0]->getMatrix();
	mTrans->setMatrix(transMatrix);
	mTrans->addChild(group);

	//进行裁切
	startClipBottomScene(mTrans);

	InterfaceOsgEditorFactory iOsgEditorFactory;
	InterfaceOsgEditor* iOsgEditor = iOsgEditorFactory.create();
	iOsgEditor->SetImageDataVariance(mTrans);
}

bool CAccurateClipper::FindBottomLevelOsgbModelInRect(osg::ref_ptr<osg::MatrixTransform> mTrans, osg::ref_ptr<osg::Group> group)
{
	std::vector<std::string> vecHalfInPolygonFileName;
	std::vector<std::string> vecTotalInPolygonFileName;

	osg::ref_ptr<osg::Group> mGroup = mTrans->getChild(0)->asGroup();
	osg::Matrix worldMatrix = mTrans->getMatrix();

	InterfaceOsgClipFactory IClipFactory;
	InterfaceOsgClip* IClip = IClipFactory.create(BY_POLYGON);
	IClip->obtainSelTileName(vecScope, mGroup, worldMatrix, vecHalfInPolygonFileName, vecTotalInPolygonFileName);

	InterfaceOsgFinderFactory IOsgFinderFactory;
	InterfaceOsgFinder* IOsgFinder = IOsgFinderFactory.create();

	std::vector<std::string> vecBottomLevelFileName;

	for (int i = 0; i < mGroup->getNumChildren(); i++)
	{
		osg::ref_ptr<osg::PagedLOD> pagedLod = dynamic_cast<osg::PagedLOD*> (mGroup->getChild(i));

		if (pagedLod)
		{
			std::vector<std::string> vecTileBottomLevelFileName;
			std::string tileName = pagedLod->getFileName(1);

			if (std::find(vecHalfInPolygonFileName.begin(), vecHalfInPolygonFileName.end(), tileName) != vecHalfInPolygonFileName.end())
			{
				IOsgFinder->FindLodBottomLevelFile(pagedLod, vecTileBottomLevelFileName);
			}
			else if (std::find(vecTotalInPolygonFileName.begin(), vecTotalInPolygonFileName.end(), tileName) != vecTotalInPolygonFileName.end())
			{
				IOsgFinder->FindLodBottomLevelFile(pagedLod, vecTileBottomLevelFileName);
			}
			else
			{
				continue;
			}

			vecBottomLevelFileName.insert(vecBottomLevelFileName.end(), vecTileBottomLevelFileName.begin(), vecTileBottomLevelFileName.end());
		}
	}

	for (int i = 0; i < vecBottomLevelFileName.size(); i++)
	{
		osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(vecBottomLevelFileName[i]);
		group->addChild(node);
	}

	return true;
};

void CAccurateClipper::StartAccurateClipBottomScene(osg::ref_ptr<osg::MatrixTransform> mTrans, bool bAccurate)
{
	InterfaceOsgClipFactory IClipFactory;
	InterfaceOsgClip* IClip = IClipFactory.create(BY_ACCURATE_POLYGON);
	IClip->clipAccurateOsgHeader(mTrans, vecScope, true,bAccurate);
}


void CAccurateClipper::startClipBottomScene(osg::ref_ptr<osg::MatrixTransform> mTrans)
{
	InterfaceOsgClipFactory IClipFactory;
	InterfaceOsgClip* IClip = IClipFactory.create(BY_POLYGON);
	IClip->clipOsgHeader(mTrans, vecScope, true);
}


bool CAccurateClipper::FindTransNode(std::vector<osg::MatrixTransform*> &vecTrans)
{
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();
	IFinder->FindTransNode(sceneNode, vecTrans);

	if (vecTrans.size() == 0)
	{
		return false;
	}
}
