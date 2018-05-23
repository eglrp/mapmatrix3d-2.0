#include "stdafx.h"
#include "HighLighter.h"
#include "osgUserDefineEffect.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceOsgEditor.h"
#include <osgFX/Cartoon>

bool CHighLighter::StartHighLight(osg::ref_ptr<osg::Node> node)
{
	//高亮改节点
	osg::ref_ptr<COsgUserDefineEffect> fxPolygonHighLight = new COsgUserDefineEffect;
	osg::ref_ptr<osg::Node> childNode = node->asTransform()->getChild(0);

	if (!childNode)
	{
		return false;
	}

	fxPolygonHighLight->addChild(childNode);
	fxPolygonHighLight->setWireframeColor(osg::Vec4d(0.8, 0.5, 0.0, 1.0));

	node->asTransform()->replaceChild(childNode, fxPolygonHighLight);

	return true;
}

bool CHighLighter::highLightByColor(osg::ref_ptr<osg::Node> node)
{
	InterfaceOsgEditorFactory iOsgEditorFactory;
	InterfaceOsgEditor* iOsgEditor = iOsgEditorFactory.create();
	iOsgEditor->SetObjectHighLight(node, osg::Vec4d(0.0, 1.0, 1.0, 1.0));

	osg::ref_ptr<osg::PagedLOD> pagedLod = dynamic_cast<osg::PagedLOD*> (node->asGroup());

	if (!pagedLod)
	{
		return false;
	}

	osg::ref_ptr<osg::PolygonOffset> polyoffset = new osg::PolygonOffset;
	polyoffset->setFactor(-15.0f);
	polyoffset->setUnits(1.0f);
	pagedLod->getOrCreateStateSet()->setAttributeAndModes(polyoffset.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

	for (int i = 0; i < pagedLod->getRangeList().size(); i++)
	{
		pagedLod->setRange(i, 0, 1e+30);
	}

	return true;
}

bool CHighLighter::cancelHighLightByColor(osg::ref_ptr<osg::Node> node)
{
	osg::ref_ptr<osg::PagedLOD> pagedLod = dynamic_cast<osg::PagedLOD*> (node->asGroup());

	if (!pagedLod)
	{
		return false;
	}

	for (int i = 0; i < pagedLod->getRangeList().size(); i++)
	{
		pagedLod->setRange(i, 0, 0);
	}

	return true;
}

bool CHighLighter::highLightPagedLod(osg::ref_ptr<osg::Node> node)
{
	//高亮改节点
	osg::ref_ptr<COsgUserDefineEffect> fxPolygonHighLight = new COsgUserDefineEffect;
	osg::ref_ptr<osg::PagedLOD> pagedLod = dynamic_cast<osg::PagedLOD*> (node->asGroup());

	if (!pagedLod)
	{
		return false;
	}


	if (pagedLod->getNumParents() == 0)
	{
		return false;
	}

	fxPolygonHighLight->addChild(pagedLod);
	fxPolygonHighLight->setWireframeColor(osg::Vec4d(0.8, 0.5, 0.0, 1.0));
	fxPolygonHighLight->setWireframeLineWidth(3);

	for (int i = 0; i < pagedLod->getRangeList().size(); i++)
	{
		pagedLod->setRange(i, 0, 1e+30);
	}

	pagedLod->getParent(0)->replaceChild(pagedLod, fxPolygonHighLight);
}

bool CHighLighter::highLightCartoon(osg::ref_ptr<osg::Node> node)
{
	osg::ref_ptr<osgFX::Cartoon> ct = new osgFX::Cartoon();
	ct->setOutlineColor(osg::Vec4d(0.8, 0.5, 0.0, 1.0));
	ct->setOutlineLineWidth(580);
	ct->setLightNumber(0);

	osg::ref_ptr<osg::PagedLOD> pagedLod = dynamic_cast<osg::PagedLOD*> (node->asGroup());

	if (!pagedLod)
	{
		return false;
	}


	if (pagedLod->getNumParents() == 0)
	{
		return false;
	}

	ct->addChild(pagedLod);

	for (int i = 0; i < pagedLod->getRangeList().size(); i++)
	{
		pagedLod->setRange(i, 0, 1e+30);
	}

	pagedLod->getParent(0)->replaceChild(pagedLod, ct);
}


void CHighLighter::CancelHighLight(osg::ref_ptr<osg::Node> node)
{
	//取消高亮
	osg::ref_ptr<osg::Node> childNode = node->asTransform()->getChild(0);

	if (!childNode)
	{
		return;
	}

	osg::ref_ptr<COsgUserDefineEffect> fxPolygonHighLight = dynamic_cast<COsgUserDefineEffect*> (childNode.get());

	if (!fxPolygonHighLight)
	{
		return;
	}

	osg::ref_ptr<osg::Node> grandChild = fxPolygonHighLight->getChild(0);

	if (!grandChild)
	{
		return;
	}

	node->asTransform()->replaceChild(childNode, grandChild);
}

void CHighLighter::CancelAllHighLight(osg::ref_ptr<osg::Node> node)
{
	std::vector<osg::MatrixTransform*> vecMTrans;
	InterfaceOsgFinderFactory iOsgFinderFactory;
	InterfaceOsgFinder* iOsgFinder = iOsgFinderFactory.create();
	iOsgFinder->FindTransNode(node, vecMTrans);

	if (vecMTrans.size() == 0)
	{
		return;
	}

	for (int i = 0; i < vecMTrans.size(); i++)
	{
		std::string name = vecMTrans[i]->getName();

		if (name != "ObjectMatrix")
		{
			continue;
		}

		//取消高亮节点
		CancelHighLight(vecMTrans[i]);
	}
}


void CHighLighter::CancelHighLightPagedLod(osg::ref_ptr<osg::Node> node)
{
	//取消高亮
	osg::ref_ptr<osg::PagedLOD> pagedLod = dynamic_cast<osg::PagedLOD*> (node->asGroup());

	if (!pagedLod)
	{
		return;
	}

	if (pagedLod->getNumParents() == 0)
	{
		return;
	}

	osg::ref_ptr<osg::Group> fxGroup = pagedLod->getParent(0);
	
	if (!fxGroup)
	{
		return;
	}

	if (fxGroup->getNumParents() == 0)
	{
		return;
	}

	fxGroup->getParent(0)->replaceChild(fxGroup, pagedLod);

	for (int i = 0; i < pagedLod->getRangeList().size(); i++)
	{
		pagedLod->setRange(i, 0, 0);
	}
}


void CHighLighter::CancelAllHighLightPagedLod(osg::ref_ptr<osg::Node> node)
{
	std::vector<osg::PagedLOD*> vecPagedLod;
	InterfaceOsgFinderFactory iOsgFinderFactory;
	InterfaceOsgFinder* iOsgFinder = iOsgFinderFactory.create();
	iOsgFinder->FindPagedLod(node, vecPagedLod);

	if (vecPagedLod.size() == 0)
	{
		return;
	}

	for (int i = 0; i < vecPagedLod.size(); i++)
	{
		std::string name = vecPagedLod[i]->getName();

		if (name.find("object") == std::string::npos)
		{
			continue;
		}

		//取消高亮节点
		CancelHighLightPagedLod(vecPagedLod[i]);
	}
}


