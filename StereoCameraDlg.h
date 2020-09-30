
// StereoCameraDlg.h : 头文件
//

#pragma once
#define _USE_MATH_DEFINES

#include "afxwin.h"
#include "afxcmn.h"
#include "stdlib.h"
#include <vector>
#include <string>
#include <algorithm>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <fstream>
#include <cmath>
#include <ctime>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "camerads.h"
#include "CameraCalib.h"
#include "LMOptimalFor3D.h"
#include "MeasureDistance.h"
#include "CvvImage.h"
#include "Camera.h"
#include "DataComm.h"
#include "LOWCOMDlg.h"
#include "ParamReviseDlg.h"

#include "Utils.h"

using namespace std;
using namespace cv;

// CStereoCameraDlg 对话框
class CStereoCameraDlg : public CDialogEx
{
	// 构造
public:
	CStereoCameraDlg(CWnd* pParent = NULL);	// 标准构造函数

	// 对话框数据
	enum { IDD = IDD_STEREOCAMERA_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CString m_workDir;
	CameraCalib m_cameraCalibrator;
	CameraCalib::CalibrationParames m_CalibrationResult1;
	CameraCalib::CalibrationParames m_CalibrationResult2;
	CameraCalib::CalibrationParames m_SignalCalibrationResult;

	MeasureDistance m_measureDist;
	//Camera m_cameraSupport1;   //相机1
	//Camera m_cameraSupport2;  //相机2
	int m_camType;
	bool m_cameraStatusOK = false;
	bool m_cameraBothOpen = false; //上下相机是否同时打开
	BOOL m_isWhichCamera;          //判断相机位置

	//修正摄像机参数
	Mat m_rotateMat;
	Mat m_translateMat;

	int m_imgCount;  //拍摄时的计数
	CStatic	m_PicLfArea;      //左视图
	CStatic	m_PicRtArea;      //右视图


#pragma region 控件关联变量
	//相机标定参数
    int m_nBoardWidth;         //标定板每行角点数 
	int m_nBoardHeight;        //标定板每列角点数
	float m_nSquareSize;      //标定板棋格实际尺寸
	float m_nPixelSize;       //像元尺寸

	double m_ZmeasRange1;       //Z轴测量range
	double m_ZmeasRange2;
	//修正参数
	double up_x;
	double up_y;
	double down_x;
	double down_y;
	CListBox m_RunStaInfo;    // 运行状态信息;
	BOOL m_calRadBN;            //是否在线计算
	BOOL m_calRadArg;           //计算几何参数Radio

	//ComboxSelect
	CString m_CameraSelect;
	int m_CameraSelectID = 0;
	CComboBox m_CameraSelectCombox;

	//picture control
	CStatic m_picDraw;
	//确定坐标图四周预留的空白大小
	const int mytop = 50;
	const int mybottom = 50;
	const int myleft = 120;
	const int myright = 100;
	float minX;
	float maxX;
	float minY;
	float maxY;
	const int m_margin = 5;
	float intervalX;
	float intervalY;

	//重建出来的结构光三维坐标
	vector<Point2d> m_deal2DPoints1;
	vector<Point2d> m_deal2DPoints2;
	vector<Point2d> m_deal2DPointsAll;
	double m_XiuVer;
	double m_XiuHor;

	//////////////////////////////
	vector<Mat> upLayerMat;
	vector<Mat> downLayerMat;
	bool doLayerTest = false;

#pragma endregion 控件关联变量


	//-----------------------功能函数--------------------------

	/*----------------------------
	* 功能 : SDK初始化相机
	*----------------------------
	* 返回 : 0 - 操作失败，1 - 操作成功
	*/
	bool InitCamera();

	Mat* Snapshot(int hCamera, int type);

	/*----------------------------
	* 功能 : 显示图像
	*		 将要绘制的图像 src 显示到控件号为 ID 的 Picture 控件
	*----------------------------
	* 参数 : src	[in]	待显示图像
	* 参数 : ID		[in]	图像窗口控件ID
	*/
	void ShowImage(Mat& src, UINT ID);

	vector<CStringA> DoSelectFiles(LPCTSTR lpszDefExt, DWORD dwFlags, LPCTSTR lpszFilter, LPCTSTR lpstrTitle, LPCTSTR lpstrInitialDir);

	//显示运行状态信息，由于执行的次数较多，定义为内联函数;
	void inline RunStateInfoShow(LPCTSTR Info)
	{
		m_RunStaInfo.AddString(Info);
		m_RunStaInfo.SetCurSel(m_RunStaInfo.GetCount() - 1);

	}



	void ShowRes();
	void DrawPic(CDC *pDC, CRect &rectPicture, vector<Point2d> Points, double xmin, double xmax, double ymin, double ymax);
	//绘制图像

	//读入相机标定参数
	void InitCalibParams(CameraCalib::CalibrationParames& cameraCalibParams, int whichNum);



public:
	afx_msg void OnBnClickedBn2stereocalib();
	afx_msg void OnBnClickedBnsetlfcam();
	afx_msg void OnBnClickedBnsetrtcam();
	afx_msg void OnClose();
	afx_msg void OnBnClickedBnruncam();
	afx_msg void OnBnClickedBnDefaultcamcalibparam();
	afx_msg void OnBnClickedBngrabbothcam();
	afx_msg void OnBnClickedBnsavecalibparams();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedBnxiuzheng();
	afx_msg void OnBnClickedBnreadfrommatlab();
	afx_msg void OnCbnSelchangeCameraselect();
	afx_msg void OnBnClickedBncalculate();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedBtrunmind();
	afx_msg void OnBnClickedBnrunupdown();
	afx_msg void OnBnClickedMeasurea1a2a3a4();

	DECLARE_EVENTSINK_MAP()


	afx_msg void OnBnClickedBnsaveConfig();

	afx_msg void OnBnClickedOpenlayerpic();
	afx_msg void OnBnClickedCloselayerpic();
	afx_msg void OnBnClickedBnadjust();
	afx_msg void OnBnClickedBnRevset();
	afx_msg void OnBnClickedButton1();

	afx_msg void OnBnClickedButton2();
};
