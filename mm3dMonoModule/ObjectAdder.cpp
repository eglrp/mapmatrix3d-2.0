#include "stdafx.h"
#include "ObjectAdder.h"
#include "InterfaceOsgEditor.h"
#include "InterfaceOsgFinder.h"
#include <osg/ComputeBoundsVisitor>

void CObjectAdder::addObjectToScene(osg::ref_ptr<osg::Node> sceneNode, osg::ref_ptr<osg::MatrixTransform> objectNode)
{
	//加到头文件之中
	std::vector<osg::MatrixTransform*> vecMatrixTrans;
	InterfaceOsgFinderFactory iOsgFinderFactory;
	InterfaceOsgFinder* iOsgFinder = iOsgFinderFactory.create();
	iOsgFinder->FindTransNode(sceneNode, vecMatrixTrans);

	if (vecMatrixTrans.size() != 0)
	{
		//构建pagedLod
		osg::ref_ptr<osg::Group> objectGroup = objectNode->getChild(0)->asGroup();
		osg::ref_ptr<osg::PagedLOD> pagedLod = new osg::PagedLOD;

		for (int i = 0; i < objectGroup->getNumChildren(); i++)
		{
			osg::ref_ptr<osg::Node> childNode = objectGroup->getChild(i);
			pagedLod->addChild(childNode);
			pagedLod->setRange(i, 0, 0);
		}

		osg::Matrix worldMatrix = vecMatrixTrans[0]->getMatrix();

		InterfaceOsgEditorFactory iOsgEditorFactory;
		InterfaceOsgEditor* iOsgEditor = iOsgEditorFactory.create();
		iOsgEditor->ReCalPagedLodCenter(pagedLod);

		osg::Vec3d center = pagedLod->getCenter();
		center = center * worldMatrix;
		osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
		trans->setMatrix(worldMatrix);
		trans->addChild(pagedLod);

		osg::ComputeBoundsVisitor cbbv;
		trans->accept(cbbv);
		osg::BoundingBox bb = cbbv.getBoundingBox();
		double xMin = bb.xMin(); double yMin = bb.yMin(); double xMax = bb.xMax(); double yMax = bb.yMax();
		char strID[MAX_PATH];
		sprintf_s(strID, "%lf", center.x() * 100000);
		int id = atoi(strID);

		char strObj[MAX_PATH];
		sprintf_s(strObj, "object id; %d;center; %.1lf, %.1lf, %.1lf;range; %.1lf, %.1lf, %.1lf, %.1lf;",
			id, center.x(), center.y(), center.z(), xMin, yMin, xMax, yMax);
		pagedLod->setName(strObj);

		vecMatrixTrans[0]->getChild(0)->asGroup()->addChild(pagedLod);
	}
}