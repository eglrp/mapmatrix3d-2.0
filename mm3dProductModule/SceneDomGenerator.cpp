#include "StdAfx.h"
#include "SceneDomGenerator.h"

#include "qcomm.h"
#include <osg/ComputeBoundsVisitor>

bool CSceneDomGenerator::StartProducingDOM(osg::ref_ptr<osg::Node> sceneNode, std::string SceneFileName, std::string OutputFileName, double Res, int XTileNum,
	int YTileNum, InterfaceSigEventCallBack* ISigCallBack)
{
	InitDomGeneratorPara(SceneFileName, OutputFileName, Res, XTileNum, YTileNum, ISigCallBack);

	bool isSuccess = false;
	std::vector<osg::MatrixTransform*> vecTransNode;
	InterfaceOsgFinderFactory IFactory;
	InterfaceOsgFinder* IOsgFinder = IFactory.create();
	IOsgFinder->FindTransNode(sceneNode, vecTransNode);

	if (vecTransNode.size() == 1)
	{
		_logop("开始生成单个DOM");
		ISigCallBack->EmitSigShowOpsHint("正在生成DOM...当前数据为第1块，共1块, 请不要切换屏幕的分辨率，计算机设置从不休眠。");
		isSuccess = GenerateDOMForSingleModel(vecTransNode[0], outputFileName, false);
	}
	else if (vecTransNode.size() >= 2)
	{
		_logop("开始生成多个DOM");
		isSuccess = GenerateMultiDomForSingleModel(vecTransNode);
	}

	ISigCallBack->EmitSigShowOpsHint("");
	ISigCallBack->EmitSigEndProgressFun();
	return isSuccess;
}

bool CSceneDomGenerator::StartGeneratingDOM(std::string outputDomFileName, std::string outputCoordFileName, osg::ref_ptr<osg::MatrixTransform> mTrans, 
	bool isCombine)
{
	int rPos = sceneFileName.rfind('\\'); 
	std::string sceneFilepath = sceneFileName.substr(0, rPos + 1);

	int pos = 0;
	ISigCallBack->EmitSigStartProgressFun();
	InterfaceMFCExcuteFactory IMFCExecuteFactory;
	InterfaceMFCExcute* IMFCExecute = IMFCExecuteFactory.create();

	osg::Matrix matrix = mTrans->getMatrix();
	std::vector<std::string> vecImgFile;
	std::vector<std::string> vecCoordTxtFileName;
	
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();
	
	std::vector<osg::PagedLOD*> vecPagedLod;
	IFinder->FindPagedLod(mTrans, vecPagedLod);

	if (vecPagedLod.size() == 0)
	{
		return false;
	}

	_logop("完成节点查找");

	for (int i = 0; i < vecPagedLod.size(); i++)											
	{
		osg::ref_ptr<osg::Node> node = vecPagedLod[i];
		std::string fileName = vecPagedLod[i]->getFileName(1);

		std::vector<std::string> vecChildFileName;
		IFinder->FindLodBottomLevelFile(node, vecChildFileName);

		if (vecChildFileName.size() == 0)
		{
			continue;
		}
		
		osg::ref_ptr<osg::Group> group = new osg::Group;

		for (int j = 0; j < vecChildFileName.size(); j++)									 //生成block影像
		{
			if (vecChildFileName[j].size() <= 0)
			{
				continue;
			}

			osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(vecChildFileName[j]);

			if (!node)
			{
				continue;
			}

			if (IFinder->FindGeode(node) == false)
			{
				continue;
			}

			node->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);
			group->addChild(node);
		}

		if (group->getNumChildren() == 0)
		{
			continue;
		}

		int pos = vecChildFileName[0].rfind('.') + 1;
		std::string outRectDomFileName = vecChildFileName[0].substr(0, pos) + "jpg";

		if (produceNodeDOM(group, outRectDomFileName, matrix))
		{
			vecImgFile.push_back(outRectDomFileName);
			vecCoordTxtFileName.push_back(vecChildFileName[0].substr(0, pos) + "txt");
		}

		//计算进程
		pos = (int)((double) (i + 1) / (double) vecPagedLod.size() * 900);
		ISigCallBack->EmitSigSetProgressFun(pos);
	}

	std::vector<osg::Vec4d> vecRect = findImageRect(vecCoordTxtFileName);			   //获取image范围
	osg::Vec4d maxRect = findMaxRect(vecRect);										   //找到最大范围

	//计算范围创建影像
	int row = (maxRect.w() - maxRect.z()) / res;
	int col = (maxRect.y() - maxRect.x()) / res;
	//创建
	GDALAllRegister();          //GDAL所有操作都需要先注册格式
	GDALDriver *pDriver = GetGDALDriverManager()->GetDriverByName("GTIFF"); //图像驱动
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");  
	char** ppszOptions = CSLSetNameValue(NULL, "BIGTIFF", "IF_NEEDED"); //配置图像信息	
	int nBandCount = 3;
	GDALDataset* poDataset = pDriver->Create(outputDomFileName.c_str(), col, row, nBandCount, GDT_Byte, ppszOptions);

	_logop("开始合并真正射影像");

	for (int i = 0; i < vecRect.size(); i++)
	{
		osg::Vec4d rect;

		if (FindTileImageRect(vecCoordTxtFileName[i], rect))
		{
			combineImage(poDataset, vecImgFile[i], vecRect[i], maxRect);
		}

		IMFCExecute->RemoveFile(vecImgFile[i]);
		IMFCExecute->RemoveFile(vecCoordTxtFileName[i]);

		//计算进程
		pos = (int)((double) (i + 1) / (double) vecRect.size() * 90) + 900;
		ISigCallBack->EmitSigSetProgressFun(pos);
	}

	if (!isCombine)
	{
		StartRepairDom(mTrans, poDataset, sceneFilepath, maxRect);
	}

	//设置仿射变换参数
	double  adfGeoTransform[6] = {0};
	adfGeoTransform[0] = maxRect.x();
	adfGeoTransform[3] = maxRect.w();
	adfGeoTransform[1] = res;
	adfGeoTransform[5] = -res;
	poDataset->SetGeoTransform(adfGeoTransform);
	GDALClose(poDataset);
	poDataset = NULL;

	double xRot = 0;
	double yRot = 0;
	FILE* fpCoord = fopen(outputCoordFileName.c_str(), "w");
	fprintf(fpCoord, "%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n", res, xRot, yRot, -res, maxRect.x(), maxRect.w());
	fclose(fpCoord);
	_logop("输出单块dom");

	//ISigCallBack->EmitSigEndProgressFun();
}

bool CSceneDomGenerator::StartRepairDom(osg::ref_ptr<osg::MatrixTransform> mTrans, GDALDataset* poDataset, std::string sceneFilepath, osg::Vec4d maxRect)
{
	_logop("开始进行修补空洞");
	std::vector<std::string> vecRepairImgFile;
	std::vector<std::string> vecRepairCoordTxtFileName;
	//修补空洞DOM生成
	std::vector<osg::MatrixTransform*> mRepairMTrans;
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();
	IFinder->FindRepairTransNode(mTrans, mRepairMTrans);			//查找修补的trans节点

	for (int i = 0; i < mRepairMTrans.size(); i++)
	{
		osg::ref_ptr<osg::Node> repairNode = mRepairMTrans[i]->getChild(0);
		osg::Matrix worldMatrix;
		osg::MatrixList matrixList = repairNode->getWorldMatrices();

		if (matrixList.size() != 0)
		{
			osg::Matrix newMatrix;

			for (int i = 0; i < matrixList.size(); i++)
			{
				newMatrix.postMult(matrixList[i]);
			}

			worldMatrix = newMatrix;
		}

		char strMsg[99];
		sprintf_s(strMsg, "repair%d.jpg", i);
		std::string repairTitle = strMsg;
		std::string repairDomFileName = sceneFilepath + repairTitle;
		int pos = repairDomFileName.rfind('.') + 1;

		osg::ref_ptr<osg::Node> node = mRepairMTrans[i]->getChild(0);

		if (produceNodeDOM(node, repairDomFileName, worldMatrix))
		{
			vecRepairImgFile.push_back(repairDomFileName);
			vecRepairCoordTxtFileName.push_back(repairDomFileName.substr(0, pos) + "txt");
		}
	}	

	_logop("完成修补空洞的正射影像生成");
	
	_logop("开始进行真正射影像修补空洞");
	//进行修补图像的填补
	std::vector<osg::Vec4d> vecRepairRect = findImageRect(vecRepairCoordTxtFileName);			   //获取image范围

	for (int i = 0; i < vecRepairRect.size(); i++)
	{
		osg::Vec4d rect;

		if (FindTileImageRect(vecRepairCoordTxtFileName[i], rect))
		{
			GDALDataset* poRepairBlockDataset = (GDALDataset*) GDALOpen(vecRepairImgFile[i].c_str(), GA_ReadOnly);

			if (poRepairBlockDataset == NULL)
			{
				GDALClose(poRepairBlockDataset);
				poRepairBlockDataset = NULL;
				return FALSE;
			}

			double blockXSize = 10000 * res;
			double blockYSize = 10000 * res;

			double left = rect.x(); double right = rect.y(); double bottom = rect.z(); double top = rect.w();
			int blockXNum = (right - left) / blockXSize + 1;
			int blockYNum = (top - bottom) / blockYSize + 1;

			for (int j = 0; j < blockXNum; j++)
			{
				for (int k = 0; k < blockYNum; k++)
				{
					double blockLeft = j * blockXSize + left;
					double blockRight = 0;
					double blockBottom = k * blockYSize + bottom;
					double blockTop = 0;

					if (j != blockXNum - 1)
					{
						blockRight = blockLeft + blockXSize;
					}
					else
					{
						blockRight = right;
					}

					if (k != blockYNum - 1)
					{
						blockTop = blockBottom + blockYSize;
					}
					else
					{
						blockTop = top;
					}

					osg::Vec4d blockRect(blockLeft, blockRight, blockBottom, blockTop);
					combineBigRepairImage(poDataset, poRepairBlockDataset, blockRect, rect, maxRect);
				}
			}

			GDALClose(poRepairBlockDataset);
		}

		InterfaceMFCExcuteFactory IMFCExecuteFactory;
		InterfaceMFCExcute* IMFCExecute = IMFCExecuteFactory.create();
		IMFCExecute->RemoveFile(vecRepairImgFile[i]);
		IMFCExecute->RemoveFile(vecRepairCoordTxtFileName[i]);

		//计算进程
		int pos = (int)((double) (i + 1) / (double) vecRepairRect.size() * 9) + 990;
		ISigCallBack->EmitSigSetProgressFun(pos);
	}

	_logop("完成真正射影像修补空洞");
}


osg::Vec4d CSceneDomGenerator::findMaxRect(std::vector<osg::Vec4d> vecRect)
{
	double xMin = vecRect[0].x();
	double xMax = vecRect[0].y();
	double yMin = vecRect[0].z();
	double yMax = vecRect[0].w();

	for (int i = 1; i < vecRect.size(); i++)
	{
		if (xMin > vecRect[i].x())
		{
			xMin = vecRect[i].x();
		}

		if (xMax < vecRect[i].y())
		{
			xMax = vecRect[i].y();
		}

		if (yMin > vecRect[i].z())
		{
			yMin = vecRect[i].z();
		}

		if (yMax < vecRect[i].w())
		{
			yMax = vecRect[i].w();
		}
	}

	return osg::Vec4d(xMin, xMax, yMin, yMax);
}

void CSceneDomGenerator::combineImage(CvMat* imgMat, std::string imgFileName, osg::Vec4d rect, osg::Vec4d maxRect)
{
	InterfaceMFCExcuteFactory IMfcExcuteFactory;
	InterfaceMFCExcute* IMfcExcute = IMfcExcuteFactory.create();

	IplImage* img = cvLoadImage(imgFileName.c_str());

	if (img == NULL)
	{
		return;
	}

	uchar* data = (uchar*)img->imageData;
	uchar* combineData = imgMat->data.ptr;
	int widthStep = img->widthStep;
	int rowDiff = (maxRect.w() - rect.w()) / res;
	int colDiff = (rect.x() - maxRect.x()) / res;

	for (int i = 0; i < img->height; i++)
	{
		for (int j = 0; j < img->width; j++)
		{
			int rowImg = i + rowDiff;
			int colImg = j + colDiff;

			if (imgMat->width - 1< colImg)
			{
				continue;
			}

			if (imgMat->height - 1< rowImg)
			{
				continue;
			}

			int originR = combineData[rowImg * imgMat->width * 3 + colImg * 3 + 0];
			int originG = combineData[rowImg * imgMat->width * 3 + colImg * 3 + 1];
			int originB = combineData[rowImg * imgMat->width * 3 + colImg * 3 + 2];
			double aveOriginRGB = (originB + originG + originR) / 3;

			int R = data[i * widthStep + j * 3 + 0];
			int G = data[i * widthStep + j * 3 + 1];
			int B = data[i * widthStep + j * 3 + 2];
			double aveRGB = (B + G + R) / 3;

			if (aveOriginRGB < aveRGB)													//用周边的高亮像素覆盖
			{
				combineData[rowImg * imgMat->width * 3 + colImg * 3 + 0] = data[i * widthStep + j * 3 + 0];
				combineData[rowImg * imgMat->width * 3 + colImg * 3 + 1] = data[i * widthStep + j * 3 + 1];
				combineData[rowImg * imgMat->width * 3 + colImg * 3 + 2] = data[i * widthStep + j * 3 + 2];
			}
		}
	}

	cvReleaseImage(&img);
}

void CSceneDomGenerator::combineImage(GDALDataset* poDataset, std::string imgFileName, osg::Vec4d rect, osg::Vec4d maxRect)
{
	GDALAllRegister();
	GDALDataset* poBlockDataset = (GDALDataset*) GDALOpen(imgFileName.c_str(), GA_ReadOnly);

	if (poBlockDataset == NULL)
	{
		GDALClose(poBlockDataset);
		poBlockDataset = NULL;
		return;
	}

	int rowDiff = (maxRect.w() - rect.w()) / res;
	int colDiff = (rect.x() - maxRect.x()) / res;

	int totalCol = poDataset->GetRasterXSize();
	int totalRow = poDataset->GetRasterYSize();
	int totalBandCount = poDataset->GetRasterCount();

	int blockCol = poBlockDataset->GetRasterXSize();
	int blockRow = poBlockDataset->GetRasterYSize();

	if (blockCol <= 0 || blockRow <= 0)
	{
		return;
	}

	int bandCount = poBlockDataset->GetRasterCount();

	int newBlockCol = 0;
	int newBlockRow = 0;

	if (rowDiff + blockRow > totalRow)
	{
		newBlockRow = totalRow - rowDiff;
	}
	else
	{
		newBlockRow = blockRow;
	}

	if (colDiff + blockCol > totalCol)
	{
		newBlockCol = totalCol - colDiff;
	}
	else
	{
		newBlockCol = blockCol;
	}

	if (newBlockCol <= 0 || newBlockRow <= 0)
	{
		return;
	}

	uchar* pBlockData = new uchar[blockRow * blockCol * bandCount];
	poBlockDataset->RasterIO(GF_Read, 0, 0, blockCol, blockRow, pBlockData, blockCol, blockRow, GDT_Byte, bandCount, NULL, 0, 0, 0);

	uchar* pTotalBlockData = new uchar[newBlockRow * newBlockCol * bandCount];
	poDataset->RasterIO(GF_Read, colDiff, rowDiff, newBlockCol, newBlockRow, pTotalBlockData, newBlockCol, newBlockRow, GDT_Byte, bandCount, NULL, 0, 0, 0);

	for (int i = 0; i < newBlockRow; i++)
	{
		for (int j = 0; j < newBlockCol; j++)
		{
			int originR = pTotalBlockData[0 * newBlockCol * newBlockRow + i * newBlockCol + j];
			int originG = pTotalBlockData[1 * newBlockCol * newBlockRow + i * newBlockCol + j];
			int originB = pTotalBlockData[2 * newBlockCol * newBlockRow + i * newBlockCol + j];
			double aveOriginRGB = (originB + originG + originR) / 3;

			int R = pBlockData[0 * blockCol * blockRow + i * blockCol + j];
			int G = pBlockData[1 * blockCol * blockRow + i * blockCol + j];
			int B = pBlockData[2 * blockCol * blockRow + i * blockCol + j];

			double aveRGB = (B + G + R) / 3;

			if (aveOriginRGB < aveRGB)													//用周边的高亮像素覆盖
			{
				pTotalBlockData[0 * newBlockCol * newBlockRow + i * newBlockCol + j] = pBlockData[0 * blockCol * blockRow + i * blockCol + j];
				pTotalBlockData[1 * newBlockCol * newBlockRow + i * newBlockCol + j] = pBlockData[1 * blockCol * blockRow + i * blockCol + j];
				pTotalBlockData[2 * newBlockCol * newBlockRow + i * newBlockCol + j] = pBlockData[2 * blockCol * blockRow + i * blockCol + j];
			}
		}
	}

	poDataset->RasterIO(GF_Write, colDiff, rowDiff, newBlockCol, newBlockRow, pTotalBlockData, newBlockCol, newBlockRow, GDT_Byte, bandCount, NULL, 0, 0, 0);

	delete[] pBlockData;
	delete[] pTotalBlockData;

	GDALClose(poBlockDataset);
	poBlockDataset = NULL;
}

void CSceneDomGenerator::combineRepairImage(GDALDataset* poDataset, std::string repairImgFileName, osg::Vec4d rect, osg::Vec4d maxRect)
{
	GDALAllRegister();
	GDALDataset* poBlockDataset = (GDALDataset*) GDALOpen(repairImgFileName.c_str(), GA_ReadOnly);

	if (poBlockDataset == NULL)
	{
		GDALClose(poBlockDataset);
		poBlockDataset = NULL;
		return;
	}

	int rowDiff = (maxRect.w() - rect.w()) / res;
	int colDiff = (rect.x() - maxRect.x()) / res;

	int totalCol = poDataset->GetRasterXSize();
	int totalRow = poDataset->GetRasterYSize();
	int totalBandCount = poDataset->GetRasterCount();

	int blockCol = poBlockDataset->GetRasterXSize();
	int blockRow = poBlockDataset->GetRasterYSize();
	int bandCount = poBlockDataset->GetRasterCount();

	int newBlockCol = 0;
	int newBlockRow = 0;

	if (rowDiff + blockRow > totalRow)
	{
		newBlockRow = totalRow - rowDiff;
	}
	else
	{
		newBlockRow = blockRow;
	}

	if (colDiff + blockCol > totalCol)
	{
		newBlockCol = totalCol - colDiff;
	}
	else
	{
		newBlockCol = blockCol;
	}

	uchar* pBlockData = new uchar[blockRow * blockCol * bandCount];
	poBlockDataset->RasterIO(GF_Read, 0, 0, blockCol, blockRow, pBlockData, blockCol, blockRow, GDT_Byte, bandCount, NULL, 0, 0, 0);

	uchar* pTotalBlockData = new uchar[newBlockRow * newBlockCol * bandCount];
	poDataset->RasterIO(GF_Read, colDiff, rowDiff, newBlockCol, newBlockRow, pTotalBlockData, newBlockCol, newBlockRow, GDT_Byte, bandCount, NULL, 0, 0, 0);

	for (int i = 0; i < newBlockRow; i++)
	{
		for (int j = 0; j < newBlockCol; j++)
		{
			int originR = pTotalBlockData[0 * newBlockCol * newBlockRow + i * newBlockCol + j];
			int originG = pTotalBlockData[1 * newBlockCol * newBlockRow + i * newBlockCol + j];
			int originB = pTotalBlockData[2 * newBlockCol * newBlockRow + i * newBlockCol + j];
			double aveOriginRGB = (originB + originG + originR) / 3;

			if (aveOriginRGB  <= 10)												//用周边的高亮像素覆盖
			{
				pTotalBlockData[0 * newBlockCol * newBlockRow + i * newBlockCol + j] = pBlockData[0 * blockCol * blockRow + i * blockCol + j];
				pTotalBlockData[1 * newBlockCol * newBlockRow + i * newBlockCol + j] = pBlockData[1 * blockCol * blockRow + i * blockCol + j];
				pTotalBlockData[2 * newBlockCol * newBlockRow + i * newBlockCol + j] = pBlockData[2 * blockCol * blockRow + i * blockCol + j];
			}
		}
	}

	poDataset->RasterIO(GF_Write, colDiff, rowDiff, newBlockCol, newBlockRow, pTotalBlockData, newBlockCol, newBlockRow, GDT_Byte, bandCount, NULL, 0, 0, 0);

	delete[] pBlockData;
	delete[] pTotalBlockData;

	GDALClose(poBlockDataset);
	poBlockDataset = NULL;
}

void CSceneDomGenerator::combineBigRepairImage(GDALDataset* poDataset, GDALDataset* poBigRepairImageDataset, osg::Vec4d rect, osg::Vec4d bigRepairImageMaxRect,
	osg::Vec4d maxRect)
{
	int rowFinalDiff = (maxRect.w() - rect.w()) / res;																			//该范围在最终影像的位置
	int colFinalDiff = (rect.x() - maxRect.x()) / res;

	int RowBigRepairImageDiff = (bigRepairImageMaxRect.w() - rect.w()) / res;													//该范围在大影像的位置
	int ColBigRepairImageDiff = (rect.x() - bigRepairImageMaxRect.x()) / res;

	int totalCol = poDataset->GetRasterXSize();																					//总的大小
	int totalRow = poDataset->GetRasterYSize();
	int totalBandCount = poDataset->GetRasterCount();

	int bigImageCol = poBigRepairImageDataset->GetRasterXSize();																//大影影像的大小
	int bigImageRow = poBigRepairImageDataset->GetRasterYSize();

	int blockImageCol = (rect.y() - rect.x()) / res;
	int blockImageRow = (rect.w() - rect.z()) / res;

	if (bigImageCol <= 0 || bigImageRow <= 0)
	{
		return;
	}

	if (blockImageCol <= 0 || blockImageRow <= 0)
	{
		return;
	}

	int bandCount = poBigRepairImageDataset->GetRasterCount();

	//计算该块在最后影像的大小
	int newFinalBlockCol = 0;
	int newFinalBlockRow = 0;

	if (rowFinalDiff + blockImageRow > totalRow)
	{
		newFinalBlockRow = totalRow - rowFinalDiff;
	}
	else
	{
		newFinalBlockRow = blockImageRow;
	}

	if (colFinalDiff + blockImageCol > totalCol)
	{
		newFinalBlockCol = totalCol - colFinalDiff;
	}
	else
	{
		newFinalBlockCol = blockImageCol;
	}

	//计算该块在大影像的大小
	int newBigRepairImageBlockCol = 0;
	int newBigRepairImageBlockRow = 0;

	if (RowBigRepairImageDiff + blockImageRow > bigImageRow)
	{
		newBigRepairImageBlockRow = bigImageRow - RowBigRepairImageDiff;
	}
	else
	{
		newBigRepairImageBlockRow = blockImageRow;
	}

	if (ColBigRepairImageDiff + blockImageCol > bigImageCol)
	{
		newBigRepairImageBlockCol = bigImageCol - ColBigRepairImageDiff;
	}
	else
	{
		newBigRepairImageBlockCol = blockImageCol;
	}

	if (newFinalBlockCol <= 0 || newFinalBlockRow <= 0)
	{
		return;
	}

	if (newBigRepairImageBlockCol <= 0 || newBigRepairImageBlockRow <= 0)
	{
		return;
	}

	uchar* pBigRepairImageData = new uchar[newBigRepairImageBlockCol * newBigRepairImageBlockRow * bandCount];
	poBigRepairImageDataset->RasterIO(GF_Read, ColBigRepairImageDiff, RowBigRepairImageDiff, newBigRepairImageBlockCol, newBigRepairImageBlockRow, pBigRepairImageData, newBigRepairImageBlockCol, newBigRepairImageBlockRow, GDT_Byte, bandCount, NULL, 0, 0, 0);

	uchar* pTotalBlockData = new uchar[newFinalBlockRow * newFinalBlockCol * bandCount];
	poDataset->RasterIO(GF_Read, colFinalDiff, rowFinalDiff, newFinalBlockCol, newFinalBlockRow, pTotalBlockData, newFinalBlockCol, newFinalBlockRow, GDT_Byte, bandCount, NULL, 0, 0, 0);

	for (int i = 0; i < newFinalBlockRow; i++)
	{
		for (int j = 0; j < newFinalBlockCol; j++)
		{
			int originR = pTotalBlockData[0 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j];
			int originG = pTotalBlockData[1 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j];
			int originB = pTotalBlockData[2 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j];
			double aveOriginRGB = (originB + originG + originR) / 3;

			int R = pBigRepairImageData[0 * newBigRepairImageBlockCol * newBigRepairImageBlockRow + i * newBigRepairImageBlockCol + j];
			int G = pBigRepairImageData[1 * newBigRepairImageBlockCol * newBigRepairImageBlockRow + i * newBigRepairImageBlockCol + j];
			int B = pBigRepairImageData[2 * newBigRepairImageBlockCol * newBigRepairImageBlockRow + i * newBigRepairImageBlockCol + j];

			double aveRGB = (B + G + R) / 3;

			if (aveOriginRGB  <= 0 && aveRGB > 10)												//用周边的高亮像素覆盖
			{
				pTotalBlockData[0 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j] = R;
				pTotalBlockData[1 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j] = G;
				pTotalBlockData[2 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j] = B;
			}
			else if (aveOriginRGB > 0 && aveOriginRGB <= 30 && aveRGB > 30)
			{
				if (i - 1 < 0 || j - 1 < 0 || i + 1 > newFinalBlockRow - 1 || j + 1 > newFinalBlockCol - 1)
				{
					continue;
				}

				osg::Vec3d SumPixel = osg::Vec3d(0, 0, 0);
				int countPixel = 0;

				for (int k = 0; k < 3; k++)
				{
					for (int l = 0; l < 3; l++)
					{
						int pixelR = pTotalBlockData[0 * newFinalBlockCol * newFinalBlockRow + (i + k - 1) * newFinalBlockCol + j + l - 1];
						int pixelG = pTotalBlockData[1 * newFinalBlockCol * newFinalBlockRow + (i + k - 1) * newFinalBlockCol + j + l - 1];
						int pixelB = pTotalBlockData[2 * newFinalBlockCol * newFinalBlockRow + (i + k - 1) * newFinalBlockCol + j + l - 1];
						double avePixelRGB = (pixelR + pixelG + pixelB) / 3;
						
						if (avePixelRGB > 30)
						{
							SumPixel.x() += pixelR;
							SumPixel.y() += pixelG;
							SumPixel.z() += pixelB;
							countPixel++;
						}
					}
				}

				if (countPixel > 0)
				{
					double AvePixelR = SumPixel.x() / countPixel;
					double AvePixelG = SumPixel.y() / countPixel;
					double AvePixelB = SumPixel.z() / countPixel;

					if ((AvePixelB + AvePixelG + AvePixelR) > 90)
					{
						pTotalBlockData[0 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j] = AvePixelR;
						pTotalBlockData[1 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j] = AvePixelG;
						pTotalBlockData[2 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j] = AvePixelB;
					}
				}
			}
			
		}
	}

	poDataset->RasterIO(GF_Write, colFinalDiff, rowFinalDiff, newFinalBlockCol, newFinalBlockRow, pTotalBlockData, newFinalBlockCol, newFinalBlockRow, GDT_Byte, bandCount, NULL, 0, 0, 0);

	delete[] pBigRepairImageData;
	delete[] pTotalBlockData;
}

void CSceneDomGenerator::combineBigImage(GDALDataset* poDataset, GDALDataset* poBigImageDataset, osg::Vec4d rect, osg::Vec4d bigImageMaxRect, osg::Vec4d maxRect)
{
	int rowFinalDiff = (maxRect.w() - rect.w()) / res;																			//该范围在最终影像的位置
	int colFinalDiff = (rect.x() - maxRect.x()) / res;

	int RowBigImageDiff = (bigImageMaxRect.w() - rect.w()) / res;																//该范围在大影像的位置
	int ColBigImageDiff = (rect.x() - bigImageMaxRect.x()) / res;

	int totalCol = poDataset->GetRasterXSize();																					//总的大小
	int totalRow = poDataset->GetRasterYSize();
	int totalBandCount = poDataset->GetRasterCount();

	int bigImageCol = poBigImageDataset->GetRasterXSize();																		//大影影像的大小
	int bigImageRow = poBigImageDataset->GetRasterYSize();

	int blockImageCol = (rect.y() - rect.x()) / res;
	int blockImageRow = (rect.w() - rect.z()) / res;

	if (bigImageCol <= 0 || bigImageRow <= 0)
	{
		return;
	}

	if (blockImageCol <= 0 || blockImageRow <= 0)
	{
		return;
	}

	int bandCount = poBigImageDataset->GetRasterCount();

	//计算该块在最后影像的大小
	int newFinalBlockCol = 0;
	int newFinalBlockRow = 0;

	if (rowFinalDiff + blockImageRow > totalRow)
	{
		newFinalBlockRow = totalRow - rowFinalDiff;
	}
	else
	{
		newFinalBlockRow = blockImageRow;
	}

	if (colFinalDiff + blockImageCol > totalCol)
	{
		newFinalBlockCol = totalCol - colFinalDiff;
	}
	else
	{
		newFinalBlockCol = blockImageCol;
	}

	//计算该块在大影像的大小
	int newBigImageBlockCol = 0;
	int newBigImageBlockRow = 0;

	if (RowBigImageDiff + blockImageRow > bigImageRow)
	{
		newBigImageBlockRow = bigImageRow - RowBigImageDiff;
	}
	else
	{
		newBigImageBlockRow = blockImageRow;
	}

	if (ColBigImageDiff + blockImageCol > bigImageCol)
	{
		newBigImageBlockCol = bigImageCol - ColBigImageDiff;
	}
	else
	{
		newBigImageBlockCol = blockImageCol;
	}

	if (newFinalBlockCol <= 0 || newFinalBlockRow <= 0)
	{
		return;
	}

	if (newBigImageBlockCol <= 0 || newBigImageBlockRow <= 0)
	{
		return;
	}

	uchar* pBigImageData = new uchar[newBigImageBlockCol * newBigImageBlockRow * bandCount];
	poBigImageDataset->RasterIO(GF_Read, ColBigImageDiff, RowBigImageDiff, newBigImageBlockCol, newBigImageBlockRow, pBigImageData, newBigImageBlockCol, newBigImageBlockRow, GDT_Byte, bandCount, NULL, 0, 0, 0);

	uchar* pTotalBlockData = new uchar[newFinalBlockRow * newFinalBlockCol * bandCount];
	poDataset->RasterIO(GF_Read, colFinalDiff, rowFinalDiff, newFinalBlockCol, newFinalBlockRow, pTotalBlockData, newFinalBlockCol, newFinalBlockRow, GDT_Byte, bandCount, NULL, 0, 0, 0);

	for (int i = 0; i < newFinalBlockRow; i++)
	{
		for (int j = 0; j < newFinalBlockCol; j++)
		{
			int originR = pTotalBlockData[0 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j];
			int originG = pTotalBlockData[1 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j];
			int originB = pTotalBlockData[2 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j];
			double aveOriginRGB = (originB + originG + originR) / 3;

			int R = pBigImageData[0 * newBigImageBlockCol * newBigImageBlockRow + i * newBigImageBlockCol + j];
			int G = pBigImageData[1 * newBigImageBlockCol * newBigImageBlockRow + i * newBigImageBlockCol + j];
			int B = pBigImageData[2 * newBigImageBlockCol * newBigImageBlockRow + i * newBigImageBlockCol + j];

			double aveRGB = (B + G + R) / 3;

			if (aveOriginRGB < aveRGB)													//用周边的高亮像素覆盖
			{
				pTotalBlockData[0 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j] = R;
				pTotalBlockData[1 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j] = G;
				pTotalBlockData[2 * newFinalBlockCol * newFinalBlockRow + i * newFinalBlockCol + j] = B;
			}
		}
	}

	poDataset->RasterIO(GF_Write, colFinalDiff, rowFinalDiff, newFinalBlockCol, newFinalBlockRow, pTotalBlockData, newFinalBlockCol, newFinalBlockRow, GDT_Byte, bandCount, NULL, 0, 0, 0);

	delete[] pBigImageData;
	delete[] pTotalBlockData;
}

std::vector<osg::Vec4d> CSceneDomGenerator::findDOMRect(std::vector<std::string> vecCoordFileName, std::vector<std::string> vecImageFileName)
{
	//寻找所有影像范围
	GDALAllRegister();
	std::vector<osg::Vec4d> vecRect;

	for (int i = 0; i < vecCoordFileName.size(); i++)
	{
		GDALDataset* poDomDataset = (GDALDataset*) GDALOpen(vecImageFileName[i].c_str(), GA_ReadOnly);

		if (poDomDataset == NULL)
		{
			GDALClose(poDomDataset);
			poDomDataset = NULL;
			std::vector<osg::Vec4d> vecRec;
			return vecRec;
		}

		int imgHeight = poDomDataset->GetRasterYSize();
		int imgWidth = poDomDataset->GetRasterXSize();

		GDALClose(poDomDataset);
		poDomDataset = NULL;

		FILE* fpRect = fopen(vecCoordFileName[i].c_str(), "r");

		if (fpRect == NULL)
		{
			continue;
		}

		double left = 0;
		double top = 0;
		double xRot = 0;
		double yRot = 0;
		double xRes = 0;
		double yRes = 0;
		fscanf(fpRect, "%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n", &xRes, &xRot, &yRot, &yRes, &left, &top);
		double bottom = top + imgHeight * yRes;
		double right = left + imgWidth * xRes;
		vecRect.push_back(osg::Vec4d(left, right, bottom, top));
		fclose(fpRect);
	}

	return vecRect;
}

std::vector<osg::Vec4d> CSceneDomGenerator::findImageRect(std::vector<std::string> vecCoordTxtFileName)
{
	//寻找所有影像范围
	std::vector<osg::Vec4d> vecRect;

	for (int i = 0; i < vecCoordTxtFileName.size(); i++)
	{
		osg::Vec4d rect;

		if (FindTileImageRect(vecCoordTxtFileName[i], rect))
		{
			vecRect.push_back(rect);
		}
	}

	return vecRect;
}

bool CSceneDomGenerator::FindTileImageRect(std::string txtFileName, osg::Vec4d &rect)
{
	FILE* fpRect = fopen(txtFileName.c_str(), "r");

	if (fpRect == NULL)
	{
		return false;
	}

	double left = 0;
	double top = 0;
	double right = 0;
	double bottom = 0;
	double xRes = 0;
	double yRes = 0;
	fscanf(fpRect, "%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n", &left, &top, &xRes, &yRes, &right, &bottom);
	rect = osg::Vec4d(left, right, bottom, top);
	fclose(fpRect);

	return true;
}

bool CSceneDomGenerator::produceNodeDOM(osg::ref_ptr<osg::Node> loadedModel, std::string &outRectDomFileName, osg::Matrix matrix)
{
	loadedModel->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);		//设置全光源
	loadedModel->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

	osg::ref_ptr<osg::MatrixTransform> mTrans = new osg::MatrixTransform;
	mTrans->setMatrix(matrix);
	mTrans->addChild(loadedModel);

	//计算分块大小
	std::vector<CoordDouble3D> rectNode;

	if(FindMinMaxNode(loadedModel, matrix, rectNode) == false)
	{
		return false;
	}

	bool isSuccess = ProduceRectDom(mTrans, rectNode, outRectDomFileName);

	return true;
}

bool CSceneDomGenerator::ProduceRectDom(osg::ref_ptr<osg::MatrixTransform> mTrans, std::vector<CoordDouble3D> vecCoord, std::string outRectDomFileName)
{
	mTrans->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE);		//设置全光源
	mTrans->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

	std::vector<CoordDouble3D> rectNode;
	bool isSuccess = findMinMax(vecCoord, rectNode);

	if (!isSuccess)
	{
		return false;
	}

	osg::Matrix matrix = mTrans->getMatrix();
	double rectLeft = rectNode[0].x;
	double rectRight = rectNode[1].x;
	double rectBottom = rectNode[0].y;
	double rectTop = rectNode[1].y;
	double znear = rectNode[0].z;
	double zfar = rectNode[1].z;

	int width = (rectRight - rectLeft) / res + 1;								//加1 保证边缘没问题				
	int height = (rectTop - rectBottom) / res + 1;

	if (width <= 0 || height <= 0)
	{
		return false;
	}

	int ScreenWidth = GetSystemMetrics (SM_CXSCREEN); 
	int ScreenHeight= GetSystemMetrics (SM_CYSCREEN); 

	if (width > ScreenWidth || height > ScreenHeight)				//超过了屏幕的分辨率了，需要分块进行截屏
	{
		int xNum = width / ScreenWidth;
		int yNum = height / ScreenHeight;
		vecSplitImgFileName.clear();
		vecSplitCoordFileName.clear();

		//拍照
		for (int i = 0; i <= yNum; i++)
		{
			for (int j = 0; j <= xNum; j++)
			{
				//计算细分块的范围
				int newWidth, newHeight;
				double newRectLeft, newRectRight, newRectBottom, newRectTop;
				double newRectZnear = znear;
				double newRectZfar = zfar;

				if (j == xNum)
				{
					newRectLeft = rectLeft + xNum * ScreenWidth * res;
					newRectRight = rectRight;
					newWidth = (newRectRight - newRectLeft) / res;
				}
				else
				{
					newRectLeft = rectLeft + j * ScreenWidth * res;
					newRectRight = rectLeft + (j + 1) * ScreenWidth * res;
					newWidth = ScreenWidth;
				}

				if (i == yNum)
				{
					newRectBottom = rectBottom + yNum * ScreenHeight * res;
					newRectTop = rectTop;
					newHeight = (newRectTop - newRectBottom) / res;
				}
				else
				{
					newRectBottom = rectBottom + i * ScreenHeight * res;
					newRectTop = rectBottom + (i + 1) * ScreenHeight * res;
					newHeight = ScreenHeight;
				}

				bool isSuccess = produceSplitedDOM(i, j, mTrans, newWidth, newHeight, newRectLeft, newRectRight, newRectBottom, newRectTop, newRectZnear, newRectZfar, outRectDomFileName);

				if (isSuccess == false)
				{
					return false;
				}
			}
		}

		InterfaceMFCExcuteFactory IMFCExecuteFactory;
		InterfaceMFCExcute* IMFCExecute = IMFCExecuteFactory.create();
		CvMat* imgMat = cvCreateMat(height, width, CV_8UC3);
		//合并影像
		std::vector<osg::Vec4d> vecCoordRect = findImageRect(vecSplitCoordFileName);

		for (int i = 0; i < vecSplitImgFileName.size(); i++)
		{
			combineImage(imgMat, vecSplitImgFileName[i], vecCoordRect[i], osg::Vec4d(rectLeft, rectRight, rectBottom, rectTop));

			IMFCExecute->RemoveFile(vecSplitImgFileName[i]);
			IMFCExecute->RemoveFile(vecSplitCoordFileName[i]);
		}

		cvSaveImage(outRectDomFileName.c_str(), imgMat);
		cvReleaseMat(&imgMat);
		imgMat = NULL;
	}
	else
	{
		bool isSuccess = produceScreenDOM(mTrans, width, height, rectLeft, rectRight, rectBottom, rectTop, znear, 
			zfar, outRectDomFileName);

		if (!isSuccess)
		{
			return false;
		}
	}

	int posEnd = outRectDomFileName.rfind('.');
	std::string outFileTxt = outRectDomFileName.substr(0, posEnd) + ".txt";

	//输出坐标
	FILE* fpOut = fopen(outFileTxt.c_str(), "w");
	fprintf(fpOut, "%lf\n%lf\n%lf\n%lf%\n%lf\n%lf\n", rectLeft, rectTop, res, res, rectRight, rectBottom);
	fclose(fpOut);

	return true;
};

bool CSceneDomGenerator::produceScreenDOM(osg::ref_ptr<osg::MatrixTransform> node, int newWidth, int newHeight, double newRectLeft, double newRectRight, 
	double newRectBottom, double newRectTop, double newRectZnear, double newRectZfar, std::string newOutFileName)
{
	osgOffScreenViewer viewer;
	osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	viewer.setCamera(camera.get());
	// Set the final SceneData to show
	viewer.setSceneData(node.get());
	// Realize GUI
	viewer.realize();

	// Build matrix for computing target vector
	camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera->setViewport(0, 0, newWidth, newHeight);

	osg::Vec3d eye(0, 0, newRectZfar + 1);
	osg::Vec3d up(0, 1, 0);
	osg::Vec3d center(0, 0, newRectZnear);
	camera->setViewMatrixAsLookAt(eye, center, up);


	camera->setProjectionMatrixAsOrtho(newRectLeft, newRectRight, newRectBottom, newRectTop, newRectZnear, newRectZfar);			//计算正射投影范围
	
	camera->setFinalDrawCallback(new WindowCaptureCallback(GL_BACK, newOutFileName, 1, newWidth, newHeight));						//获取分块影像	
	viewer.renderingTraversals();
	return true;
}

bool CSceneDomGenerator::produceSpecificScreenDOM(osg::ref_ptr<osg::MatrixTransform> node, int newWidth, int newHeight, double newRectLeft, double newRectRight, 
	double newRectBottom, double newRectTop, double newRectZnear, double newRectZfar, osg::Matrix viewMatrix,  std::string newOutFileName)
{
	osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	osgOffScreenViewer viewer;
	viewer.setCamera(camera.get());
	// Set the final SceneData to show
	viewer.setSceneData(node.get());
	// Realize GUI
	viewer.realize();

	// Build matrix for computing target vector
	camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera->setViewport(0, 0, newWidth, newHeight);
	camera->setProjectionMatrixAsOrtho(newRectLeft, newRectRight, newRectBottom, newRectTop, newRectZnear, newRectZfar);			//计算正射投影范围
	camera->setViewMatrix(viewMatrix);
	osg::Vec3f eye, center, up;
	camera->getViewMatrixAsLookAt(eye, center, up);

	char strMsg[MAX_PATH];
	sprintf_s(strMsg, "eye:%f %f %f center:%f %f %f up:%f %f %f\n", eye.x(), eye.y(), eye.z(), center.x(), center.y(), center.z(), up.x(), up.y(), up.z());
	_logop(strMsg);

	camera->setFinalDrawCallback(new WindowCaptureCallback(GL_BACK, newOutFileName, 1, newWidth, newHeight));						//获取分块影像	
	viewer.renderingTraversals();
	return true;
}

bool CSceneDomGenerator::produceSplitedDOM(int i, int j, osg::ref_ptr<osg::MatrixTransform> mTrans, int newWidth, int newHeight, double newRectLeft, 
	double newRectRight, double newRectBottom, double newRectTop, double newRectZnear, double newRectZfar, std::string outRectDomFileName)
{
	stringstream ss;
	std::string rowStr;
	ss<<i;
	ss>>rowStr;

	stringstream ss1;
	std::string colStr;
	ss1<<j;
	ss1>>colStr;

	int pos = outRectDomFileName.rfind('\\');
	std::string newOutFileName = outRectDomFileName.substr(0, pos) + "\\" + "_" + rowStr + "_" + colStr + ".jpg";

	//截图
	osg::ref_ptr<osg::MatrixTransform> node = dynamic_cast<osg::MatrixTransform*>(mTrans->clone(osg::CopyOp::DEEP_COPY_ALL));

	if(produceScreenDOM(node, newWidth, newHeight, newRectLeft, newRectRight, newRectBottom, newRectTop, newRectZnear, newRectZfar, newOutFileName))
	{
		//输出坐标
		std::string outFileTxt = outRectDomFileName.substr(0, pos) + "\\" + "_" + rowStr + "_" + colStr + ".txt";
		FILE* fpOut = fopen(outFileTxt.c_str(), "w");
		fprintf(fpOut, "%lf\n%lf\n%lf\n%lf%\n%lf\n%lf\n", newRectLeft, newRectTop, res, res, newRectRight, newRectBottom);
		fclose(fpOut);

		vecSplitImgFileName.push_back(newOutFileName);
		vecSplitCoordFileName.push_back(outFileTxt);

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

bool CSceneDomGenerator::produceSpecificSplitedDOM(int i, int j, osg::ref_ptr<osg::MatrixTransform> mTrans, int newWidth, int newHeight, double newRectLeft, 
	double newRectRight, double newRectBottom, double newRectTop, double newRectZnear, double newRectZfar, osg::Matrix viewMatrix, std::string outFileName)
{
	stringstream ss;
	std::string rowStr;
	ss<<i;
	ss>>rowStr;

	stringstream ss1;
	std::string colStr;
	ss1<<j;
	ss1>>colStr;
	int pos = outFileName.rfind('\\');
	std::string newOutFileName = outFileName.substr(0, pos) + "\\" + "_" + rowStr + "_" + colStr + ".jpg";

	//截图
	osg::ref_ptr<osg::MatrixTransform> node = dynamic_cast<osg::MatrixTransform*>(mTrans->clone(osg::CopyOp::DEEP_COPY_ALL));

	if(produceSpecificScreenDOM(node, newWidth, newHeight, newRectLeft, newRectRight, newRectBottom, newRectTop, newRectZnear, newRectZfar, viewMatrix, newOutFileName))
	{
		//输出坐标
		std::string outFileTxt =  outFileName.substr(0, pos) + "\\" + "_" + rowStr + "_" + colStr + ".txt";
		FILE* fpOut = fopen(outFileTxt.c_str(), "w");
		fprintf(fpOut, "%lf\n%lf\n%lf\n%lf%\n%lf\n%lf\n", newRectLeft, newRectTop, res, res, newRectRight, newRectBottom);
		fclose(fpOut);

		vecSplitImgFileName.push_back(newOutFileName);
		vecSplitCoordFileName.push_back(outFileTxt);

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

bool CSceneDomGenerator::FindMinMaxNode(osg::ref_ptr<osg::Node> node, osg::Matrix matrix, std::vector<CoordDouble3D> &minMaxCoord)
{		
	osg::ref_ptr<osg::Vec3Array> extracted_verts;
	InterfaceOsgFinderFactory IFactory;
	InterfaceOsgFinder* IOsgFinder = IFactory.create();
	IOsgFinder->ExtractVerts(node, extracted_verts);

	std::vector <osg::Vec3>::iterator iter = extracted_verts.get()->begin();
	std::vector<CoordDouble3D> coord;

	for (int i = 0; i < extracted_verts->size(); i++)
	{
		double x = iter->x();
		double y = iter->y();
		double z = iter->z();
		osg::Vec3d pt(x, y, z);
		pt = pt * matrix;
		coord.push_back(CoordDouble3D(pt.x(), pt.y(), pt.z()));
		iter++;
	}

	bool isSuccess = findMinMax(coord, minMaxCoord);

	return isSuccess;
}

bool CSceneDomGenerator::findMinMax(std::vector<CoordDouble3D> vecMinMax, std::vector<CoordDouble3D> &coord)
{
	int size = vecMinMax.size();

	if (size <= 0)
	{
		return false;
	}

	double xMin = vecMinMax[0].x;
	double yMin = vecMinMax[0].y;
	double zMin = vecMinMax[0].z;
	double xMax = vecMinMax[0].x;
	double yMax = vecMinMax[0].y;
	double zMax = vecMinMax[0].z;

	for (int i = 0; i < vecMinMax.size(); i++)
	{
		if (xMin > vecMinMax[i].x)
		{
			xMin =  vecMinMax[i].x;
		}

		if (yMin >  vecMinMax[i].y)
		{
			yMin =  vecMinMax[i].y;
		}

		if (zMin >  vecMinMax[i].z)
		{
			zMin =  vecMinMax[i].z;
		}

		if (xMax <  vecMinMax[i].x)
		{
			xMax =  vecMinMax[i].x;
		}

		if (yMax <  vecMinMax[i].y)
		{
			yMax =  vecMinMax[i].y;
		}

		if (zMax < vecMinMax[i].z)
		{
			zMax = vecMinMax[i].z;
		}
	}

	coord.push_back(CoordDouble3D(xMin, yMin, zMin));
	coord.push_back(CoordDouble3D(xMax, yMax, zMax));

	char strMsg[MAX_PATH];
	sprintf_s(strMsg, "xMin:%lf yMin:%lf zMin:%lf xMax:%lf yMax:%lf zMax:%lf\n", xMin, yMin, zMin, xMax, yMax, zMax);
	_logop(strMsg);
	return TRUE;
}

std::vector<std::vector<std::string>> CSceneDomGenerator::SeparateSceneTile(std::vector<std::string> vecChildFilePath)
{
	int rowMin = 999999, rowMax = -9999, colMin = 999999, colMax = -99999;

	for (int i = 0; i < vecChildFilePath.size(); i++)
	{
		int rPos = vecChildFilePath[i].rfind('_+');
		int pos = vecChildFilePath[i].find('_+');
		int length = vecChildFilePath[i].length() - 1;
		std::string rowStr = vecChildFilePath[i].substr(pos + 2, rPos);
		std::string colStr = vecChildFilePath[i].substr(rPos + 2, length);
		int row = atoi(rowStr.c_str());
		int col = atoi(colStr.c_str());

		if (rowMin > row)
		{
			rowMin = row;
		}

		if (rowMax < row)
		{
			rowMax = row;
		}

		if (colMin > col)
		{
			colMin = col;
		}

		if (colMax < col)
		{
			colMax = col;
		}

		double blockPercent = (double)(i + 1) / (double) vecChildFilePath.size();

		int percent =  blockPercent * 20 + 150;
		ISigCallBack->EmitSigSetProgressFun(percent);
	}

	int rowNum = (int) ceil((double)(rowMax - rowMin + 1) / (double)xTileNum); 
	int colNum = (int) ceil((double)(colMax - colMin + 1) / (double)yTileNum);
	int groupNum = rowNum * colNum;

	std::vector<std::vector<std::string>> vecGroupFilePath;

	for (int i = 0; i < groupNum; i++)
	{
		std::vector<std::string> groupFilePath;
		vecGroupFilePath.push_back(groupFilePath);
	}

	for (int i = 0; i < vecChildFilePath.size(); i++)
	{
		int rPos = vecChildFilePath[i].rfind('_+');
		int pos = vecChildFilePath[i].find('_+');
		int length = vecChildFilePath[i].length() - 1;
		std::string rowStr = vecChildFilePath[i].substr(pos + 2, rPos);
		std::string colStr = vecChildFilePath[i].substr(rPos + 2, length);
		int row = atoi(rowStr.c_str());
		int col = atoi(colStr.c_str());
		int rowIndex = (row - rowMin) / xTileNum;
		int colIndex = (col - colMin) / yTileNum;
		vecGroupFilePath[rowIndex * colNum + colIndex].push_back(vecChildFilePath[i]);

		double blockPercent = (double)(i + 1) / (double) vecChildFilePath.size();

		int percent = blockPercent * 30 + 170;
		ISigCallBack->EmitSigSetProgressFun(percent);
	}

	return vecGroupFilePath;
}

bool CSceneDomGenerator::GenerateDOMForSingleModel(osg::ref_ptr<osg::MatrixTransform> mTrans, std::string outputDomFileName, bool isCombine)
{
	int pos = outputDomFileName.rfind('.');
	std::string outputCoordFileName = outputDomFileName.substr(0, pos) + ".tfw";
	bool isSuccess = StartGeneratingDOM(outputDomFileName, outputCoordFileName, mTrans,isCombine);

	return isSuccess;
}

bool CSceneDomGenerator::GenerateMultiDomForSingleModel(std::vector<osg::MatrixTransform*> vecTransNode)
{
	std::vector<std::string> vecImageFileName;
	std::vector<std::string> vecCoordFileName;
	bool isSuccess = false;

	//获取block目录
	int pos = sceneFileName.rfind('\\');
	std::string sceneFilePath = sceneFileName.substr(0, pos);

	std::vector<std::string> vecSceneFilePath;

	//生成多个block的DOM
	for (int i = 0; i < vecTransNode.size(); i++)
	{
		osg::ref_ptr<osg::Vec3Array> extracted_verts;
		InterfaceOsgFinderFactory IOsgFinderFactory;
		InterfaceOsgFinder* IOsgFinder = IOsgFinderFactory.create();
		IOsgFinder->ExtractVerts(vecTransNode[i], extracted_verts);

		if (extracted_verts->size() > 0)
		{
			char strMsg[MAX_PATH];
			sprintf_s(strMsg, "正在生成DOM...当前数据为第%d块，共%d块，请不要切换屏幕的分辨率，计算机设置从不休眠。", i + 1, vecTransNode.size());
			ISigCallBack->EmitSigShowOpsHint(strMsg);
			ISigCallBack->EmitSigStartProgressFun();

			osg::ref_ptr<osg::MatrixTransform> mTrans = vecTransNode[i];

			std::string folderName;
			IOsgFinder->FindTransFolderName(mTrans, folderName);

			if (folderName.empty())
			{
				continue;
			}

			std::string blockSceneFilePath = sceneFilePath + "\\" + folderName + "\\";

			std::string sceneImageFileName = blockSceneFilePath + "TDOM.tif";
			std::string sceneCoordFileName = blockSceneFilePath + "TDOM.tfw";
			//对单个block生成DOM
			if (GenerateDOMForSingleModel(mTrans, sceneImageFileName, true))
			{
				//保存影像和范围
				vecImageFileName.push_back(sceneImageFileName);
				vecCoordFileName.push_back(sceneCoordFileName);
				isSuccess = true;

				if (i < vecTransNode.size() - 1)//不是最后一块数据，结束进度条
				{
					ISigCallBack->EmitSigEndProgressFun();
				}
			}
		}

		vecSceneFilePath.push_back(sceneFilePath);
	}

	ISigCallBack->EmitSigShowOpsHint("正在合并数据...请稍等");

	if (isSuccess)
	{
		isSuccess = OutputImageAndCoords(vecTransNode, vecSceneFilePath, vecImageFileName, vecCoordFileName);
	}

	return isSuccess;
}

bool CSceneDomGenerator::OutputImageAndCoords(std::vector<osg::MatrixTransform*> vecTransNode, std::vector<std::string> vecSceneFilePath, 
	std::vector<std::string> vecImageFileName, std::vector<std::string> vecCoordFileName)
{
	if (vecImageFileName.size() == 0 || vecCoordFileName.size() == 0)
	{
		return false;
	}

	std::vector<osg::Vec4d> vecRect = findDOMRect(vecCoordFileName, vecImageFileName);	//获取image范围

	if (vecRect.size() == 0)
	{
		return false;
	}

	osg::Vec4d maxRect = findMaxRect(vecRect);										    //找到最大范围

	//计算范围创建影像
	int row = (maxRect.w() - maxRect.z()) / res;
	int col = (maxRect.y() - maxRect.x()) / res;

	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8","NO");
	char** ppszOptions = CSLSetNameValue(NULL, "BIGTIFF", "IF_NEEDED"); //配置图像信息	
	int nBandCount = 3;
	GDALDriver *pDriver = GetGDALDriverManager()->GetDriverByName("GTIFF"); //图像驱动
	GDALDataset* poDataset = pDriver->Create(outputFileName.c_str(), col, row, nBandCount, GDT_Byte, ppszOptions);

	double blockXSize = 10000 * res;
	double blockYSize = 10000 * res;

	for (int i = 0; i < vecRect.size(); i++)
	{
		GDALDataset* poBlockDataset = (GDALDataset*) GDALOpen(vecImageFileName[i].c_str(), GA_ReadOnly);

		if (poBlockDataset == NULL)
		{
			GDALClose(poBlockDataset);
			poBlockDataset = NULL;
			continue;
		}

		double left = vecRect[i].x(); double right = vecRect[i].y(); double bottom = vecRect[i].z(); double top = vecRect[i].w();
		int blockXNum = (right - left) / blockXSize + 1;
		int blockYNum = (top - bottom) / blockYSize + 1;

		for (int j = 0; j < blockXNum; j++)
		{
			for (int k = 0; k < blockYNum; k++)
			{
				double blockLeft = j * blockXSize + left;
				double blockRight = 0;
				double blockBottom = k * blockYSize + bottom;
				double blockTop = 0;

				if (j != blockXNum - 1)
				{
					blockRight = blockLeft + blockXSize;
				}
				else
				{
					blockRight = right;
				}

				if (k != blockYNum - 1)
				{
					blockTop = blockBottom + blockYSize;
				}
				else
				{
					blockTop = top;
				}

				osg::Vec4d blockRect(blockLeft, blockRight, blockBottom, blockTop);
				combineBigImage(poDataset, poBlockDataset, blockRect, vecRect[i], maxRect);
			}
		}

		GDALClose(poBlockDataset);
		poBlockDataset = NULL;
	}

	//修补空洞

	for (int i = 0; i < vecTransNode.size(); i++)
	{
		osg::ref_ptr<osg::Vec3Array> extracted_verts;
		InterfaceOsgFinderFactory IOsgFinderFactory;
		InterfaceOsgFinder* IOsgFinder = IOsgFinderFactory.create();
		IOsgFinder->ExtractVerts(vecTransNode[i], extracted_verts);

		if (extracted_verts->size() == 0)
		{
			continue;
		}

		StartRepairDom(vecTransNode[i], poDataset, vecSceneFilePath[i], maxRect);
	}

	double  adfGeoTransform[6] = {0};
	adfGeoTransform[0] = maxRect.x();
	adfGeoTransform[3] = maxRect.w();
	adfGeoTransform[1] = res;
	adfGeoTransform[5] = -res;

	poDataset->SetGeoTransform(adfGeoTransform);
	GDALClose(poDataset);
	poDataset = NULL;

	int pos = outputFileName.rfind('.');
	std::string coordFileName = outputFileName.substr(0, pos) + ".tfw";
	double xRot = 0;
	double yRot = 0;
	FILE* fpCoord = fopen(coordFileName.c_str(), "w");
	fprintf(fpCoord, "%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n", res, xRot, yRot, -res, maxRect.x(), maxRect.w());
	fclose(fpCoord);

	return true;
}

void CSceneDomGenerator::InitDomGeneratorPara(std::string SceneFileName, std::string OutputFileName, double Res, int XTileNum, int YTileNum, 
	InterfaceSigEventCallBack* iSigCallBack)
{
	sceneFileName = SceneFileName;
	outputFileName = OutputFileName;
	res = Res;
	xTileNum = XTileNum;
	yTileNum = YTileNum;
	ISigCallBack = iSigCallBack;
}