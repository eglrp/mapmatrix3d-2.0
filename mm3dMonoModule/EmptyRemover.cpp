#include "stdafx.h"
#include "EmptyRemover.h"
#include "InterfaceOsgFinder.h"

void COsgEmptyRemover::RemoveEmptyGeode(osg::ref_ptr<osg::MatrixTransform> mTrans)
{
	//Çå³ý¿Õ°×geode
	InterfaceOsgFinderFactory iOsgFinderFactory;
	InterfaceOsgFinder* IOsgFinder = iOsgFinderFactory.create();

	std::vector<osg::Geode*> vecGeode = IOsgFinder->FindAndGetGeode(mTrans);

	for (int i = 0; i < vecGeode.size(); i++)
	{
		int triNum = 0;
		IOsgFinder->FindTriangleNum(vecGeode[i], triNum);

		if (triNum == 0)
		{
			osg::ref_ptr<osg::Group> parent = vecGeode[i]->getParent(0);
			parent->removeChild(vecGeode[i]);
		}
	}
}