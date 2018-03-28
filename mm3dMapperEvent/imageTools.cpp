#include "stdafx.h"
#include "imageTools.h"
#include "GDAL_PLUS.h"
#include <fstream>
#include "cvLine.h"

IplImage* CImageTool::combineImage(IplImage* image1, IplImage*image2)
{
	int w = image1->width;
	int h = image1->height;

	IplImage* M = cvCreateImage(cvSize(w, h), 8, 1);
	cvZero(M);
	int Mwidthstep = M->widthStep;
	for (size_t i = 0; i < w; ++i)
	{
		for (size_t j = 0; j < h; ++j)
		{
			if (image1->imageData[j * Mwidthstep + i] != 0 || image2->imageData[j * Mwidthstep + i] != 0)
			{
				image1->imageData[j * Mwidthstep + i] = 255;
			}
		}
	}
	//cvReleaseImage(&image1);
	//cvReleaseImage(&image2);
	return NULL;
}

int CImageTool::ReadImageToBuff(const char* inputFileName, double *&pImageBuf, int&width, int& height) throw()
{
	GDALAllRegister();         //利用GDAL读取图片，先要进行注册  
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");   //设置支持中文路径  

														 //准备读取图片  
	GDALDataset *ReadDataSet = (GDALDataset*)GDALOpen(inputFileName, GA_ReadOnly);

	width = ReadDataSet->GetRasterXSize();
	height = ReadDataSet->GetRasterYSize();
	int bandCount = ReadDataSet->GetRasterCount();

	pImageBuf = NULL;
	pImageBuf = new(std::nothrow) double[width*height*bandCount];
	if (NULL == pImageBuf)
	{
		delete ReadDataSet; ReadDataSet = NULL;
		return -1;
	}

	if (ReadDataSet->RasterIO(GF_Read, 0, 0, width, height, pImageBuf, width, height, GDT_Float64, bandCount, NULL, 0, 0, 0) == CE_Failure)
	{
		delete ReadDataSet; ReadDataSet = NULL;
		delete[] pImageBuf; pImageBuf = NULL;
		return -4;
	}
	delete ReadDataSet; ReadDataSet = NULL;
	return 0;
}

bool CImageTool::getCoordInfo(const char* inputFileName, double& leftX, double& leftY, double& resX, double& resY)
{
	fstream file(inputFileName);
	if (!file)
	{
		return false;
	}
	string line;

	getline(file, line);
	resX = atof(line.c_str());
	getline(file, line);
	getline(file, line);
	getline(file, line);
	resY = atof(line.c_str());
	getline(file, line);
	leftX = atof(line.c_str());
	getline(file, line);
	leftY = atof(line.c_str());

	file.close();
	return true;
}

void CImageTool::calcIntersectPointOfLine(Line& l1, Line& l2, osg::Vec3d& pt)
{
	pt.x() = (l2.b - l1.b) / (l1.k - l2.k);
	pt.y() = l1.k*pt.x() + l1.b;
}

bool CImageTool::checkColor(osg::Vec3d& c1, osg::Vec3d& c2, double err)
{
	double dis = distance2(c1, c2);
	if (dis <= sqrt(3) * 255 * (1 - err))
	{
		return true;
	}
	return false;
}

IplImage* CImageTool::EqualizeHistColorImage(IplImage *pImage)
{
	IplImage *pEquaImage = cvCreateImage(cvGetSize(pImage), pImage->depth, 3);

	// 原图像分成各通道后再均衡化,最后合并即彩色图像的直方图均衡化
	const int MAX_CHANNEL = 4;
	IplImage *pImageChannel[MAX_CHANNEL] = { NULL };

	int i;
	for (i = 0; i < pImage->nChannels; i++)
		pImageChannel[i] = cvCreateImage(cvGetSize(pImage), pImage->depth, 1);

	cvSplit(pImage, pImageChannel[0], pImageChannel[1], pImageChannel[2], pImageChannel[3]);

	for (i = 0; i < pImage->nChannels; i++)
		cvEqualizeHist(pImageChannel[i], pImageChannel[i]);

	cvMerge(pImageChannel[0], pImageChannel[1], pImageChannel[2], pImageChannel[3], pEquaImage);

	for (i = 0; i < pImage->nChannels; i++)
		cvReleaseImage(&pImageChannel[i]);

	return pEquaImage;
}

double CImageTool::getHeightInDSM(double* dsmImage, int x, int y, int widStep)
{
	return dsmImage[x + y*widStep];
}

void CImageTool::searchByHeight(IplImage* contourImage,double* dsmData, Coord& coord, double seedH, double heiErr, std::vector<osg::Vec2i>& vecPoints)
{
	std::vector<osg::Vec2i> vec1;
	double mh;
	IplImage* M = cvCreateImage(cvSize(coord.width, coord.height), 8, 1);
	IplImage* M1 = cvCreateImage(cvSize(coord.width, coord.height), 8, 1);

	cvZero(M);
	cvZero(M1);
	int Mwidthstep = M->widthStep;

	M1->imageData[coord.y * Mwidthstep + coord.x] = 255;
	std::vector<osg::Vec2i> vecTemp_, vecTemp;
	vecTemp_.push_back(osg::Vec2i(coord.x, coord.y));

	while (vecTemp_.size() > 0)
	{
		vecTemp.assign(vecTemp_.begin(), vecTemp_.end());
		vecTemp_.clear();
		for (int i = 0; i < vecTemp.size(); ++i)
		{
			//获取种子点周围8个点
			std::vector<osg::Vec2i> vec;
			vec.push_back(osg::Vec2i(vecTemp[i].x() - 1, vecTemp[i].y() - 1));
			vec.push_back(osg::Vec2i(vecTemp[i].x(), vecTemp[i].y() - 1));
			vec.push_back(osg::Vec2i(vecTemp[i].x() + 1, vecTemp[i].y() - 1));
			vec.push_back(osg::Vec2i(vecTemp[i].x() - 1, vecTemp[i].y()));
			vec.push_back(osg::Vec2i(vecTemp[i].x() + 1, vecTemp[i].y()));
			vec.push_back(osg::Vec2i(vecTemp[i].x() - 1, vecTemp[i].y() + 1));
			vec.push_back(osg::Vec2i(vecTemp[i].x(), vecTemp[i].y() + 1));
			vec.push_back(osg::Vec2i(vecTemp[i].x() + 1, vecTemp[i].y() + 1));

			for (int j = 0; j < vec.size(); ++j)
			{
				if (vec[j].x() >= 0 && vec[j].x() < coord.width && vec[j].y() >= 0 && vec[j].y() < coord.height)//如果没有出界
				{
					if (M->imageData[vec[j].y() * Mwidthstep + vec[j].x()] == 0)//如果该点没有搜索过
					{
						M->imageData[vec[j].y() * Mwidthstep + vec[j].x()] = 1;//标记为已搜索
																			   //获取该点颜色
						mh = getHeightInDSM(dsmData,vec[j].x(), vec[j].y(), Mwidthstep);

						if (abs(mh - seedH) <= heiErr)
						{
							vecTemp_.push_back(vec[j]);
							if (M->imageData[vec[j].y() * Mwidthstep + vec[j].x()] != 2)
							{
								M->imageData[vec[j].y() * Mwidthstep + vec[j].x()] = 2;//标记为已保存
								M1->imageData[vec[j].y() * Mwidthstep + vec[j].x()] = 255;
							}
						}
					}
				}
			}
		}
	}



	//声明IplImage指针,计算轮廓  
	IplImage* pImg = NULL;
	IplImage* pContourImg = NULL;
	CvMemStorage * storage = cvCreateMemStorage(0);
	CvSeq * contour = 0;

	combineImage(contourImage, M1);
	int Num = cvFindContours(contourImage, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	for (; contour != 0; contour = contour->h_next)
	{
		for (int i = 0; i < contour->total; i++)
		{
			CvPoint* p = (CvPoint*)cvGetSeqElem(contour, i);
			vecPoints.push_back(osg::Vec2i(p->x, p->y));
		}
	}
	cvReleaseImage(&pImg);
	cvReleaseImage(&pContourImg);
	cvReleaseImage(&M1);
	cvReleaseImage(&M);
}


void CImageTool::pointsToContour3D(osg::Group* mRoot,std::vector<osg::Vec3d>& vecPt3D,int N, std::vector<osg::Vec3d>& vecResult)
{
	//抽稀
	std::vector<osg::Vec3d> vecSample;
	double H = vecPt3D[1].z();
	vecResult.clear();
	osg::Vec3d pt = vecPt3D.front();
	vecResult.push_back(pt);

	while (vecPt3D.size() >= N)
	{
		if (vecSample.size() == N)
		{
			vecSample.erase(vecSample.begin(), vecSample.begin() + N / 2);
		}
		if (vecSample.size() > N)
		{
			vecSample.erase(vecSample.begin(), vecSample.begin() + N);
		}
		for (size_t i = 0; i < N; i++)
		{
			vecSample.push_back(vecPt3D.back());
			vecPt3D.pop_back();
		}

		float a, b, c, d;
		CVLine(vecSample, a, b, c, d);
		double k = b / a;
		double B = d - k*c;
		Line l1(k, B);
		Line l3(-1 / k, vecSample.back().y() - (-1 / k)*vecSample.back().x());
		osg::Vec3d pt2(0, 0, H);
		calcIntersectPointOfLine(l1, l3, pt2);
		osg::Vec3d  p2;
		osg::ref_ptr<osgUtil::LineSegmentIntersector> ls1 = new osgUtil::LineSegmentIntersector(osg::Vec3d(pt2.x(), pt2.y(), pt2.z() + 1000),
			osg::Vec3d(pt2.x(), pt2.y(), pt2.z() - 1000));
		// 创建一个IV
		osgUtil::IntersectionVisitor iv1(ls1);
		// 把线段添加到IV中
		mRoot->accept(iv1);
		if (ls1.valid() && ls1->containsIntersections())
		{
			p2 = (ls1->getFirstIntersection().getWorldIntersectPoint());
			if (pt.z() != 0 && p2.z() != 0)
			{
				vecResult.push_back(p2);
				pt = p2;
			}
		}
	}
}

void CImageTool::searchByColor(IplImage* contourImage, IplImage* image, double* dsmData, Coord& coord, osg::Vec3d seedColor,  double heiErr, std::vector<osg::Vec2i>& vecPoints)
{
	std::vector<osg::Vec2i> vec1;
	int w = image->width;
	int h = image->height;

	uchar* ptr;
	IplImage* M = cvCreateImage(cvSize(w, h), 8, 1);
	IplImage* M1 = cvCreateImage(cvSize(w, h), 8, 1);

	cvZero(M);
	cvZero(M1);
	int Mwidthstep = M->widthStep;

	M1->imageData[coord.y * Mwidthstep + coord.x] = 255;
	std::vector<osg::Vec2i> vecTemp_, vecTemp;
	vecTemp_.push_back(osg::Vec2i(coord.x, coord.y));

	while (vecTemp_.size() > 0)
	{
		vecTemp.assign(vecTemp_.begin(), vecTemp_.end());
		vecTemp_.clear();
		for (int i = 0; i < vecTemp.size(); ++i)
		{
			//获取种子点周围4个点
			std::vector<osg::Vec2i> vec;
			//vec.push_back(osg::Vec2i(vecTemp[i].x() - 1, vecTemp[i].y() - 1));
			vec.push_back(osg::Vec2i(vecTemp[i].x(), vecTemp[i].y() - 1));
			//vec.push_back(osg::Vec2i(vecTemp[i].x() + 1, vecTemp[i].y() - 1));
			vec.push_back(osg::Vec2i(vecTemp[i].x() - 1, vecTemp[i].y()));
			vec.push_back(osg::Vec2i(vecTemp[i].x() + 1, vecTemp[i].y()));
			//vec.push_back(osg::Vec2i(vecTemp[i].x() - 1, vecTemp[i].y() + 1));
			vec.push_back(osg::Vec2i(vecTemp[i].x(), vecTemp[i].y() + 1));
			//vec.push_back(osg::Vec2i(vecTemp[i].x() + 1, vecTemp[i].y() + 1));

			for (int j = 0; j < vec.size(); ++j)
			{
				if (vec[j].x() >= 0 && vec[j].x() < w&&vec[j].y() >= 0 && vec[j].y() < h)//如果没有出界
				{
					if (M->imageData[vec[j].y() * Mwidthstep + vec[j].x()] == 0)//如果该点没有搜索过
					{
						M->imageData[vec[j].y() * Mwidthstep + vec[j].x()] = 1;//标记为已搜索
																			   //获取该点颜色
						ptr = cvPtr2D(image, vec[j].y(), vec[j].x(), NULL);
						
						if (checkColor(seedColor, osg::Vec3d(ptr[0], ptr[1], ptr[2]), 0.9) &&
							abs(dsmData[vecTemp[i].x() + vecTemp[i].y()*Mwidthstep] - dsmData[vec[j].x() + vec[j].y()*Mwidthstep]) < heiErr)
						{
							vecTemp_.push_back(vec[j]);
							if (M->imageData[vec[j].y() * Mwidthstep + vec[j].x()] != 2)
							{
								M->imageData[vec[j].y() * Mwidthstep + vec[j].x()] = 2;//标记为已保存
								M1->imageData[vec[j].y() * Mwidthstep + vec[j].x()] = 255;
							}
						}
					}
				}
			}
		}
	}
	//声明IplImage指针  
	IplImage* pImg = NULL;
	IplImage* pContourImg = NULL;
	CvMemStorage * storage = cvCreateMemStorage(0);
	CvSeq * contour = 0;
	
	combineImage(contourImage, M1);
	int Num = cvFindContours(contourImage, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	for (; contour != 0; contour = contour->h_next)
	{
		for (int i = 0; i < contour->total; i++)
		{
			CvPoint* p = (CvPoint*)cvGetSeqElem(contour, i);
			vecPoints.push_back(osg::Vec2i(p->x, p->y));
		}
	}
	cvReleaseImage(&pImg);
	cvReleaseImage(&pContourImg);
	cvReleaseImage(&M1);
	cvReleaseImage(&M);
}