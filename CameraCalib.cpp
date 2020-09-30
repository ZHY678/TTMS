/********************************************************************
创建 :	2017/4/20
文件 :	CameraCalib.cpp
类名 :	CameraCalib

功能 :	摄像机标定实现代码
*********************************************************************/

#include "stdafx.h"
#include "CameraCalib.h"

CameraCalib::CameraCalib()
{
}

CameraCalib::~CameraCalib()
{
}

/*----------------------------
* 功能 : 在图像右下角显示指定文字信息
*----------------------------
* 函数 : StereoCalib::showText
* 访问 : private
* 返回 : void
*
* 参数 : img	[in]	图像
* 参数 : text	[in]	文字信息
*/
//void CameraCalib::showText(cv::Mat& img, string text)
//{
//
//}



/*----------------------------
* 功能 : 检测棋盘角点
*----------------------------
* 函数 : StereoCalib::detectCorners
* 访问 : public
* 返回 : 0 - 操作失败，1 - 操作成功
*
* 参数 : img1			[in]	左视图
* 参数 : img2			[in]	右视图
* 参数 : cornerDatas	[i/o]	待导出的棋盘角点数据
* 参数 : imageCount		[in]	当前棋盘角点检测的成功次数
*/
int CameraCalib::detectCorners(cv::Mat& img1, cv::Mat& img2, CornerDatas& cornerDatas, int imageCount)
{

	// 提取当前棋盘对应的角点坐标子矩阵
	vector<cv::Point2f>& corners1 = cornerDatas.leftImgPoints[imageCount];
	vector<cv::Point2f>& corners2 = cornerDatas.rightImgPoints[imageCount];

	// 寻找棋盘及其角点
	bool found1, found2;
	int flags = CV_CALIB_CB_ADAPTIVE_THRESH + CV_CALIB_CB_NORMALIZE_IMAGE + CV_CALIB_CB_FAST_CHECK;

	found1 = findChessboardCorners(img1, cornerDatas.boardSize, corners1);
	found2 = findChessboardCorners(img2, cornerDatas.boardSize, corners2);

	// 若左右视图都成功检测到所有角点
	// 则将检测到的角点坐标精确化
	if (found1 && found2)
	{
		//转换为灰度图
		cv::Mat gray1, gray2;
		cvtColor(img1, gray1, CV_RGB2GRAY);
		cvtColor(img2, gray2, CV_RGB2GRAY);

		//计算角点的精确坐标
		cv::Size regionSize(11, 11);
		cornerSubPix(gray1, corners1, regionSize, cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 30, 0.05));
		cornerSubPix(gray2, corners2, regionSize, cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 30, 0.05));
	}

	// 显示检测到的角点
	drawChessboardCorners(img1, cornerDatas.boardSize, corners1, found1);
	drawChessboardCorners(img2, cornerDatas.boardSize, corners2, found2);

	return (found1 && found2) ? 1 : 0;
}

/*----------------------------
* 功能 : 初始化棋盘角点数据信息
*----------------------------
* 返回 : 0 - 操作失败，1 - 操作成功
*
* 参数 : nImages[in]	棋盘图像数
* 参数 : imageSize[in]	图像分辨率
* 参数 : boardSize[in]	棋盘角点数
* 参数 : squareWidth[in]	 棋盘方块相邻两个角点的实际物理长度
* 参数 : cornerDatas[out]	初始化后的棋盘角点数据
*/
int CameraCalib::initCornerData(int nImages, cv::Size imageSize, cv::Size boardSize, float squareWidth, CornerDatas& cornerDatas)
{
	cornerDatas.nImages = nImages;
	cornerDatas.imageSize = imageSize;
	cornerDatas.boardSize = boardSize;
	cornerDatas.nPointsPerImage = boardSize.width * boardSize.height;
	cornerDatas.objectPoints.resize(nImages, vector<cv::Point3f>(cornerDatas.nPointsPerImage, cv::Point3f(0, 0, 0)));
	cornerDatas.leftImgPoints.resize(nImages, vector<cv::Point2f>(cornerDatas.nPointsPerImage, cv::Point2f(0, 0)));
	cornerDatas.rightImgPoints.resize(nImages, vector<cv::Point2f>(cornerDatas.nPointsPerImage, cv::Point2f(0, 0)));

	//计算棋盘角点的世界坐标值
	int i, j, k, n;
	for (i = 0; i < nImages; i++)
	{
		n = 0;
		for (j = 0; j < boardSize.height; j++)
			for (k = 0; k < boardSize.width; k++)
				cornerDatas.objectPoints[i][n++] = cv::Point3f(k*squareWidth, j*squareWidth, 0);
	}

	return 1;
}


///*----------------------------
//* 功能 : 保存已标定好的摄像机内部参数
//*----------------------------
//* 函数 : StereoCalib::saveCameraParams
//* 访问 : public
//* 返回 : 0 - 操作失败，1 - 操作成功
//*
//* 参数 : cameraParams	[in]	已标定的摄像机参数
//* 参数 : filename		[in]	参数文件路径/文件名
//*/
//int CameraCalib::saveCameraParams(const CameraParams& cameraParams, const char* filename /* = "cameraParams.yml" */)
//{
//	std::string filename_ = filename;
//
//	//按当前时间生成文件名
//	if (filename_ == "" || filename_ == "cameraParams.yml")
//	{
//		int strLen = 20;
//		char *pCurrTime = (char*)malloc(sizeof(char)*strLen);
//		memset(pCurrTime, 0, sizeof(char)*strLen);
//		time_t now;
//		time(&now);
//		strftime(pCurrTime, strLen, "%Y_%m_%d_%H_%M_%S_", localtime(&now));
//
//		filename_ = pCurrTime;
//		filename_ += "cameraParams.yml";
//	}
//
//	//写入数据
//	cv::FileStorage fs(filename_.c_str(), cv::FileStorage::WRITE);
//	if (fs.isOpened())
//	{
//		time_t rawtime;
//		time(&rawtime);
//		fs << "calibrationDate" << asctime(localtime(&rawtime));
//
//		char flagText[1024];
//		sprintf_s(flagText, "flags: %s%s%s%s%s",
//			cameraParams.flags & CV_CALIB_FIX_K3 ? "fix_k3" : "",
//			cameraParams.flags & CV_CALIB_USE_INTRINSIC_GUESS ? " + use_intrinsic_guess" : "",
//			cameraParams.flags & CV_CALIB_FIX_ASPECT_RATIO ? " + fix_aspect_ratio" : "",
//			cameraParams.flags & CV_CALIB_FIX_PRINCIPAL_POINT ? " + fix_principal_point" : "",
//			cameraParams.flags & CV_CALIB_ZERO_TANGENT_DIST ? " + zero_tangent_dist" : "");
//		cvWriteComment(*fs, flagText, 0);
//
//		fs << "flags" << cameraParams.flags;
//
//		fs << "imageSize" << "[" << cameraParams.imageSize.width << cameraParams.imageSize.height << "]";
//
//		fs << "cameraMatrix" << cameraParams.cameraMatrix;
//		fs << "distortionCoefficients" << cameraParams.distortionCoefficients;
//
//		int nImages = cameraParams.rotations.size();
//		fs << "nImages" << nImages;
//		for (UINT i = 0; i < nImages; i++)
//		{
//			char matName[50];
//			sprintf_s(matName, "rotaionMatrix_%d", i);
//
//			fs << matName << cameraParams.rotations[i];
//		}
//		for (UINT i = 0; i < nImages; i++)
//		{
//			char matName[50];
//			sprintf_s(matName, "translationMatrix_%d", i);
//
//			fs << matName << cameraParams.translations[i];
//		}
//
//		fs.release();
//		return 1;
//	}
//	else
//	{
//		return 0;
//	}
//}
//
//

/*----------------------------
* 功能 : 执行双目摄像机标定
*		 首先进行单目标定，再进行双目标定
*----------------------------
* 返回 : 0 - 操作失败，1 - 操作成功
*
* 参数 : cornerDatas			        [in]	棋盘角点数据
* 参数 : CalibrationParames			[in]	双目标定数据
*/
int CameraCalib::calibrateStereoCamera(CornerDatas& cornerDatas, CalibrationParames& calibrationResult)
{
	//开始标定计算
	Mat cameraMatrix[2], distCoeffs[2];
	vector<Mat> rvecs[2], tvecs[2]; //旋转矩阵和平移矩阵
	cameraMatrix[0] = Mat(3, 3, CV_32FC1, Scalar::all(0));
	cameraMatrix[1] = Mat(3, 3, CV_32FC1, Scalar::all(0));
	
	//分别对左相机和右相机进行单目标定
	calibrateCamera(cornerDatas.objectPoints, cornerDatas.leftImgPoints, cornerDatas.imageSize, cameraMatrix[0], distCoeffs[0], rvecs[0], tvecs[0], CV_CALIB_FIX_K3);
	calibrateCamera(cornerDatas.objectPoints, cornerDatas.rightImgPoints, cornerDatas.imageSize, cameraMatrix[1], distCoeffs[1], rvecs[1], tvecs[1], CV_CALIB_FIX_K3);

	Mat R, T, E, F;
	//立体标定
	double rms = stereoCalibrate(cornerDatas.objectPoints, cornerDatas.leftImgPoints, cornerDatas.rightImgPoints, cameraMatrix[0], distCoeffs[0], cameraMatrix[1], distCoeffs[1], cornerDatas.imageSize,
		R, T, E, F, TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 100, 1e-5), CV_CALIB_USE_INTRINSIC_GUESS | CV_CALIB_FIX_K3);

	calibrationResult.mLeftCameraInParas = cameraMatrix[0];
	calibrationResult.mLeftCameraDistorParas = distCoeffs[0];
	calibrationResult.dLeftCameraFocalLen = (cameraMatrix[0].at<double>(0, 0) + cameraMatrix[0].at<double>(1, 1)) / 2 * calibrationResult.dPixelSize;

	calibrationResult.mRightCameraInParas = cameraMatrix[1];
	calibrationResult.mRightCameraDistorParas = distCoeffs[1];
	calibrationResult.dRightCameraFocalLen = (cameraMatrix[1].at<double>(0, 0) + cameraMatrix[1].at<double>(1, 1)) / 2 * calibrationResult.dPixelSize;

	calibrationResult.mL2RRotation33 = R;
	cv::Rodrigues(R, calibrationResult.mL2RRotation);
	calibrationResult.mL2RTranslation = T;
	calibrationResult.mSsential = E;
	calibrationResult.mFoundational = F;
	calibrationResult.dCaliratebError = rms;

	/*
	//----------------------------------------------优化标定结果------------------------------------------
	//设置待优化的参数;
	int nImgPointCount = cornerDatas.leftImgPoints.size();
	int nObjectPointCount = cornerDatas.objectPoints[0].size();
	UINT nParaNum = 8 + 8 + 6 + 6 * nImgPointCount + 3 * nObjectPointCount;  //待优化参数个数;
	double* pPara = new double[nParaNum];
	//左相机的内部参数;
	int nIndex = 0;
	*(pPara + nIndex++) = calibrationResult.mLeftCameraInParas.at<double>(0, 0);
	*(pPara + nIndex++) = calibrationResult.mLeftCameraInParas.at<double>(1, 1);
	*(pPara + nIndex++) = calibrationResult.mLeftCameraInParas.at<double>(0, 2);
	*(pPara + nIndex++) = calibrationResult.mLeftCameraInParas.at<double>(1, 2);
	////畸变;
	*(pPara + nIndex++) = calibrationResult.mLeftCameraDistorParas.at<double>(0, 0);
	*(pPara + nIndex++) = calibrationResult.mLeftCameraDistorParas.at<double>(0, 1);
	*(pPara + nIndex++) = calibrationResult.mLeftCameraDistorParas.at<double>(0, 2);
	*(pPara + nIndex++) = calibrationResult.mLeftCameraDistorParas.at<double>(0, 3);

	//右相机内参;
	*(pPara + nIndex++) = calibrationResult.mRightCameraInParas.at<double>(0, 0);
	*(pPara + nIndex++) = calibrationResult.mRightCameraInParas.at<double>(1, 1);
	*(pPara + nIndex++) = calibrationResult.mRightCameraInParas.at<double>(0, 2);
	*(pPara + nIndex++) = calibrationResult.mRightCameraInParas.at<double>(1, 2);
	////畸变;
	*(pPara + nIndex++) = calibrationResult.mRightCameraDistorParas.at<double>(0, 0);
	*(pPara + nIndex++) = calibrationResult.mRightCameraDistorParas.at<double>(0, 1);
	*(pPara + nIndex++) = calibrationResult.mRightCameraDistorParas.at<double>(0, 2);
	*(pPara + nIndex++) = calibrationResult.mRightCameraDistorParas.at<double>(0, 3);


	////左到右的旋转平移矩阵;
	*(pPara + nIndex++) = calibrationResult.mL2RRotation.at<double>(0, 0);
	*(pPara + nIndex++) = calibrationResult.mL2RRotation.at<double>(1, 0);
	*(pPara + nIndex++) = calibrationResult.mL2RRotation.at<double>(2, 0);

	*(pPara + nIndex++) = calibrationResult.mL2RTranslation.at<double>(0, 0);
	*(pPara + nIndex++) = calibrationResult.mL2RTranslation.at<double>(1, 0);
	*(pPara + nIndex++) = calibrationResult.mL2RTranslation.at<double>(2, 0);
	//标定板世界坐标点;
	for (UINT i = 0; i < cornerDatas.objectPoints[0].size(); i++)
	{
		Point3f point = cornerDatas.objectPoints[0][i];
		*(pPara + nIndex++) = point.x;
		*(pPara + nIndex++) = point.y;
		*(pPara + nIndex++) = point.z;
	}

	//左相机的RT;
	for (UINT i = 0; i < nImgPointCount; i++)
	{
		*(pPara + nIndex++) = rvecs[0][i].at<double>(0, 0);
		*(pPara + nIndex++) = rvecs[0][i].at<double>(1, 0);
		*(pPara + nIndex++) = rvecs[0][i].at<double>(2, 0);

		*(pPara + nIndex++) = tvecs[0][i].at<double>(0, 0);
		*(pPara + nIndex++) = tvecs[0][i].at<double>(1, 0);
		*(pPara + nIndex++) = tvecs[0][i].at<double>(2, 0);
	}

	//进行LM优化;
	int pointSum = cornerDatas.nImages * cornerDatas.nPointsPerImage;
	LMOptimalFor3D OptimalForCalibrate;
	double *pErrEver = new double[1];
	OptimalForCalibrate.TNFP_LM(pPara, 2 * pointSum, nParaNum, pErrEver, cornerDatas.leftImgPoints, cornerDatas.rightImgPoints, cornerDatas.objectPoints[0]);

	nIndex = 0;
	//左相机内部参数;
	calibrationResult.mLeftCameraInParas.at<double>(0, 0) = *(pPara + nIndex++);
	calibrationResult.mLeftCameraInParas.at<double>(1, 1) = *(pPara + nIndex++);
	calibrationResult.mLeftCameraInParas.at<double>(0, 2) = *(pPara + nIndex++);
	calibrationResult.mLeftCameraInParas.at<double>(1, 2) = *(pPara + nIndex++);
	////畸变;
	calibrationResult.mLeftCameraDistorParas.at<double>(0, 0) = *(pPara + nIndex++);
	calibrationResult.mLeftCameraDistorParas.at<double>(0, 1) = *(pPara + nIndex++);
	calibrationResult.mLeftCameraDistorParas.at<double>(0, 2) = *(pPara + nIndex++);
	calibrationResult.mLeftCameraDistorParas.at<double>(0, 3) = *(pPara + nIndex++);

	//右相机内参;
	calibrationResult.mRightCameraInParas.at<double>(0, 0) = *(pPara + nIndex++);
	calibrationResult.mRightCameraInParas.at<double>(1, 1) = *(pPara + nIndex++);
	calibrationResult.mRightCameraInParas.at<double>(0, 2) = *(pPara + nIndex++);
	calibrationResult.mRightCameraInParas.at<double>(1, 2) = *(pPara + nIndex++);
	////畸变;
	calibrationResult.mRightCameraDistorParas.at<double>(0, 0) = *(pPara + nIndex++);
	calibrationResult.mRightCameraDistorParas.at<double>(0, 1) = *(pPara + nIndex++);
	calibrationResult.mRightCameraDistorParas.at<double>(0, 2) = *(pPara + nIndex++);
	calibrationResult.mRightCameraDistorParas.at<double>(0, 3) = *(pPara + nIndex++);


	////左到右的旋转平移矩阵;
	calibrationResult.mL2RRotation.at<double>(0, 0) = *(pPara + nIndex++);
	calibrationResult.mL2RRotation.at<double>(1, 0) = *(pPara + nIndex++);
	calibrationResult.mL2RRotation.at<double>(2, 0) = *(pPara + nIndex++);

	calibrationResult.mL2RTranslation.at<double>(0, 0) = *(pPara + nIndex++);
	calibrationResult.mL2RTranslation.at<double>(1, 0) = *(pPara + nIndex++);
	calibrationResult.mL2RTranslation.at<double>(2, 0) = *(pPara + nIndex++);


	////左相机的焦距; 
	calibrationResult.dLeftCameraFocalLen = (cameraMatrix[0].at<double>(0, 0) + cameraMatrix[0].at<double>(1, 1)) / 2 * calibrationResult.dPixelSize;
	////右相机焦距;
	calibrationResult.dRightCameraFocalLen = (cameraMatrix[1].at<double>(0, 0) + cameraMatrix[1].at<double>(1, 1)) / 2 * calibrationResult.dPixelSize;
	calibrationResult.dCaliratebError = *pErrEver;
	*/

	return 1;
}
