
#include "stdafx.h"
#include "MeasureDistance.h"

//大津法求阈值
int OtsuAlgThreshold(const Mat image);
//steger中心线计算卷积
float NewGaussianConv2D(const IplImage* img, const CvMat* kernel, int row, int col);

double TRANS_THRESHOLD = 0;
double EIGENVALUE_THRESHOLD = 0.001f;
int COUNT_THRES = 8;//园域内剔除点个数
int COUNT_THRES2 = 10;//剔除长度

MeasureDistance::MeasureDistance()
{
	R_L = atof(GetPropertyByName("layer position", "R_L").c_str());
	R_L2 = atof(GetPropertyByName("layer position", "R_L2").c_str());
	MEAS_MAX_Z =atof(GetPropertyByName("Position", "MEAS_MAX_Z ").c_str());
	MEAS_VALI_MIN_Z = atof(GetPropertyByName("Position", "MEAS_VALI_MIN_Z").c_str());
	MEAS_VALI_MAX_Z =atof(GetPropertyByName("Position", "MEAS_VALI_MAX_Z ").c_str());
	MEAS_VALI_MIN_Y =atof(GetPropertyByName("Position", "MEAS_VALI_MIN_Y").c_str());
	MEAS_VALI_MAX_Y =atof(GetPropertyByName("Position", "MEAS_VALI_MAX_Y ").c_str());
	Test_Calibrate = atof(GetPropertyByName("Position", "Test_Calibrate ").c_str());
}

MeasureDistance::~MeasureDistance()
{
}

float MeasureDistance::getThresh(vector<float>& vData)
{
	if (vData.empty())
	{
		return 0;
	}

	float fMeanAll = 0.0;
	for (int i = 0; i < vData.size(); ++i)
	{
		fMeanAll = fMeanAll + vData[i];
	}
	fMeanAll = fMeanAll / vData.size();

	float fMeanMin = 0.0;
	int nMinNum = 0;
	for (int i = 0; i < vData.size(); ++i)
	{
		if (vData[i] < fMeanAll)
		{
			fMeanMin = fMeanMin + vData[i];
			nMinNum++;
		}
	}
	fMeanMin = fMeanMin / nMinNum;
	//return fMeanAll; //find error. 
	return fMeanMin;
}

bool MeasureDistance::DetectLinerCenterHor(cv::Mat &src, cv::Mat &dst, vector<cv::Point2f>& vSubCenterPoint)
{
	//图像预处理;
	vSubCenterPoint.clear();
	cv::Mat Image, imagegray;
	Image = src.clone();
	//Image=cv::imread("1.bmp",1);
	const int Width = Image.cols;
	const int Height = Image.rows;
	int Ilow = 5;   //当前灰度阈值
	int Ihigh = 50; //相邻灰度差阈值
	int BandWidth = 15; // 1/2光带宽度
	int thresWidth = BandWidth * 2; //光带宽度检测阈值
	if (src.channels() == 3)
	{
		cvtColor(src, imagegray, CV_BGR2GRAY);
	}
	else
	{
		imagegray = src.clone();
	}

	medianBlur(imagegray, imagegray, 3);

	//提取光带的粗略中心位置;
	//首先，1*3滑动窗口，求灰度和
	int nWindowSize = 3;
	vector<int> Bool(Height, 0);
	//int *Bool=new int [Height];//数组用来判断该行是否有激光线
	//double **GrayAdd = new double*[Height];
	vector<vector<int> > GrayAdd(Height);
	for (int i = 0; i < Height; ++i)
		GrayAdd[i].resize(Width, 0);

	//找出灰度和最大位置,作为粗略中心位置;
	vector<int> C(Height);//数组存储每行中心位置;
	for (int i = 0; i < Height; i++)
	{
		uchar* img_row = imagegray.ptr<uchar>(i);
		C[i] = 0;
		double row_max = 0;
		int idx_left = 0;
		int idx_right = 0;

		for (int j = BandWidth; j < Width - BandWidth; j++)
		{
			if (img_row[j] > Ilow)
			{
				float aa = abs(img_row[j] - img_row[j - 1]);
				float bb = abs(img_row[j] - img_row[j + 1]);

				if ((aa < Ihigh) && (bb < Ihigh))
				{
					GrayAdd[i][j] = img_row[j - 1] + img_row[j] + img_row[j + 1];
					Bool[i] = 1;

					if (row_max < GrayAdd[i][j])
					{
						row_max = GrayAdd[i][j];
						idx_left = j;
					}
					if (row_max == GrayAdd[i][j])
					{
						idx_right = j;
					}
				}
			}
		}
		C[i] = idx_left + (idx_right - idx_left) / 2;
	}

	//检测光带宽度，去掉背景区域
	for (int i = 0; i < Height; i++)
	{
		int center = C[i];
		if (center != 0)
		{
			int center_val = imagegray.at<uchar>(i, center);
			int left_check = center - thresWidth;
			int right_check = center + thresWidth;
			if (left_check < 0)
				left_check = 0;
			if (right_check >= Width)  //buh fix
				right_check = Width - 1;

			int left_val = imagegray.at<uchar>(i, left_check);
			int right_val = imagegray.at<uchar>(i, right_check);
			int aa = abs(2 * center_val - (left_val + right_val));
			if (aa < 10 || (center_val - left_val < 10) || (center_val - right_val < 10))
			{
				C[i] = 0; //光带去掉去掉此行
			}
		}
	}

	//观察粗略中心线位置;
	cv::Mat imageLook = imagegray.clone();
	//cvtColor(imageLook, imageLook, CV_GRAY2BGR);
	for (int i = 0; i < Height; i++)
	{
		if (Bool[i] == 1)
		{
			int m = C[i];
			imageLook.at<uchar>(i, m) = 0;
		}

	}
	//addWeighted(Image, 0.5, imageLook, 0.5, 0, imageLook);
	//imshow("粗略",imageLook);


	//找出光带区域，对光带区域图像进行低通滤波;
	//cv::imwrite("culue.bmp", imageLook);
	int Nw = BandWidth;
	cv::Mat imageLiner = cv::Mat::zeros(Height, Width, CV_32FC1);

	vector<float> vLaserData; //用来计算阈值;
	vector<float> vThresh(Height, 0.0);
	for (int i = 0; i < Height; i++)
	{
		vLaserData.clear();
		int m = C[i];
		if (m && (m - Nw) >= 0 && (m + Nw) < Width)
		{
			for (int j = m - Nw; j <= m + Nw; j++)
			{
				if (j<2 || j>Width - 3)
				{
					continue;
				}

				float fTemp = 0.370286*imagegray.at<uchar>(i, j) + 0.271843*(imagegray.at<uchar>(i, j - 1) + imagegray.at<uchar>(i, j + 1))
					+ 0.095016*(imagegray.at<uchar>(i, j - 2) + imagegray.at<uchar>(i, j + 2));
				imageLiner.at<float>(i, j) = sqrt(sqrt(fTemp));
				vLaserData.push_back(imageLiner.at<float>(i, j));
			}
			float fTh = getThresh(vLaserData);
			vThresh[i] = fTh;
		}
	}
	Mat imageLiner_tmp = imageLiner.clone();
	for (int i = 0; i < Height; i++)
	{
		int m = C[i];
		for (int j = m - Nw; j <= m + Nw; j++)
		{
			if (j<2 || j>Width - 3)
			{
				continue;
			}
			if (imageLiner.at<float>(i, j) < vThresh[i])
			{
				imageLiner_tmp.at<float>(i, j) = 0;
			}
		}
	}
	//提取精确中心位置;
	vector<vector<double> > Gx(Height);
	for (int i = 0; i < Height; ++i)
		Gx[i].resize(Width, 0);

	vector<vector<double> > P(Height);

	for (int i = 0; i < Height; ++i)
		P[i].resize(Width, 0);                    //存放梯度值对应位置

	for (int m = 0; m < Height; m++)
	{
		if (vThresh[m] <= 0.0)
		{
			continue;
		}

		int xm = C[m];
		if (!xm || (xm - Nw) < 0 || (xm + Nw + 1) >= Width)
		{
			continue;
		}
		for (int n = xm - Nw; n <= xm + Nw; n++)
		{
			if (imageLiner.at<float>(m, n) < vThresh[m] && imageLiner.at<float>(m, n + 1) < vThresh[m])
			{
				Gx[m][n] = 0.0;
			}
			else if ((imageLiner.at<float>(m, n) < vThresh[m] || imageLiner.at<float>(m, n) == vThresh[m]) && imageLiner.at<float>(m, n + 1) > vThresh[m])
			{
				Gx[m][n] = abs(imageLiner.at<float>(m, n + 1) - vThresh[m]);
				P[m][n] = n + 1 - Gx[m][n] / (2 * abs(imageLiner.at<float>(m, n) - imageLiner.at<float>(m, n + 1)));
			}
			else if ((imageLiner.at<float>(m, n + 1) < vThresh[m] || imageLiner.at<float>(m, n + 1) == vThresh[m]) && imageLiner.at<float>(m, n) > vThresh[m])
			{
				Gx[m][n] = abs(imageLiner.at<float>(m, n) - vThresh[m]);
				P[m][n] = n + Gx[m][n] / (2 * abs(imageLiner.at<float>(m, n) - imageLiner.at<float>(m, n + 1)));
			}
			else if (imageLiner.at<float>(m, n) > vThresh[m] && imageLiner.at<float>(m, n + 1) > vThresh[m])
			{
				Gx[m][n] = abs(imageLiner.at<float>(m, n) - imageLiner.at<float>(m, n + 1));
				P[m][n] = n + 0.5;
			}
		}

	}
	cv::Mat img5;
	cvtColor(imageLiner, img5, CV_GRAY2BGR);
	if (Image.channels() == 1)
	{
		cvtColor(Image, Image, CV_GRAY2BGR);
	}
	vector<float> Cw(Height);//数组存储中心线位置;
	for (int i = 0; i < Height; i++)
	{
		double m = C[i];
		double mm = 0;
		float nn = 0;
		if (m && (m - Nw) >= 0 && (m + Nw) < Width)
		{
			for (int j = m - Nw; j < m + Nw; j++)
			{
				mm = Gx[i][j] * P[i][j] + mm;
				nn = Gx[i][j] + nn;
			}
			if (!(mm && nn))
			{
				Cw[i] = 0.0;
				continue;
			}
			Cw[i] = mm / nn;
			int pp = (int)(Cw[i] + 0.5);
			//int pp2 = (int)Cw[i];
			//if (pp < 0 || pp >= Width)
			//{
			//	continue;
			//}
			Image.at<cv::Vec3b>(i, pp) = cv::Vec3b(255, 0, 0);
		}
		else
		{
			Cw[i] = 0.0;
		}

	}

	vSubCenterPoint.clear();
	for (int i = 0; i < Height; i++)
	{
		if (Cw[i] != 0.0)
		{
			vSubCenterPoint.push_back(cv::Point2f(Cw[i], i));
		}
	}

	cvtColor(Image, dst, CV_BGR2GRAY);
	//imshow("w", dst);
	//waitKey(0);

	return true;
}

bool MeasureDistance::insert(vector<Point2f>&Inpoint)
{
	if (Inpoint.size() == NULL)
	{
		return false;
	}
	Point2f c, d, e;
	sort(Inpoint.begin(), Inpoint.end(), [](Point2f a, Point2f b){
		return a.y < b.y; });
		for (int i = 40; i < Inpoint.size() - 40; i++)
		{
			if ((abs(Inpoint[i].x - Inpoint[i + 1].x)>2) && (abs(Inpoint[i].x - Inpoint[i + 2].x)>2))
			{
				c.x = Inpoint[i].x;
				c.y = Inpoint[i].y;
				for (int j = i + 20; j<Inpoint.size() - 50; j++)
				{
					if ((abs(Inpoint[j].x - Inpoint[j + 1].x)<1) && (abs(Inpoint[j].x - Inpoint[j + 2].x)<1))
					{
						d.x = Inpoint[j].x;
						d.y = Inpoint[j].y;
						break;
					}
				}
				break;

			}

		}
		for (int i = 0; i < 2 * (d.y - c.y); i++)
		{
			e.x = c.x + i*(d.x - c.x) / (d.y - c.y) / 2;
			e.y = c.y + i / 2;
			Inpoint.push_back(e);
		}
		return true;
}

float NewGaussianConv2D(const IplImage* img, const CvMat* kernel, int row, int col)
{
	assert(img);
	assert(kernel);

	if ((row < 0) || (row > img->height) || (col < 0) || (col > img->width))
	{
		cout << "boundary error" << endl;
		return -1;
	}

	float retVal = 0.0;

	int iKernelW = kernel->width;
	int iKernelH = kernel->height;

	int iKernelCenX = iKernelW >> 1;   //模板中心x
	int iKernelCenY = iKernelH >> 1;   //模板中心y

	int m = 0;
	int n = 0;
	int index_xLU = 0;
	int index_yLU = 0;
	int index_xRU = 0;
	int index_yRU = 0;
	int index_xLD = 0;
	int index_yLD = 0;
	int index_xRD = 0;
	int index_yRD = 0;

	const double precision = 0.00000001;

	if ((row < iKernelCenY) || (row >= (img->height - iKernelCenY))
		|| (col < iKernelCenX) || (col >= (img->width - iKernelCenX)))
	{
		for (m = 0; m < kernel->height; m++)
		{
			for (n = 0; n < kernel->width; n++)
			{
				index_xLU = col + iKernelCenX - n;
				index_yLU = row + iKernelCenY - m;

				if ((index_yLU >= 0) && (index_yLU < img->height)
					&& (index_xLU >= 0) && (index_xLU < img->width)
					)
				{
					retVal += (((float*)(img->imageData + img->widthStep*index_yLU))[index_xLU]
						* cvmGet(kernel, m, n));
				} //end if
			}//end for n
		}//end for m
	}//end if
	else
	{
		//去除边缘后，根据高斯模板的对称性，用先加减的方式，减少一大半的除法
		//dxx,dyy,dxy
		if (fabs(cvmGet(kernel, 0, 0) - cvmGet(kernel, iKernelH - 1, iKernelW - 1)) < precision)
		{
			//dxx,dyy
			if (fabs(cvmGet(kernel, 0, 0) - cvmGet(kernel, iKernelH - 1, 0)) < precision)
			{
				for (m = 0; m < iKernelCenY; m++)
				{
					for (n = 0; n < iKernelCenX; n++)
					{
						index_xLU = col + iKernelCenX - n;
						index_yLU = row + iKernelCenY - m;
						index_xRU = col - iKernelCenX + n;
						index_yRU = row + iKernelCenY - m;
						index_xLD = col + iKernelCenX - n;
						index_yLD = row - iKernelCenY + m;
						index_xRD = col - iKernelCenX + n;
						index_yRD = row - iKernelCenY + m;

						retVal += ((((float*)(img->imageData + img->widthStep * index_yLU))[index_xLU]
							+ ((float*)(img->imageData + img->widthStep * index_yRD))[index_xRD]
							+ ((float*)(img->imageData + img->widthStep * index_yLD))[index_xLD]
							+ ((float*)(img->imageData + img->widthStep * index_yRU))[index_xRU]
							) * cvmGet(kernel, m, n));
					}//end for n
				}//end for m

				m = iKernelCenY;
				index_yLU = row + iKernelCenY - m;
				index_yRD = row - iKernelCenY + m;

				for (n = 0; n < iKernelCenX; n++)
				{
					index_xLU = col + iKernelCenX - n;
					index_xRD = col - iKernelCenX + n;

					retVal += ((((float*)(img->imageData + img->widthStep * index_yLU))[index_xLU]
						+ ((float*)(img->imageData + img->widthStep * index_yRD))[index_xRD]
						) * cvmGet(kernel, m, n));
				}//end for n

				n = iKernelCenX;
				index_xLU = col + iKernelCenX - n;
				index_xRD = col - iKernelCenX + n;

				for (m = 0; m < iKernelCenY; m++)
				{
					index_yLU = row + iKernelCenY - m;
					index_yRD = row - iKernelCenY + m;

					retVal += ((((float*)(img->imageData + img->widthStep * index_yLU))[index_xLU]
						+ ((float*)(img->imageData + img->widthStep * index_yRD))[index_xRD]
						) * cvmGet(kernel, m, n));
				}//end for m

				retVal += (((float*)(img->imageData + img->widthStep * row))[col] * cvmGet(kernel, m, n));
			}//end if
			else //dxy
			{
				for (m = 0; m < iKernelCenY; m++)
				{
					for (n = 0; n < iKernelCenX; n++)
					{
						index_xLU = col + iKernelCenX - n;
						index_yLU = row + iKernelCenY - m;
						index_xRU = col - iKernelCenX + n;
						index_yRU = row + iKernelCenY - m;
						index_xLD = col + iKernelCenX - n;
						index_yLD = row - iKernelCenY + m;
						index_xRD = col - iKernelCenX + n;
						index_yRD = row - iKernelCenY + m;

						retVal += ((((float*)(img->imageData + img->widthStep * index_yLU))[index_xLU]
							+ ((float*)(img->imageData + img->widthStep * index_yRD))[index_xRD]
							- ((float*)(img->imageData + img->widthStep * index_yLD))[index_xLD]
							- ((float*)(img->imageData + img->widthStep * index_yRU))[index_xRU]
							) * cvmGet(kernel, m, n));
					}//end for n
				}//end for m
			}//end else
		}
		else  //dx,dy
		{
			//dx
			if (fabs(cvmGet(kernel, 0, 0) - cvmGet(kernel, 0, iKernelW - 1)) < precision)
			{
				for (m = 0; m < iKernelCenY; m++)
				{
					for (n = 0; n < iKernelCenX; n++)
					{
						index_xLU = col + iKernelCenX - n;
						index_yLU = row + iKernelCenY - m;
						index_xRU = col - iKernelCenX + n;
						index_yRU = row + iKernelCenY - m;
						index_xLD = col + iKernelCenX - n;
						index_yLD = row - iKernelCenY + m;
						index_xRD = col - iKernelCenX + n;
						index_yRD = row - iKernelCenY + m;

						retVal += ((((float*)(img->imageData + img->widthStep * index_yLU))[index_xLU]
							+ ((float*)(img->imageData + img->widthStep * index_yRU))[index_xRU]
							- ((float*)(img->imageData + img->widthStep * index_yLD))[index_xLD]
							- ((float*)(img->imageData + img->widthStep * index_yRD))[index_xRD]
							) * cvmGet(kernel, m, n));
					}//end for n
				}//end for m

				n = iKernelCenX;
				index_xLU = col + iKernelCenX - n;
				index_xRD = col - iKernelCenX + n;

				for (m = 0; m < iKernelCenY; m++)
				{
					index_yLU = row + iKernelCenY - m;
					index_yRD = row - iKernelCenY + m;

					retVal += ((((float*)(img->imageData + img->widthStep * index_yLU))[index_xLU]
						- ((float*)(img->imageData + img->widthStep * index_yRD))[index_xRD]
						) * cvmGet(kernel, m, n));
				}//end for m
			}//end if
			else //dy
			{
				for (m = 0; m < iKernelCenY; m++)
				{
					for (n = 0; n < iKernelCenX; n++)
					{
						index_xLU = col + iKernelCenX - n;
						index_yLU = row + iKernelCenY - m;
						index_xRU = col - iKernelCenX + n;
						index_yRU = row + iKernelCenY - m;
						index_xLD = col + iKernelCenX - n;
						index_yLD = row - iKernelCenY + m;
						index_xRD = col - iKernelCenX + n;
						index_yRD = row - iKernelCenY + m;

						retVal += ((((float*)(img->imageData + img->widthStep * index_yLU))[index_xLU]
							- ((float*)(img->imageData + img->widthStep * index_yRU))[index_xRU]
							+ ((float*)(img->imageData + img->widthStep * index_yLD))[index_xLD]
							- ((float*)(img->imageData + img->widthStep * index_yRD))[index_xRD]
							) * cvmGet(kernel, m, n));
					}//end for n
				}//end for m

				m = iKernelCenY;
				index_yLU = row + iKernelCenY - m;
				index_yRD = row - iKernelCenY + m;

				for (n = 0; n < iKernelCenX; n++)
				{
					index_xLU = col + iKernelCenX - n;
					index_xRD = col - iKernelCenX + n;

					retVal += ((((float*)(img->imageData + img->widthStep * index_yLU))[index_xLU]
						- ((float*)(img->imageData + img->widthStep * index_yRD))[index_xRD]
						) * cvmGet(kernel, m, n));
				}//end for m
			}//end else
		}//end else
	}//end else

	return retVal;
}
void MeasureDistance::stegerCenter(IplImage* pSrcImg, IplImage* pBinImg, vector<Point2f> &centerPoint, vector<int> &lineWidth, int kind)
{
	double totalStartT = cvGetTickCount();
	Point2f data;
	CvSize imgSize = cvGetSize(pSrcImg);

	/*if (kind == 2)
	{
		enhance(pSrcImg);
	}*/

	IplImage* pGrayImg = cvCreateImage(imgSize, IPL_DEPTH_8U, 1);   //掩膜
	if (pSrcImg->nChannels > 1)
	{
		cvCvtColor(pSrcImg, pGrayImg, CV_BGR2GRAY);
	}
	else
	{
		cvCopy(pSrcImg, pGrayImg);
	}

	IplImage* pShowImg = cvCreateImage(imgSize, IPL_DEPTH_8U, 3);
	IplImage* pMask = cvCreateImage(imgSize, IPL_DEPTH_8U, 1);   //掩膜
	Mat pOutImg;
	//均初始化为0
	cvSet(pBinImg, cvScalarAll(0));
	//cvSet(pOutImg, cvScalarAll(0));

	cvCvtColor(pGrayImg, pShowImg, CV_GRAY2BGR);//转为为彩色图便于观察
	cvSet(pMask, cvScalarAll(0));

	//输入图像转成浮点型，方便计算
	IplImage* pSrcImgDouble = cvCreateImage(imgSize, IPL_DEPTH_32F, 1);
	cvConvertScale(pGrayImg, pSrcImgDouble, 1.0 / 255);

	//输入图像做二值化处理，用来做掩膜
	cvThreshold(pSrcImgDouble, pMask, TRANS_THRESHOLD / 255, 1, CV_THRESH_BINARY);

	if (lineWidth.empty())
		/*NULL;*/

		return;
	////以步长设置线宽
	//int widthStep = 10;
	//vector<float> stepLineWidth(lineWidth.size(), 0);
	//for (int i = 0; i != lineWidth.size(); i++)
	//{
	//	float width = 0;
	//	for (int k = i; k < i + widthStep && k <lineWidth.size(); k++)
	//	{
	//		width = width + lineWidth[k];
	//	}
	//	width /= widthStep;
	//	stepLineWidth[i] = width;
	//}

	const int WIDTH_GRADIEN = 1;
	vector<int> widthVec, widSum;
	vector<float> stepLineWidth;
	for (int i = 0; i < lineWidth.size(); i++)
	{
		int tk = 1;
		for (int j = i + 1; j < lineWidth.size() - 1; j++)
		{
			if ((abs(lineWidth[j + 1] - lineWidth[j]) <= WIDTH_GRADIEN) && (abs(lineWidth[j] - lineWidth[j - 1]) <= WIDTH_GRADIEN) && (abs(lineWidth[i] - lineWidth[j]) <= 2 * WIDTH_GRADIEN))
			{
				tk++;
			}
			else
			{
				break;
			}
		}
		widthVec.push_back(tk);
		int widsum = 0;
		for (int a : widthVec)
		{
			widsum += a;
		}
		widSum.push_back(widsum);
		stepLineWidth.push_back(lineWidth[i + (tk) / 2]);
		i = i + tk - 1;
	}
	//for (int m = 0; m < stepLineWidth.size(); m+=widthStep)
	//{
	
	for (int m = 0, wi = 0; m < stepLineWidth.size(); wi += widthVec[m], m++)
	{
	//计算高斯卷积模板
	const int sigma = float(stepLineWidth[m]) / sqrt(3)+2;
	//const int sigma =4;
	const int num = 2 * 3 * sigma + 1;
	const int MidVal = num / 2;
	const float PI = 3.14159f;

	CvMat* gaussianDx = cvCreateMat(num, num, CV_32FC1);
	CvMat* gaussianDy = cvCreateMat(num, num, CV_32FC1);
	CvMat* gaussianDxx = cvCreateMat(num, num, CV_32FC1);
	CvMat* gaussianDyy = cvCreateMat(num, num, CV_32FC1);
	CvMat* gaussianDxy = cvCreateMat(num, num, CV_32FC1);

	for (int i = 0; i < num; i++)
	{
		for (int j = 0; j < num; j++)
		{
			//squareSigma是sigma平方, powFourSigma是sigma四次方
			//x_index是x, y_index是y, squareX是x平方, squareY是y平方
			//expTerm是指数项, expTermDivA是指数项除以2π*sigma的四次方
			//expTermDivB指数项除以2πsigma的六次方
			int squareSigma = sigma * sigma;
			int powFourSigma = squareSigma * squareSigma;
			int x_index = i - MidVal;
			int y_index = j - MidVal;
			int squareX = x_index * x_index;
			int squareY = y_index * y_index;
			float expTerm = exp(-(squareX + squareY) / 2.0f / squareSigma);
			float expTermDivA = expTerm / PI / powFourSigma / 2;
			float expTermDivB = expTermDivA / squareSigma;

			float tmpxx = (squareX - squareSigma) * expTermDivB;
			float tmpyy = (squareY - squareSigma) * expTermDivB;
			float tmpxy = x_index * y_index * expTermDivB;

			((float*)(gaussianDx->data.ptr + gaussianDx->step * i))[j] = -(x_index * expTermDivA);
			((float*)(gaussianDy->data.ptr + gaussianDy->step * i))[j] = -(y_index * expTermDivA);
			((float*)(gaussianDxx->data.ptr + gaussianDxx->step * i))[j] = (squareX - squareSigma) * expTermDivB;
			((float*)(gaussianDyy->data.ptr + gaussianDyy->step * i))[j] = (squareY - squareSigma) * expTermDivB;
			((float*)(gaussianDxy->data.ptr + gaussianDxy->step * i))[j] = x_index * y_index * expTermDivB;
		}
	}

	float dxVal = 0.0f;
	float dyVal = 0.0f;
	float dxxVal = 0.0f;
	float dxyVal = 0.0f;
	float dyyVal = 0.0f;
	//FILE* validPointfp = fopen("validPoint.txt", "w");

	double startProcTime = cvGetTickCount();
	for (int i = wi; i < wi + widthVec[m]; i++)
	/*for (int i = 0; i < imgSize.height; i++)*/
	{
		
		for (int j = 0; j < imgSize.width; j++)
		{
			if (((uchar*)(pMask->imageData + pMask->widthStep * i))[j] == 1)
			{
				//double startConvTime = cvGetTickCount();

				//计算各点经过高斯卷积后的值
				//   dxVal = GaussianConv2D(pSrcImgDouble, gaussianDx, i, j);
				//dyVal = GaussianConv2D(pSrcImgDouble, gaussianDy, i, j);
				//dxxVal = GaussianConv2D(pSrcImgDouble, gaussianDxx, i, j);				
				//dyyVal = GaussianConv2D(pSrcImgDouble, gaussianDyy, i, j);	
				//dxyVal = GaussianConv2D(pSrcImgDouble, gaussianDxy, i, j);
				dxVal = NewGaussianConv2D(pSrcImgDouble, gaussianDx, i, j);
				dyVal = NewGaussianConv2D(pSrcImgDouble, gaussianDy, i, j);
				dxxVal = NewGaussianConv2D(pSrcImgDouble, gaussianDxx, i, j);
				dyyVal = NewGaussianConv2D(pSrcImgDouble, gaussianDyy, i, j);
				dxyVal = NewGaussianConv2D(pSrcImgDouble, gaussianDxy, i, j);

				if ((dxVal == -1) || (dyVal == -1) || (dxxVal == -1) || (dyyVal == -1) || (dxyVal == -1))
				{
					cout << "row or col error" << endl;
					exit(1);
				}

				//double endConvTime = cvGetTickCount();
				//std::cout << "conv time: " << (endConvTime - startConvTime) / cvGetTickFrequency() << endl;

				//计算各个像素的hessian矩阵的最大绝对特征值
				//double startTim = cvGetTickCount();

				float _maxAbsEigVal = 0.0f;
				float _selectedEigVal = 0.0f;
				float _eigVecX = 0.0f;
				float _eigVecY = 0.0f;
				float eigenval_a = 0.0f;
				float eigenval_b = 0.0f;
				float _tmpA = 0.0f;
				float _tmpB = 0.0f;
				float _tmpC = 0.0f;
				float _tmpD = 0.0f;

				//计算特征值
				_tmpA = dxxVal + dyyVal;
				_tmpB = _tmpA * _tmpA;
				_tmpC = dxyVal * dxyVal;
				_tmpD = dxxVal * dyyVal - _tmpC;
				_tmpB = sqrt(_tmpB - 4 * _tmpD);

				eigenval_a = (_tmpA + _tmpB) / 2;
				eigenval_b = (_tmpA - _tmpB) / 2;

				//获取最大绝对特征值
				_tmpA = fabs(eigenval_a);
				_tmpB = fabs(eigenval_b);

				if (_tmpA > _tmpB)
				{
					_maxAbsEigVal = _tmpA;
					_selectedEigVal = eigenval_a;
				}
				else
				{
					_maxAbsEigVal = _tmpB;
					_selectedEigVal = eigenval_b;
				}

				//计算最大绝对特征值对应的特征向量
				_tmpA = _selectedEigVal - dxxVal;
				_tmpB = _tmpA * _tmpA;

				//可取正负，但是是法线方向，故可取正数
				_eigVecY = sqrt(1.0 / (_tmpC / _tmpB + 1));
				_eigVecX = dxyVal * _eigVecY / (_tmpA + 0.000000001);

				double endTim = cvGetTickCount();
				//cout << "caclmaxEigen time: " << (endTim - startTim) / cvGetTickFrequency() << endl;
				//根据一阶导数为0以及二阶导数大于指定阈值求线条边缘中心
				if (_maxAbsEigVal > EIGENVALUE_THRESHOLD)
				{
					float _tmpTVal = 0.0f;
					float absDxAndDy = 0.0;
					float subPixel_x = 0.0;
					float subPixel_y = 0.0;

					_tmpA = _eigVecX * dxVal + _eigVecY * dyVal;

					_tmpB = _eigVecX * _eigVecX * dxxVal + 2 * _eigVecX * _eigVecY * dxyVal
						+ _eigVecY * _eigVecY * dyyVal;

					_tmpTVal = -_tmpA / (_tmpB + 0.000000001);

					subPixel_x = _eigVecX * _tmpTVal;
					subPixel_y = _eigVecY * _tmpTVal;

					if ((fabs(subPixel_x) <= 0.5) && (fabs(subPixel_y) <= 0.5))
					{
						((uchar*)(pBinImg->imageData + pBinImg->widthStep*i))[j] = 255;
						((uchar*)(pShowImg->imageData + pShowImg->widthStep*i + pShowImg->nChannels*j))[0] = 0;
						((uchar*)(pShowImg->imageData + pShowImg->widthStep*i + pShowImg->nChannels*j))[1] = 0;
						((uchar*)(pShowImg->imageData + pShowImg->widthStep*i + pShowImg->nChannels*j))[2] = 255;

						//fprintf(validPointfp, "%10f,   %10f\n", i + subPixel_x, j + subPixel_y);
						data.y = i + subPixel_x;
						data.x = j + subPixel_y;
						centerPoint.push_back(data);

					}//end if
				}

			}
		}

	}

	//fclose(validPointfp);
	//double endProcTime = cvGetTickCount();
	//std::cout << "process time: " << (endProcTime - startProcTime) / cvGetTickFrequency() << endl;

	//释放高斯卷积模板占用的内存
	cvReleaseMat(&gaussianDx);
	cvReleaseMat(&gaussianDy);
	cvReleaseMat(&gaussianDxx);
	cvReleaseMat(&gaussianDyy);
	cvReleaseMat(&gaussianDxy);
	}

	//double disStartT = cvGetTickCount();
	//cvNamedWindow("src", CV_WINDOW_AUTOSIZE);
	//cvNamedWindow("out", CV_WINDOW_AUTOSIZE);
	//cvMoveWindow("src", 100, 100);
	//cvMoveWindow("out", 300, 300);
	//cvShowImage("src", pSrcImg);
	//cvShowImage("out", pBinImg);

	/*double disEndT = cvGetTickCount();
	std::cout << "display time: " << (disEndT - disStartT) / cvGetTickFrequency() << endl;
	double totalEndT = cvGetTickCount();
	std::cout << "total time: " << (totalEndT - totalStartT) / cvGetTickFrequency() << endl;*/
	//cvWaitKey(0);

	//cvDestroyAllWindows();
	//cvReleaseImage(&pSrcImg);
	cvReleaseImage(&pMask);
	cvReleaseImage(&pSrcImgDouble);
	cvReleaseImage(&pGrayImg);
	cvReleaseImage(&pShowImg);
}


int OtsuAlgThreshold(const Mat image)
{
	if (image.channels() != 1)
	{
		cout << "Please input Gray-image!" << endl;
		return 0;
	}
	int T = 0; //Otsu算法阈值  
	double varValue = 0; //类间方差中间值保存  
	double w0 = 0; //前景像素点数所占比例  
	double w1 = 0; //背景像素点数所占比例  
	double u0 = 0; //前景平均灰度  
	double u1 = 0; //背景平均灰度  
	double Histogram[256] = { 0 }; //灰度直方图，下标是灰度值，保存内容是灰度值对应的像素点总数  
	uchar *data = image.data;
	double totalNum = image.rows*image.cols; //像素总数  
	//计算灰度直方图分布，Histogram数组下标是灰度值，保存内容是灰度值对应像素点数  
	for (int i = 0; i<image.rows; i++)   //为表述清晰，并没有把rows和cols单独提出来  
	{
		for (int j = 0; j<image.cols; j++)
		{
			Histogram[data[i*image.step + j]]++;
		}
	}
	for (int i = 0; i<255; i++)
	{
		//每次遍历之前初始化各变量  
		w1 = 0;       u1 = 0;       w0 = 0;       u0 = 0;
		//***********背景各分量值计算**************************  
		for (int j = 0; j <= i; j++) //背景部分各值计算  
		{
			w1 += Histogram[j];  //背景部分像素点总数  
			u1 += j*Histogram[j]; //背景部分像素总灰度和  
		}
		if (w1 == 0) //背景部分像素点数为0时退出  
		{
			break;
		}
		u1 = u1 / w1; //背景像素平均灰度  
		w1 = w1 / totalNum; // 背景部分像素点数所占比例  
		//***********背景各分量值计算**************************  

		//***********前景各分量值计算**************************  
		for (int k = i + 1; k<255; k++)
		{
			w0 += Histogram[k];  //前景部分像素点总数  
			u0 += k*Histogram[k]; //前景部分像素总灰度和  
		}
		if (w0 == 0) //前景部分像素点数为0时退出  
		{
			break;
		}
		u0 = u0 / w0; //前景像素平均灰度  
		w0 = w0 / totalNum; // 前景部分像素点数所占比例  
		//***********前景各分量值计算**************************  

		//***********类间方差计算******************************  
		double varValueI = w0*w1*(u1 - u0)*(u1 - u0); //当前类间方差计算  
		if (varValue<varValueI)
		{
			varValue = varValueI;
			T = i;
		}
	}
	return T;
}
bool MeasureDistance::match(Mat& leftImg, Mat& rightImg, Mat& F, vector<Point2f>& lefts, vector<Point2f>& rights, vector<Match>& matches, Mat& matchMat, double threshold)
{
	//double threshold = 3.0; //处理噪声点阈值
	if (lefts.empty() || rights.empty())
	{
		return false;
	}
	/*vector<Point2f> rights;
	interWithY(rights1, rights);*/

	//interWithY(lefts1, lefts);
	//rights = rights1;
	Mat_<double> F_ = F;
	vector<Vec3f> line;
	vector<Match> firstMatch;
	//点到是直线的距离;
	computeCorrespondEpilines(lefts, 1, F, line);

	for (int i = 0; i != line.size(); i++)
	{
		Point2f p_left = lefts[i];
		Point2f p_right;
		double min_d = INFINITY;
		int idx = 0;
		for (int j = 0; j != rights.size(); j++)
		{
			p_right = rights[j];

			double d = abs(line[i][0] * p_right.x + line[i][1] * p_right.y + line[i][2]) / (sqrt(line[i][0] * line[i][0] + line[i][1] * line[i][1]));

			if (min_d > d)
			{
				idx = j;
				min_d = d;
			}
		}

		//阈值约束
		if (min_d <threshold )
		{		
			firstMatch.push_back(Match(p_left, rights[idx], min_d));//
		}
	}

	vector<Match>  sortMatch(firstMatch.size());
	copy(firstMatch.begin(), firstMatch.end(), sortMatch.begin());
	//过滤,去除一对多匹配
	sort(sortMatch.begin(), sortMatch.end(),
		[](Match a, Match b){
		if (a.right.x == b.right.x && a.right.y == b.right.y)
		{
			return a.distance < b.distance;
		}
		else
		{
			if (a.right.y == b.right.y)
			{
				return a.right.x < b.right.x;
			}
			else return a.right.y < b.right.y;
		}
	});

	vector<Match>  eraseMatch(sortMatch.size());
	copy(sortMatch.begin(), sortMatch.end(), eraseMatch.begin());

	int idx = 0;
	for (int i = 0; i != eraseMatch.size(); i++)
	{
		if ((eraseMatch[idx].right.x != eraseMatch[i].right.x) || (eraseMatch[idx].right.y != eraseMatch[i].right.y))
		{
			eraseMatch[idx + 1] = eraseMatch[i];
			idx++;
		}
	}
	if (!eraseMatch.empty() && idx+1 < eraseMatch.size())
		eraseMatch.erase(eraseMatch.begin() + idx + 1, eraseMatch.end());

	//插值提高匹配精度
	matches = eraseMatch;

	//show point match 
	if (!leftImg.empty() && !rightImg.empty())
	{
		Size si = leftImg.size();
		Mat matchImg(leftImg.rows, leftImg.cols * 2, CV_8UC3, Scalar(0, 0, 0));
		cvtColor(leftImg, leftImg, CV_GRAY2BGR);
		cvtColor(rightImg, rightImg, CV_GRAY2BGR);
		leftImg.copyTo(matchImg(Rect(0, 0, si.width, si.height)));
		rightImg.copyTo(matchImg(Rect(si.width, 0, si.width, si.height)));
		for (int i = 0; i < firstMatch.size(); i += 5)
		{
			Point start = firstMatch[i].left;
			Point end(firstMatch[i].right.x + si.width, firstMatch[i].right.y);
			//cv::circle(matchImg, start, 2, Scalar(0, 0, 255));
			//cv::circle(matchImg, end, 2, Scalar(0, 0, 255));
			cv::line(matchImg, start, end, Scalar(0, 255, 255));

		}

	    matchMat = matchImg.clone();
		//imshow("match.", matchImg);
		//waitKey();
	}

	return true;
}

void MeasureDistance::interWithY(vector<Point2f>& lefts, vector<Point2f>& rights)
{
	//插值y，提高y轴精度
	sort(lefts.begin(), lefts.end(), [](Point2f a, Point2f b){
	return a.y>b.y; });

	const int LenY = 2;

	for (int i = 0; i != lefts.size() - 1; i++)
	{
		Point2f pl = lefts[i];
		double Insert_ly = (lefts[i + 1].y - lefts[i].y);
		double Insert_lx = (lefts[i + 1].x - lefts[i].x);
		double pl_x[LenY], pl_y[LenY];
		int idx_l = 0, idx_r = 0;
		for (int l = 0; l < LenY; l++)
		{
			
			//插值数组
			pl_y[l] = pl.y +l*(Insert_ly / LenY);
			pl_x[l] = pl.x + +l*(Insert_ly / LenY);
			Point2f m_pl(pl_x[l], pl_y[l]);
			rights.push_back(m_pl);
		}
	}
}
bool MeasureDistance::reprojectTo3D(Mat& M1, Mat& M2, Mat& R, Mat& t, vector<Match> matches, vector<Point3d>& real_points)
{
	Mat_<double> M1_ = M1;
	Mat_<double> M2_ = M2;
	Mat_<double> R_ = R;
	Mat_<double> t_ = t;

	double fl = (M1_(0, 0) + M1_(1, 1)) / 2;
	double fr = (M2_(0, 0) + M2_(1, 1)) / 2;

	for (int i = 0; i != matches.size(); i++)
	{
		Point2f left = matches[i].left;
		Point2f right = matches[i].right;
		double Xl = left.x - M1_(0, 2);
		double Xr = right.x - M2_(0, 2);
		double Yl = left.y - M1_(1, 2);
		double Yr = right.y - M2_(1, 2);

		Point3f p;

		p.z = fl*(fr*t_(0) - Xr*t_(2))
			/ ((Xr*(Xl*R_(2, 0) + Yl*R_(2, 1) + fl*R_(2, 2)) - fr*(Xl*R_(0, 0) + Yl*R_(0, 1) + fl*R_(0, 2))));
		p.x = p.z*Xl / fl;
		p.y = p.z*Yl / fl;

		real_points.push_back(p);
	}
	return true;
}

bool MeasureDistance::reprojectTo3D_ARC(Mat& M1, Mat& R, Mat& t, vector<Point2f> input, vector<Point3d>& real_points)

{
	//M1内参矩阵
	Mat mLeftRotation = R;            //单目旋转平移矩阵
	Mat mLeftTranslation = t;
	Mat mLeftRT = Mat(3, 4, CV_64F);//左相机M矩阵  
	for (int i = 0; i < 3; i++)
	{
		mLeftRotation.at<double>(i, 2) = mLeftTranslation.at<double>(i, 0);
	}
	Mat W = Mat::zeros(3, 3, CV_64F);
	W = M1*mLeftRotation;
	ofstream fout("test.txt");
	for (int i = 0; i != input.size(); i++)
	{
		Mat A = Mat(3, 1, CV_64F);
		A.at<double>(0, 0) = input[i].x;
		A.at<double>(1, 0) = input[i].y;
		A.at<double>(2, 0) = 1;


		Mat XYZ = Mat(3, 1, CV_64F);
		solve(W, A, XYZ, DECOMP_SVD);

		Point3d p(0, XYZ.at<double>(1, 0) / XYZ.at<double>(2, 0), XYZ.at<double>(0, 0) / XYZ.at<double>(2, 0));
      	real_points.push_back(p);
		fout << p.z << "," << p.y << endl;
	}
	return true;
}

bool MeasureDistance::reprojectTo3D_LSM(Mat& M1, Mat& M2, Mat& R, Mat& t, vector<Match> matches, vector<Point3d>& real_points)
{
	Mat mLeftRotation = Mat::eye(3, 3, CV_64F);
	Mat mLeftTranslation = Mat::zeros(3, 1, CV_64F);
	Mat mLeftRT = Mat(3, 4, CV_64F);//左相机M矩阵  
	hconcat(mLeftRotation, mLeftTranslation, mLeftRT);
	Mat mLeftM = M1*mLeftRT;

	Mat mRightRotation = R;
	Mat mRightTranslation = t;
	Mat mRightRT = Mat(3, 4, CV_64F);//右相机M矩阵  
	hconcat(mRightRotation, mRightTranslation, mRightRT);
	Mat mRightM = M2 * mRightRT;

	for (int i = 0; i != matches.size(); i++)
	{
		Point2d uvLeft = matches[i].left;
		Point2d uvRight = matches[i].right;

		//最小二乘法A矩阵   
		Mat A = Mat(4, 3, CV_64F);
		A.at<double>(0, 0) = uvLeft.x * mLeftM.at<double>(2, 0) - mLeftM.at<double>(0, 0);
		A.at<double>(0, 1) = uvLeft.x * mLeftM.at<double>(2, 1) - mLeftM.at<double>(0, 1);
		A.at<double>(0, 2) = uvLeft.x * mLeftM.at<double>(2, 2) - mLeftM.at<double>(0, 2);

		A.at<double>(1, 0) = uvLeft.y * mLeftM.at<double>(2, 0) - mLeftM.at<double>(1, 0);
		A.at<double>(1, 1) = uvLeft.y * mLeftM.at<double>(2, 1) - mLeftM.at<double>(1, 1);
		A.at<double>(1, 2) = uvLeft.y * mLeftM.at<double>(2, 2) - mLeftM.at<double>(1, 2);

		A.at<double>(2, 0) = uvRight.x * mRightM.at<double>(2, 0) - mRightM.at<double>(0, 0);
		A.at<double>(2, 1) = uvRight.x * mRightM.at<double>(2, 1) - mRightM.at<double>(0, 1);
		A.at<double>(2, 2) = uvRight.x * mRightM.at<double>(2, 2) - mRightM.at<double>(0, 2);

		A.at<double>(3, 0) = uvRight.y * mRightM.at<double>(2, 0) - mRightM.at<double>(1, 0);
		A.at<double>(3, 1) = uvRight.y * mRightM.at<double>(2, 1) - mRightM.at<double>(1, 1);
		A.at<double>(3, 2) = uvRight.y * mRightM.at<double>(2, 2) - mRightM.at<double>(1, 2);

		//最小二乘法B矩阵  
		Mat B = Mat(4, 1, CV_64F);
		B.at<double>(0, 0) = mLeftM.at<double>(0, 3) - uvLeft.x * mLeftM.at<double>(2, 3);
		B.at<double>(1, 0) = mLeftM.at<double>(1, 3) - uvLeft.y * mLeftM.at<double>(2, 3);
		B.at<double>(2, 0) = mRightM.at<double>(0, 3) - uvRight.x * mRightM.at<double>(2, 3);
		B.at<double>(3, 0) = mRightM.at<double>(1, 3) - uvRight.y * mRightM.at<double>(2, 3);

		Mat XYZ = Mat(3, 1, CV_64F);
		solve(A, B, XYZ, DECOMP_SVD);
		Point3d p(XYZ.at<double>(0, 0), XYZ.at<double>(1, 0), XYZ.at<double>(2, 0));
		real_points.push_back(p);
	}

	return true;
}
bool MeasureDistance::caculateDistance(Mat& left_img,Mat& right_img, const CameraCalib::CalibrationParames& calibResult, vector<Point3d>& real_points, Mat& outLeftCenterMat, Mat& outRightCenterMat, Mat& matchCenterMat, int Up_Down, int kind, int CENTER_FLAG)
{
	Mat M1 = calibResult.mLeftCameraInParas;
	Mat M2 = calibResult.mRightCameraInParas;
	Mat D1 = calibResult.mLeftCameraDistorParas;
	Mat D2 = calibResult.mRightCameraDistorParas;
	Mat F = calibResult.mFoundational;
	Mat R = calibResult.mL2RRotation33;
	Mat t = calibResult.mL2RTranslation;

	if (left_img.empty() || right_img.empty())
		return false;
	vector<Match> matches;

	Mat undist_left;
	Mat undist_right;
	undistort(left_img, undist_left, M1, D1);
	undistort(right_img, undist_right, M2, D2);

	//转化成灰度图
	Mat grayLeft(left_img.size(), CV_8UC1);
	Mat grayRight(left_img.size(), CV_8UC1);
	if (undist_left.channels() > 1)
	{
		cvtColor(undist_left, grayLeft, CV_BGR2GRAY);
	}
	else
	{
		grayLeft = undist_left;
	}
	if (undist_right.channels() > 1)
	{
		cvtColor(undist_right, grayRight, CV_BGR2GRAY);
	}
	else
	{
		grayRight = undist_right;
	}

	outLeftCenterMat = Mat::zeros(grayLeft.size(), CV_8UC1);
	outRightCenterMat = Mat::zeros(grayRight.size(), CV_8UC1);
	vector<Point2f> leftCtrPoint, leftCtrPoint1, leftCtrPoint2;
	vector<Point2f> rightCtrPoint, rightCtrPoint1, rightCtrPoint2;
	if (CENTER_FLAG == CENTER_FLAG_STEGER)
	{
		IplImage outLeftImg = outLeftCenterMat;
		IplImage outRightImg = outRightCenterMat;
		Mat out1(grayLeft.rows, grayLeft.cols, CV_8UC1, Scalar(0));
		Mat out2(grayLeft.rows, grayLeft.cols, CV_8UC1, Scalar(0));
		vector<Point2f>  data7, data8;
		Point3f a;                     //中心坐标点——上下左右视图和激光强弱标志位
		vector<int> linWidth1(grayLeft.rows, 0);
		vector<int> linWidth2(grayLeft.rows, 0);
		GetDisparityRIO(grayLeft,a,Up_Down,0);
		DetectLinerCenterM(grayLeft, out1, data7, a, linWidth1);
		IplImage leftImg = IplImage(out1);
		stegerCenter(&leftImg, &outLeftImg, leftCtrPoint, linWidth1, kind);
		GetDisparityRIO(grayRight, a, Up_Down,1);
		DetectLinerCenterM(grayRight, out2, data8, a, linWidth2);
		IplImage rightImg = IplImage(out2);
		stegerCenter(&rightImg, &outRightImg, rightCtrPoint, linWidth2, kind);	
	}
	match(grayLeft, grayRight, F, leftCtrPoint, rightCtrPoint, matches, matchCenterMat, 0.6);

	
	reprojectTo3D(M1, M2, R, t, matches, real_points);
	return true;
}
bool MeasureDistance::caculateDistance2(vector<Mat>& left_imgs, vector<Mat>& right_imgs, const CameraCalib::CalibrationParames& calibResult, vector<Point3d>& real_points, Mat& outLeftCenterMat, Mat& outRightCenterMat, Mat& matchCenterMat, int Up_Down, int kind, int CENTER_FLAG)
{
	Mat M1 = calibResult.mLeftCameraInParas;
	Mat M2 = calibResult.mRightCameraInParas;
	Mat D1 = calibResult.mLeftCameraDistorParas;
	Mat D2 = calibResult.mRightCameraDistorParas;
	Mat F = calibResult.mFoundational;
	Mat R = calibResult.mL2RRotation33;
	Mat t = calibResult.mL2RTranslation;
	Mat left_img ;
	Mat right_img ;
	/*if (left_imgs.empty() || right_imgs.empty())
		return false;*/
	vector<Match> matches;
	vector<Point2f> leftCtrPoint1;
	vector<Point2f> rightCtrPoint1;
	for (int k = 0; k < 1; k++)
	{
		Mat undist_left;
		Mat undist_right;
		undistort(left_imgs[k], undist_left, M1, D1);
		undistort(right_imgs[k], undist_right, M2, D2);

		//转化成灰度图
		Mat grayLeft(undist_left.size(), CV_8UC1);
		Mat grayRight(undist_right.size(), CV_8UC1);
		if (undist_left.channels() > 1)
		{
			cvtColor(undist_left, grayLeft, CV_BGR2GRAY);
		}
		else
		{
			grayLeft = undist_left;
		}
		if (undist_right.channels() > 1)
		{
			cvtColor(undist_right, grayRight, CV_BGR2GRAY);
		}
		else
		{
			grayRight = undist_right;
		}
		left_img = grayLeft.clone();
		right_img = grayRight.clone();
		outLeftCenterMat = Mat::zeros(grayLeft.size(), CV_8UC1);
		outRightCenterMat = Mat::zeros(grayRight.size(), CV_8UC1);
		vector<Point2f> leftCtrPoint;
		vector<Point2f> rightCtrPoint;
			IplImage outLeftImg = outLeftCenterMat;
			IplImage outRightImg = outRightCenterMat;
			Mat out1(grayLeft.rows, grayLeft.cols, CV_8UC1, Scalar(0));
			Mat out2(grayLeft.rows, grayLeft.cols, CV_8UC1, Scalar(0));
			vector<Point2f>  data7, data8;
			Point3f a;                     //中心坐标点——上下左右视图和激光强弱标志位
			vector<int> linWidth1(grayLeft.rows, 0);
			vector<int> linWidth2(grayLeft.rows, 0);
			GetDisparityRIO(grayLeft, a, Up_Down, 0);
			DetectLinerCenterM(grayLeft, out1, data7, a, linWidth1);
			IplImage leftImg = IplImage(out1);
			stegerCenter(&leftImg, &outLeftImg, leftCtrPoint, linWidth1, kind);
			GetDisparityRIO(grayRight, a, Up_Down, 1);
			DetectLinerCenterM(grayRight, out2, data8, a, linWidth2);
			IplImage rightImg = IplImage(out2);
			stegerCenter(&rightImg, &outRightImg, rightCtrPoint, linWidth2, kind);
			Mat outLeftImg11 = Mat(&outLeftImg);
			Mat outRightImg12 = Mat(&outRightImg);
		leftCtrPoint1.insert(leftCtrPoint1.end(), leftCtrPoint.begin(), leftCtrPoint.end());
		rightCtrPoint1.insert(rightCtrPoint1.end(), rightCtrPoint.begin(), rightCtrPoint.end());
	}
	match(left_img, right_img, F, leftCtrPoint1, rightCtrPoint1, matches, matchCenterMat, 0.5);
	reprojectTo3D(M1, M2, R, t, matches, real_points);
	/*ofstream fout2("D:\\document\\Debug\\real_points.txt");
	for (int i = 0; i != real_points.size(); i++)
	{
		Point3d p = real_points[i];
		fout2 << p.x << "," << p.y << p.z << endl;
	}*/
	return true;
}
bool MeasureDistance::caculateDistance_ARC(Mat& img, const CameraCalib::CalibrationParames& calibResult, vector<Point3d>& real_points, Mat& outCenterMat,int CENTER_FLAG)
{
	Mat M = calibResult.mLeftCameraInParas;
	Mat D = calibResult.mLeftCameraDistorParas;
	Mat R = calibResult.trianRotation;
	Mat t = calibResult.trianTanslation;

	vector<Match> matches;

	Mat undist_img;
	undistort(img, undist_img, M, D);

	//转化成灰度图
	Mat gray;
	if (undist_img.channels() > 1)
	{
		cvtColor(undist_img, gray, CV_BGR2GRAY);
	}
	else
	{
		gray = undist_img;
	}

	outCenterMat = Mat::zeros(gray.size(), CV_8UC1);
	vector<Point2f> ctrPoint, ctrPoint1;
	if (CENTER_FLAG == CENTER_FLAG_STEGER)
	{
		IplImage outImg = outCenterMat;
		Mat out1(gray.rows, gray.cols, CV_8UC1, Scalar(0));
		vector<Point2f>  data7, data8;
		Point3f a;                                           //中心坐标点
		vector<int> linWidth1(gray.rows, 0);
		//GetDisparityRIO(gray, a, 0, 0);
		a.x = 850;
		DetectLinerCenterM(gray, out1, data7, a, linWidth1);
		IplImage s_img = IplImage(out1);
		stegerCenter(&s_img, &outImg, ctrPoint, linWidth1, 9);
	}
	reprojectTo3D_ARC(M, R, t, ctrPoint, real_points);
	return true;
}
bool MeasureDistance::measureA1A2(vector<Point2d> points, double &a1_maxx, double &a1_maxy, double &a2_minx, double &a2_miny, double &a2, 
	vector<Point2d> &pointsA1, vector<Point2d>& pointsA2, vector<Point2d>& pointsA12)
{
	if (points.size() < 10)
	{
		return false;
	}
	//yList

	points.erase(points.begin(), points.begin() + 3);
	vector<double> ylist(points.size(), 0.0);
	vector<double> xlist(points.size(), 0.0);
	int colNumSize = MEAS_VALI_MAX_Z - MEAS_VALI_MIN_Z;
	vector<int> yCount(colNumSize, 0);
	vector<double> yValue(colNumSize, 0.0);
	int maxIdx = 0;  //存储拥有最多点的1mm列
	//计算a2时以1mm为单位存储在此1mm列内点的个数
	for (int i = 0; i != points.size(); i++)
	{
		Point2d p = points[i];
		ylist[i] = p.y; //存储ylist
		xlist[i] = p.x;
		yCount[floor(p.x) - MEAS_VALI_MIN_Z]++;
		yValue[floor(p.x) - MEAS_VALI_MIN_Z] += p.x;
	}
	sort(ylist.begin(), ylist.end());
	sort(xlist.begin(), xlist.end());
	int ylistSize = ylist.size();
	a1_maxx = (xlist[ylistSize - 1] + xlist[ylistSize - 2] + xlist[ylistSize - 3]) / 3;
	a1_maxy = (ylist[ylistSize - 1] + ylist[ylistSize - 2] + ylist[ylistSize - 3]) / 3;
	a2_minx = (xlist[0] + xlist[1] + xlist[2]) / 3;

	for (int i = 0; i < colNumSize; i++)
	{
		if (yCount[i]>5)
		{
			maxIdx = i;
			break;
		}
	}

	//求此1mm内深度轴信息的均值g
	a2 = yValue[maxIdx] / yCount[maxIdx];

	int t = 0;
	for (int i = 0; i<points.size(); i++)
	{
		if (points[i].x<a2 + 5 && points[i].x>a2_minx&& points[i].y>t)
		{
			t = points[i].y;
		}
	}

	for (int i = 0; i != points.size(); i++)
	{
		if (points[i].x>a2 && points[i].y>t - 3)
			pointsA1.push_back(points[i]);
		else if ((a2_minx<points[i].x) && (points[i].x<(a2 + 10)))
			pointsA2.push_back(points[i]);
		else
			pointsA12.push_back(points[i]);

	}
	//排序ylist
	return true;
}
bool MeasureDistance::measureA1A2_NEW(vector<Point2d> points, double &a1_maxx, double &a1_maxy, double &a2_minx, double &a2_miny, double &a2,vector<Point2d> &pointsA1, vector<Point2d>& pointsA2, vector<Point2d>&pointsA12)
{
	if (points.empty() || points.size()<20)
	{
		return false;
	}
	vector<float>pointsA2X,pointsA2X_Z2;
	vector<float>pointsA1Y,pointsA1Y_Z2;
/***********************************************a2*/
	vector<double> ylist(points.size(), 0.0);
	vector<double> xlist(points.size(), 0.0);
	int colNumSize = MEAS_VALI_MAX_Z - MEAS_VALI_MIN_Z;
	vector<int> yCount(colNumSize, 0);
	vector<double> yValue(colNumSize, 0.0);
	int maxIdx = 0;  //存储拥有最多点的1mm列
	//计算a2时以1mm为单位存储在此1mm列内点的个数
	for (int i = 0; i != points.size(); i++)
	{
		Point2d p = points[i];
		ylist[i] = p.y; //存储ylist
		xlist[i] = p.x;
		yCount[floor(p.x) - MEAS_VALI_MIN_Z]++;
		yValue[floor(p.x) - MEAS_VALI_MIN_Z] += p.x;
	}
	sort(ylist.begin(), ylist.end());
	sort(xlist.begin(), xlist.end());
	int ylistSize = ylist.size();
	a1_maxx = (xlist[ylistSize - 1] + xlist[ylistSize - 2] + xlist[ylistSize - 3]) / 3;
	a1_maxy = (ylist[ylistSize - 1] + ylist[ylistSize - 2] + ylist[ylistSize - 3]) / 3;
	a2_minx = (xlist[0] + xlist[1] + xlist[2]) / 3;
	a2_miny = (ylist[0] + ylist[1] + ylist[2]) / 3;
	for (int i = 0; i < colNumSize; i++)
	{
		if (yCount[maxIdx] < yCount[i])
		{
			maxIdx = i;
		}
		if (yCount[i]>20)
		{
			maxIdx = i;
			break;
		}
	}
	//求此1mm内深度轴信息的均值g
	double a2_miny2 =a2= yValue[maxIdx] / yCount[maxIdx];
	int a2_num = yCount[maxIdx];
	/*maxIdx = 0;
	for (int i = 0; i < colNumSize; i++)
	{
		if (yCount[i]>15)
		{
			maxIdx = i;
			break;
		}
	}
	a2 = yValue[maxIdx] / yCount[maxIdx];*/
		/**************a1***********************/
	//确定廓形范围
	int colNumSizey = (a1_maxy - a2_miny -60 ) / 10;
	if (colNumSizey <= 0)
		return false;
	vector<int> xCount(colNumSizey, 0);
	vector<double> xValue(colNumSizey, 0.0);
	int maxIdy = 0;
	int a1_max_y2=110;
	int a2_max_y2 = 80;
	if (a1_maxy >450 && a2_miny<250)
	{
		a1_max_y2 = 280;
	}
	for (int i = 0; i < points.size(); i++)
	{
		Point2d p = points[i];
		if ((p.y>a2_miny + a2_max_y2) && (p.y>a1_maxy - a1_max_y2) && (p.y<a1_maxy) && (p.x<a2_miny2 + 100))
		{
			xCount[floor((p.y - a2_miny - 70) / 10)]++;
			xValue[floor((p.y - a2_miny - 70) / 10)] += (p.y / 10);
		}

	}

	for (int i = 0; i < colNumSizey; i++)
	{
		if (xCount[maxIdy] < xCount[i])
		{
			maxIdy = i;
		}
	}
	double a1 = (xValue[maxIdy] / xCount[maxIdy]) * 10;
	/*****************************************************/
	int a1_num = xCount[maxIdy];
	//vector<double> ylista2;
	////int t = -1000;
	//for (int i = 0; i<points.size(); i++)//找到上部轮廓拐点处的y坐标
	//{
	//	/*if (points[i].x<a2 + 3 && points[i].x>a2 - 3 && points[i].y>t)
	//	{
	//		t = points[i].y;
	//	}*/
	//	Point2d p = points[i];
	//	if (abs(p.x - a2_miny2)<3)
	//		ylista2.push_back(p.y); //存储ylist
	//}
	//if (ylista2.empty())
	//	return false;
	//sort(ylista2.begin(), ylista2.end());
	vector<double> ylist2;
	for (int i = 0; i != points.size(); i++)
	{
		if (points[i].y >a1-120 && points[i].y< a1 + 50 && points[i].x < a2_miny2 + 110 && points[i].x > a2 - 5)
		{
			pointsA12.push_back(points[i]);
			ylist2.push_back(points[i].y);
		}
			

		//if (points[i].x > a2_miny2 +2 )
		//	pointsA1.push_back(points[i]);
		//else if ((a2_minx<points[i].x) && (points[i].x < (a2_miny2 + 4)))
		//	pointsA2.push_back(points[i]);
	}
	//float p[2];
	//fittingCurve(pointsA1,2,p);
	//for (int i = 0; i < pointsA1.size(); i++)
	//{
	//	if (abs(p[1] * pointsA1[i].x + p[0] - pointsA1[i].y)>5)
	//	{
	//		pointsA1.erase(pointsA1.begin() + i);
	//		i--;
	//	}

	//}
	sort(ylist2.begin(), ylist2.end());
	int ylistSize2 = ylist2.size();
	if (ylistSize2<3)
	{
	return false;
	}
	a1_maxy = (ylist2[ylistSize2 - 1] + ylist2[ylistSize2 - 2] + ylist2[ylistSize2 - 3]) / 3;
	a2_miny = ylist2[0];
	a2_minx = a2_miny2;
	/*if (pointsA2.size()<8)
	{
		return false;
	}
	pointsA2.erase(pointsA2.end() -8, pointsA2.end());	*/
	//vector<double> ylista1;
	//vector<double> xlista22;
		int colNumSize2 = a1_maxx - a2+25;
		vector<int> yCount2(colNumSize2, 0);
		vector<double> yValue2(colNumSize2, 0.0);
	for (int i = 0; i < pointsA12.size(); i++)
	{
		pointsA1Y.push_back(pointsA12[i].y);
		pointsA2X.push_back(pointsA12[i].x);
	}
	morphology_oc(pointsA2X, pointsA2X_Z2, 31);
	morphology_oc(pointsA1Y, pointsA1Y_Z2, 11);
	for (int i = 0; i < pointsA12.size(); i++)
	{
		Point2d p = pointsA12[i];
		pointsA12[i].x = pointsA2X_Z2[i];
		pointsA12[i].y = pointsA1Y_Z2[i];
		//ylista1.push_back(pointsA1Y_Z2[i]);
		//xlista22.push_back(pointsA2X_Z2[i]);
		if (a2 - 20 < p.x&&p.x < a1_maxx)
		{
			yCount2[floor(p.x) - a2 + 20]++;
			yValue2[floor(p.x) - a2 + 20] += p.x;
		}
		
	}
	//sort(ylista1.begin(), ylista1.end());
	//sort(xlista22.begin(), xlista22.end());
	//a1_maxy = (ylista1[ylista1.size() - 1] + ylista1[ylista1.size() - 2] + ylista1[ylista1.size() - 3]) / 3;
	maxIdx = 0;
	for (int i = 0; i < colNumSize2; i++)
	{
		if (yCount2[maxIdx] < yCount2[i])
		{
			maxIdx = i;
		}
		if (yCount2[i]>15)
		{
			maxIdx = i;
			break;
		}
	}
	a2 = yValue2[maxIdx] / yCount2[maxIdx];
	if (pointsA12.size()<4)
	{
		return false;
	}
	pointsA12.erase(pointsA12.end() - 4, pointsA12.end());
	if (a1 - a2_miny>70 && a1_num>20 && a2_num>20)
		a2_miny = a1;
	else
	{
		a2_minx = 0;
		a2_miny = 0;
	}	
	return true;

}
bool MeasureDistance::measureA3A4(vector<Point2d> points, double &a3, double &a4, double &a3_min, double &a4_min, 
	vector<Point2d> &pointsA3, vector<Point2d>& pointsA4, vector<Point2d>& pointsA5, double &a2_minx1, double &a2_miny1)
{
	if (points.empty() || points.size()<10)
	{
		return false;
	}
	vector<float>pointsA2X, pointsA2X_Z2;
	vector<float>pointsA1Y, pointsA1Y_Z2;
	//a4**************************************************************************************/
	points.erase(points.begin(), points.begin() + 5);
	points.erase(points.end() - 5, points.end());

	vector<double> ylist(points.size(), 0.0);
	vector<double> xlist(points.size(), 0.0);

	//计算a2时以1mm为单位存储在此1mm列内点的个数
	for (int i = 0; i != points.size(); i++)
	{
		Point2d p = points[i];
		ylist[i] = p.y; //存储ylist
		xlist[i] = p.x;

	}
	sort(ylist.begin(), ylist.end());
	sort(xlist.begin(), xlist.end());
	int maxIdy = 0;
	double dminY = ylist[0];
	double dmaxY = ylist[ylist.size() - 1];
	double dminX = xlist[0];
	/*if (a2_miny1>500 && a2_miny1 < 600)
	{
		if (dminY<370 && dmaxY>370)
		dminY = 370;
		if (dmaxY>600&&dminY<600)
		dmaxY = 600;
	}
	if (a2_miny1>300 && a2_miny1 < 400)
	{
		if (dminY<150 && dmaxY>150)
		dminY = 150;
		if (dmaxY>300 && dminY<300)
		dmaxY = 300;
	}*/
	int maxY = floor(dmaxY + 1);
	int minY = floor(dminY);
	int K = 0;
//	double a4min, a4min2;
	vector<int> xCount(maxY - minY, 0);
	vector<double> xValue(maxY - minY, 0.0);
	/*A4点查找*/
	for (int i = 0; i != points.size(); i++)
	{
		if (points[i].y > dminY&& points[i].y < maxY&& points[i].x>dminX + 10)
		{
			xCount[floor(points[i].y - dminY)]++;
			xValue[floor(points[i].y - dminY)] += points[i].y;
		}
	}
	for (int i = 0; i < maxY - minY; i++)
	{
		if (xCount[maxIdy] < xCount[i])
		{
			maxIdy = i;
		}
		if (xCount[i] > 15)
		{
			maxIdy = i;
			break;
		}
	}
	a4 = xValue[maxIdy] / xCount[maxIdy];

	double find_rowx_thres = 2;
	vector<double> rowsx;
	for (int i = 0; i != points.size(); i++)
	{
		if (points[i].x<MEAS_VALI_MAX_Z && abs(points[i].y - a4) < find_rowx_thres)
		{
			rowsx.push_back(points[i].x);
		}
	}
	if (rowsx.empty())
		return false;
	sort(rowsx.begin(), rowsx.end());
	double minx = (rowsx[0] + rowsx[1] + rowsx[2]) / 3;  //a4行对应的最小列坐标
	a4_min = minx + 5;
	/***********************************************************************************/
	/**a2**************************************************************************************/
	int colNumSize = MEAS_VALI_MAX_Z - MEAS_VALI_MIN_Z;
	vector<int> yCount2(colNumSize, 0);
	vector<double> yValue2(colNumSize, 0.0);
	for (int i = 0; i < points.size(); i++)
	{
		Point2d p = points[i];
		if (p.y>a4 + 2 && p.x<a4_min)
		{
			yCount2[floor(p.x) - MEAS_VALI_MIN_Z]++;
			yValue2[floor(p.x) - MEAS_VALI_MIN_Z] += p.x;
		}
	}
	int maxIdx = 0;  //存储拥有最多点的1mm列
	for (int i = 0; i < colNumSize; i++)
	{
		if (yCount2[maxIdx] < yCount2[i])
		{
			maxIdx = i;
		}
		if (yCount2[i] > 10)
		{
			maxIdx = i;
			break;
		}
	}
	//求此1mm内深度轴信息的均值g
	double a2_minx = yValue2[maxIdx] / yCount2[maxIdx];
	/******************************************************************/
	vector<float>pointsA3X;
	vector<float>pointsA3XX, pointsA3X_Z2;
	vector<float>pointsA5Y, pointsA5Y_Z2, pointsA5X, pointsA5X_Z2;
	vector<double> A3xlist;
	if (a2_miny1>400 && a2_miny1<620 && (a2_minx - a2_minx1)>30 && (a2_minx - a2_minx1)<60)
	{
		a3 = a2_minx;
		//a2_minx1 = a2_minx;
	}
	else
	{
		if (Test_Calibrate == 1)
		{
			if (a2_minx>670 && a2_minx<690)
			{
				minx = 685;
				a2_minx = 650;
			}
			if (a2_minx1<610 || a2_minx1>680)
			{
				minx = 685;
				a2_minx = 650;
			}
		}	
		a2_minx1 = a2_minx;
		//在点(minx,a4)周围范围内寻找a3值
		/*横向纵向点分开处理*/
		for (int i = 0; i != points.size(); i++)
		{
			if ((a4 - 10 < points[i].y) && (points[i].y<(a4 + 10)) && points[i].x>minx + 5  && points[i].x < minx + 70)
			{
				pointsA4.push_back(points[i]);
			}
			else if ((a2_minx + 14 < points[i].x) && (points[i].x < a2_minx + 80) && (points[i].y < a4 + 30) && (a4 - 10 < points[i].y))
			{
				pointsA3.push_back(points[i]);
			}
		}

		if (pointsA3.empty())
		{
			//a3 = a2_minx;

			pointsA3 = pointsA4;

			for (int i = 0; i < pointsA3.size(); i++)
			{
				pointsA3X.push_back(pointsA3[i].x);
			}
			morphology_oc(pointsA3X, pointsA3X_Z2, 3);
			for (int i = 0; i < pointsA3.size(); i++)
			{
				//pointsA2X_Z.push_back((pointsA2X_OC[i] + pointsA2X_CO[i]) / 2);
				pointsA3[i].x = pointsA3X_Z2[i];
				A3xlist.push_back(pointsA3[i].x);
			}
			a3 = 0;
			if (A3xlist.size() < 4)
				a3 = a2_minx;
			else
			{
				std::sort(A3xlist.begin(), A3xlist.end());
				int A3xlistSize = A3xlist.size();
				double a3minx = (A3xlist[0] + A3xlist[1] + A3xlist[2]) / 3;
				colNumSize = MEAS_VALI_MAX_Z - a2_minx + 6;
				vector<double> a3_count(colNumSize, 0.0);
				vector<double> a3_val(colNumSize, 0.0);
				int a3_max_count_idx = 0;
				for (int i = 0; i != pointsA3.size(); i++)
				{
					if ((MEAS_VALI_MAX_Z > pointsA3[i].x) && (a2_minx < pointsA3[i].x))
					{
						a3_count[floor(pointsA3[i].x - a2_minx + 5)]++;
						a3_val[floor(pointsA3[i].x - a2_minx + 5)] += pointsA3[i].x;
					}
				}
				for (int i = 1; i <colNumSize - 1; i++)
				{
					if (a3_count[i]>a3_count[a3_max_count_idx])//a3_count[a3_max_count_idx]
					{
						a3_max_count_idx = i;
					}
					if (a3_count[i] > 2)
					{
						a3_max_count_idx = i;
						break;
					}
				}
				//计算a3

				if (a3_count[a3_max_count_idx] > 0)
				{
					a3 = a3_val[a3_max_count_idx] / a3_count[a3_max_count_idx];
					//a3 = (a3_val[a3_max_count_idx - 1] + a3_val[a3_max_count_idx] + a3_val[a3_max_count_idx + 1]) / (a3_count[a3_max_count_idx - 1] + a3_count[a3_max_count_idx] + a3_count[a3_max_count_idx + 1]);
				}
				/*if (a3_count[a3_max_count_idx-1]+a3_count[a3_max_count_idx] + a3_count[a3_max_count_idx + 1]<3)
				a3 = a2_minx;*/
			}
		}
		else
		{
			//pointsA3.erase(pointsA3.begin(), pointsA3.begin()+4);

			for (int i = 0; i < pointsA3.size(); i++)
			{
				pointsA3X.push_back(pointsA3[i].x);
			}
			morphology_oc(pointsA3X, pointsA3X_Z2, 3);
			for (int i = 0; i < pointsA3.size(); i++)
			{
				//pointsA2X_Z.push_back((pointsA2X_OC[i] + pointsA2X_CO[i]) / 2);
				pointsA3[i].x = pointsA3X_Z2[i];
				A3xlist.push_back(pointsA3[i].x);
			}
			a3 = 0;
			if (A3xlist.size() < 4)
				a3 = a2_minx;
			else
			{
				std::sort(A3xlist.begin(), A3xlist.end());
				int A3xlistSize = A3xlist.size();
				double a3minx = (A3xlist[0] + A3xlist[1] + A3xlist[2]) / 3;
				colNumSize = MEAS_VALI_MAX_Z - a2_minx + 6;
				vector<double> a3_count(colNumSize, 0.0);
				vector<double> a3_val(colNumSize, 0.0);
				int a3_max_count_idx = 0;
				for (int i = 0; i != pointsA3.size(); i++)
				{
					if ((MEAS_VALI_MAX_Z > pointsA3[i].x) && (a2_minx < pointsA3[i].x))
					{
						a3_count[floor(pointsA3[i].x - a2_minx + 5)]++;
						a3_val[floor(pointsA3[i].x - a2_minx + 5)] += pointsA3[i].x;
					}
				}
				for (int i = 1; i <colNumSize - 1; i++) 
				{
					if (a3_count[i]>a3_count[a3_max_count_idx])//a3_count[a3_max_count_idx]
					{
						a3_max_count_idx = i;
					}
					if (a3_count[i] > 2)
					{
					a3_max_count_idx = i;
					break;
					}
				}
				//计算a3

				if (a3_count[a3_max_count_idx] > 0)
				{
					a3 = a3_val[a3_max_count_idx] / a3_count[a3_max_count_idx];
					//a3 = (a3_val[a3_max_count_idx - 1] + a3_val[a3_max_count_idx] + a3_val[a3_max_count_idx + 1]) / (a3_count[a3_max_count_idx - 1] + a3_count[a3_max_count_idx] + a3_count[a3_max_count_idx + 1]);
				}
				/*if (a3_count[a3_max_count_idx-1]+a3_count[a3_max_count_idx] + a3_count[a3_max_count_idx + 1]<3)
					a3 = a2_minx;*/
			}
		}
	}
		for (int i = 0; i != points.size(); i++)
		{
			if (points[i].x > a3 + 2 && points[i].x < a3 + 80 && (points[i].y < a4 + 30) && (points[i].y > a4 - 10))
			{
				pointsA5.push_back(points[i]);
			}
		}
		for (int i = 0; i < pointsA5.size(); i++)
		{
			pointsA5X.push_back(pointsA5[i].x);
			pointsA5Y.push_back(pointsA5[i].y);
		}
		morphology_oc(pointsA5X, pointsA5X_Z2, 3);
		morphology_oc(pointsA5Y, pointsA5Y_Z2, 3);
		for (int i = 0; i < pointsA5.size(); i++)
		{
			pointsA5[i].x = pointsA5X_Z2[i];
			pointsA5[i].y = pointsA5Y_Z2[i];
		}
		for (int i = 0; i < pointsA5.size(); i++)
		{
			if (pointsA5[i].y < a4 - 4 || pointsA5[i].x < a3 - 1)
			{
				pointsA5.erase(pointsA5.begin() + i);
				i--;
			}

		}
		vector<Point2d> pointA55;
		fitting(pointsA5, pointA55, 3);
		if (pointA55.empty())
			return false;
		std::sort(pointA55.begin(), pointA55.end(), [](Point2d a, Point2d b)
		{
			return a.y < b.y;
		});
		a4 = pointA55[0].y;
		vector<Point2d> pointA33;
		fitting(pointsA5, pointA33, 11);   //改动
		if (pointA33.empty())
			return false;
		std::sort(pointA33.begin(), pointA33.end(), [](Point2d a, Point2d b)
		{
			return a.x < b.x;
		});
		a3 = pointA33[0].x;//改动
		return true;	
}
bool MeasureDistance::measureA3A4(vector<Point2d> points, double &a3, double &a4, double &a3_min, double &a4_min,
	vector<Point2d> &pointsA3, vector<Point2d>& pointsA4, vector<Point2d>& pointsA5)
{
	if (points.empty() || points.size()<10)
	{
		return false;
	}
	vector<float>pointsA2X, pointsA2X_Z2;
	vector<float>pointsA1Y, pointsA1Y_Z2;
	//a4**************************************************************************************/
	points.erase(points.begin(), points.begin() + 5);
	points.erase(points.end() - 5, points.end());
	vector<double> ylist(points.size(), 0.0);
	vector<double> xlist(points.size(), 0.0);
	int colNumSize = MEAS_VALI_MAX_Z - MEAS_VALI_MIN_Z;
	
	//计算a2时以1mm为单位存储在此1mm列内点的个数
	for (int i = 0; i != points.size(); i++)
	{
		Point2d p = points[i];
		ylist[i] = p.y; //存储ylist
		xlist[i] = p.x;
		
	}
	std::sort(ylist.begin(), ylist.end());
	std::sort(xlist.begin(), xlist.end());
	int maxIdy = 0;
	double dminY = ylist[0];
	double dmaxY = ylist[ylist.size() - 1];
	double dminX = xlist[0];
	int maxY = floor(dmaxY + 1);
	int minY = floor(dminY);
	int K = 0;
	//double a4min, a4min2;
	vector<int> xCount(maxY - minY, 0);
	vector<double> xValue(maxY - minY, 0.0);
	/*A4点查找*/
	for (int i = 0; i != points.size(); i++)
	{
		if (points[i].y > dminY&& points[i].y < maxY&& points[i].x>dminX + 10)
		{
			xCount[floor(points[i].y - dminY)]++;
			xValue[floor(points[i].y - dminY)] += points[i].y;
		}
	}
	for (int i = 0; i < maxY - minY; i++)
	{
		if (xCount[maxIdy] < xCount[i])
		{
			maxIdy = i;
		}
		if (xCount[i] > 15)
		{
			maxIdy = i;
			break;
		}
	}
	a4= xValue[maxIdy] / xCount[maxIdy];

	/*找到a4点所在行里最小的列坐标*/
	double find_rowx_thres = 2;
	vector<double> rowsx;
	for (int i = 0; i != points.size(); i++)
	{
		if (points[i].x>MEAS_VALI_MIN_Z+50&&points[i].x<MEAS_VALI_MAX_Z && abs(points[i].y - a4) < find_rowx_thres)
		{
			rowsx.push_back(points[i].x);
		}
	}
	if (rowsx.size()<5)
		return false;
	std::sort(rowsx.begin(), rowsx.end());
	double minx = (rowsx[1] + rowsx[2] + rowsx[4]) / 3;  //a4行对应的最小列坐标
	a4_min = minx + 5;
	/***********************************************************************************/
	/**a2**************************************************************************************/
	vector<int> yCount2(colNumSize, 0);
	vector<double> yValue2(colNumSize, 0.0);
	for (int i = 0; i < points.size(); i++)
	{
		Point2d p = points[i];
		if (p.y>a4 + 5 && p.x<a4_min+10)
		{
			yCount2[floor(p.x) - MEAS_VALI_MIN_Z]++;
			yValue2[floor(p.x) - MEAS_VALI_MIN_Z] += p.x;
		}	
	}
	int maxIdx = 0;  //存储拥有最多点的1mm列
	for (int i = 0; i < colNumSize; i++)
	{
		if (yCount2[maxIdx] < yCount2[i])
		{
			maxIdx = i;
		}
		if (yCount2[i] >20)
		{
			maxIdx = i;
			break;
		}
	}
	//求此1mm内深度轴信息的均值g
 double a2_minx = yValue2[maxIdx] / yCount2[maxIdx];
 /**a3*************************************************************************************/
	
	//在点(minx,a4)周围范围内寻找a3值
	/*横向纵向点分开处理*/
	for (int i = 0; i != points.size(); i++)
	{
		if ((a4 - 10 < points[i].y) && (points[i].y<(a4 + 10)) && points[i].x>minx + 5 && points[i].x < minx + 70)
		{
			pointsA4.push_back(points[i]);
		}
		else if ((a2_minx + 15 < points[i].x) && (points[i].x < a2_minx + 80) && (points[i].y < a4 + 40) && (a4 - 10 < points[i].y))
		{
			pointsA3.push_back(points[i]);
		}
	}
	vector<float>pointsA3X;
	vector<float>pointsA3XX, pointsA3X_Z2;
	vector<float>pointsA5Y, pointsA5Y_Z2, pointsA5X, pointsA5X_Z2;
	vector<double> A3xlist;
	if (pointsA3.empty())
	{
		a3 = a2_minx;		
	}
	else
	{
		//pointsA3.erase(pointsA3.begin(), pointsA3.begin()+4);
		
		for (int i = 0; i < pointsA3.size(); i++)
		{
			pointsA3X.push_back(pointsA3[i].x);
		}
		morphology_oc(pointsA3X, pointsA3X_Z2, 3);
		for (int i = 0; i < pointsA3.size(); i++)
		{
			//pointsA2X_Z.push_back((pointsA2X_OC[i] + pointsA2X_CO[i]) / 2);
			pointsA3[i].x = pointsA3X_Z2[i];
			A3xlist.push_back(pointsA3[i].x);
		}
		a3 = 0;
		if (A3xlist.size() < 4)
			a3 = a2_minx;
		else
		{
			sort(A3xlist.begin(), A3xlist.end());
			int A3xlistSize = A3xlist.size();
			double a3minx = (A3xlist[0] + A3xlist[1] + A3xlist[2]) / 3;
			colNumSize = MEAS_VALI_MAX_Z - a2_minx + 6;
			vector<int> a3_count(colNumSize, 0);
			vector<double> a3_val(colNumSize, 0.0);
			int a3_max_count_idx = 0;
			for (int i = 0; i != pointsA3.size(); i++)
			{
				if ((MEAS_VALI_MAX_Z > pointsA3[i].x) && (a2_minx < pointsA3[i].x))
				{
					a3_count[floor(pointsA3[i].x - a2_minx + 5)]++;
					a3_val[floor(pointsA3[i].x - a2_minx + 5)] += pointsA3[i].x;
				}
			}
			for (int i = 0; i <colNumSize-1; i++)
			{
				if (a3_count[i]>a3_count[a3_max_count_idx])//a3_count[a3_max_count_idx]
				{
					a3_max_count_idx = i;
				}
				if (a3_count[i] > 4)
				{
					a3_max_count_idx = i;
					break;
				}
			}
			//计算a3

			if (a3_count[a3_max_count_idx] > 0)
			{
				a3 = a3_val[a3_max_count_idx] / a3_count[a3_max_count_idx];
			}
			if (a3_count[a3_max_count_idx]+a3_count[a3_max_count_idx+1]<7)
				a3 = a2_minx;
		}
	}
		for (int i = 0; i != points.size(); i++)
		{
			if (points[i].x >a3 - 2 && points[i].x <a3 + 80 && (points[i].y < a4 + 30) && (points[i].y > a4 - 10))
			{
				pointsA5.push_back(points[i]);
			}
		}
		for (int i = 0; i < pointsA5.size(); i++)
		{
			pointsA5X.push_back(pointsA5[i].x);
			pointsA5Y.push_back(pointsA5[i].y);
		}
		morphology_oc(pointsA5X, pointsA5X_Z2, 3);
		morphology_oc(pointsA5Y, pointsA5Y_Z2, 3);
		for (int i = 0; i < pointsA5.size(); i++)
		{
			pointsA5[i].x = pointsA5X_Z2[i];
			pointsA5[i].y = pointsA5Y_Z2[i];
		}
		for (int i = 0; i < pointsA5.size(); i++)
		{
			if (pointsA5[i].y < a4 - 4 || pointsA5[i].x < a3 - 3)
			{
				pointsA5.erase(pointsA5.begin() + i);
				i--;
			}

		}
		vector<Point2d> pointA55;
		fitting(pointsA5, pointA55, 3);
		if (pointA55.empty())
			return false;
		sort(pointA55.begin(), pointA55.end(), [](Point2d a, Point2d b)
		{
			return a.y< b.y;
		});
		a4 = pointA55[0].y;
		vector<Point2d> pointA33;
		fitting(pointsA5, pointA33);
		if (pointA33.empty())
			return false;
		sort(pointA33.begin(), pointA33.end(), [](Point2d a, Point2d b)
		{
			return a.x< b.x;
		});
		a3 = pointA33[0].x;
		//vector<double> rowsa3;
		//for (int i = 0; i != pointsA5.size(); i++)
		//{
		//	if (pointsA5[i].x<MEAS_VALI_MAX_Z && pointsA5[i].x>a3 && abs(pointsA5[i].y - a4) < 0.5)
		//	{
		//		rowsa3.push_back(pointsA5[i].x);
		//	}
		//}
		//if (rowsa3.empty())
		//	return false;
		//sort(rowsa3.begin(), rowsa3.end());
		//int s = rowsa3.size();
		//a3 = (rowsa3[s - 3] + rowsa3[s - 2] + rowsa3[s - 1] ) / 3;  //a4行对应的最小列坐标
		/***************************/
	//a3 = (pointsA3[0].x + pointsA3[0].x + pointsA3[0].x) / 3;
	return true;
}

bool MeasureDistance::deal3DPoints(const vector<Point3d>& real_points, vector<Point2d>& outPoints2D, const Mat& rotateMat, double camHeigh)
{
	if (real_points.empty())
	{
		return false;
	}

	//旋转矩阵
	Matx33d rotate(rotateMat);
	vector<Point2d> tmpPoints2D;

	for (int i = 0; i != real_points.size(); i++)
	{
		Point3d p = real_points[i];
		//过滤
		if (p.z < 0 || p.z >MEAS_MAX_Z)
		{
			continue;
		}

		//旋转
		p.y = -1.0 * p.y;

		//bug fix 2017.7.5
		Matx31d pV(p.x, p.y, p.z);
		Matx31d tmp = rotate*pV;
		p.x = tmp(0);
		p.y = tmp(1);
		p.z = tmp(2);

		//过滤噪声点
		if (p.z < MEAS_VALI_MIN_Z-200 || p.z > MEAS_VALI_MAX_Z-100)
			continue;
		tmpPoints2D.push_back(Point2f(p.z, p.y));
		
	}

	////判断点领域内是否拥有足够多的点，如果没有去除
	do2DPointsOpt(tmpPoints2D, outPoints2D);
	//outPoints2D = tmpPoints2D;
	return true;
}
bool MeasureDistance::do2DPointsOpt(const vector<Point2d>& inPoints2D, vector<Point2d>& outPoints2D)
{
	if (inPoints2D.empty())
	{
		return false;
	}

	//判断点领域内是否拥有足够多的点，如果没有去除
	int range =15;
	double DIS_THRES =10;
	int dealPointSize = inPoints2D.size();
	vector<int> forwardDis(dealPointSize, 0);
	vector<int> backDis(dealPointSize, 0);
	//init forwardDis，前向扫描
	for (int i = 0; i < dealPointSize - range; i++)
	{
		int disCount = 0;
		for (int j = 0; j < range; j++)
		{
			double dis = (inPoints2D[i].x - inPoints2D[i + j].x)*(inPoints2D[i].x - inPoints2D[i + j].x)
				+ (inPoints2D[i].y - inPoints2D[i + j].y)*(inPoints2D[i].y - inPoints2D[i + j].y);
			dis = sqrt(dis);
			if (dis < DIS_THRES)
			{
				disCount++;
			}

		}
		forwardDis[i] = disCount;
	}

	//init backDis，后向扫描
	for (int i = range; i < dealPointSize; i++)
	{
		int disCount = 0;
		for (int j = 0; j < range; j++)
		{
			double dis = (inPoints2D[i].x - inPoints2D[i - j].x)*(inPoints2D[i].x - inPoints2D[i - j].x)
				+ (inPoints2D[i].y - inPoints2D[i - j].y)*(inPoints2D[i].y - inPoints2D[i - j].y);
			dis = sqrt(dis);
			if (dis < DIS_THRES)
			{
				disCount++;
			}

		}
		backDis[i] = disCount;
	}

	for (int i = 0; i < dealPointSize; i++)
	{
		//去除
		if ((forwardDis[i] > COUNT_THRES) || (backDis[i]>COUNT_THRES))
		{
			outPoints2D.push_back(inPoints2D[i]);
		}
	}

	return true;
}

bool MeasureDistance::deal2DPoints(vector<Point2d>& points2D, Mat* const rotateMat, Mat* const translation)
{
	if (points2D.empty())
	{
		return false;
	}

	//旋转矩阵
	//if (!(*rotateMat).empty())
	//{
	//	Matx33d rotate(*rotateMat);
	//	Mat tmpEye = Mat::eye(rotateMat->size(), rotateMat->type());
	//	Matx33d tmp(tmpEye);
	//	tmp = rotate - tmp;
	//	int nozeroCount = cv::countNonZero(tmp);
	//	if (nozeroCount != 0)
	//	{
	//		for (int i = 0; i != points2D.size(); i++)
	//		{
	//			Point2d p = points2D[i];
	//			//旋转
	//			double tmp_y = p.y*rotate(1, 1) + p.x*rotate(1, 2);
	//			double tmp_x = p.y*rotate(2, 1) + p.x*rotate(2, 2);
	//			points2D[i].y = tmp_y;
	//			points2D[i].x = tmp_x;
	//		}
	//	}
	//}
	//平移向量
	if (!(*translation).empty() && (*translation).size() == Size2i(1, 2))
	{
		Matx21d transData(*translation);
		if (transData(0, 0) != 0.0 || transData(1, 0) != 0.0)
		{
			for (int i = 0; i != points2D.size(); i++)
			{
				points2D[i].x += transData(0, 0);
				points2D[i].y += transData(1, 0);
			}
		}
	}

	//判断是否超出测量范围
	for (int i = 0; i != points2D.size(); i++)
	{
		if (points2D[i].x<MEAS_VALI_MIN_Z || points2D[i].x>MEAS_VALI_MAX_Z || points2D[i].y>MEAS_VALI_MAX_Y || points2D[i].y<MEAS_VALI_MIN_Y)
		{
			points2D.erase(points2D.begin() + i);
			i--;
		}
	}

	return true;

}

bool MeasureDistance::deal2DPoints(vector<Point2d>& points2D, double theta, double xT, double yT)
{
	Mat rotateMat;
	double angle = theta / 180 * M_PI;
	double dat1[] = { 1, 0, 0, 0, cos(angle), -1 * sin(angle), 0, sin(angle), cos(angle) };
	rotateMat = Mat(3, 3, CV_64FC1, dat1);

	double data[] = { xT, yT };
	Mat transData(2, 1, CV_64FC1, data);
	deal2DPoints(points2D, &rotateMat, &transData);

	return true;
}

bool MeasureDistance::fillyzList(vector<Point2d>& points2D, vector<double>& ylist, vector<double>& zlist)
{
	if (points2D.empty())
	{
		return false;
	}
	for (int i = 0; i != points2D.size(); i++)
	{
		ylist.push_back(points2D[i].y);
		zlist.push_back(points2D[i].x);

	}
	return true;
}

bool MeasureDistance::cacul(const vector<Mat>& measImgs, const CameraCalib::CalibrationParames& upCalibParams, const CameraCalib::CalibrationParames& downCalibParams,
	vector<Point3d>& upRealPoints, vector<Point3d>& downRealPoints)
{
	if (measImgs.empty() || measImgs.size()!=4)
		return false;
	//处理上部相机
		Mat upMat, downMat;
		subtract(measImgs[1], measImgs[0], upMat); //上部图像
		subtract(measImgs[3], measImgs[2], downMat); //下部图像
		//切分左右视图
		Size2i img_size1(upMat.cols / 2, upMat.rows);
		Mat img_left = upMat(Rect(0, 0, img_size1.width, img_size1.height));
		Mat img_right = upMat(Rect(img_size1.width, 0, img_size1.width, img_size1.height));
		//开始测量
		Mat upLeftCenter, upRithtCenter;
		Mat upMatch;
		caculateDistance(img_left, img_right, upCalibParams, upRealPoints, upLeftCenter, upRithtCenter, upMatch, 0, 1, 2);
	
		Mat d_img_left = downMat(Rect(0, 0, img_size1.width, img_size1.height));
		Mat d_img_right = downMat(Rect(img_size1.width, 0, img_size1.width, img_size1.height));
	//处理下部相机

		Mat downLeftCenter, downRithtCenter;
		Mat downMatch;//
		caculateDistance(d_img_left, d_img_right, downCalibParams, downRealPoints, downLeftCenter, downRithtCenter, downMatch, 1, 2, 2);
		return true;
}

bool MeasureDistance::cacul(const vector<Mat>& measImgs, const vector<Mat>& measImgs2, const CameraCalib::CalibrationParames& upCalibParams, const CameraCalib::CalibrationParames& downCalibParams,
	vector<Point3d>& upRealPoints, vector<Point3d>& downRealPoints)
{
	if (measImgs.empty() || measImgs.size() != 4 || measImgs2.size() != 4)
		return false;
	//处理上部相机

	vector<Mat>img_left1, img_right1, img_left2, img_right2;
	
		Mat upMat, downMat;
		subtract(measImgs[1], measImgs[0], upMat); //上部图像
		subtract(measImgs[3], measImgs[2], downMat); //下部图像
		//切分左右视图
		Size2i img_size1(upMat.cols / 2, upMat.rows);
		Mat img_left = upMat(Rect(0, 0, img_size1.width, img_size1.height));
		Mat img_right = upMat(Rect(img_size1.width, 0, img_size1.width, img_size1.height));
		img_left1.push_back(img_left);
		img_right1.push_back(img_right);
		Mat d_img_left = downMat(Rect(0, 0, img_size1.width, img_size1.height));
		Mat d_img_right = downMat(Rect(img_size1.width, 0, img_size1.width, img_size1.height));
		img_left2.push_back(d_img_left);
		img_right2.push_back(d_img_right);
	
		subtract(measImgs2[1], measImgs2[0], upMat); //上部图像
		subtract(measImgs2[3 ], measImgs2[2], downMat); //下部图像
		//切分左右视图
		 img_left = upMat(Rect(0, 0, img_size1.width, img_size1.height));
		 img_right = upMat(Rect(img_size1.width, 0, img_size1.width, img_size1.height));
		img_left1.push_back(img_left);
		img_right1.push_back(img_right);
		d_img_left = downMat(Rect(0, 0, img_size1.width, img_size1.height));
		d_img_right = downMat(Rect(img_size1.width, 0, img_size1.width, img_size1.height));
		img_left2.push_back(d_img_left);
		img_right2.push_back(d_img_right);

	//开始测量
	Mat upLeftCenter, upRithtCenter;
	Mat upMatch;
	caculateDistance2(img_left1, img_right1, upCalibParams, upRealPoints, upLeftCenter, upRithtCenter, upMatch, 0, 1, 2);


	//处理下部相机

	Mat downLeftCenter, downRithtCenter;
	Mat downMatch;//
	caculateDistance2(img_left2, img_right2, downCalibParams, downRealPoints, downLeftCenter, downRithtCenter, downMatch, 1, 2, 2);
	return true;
}
bool MeasureDistance::GetDisparityRIO(cv::Mat& image1, Point3f &a,int Up_Down,int left_right)
{	
	int RIOthreshold, height_weit=1;
	RIOthreshold = OtsuAlgThreshold(image1);//灰度阈值最低值
	a.z = 0;                          //光带面积较小标志
	TRANS_THRESHOLD = 1;            //strger方法灰度阈值
	EIGENVALUE_THRESHOLD = 0.0001;  //海森矩阵特征值判断
	COUNT_THRES = 4;                //全局变量——剔除点的个数
	COUNT_THRES2 =6;
	for (int j = 0; j < 2; j++)   
	{
		vector<vector<Point>> contors;
		vector<vector<Point>> contors2;
		Mat out = Mat::zeros(image1.rows, image1.cols, CV_8UC1);
		//Mat result(out.size(), CV_8U, Scalar(255));
		out = image1.clone();
		
		int left_j, right_j;
		if (Up_Down == 0)
		{
			if (j==0)
			{   //第一次循环初值
				TRANS_THRESHOLD=4;
				COUNT_THRES = 4;//全局变量——剔除点的个数
				COUNT_THRES2 = 6;
			}		
			threshold(out, out, RIOthreshold, 255, 0);
			erode(out, out, Mat());
			dilate(out, out, Mat());
			erode(out, out, Mat());	
			//height_weit = 2;
			
			if (left_right == 0)
			{
				a.y = 0;//shangxiabiaozhi
				a.x = 780;
				a.x = atof(GetPropertyByName("layer position", "up left x").c_str());
				left_j = a.x - R_L;
				right_j = a.x + R_L;
			}
			else
			{
				a.y = 1;//shangxiabiaozhi
				a.x = 450;
				a.x = atof(GetPropertyByName("layer position", "up right x").c_str());
				left_j = a.x - R_L;
				right_j = a.x + R_L;
			}
		}
		else
		{
			if (j == 0)
			{   //第一次循环初值
				TRANS_THRESHOLD = 2;
				COUNT_THRES = 4;
				COUNT_THRES2 = 6;
			}		
			threshold(out, out, RIOthreshold, 255, 0);
			dilate(out, out, Mat());
			erode(out, out, Mat());
			dilate(out, out, Mat());
			//height_weit = 2;
			
			if (left_right == 0)
			{
				a.y = 0;
				a.x = 710;
				a.x = atof(GetPropertyByName("layer position", "down left x").c_str());
				left_j = a.x - R_L;
				right_j = a.x + R_L;
			}
			else
			{
				a.y = 1;
				a.x = 570;
				a.x = atof(GetPropertyByName("layer position", "down right x").c_str());
				left_j = a.x - R_L;
				right_j = a.x + R_L;
			}
		}


		findContours(out, contors, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		//drawContours(result, contors, -1, 0, 2);
		RotatedRect ty, ty2;
		int s = 0, b = 0;
		float s2=0;
		vector<float> centers;
		for (int i = 0; i < contors.size(); i++)
		{
			ty2 = minAreaRect(contors[i]);
			double d = ((ty2.size.height + 1) / (ty2.size.width + 1));
			if (d < 1)
			{
				d = 1 / d;
			}
			if (s2 < (contors[i].size()) && ty2.center.x>left_j && ty2.center.x < right_j&&d>height_weit)//(s<(contourArea(contors[i])&&)	
				//if (s < (contourArea(contors[i])))
			{
				s = contourArea(contors[i]);
				s2 = contors[i].size();
				a.x = int(ty2.center.x);
			}

		}
		if (s2==0)
		{
			a.x = 0;
			RIOthreshold = RIOthreshold + 30;
		}
		else if (s>10000&&j==0)
		{
			RIOthreshold = RIOthreshold + 50;
			height_weit = 2;
			a.z = 1;//像素值高
			TRANS_THRESHOLD = 60;
			EIGENVALUE_THRESHOLD = 0.001;
			COUNT_THRES = 8;
		}
		else if (s>4000&&j==0)
		{
			RIOthreshold = RIOthreshold + 30;
			height_weit = 2;
			a.z = 2;//像素值中
			TRANS_THRESHOLD = 40;
			COUNT_THRES = 6;
		}
		else if (s>1800&&j==0)
		{
			RIOthreshold = RIOthreshold + 20;
			a.z = 3;//像素值低
			COUNT_THRES = 5;
			TRANS_THRESHOLD = 20;
			if (Up_Down == 0)
			{
				TRANS_THRESHOLD = 50;
			}
		}
		else if (s>900 && j == 0)
		{
			RIOthreshold = RIOthreshold + 10;
			TRANS_THRESHOLD = 10;
			if (Up_Down == 0)
			{
				TRANS_THRESHOLD = 40;
			}	
			COUNT_THRES2 =10;
			COUNT_THRES = 4;
		}
		else if (s>600 && s<1000)
		{
			if (Up_Down == 0)
			{
				TRANS_THRESHOLD = 30;
			}
			COUNT_THRES = 4;
			COUNT_THRES2 = 6;
			break;
		}
		else
			break;
	}	
	return true;
}
bool MeasureDistance::fitting(vector<Point2d> &pointsIn, vector<Point2d>&pointsOut)//GetDisparityRIO   fitting
{
	if (pointsIn.size() <10)
	{
		return false;
	}
	/*sort(pointsIn.begin(), pointsIn.end(),
		[](Point2d a, Point2d b)
	{
		return a.y< b.y;
	});*/

	Point2d out;
	for (int i = 0; i < pointsIn.size()-10;i++)
	{
		vector<Point2d> tem;
		vector<Point2d> tem2;
			tem.push_back(pointsIn[i]);
			int n = 0;
			for (int j = 1; (i + j<pointsIn.size()) && (abs(pointsIn[i + j].y - pointsIn[i].y)<1); j++)
			{
				tem.push_back(pointsIn[i + j]);
			}
			n = tem.size();
			if (n == 0)
				continue;
			double sum = 0, sumx = 0;
			for (int t = 0; t <n; t++)
			{
				sum = tem[t].x + sum;
				//sumx = tem[t].x + sumx;
			}
			double mean = sum / n; //均值
			double accum = 0;

			for (int t = 0; t < n; t++)
			{
				accum += (tem[t].x - mean)*(tem[t].x - mean);
			}
			double stdev = sqrt(accum / n);//方差

			for (int t = 0; t < n; t++)
			{
				if (abs((tem[t].x - mean))<3*stdev)
				{
					tem2.push_back(tem[t]);
				}
			}
			double sum2 = 0;
			for (int t = 0; t <tem2.size(); t++)
			{
				sum2 = tem2[t].x + sum2;
				//sumx = pointsIn[i+t].x+sumx;
			}
			if (tem2.size() == 0)
				continue;
			out.x = sum2 / tem2.size(); //均值
			out.y = pointsIn[i].y +0.5;
			if (tem2.size() != 0)
			{
				pointsOut.push_back(out);
			}
	}
	return true;
}

bool MeasureDistance::fitting(vector<Point2d> &pointsIn, vector<Point2d>&pointsOut, int m)//P1-P5 fitting

{
	vector<Point2d> pointsMd;
	if (pointsIn.size() < 10)
	{
		return false;
	}
	Point2d out;
	for (int i = 0; i < pointsIn.size() -10;)
	{
		vector<Point2d> tem;
		vector<Point2d> tem2;
		tem.push_back(pointsIn[i]);
		int n = 0;

		for (int j = 1; (i + j<pointsIn.size()) && (abs(pointsIn[i + j].x - pointsIn[i].x)<3); j++)
		{
			tem.push_back(pointsIn[i + j]);
		}

		n = tem.size();
		double sum = 0, sumx = 0;
		for (int t = 0; t <n; t++)

		{
			sum = tem[t].y + sum;
			//sumx = tem[t].x + sumx;
		}

		double mean = sum / n; //均值
		double accum = 0;

		for (int t = 0; t < n; t++)
		{
			accum += (tem[t].y - mean)*(tem[t].y - mean);
		}

		double stdev = sqrt(accum / n);//方差

		for (int t = 0; t < n; t++)
		{
			if (abs((tem[t].y - mean))<stdev)
			{
				tem2.push_back(tem[t]);
			}
		}

		double sum2 = 0;
		for (int t = 0; t <tem2.size(); t++)
		{
			sum2 = tem2[t].y + sum2;
			//sumx = pointsIn[i+t].x+sumx;
		}

		out.y = sum2 / tem2.size(); //均值
		out.x = pointsIn[i].x + 1.5;
		if (tem2.size() != 0)
		{
			pointsOut.push_back(out);
		}	
		i = i + 1;
	}
	//do2DPointsOpt(pointsMd, pointsOut);
	return true;
}


bool MeasureDistance::fitting_med(vector<Point2f> &pointsIn, vector<Point2f>&pointsOut)//P1-P5 fitting

{
	float stdev_nun;
	if (COUNT_THRES2==10)
		stdev_nun = 0.6;
	else
		stdev_nun = 0.8;
	vector<Point2f> pointsMd;
	vector<float> stdevs;
	if (pointsIn.size() < COUNT_THRES2)
	{
		return false;
	}
	Point2f out;
	for (int i = 0; i < pointsIn.size() - COUNT_THRES2;)
	{
		vector<Point2f> tem;
		vector<Point2f> tem2;
		tem.push_back(pointsIn[i]);
		int n = 0;

		for (int j = 1; j<COUNT_THRES2; j++) //(i + j<pointsIn.size()) && (abs(pointsIn[i + j].x - pointsIn[i].x)<4)
		{
			tem.push_back(pointsIn[i + j]);
		}

		n = tem.size();
		float sum = 0, sumx = 0;
		for (int t = 0; t <n; t++)

		{
			sum = tem[t].x + sum;
			//sumx = tem[t].x + sumx;
		}

		float mean = sum / n; //均值
		float accum = 0;

		for (int t = 0; t < n; t++)
		{
			accum += (tem[t].x - mean)*(tem[t].x - mean);
		}

		float stdev = sqrt(accum / n);//方差
		if (stdev < stdev_nun)
		{
			pointsOut.insert(pointsOut.end(),tem.begin(),tem.end());
		}
		stdevs.push_back(stdev);
		i = i + COUNT_THRES2;
	}
	//do2DPointsOpt(pointsMd, pointsOut);
	return true;
}
bool MeasureDistance::fitting_intrest(vector<Point2f> &pointsIn, vector<Point2f>&pointsOut)//P1-P5 fitting
{
	vector<Point2f> pointsMd;
	vector<Point2f> stdevs;
	int COUNT_THRES3=3;
	if (pointsIn.size() < COUNT_THRES3)
	{
		return false;
	}
	Point2f out;
	for (int i = 0; i < pointsIn.size() - COUNT_THRES3;)
	{
		if (i < 550)
		{
			pointsOut.push_back(pointsIn[i]);
			i++;
			continue;
		}
		vector<Point2f> tem;
		vector<Point2d> tem2;
		tem.push_back(pointsIn[i]);
		int n = 0;

		for (int j = 1; j<COUNT_THRES3; j++) //(i + j<pointsIn.size()) && (abs(pointsIn[i + j].x - pointsIn[i].x)<4)
		{
			tem.push_back(pointsIn[i + j]);
		}

		n = tem.size();
		float p[3];
		for (int k = 0; k < tem.size(); k++)
			tem2.push_back(Point2d(tem[k]));
	  fittingCurve(tem2, 3, p);
		if (p[1] < 0.5)
		{
			pointsOut.insert(pointsOut.end(), tem.begin(), tem.end());
		}
		stdevs.push_back(Point2f(p[0], p[1]));
		i = i + COUNT_THRES3;
	}
	//do2DPointsOpt(pointsMd, pointsOut);
	return true;
}
bool MeasureDistance::DetectLinerCenterM(cv::Mat &src, cv::Mat &dst, vector<cv::Point2f>& vSubCenterPoint, Point3f&a, vector<int> &lineWidth)
{
	vSubCenterPoint.clear();
	cv::Mat Image, imagegray;
	Image = src.clone();
	//Image=cv::imread("1.bmp",1);
	const int Width = Image.cols;
	const int Height = Image.rows;
	if (a.x < R_L2 || a.x > Width - R_L2)
		return false;
	int Fiter;
	float Minintr1, Minintr2, Minintr3, Multiple;
	if (a.z ==0)          //做差后激光非常暗
	{
		Fiter = 5;//滤波系数
		Minintr1 = 0.7;
		Minintr2 = 0.2;
		Minintr3 = 0.3;
	}
	else if (a.z ==2)        //比较亮
	{
		Fiter = 5;
		Minintr1 = 0.6;
		Minintr2 = 0.3;
		Minintr3 = 0.4;
	}
	else if (a.z == 3)       //暗
	{
		Fiter = 5;
		Minintr1 = 0.5;
		Minintr2 = 0.2;
		Minintr3 = 0.3;
	}
	else                   //非常亮a.z == 1
	{
		Fiter = 5;
		Minintr1 = 0.6;
		Minintr2 = 0.4;
		Minintr3 = 0.5;
	}
	imagegray = src.clone();
	vector<float>MedfilGrayImageMax, MedfilGrayImageMaxDown;
	Point2f E;
	for (int i = 0; i < Height; i++)
	{
		vector<uchar>B(Width, 0);
		for (int j = a.x - R_L2; j < a.x + R_L2; j++)
		{		
				B[j] = Image.at<uchar>(i, j);
		}
		sort(B.begin(), B.end());
		float  maxp = B[B.size() - 1];
		int K = 0;
		for (int i = 0; i<B.size(); i++)
		{
			if (B[i]>maxp*Minintr2)
				K++;
		}
		float minp;
		if (K >12)
		{
			if (maxp<150)
			{
				minp = maxp*Minintr1;
			}
			else
			minp = maxp*Minintr3;
		}
		else if (K > 6)
		{
			minp = maxp*Minintr3;
		}
		else
		{
			minp = maxp*Minintr2;
		}
		//minMaxIdx(Row, minp, maxp);
		/**minp = *maxp - 4;*/
		MedfilGrayImageMax.push_back(maxp);
		MedfilGrayImageMaxDown.push_back(minp);
	}
	vector<float> C(Height);//数组存储每行中心位置;
	vector<float> D;
	vector<float>MedfilGrayImageMaxCol_L(Height), MedfilGrayImageMaxCol_L2;
	vector<float>MedfilGrayImageMaxCol_R(Height), MedfilGrayImageMaxCol_R2;
	for (int i = 0; i < Height; i++)
	{
		bool	FindStartFlag = 0; //找到亮带标识
		bool	MeetMaxFlag = 0;   //找到最大值标识
		bool	FillFlag = 1;     //填充使能标识
		C[i] = 0;
		MedfilGrayImageMaxCol_L[i] = 0;
		MedfilGrayImageMaxCol_R[i] = 0;
		vector<float>MedfilGrayImageMaxCol;
		if (a.y == 1)
		{
			for (int j = a.x - R_L2; j < a.x + R_L2; j++)
			{
				float xiangshu = imagegray.at<uchar>(i, j);
				float xiangshu2 = imagegray.at<uchar>(i, j+1);
				if (xiangshu>MedfilGrayImageMaxDown[i] )
				{
						MedfilGrayImageMaxCol.push_back(j);
						FindStartFlag = 1;
						if (xiangshu == MedfilGrayImageMax[i])
							MeetMaxFlag = 1;
				}
				else if (FindStartFlag == 1)
				{
					//if (a.y == 0 && (MeetMaxFlag == 0)) // 找到亮带后，但是还没找到最大值时又低于亮带阈值，则从新开始填充
					//{
					//	MedfilGrayImageMaxCol.clear();
					//}
					//else // 找到亮带和最大值时，如果出现低于亮带阈值的就不再填充
					if (xiangshu2<MedfilGrayImageMaxDown[i] && MeetMaxFlag == 1)
						break;
					else 
					{
						MedfilGrayImageMaxCol.push_back(j);
					}
				}
			}
		}
		else
		{
			for (int j = a.x + R_L2; j > a.x - R_L2; j--)
			{
				float xiangshu = imagegray.at<uchar>(i, j);
				float xiangshu2 = imagegray.at<uchar>(i, j - 1);
				if (xiangshu>MedfilGrayImageMaxDown[i] )
				{
					    MedfilGrayImageMaxCol.push_back(j);
						FindStartFlag = 1;
						if (xiangshu ==MedfilGrayImageMax[i])
							MeetMaxFlag = 1;
				}
				else if (FindStartFlag == 1)
				{
					//if (a.y == 0 && (MeetMaxFlag == 0)) // 找到亮带后，但是还没找到最大值时又低于亮带阈值，则从新开始填充
					//{
					//	MedfilGrayImageMaxCol.clear();
					//}
					//else // 找到亮带和最大值时，如果出现低于亮带阈值的就不再填充
					if (xiangshu2<MedfilGrayImageMaxDown[i] && MeetMaxFlag == 1)
						break;
					else  
					{
						MedfilGrayImageMaxCol.push_back(j);
					}
				}
			}
		}	
		if (MedfilGrayImageMaxCol.size() >0)
		{
			if (a.y == 1)
			{
				int T = MedfilGrayImageMaxCol.size() - 1;
				MedfilGrayImageMaxCol_L[i] = MedfilGrayImageMaxCol[0];
				MedfilGrayImageMaxCol_R[i] = MedfilGrayImageMaxCol[T];
				C[i] = (MedfilGrayImageMaxCol[0] + MedfilGrayImageMaxCol[T]) / 2;
			}
			else
			{
				int T = MedfilGrayImageMaxCol.size() - 1;
				MedfilGrayImageMaxCol_L[i] = MedfilGrayImageMaxCol[T];
				MedfilGrayImageMaxCol_R[i] = MedfilGrayImageMaxCol[0];
				C[i] = (MedfilGrayImageMaxCol[0] + MedfilGrayImageMaxCol[T]) / 2;
			}
		}

	}
	medianfilter(MedfilGrayImageMaxCol_L, MedfilGrayImageMaxCol_L2, Fiter);
	medianfilter(MedfilGrayImageMaxCol_R, MedfilGrayImageMaxCol_R2, Fiter);
	for (int i = 0; i < MedfilGrayImageMaxCol_L.size(); i++)
	{
		if (MedfilGrayImageMaxCol_L[i] < MedfilGrayImageMaxCol_L2[i])
		{
			MedfilGrayImageMaxCol_L2[i] = MedfilGrayImageMaxCol_L[i];
		}
	}
	for (int i = 0; i < MedfilGrayImageMaxCol_R.size(); i++)
	{
		if (MedfilGrayImageMaxCol_R[i] > MedfilGrayImageMaxCol_R2[i])
		{
			MedfilGrayImageMaxCol_R2[i] = MedfilGrayImageMaxCol_R[i];
		}
	}
	/*MedfilGrayImageMaxCol_L2 = MedfilGrayImageMaxCol_L;
	MedfilGrayImageMaxCol_R2 = MedfilGrayImageMaxCol_R;*/
	D = C;
	//medianfilter(C, D, Fiter);
	for (int i = 0; i < Height; i++)
	{
		vector<float> input, output;
		if (abs(MedfilGrayImageMaxCol_L2[i] - MedfilGrayImageMaxCol_R2[i]) < 16 )
		{
			lineWidth[i] = MedfilGrayImageMaxCol_R2[i] - MedfilGrayImageMaxCol_L2[i];
			for (int j = int(MedfilGrayImageMaxCol_L2[i]); j <=(MedfilGrayImageMaxCol_R2[i]); j++)
			{
					//input.push_back(imagegray.at<uchar>(i, j));
					dst.at <uchar>(i, j) = imagegray.at<uchar>(i, j);
			}
		}
		else if (D[i]>8 && D[i]<Width-8)
		{
			lineWidth[i] = 16;
			for (int j = int(D[i] - 8); j < int(D[i] + 8); j++)
			{			
					//input.push_back(imagegray.at<uchar>(i, j));
				dst.at <uchar>(i, j) = imagegray.at<uchar>(i, j);
			}
		}
		/*output = input;
		gaussianfilter(input, output, 1);
		if (abs(MedfilGrayImageMaxCol_L2[i] - MedfilGrayImageMaxCol_R2[i]) < 16)
		{
			for (int j = int(MedfilGrayImageMaxCol_L2[i]), k = 0; j <=(MedfilGrayImageMaxCol_R2[i]); j++)
			{
				dst.at <uchar>(i, j) = output[k];
				k++;
			}
		}
		else if(D[i]>8 && D[i]<Width - 8)
		{
			for (int j = int(D[i] - 8), k = 0; j < int(D[i] + 8); j++)
			{
				dst.at <uchar>(i, j) = output[k];
				k++;
			}
		}*/
		//if (dst.at<uchar>(i, int(D[i]))<50)
		//dst.at<uchar>(i, int(D[i])) = 100;
	}
	Mat Img = dst.clone();
	for (int i = 0; i < Height; i++)
	{
		E.x = D[i];
		E.y = i;
		Img.at<uchar>(i, int(D[i])) = 255;
		vSubCenterPoint.push_back(E);
	}
	/*ofstream fout3("D:\\document\\Debug\\CtrPoint2.txt");
	fout3 << "-------------------------------------------" << endl << endl << endl;
	for (int i = 0; i != Height; i++)
	{
		fout3<< D[i] << "," <<i<< endl;
	}*/

	return true;
}

void MeasureDistance::medianfilter(vector<float>& signal, vector<float>& result, int N)
{
	//   Move window through all elements of the signal
	for (int i = 0; i < signal.size(); i++)
	{
		//   Pick up window elements
		vector<float> window;
		for (int j = 0; j < 2 * N + 1; ++j)
		{
			if ((i <N && j<N) || ((i>signal.size() - N - 1) && j>N))
			{
				window.push_back(0);
			}
			else
				window.push_back(signal[i - N + j]);
		}
		//   Order elements (only half of them)
		for (int j = 0; j <N + 1; ++j)
		{
			//   Find position of minimum element
			int min = j;
			for (int k = j + 1; k <2 * N + 1; ++k)
			if (window[k] < window[min])
				min = k;
			//   Put found minimum element in its place
			const float temp = window[j];
			window[j] = window[min];
			window[min] = temp;
		}
		//   Get result - the middle element
		result.push_back(window[N]);
	}
}
void MeasureDistance::gaussianfilter(vector<float>& signal, vector<float>& result, float sigma)
{
	int ws = ceil((sigma * 6 - 1) / 2) * 2 + 1;  //窗口大小
	int wc = ws / 2;				//窗口中心
	float* gsjz = new float[ws];
	float sum = 0;
	//生成高斯矩阵
	for (int j = 0; j<ws; j++)	//lie
	{
		int x = j - wc;
		gsjz[j] = exp((-x*x) / (2 * sigma*sigma)) / sqrt(2 * 3.1415*sigma*sigma); //一维高斯矩阵公式
		sum += gsjz[j];
	}
	for (int i = 0; i<ws; i++)
	{
		gsjz[i] /= sum;
	}

	signal.insert(signal.begin(), 0);
	signal.insert(signal.end(), 0);
	//   Move window through all elements of the signal
	for (int i = 0; i<signal.size(); i++)
	{
		float sum2 = 0;
		for (int a = -wc; a<wc; a++)
		{
			if (i - wc >= 0 && i + wc<signal.size())
			{
				sum2 += signal[a + i] * gsjz[wc + a];
			}
		}
		result.push_back(sum2);
	}
	result.erase(result.begin());
	result.erase(result.end() - 1);
}
/*腐蚀*/
void MeasureDistance::morphology_corrode(vector<float>&input, vector<float>&output, int scale,int k)
{
    
	for (int i = 0; i < scale; i++)
	{
		vector<float> fg;
		for (int j = 0; j <k; j++)
		{
			if (i + j < scale )
			fg.push_back(input[i + j]);
		}
		sort(fg.begin(), fg.end());
		output.push_back(fg[0]);
	}
	
	
}
/*膨胀*/
void MeasureDistance::morphology_dilate(vector<float>&input, vector<float>&output, int scale,int k)
{
	
	for (int i = 0; i < scale; i++)
	{
		vector<float> fg;
		for (int j = 0; j <k; j++)
		{
			if (i - j > 0 || i - j == 0)
				fg.push_back(input[i - j]);
		}
		sort(fg.begin(), fg.end());
		output.push_back(fg[fg.size()-1]);
	}
}
void MeasureDistance::morphology_oc(vector<float>&input, vector<float>&output,int k)
{
	int N = input.size();
	vector<float>pointsAX, pointsAX_O, pointsAX_C, pointsAX_OC, pointsAX_CO, pointsAX_Z, pointsAX_Z2;
	for (int i = 0; i < input.size(); i++)
	{
		pointsAX.push_back(input[i]);
	}
	morphology_corrode(pointsAX, pointsAX_O, N,k);
	morphology_dilate(pointsAX_O, pointsAX_OC, N,k);//开运算
	morphology_dilate(pointsAX, pointsAX_C, N,k);
	morphology_corrode(pointsAX_C, pointsAX_CO, N,k);//闭运算
	for (int i = 0; i < input.size(); i++)
	{
		//pointsA2X_Z.push_back((pointsA2X_OC[i] + pointsA2X_CO[i]) / 2);
		output.push_back((pointsAX_OC[i] + pointsAX_CO[i]) / 2);
	}
}
bool MeasureDistance::fittingCurve(vector<Point2d> &vec, int times, float *p) //输入点，次数，输出曲线参数    （最小二乘法拟合函数）
{
	float *py = new float[vec.size()];
	float *px = new float[times*vec.size()];
	int i = 0;
	Point2d* P = &vec[0];
	for (vector<Point2d>::iterator itr = vec.begin(); itr != vec.end(); ++itr)
	{
		py[i] = (*itr).y;
		int j = 0;
		while (j<times)
		{
			px[times*i + j] = pow(((*itr).x), float(j));
			j++;
		}
		i++;
	}
	Mat X = Mat(vec.size(), times, CV_32FC1, px);
	float* Xp = &(X.at<float>(0));
	Mat X_T;
	transpose(X, X_T);
	Mat Y = Mat(vec.size(), 1, CV_32FC1, py);
	Mat para = ((X_T*X).inv())*X_T*Y;
	for (int s = 0; s<times; s++)
	{
		p[s] = para.at<float>(s);
	}
	delete[] px;
	delete[] py;
	return true;
}