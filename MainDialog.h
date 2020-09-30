#pragma once
#include "StereoCameraDlg.h"
#include "MeasureDistance.h"
#include "CameraCalib.h"
#include "Camera.h"
#include "mscomm1.h"
#include "DataComm.h"
#include "InstructionDlg.h"
#include "ColoredStatusBarCtrl.h"
#include "Creminder.h"
#include "afxwin.h"
#include "Utils.h"
UINT ThreadFunc(LPVOID lpParam);
UINT ThreadFunc1(LPVOID lpParam);
UINT ThreadFunc2(LPVOID lpParam);
UINT ThreadFunc3(LPVOID lpParam);
extern BOOL bExit;
#define WM_RECVDATA WM_USER+1
// CMainDialog dialog
extern bool openInstructionBOOL;
class CMainDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CMainDialog)

public:
	CMainDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMainDialog();

// Dialog Data
	enum { IDD = IDD_MAIN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg LRESULT OnRecvData(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	CWinThread* m_pThread = NULL;
	CWinThread* m_pThread1 = NULL;
	CWinThread* m_pThread2 = NULL;
	CWinThread* m_pThread3 = NULL;
	CString m_workDir;
	MeasureDistance m_measureDist;
	CStereoCameraDlg * m_stereDlg = NULL;
	CInstructionDlg  *m_pInstructionDlg = NULL;
	Creminder  *m_reminderDlg = NULL;
	BOOL measurefinish=true;
	BOOL measuredownfinish = false;
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
	//vector<Point3d> m_realPoints;
	vector<Point2d> m_deal2DPoints;
	vector<Point2d> m_dealPoints1, m_dealPoints11;
	vector<Point2d> m_dealPoints2, m_dealPoints22;
	CStringA s_a1, s_a2, s_a3, s_a4;
	double  a3=0, a4=0, a1_maxy=0, a2=0;
	double  a2_minx = 0, a2_miny = 0, a2_minx2;
	vector<Point2d> pointsA1, pointsA2, pointsA, pointsA3, pointsA4, pointsA5;
	vector<Mat> m_measPics;
	vector<Mat> m_measPics2;
	////保存图片的map
	map<string, Mat> m_savePics;
	CString m_savePlace; //保存图片路径
	int showstapic;
	string up_camera;
	string down_camera;
	CameraCalib::CalibrationParames m_CalibrationResult1;
	CameraCalib::CalibrationParames m_CalibrationResult2;
	double up_x;
	double up_y;
	double down_x;
	double down_y;
	double A1, A2, A3, A4, trackDis_standard;
	bool savetx;
	void  Read_config();
	virtual BOOL OnInitDialog();

	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnBnClickedBncaculatemain();

	//绘制图像
	void CMainDialog::DrawPic(CDC *pDC, CRect &rectPicture, vector<Point2d>Points, double xmin, double xmax, double ymin, double ymax);
	void CMainDialog::DrawPic(CDC *pDC, CRect &rectPicture, vector<Point2d>Points1, vector<Point2d>Points2, double xmin, double xmax, double ymin, double ymax, float showTrackDis);
	void ShowRes();
	//拍摄相片 ,num是拍摄张数
	void CameraTakePics(Camera& cameraSupport, int num, vector<Mat>& imgs);
	void CameraTakePics(Camera& cameraSupport, Mat& imgs);
	void MeanCameraImgs(const vector<Mat>& inImgs, Mat& outImg);
	void SaveCameraImgs(const vector<Mat>& imgs);
	void Measure();
	void Measureup();
	void Measuredown();
	BOOL Takepicture();
	void savetxt();
	//设置电量图标,type =1 初始化, type=2 更新
	void SetBatteryPic(int battery,int type);
	vector<CStringA> DoSelectFiles(LPCTSTR lpszDefExt, DWORD dwFlags, LPCTSTR lpszFilter, LPCTSTR lpstrTitle, LPCTSTR lpstrInitialDir);
	void InitCalibParams(CameraCalib::CalibrationParames& cameraCalibParams, int whichNum);
	//void ShowImage(Mat& src, UINT ID);
	afx_msg void OnBnClickedBnDebug();
	DECLARE_EVENTSINK_MAP()
	void OnCommMscomm1();
	CString m_SDtrackDis;
	CString m_SDhorizontal;
	CString m_SDbattery;
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CBrush m_bkBrush;
	CStatusBar  m_bar;

private:
	CFont font;
	CFont m_Font;
	CFont m_font;
public:
	afx_msg void OnBnClickedInstruction();
	CButton m_check_save;
	afx_msg void OnBnClickedTest();
	//CEdit m_edit_testprocess;
	bool m_save;
	bool m_commStatus = false;
	int standlevel;
	//bool starting = 1;
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnEnChangeA1hh();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
};
