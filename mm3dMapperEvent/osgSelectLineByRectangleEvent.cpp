// osgSelectLineByRectangleEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgSelectLineByRectangleEvent.h"
#include  <osgFX/Scribe>
#include "qcomm.h"

void COsgSelectLineByRectangleEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false; 

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_SELECT_LINE_BY_RECTANGLE);

	if (isActivated == false && flag == true)
	{
		CancelHighLightLineForAll(mapperGroup, vecHighLightTrans);
		iSigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{
		iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键拖拉选择线段，按Delete删除所选线段，按ESC退出事件");
	}
}

bool COsgSelectLineByRectangleEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if(ea.getEventType() == osgGA::GUIEventAdapter::DRAG && GetKeyState(VK_CONTROL) < 0)				//拖动
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			if (isFirstPoint)
			{
				isFirstPoint = false;
				vecWorldCoord.push_back(worldPt);
			}

			iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键拖拉选择线段，按Delete删除所选线段，按ESC退出事件");
		}
		else if(ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && GetKeyState(VK_CONTROL) >= 0)
		{
			isFirstPoint = true;
			vecWorldCoord.clear();
			return false;
		}
		else if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			osg::Matrix worldMatrix = pickResult.matrix;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			osg::Matrix viewMatrix = mViewer->getCamera()->getViewMatrix();
			bool isSuccess = CtrlReleaseSelectLineByRectangle(mapperGroup, vecWorldCoord, worldPt, viewMatrix, vecHighLightTrans);

			if (!isSuccess)
			{
				return false;
			}

			isFirstPoint = true;
			iSigEventCallBack->EmitSigShowOpsHint("按Backspace 取消当前选择，按Z键取消全部选择");

		}
		else if(IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)					 //BackSpace
			{
				BackSpaceCancelHighLightLineInRectangle(vecHighLightTrans);

				iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键拖拉选择线段，按Delete删除所选线段，按ESC退出事件");
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{
				//取消高亮
				CancelHighLightLineForAll(mapperGroup, vecHighLightTrans);

				iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键拖拉选择线段，按Delete删除所选线段，按ESC退出事件");
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Delete)
			{
				COsgDeleteSelectLine(mapperGroup, vecHighLightTrans);
			}
		}
	}

	return false;
}

bool COsgSelectLineByRectangleEvent::CtrlReleaseSelectLineByRectangle(osg::ref_ptr<osg::Group> MapperGroup, std::vector<osg::Vec3d> &VecWorldCoord, osg::Vec3d worldPt, osg::Matrix viewMatrix, std::vector<std::vector<osg::MatrixTransform*>> &VecHighLightTrans)
{
	int totalNum = VecWorldCoord.size();

	if (totalNum == 0)
	{
		return false;
	}

	double x1 = VecWorldCoord[totalNum - 1].x(); double y1 = VecWorldCoord[totalNum - 1].y(); double x2 = worldPt.x(); double y2 = worldPt.y();
	//osg::Vec3 northVec = osg::Y_AXIS * viewMatrix;
	//double x0 = northVec.x(); double y0 = northVec.y();

	//double k = (x0 * x1 - x0 * x0 + y0 * y1 - y0 * y0)  / (x0 * x2 + y0 * y2);
	double x3 = x1/*k * x2 + x0*/;
	double y3 = y2/*k * y2 + y0*/;

	/*x0 = x0 * -1; y0 = y0 * -1;
	k = (x0 * x1 - x0 * x0 + y0 * y1 - y0 * y0)  / (x0 * x2 + y0 * y2);*/
	double x4 = x2/*k * x2 + x0*/;
	double y4 = y1/*k * y2 + y0*/;

	std::vector<CL::Vec2d> pol;
	pol.push_back(CL::Vec2d(x1, y1));
	pol.push_back(CL::Vec2d(x3, y3));
	pol.push_back(CL::Vec2d(x2, y2));
	pol.push_back(CL::Vec2d(x4, y4));

	//对于线段筛选并进行高亮
	HighLightLineInRectangle(MapperGroup, pol, VecHighLightTrans);

	VecWorldCoord.clear();
	return true;
}

void COsgSelectLineByRectangleEvent::HighLightLineInRectangle(osg::ref_ptr<osg::Group> MapperGroup, std::vector<CL::Vec2d> pol, std::vector<std::vector<osg::MatrixTransform*>> &VecHighLightTrans)
{
	if (pol.size() == 0)
	{
		return;
	}

	std::vector<osg::MatrixTransform*> vecHighlightGroup;

	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();
	std::vector<osg::MatrixTransform*> vecTrans;
	IFinder->FindTransNode(MapperGroup, vecTrans);

	for (int i = 0; i < vecTrans.size(); i++)
	{
		osg::ref_ptr<osg::MatrixTransform> mTrans =  vecTrans[i];
		bool isInPolygon = false;

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
				osg::Vec3d pt(extracted_verts->at(j).x(), extracted_verts->at(j).y(), extracted_verts->at(j).z());
				pt = pt * matrix;

				if (Point_In_Polygon_2D(pt.x(), pt.y(), pol))
				{
					isInPolygon = true;
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
		VecHighLightTrans.push_back(vecHighlightGroup);
	}
}

void COsgSelectLineByRectangleEvent::BackSpaceCancelHighLightLineInRectangle(std::vector<std::vector<osg::MatrixTransform*>> &VecHighLightTrans)
{
	if (VecHighLightTrans.size() == 0)
	{
		return;
	}

	int totalNum = VecHighLightTrans.size();
	std::vector<osg::MatrixTransform*> vecHighLightGroup = VecHighLightTrans[totalNum - 1];

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

	if (VecHighLightTrans.size() > 0)
	{
		VecHighLightTrans.pop_back();
	}

	//for (int i = 0; i < MapperGroup->getNumChildren(); i++)
	//{
	//	osg::ref_ptr<osg::Group> parent = MapperGroup->getChild(i)->asGroup();

	//	if (parent)
	//	{
	//		int childNum = parent->getNumChildren();

	//		for (int j = childNum - 1; j >= 0; j--)
	//		{
	//			osg::ref_ptr<osg::MatrixTransform> mTrans = dynamic_cast<osg::MatrixTransform*> (parent->getChild(j)->asTransform());

	//			if (!mTrans)
	//			{
	//				continue;
	//			}

	//			if (mTrans->getName() != "highLight")
	//			{
	//				continue;
	//			}

	//			bool isInPolygon = false;
	//			osg::ref_ptr<osg::Vec3Array> extracted_verts = new osg::Vec3Array;
	//			InterfaceOsgFinderFactory IFinderFactory;
	//			InterfaceOsgFinder* IFinder = IFinderFactory.create();
	//			IFinder->ExtractVerts(mTrans, extracted_verts);
	//			osg::Matrix matrix = mTrans->getMatrix();

	//			if (extracted_verts->size() == 0)
	//			{
	//				continue;
	//			}

	//			for (int j = 0; j < extracted_verts->size(); j++)
	//			{
	//				osg::Vec3d pt(extracted_verts->at(j).x(), extracted_verts->at(j).y(), extracted_verts->at(j).z());
	//				pt = pt * matrix;

	//				if (Point_In_Polygon_2D(pt.x(), pt.y(), pol))
	//				{
	//					isInPolygon = true;
	//					std::vector<osg::Geode*> vecGeode = IFinder->FindAndGetGeode(mTrans);

	//					for (int k = 0; k < vecGeode.size(); k++)
	//					{
	//						osg::ref_ptr<osg::Geode> geode = vecGeode[k];

	//						if (geode)
	//						{
	//							int numDrawables = geode->getNumDrawables();

	//							for(unsigned int i=0; i < geode->getNumDrawables(); ++i) 
	//							{ 
	//								osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode->getDrawable(i)); 
	//								osg::Vec4dArray* colorArray = new osg::Vec4dArray;
	//								colorArray->push_back(osg::Vec4d(1.f, 0.f, 0.f, 1.f));
	//								geom->setColorArray(colorArray);
	//								geom->setColorBinding(osg::Geometry::BIND_OVERALL);
	//								geom->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	//							} 			 
	//						}
	//					}

	//					mTrans->setName("");
	//				}
	//			}
	//		}
	//	}
	//}
}

void COsgSelectLineByRectangleEvent::CancelHighLightLineForAll(osg::ref_ptr<osg::Group> MapperGroup, std::vector<std::vector<osg::MatrixTransform*>> &VecHighLightTrans)
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

	vecHighLightTrans.clear();
}

void COsgSelectLineByRectangleEvent::COsgDeleteSelectLine(osg::ref_ptr<osg::Group> MapperGroup, std::vector<std::vector<osg::MatrixTransform*>> &VecHighLightTrans)
{
	if (mapperGroup->getNumChildren() == 0)
	{
		return;
	}

	for (int i = 0; i < mapperGroup->getNumChildren(); i++)
	{
		std::vector<osg::MatrixTransform*> vecMatrixTrans;
		InterfaceOsgFinderFactory IFinderFactory;
		InterfaceOsgFinder* IFinder = IFinderFactory.create();
		IFinder->FindTransNode(MapperGroup, vecMatrixTrans);

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

	VecHighLightTrans.pop_back();
}

