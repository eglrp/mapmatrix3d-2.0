#include "stdafx.h"
#include "OsgPagedLodOutputer.h"
#include "InterfaceMFCExcute.h"
#include "InterfaceOsgEditor.h"

void COsgPagedLodOutputer::replacePagedLodClipFilePath(osg::ref_ptr<osg::MatrixTransform> mTrans, std::string inFilePath, std::vector<std::string> vecHalfInPolygonFileName)
{
	//把路径替换
	osg::ref_ptr<osg::Group> mGroup = mTrans->getChild(0)->asGroup();
	int childNum = mGroup->getNumChildren();

	for (int i = 0; i < childNum; i++)
	{
		osg::ref_ptr<osg::PagedLOD> pagedLod = dynamic_cast<osg::PagedLOD*> (mGroup->getChild(i));

		if (!pagedLod)
		{
			continue;
		}

		std::string fileName = pagedLod->getFileName(1);

		if (std::find(vecHalfInPolygonFileName.begin(), vecHalfInPolygonFileName.end(), fileName) != vecHalfInPolygonFileName.end())
		{
			std::string subFileName;

			if (fileName.find("Clip") != -1)
			{
				subFileName = fileName;
			}
			else
			{
				InterfaceMFCExcuteFactory iMFCExcuteFactory;
				InterfaceMFCExcute* IExcute = iMFCExcuteFactory.create();
				std::string tileFileName = IExcute->ObtainTileFileName(fileName);
				std::string blockFileName = IExcute->ObtainBlockName(fileName);
				subFileName = blockFileName + ".\\Clip" + tileFileName;
			}

			int posL = subFileName.find('\\');
			std::string newSubFileName = subFileName.substr(posL, subFileName.length() - 1);
			std::string newChildLodFileName = inFilePath + newSubFileName;
			pagedLod->setFileName(1, subFileName);
		}
	}
}


bool COsgPagedLodOutputer::outputPagedLodNode(osg::PagedLOD& pagedLod, osg::ref_ptr<osg::Node> node)
{
	if (!node)
	{
		return false;
	}

	//先删除
	std::string childFileTitle = pagedLod.getFileName(1);

	//检查clip文件夹
	std::string outChildFileName;
	CheckClipFolder(outFilePath, childFileTitle, outChildFileName);

	//检测文件是否存在
	InterfaceMFCExcuteFactory IExcuteFactory;
	InterfaceMFCExcute* IExcute = IExcuteFactory.create();
	bool isExsists = IExcute->IsFileExists(outChildFileName);

	if (isExsists)
	{
		IExcute->RemoveFile(outChildFileName);
	}

	//创建目录
	int pos = outChildFileName.rfind('\\');
	std::string outChildFilePath = outChildFileName.substr(0, pos + 1);
	IExcute->createFolder(outChildFilePath);											

	//输出
	WriteNodeToFile(node, outChildFileName);

	return true;
}

void COsgPagedLodOutputer::setOutFilePath(std::string OutFilePath)
{
	outFilePath = OutFilePath;
}


void COsgPagedLodOutputer::OutputTotalInScopeOsgb(std::string inFilePath, std::vector<std::string> &vecTotalInPolygonFileName, CStProgressSetter stProgressSetter)
{
	/*int secondHalfPercentStart = stProgressSetter.percentStart + halfPercentInterval;
	int otherHalfPercentInterval = stProgressSetter.percentInterval - halfPercentInterval;*/
	InterfaceMFCExcuteFactory IMFCExcuteFactory;
	InterfaceMFCExcute* IMFCExcute = IMFCExcuteFactory.create();

	for (int i = 0; i < vecTotalInPolygonFileName.size(); i++)
	{
		stProgressSetter.SetProgress(i, vecTotalInPolygonFileName.size());

		int pos = vecTotalInPolygonFileName[i].rfind('\\');
		std::string osgbFilePath = inFilePath + vecTotalInPolygonFileName[i].substr(0, pos);
		int length = vecTotalInPolygonFileName[i].length() - 1;

		std::string outOsgbFileName;
		CheckClipFolder(outFilePath, vecTotalInPolygonFileName[i], outOsgbFileName);

		pos = outOsgbFileName.rfind('\\');
		std::string outOsgbFilePath = outOsgbFileName.substr(0, pos);

		IMFCExcute->createFolder(outOsgbFilePath);
		IMFCExcute->FileCopyTo(osgbFilePath, outOsgbFilePath, "*.osgb");
	}
}


void COsgPagedLodOutputer::CheckClipFolder(std::string filePath, std::string fileTitle, std::string &outFileName)
{
	//判断是否有clip
	int pos = filePath.find("Clip");

	if (pos != -1)
	{
		outFileName = filePath + ObtainTileFileName(fileTitle);
	}
	else
	{
		outFileName = filePath + fileTitle;														// 合并
	}
}

std::string COsgPagedLodOutputer::ObtainTileFileName(std::string fileName)
{
	int pos = fileName.find("Tile");
	int length = fileName.length();
	std::string tileFileName = ".\\" + fileName.substr(pos, length - pos);
	return tileFileName;
}

void COsgPagedLodOutputer::WriteNodeToFile(osg::ref_ptr<osg::Node> node, std::string outFileNameInPiece)
{
	//输出照片
	osg::ref_ptr<osgDB::Options> spOptions = new osgDB::Options;
	spOptions->setPluginStringData("WriteImageHint", "IncludeFile");
	//设置相对路径
	InterfaceOsgEditorFactory IEditorFactory;
	InterfaceOsgEditor* IEditor = IEditorFactory.create();
	IEditor->SetLodFileName(node, "");

	osgDB::writeNodeFile(*node, outFileNameInPiece, spOptions.get());
}
