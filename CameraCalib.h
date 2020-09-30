

#pragma once

#include <vector>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <fstream>

//#include "LMOptimalFor3D.h"

using namespace std;
using namespace cv;

class CameraCalib
{
public:
	CameraCalib();
	~CameraCalib();


	/***
	*	���̽ǵ����� �ṹ��
	*/
	struct CornerDatas
	{
		int			nImages;			// ����ͼ����
		int			nPointsPerImage;	// ÿ�����̵Ľǵ���
		cv::Size 	imageSize;			// ͼ��ֱ���
		cv::Size	    boardSize;			// ���̳ߴ�
		vector<vector<cv::Point3f> >	objectPoints;	// ���̽ǵ�������������
		vector<vector<cv::Point2f> >	leftImgPoints;	// ����ͼ�����̽ǵ�������������
		vector<vector<cv::Point2f> >	rightImgPoints;	// ����ͼ�����̽ǵ�������������
	};


	/***
	*	�궨�������
	*/
	struct CalibrationParames
	{
		cv::Mat  	mLeftCameraInParas          = cv::Mat::zeros(3, 3, CV_32FC1);       // ��������ڲ��궨������3*3��;
		cv::Mat  	mRightCameraInParas         = cv::Mat::zeros(3, 3, CV_32FC1);       // ��������ڲ��궨������3*3��;
		cv::Mat  	mLeftCameraDistorParas      = cv::Mat::zeros(1, 5, CV_32FC1);       // ������Ļ������(1*5);
		cv::Mat  	mRightCameraDistorParas     = cv::Mat::zeros(1, 5, CV_32FC1);       // ������Ļ������(1*5);

		double      dLeftCameraFocalLen         = 0;          // ������Ľ��࣬�ɱ궨��������õ�;
		double      dRightCameraFocalLen        = 0;          // ������Ľ��࣬�ɱ궨��������õ�;

		cv::Mat  	mL2RRotation                = cv::Mat::zeros(3, 1, CV_32FC1);	            // ��ת����(3*1);
		cv::Mat 	mL2RTranslation             = cv::Mat::zeros(3, 1, CV_32FC1);	            // ƽ������(3*1);
		cv::Mat  	mL2RRotation33              = cv::Mat::zeros(3, 3, CV_32FC1);		        // ��ת����(3*3);

		cv::Mat  	mSsential                   = cv::Mat::zeros(3, 3, CV_32FC1);		        // ���ʾ���;
		cv::Mat 	mFoundational               = cv::Mat::zeros(3, 3, CV_32FC1);	            // ��������;(3*3)    R = F*L    (2) = F*(1);
		double      dErr                        = 0;               //��ͶӰ�в�;
		BOOL	    bIsBinocularCameraCalibration = 1;	           // ˫Ŀ�궨���õı�־λ;
		double      dCaliratebError             = 0;               //�궨���;
		CString     strCalibrateTime            = _T("");          //�궨ʱ��;

		CString      sCaliResultFilePath        = _T("");          //��Ŀ����궨����洢·��;
		int       dBoardWidth;          //�궨�������ÿ�ж��ٽǵ�
		int       dBoardHeight;         //�궨�������ÿ�ж��ٽǵ�
		double       dSquareSize;		   //�궨�����ʵ�ʳߴ�
		double       dPixelSize;           // �������Ԫ�ߴ磬���Դ������˵�����ϲ�ѯ;

		cv::Mat      amendRotMat = cv::Mat::eye(3,3,CV_32FC1);          //�����������ת����
		cv::Mat     trianRotation;             //���ǲ������궨����ת����
		cv::Mat     trianTanslation;           //���ǲ������궨��ƽ������ 

		BOOL WriteYMLFile(int num=1)
		{
			if (!PathFileExists(sCaliResultFilePath))  //�����·�����������½���·��;
			{
				CreateDirectory(sCaliResultFilePath, NULL);
			}

			if (bIsBinocularCameraCalibration)
			{
				CString strBinoCali;
				strBinoCali.Format(_T("%s\\BinocularCaliResult_%d.yml"), sCaliResultFilePath, num);

				string sCaliResultPath((LPCSTR)CStringA(strBinoCali));
				cv::FileStorage fs(sCaliResultPath, cv::FileStorage::WRITE);

				if (!fs.isOpened())
				{
					AfxMessageBox(_T("�궨��������ļ�ʱ��������"));
					return FALSE;
				}

				time_t rawtime;
				time(&rawtime);
				fs << "calibrationDate" << asctime(localtime(&rawtime));
				fs << "dBoardHeight" << dBoardHeight;
				fs << "dBoardWidth" << dBoardWidth;
				fs << "dSquareSize" << dSquareSize;
				fs << "dPixelSize" << dPixelSize;
				fs << "amendRotMat" << amendRotMat;
				fs << "mLeftCameraInParas" << mLeftCameraInParas << "mLeftCameraDistorParas" << mLeftCameraDistorParas;
				fs << "mRightCameraInParas" << mRightCameraInParas << "mRightCameraDistorParas" << mRightCameraDistorParas;
				fs << "dLeftCameraFocalLen" << dLeftCameraFocalLen << "dRightCameraFocalLen" << dRightCameraFocalLen;
				fs << "mL2RRotation33" << mL2RRotation33 << "mL2RTranslation" << mL2RTranslation;
				fs << "mSsential" << mSsential << "mFoundational" << mFoundational;
				fs << "dCaliratebError" << dCaliratebError;

				//�����ʱ�䣨����opencv��FileStorage����û�й���CString�����أ���˽�CSringת����string��;
				string sCaliTime((LPCSTR)CStringA(strCalibrateTime));

				fs << "strCalibrateTime" << sCaliTime;

				fs.release();
			}
			if (!bIsBinocularCameraCalibration)
			{
				CString strBinoCali;
				strBinoCali.Format(_T("%s\\SingleCaliResult.yml"), sCaliResultFilePath);

				string sCaliResultPath((LPCSTR)CStringA(strBinoCali));
				cv::FileStorage fs(sCaliResultPath, cv::FileStorage::WRITE);

				if (!fs.isOpened())
				{
					AfxMessageBox(_T("�궨�����������"));
					return FALSE;
				}

				time_t rawtime;
				time(&rawtime);
				fs << "calibrationDate" << asctime(localtime(&rawtime));
				fs << "dBoardHeight" << dBoardHeight;
				fs << "dBoardWidth" << dBoardWidth;
				fs << "dSquareSize" << dSquareSize;
				fs << "dPixelSize" << dPixelSize;
				fs << "mCameraInParas" << mLeftCameraInParas;
				fs << "mCameraDistorParas" << mLeftCameraDistorParas;
				fs << "mTrianRotation" << trianRotation;
				fs << "mTrianTanslation" << trianTanslation;
				fs << "dCameraFocalLen" << dLeftCameraFocalLen;
				fs << "dCaliratebError" << dCaliratebError;
				//����궨ʱ�䣨����opencv��FileStorage����û�й���CString�����أ���˽�CSringת����string��;
				string sCaliTime((LPCSTR)CStringA(strCalibrateTime));
				fs << "strCalibrateTime" << sCaliTime;

				fs.release();
			}
			return TRUE;
		}

		BOOL ReadYMLFile(int num=1)
		{
			if (!PathFileExists(sCaliResultFilePath))  //�����·���������򱨴�;
			{
				//AfxMessageBox(_T("�궨�ļ������ڣ�"));
				return FALSE;
			}

			if (bIsBinocularCameraCalibration)
			{
				CString strBinoCali;
				strBinoCali.Format(_T("%s\\BinocularCaliResult_%d.yml"), sCaliResultFilePath, num);

				string sCaliResultPath((LPCSTR)CStringA(strBinoCali));
				cv::FileStorage fs(sCaliResultPath, cv::FileStorage::READ);
				if (!fs.isOpened())
				{
					AfxMessageBox(_T("�궨������ȡʧ�ܣ�"));
					return FALSE;
				}

				fs["dBoardWidth"] >> dBoardWidth;
				fs["dBoardHeight"] >> dBoardHeight;
				fs["dSquareSize"] >> dSquareSize;
				fs["dPixelSize"] >> dPixelSize;
				fs["amendRotMat"] >> amendRotMat;
				fs["mLeftCameraInParas"] >> mLeftCameraInParas;
				fs["mLeftCameraDistorParas"] >> mLeftCameraDistorParas;
				fs["mRightCameraInParas"] >> mRightCameraInParas;
				fs["mRightCameraDistorParas"] >> mRightCameraDistorParas;
				fs["dLeftCameraFocalLen"] >> dLeftCameraFocalLen;
				fs["dRightCameraFocalLen"] >> dRightCameraFocalLen;

				fs["mL2RRotation33"] >> mL2RRotation33;
				fs["mL2RTranslation"] >> mL2RTranslation;
				fs["mSsential"] >> mSsential;
				fs["mFoundational"] >> mFoundational;
				fs["dCaliratebError"] >> dCaliratebError;

				//��ȡ�궨ʱ�䣨����opencv��FileStorage����û�й���CString�����أ���˽�CSringת����string��;
				string sCaliTime;
				fs["strCalibrateTime"] >> sCaliTime;
				strCalibrateTime = sCaliTime.c_str();
			}
			if (!bIsBinocularCameraCalibration)
			{
				CString strBinoCali;
				strBinoCali.Format(_T("%s\\SingleCaliResult.yml"), sCaliResultFilePath);

				string sCaliResultPath((LPCSTR)CStringA(strBinoCali));
				cv::FileStorage fs(sCaliResultPath, cv::FileStorage::READ);
				if (!fs.isOpened())
				{
					AfxMessageBox(_T("�궨������ȡʧ�ܣ�"));
					return FALSE;
				}

				fs["dBoardWidth"] >> dBoardWidth;
				fs["dBoardHeight"] >> dBoardHeight;
				fs["dSquareSize"] >> dSquareSize;
				fs["dPixelSize"] >> dPixelSize;
				fs["mCameraInParas"] >> mLeftCameraInParas;
				fs["mCameraDistorParas"] >> mLeftCameraDistorParas;
				fs["mTrianRotation"] >> trianRotation;
				fs["mTrianTanslation"] >> trianTanslation;
				fs["dCaliratebError"] >> dCaliratebError;
				fs["dCameraFocalLen"] >>dLeftCameraFocalLen;

			}


			return TRUE;
		}
	};

	

	/*----------------------------
	* ���� : ��ʼ�����̽ǵ�������Ϣ
	*----------------------------
	* ���� : 0 - ����ʧ�ܣ�1 - �����ɹ�
	*
	* ���� : nImages		[in]	����ͼ����
	* ���� : imageSize		[in]	ͼ��ֱ���
	* ���� : boardSize		[in]	���̽ǵ���
	* ���� : squareWidth	[in]	���̷�����
	* ���� : cornerDatas	[out]	��ʼ��������̽ǵ�����
	*/
	int initCornerData(int nImages, cv::Size imageSize, cv::Size boardSize, float squareWidth, CornerDatas& cornerDatas);

	/*----------------------------
	* ���� : ������̽ǵ�
	*----------------------------
	* ���� : 0 - ����ʧ�ܣ�1 - �����ɹ�
	*
	* ���� : img1			[in]	����ͼ
	* ���� : img2			[in]	����ͼ
	* ���� : cornerDatas	[i/o]	�����������̽ǵ�����
	* ���� : imageCount		[in]	��ǰ���̽ǵ���ĳɹ�����
	*/
	int detectCorners(cv::Mat& img1, cv::Mat& img2, CornerDatas& cornerDatas, int imageCount);

	/*----------------------------
	* ���� : �����ѱ궨�õ�������ڲ�����
	*----------------------------
	* ���� : StereoCalib::saveCameraParams
	* ���� : public
	* ���� : 0 - ����ʧ�ܣ�1 - �����ɹ�
	*
	* ���� : cameraParams	[in]	�ѱ궨�����������
	* ���� : filename		[in]	�����ļ�·��/�ļ���
	*/
//	int saveCameraParams(const CameraParams& cameraParams, const char* filename = "cameraParams.yml");

	/*----------------------------
	* ���� : ִ�е�Ŀ������궨
	*----------------------------
	* ���� : StereoCalib::calibrateSingleCamera
	* ���� : public
	* ���� : 0 - ����ʧ�ܣ�1 - �����ɹ�
	*
	* ���� : cornerDatas			[in]	���̽ǵ�����
	* ���� : cameraParams			[out]	˫Ŀ�궨����
	*/
//	int calibrateSingleCamera(CornerDatas& cornerDatas, CameraParams& cameraParams);

	/*----------------------------
	* ���� : ִ��˫Ŀ������궨
	*		 ��ÿ���������δ�궨�������Ƚ��е�Ŀ�궨���ٽ���˫Ŀ�궨
	*----------------------------
	* ���� : StereoCalib::calibrateStereoCamera
	* ���� : public
	* ���� : 0 - ����ʧ�ܣ�1 - �����ɹ�
	*
	* ���� : cornerDatas			[in]	���̽ǵ�����
	* ���� : stereoParams			[i/o]	˫Ŀ�궨����
	* ���� : cameraUncalibrated		[in]	ÿ��������Ƿ��Ѿ������궨
	*/
	int calibrateStereoCamera(CornerDatas& cornerDatas, CalibrationParames& m_CalibrationResult);


protected:
private:
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
	//void showText(cv::Mat& img, string text);

};