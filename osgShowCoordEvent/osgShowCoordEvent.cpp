// osgShowCoordEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgShowCoordEvent.h"


bool COsgShowCoordEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	isActivated = true;

	if (isActivated)
	{
		if(IsMouseMove(ea))											//左键
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			osg::NodePath np = pickResult.nodePath;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			//反馈坐标
			int row = 0; 
			int col = 0;
			for (int i = 0; i < np.size(); i++)
			{
				osg::ref_ptr<osg::PagedLOD> pagedLOD = dynamic_cast<osg::PagedLOD*> (np[i]);
				if (pagedLOD)
				{
					std::string fileName = pagedLOD->getFileName(1);

					if (fileName.size() == 0)
					{
						continue;
					}

					int rPos = fileName.rfind('_+');
					int pos = fileName.find('_+');
					int length = fileName.length() - 1;
					std::string rowStr = fileName.substr(pos + 2, rPos);
					std::string colStr = fileName.substr(rPos + 2, length);
					row = atof(rowStr.c_str());
					col = atof(colStr.c_str());
					if (row > 0 && col > 0)
					{
						break;
					}
				}
			}

			osg::ref_ptr<osg::Camera> camera = dynamic_cast<osg::Camera*> (textGroup->getChild(0));

			if(camera)
			{
				osg::ref_ptr<osg::Geode> geode = camera->getChild(0)->asGeode();

				if (geode)
				{
					osg::ref_ptr<osgText::Text> posText = dynamic_cast<osgText::Text*> (geode->getDrawable(0));
					osg::ref_ptr<osgText::Text> posText1 = dynamic_cast<osgText::Text*> (geode->getDrawable(1));
					osg::ref_ptr<osgText::Text> posText2 = dynamic_cast<osgText::Text*> (geode->getDrawable(2));
					osg::ref_ptr<osgText::Text> posText3 = dynamic_cast<osgText::Text*> (geode->getDrawable(3));
					if (posText)
					{
						char strMsg[99];
						sprintf_s(strMsg, "Row:%d Col:%d", row, col);
						posText->setText(strMsg);
					}
					if (posText1)
					{
						char strMsg[99];
						sprintf_s(strMsg, "[X]:%lf",  worldPt.x());
						posText1->setText(strMsg);
					}
					if (posText2)
					{
						char strMsg[99];
						sprintf_s(strMsg, "[Y]:%lf",  worldPt.y());
						posText2->setText(strMsg);
					}
					if (posText3)
					{
						char strMsg[99];
						sprintf_s(strMsg, "[Z]:%lf", worldPt.z());
						posText3->setText(strMsg);
					}
				}
			}

		}
	}

	return false;
}

void COsgShowCoordEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{

}