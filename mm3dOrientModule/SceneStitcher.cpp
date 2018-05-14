#include "StdAfx.h"
#include "OSG_Plus.h"
#include "SceneStitcher.h"
#include <string>
#include "InterfaceOsgFinder.h"
#include "InterfaceOsgEditor.h"

std::string CSceneStitcher::ObtainFolderName(std::string inFileName)
{
	int posEnd = inFileName.rfind('\\');
	std::string inFilePath = inFileName.substr(0, posEnd);

	int posStart = inFilePath.rfind('\\');
	posEnd = inFilePath.length() - 1;
	std::string outFolder = inFilePath.substr(posStart, posEnd);
	return outFolder;
}

void CSceneStitcher::ObtainDatabasePath(std::string inFileName, std::string &outFileName)
{
	int posEnd = inFileName.rfind('\\');
	std::string inFilePath = inFileName.substr(0, posEnd);

	posEnd = inFilePath.rfind('\\');
	outFileName = inFilePath.substr(0, posEnd);
}

bool CSceneStitcher::IsPathCorrect(std::vector<std::string> vecSceneFileName)
{
	std::string dataBasePathName = "";

	for (int i = 0; i < vecSceneFileName.size(); i++)
	{
		if (i == 0)
		{
			 ObtainDatabasePath(vecSceneFileName[i], dataBasePathName);
		}
		else
		{
			std::string secondDataBasePathName = "";
			ObtainDatabasePath(vecSceneFileName[i], secondDataBasePathName);

			if (dataBasePathName != secondDataBasePathName)
			{
				return false;
			}
		}
	}
	
	return true;
}

void CSceneStitcher::SceneStitch(osg::ref_ptr<osg::Group> sceneGroup, std::vector<std::string> vecSceneFileName, osg::ref_ptr<osg::Group> &combineGroup)
{
	int childNum = sceneGroup->getNumChildren();

	for (int i = 0; i < childNum; i++)
	{
		osg::ref_ptr<osg::Node> childNode = sceneGroup->getChild(i);
		std::string fileName = vecSceneFileName[i];
		std::string outFolder = ObtainFolderName(fileName);

		InterfaceOsgEditorFactory IEditorFactory;
		InterfaceOsgEditor* IEditor = IEditorFactory.create();
		IEditor->CombineScene(childNode, outFolder);

		combineGroup->addChild(childNode);
	}
}

void CSceneStitcher::SceneSplit(osg::ref_ptr<osg::Node> sceneNode, std::string inFileName)
{
	InterfaceOsgEditorFactory IEditorFactory;
	InterfaceOsgEditor* IEditor = IEditorFactory.create();
	IEditor->SplitSceneLod(sceneNode, inFileName);
}

void CSceneStitcher::AdjustWholeSceneFlatPlaneXY(osg::ref_ptr<osg::Node> selectNode, double xMove, double yMove)
{
	//增加x和y
	InterfaceOsgEditorFactory IEditorFactory;
	InterfaceOsgEditor* IEditor = IEditorFactory.create();
	IEditor->ChangeTransMatrix(selectNode, xMove, yMove, 0);
}

void CSceneStitcher::AdjustWholeSceneHeight(osg::ref_ptr<osg::Node> selectNode, double zMove)
{
	//增加z
	InterfaceOsgEditorFactory IEditorFactory;
	InterfaceOsgEditor* IEditor = IEditorFactory.create();
	IEditor->ChangeTransMatrix(selectNode, 0, 0, zMove);
}