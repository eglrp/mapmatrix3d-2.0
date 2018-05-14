#pragma once
#include "OSG_Plus.h"
#include "StProgressSetter.h"

class COsgPagedLodOutputer
{
public:
	COsgPagedLodOutputer(std::string OutFilePath)
	{
		outFilePath = OutFilePath;
	}

	virtual bool outputPagedLodNode(osg::PagedLOD& pagedLod, osg::ref_ptr<osg::Node> node);

	virtual void setOutFilePath(std::string OutFilePath);

	virtual void CheckClipFolder(std::string filePath, std::string fileTitle, std::string &outFileName);
	
	virtual void replacePagedLodClipFilePath(osg::ref_ptr<osg::MatrixTransform> mTrans, std::string inFilePath, std::vector<std::string> vecHalfInPolygonFileName);

	virtual void OutputTotalInScopeOsgb(std::string inFilePath, std::vector<std::string> &vecTotalInPolygonFileName, CStProgressSetter stProgressSetter);

protected:
	virtual std::string ObtainTileFileName(std::string fileName);

	virtual void WriteNodeToFile(osg::ref_ptr<osg::Node> node, std::string outFileNameInPiece);

private:
	std::string outFilePath;
};

