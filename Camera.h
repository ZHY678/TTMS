#ifndef __CAM_HEADER
#define __CAM_HEADER

#pragma once
#include "CameraDS.h"
//#include "CameraApi.h"
#include <map>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "Utils.h"

using namespace cv;
using namespace std;

class Camera
{

public:
	//int upCameraCode;
	//int downCameraCode;

	Mat frame;  //��ǰ֡
	static int cam_count; //����ͷ��Ŀ
	static map<int,string> camDevices;
	CCameraDS camDs;  //DirectShow Support
	static int width;  //���ͼ��ֱ���
	static int height;
	bool isOpen = false;  //�Ƿ�����

	Camera();
	~Camera();

	enum CAM_TYPE{ MIND = 1, UVC = 2 };  //��������� 1 MINDVISION ��2 UVC��������ͷ

	//������������ͳ�ʼ������ͷ
	static bool CameraInit(int type);
	//�򿪻���������ͷ
	bool CameraOpen(int type, int deviceid = 0, bool openProperty = false);
	//�ر�����ͷ
	void CameraClose();
	//void CameraPlay();
	//void CameraPause();
	//�õ�һ֡ͼ��
	Mat getOneFrame();
	bool CameraIsOpen();
	static void CameraTakePics(Camera& cameraSupport, int num, vector<Mat>& imgs);
	static void GetMeasPic(vector<Mat>& imgs, Mat& outMat);
	static void SplitPic(Mat& src, Mat& dstLeft, Mat& dstRight);
};

//ȫ��
extern Camera m_cameraSupport1;   //���1
extern Camera m_cameraSupport2;  //���2


#endif