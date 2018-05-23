#include "StdAfx.h"
#include "cv_plus.hpp"
#include "SceneHorizoner.h"
#include "InterfaceOsgEditor.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceMFCExcute.h"

CSceneHorizoner::CSceneHorizoner(void)
{
}


CSceneHorizoner::~CSceneHorizoner(void)
{
}

void CSceneHorizoner::StartModelHorizontalization(osg::ref_ptr<osg::Group>& sceneGroup)
{
	int childNum = sceneGroup->getNumChildren();

	for (int i = 0; i < childNum; i++)
	{
		osg::ref_ptr<osg::Node> sceneNode = sceneGroup->getChild(i);

		//计算主方向
		osg::Matrix matrix;
		CalculateSceneRotMat(sceneNode, matrix);

		//求逆
		osg::Matrix iMatrix;
		iMatrix = iMatrix.inverse(matrix);

		//设置变换节点
		osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform();
		trans->setMatrix(iMatrix);

		//替换原来节点
		osg::ref_ptr<osg::Node> copyNode = dynamic_cast<osg::Node*> (sceneNode->clone(osg::CopyOp::DEEP_COPY_ALL));
		trans->addChild(sceneNode.get());
		sceneGroup->replaceChild(sceneNode, trans);
	}

}

void CSceneHorizoner::ModelMirror(osg::ref_ptr<osg::Group> &sceneGroup)
{
	osg::Matrix matrix;
	double db[16] = {0};
	db[0] = 1; db[5] = -1; db[10] = 1; db[15] = 1; 
	matrix.set(db);

	InterfaceOsgEditorFactory IEditorFactory;
	InterfaceOsgEditor* IEditor = IEditorFactory.create();
	IEditor->MultiplyTransMatrix(sceneGroup, matrix);
}

void CSceneHorizoner::ModelUpsideDown(osg::ref_ptr<osg::Group> &sceneGroup)
{
	osg::Matrix matrix;
	double db[16] = {0};
	db[0] = -1; db[5] = -1; db[10] = -1; db[15] = 1; 
	matrix.set(db);
	
	InterfaceOsgEditorFactory IEditorFactory;
	InterfaceOsgEditor* IEditor = IEditorFactory.create();
	IEditor->MultiplyTransMatrix(sceneGroup, matrix);
}

void CSceneHorizoner::CalculateSceneRotMat(osg::ref_ptr<osg::Node> sceneNode, osg::Matrix &matrix)
{ 
	osg::ref_ptr<osg::Vec3Array> extracted_verts = new osg::Vec3Array;
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IEditor = IFinderFactory.create();
	IEditor->ExtractVerts(sceneNode, extracted_verts);

	int size_t = extracted_verts->size(); 

	if (size_t == 0)
	{
		InterfaceMFCExcuteFactory IExcuteFactory;
		InterfaceMFCExcute* IExcute = IExcuteFactory.create();
		IExcute->PopUpMessage("场景中无三维点!");
		return;
	}

	//PCA计算旋转矩阵
	CvMat* pData = cvCreateMat(size_t, 3, CV_64FC1);
	CvMat* pMean  = cvCreateMat(1, 3, CV_64FC1);
	CvMat* pEigVals = cvCreateMat(1, 3, CV_64FC1);
	CvMat* pEigVecs = cvCreateMat(3, 3, CV_64FC1);
	cvSetZero(pMean);
	cvSetZero(pData);
	cvSetZero(pEigVals);
	cvSetZero(pEigVecs);
	int nRow = size_t;
	int nCol = 3;
	double* dbData= pData->data.db;  

	//构建数据矩阵
	std::vector<osg::Vec3>::iterator iter = extracted_verts.get()->begin(); 

	for (int i = 0; i < nRow; i++) 
	{ 
		double x = iter->x();
		double y = iter->y();
		double z = iter->z();
		dbData[i * nCol + 0] = x;
		dbData[i * nCol + 1] = y;
		dbData[i * nCol + 2] = z;
		iter++;
	}; 

	cvCalcPCA(pData, pMean, pEigVals, pEigVecs, CV_PCA_DATA_AS_ROW);				//计算PCA

	double* db = (double*) matrix.ptr();

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			CvScalar scalar= cvGet2D(pEigVecs, i, j);
			db[i * 4 + j] = scalar.val[0];
		}
	}

	cvReleaseMat(&pMean);
	cvReleaseMat(&pData);
	cvReleaseMat(&pEigVals);
	cvReleaseMat(&pEigVecs);

	return ;
}