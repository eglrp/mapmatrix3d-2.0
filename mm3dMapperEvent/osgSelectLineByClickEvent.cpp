// osgSelectLineByClickEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgSelectLineByClickEvent.h"

void COsgSelectLineByClickEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false; 

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_SELECT_LINE_BY_CLICK);

	if (isActivated == false && flag == true)
	{
		CancelHighLightLineForAll(mapperGroup);
		iSigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{
		iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键选择线段，按Delete删除所选线段，按ESC退出事件");
	}
}

bool COsgSelectLineByClickEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)//单击事件
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			worldPt = pickResult.worldPt;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}
			//高亮显示
			HighLightLine(mapperGroup, worldPt);
		}
		else if(IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Delete)
			{
				COsgDeleteSelectLine(mapperGroup);
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)
			{
				//取消高亮
				CancelHighLightLine(mapperGroup);

				iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键选择线段，按Delete删除所选线段，按ESC退出事件");
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{
				CancelHighLightLineForAll(mapperGroup);
			}
		}
	}
	
	return false;
}

void COsgSelectLineByClickEvent::CancelHighLightLineForAll(osg::ref_ptr<osg::Group> MapperGroup)
{
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();
	std::vector<osg::MatrixTransform*> vecTrans;
	IFinder->FindTransNode(MapperGroup, vecTrans);

	for (int i = 0; i < vecTrans.size(); i++)
	{
		osg::ref_ptr<osg::MatrixTransform> mTrans =  vecTrans[i];
		if (mTrans && mTrans->getName() == "highLight")
		{
			
			std::vector<osg::Geode*> vecGeode = IFinder->FindAndGetGeode(mTrans);
			for (int k = 0; k < vecGeode.size(); k++)
			{
				osg::ref_ptr<osg::Geode> geode = vecGeode[k];

				if (geode)
				{
					int numDrawables = geode->getNumDrawables();

					for(unsigned int i=0; i < geode->getNumDrawables(); ++i) 
					{ 
						osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode->getDrawable(i)); 
						osg::Vec4dArray* colorArray = new osg::Vec4dArray;
						colorArray->push_back(osg::Vec4d(1.0f, 0.f, 0.f, 0.f));
						geom->setColorArray(colorArray);
						geom->setColorBinding(osg::Geometry::BIND_OVERALL);
						geom->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
					} 			 
				}
			}
			mTrans->setName("");
		}
	}
}

void COsgSelectLineByClickEvent::CancelHighLightLine(osg::ref_ptr<osg::Group> MapperGroup)
{
	if (vecHighLightTrans.size() == 0)
	{
		return;
	}

	int totalNum = vecHighLightTrans.size();
	std::vector<osg::MatrixTransform*> vecHighLightGroup = vecHighLightTrans[totalNum - 1];

	for(int i = 0; i < vecHighLightGroup.size(); i++)
	{
		osg::ref_ptr<osg::MatrixTransform> mTrans = vecHighLightGroup[i];
		InterfaceOsgFinderFactory IFinderFactory;
		InterfaceOsgFinder* IFinder = IFinderFactory.create();
		std::vector<osg::Geode*> vecGeode = IFinder->FindAndGetGeode(mTrans);

		for (int k = 0; k < vecGeode.size(); k++)
		{
			osg::ref_ptr<osg::Geode> geode = vecGeode[k];

			if (geode)
			{
				int numDrawables = geode->getNumDrawables();

				for(unsigned int i=0; i < geode->getNumDrawables(); ++i) 
				{ 
					osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode->getDrawable(i)); 
					osg::Vec4dArray* colorArray = new osg::Vec4dArray;
					colorArray->push_back(osg::Vec4d(1.f, 0.f, 0.f, 1.f));
					geom->setColorArray(colorArray);
					geom->setColorBinding(osg::Geometry::BIND_OVERALL);
					geom->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
				} 			 
			}
		}

		mTrans->setName("");

	}

	if (vecHighLightTrans.size() > 0)
	{
		vecHighLightTrans.pop_back();
	}
}

void COsgSelectLineByClickEvent::HighLightLine(osg::ref_ptr<osg::Group> MapperGroup, osg::Vec3d pt)
{
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();
	std::vector<osg::MatrixTransform*> vecTrans;
	IFinder->FindTransNode(MapperGroup, vecTrans);
	std::vector<osg::MatrixTransform*> vecHighlightGroup;

	double r = 0.05;
	if (pt.x() > 100000 || pt.y() > 100000)//绝对定向坐标采用大步长
	{
		r = 0.1;
	}
	for (int i = 0; i < vecTrans.size(); i++)
	{
		osg::ref_ptr<osg::MatrixTransform> mTrans =  vecTrans[i];
		if (mTrans && mTrans->getName() != "highLight")
		{
			osg::ref_ptr<osg::Vec3Array> extracted_verts = new osg::Vec3Array;
			IFinder->ExtractVerts(mTrans, extracted_verts);
			osg::Matrix matrix = mTrans->getMatrix();

			if (extracted_verts->size() == 0)
			{
				return;
			}
			for (int j = 0; j < extracted_verts->size(); j++)
			{
				osg::Vec3d pt1(extracted_verts->at(j).x(), extracted_verts->at(j).y(), extracted_verts->at(j).z());
				pt1 = pt1 * matrix;

				if(distancep2p(pt,pt1) < r)
				{
					std::vector<osg::Geode*> vecGeode = IFinder->FindAndGetGeode(mTrans);
					for (int k = 0; k < vecGeode.size(); k++)
					{
						osg::ref_ptr<osg::Geode> geode = vecGeode[k];

						if (geode)
						{
							int numDrawables = geode->getNumDrawables();

							for(unsigned int i=0; i < geode->getNumDrawables(); ++i) 
							{ 
								osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode->getDrawable(i)); 
								osg::Vec4dArray* colorArray = new osg::Vec4dArray;
								colorArray->push_back(osg::Vec4d(0.f, 1.f, 1.f, 0.f));
								geom->setColorArray(colorArray);
								geom->setColorBinding(osg::Geometry::BIND_OVERALL);
								geom->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
							} 			 
						}
					}

					mTrans->setName("highLight");
					vecHighlightGroup.push_back(mTrans);
					break;
				}
				
			}
		}
	}
	if (vecHighlightGroup.size() > 0)
	{
		vecHighLightTrans.push_back(vecHighlightGroup);
	}
}

void COsgSelectLineByClickEvent::COsgDeleteSelectLine(osg::ref_ptr<osg::Group> mapperGroup)
{
	//for (int i = 0; i < mapperGroup->getNumChildren(); i++)
	//{

	//	osg::ref_ptr<osg::Group> parent = mapperGroup->getChild(i)->asGroup();
	//	
	//	if (parent)
	//	{
	//		if (parent->getName() == "highLight")
	//		{
	//			mapperGroup->removeChild(i);
	//			continue;
	//		}
	//		int childNum = parent->getNumChildren();

	//		for (int j = childNum - 1; j >= 0; j--)
	//		{
	//			osg::ref_ptr<osg::MatrixTransform> node = dynamic_cast<osg::MatrixTransform*> (parent->getChild(j));

	//			if (!node)
	//			{
	//				//return;
	//				continue;
	//			}

	//			if (node->getName() == "highLight")
	//			{
	//				parent->removeChild(node);
	//			}
	//		}

	//	}
	//}
	for (int i = 0; i < mapperGroup->getNumChildren(); i++)
	{
		std::vector<osg::MatrixTransform*> vecMatrixTrans;
		InterfaceOsgFinderFactory IFinderFactory;
		InterfaceOsgFinder* IFinder = IFinderFactory.create();
		IFinder->FindTransNode(mapperGroup, vecMatrixTrans);

		for (int j = 0; j < vecMatrixTrans.size(); j++)
		{
			osg::ref_ptr<osg::MatrixTransform> mTrans = vecMatrixTrans[j];

			if (mTrans && mTrans->getName() == "highLight")
			{
				if (mTrans->getNumParents() == 0)
				{
					continue;
				}

				osg::ref_ptr<osg::Group> parent = mTrans->getParent(0);

				if (parent)
				{
					parent->removeChild(mTrans);
				}
			}
		}
	}
}


//double COsgSelectLineByClickEvent::distanceOfPointAndLine(osg::Vec3d pt,osg::Vec3d line1,osg::Vec3d line2)
//{
//	double a = distancep2p(pt,line1);
//	double b = distancep2p(pt,line2);
//	double c = distancep2p(line1,line2);
//	double l = (a+b+c)/2;
//	double s = sqrt( l*(l-a)*(l-b)*(l-c) );
//	return 2*s/c;
//}