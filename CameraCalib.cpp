/********************************************************************
���� :	2017/4/20
�ļ� :	CameraCalib.cpp
���� :	CameraCalib

���� :	������궨ʵ�ִ���
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
* ���� : ��ͼ�����½���ʾָ��������Ϣ
*----------------------------
* ���� : StereoCalib::showText
* ���� : private
* ���� : void
*
* ���� : img	[in]	ͼ��
* ���� : text	[in]	������Ϣ
*/
//void CameraCalib::showText(cv::Mat& img, string text)
//{
//
//}



/*----------------------------
* ���� : ������̽ǵ�
*----------------------------
* ���� : StereoCalib::detectCorners
* ���� : public
* ���� : 0 - ����ʧ�ܣ�1 - �����ɹ�
*
* ���� : img1			[in]	����ͼ
* ���� : img2			[in]	����ͼ
* ���� : cornerDatas	[i/o]	�����������̽ǵ�����
* ���� : imageCount		[in]	��ǰ���̽ǵ���ĳɹ�����
*/
int CameraCalib::detectCorners(cv::Mat& img1, cv::Mat& img2, CornerDatas& cornerDatas, int imageCount)
{

	// ��ȡ��ǰ���̶�Ӧ�Ľǵ������Ӿ���
	vector<cv::Point2f>& corners1 = cornerDatas.leftImgPoints[imageCount];
	vector<cv::Point2f>& corners2 = cornerDatas.rightImgPoints[imageCount];

	// Ѱ�����̼���ǵ�
	bool found1, found2;
	int flags = CV_CALIB_CB_ADAPTIVE_THRESH + CV_CALIB_CB_NORMALIZE_IMAGE + CV_CALIB_CB_FAST_CHECK;

	found1 = findChessboardCorners(img1, cornerDatas.boardSize, corners1);
	found2 = findChessboardCorners(img2, cornerDatas.boardSize, corners2);

	// ��������ͼ���ɹ���⵽���нǵ�
	// �򽫼�⵽�Ľǵ����꾫ȷ��
	if (found1 && found2)
	{
		//ת��Ϊ�Ҷ�ͼ
		cv::Mat gray1, gray2;
		cvtColor(img1, gray1, CV_RGB2GRAY);
		cvtColor(img2, gray2, CV_RGB2GRAY);

		//����ǵ�ľ�ȷ����
		cv::Size regionSize(11, 11);
		cornerSubPix(gray1, corners1, regionSize, cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 30, 0.05));
		cornerSubPix(gray2, corners2, regionSize, cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 30, 0.05));
	}

	// ��ʾ��⵽�Ľǵ�
	drawChessboardCorners(img1, cornerDatas.boardSize, corners1, found1);
	drawChessboardCorners(img2, cornerDatas.boardSize, corners2, found2);

	return (found1 && found2) ? 1 : 0;
}

/*----------------------------
* ���� : ��ʼ�����̽ǵ�������Ϣ
*----------------------------
* ���� : 0 - ����ʧ�ܣ�1 - �����ɹ�
*
* ���� : nImages[in]	����ͼ����
* ���� : imageSize[in]	ͼ��ֱ���
* ���� : boardSize[in]	���̽ǵ���
* ���� : squareWidth[in]	 ���̷������������ǵ��ʵ��������
* ���� : cornerDatas[out]	��ʼ��������̽ǵ�����
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

	//�������̽ǵ����������ֵ
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
//* ���� : �����ѱ궨�õ�������ڲ�����
//*----------------------------
//* ���� : StereoCalib::saveCameraParams
//* ���� : public
//* ���� : 0 - ����ʧ�ܣ�1 - �����ɹ�
//*
//* ���� : cameraParams	[in]	�ѱ궨�����������
//* ���� : filename		[in]	�����ļ�·��/�ļ���
//*/
//int CameraCalib::saveCameraParams(const CameraParams& cameraParams, const char* filename /* = "cameraParams.yml" */)
//{
//	std::string filename_ = filename;
//
//	//����ǰʱ�������ļ���
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
//	//д������
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
* ���� : ִ��˫Ŀ������궨
*		 ���Ƚ��е�Ŀ�궨���ٽ���˫Ŀ�궨
*----------------------------
* ���� : 0 - ����ʧ�ܣ�1 - �����ɹ�
*
* ���� : cornerDatas			        [in]	���̽ǵ�����
* ���� : CalibrationParames			[in]	˫Ŀ�궨����
*/
int CameraCalib::calibrateStereoCamera(CornerDatas& cornerDatas, CalibrationParames& calibrationResult)
{
	//��ʼ�궨����
	Mat cameraMatrix[2], distCoeffs[2];
	vector<Mat> rvecs[2], tvecs[2]; //��ת�����ƽ�ƾ���
	cameraMatrix[0] = Mat(3, 3, CV_32FC1, Scalar::all(0));
	cameraMatrix[1] = Mat(3, 3, CV_32FC1, Scalar::all(0));
	
	//�ֱ�����������������е�Ŀ�궨
	calibrateCamera(cornerDatas.objectPoints, cornerDatas.leftImgPoints, cornerDatas.imageSize, cameraMatrix[0], distCoeffs[0], rvecs[0], tvecs[0], CV_CALIB_FIX_K3);
	calibrateCamera(cornerDatas.objectPoints, cornerDatas.rightImgPoints, cornerDatas.imageSize, cameraMatrix[1], distCoeffs[1], rvecs[1], tvecs[1], CV_CALIB_FIX_K3);

	Mat R, T, E, F;
	//����궨
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
	//----------------------------------------------�Ż��궨���------------------------------------------
	//���ô��Ż��Ĳ���;
	int nImgPointCount = cornerDatas.leftImgPoints.size();
	int nObjectPointCount = cornerDatas.objectPoints[0].size();
	UINT nParaNum = 8 + 8 + 6 + 6 * nImgPointCount + 3 * nObjectPointCount;  //���Ż���������;
	double* pPara = new double[nParaNum];
	//��������ڲ�����;
	int nIndex = 0;
	*(pPara + nIndex++) = calibrationResult.mLeftCameraInParas.at<double>(0, 0);
	*(pPara + nIndex++) = calibrationResult.mLeftCameraInParas.at<double>(1, 1);
	*(pPara + nIndex++) = calibrationResult.mLeftCameraInParas.at<double>(0, 2);
	*(pPara + nIndex++) = calibrationResult.mLeftCameraInParas.at<double>(1, 2);
	////����;
	*(pPara + nIndex++) = calibrationResult.mLeftCameraDistorParas.at<double>(0, 0);
	*(pPara + nIndex++) = calibrationResult.mLeftCameraDistorParas.at<double>(0, 1);
	*(pPara + nIndex++) = calibrationResult.mLeftCameraDistorParas.at<double>(0, 2);
	*(pPara + nIndex++) = calibrationResult.mLeftCameraDistorParas.at<double>(0, 3);

	//������ڲ�;
	*(pPara + nIndex++) = calibrationResult.mRightCameraInParas.at<double>(0, 0);
	*(pPara + nIndex++) = calibrationResult.mRightCameraInParas.at<double>(1, 1);
	*(pPara + nIndex++) = calibrationResult.mRightCameraInParas.at<double>(0, 2);
	*(pPara + nIndex++) = calibrationResult.mRightCameraInParas.at<double>(1, 2);
	////����;
	*(pPara + nIndex++) = calibrationResult.mRightCameraDistorParas.at<double>(0, 0);
	*(pPara + nIndex++) = calibrationResult.mRightCameraDistorParas.at<double>(0, 1);
	*(pPara + nIndex++) = calibrationResult.mRightCameraDistorParas.at<double>(0, 2);
	*(pPara + nIndex++) = calibrationResult.mRightCameraDistorParas.at<double>(0, 3);


	////���ҵ���תƽ�ƾ���;
	*(pPara + nIndex++) = calibrationResult.mL2RRotation.at<double>(0, 0);
	*(pPara + nIndex++) = calibrationResult.mL2RRotation.at<double>(1, 0);
	*(pPara + nIndex++) = calibrationResult.mL2RRotation.at<double>(2, 0);

	*(pPara + nIndex++) = calibrationResult.mL2RTranslation.at<double>(0, 0);
	*(pPara + nIndex++) = calibrationResult.mL2RTranslation.at<double>(1, 0);
	*(pPara + nIndex++) = calibrationResult.mL2RTranslation.at<double>(2, 0);
	//�궨�����������;
	for (UINT i = 0; i < cornerDatas.objectPoints[0].size(); i++)
	{
		Point3f point = cornerDatas.objectPoints[0][i];
		*(pPara + nIndex++) = point.x;
		*(pPara + nIndex++) = point.y;
		*(pPara + nIndex++) = point.z;
	}

	//�������RT;
	for (UINT i = 0; i < nImgPointCount; i++)
	{
		*(pPara + nIndex++) = rvecs[0][i].at<double>(0, 0);
		*(pPara + nIndex++) = rvecs[0][i].at<double>(1, 0);
		*(pPara + nIndex++) = rvecs[0][i].at<double>(2, 0);

		*(pPara + nIndex++) = tvecs[0][i].at<double>(0, 0);
		*(pPara + nIndex++) = tvecs[0][i].at<double>(1, 0);
		*(pPara + nIndex++) = tvecs[0][i].at<double>(2, 0);
	}

	//����LM�Ż�;
	int pointSum = cornerDatas.nImages * cornerDatas.nPointsPerImage;
	LMOptimalFor3D OptimalForCalibrate;
	double *pErrEver = new double[1];
	OptimalForCalibrate.TNFP_LM(pPara, 2 * pointSum, nParaNum, pErrEver, cornerDatas.leftImgPoints, cornerDatas.rightImgPoints, cornerDatas.objectPoints[0]);

	nIndex = 0;
	//������ڲ�����;
	calibrationResult.mLeftCameraInParas.at<double>(0, 0) = *(pPara + nIndex++);
	calibrationResult.mLeftCameraInParas.at<double>(1, 1) = *(pPara + nIndex++);
	calibrationResult.mLeftCameraInParas.at<double>(0, 2) = *(pPara + nIndex++);
	calibrationResult.mLeftCameraInParas.at<double>(1, 2) = *(pPara + nIndex++);
	////����;
	calibrationResult.mLeftCameraDistorParas.at<double>(0, 0) = *(pPara + nIndex++);
	calibrationResult.mLeftCameraDistorParas.at<double>(0, 1) = *(pPara + nIndex++);
	calibrationResult.mLeftCameraDistorParas.at<double>(0, 2) = *(pPara + nIndex++);
	calibrationResult.mLeftCameraDistorParas.at<double>(0, 3) = *(pPara + nIndex++);

	//������ڲ�;
	calibrationResult.mRightCameraInParas.at<double>(0, 0) = *(pPara + nIndex++);
	calibrationResult.mRightCameraInParas.at<double>(1, 1) = *(pPara + nIndex++);
	calibrationResult.mRightCameraInParas.at<double>(0, 2) = *(pPara + nIndex++);
	calibrationResult.mRightCameraInParas.at<double>(1, 2) = *(pPara + nIndex++);
	////����;
	calibrationResult.mRightCameraDistorParas.at<double>(0, 0) = *(pPara + nIndex++);
	calibrationResult.mRightCameraDistorParas.at<double>(0, 1) = *(pPara + nIndex++);
	calibrationResult.mRightCameraDistorParas.at<double>(0, 2) = *(pPara + nIndex++);
	calibrationResult.mRightCameraDistorParas.at<double>(0, 3) = *(pPara + nIndex++);


	////���ҵ���תƽ�ƾ���;
	calibrationResult.mL2RRotation.at<double>(0, 0) = *(pPara + nIndex++);
	calibrationResult.mL2RRotation.at<double>(1, 0) = *(pPara + nIndex++);
	calibrationResult.mL2RRotation.at<double>(2, 0) = *(pPara + nIndex++);

	calibrationResult.mL2RTranslation.at<double>(0, 0) = *(pPara + nIndex++);
	calibrationResult.mL2RTranslation.at<double>(1, 0) = *(pPara + nIndex++);
	calibrationResult.mL2RTranslation.at<double>(2, 0) = *(pPara + nIndex++);


	////������Ľ���; 
	calibrationResult.dLeftCameraFocalLen = (cameraMatrix[0].at<double>(0, 0) + cameraMatrix[0].at<double>(1, 1)) / 2 * calibrationResult.dPixelSize;
	////���������;
	calibrationResult.dRightCameraFocalLen = (cameraMatrix[1].at<double>(0, 0) + cameraMatrix[1].at<double>(1, 1)) / 2 * calibrationResult.dPixelSize;
	calibrationResult.dCaliratebError = *pErrEver;
	*/

	return 1;
}
