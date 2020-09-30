// MainDialog.cpp : implementation file
//

#include "stdafx.h"
#include "StereoCamera.h"
#include "MainDialog.h"
#include "afxdialogex.h"
#include "Creminder.h"
#include <WinSock2.h>
#include <IPHlpApi.h>
#include <iostream>
#include <Windows.h>
#include <string>
//#include "RegisterDlg.h"
#include "WizardDlg.h"

#pragma comment(lib, "Iphlpapi.lib")

using namespace std;

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

//用来储存相关信息
DWORD deax;
DWORD debx;
DWORD decx;
DWORD dedx;

//CONTROL         IDB_BATTERY0, IDC_battery0, "Static", SS_BITMAP, 229, 12, 31, 16, NOT WS_GROUP
//CONTROL         IDB_BATTERY1, IDC_battery1, "Static", SS_BITMAP, 229, 12, 31, 16, NOT WS_GROUP
//CONTROL         IDB_BATTERY2, IDC_battery2, "Static", SS_BITMAP, 229, 12, 31, 16, NOT WS_GROUP
//CONTROL         IDB_BATTERY3, IDC_battery3, "Static", SS_BITMAP, 229, 12, 31, 16, NOT WS_GROUP
//CONTROL         IDB_BATTERY4, IDC_battery4, "Static", SS_BITMAP, 229, 12, 31, 16, NOT WS_GROUP
//CONTROL         "", IDC_PICSTATIC_MAIN, "Static", SS_BITMAP, 6, 30, 7, 7, NOT WS_GROUP
//CONTROL         IDB_BITMAP9, IDC_EARE, "Static", SS_BITMAP, 5, 355, 21, 18, NOT WS_GROUP
//CONTROL         IDB_BITMAP5, IDC_STATIC_FINISH1, "Static", SS_BITMAP, 5, 355, 21, 18, NOT WS_GROUP
//CONTROL         IDB_BITMAP4, IDC_STATIC_MESURE1, "Static", SS_BITMAP, 5, 355, 21, 18, NOT WS_GROUP
//CONTROL         IDB_BITMAP5, IDC_STATIC_FINISH2, "Static", SS_BITMAP, 85, 355, 21, 18, NOT WS_GROUP
//CONTROL         IDB_BITMAP5, IDC_STATIC_FINISH3, "Static", SS_BITMAP, 165, 355, 21, 18, NOT WS_GROUP
//CONTROL         IDB_BITMAP4, IDC_STATIC_MESURE2, "Static", SS_BITMAP, 85, 355, 21, 18, NOT WS_GROUP
//CONTROL         IDB_BITMAP4, IDC_STATIC_MESURE3, "Static", SS_BITMAP, 165, 355, 21, 18, NOT WS_GROUP
//CONTROL         IDB_BITMAP9, IDC_STATIC_STATE, "Static", SS_BITMAP, 85, 355, 21, 18, NOT WS_GROUP
//  上面的东西不要乱删
bool openInstructionBOOL = false;
static UINT indicators[] = { IDS_INDICATOR_MESSAGE, IDS_INDICATOR_COMMSTATUS, IDS_INDICATOR_SAVERESULTS, IDS_INDICATOR_TIME };

BOOL Flag = FALSE;

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
CMutex mutex;
//测量线程
/*****************************************************************/
UINT ThreadFunc(LPVOID lpParam)
{
	CMainDialog * pC = (CMainDialog *)lpParam;
	while (true)
	{
		pC->Takepicture();
		pC->m_pThread->SuspendThread();
	}
	return 0;
}
UINT ThreadFunc1(LPVOID lpParam)
{
	CMainDialog * pC = (CMainDialog *)lpParam;
	while (true)
	{
		if (pC->measuredownfinish)
		{
			pC->Measuredown();
			pC->measuredownfinish = false;
			pC->measurefinish = true;
			start_measure = false;
			pC->m_pThread1->SuspendThread();
		}
	}
	return 0;
}
UINT ThreadFunc2(LPVOID lpParam)
{
	CMainDialog * pC = (CMainDialog *)lpParam;
	while (true)
	{
		pC->Measureup();
		pC->measuredownfinish = true;
		
		pC->m_pThread2->SuspendThread();
	}
	return 0;
}
UINT ThreadFunc3(LPVOID lpParam)
{
	CMainDialog * pC = (CMainDialog *)lpParam;
	while (true)
	{
		CSingleLock singleLock(&mutex);
		singleLock.Lock();
		pC->SaveCameraImgs(pC->m_measPics2);
		pC->savetxt();
		pC->PostMessage(WM_RECVDATA, 0, 5);
		pC->SaveCameraImgs(pC->m_measPics);
		singleLock.Unlock();
		save_result = false;
		pC->m_pThread3->SuspendThread();	
	}
	return 0;
}
/************************************************/
IMPLEMENT_DYNAMIC(CMainDialog, CDialogEx)

CMainDialog::CMainDialog(CWnd* pParent /*=NULL*/)
: CDialogEx(CMainDialog::IDD, pParent)
, m_SDtrackDis(_T(""))
, m_SDhorizontal(_T(""))
, m_SDbattery(_T(""))
{

}

CMainDialog::~CMainDialog()
{
	if (NULL != m_stereDlg)
	{
		delete m_stereDlg;
	}
	if (NULL != m_pInstructionDlg)
	{
		delete m_pInstructionDlg;
	}

}

void CMainDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_TCHART1, m_chart3);
	DDX_Control(pDX, IDC_PICDRAW_MAIN, m_picDraw);
	DDX_Control(pDX, IDC_MSCOMM1, m_MSCOMM1);
	DDX_Text(pDX, IDC_SD_trackDis, m_SDtrackDis);
	DDX_Text(pDX, IDC_SD_horizontal, m_SDhorizontal);
	DDX_Text(pDX, IDC_SD_battery, m_SDbattery);
	//DDX_Control(pDX, IDC_EDIT_TESTPROCESS, m_edit_testprocess);
}

BEGIN_MESSAGE_MAP(CMainDialog, CDialogEx)
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDC_BNCaculateMain, &CMainDialog::OnBnClickedBncaculatemain)
	ON_BN_CLICKED(IDC_BN_DEBUG, &CMainDialog::OnBnClickedBnDebug)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	//ON_BN_CLICKED(IDC_INSTRUCTION, &CMainDialog::OnBnClickedInstruction)
	ON_BN_CLICKED(IDC_TEST, &CMainDialog::OnBnClickedTest)
	ON_WM_DRAWITEM()
	ON_EN_CHANGE(IDC_A1HH, &CMainDialog::OnEnChangeA1hh)
	ON_MESSAGE(WM_RECVDATA,OnRecvData)
	//ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()
/************************************/
LRESULT CMainDialog::OnRecvData(WPARAM wParam, LPARAM lParam)
{
	//m_stereDlg->UpdateData(TRUE);
	CTime time;
	CString str;
	time = CTime::GetCurrentTime();
	switch (int(lParam))
	{
	case 1:
		m_bar.SetPaneText(0, "测量...", 1);
		str.Format(_T("未存储 %s"), time.Format("%H:%M:%S"));
		m_bar.SetPaneText(1, str, 1);
		GetDlgItem(IDC_STATIC_FINISH2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_MESURE2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_FINISH1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_MESURE1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EARE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_STATE)->ShowWindow(SW_HIDE);
		break;
	case 2:
		m_bar.SetPaneText(0, "异常...", 1);
		GetDlgItem(IDC_STATIC_FINISH1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_MESURE1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EARE)->ShowWindow(SW_SHOW);
		break;
	case 3:
		m_bar.SetPaneText(1, "相机异常:需重启", 1);
		GetDlgItem(IDC_STATIC_STATE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_MESURE2)->ShowWindow(SW_HIDE);
		/*m_bar.SetPaneText(0, "测量下...", 1);
		GetDlgItem(IDC_STATIC_FINISH1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_MESURE1)->ShowWindow(SW_HIDE);*/
		break;
	case 4:
		m_bar.SetPaneText(0, "测量完成", 1);
		GetDlgItem(IDC_STATIC_MESURE1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_FINISH1)->ShowWindow(SW_HIDE);
		break;
	case 5:
		str.Format(_T("已存储 %s"), time.Format("%H:%M:%S"));
		m_bar.SetPaneText(1, str, 1);
		GetDlgItem(IDC_STATIC_MESURE2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_FINISH2)->ShowWindow(SW_HIDE);
		break;
	default:
		break;
	}
	return true;
}
/***********************************************************/
BOOL CMainDialog::OnInitDialog()
{
CDialogEx::OnInitDialog();
//状态栏设置
	m_bar.Create(this);
	m_bar.SetIndicators((unsigned int *)indicators, sizeof(indicators) / sizeof(UINT));
	CRect rect;
	GetClientRect(&rect);
	m_bar.SetPaneInfo(0, IDS_INDICATOR_MESSAGE, SBPS_NORMAL, 84);
	m_bar.SetPaneInfo(1, IDS_INDICATOR_COMMSTATUS, SBPS_NORMAL,158);
	m_bar.SetPaneInfo(2, IDS_INDICATOR_SAVERESULTS, SBPS_NORMAL, 84);
	m_bar.SetPaneInfo(3, IDS_INDICATOR_TIME, SBPS_STRETCH, 0);
	LOGFONT logfont;
	GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(logfont), &logfont);
	logfont.lfCharSet = GB2312_CHARSET;
	logfont.lfWeight  = FW_NORMAL;
	logfont.lfHeight  = 30;
	lstrcpy(logfont.lfFaceName, _T("微软雅黑"));//微软雅黑
	static CFont sfontStatusBar;
	sfontStatusBar.CreateFontIndirect(&logfont);
	m_bar.SetFont(&sfontStatusBar);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, ID_INDICATOR_REC);

	for (int i = 0; i < 4; i++)
	{
		m_bar.GetStatusBarCtrl().SetText("", i, SBT_OWNERDRAW);
		//m_bar.SetPaneInfo(i, m_bar.GetItemID(i), SBPS_STRETCH, NULL);
	}
	CTime time;
	CString str;
	time = CTime::GetCurrentTime();
	str.Format(_T("未存储 %s"), time.Format("%H:%M:%S"));
	GetDlgItem(IDC_STATIC_FINISH2)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_MESURE2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_STATE)->ShowWindow(SW_HIDE);
	m_bar.SetPaneText(1, str, 1);
	m_bar.SetPaneText(0, "测试完成", 1);
	/*if (m_commStatus)*/
	m_bar.SetPaneText(2, "通讯正常", 1);
	CTime time1;
	time1 = CTime::GetCurrentTime();
	m_bar.SetPaneText(3, time1.Format("%H:%M:%S"), 1);
	//else
	//{
	//	m_bar.SetPaneText(2, "通讯异常", 1);//m_bar.m_commStatus = _T("通讯异常"); m_bar.GetStatusBarCtrl().SetBkColor(RGB(68, 68, 68));	
	//	//GetDlgItem(IDC_STATIC_FINISH3)->ShowWindow(SW_SHOW);
	//	//GetDlgItem(IDC_STATIC_MESURE3)->ShowWindow(SW_HIDE);
	//}
//设置定时器
	SetTimer(1, 1000, NULL);
	SetTimer(2, 10000, NULL);
	SetTimer(3, 2500, NULL);
//设置主界面字体大小
	CEdit *m_A1a0 = (CEdit*)GetDlgItem(IDC_STATIC_A1);
	CEdit *m_A1a1 = (CEdit*)GetDlgItem(IDC_VALUE_A1_M);
	CEdit *m_A1a2 = (CEdit*)GetDlgItem(IDC_STATIC1);
	CEdit *m_B1b0 = (CEdit*)GetDlgItem(IDC_STATIC_A2);
	CEdit *m_B1b1 = (CEdit*)GetDlgItem(IDC_VALUE_A2_M);
	CEdit *m_B1b2 = (CEdit*)GetDlgItem(IDC_STATIC2);
	CEdit *m_C1c0 = (CEdit*)GetDlgItem(IDC_STATIC_A3);
	CEdit *m_C1c1 = (CEdit*)GetDlgItem(IDC_VALUE_A3_M);
	CEdit *m_C1c2 = (CEdit*)GetDlgItem(IDC_STATIC3);
	CEdit *m_D1d0 = (CEdit*)GetDlgItem(IDC_STATIC_A4);
	CEdit *m_D1d1 = (CEdit*)GetDlgItem(IDC_VALUE_A4_M);
	CEdit *m_D1d2 = (CEdit*)GetDlgItem(IDC_STATIC4);
	CEdit *m_guijue0 = (CEdit*)GetDlgItem(IDC_STATIC_S1);
	CEdit *m_guijue1 = (CEdit*)GetDlgItem(IDC_SD_trackDis);
	CEdit *m_guijue2 = (CEdit*)GetDlgItem(IDC_STATIC5);
	CEdit *m_shuipingf0 = (CEdit*)GetDlgItem(IDC_STATIC_S2);
	CEdit *m_shuipingf1 = (CEdit*)GetDlgItem(IDC_SD_horizontal);
	CEdit *m_shuipingf2 = (CEdit*)GetDlgItem(IDC_STATIC6);
	m_Font.CreatePointFont(180, "黑体");
	font.CreateFont(
		60,                        // nHeight
		20,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily
		_T("Arial"));                 // lpszFacename
	m_A1a0->SetFont(&font);
	m_A1a1->SetFont(&font);
	m_A1a2->SetFont(&m_Font);
	m_B1b0->SetFont(&font);
	m_B1b1->SetFont(&font);
	m_B1b2->SetFont(&m_Font);
	m_C1c0->SetFont(&font);
	m_C1c1->SetFont(&font);
	m_C1c2->SetFont(&m_Font);
	m_D1d0->SetFont(&font);
	m_D1d1->SetFont(&font);
	m_D1d2->SetFont(&m_Font);
	m_guijue0->SetFont(&font);
	m_guijue1->SetFont(&font);
	m_guijue2->SetFont(&m_Font);
	m_shuipingf0->SetFont(&font);
	m_shuipingf1->SetFont(&font);
	m_shuipingf2->SetFont(&m_Font);
//获取工作路径和配置文件一些参数
	m_workDir = GetAppExecPath();
	Read_config();
	//设置控件大小位置
	CWnd *pWnd;
	pWnd = GetDlgItem(IDC_PICDRAW_MAIN);
	pWnd->SetWindowPos(NULL,1, 80, 463, 275, SWP_NOZORDER);
	//判断相机初始化成功否
	bool bool_uvc = Camera::CameraInit(Camera::CAM_TYPE::UVC);
	if (Camera::cam_count != 2)
	{
		//IDD_REMINDER_DIALOG
		m_bar.SetPaneText(1, "相机异常:需重启", 1);
		GetDlgItem(IDC_STATIC_STATE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_MESURE2)->ShowWindow(SW_HIDE);
		/*if (m_reminderDlg == NULL)
		{
		m_reminderDlg = new Creminder();
		m_reminderDlg->Create(IDD_REMINDER_DIALOG, this);
		}
		m_reminderDlg->ShowWindow(SW_SHOW);*/
		/*if (AfxMessageBox("摄像头异常？", MB_YESNO) == IDYES)
		{
			ReSetWindows(EWX_REBOOT | EWX_FORCE, true);
		}*/
	}
  else if (Camera::cam_count == 2)
		{
			m_cameraSupport1.CameraOpen(Camera::CAM_TYPE::UVC, atoi(up_camera.c_str()) - 1);
			m_cameraSupport2.CameraOpen(Camera::CAM_TYPE::UVC, atoi(down_camera.c_str()) - 1);
		}
//说明界面初始化
  //读取配置文件和标定参数.......注：软件重启生效
	ReadConfigFile();
	char strTmp[20];
	sprintf_s(strTmp, "%.1f", sensorData.trackDis);
	GetDlgItem(IDC_SD_trackDis)->SetWindowTextA(strTmp);
	sprintf_s(strTmp, "%.1f", sensorData.Lvalue);
	GetDlgItem(IDC_SD_horizontal)->SetWindowTextA(strTmp);
	GetDlgItem(IDC_SD_battery)->SetWindowTextA(to_string(sensorData.battery).c_str());
	SetBatteryPic(sensorData.battery, 1);
//显示静态图片
if (showstapic == 0)
	{
		HBITMAP hBitmap;    //添加图片
		//关联图片ID
		hBitmap = (HBITMAP)LoadImage(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDB_SHIYITU0),
			IMAGE_BITMAP, 0, 0,
			LR_LOADMAP3DCOLORS);

		CStatic *pStatic = (CStatic *)GetDlgItem(IDC_PICSTATIC_MAIN); //获得指向静态控件的指针
		pStatic->ModifyStyle(0xF, SS_BITMAP | SS_CENTERIMAGE);  //设置静态控件的样式，使其位图
		pStatic->MoveWindow(1, 80, 463, 275, TRUE);
		pStatic->SetBitmap(hBitmap);     //设置静态控件显示位图
	}	
//ShowCursor(FALSE);
//创建MFC线程并挂起
	m_pThread = AfxBeginThread(ThreadFunc, this, THREAD_PRIORITY_ABOVE_NORMAL, 0, CREATE_SUSPENDED);    
	m_pThread1 = AfxBeginThread(ThreadFunc1, this, THREAD_PRIORITY_ABOVE_NORMAL, 0, CREATE_SUSPENDED);  
	m_pThread2 = AfxBeginThread(ThreadFunc2, this, THREAD_PRIORITY_ABOVE_NORMAL, 0, CREATE_SUSPENDED); 
	m_pThread3 = AfxBeginThread(ThreadFunc3, this, THREAD_PRIORITY_ABOVE_NORMAL, 0, CREATE_SUSPENDED);
//打开串口
	open_COM();
//开始发送结束测量命令
	stop_measure();
	vector<Mat>Imgs1, Imgs2;
	CameraTakePics(m_cameraSupport1, 5, Imgs1);  //拍摄两张
	CameraTakePics(m_cameraSupport2, 5, Imgs2);
	return TRUE;
}
void CMainDialog::Read_config()
{
	m_save = atoi(GetPropertyByName("camera", "save").c_str());
	savetx = atoi(GetPropertyByName("camera", "savetxt").c_str());
	up_camera = GetPropertyByName("camera", "up_camera");
	down_camera = GetPropertyByName("camera", "down_camera");
	showstapic = atoi(GetPropertyByName("camera", "show static picture").c_str());
	standlevel = atoi(GetPropertyByName("camera", "standlevel").c_str());
	A1 = atof(GetPropertyByName("Standard Key Points Of WuXi", "A1").c_str());
	A2 = atof(GetPropertyByName("Standard Key Points Of WuXi", "A2").c_str());
	A3 = atof(GetPropertyByName("Standard Key Points Of WuXi", "A3").c_str());
	A4 = atof(GetPropertyByName("Standard Key Points Of WuXi", "A4").c_str());
	trackDis_standard = atof(GetPropertyByName("Standard Key Points Of WuXi", "trackDis_standard").c_str());
	GetRevParam(up_x, up_y, down_x, down_y);//得到配置文件的修正参数
	InitCalibParams(m_CalibrationResult1, 1);
	InitCalibParams(m_CalibrationResult2, 2);
}
void CMainDialog::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: Add your message handler code here and/or call default
	// 将最小跟踪与最大跟踪设置成相同的值使窗口大小不能改变
	lpMMI->ptMinTrackSize.x = 480; // 设定最小跟踪宽度
	lpMMI->ptMinTrackSize.y = 800; // 设定最小跟踪高度
	lpMMI->ptMaxTrackSize.x = 480; // 设定最大跟踪宽度
	lpMMI->ptMaxTrackSize.y = 800; // 设定最大跟踪高度
	CDialogEx::OnGetMinMaxInfo(lpMMI);
}

BEGIN_EVENTSINK_MAP(CMainDialog, CDialogEx)
	ON_EVENT(CMainDialog, IDC_MSCOMM1, 1, CMainDialog::OnCommMscomm1, VTS_NONE)
END_EVENTSINK_MAP()

void CMainDialog::ShowRes()
{  //显示测量结果

	FILE* validPointfp1 = fopen("validPoint1.txt", "w");
	for (int i = 0; i < pointsA.size(); i++)
	{
	fprintf(validPointfp1, "%10f,%10f\n", pointsA[i].x, pointsA[i].y);
	}

	FILE* validPointfp2 = fopen("validPoint2.txt", "w");
	for (int i = 0; i < pointsA5.size(); i++)
	{
	fprintf(validPointfp2, "%10f,%10f\n", pointsA5[i].x, pointsA5[i].y);
	}

	fclose(validPointfp1);
	fclose(validPointfp2);

	CRect picRect;
	m_picDraw.GetClientRect(&picRect);
	
	vector<double> xlist;
	vector<double> ylist;
	for (int i = 0; i != pointsA.size(); i++)
	{
		xlist.push_back(pointsA[i].x);
		ylist.push_back(pointsA[i].y);
	}
	for (int i = 0; i != pointsA5.size(); i++)
	{
		xlist.push_back(pointsA5[i].x);
		ylist.push_back(pointsA5[i].y);
	}

	if (xlist.empty() || ylist.empty())
		return;

	sort(xlist.begin(), xlist.end());
	sort(ylist.begin(), ylist.end());
	float showTrackDis =0;
	if (standlevel == 1)
	{
	    showTrackDis = (sensorData.trackDisRef + sensorData.trackDis) / 2;
	}
	DrawPic(m_picDraw.GetDC(), picRect, pointsA, pointsA5, xlist[0], xlist[xlist.size() - 1], ylist[0], ylist[ylist.size() - 1], showTrackDis);

	s_a1.Format("%.1f", a1_maxy);
	s_a3.Format("%.1f", a3);
	s_a2.Format("%.1f", a2);
	s_a4.Format("%.1f", a4);
	GetDlgItem(IDC_VALUE_A1_M)->SetWindowTextA(s_a1);
	GetDlgItem(IDC_VALUE_A2_M)->SetWindowTextA(s_a2);
	GetDlgItem(IDC_VALUE_A3_M)->SetWindowTextA(s_a3);
	GetDlgItem(IDC_VALUE_A4_M)->SetWindowTextA(s_a4);
}
void CMainDialog::savetxt()
{
	/*int savetxt = atoi(GetPropertyByName("camera", "savetxt").c_str());
	if (savetxt == 1)
	{*/
		CTime t = CTime::GetCurrentTime();
		CString Time = t.Format("%Y.%m.%d %H:%M:%S     ");
		/*CString data = t.Format("%Y-%m-%d");*/

		CString GJ, SP;
		float showTrackDis = sensorData.trackDisRef + sensorData.trackDis;
		GJ.Format("%.1f", showTrackDis);
		SP.Format("%.1f", sensorData.Lvalue);

		CString title = _T("时间\t\t\t公里标\t操作员\tA1\tA2\tA3\tA4\t轨距\t水平\t保存\n");
		CString GLB = _T("    ");
		CString CZY = _T("    ");
		CString tt = _T("\t");
		CString enter = _T("\n");
		CString Filename;
		CString place = "D:\\TTMS";
		CString place0 = "Data";
		CString save_yes = _T("按钮保存");
		//CString save_yes = _T("自动保存");
		if (save_result)
		{
			save_yes = _T("按钮保存");
		}
		if (!PathFileExists(place))
		{
			CreateDirectory(place, NULL);
		}
		CString path;
		path.Format("%s\\%s", place, place0);
		if (!PathFileExists(path))
		{
			CreateDirectory(path, NULL);
		}
		Filename.Format(_T(path + "\\便携式接触轨数字检测仪记录表.txt"));

		CStdioFile txt(Filename, CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate);
		txt.WriteString(title);
		if (txt)
		{
			txt.SeekToEnd();
			txt.WriteString(Time);
			txt.WriteString(GLB);
			txt.WriteString(tt);
			txt.WriteString(CZY);
			txt.WriteString(tt);
			txt.WriteString(s_a1);
			txt.WriteString(tt);
			txt.WriteString(s_a2);
			txt.WriteString(tt);
			txt.WriteString(s_a3);
			txt.WriteString(tt);
			txt.WriteString(s_a4);
			txt.WriteString(tt);
			txt.WriteString(GJ);
			txt.WriteString(tt);
			txt.WriteString(SP);
			txt.WriteString(tt);
			txt.WriteString(save_yes);
			txt.WriteString(enter);

			txt.Close();
		/*}*/
	}
}

void CMainDialog::CameraTakePics(Camera& cameraSupport, int num, vector<Mat>& imgs)
{
	if (!cameraSupport.CameraIsOpen()) //判断相机是否打开
		return;
	for (int i = 0; i < num; i++) //拍摄
	{
		Mat frame = cameraSupport.getOneFrame();
		imgs.push_back(frame);
	}
}
void CMainDialog::CameraTakePics(Camera& cameraSupport,Mat& imgs)
{
	if (!cameraSupport.CameraIsOpen()) //判断相机是否打开
		return;
	imgs = cameraSupport.getOneFrame();
}
BOOL CMainDialog::Takepicture()
{
	try
	{
		PostMessage(WM_RECVDATA, 0, 1);
		GetDlgItem(IDC_STATIC_FINISH1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_MESURE1)->ShowWindow(SW_HIDE);
		m_measPics.clear();
		m_measPics2.clear();
		//创建保存文件夹
		//先上下同时拍摄照片
		vector<Mat> upLayerImgs, upImgs, downLayerImgs, downImgs;
		vector<Mat> upLayerImgs2, upImgs2, downLayerImgs2, downImgs2;
		vector<Mat> upLayerImgs3, upImgs3, downLayerImgs3, downImgs3;
		Mat upImg, upLayerImg, downImg, downLayerImg;
		Mat upImg2, upLayerImg2, downImg2, downLayerImg2;
		Mat upImg3, upLayerImg3, downImg3, downLayerImg3;
		//Mat upImg4, upLayerImg4, downImg4, downLayerImg4;
	    do_close_allLayer();
		//CameraTakePics(m_cameraSupport1, 1, upImgs);  //拍摄两张
		//CameraTakePics(m_cameraSupport2, 1, downImgs);
		//CameraTakePics(m_cameraSupport1, 1, upImgs2);  //拍摄两张
		//CameraTakePics(m_cameraSupport2, 1, downImgs2);
		//CameraTakePics(m_cameraSupport1, 1, upImgs3);  //拍摄两张
		//CameraTakePics(m_cameraSupport2, 1, downImgs3);
		//upImg = m_cameraSupport1.getOneFrame();
		CameraTakePics(m_cameraSupport1, upImg);
		CameraTakePics(m_cameraSupport1, upImg2);
		CameraTakePics(m_cameraSupport1, upImg3);
		CameraTakePics(m_cameraSupport2, downImg);
		CameraTakePics(m_cameraSupport2, downImg2);
		CameraTakePics(m_cameraSupport2, downImg3);
		/*upImg2 = m_cameraSupport1.getOneFrame();	
		upImg3 = m_cameraSupport1.getOneFrame();*/
		/*upImg4 = m_cameraSupport1.getOneFrame();*/
		do_open_topLayer(); //打开上部激光
		Sleep(400);
		//CameraTakePics(m_cameraSupport1, 1, upLayerImgs);
		//upLayerImg = m_cameraSupport1.getOneFrame();
		CameraTakePics(m_cameraSupport1, upLayerImg);
		Sleep(100);
		CameraTakePics(m_cameraSupport1, upLayerImg2);
		//upLayerImg2 = m_cameraSupport1.getOneFrame();
		Sleep(100);
		CameraTakePics(m_cameraSupport1, upLayerImg3);
		//upLayerImg3 = m_cameraSupport1.getOneFrame();
		/*Sleep(100);
		upLayerImg4 = m_cameraSupport1.getOneFrame();*/
		do_close_topLayer();
		Sleep(100);
		/*if (m_commStatus == false)
		{
			measurefinish = true;
			start_measure = false;
			return false;
		}*/
		/*MeanCameraImgs(upImgs, upImg);
		MeanCameraImgs(upLayerImgs, upLayerImg);
		MeanCameraImgs(upImgs2, upImg2);
		MeanCameraImgs(upLayerImgs2, upLayerImg2);
		MeanCameraImgs(upImgs3, upImg3);
		MeanCameraImgs(upLayerImgs3, upLayerImg3);*/
		m_measPics2.push_back(upImg);
		m_measPics2.push_back(upLayerImg);
		m_measPics2.push_back(upImg2);
		m_measPics2.push_back(upLayerImg2);
		m_measPics2.push_back(upImg3);
		m_measPics2.push_back(upLayerImg3);
		/*m_measPics2.push_back(upImg4);
		m_measPics2.push_back(upLayerImg4);*/
		//SaveCameraImgs(m_measPics2);
		m_pThread2->ResumeThread();
		/*downImg = m_cameraSupport2.getOneFrame();
		downImg2 = m_cameraSupport2.getOneFrame();
		downImg3 = m_cameraSupport2.getOneFrame();*/
		/*downImg4 = m_cameraSupport2.getOneFrame();*/
		Sleep(50);
		do_open_belowLayer(); // 打开下部激光
		Sleep(400);
		//CameraTakePics(m_cameraSupport2, 1, downLayerImgs);
		CameraTakePics(m_cameraSupport2, downLayerImg);
		//downLayerImg = m_cameraSupport2.getOneFrame();
		Sleep(100);
		CameraTakePics(m_cameraSupport2, downLayerImg2);
		//downLayerImg2 = m_cameraSupport2.getOneFrame();
		Sleep(100);
		CameraTakePics(m_cameraSupport2, downLayerImg3);
		//downLayerImg3 = m_cameraSupport2.getOneFrame();
	/*	Sleep(100);
		downLayerImg4 = m_cameraSupport2.getOneFrame();*/
		do_close_belowLayer();  // 关闭xia部激光
		/*MeanCameraImgs(downImgs, downImg);
		MeanCameraImgs(downLayerImgs, downLayerImg);
		MeanCameraImgs(downImgs2, downImg2);
		MeanCameraImgs(downLayerImgs2, downLayerImg2);
		MeanCameraImgs(downImgs3, downImg3);
		MeanCameraImgs(downLayerImgs3, downLayerImg3);*/
		m_measPics.push_back(downImg);
		m_measPics.push_back(downLayerImg);
		m_measPics.push_back(downImg2);
		m_measPics.push_back(downLayerImg2);
		m_measPics.push_back(downImg3);
		m_measPics.push_back(downLayerImg3);
		/*m_measPics.push_back(downImg4);
		m_measPics.push_back(downLayerImg4);*/
		//SaveCameraImgs(m_measPics);
		m_pThread1->ResumeThread();
		Sleep(10);	
	}
	catch (...)
	{
		PostMessage(WM_RECVDATA, 0, 2);
		measuredownfinish = false;
		measurefinish = true;
		start_measure = false;
		return false;
	}
	return true;
}
void CMainDialog::Measuredown()
{
	try
	{
		//PostMessage(WM_RECVDATA,0,3);	//m_bar.SetPaneText(0, "测试下...", 1);
		vector<double> a3_n, a4_n, a2_minx1_n;
		a3 = 0, a4 = 0;
		for (int t = 0; t < 1; t++)   //改动：三次改一次
		{
			vector<Point3d>  downRealPoints;
			Mat  downMat, downMat2;
			vector<vector<Point>> contor;
			cv::subtract(m_measPics[t*2+1], m_measPics[t*2], downMat); //下部图像
			if (downMat.channels() > 1)
			{
				cvtColor(downMat, downMat2, CV_BGR2GRAY);
			}
			findContours(downMat2, contor, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
			if (contor.empty())
			{
				PostMessage(WM_RECVDATA, 0, 3);
			}
			Size2i img_size1(downMat.cols / 2, downMat.rows);
			Mat d_img_left = downMat(Rect(0, 0, img_size1.width, img_size1.height));
			Mat d_img_right = downMat(Rect(img_size1.width, 0, img_size1.width, img_size1.height));
			//处理下部相机

			Mat downLeftCenter, downRithtCenter, downMatch;
			m_measureDist.caculateDistance(d_img_left, d_img_right, m_CalibrationResult2, downRealPoints, downLeftCenter, downRithtCenter, downMatch, 1, 2, 2);
			Mat rotateMat2 = m_CalibrationResult2.amendRotMat;
			m_dealPoints2.clear();
			m_measureDist.deal3DPoints(downRealPoints, m_dealPoints2, rotateMat2, 0);
			m_measureDist.deal2DPoints(m_dealPoints2, 0, down_x, down_y);
			double   a3_min=0, a4_min=0;
			double n_a3=0, n_a4=0;
			double a2_minx1 = a2_minx;
			pointsA3.clear(); pointsA4.clear(); pointsA5.clear();
			m_measureDist.measureA3A4(m_dealPoints2, n_a3, n_a4, a3_min, a4_min, pointsA3, pointsA4, pointsA5, a2_minx1, a2_miny);
			a2_minx1_n.push_back(a2_minx1);
			a3_n.push_back(n_a3);
			a4_n.push_back(n_a4);
		}
		std::sort(a3_n.begin(), a3_n.end());
		a3 = a3_n[0]+ 46; //(a3_n[1] + a3_n[2])/2;   //改动：三次改一次
		std::sort(a4_n.begin(), a4_n.end());
		a4 = a4_n[0]; //(a4_n[1] + a4_n[2])/2;   //改动：三次改一次
		std::sort(a2_minx1_n.begin(), a2_minx1_n.end());
		a2_minx = a2_minx1_n[0];   //改动：三次改一次
		if (standlevel == 0)
		{
			if (a4 >= 240)
			{
				a4 = a4 + 1;
			}
			if (a4 >= 300)
			{
				a3 = a3 + 0.5;
			}
			if (a4 >= 320)
			{
				a3 = a3 + 0.5;
				a4 = a4 + 1;
			}
			if (a4 >= 350)
			{
				a4 = a4 + 1;
			}
			if (a4 >= 400)
			{
				a3 = a3 + 0.5;
			}
			if (a4 >= 420)
			{
				a4 = a4 + 1;
			}
		}
		if (standlevel == 1)
		{
			float showTrackDis = 0;
			showTrackDis = (sensorData.trackDisRef + sensorData.trackDis) / 2;
			a3 = a3 + showTrackDis;
		}	
		if (standlevel == 2)
		{
			a3 = a3 - A3 - 46;
			a4 = a4 - A4;
		}
		ShowRes();
		savetxt();///////
		//UpdateWindow();

		/*if (savetx)
		{
			m_pThread3->ResumeThread();
			save_result = false;
		}*/

		/*int TempSave = atoi(GetPropertyByName("camera", "save").c_str());
		if (TempSave == 1)
		{
			m_pThread3->ResumeThread();
			save_result = false;
		}
		else
		{
			savetxt();
		}*/

		//结束测量
		start_measure = false;
		PostMessage(WM_RECVDATA, 0, 4);
		save_result = false;
		if (m_commStatus)
		{
			stop_measure();
		}
	}
	// 错误信息处理
	catch (...)
	{
		PostMessage(WM_RECVDATA, 0, 2);
		measuredownfinish = false;
		measurefinish = true;
		start_measure = false;
		return ;
	}

}
void CMainDialog::Measureup()
{
	try
	{
		//PostMessage(WM_RECVDATA, 0, 2);
		vector<double> a1_n, a2_n, a2_miny_n, a2_minx_n;
		a2 = 0, a1_maxy = 0, a2_minx = 0, a2_miny=0;
		for (int i = 0; i <1; i++)   //改动：三次改一次
		{	
			vector<Point3d> upRealPoints;
			Mat  upMat, upMat2;
			vector<vector<Point>> contor;
			cv::subtract(m_measPics2[i * 2 + 1], m_measPics2[i * 2 ], upMat); //上部图像
			if (upMat.channels() > 1)
			{
				cvtColor(upMat, upMat2, CV_BGR2GRAY);
			}
			findContours(upMat2, contor, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
			if (contor.empty())
			{
				PostMessage(WM_RECVDATA, 0, 3);//判断相机是否出现异常，线程抛出消息响应
			}
			Size2i img_size1(upMat.cols / 2, upMat.rows);
			Mat img_left = upMat(Rect(0, 0, img_size1.width, img_size1.height));
			Mat img_right = upMat(Rect(img_size1.width, 0, img_size1.width, img_size1.height));
			//处理下部相机

			Mat upLeftCenter, upRithtCenter, upMatch;
			m_measureDist.caculateDistance(img_left, img_right, m_CalibrationResult1, upRealPoints, upLeftCenter, upRithtCenter, upMatch, 0, 1,2);
			//数据处理，显示
			Mat rotateMat1 = m_CalibrationResult1.amendRotMat;
			m_dealPoints1.clear();
			m_measureDist.deal3DPoints(upRealPoints, m_dealPoints1, rotateMat1, 0);
			m_measureDist.deal2DPoints(m_dealPoints1, 0, up_x, up_y);
			double a1_maxx;
			a1_maxx = 0;
			double n_a1 = 0, n_a2 = 0, n_a2_miny = 0, n_a2_minx=0;
			pointsA.clear(); pointsA1.clear(); pointsA2.clear();
			m_measureDist.measureA1A2_NEW(m_dealPoints1, a1_maxx, n_a1, n_a2_minx, n_a2_miny, n_a2, pointsA1, pointsA2, pointsA);
			a1_n.push_back(n_a1);
			a2_n.push_back(n_a2);
			a2_miny_n.push_back(n_a2_miny);
			a2_minx_n.push_back(n_a2_minx);
			Sleep(5);
		}
		std::sort(a1_n.begin(), a1_n.end());
		a1_maxy = a1_n[0]; //(a1_n[1] + a1_n[2])/2;   //改动：三次改一次
		if (a1_maxy > 465)
		{
			a1_maxy = a1_maxy + (360 - a1_maxy) / 100.0;
		}	
		std::sort(a2_n.begin(), a2_n.end());
		a2 = a2_n[0];//(a2_n[1] + a2_n[2])/2;   //改动：三次改一次
		std::sort(a2_miny_n.begin(), a2_miny_n.end());
		a2_miny = a2_miny_n[0];//(a2_n[1] + a2_n[2])/2;   //改动：三次改一次
		std::sort(a2_minx_n.begin(), a2_minx_n.end());
		a2_minx = a2_minx_n[0];//(a2_n[1] + a2_n[2])/2;   //改动：三次改一次
		// standlevel = atoi(GetPropertyByName("camera", "standlevel").c_str());
		float showTrackDis = 0;
		if (standlevel == 1)
		{
			showTrackDis = (sensorData.trackDisRef + sensorData.trackDis) / 2;
			a2 = a2 + showTrackDis;
		}
		if (standlevel == 2)
		{
			a1_maxy = a1_maxy - A1;
			a2 = a2 - A2;
		}
	}
	catch (...)
	{
		PostMessage(WM_RECVDATA, 0, 2);
		measuredownfinish = false;
		measurefinish = true;
		start_measure = false;
		return;
	}

}
void CMainDialog::OnCommMscomm1()
{
	try
	{
		VARIANT variant_inp;
		COleSafeArray safearray_inp;
		LONG Len, k;
		BYTE rxdata[1024];
		if (m_MSCOMM1.get_CommEvent() == 2)
		{
			variant_inp = m_MSCOMM1.get_Input();
			safearray_inp = variant_inp;
			Len = safearray_inp.GetOneDimSize();
			for (k = 0; k < Len; k++)
			{
				safearray_inp.GetElement(&k, rxdata + k);
			}
			uart_rx_fill(rxdata, Len); //接收串口数据
			//测量功能
			if (start_measure&&measurefinish)
			{
				start_measure = false;
				measurefinish = false;
				if (openInstructionBOOL)
				{
					m_pInstructionDlg->ShowWindow(SW_HIDE);
					openInstructionBOOL = false;
				}
				CTime time;
				CString str;
				time = CTime::GetCurrentTime();
				str.Format(_T("未存储 %s"), time.Format("%H:%M:%S"));
				GetDlgItem(IDC_STATIC_FINISH2)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_STATIC_MESURE2)->ShowWindow(SW_HIDE); 
				GetDlgItem(IDC_STATIC_STATE)->ShowWindow(SW_HIDE);
				m_bar.SetPaneText(1, str, 1);
				//Measure();
				m_pThread->ResumeThread();
				//测量
				int TempSave = atoi(GetPropertyByName("camera", "save").c_str());
				if (TempSave == 1)
				{
					GetDlgItem(IDC_STATIC_MESURE2)->ShowWindow(SW_HIDE);
					GetDlgItem(IDC_STATIC_FINISH2)->ShowWindow(SW_SHOW);
				}
				else
				{
					GetDlgItem(IDC_STATIC_MESURE2)->ShowWindow(SW_SHOW);
					GetDlgItem(IDC_STATIC_FINISH2)->ShowWindow(SW_HIDE);
				}
			}
			//下位机设置sensorData
			else if (trans_sensorData)
			{
				float showTrackDis = sensorData.trackDisRef + sensorData.trackDis;
				sensorData.Lvalue = showTrackDis * float(sensorData.Dvalue - sensorData.Dvalue0) / 6554;
				double tmp_dip = asin(float(sensorData.Dvalue - sensorData.Dvalue0) / 6554);
				sensorData.dip = 180 / M_PI*tmp_dip;

				if (sensorData.dip > 0.0001)
				{
					sensorData.dip = sensorData.dip + 0.1;
				}
				if (sensorData.dip < 0.0001)
				{
					sensorData.dip = sensorData.dip - 0.1;
				}

				//输出两位小数
				char strTmp[20];
				sprintf_s(strTmp, "%.1f", showTrackDis);
				m_SDtrackDis = CString(strTmp);
				sprintf_s(strTmp, "%.1f", sensorData.Lvalue);
				m_SDhorizontal = CString(strTmp);
				m_SDbattery = (to_string(sensorData.battery) + "%").c_str();
				/*GetDlgItem(IDC_SD_dip)->SetWindowTextA(to_string(sensorData.dip).c_str());*/
				m_commStatus = true;
				WriteConfigFile();
				trans_sensorData = false;
			}
			//上机位关机，程序关闭
			else if (power_off)
			{
				::SendMessage(AfxGetMainWnd()->m_hWnd, WM_CLOSE, 0, 0);
				power_off = false;
				/*if (m_reminderDlg == NULL)
				{
					m_reminderDlg = new Creminder();
					m_reminderDlg->Create(IDD_REMINDER_DIALOG, this);
				}
				m_reminderDlg->ShowWindow(SW_SHOW);*/
				ReSetWindows(EWX_SHUTDOWN, true);//关机??
			}
			else if (calibrating)
			{
				//Read_config();
				//m_bar.SetPaneText(0, "校准开始", 1);
				//GetDlgItem(IDC_STATIC_FINISH1)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_STATIC_MESURE1)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_EARE)->ShowWindow(SW_SHOW);
				//if (standlevel == 1)
				//{
				//	float showTrackDis = (sensorData.trackDisRef + sensorData.trackDis) / 2;
				//	a2 = a2 - showTrackDis;
				//	a3 = a3 - showTrackDis;
				//}
				//else if (standlevel == 2)
				//{
				//	a2 = a2;
				//	a2 = a2 + A2;
				//	a3 = A3 + a3;
				//	a1_maxy = a1_maxy;
				//	a1_maxy = a1_maxy + A1;
				//	a4 = A4 + a4;
				//}
				//if (a1_maxy == 0 || a2 == 0 || a3 == 0 || a4 == 0)
				//{
				//	up_x = 135.1; up_y = 686.5; down_x = 124.3; down_y = -46.0;//默认参数
				//}
				//else{
				//	up_x = up_x + A2 - a2;
				//	up_y = up_y + A1 - a1_maxy;
				//	down_x = down_x + A2 - a2_minx;
				//	down_x = down_x + A3 - a3 + 46;
				//	down_y = down_y + A4 - a4;
				//}
				//CString Temp_up_x, Temp_up_y, Temp_down_x, Temp_down_y;
				//Temp_up_x.Format(" %.1f", up_x);
				//Temp_up_y.Format(" %.1f", up_y);
				//Temp_down_x.Format(" %.1f", down_x);
				//Temp_down_y.Format(" %.1f", down_y);
				//SetPropertyByName("RiviseParams", "up_x", Temp_up_x);
				//SetPropertyByName("RiviseParams", "up_y", Temp_up_y);
				//SetPropertyByName("RiviseParams", "down_x", Temp_down_x);
				//SetPropertyByName("RiviseParams", "down_y", Temp_down_y);
				//sensorData.Dvalue0 = sensorData.Dvalue;
				//SetPropertyByName("sensorData", "Dvalue0", to_string(sensorData.Dvalue0).c_str());
				//sensorData.trackDisRef = trackDis_standard - sensorData.trackDis;
	   //         SetPropertyByName("sensorData", "trackDisRef", to_string(sensorData.trackDisRef).c_str());
				//m_bar.SetPaneText(0, "校准完成", 1);
				//Read_config();
				//calibrating = false;

				//CString strtemp, Receive;   //改动
				//for (k = 0; k < Len; k++)
				//{
				//	strtemp.Format(_T("%x"), *(rxdata + k));
				//	Receive += strtemp;
				//}
				//AfxMessageBox(_T(Receive));

				//GetDlgItem(IDC_EARE)->ShowWindow(SW_HIDE);
			}
			else if (save_result&&measurefinish)
			{
			/* GetDlgItem(IDC_STATIC_MESURE2)->ShowWindow(SW_SHOW);
			 GetDlgItem(IDC_STATIC_FINISH2)->ShowWindow(SW_HIDE);
				SaveCameraImgs(m_measPics2);
				savetxt();
				CTime time;
				CString str;
				time = CTime::GetCurrentTime();
				str.Format(_T("已存储 %s"), time.Format("%H:%M:%S"));
				m_bar.SetPaneText(1, str, 1);
				SaveCameraImgs(m_measPics);*/

				/*int TempSave = atoi(GetPropertyByName("camera", "save").c_str());
				if (TempSave == 1)
				{
					TempSave = 0;
				}
				else
				{
					TempSave = 1;
				}
				CString TempSaveString;
				TempSaveString.Format("%d", TempSave);
				SetPropertyByName("camera", "save", TempSaveString);*/

			    m_pThread3->ResumeThread();
				save_result = false;			
			}
		}
		UpdateData(FALSE);

	}
	// 错误信息处理
	catch (LPCTSTR errMsg)
	{
		AfxMessageBox(errMsg);
	}
	catch (cv::Exception& e)
	{
		char err[2048];
		sprintf_s(err, "发生错误: %s", e.what());
		CString errInfo;
		errInfo = err;
		AfxMessageBox(_T(errInfo));
	}
	catch (...)
	{
		AfxMessageBox(_T("发生未知错误！"));
	}

}

void CMainDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO:  在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()

	CBitmap   bitmap;
	bitmap.LoadBitmap(IDB_BACKGROUND);
	CBrush   brush;
	brush.CreatePatternBrush(&bitmap);
	CBrush*   pOldBrush = dc.SelectObject(&brush);
	dc.Rectangle(0, 0, 480, 800);
	dc.SelectObject(pOldBrush);
	//SetWindowPos(&wndTopMost, 0, 0, 480, 800, SWP_SHOWWINDOW);

	ShowRes();
}

void CMainDialog::SetBatteryPic(int battery,int type)
{
	if (type ==1)
	{
		if (battery < 11)
		{
			GetDlgItem(IDC_battery0)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_battery1)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery2)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery3)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery4)->ShowWindow(SW_HIDE);
		}
		else if (battery < 35)
		{
			GetDlgItem(IDC_battery0)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery1)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_battery2)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery3)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery4)->ShowWindow(SW_HIDE);
		}
		else if (battery < 65)
		{
			GetDlgItem(IDC_battery0)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery1)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery2)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_battery3)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery4)->ShowWindow(SW_HIDE);
		}
		else if (battery < 90)
		{
			GetDlgItem(IDC_battery0)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery1)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery2)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery3)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_battery4)->ShowWindow(SW_HIDE);
		}
		else if (battery <= 100)
		{
			GetDlgItem(IDC_battery0)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery1)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery2)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery3)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery4)->ShowWindow(SW_SHOW);
		}
	}
	else if (type == 2)
	{
		if (battery >= 4 && battery <= 7)
		{
			GetDlgItem(IDC_battery0)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_battery1)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery2)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery3)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery4)->ShowWindow(SW_HIDE);
		}
		else if (battery >= 21 && battery <= 24)
		{
			GetDlgItem(IDC_battery0)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery1)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_battery2)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery3)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery4)->ShowWindow(SW_HIDE);
		}
		else if (battery >= 48 && battery <= 52)
		{
			GetDlgItem(IDC_battery0)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery1)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery2)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_battery3)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery4)->ShowWindow(SW_HIDE);
		}
		else if (battery >= 76 && battery <= 79)
		{
			GetDlgItem(IDC_battery0)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery1)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery2)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery3)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_battery4)->ShowWindow(SW_HIDE);
		}
		else if (battery >= 93 && battery <= 97)
		{
			GetDlgItem(IDC_battery0)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery1)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery2)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery3)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_battery4)->ShowWindow(SW_SHOW);
		}

	}

}

void CMainDialog::OnTimer(UINT_PTR nIDEvent)
{
	CTime time;
	int battery;
	switch (nIDEvent)
	{
	case 1:
		time = CTime::GetCurrentTime();
		m_bar.SetPaneText(3, time.Format("%H:%M:%S"), 1);
		break;
	case 2:
		battery = sensorData.battery;
		SetBatteryPic(battery, 2);
	break;
	case 3:
		if (m_commStatus)
		{
			m_bar.SetPaneText(2, "通讯正常",1);
			GetDlgItem(IDC_STATIC_MESURE3)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_STATIC_FINISH3)->ShowWindow(SW_HIDE);
			m_commStatus = false;
		}
		else
		{
			GetDlgItem(IDC_STATIC_FINISH3)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_STATIC_MESURE3)->ShowWindow(SW_HIDE);
			m_bar.SetPaneText(2, "通讯异常", 1);
		}
		break;
	}
	CDialogEx::OnTimer(nIDEvent);

}


HBRUSH CMainDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性

	//if (pWnd == this)
	//{
	//	return m_bkBrush; //m_bkBrush
	//}

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_A1)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(33, 148, 180));
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_A2)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(33, 148, 180));
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_A3)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(33, 148, 180));
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_A4)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(33, 148, 180));
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_S1)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(33, 148, 180));
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_S2)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(33, 148, 180));
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	if (pWnd->GetDlgCtrlID() == IDC_STATIC1)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(255, 255, 255));
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	if (pWnd->GetDlgCtrlID() == IDC_STATIC2)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(255, 255, 255));
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	if (pWnd->GetDlgCtrlID() == IDC_STATIC3)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(255, 255, 255));
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	if (pWnd->GetDlgCtrlID() == IDC_STATIC4)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(255, 255, 255));
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	if (pWnd->GetDlgCtrlID() == IDC_STATIC5)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(255, 255, 255));
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	if (pWnd->GetDlgCtrlID() == IDC_STATIC6)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(255, 255, 255));
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}

	if (pWnd->GetDlgCtrlID() == IDC_VALUE_A1_M)
	{
		/*pDC->SetBkMode(TRANSPARENT);*/
		pDC->SetTextColor(RGB(255, 255, 255));
		pDC->SetBkColor(RGB(48, 48, 48));
		/*return (HBRUSH)GetStockObject(NULL_BRUSH);*/
	}
	if (pWnd->GetDlgCtrlID() == IDC_VALUE_A2_M)
	{
		/*pDC->SetBkMode(TRANSPARENT);*/
		pDC->SetTextColor(RGB(255, 255, 255));
		pDC->SetBkColor(RGB(48, 48, 48));
		/*return (HBRUSH)GetStockObject(NULL_BRUSH);*/
	}
	if (pWnd->GetDlgCtrlID() == IDC_VALUE_A3_M)
	{
		/*pDC->SetBkMode(TRANSPARENT);*/
		pDC->SetTextColor(RGB(255, 255, 255));
		pDC->SetBkColor(RGB(48, 48, 48));
		/*return (HBRUSH)GetStockObject(NULL_BRUSH);*/
	}
	if (pWnd->GetDlgCtrlID() == IDC_VALUE_A4_M)
	{
		/*pDC->SetBkMode(TRANSPARENT);*/
		pDC->SetTextColor(RGB(255, 255, 255));
		pDC->SetBkColor(RGB(48, 48, 48));
		/*return (HBRUSH)GetStockObject(NULL_BRUSH);*/
	}
	if (pWnd->GetDlgCtrlID() == IDC_SD_trackDis)
	{
		pDC->SetTextColor(RGB(255, 255, 255));
		pDC->SetBkColor(RGB(48, 48, 48));
		/*pDC->SetBkMode(TRANSPARENT);*/
		/*return (HBRUSH)GetStockObject(NULL_BRUSH);*/
	}
	if (pWnd->GetDlgCtrlID() == IDC_SD_horizontal)
	{
		pDC->SetTextColor(RGB(255, 255, 255));
		pDC->SetBkColor(RGB(48, 48, 48));
		/*pDC->SetBkMode(TRANSPARENT);*/
		/*return (HBRUSH)GetStockObject(NULL_BRUSH);*/
	}
	if (pWnd->GetDlgCtrlID() == IDC_ON1TEST)
	{
		pDC->SetTextColor(RGB(255, 255, 255));
		pDC->SetBkColor(RGB(69, 69, 69));
		//pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	if (pWnd->GetDlgCtrlID() == IDC_OFF1TEST)
	{
		pDC->SetTextColor(RGB(255, 255, 255));
		pDC->SetBkColor(RGB(69, 69, 69));
		//pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	if (pWnd->GetDlgCtrlID() == IDC_TIME)
	{
		pDC->SetTextColor(RGB(255, 255, 255));
		pDC->SetBkColor(RGB(69, 69, 69));
		//pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_DEBUG)
	{
		pDC->SetTextColor(RGB(255, 255, 255));
		pDC->SetBkColor(RGB(69, 69, 69));
		//pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
void CMainDialog::MeanCameraImgs(const vector<Mat>& inImgs, Mat& outImg)
{
	if (!inImgs.empty())
	{
		int len = inImgs.size();
		outImg = Mat::zeros(inImgs[0].size(), CV_32FC3);
		for (int i = 0; i < len; i++)
		{
			accumulate(inImgs[i], outImg);
		}
		outImg /= len;
		outImg.convertTo(outImg, CV_8UC3);
	}
}

void CMainDialog::SaveCameraImgs(const vector<Mat>& imgs)
{
	if (imgs.empty())
		return;
	/*int TempSave = atoi(GetPropertyByName("camera", "save").c_str());
	if (TempSave == 1)*/
	if (m_save)
	{
		CTime t = CTime::GetCurrentTime();
		CString data = t.Format("%Y-%m-%d");
		CString time = t.Format("%H-%M-%S");
		m_savePlace = CreatePicFolder(data, time);

		CString filename;
		map<string, Mat > savePicsMap;
		for (int i = 0; i < imgs.size(); i++)
		{

			if (i == 0)
				//filename.Format("%s\\UpOff", m_savePlace);
				filename.Format("%s\\1Off", m_savePlace);
			else if (i == 1)
				filename.Format("%s\\2", m_savePlace);
			else if (i == 2)
				filename.Format("%s\\3Off", m_savePlace);
			else if (i == 3)
				filename.Format("%s\\4", m_savePlace);
			else if (i == 4)
				filename.Format("%s\\5Off", m_savePlace);
			else if (i == 5)
				filename.Format("%s\\6", m_savePlace);
			/*else if (i == 6)
				filename.Format("%s\\7Off", m_savePlace);
			else if (i == 7)
				filename.Format("%s\\8", m_savePlace);*/
			CString fn;
			fn.Format("%s_L.bmp", filename);
			savePicsMap.insert(make_pair(fn.GetBuffer(0), imgs[i]));

			SaveMeasPics(savePicsMap);
		}
	}
}
void CMainDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	if (nIDCtl == IDC_BN_DEBUG)
	{
		CDC dc;
		RECT rect;
		dc.Attach(lpDrawItemStruct->hDC);

		rect = lpDrawItemStruct->rcItem;

		dc.Draw3dRect(&rect, RGB(48, 48, 48), RGB(48, 48, 48));

		dc.FillSolidRect(&rect, RGB(48, 48, 48));

		dc.SetBkColor(RGB(48, 48, 48));
		dc.SetTextColor(RGB(48, 48, 48));

		TCHAR buffer[MAX_PATH];
		::GetWindowText(lpDrawItemStruct->hwndItem, buffer, MAX_PATH);

		dc.DrawText(buffer, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		dc.Detach();
	}

	if (nIDCtl == IDC_TEST)
	{
		CDC dc;
		RECT rect;
		dc.Attach(lpDrawItemStruct->hDC);

		rect = lpDrawItemStruct->rcItem;

		dc.Draw3dRect(&rect, RGB(48, 48, 48), RGB(48, 48, 48));

		dc.FillSolidRect(&rect, RGB(48, 48, 48));

		dc.SetBkColor(RGB(48, 48, 48));
		dc.SetTextColor(RGB(48, 48, 48));

		TCHAR buffer[MAX_PATH];
		::GetWindowText(lpDrawItemStruct->hwndItem, buffer, MAX_PATH);

		dc.DrawText(buffer, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		dc.Detach();
	}

	if (nIDCtl == IDC_INSTRUCTION)
	{
		CDC dc;
		RECT rect;
		dc.Attach(lpDrawItemStruct->hDC);

		rect = lpDrawItemStruct->rcItem;

		dc.Draw3dRect(&rect, RGB(48, 48, 48), RGB(48, 48, 48));

		dc.FillSolidRect(&rect, RGB(48, 48, 48));

		dc.SetBkColor(RGB(48, 48, 48));
		dc.SetTextColor(RGB(48, 48, 48));

		TCHAR buffer[MAX_PATH];
		::GetWindowText(lpDrawItemStruct->hwndItem, buffer, MAX_PATH);

		dc.DrawText(buffer, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		dc.Detach();
	}
	if (nIDCtl == IDC_BNCaculateMain)
	{
		CDC dc;
		RECT rect;
		dc.Attach(lpDrawItemStruct->hDC);

		rect = lpDrawItemStruct->rcItem;

		dc.Draw3dRect(&rect, RGB(69, 69, 69), RGB(69, 69, 69));

		dc.FillSolidRect(&rect, RGB(69, 69, 69));

		dc.SetBkColor(RGB(69, 69, 69));
		dc.SetTextColor(RGB(69, 69, 69));

		TCHAR buffer[MAX_PATH];
		::GetWindowText(lpDrawItemStruct->hwndItem, buffer, MAX_PATH);

		dc.DrawText(buffer, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		dc.Detach();
	}	
	CDialogEx::OnDrawItem(nIDCtl, lpDrawItemStruct);
}


void CMainDialog::OnEnChangeA1hh()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}
void CMainDialog::DrawPic(CDC *pDC, CRect &rectPicture, vector<Point2d>Points1, vector<Point2d>Points2, double xmin, double xmax, double ymin, double ymax, float showTrackDis)
{
	//int showstapic = atoi(GetPropertyByName("camera", "show static picture"));

	if (showstapic == 1)
	{
		//确定图象显示大小
		int width = rectPicture.Width();
		int height = rectPicture.Height();

		if (m_dealPoints1.empty() && m_dealPoints2.empty())
			return;

		pDC->FillSolidRect(&rectPicture, RGB(69, 69, 69));
		CPen pen(PS_SOLID, 2, RGB(255, 255, 255));
		CPen yls(PS_SOLID, 5, RGB(255, 0, 0));

		minX = xmin - m_margin;
		maxX = xmax + m_margin;
		minY = ymin - m_margin;
		maxY = ymax + m_margin;
		/*minX = 670 - m_margin;
		maxX = 850 + m_margin;
		minY = 180 - m_margin;
		maxY =390 + m_margin;*/

		//确定X，Y轴每单位显示宽度 
		intervalX = (width - myleft - myright) / (maxX - minX);
		intervalY = (height - mybottom - mytop) / (maxY - minY);
		//绘制X，Y轴
		//X轴从图形区域最左端到最右端
		pDC->SelectObject(&pen);
		pDC->MoveTo(int(myleft), int(height - mybottom));
		pDC->LineTo(int(width - myright), int(height - mybottom));
		//Y轴从图形区域最底端到最顶端
		pDC->SelectObject(&pen);
		pDC->MoveTo(int(myleft), int(height - mybottom));
		pDC->LineTo(int(myleft), int(mytop));

		//确定显示刻度个数
		const int count = 5;
		//确定每个显示刻度之间的宽度
		float spaceX = (width - myleft - myright) / count;
		float spaceY = (height - mybottom - mytop) / count;
		//绘制刻度和刻度值
		CString str;
		//X轴
		for (int i = 0; i <= count; i++)
		{
			str.Format("%.0f", showTrackDis + minX + i*(maxX - minX) / count);
			pDC->MoveTo(int(myleft + spaceX*i), int(height - mybottom));
			pDC->LineTo(int(myleft + spaceX*i), int(height - (mybottom + 5)));
			pDC->SetTextColor(RGB(255, 255, 255));
			pDC->TextOut(int(myleft + spaceX*i - 10), int(height - (mybottom - 5)), str);
		}
		//Y轴
		for (int i = 0; i <= count; i++)
		{
			str.Format("%.0f", minY + i*(maxY - minY) / count);
			pDC->MoveTo(int(myleft), int(height - (mybottom + spaceY*i)));
			pDC->LineTo(int(myleft + 5), int(height - (mybottom + spaceY*i)));
			pDC->SetTextColor(RGB(255, 255, 255));
			pDC->TextOut(int(myleft - 50), int(height - (mybottom + spaceY*i + 8)), str);
		}

		//绘制数据点
		if (!Points1.empty()) //判断数据点是否为空或者测量
		{
			int x = myleft + (Points1[0].x - minX)*intervalX;
			int y = height - (mybottom + (Points1[0].y - minY)*intervalY);
			pDC->MoveTo(x, y);
			for (int i = 0; i != Points1.size(); i++)
			{
				Point2f p = Points1[i];
				int x = myleft + (p.x - minX)*intervalX;
				int y = height - (mybottom + (p.y - minY)*intervalY);
				pDC->SelectObject(&yls);
				pDC->LineTo(x, y);
				/*pDC->MoveTo(x, y);
				pDC->SetPixel(x, y, RGB(255, 255, 255));*/
			}
		}
		if (Points2.empty())
			return;
		int x = myleft + (Points2[0].x - minX)*intervalX;
		int y = height - (mybottom + (Points2[0].y - minY)*intervalY);
		pDC->MoveTo(x, y);
		for (int i = 0; i != Points2.size(); i++)
		{
			Point2f p = Points2[i];
			int x = myleft + (p.x - minX)*intervalX;
			int y = height - (mybottom + (p.y - minY)*intervalY);
			pDC->SelectObject(&yls);
			pDC->LineTo(x, y);
			 /* pDC->MoveTo(x, y);
			  pDC->SetPixel(x,y,RGB(255,255,255));*/
		}

		if (showTrackDis == 0)
		{
			//绘制X，Y轴的变量名
			pDC->TextOut(width / 2 - 82, height - 20, "距轨道内侧面的距离[mm]");
			pDC->TextOut(30, height / 2 - 70, "距");
			pDC->TextOut(30, height / 2 - 50, "轨");
			pDC->TextOut(30, height / 2 - 30, "道");
			pDC->TextOut(30, height / 2 - 10, "面");
			pDC->TextOut(30, height / 2 + 10, "的");
			pDC->TextOut(30, height / 2 + 30, "高");
			pDC->TextOut(30, height / 2 + 50, "度");
			pDC->TextOut(20, height / 2 + 70, "[mm]");
			pDC->SetTextColor(RGB(255, 255, 255));
		}
		else
		{
			//绘制X，Y轴的变量名
			pDC->TextOut(width / 2 - 80, height - 20, "距轨道中心的距离[mm]");
			pDC->TextOut(30, height / 2 - 70, "距");
			pDC->TextOut(30, height / 2 - 50, "轨");
			pDC->TextOut(30, height / 2 - 30, "道");
			pDC->TextOut(30, height / 2 - 10, "面");
			pDC->TextOut(30, height / 2 + 10, "的");
			pDC->TextOut(30, height / 2 + 30, "高");
			pDC->TextOut(30, height / 2 + 50, "度");
			pDC->TextOut(20, height / 2 + 70, "[mm]");
			pDC->SetTextColor(RGB(255, 255, 255));
		}
	}

}

vector<CStringA> CMainDialog::DoSelectFiles(
	LPCTSTR	lpszDefExt,
	DWORD	dwFlags,
	LPCTSTR	lpszFilter,
	LPCTSTR	lpstrTitle,
	LPCTSTR	lpstrInitialDir)
{
	vector<CStringA> selectedFiles;
	POSITION filePosition;
	DWORD MAXFILE = 4000;
	TCHAR* pc = new TCHAR[MAXFILE];

	CFileDialog dlg(TRUE, lpszDefExt, NULL, dwFlags, lpszFilter, NULL);

	dlg.m_ofn.nMaxFile = MAXFILE;
	dlg.m_ofn.lpstrFile = pc;
	dlg.m_ofn.lpstrFile[0] = NULL;
	dlg.m_ofn.lpstrTitle = lpstrTitle;
	dlg.m_ofn.lpstrInitialDir = lpstrInitialDir;

	if (dlg.DoModal() == IDOK)
	{
		filePosition = dlg.GetStartPosition();
		while (filePosition != NULL)
		{
			CStringA path;
			path = dlg.GetNextPathName(filePosition);
			selectedFiles.push_back(path);
		}
	}

	delete[]pc;
	return selectedFiles;
}
void CMainDialog::InitCalibParams(CameraCalib::CalibrationParames& cameraCalibParams, int whichNum)
{
	//读取标定数据;
	cameraCalibParams.sCaliResultFilePath.Format(_T("%s\\CalibrationResult"), m_workDir);
	bool bCaliSucflag = cameraCalibParams.ReadYMLFile(whichNum);
}
void CMainDialog::OnBnClickedTest()
{
	//if (openInstructionBOOL)
	//{
	//	m_pInstructionDlg->ShowWindow(SW_HIDE);
	//	openInstructionBOOL = false;
	//}
	//// TODO:  在此添加控件通知处理程序代码
	//vector<CStringA> imgFiles, imgFiles2;
	//m_measPics.clear();
	//m_measPics2.clear();
	//const char* img0_filename = NULL, *img1_filename = NULL, *img2_filename = NULL, *img3_filename = NULL,
	//	*img4_filename = NULL, *img5_filename = NULL, *img6_filename = NULL, *img7_filename = NULL;
	//imgFiles = DoSelectFiles(
	//	_T("*.bmp"),
	//	OFN_ENABLESIZING | OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY,
	//	_T("image files (*.bmp; *.png; *.jpg) |*.bmp; *.png; *.jpg; *.jpeg| All Files (*.*) |*.*||"),
	//	_T("选择图像文件"),
	//	_T("Imgs\\Left"));

	//if (imgFiles.empty())	// 判断输入图片
	//{
	//	AfxMessageBox(_T("请选择图像！"));
	//	return;
	//}
	//CTime time;
	//CString str;
	//time = CTime::GetCurrentTime();
	//str.Format(_T("未存储 %s"), time.Format("%H:%M:%S"));
	//GetDlgItem(IDC_STATIC_FINISH2)->ShowWindow(SW_SHOW);
	//GetDlgItem(IDC_STATIC_MESURE2)->ShowWindow(SW_HIDE);
	//GetDlgItem(IDC_STATIC_FINISH1)->ShowWindow(SW_SHOW);
	//GetDlgItem(IDC_STATIC_MESURE1)->ShowWindow(SW_HIDE);
	//GetDlgItem(IDC_STATIC_STATE)->ShowWindow(SW_HIDE);
	//m_bar.SetPaneText(1, str, 1);
	//m_bar.SetPaneText(0, "测试中...", 1);
	//img0_filename = imgFiles[0];
	//img1_filename = imgFiles[1];
	//img2_filename = imgFiles[2];
	//img3_filename = imgFiles[3];
	//img4_filename = imgFiles[4];
	//img5_filename = imgFiles[5];
	//Mat downImg = imread(img0_filename, 1);
	//Mat downLayerImg = imread(img1_filename, 1);
	//Mat downImg2 = imread(img2_filename, 1);
	//Mat downLayerImg2 = imread(img3_filename, 1);
	//Mat downImg3 = imread(img4_filename, 1);
	//Mat downLayerImg3 = imread(img5_filename, 1);
	//m_measPics.push_back(downImg);
	//m_measPics.push_back(downLayerImg);
	//m_measPics.push_back(downImg2);
	//m_measPics.push_back(downLayerImg2);
	//m_measPics.push_back(downImg3);
	//m_measPics.push_back(downLayerImg3);


	//imgFiles2 = DoSelectFiles(
	//	_T("*.bmp"),
	//	OFN_ENABLESIZING | OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY,
	//	_T("image files (*.bmp; *.png; *.jpg) |*.bmp; *.png; *.jpg; *.jpeg| All Files (*.*) |*.*||"),
	//	_T("选择图像文件"),
	//	_T("Imgs\\Left"));

	//if (imgFiles2.empty())	// 判断输入图片
	//{
	//	AfxMessageBox(_T("请选择图像！"));
	//	return;
	//}
	//img0_filename = imgFiles2[0];
	//img1_filename = imgFiles2[1];
	//img2_filename = imgFiles2[2];
	//img3_filename = imgFiles2[3];
	//img4_filename = imgFiles2[4];
	//img5_filename = imgFiles2[5];
	//Mat upImg = imread(img0_filename, 1);
	//Mat upLayerImg = imread(img1_filename, 1);
	//Mat upImg2 = imread(img2_filename, 1);
	//Mat upLayerImg2 = imread(img3_filename, 1);
	//Mat upImg3 = imread(img4_filename, 1);
	//Mat upLayerImg3 = imread(img5_filename, 1);
	//m_measPics2.push_back(upImg);
	//m_measPics2.push_back(upLayerImg);
	//m_measPics2.push_back(upImg2);
	//m_measPics2.push_back(upLayerImg2);
	//m_measPics2.push_back(upImg3);
	//m_measPics2.push_back(upLayerImg3);
	//m_pThread2->ResumeThread();
	//m_pThread1->ResumeThread();
	if (openInstructionBOOL)   //改动：三次改一次
	{
		m_pInstructionDlg->ShowWindow(SW_HIDE);
		openInstructionBOOL = false;
	}
	// TODO:  在此添加控件通知处理程序代码
	vector<CStringA> imgFiles, imgFiles2;
	m_measPics.clear();
	m_measPics2.clear();
	const char* img0_filename = NULL, *img1_filename = NULL, *img2_filename = NULL, *img3_filename = NULL,
		*img4_filename = NULL, *img5_filename = NULL, *img6_filename = NULL, *img7_filename = NULL;
	imgFiles = DoSelectFiles(
		_T("*.bmp"),
		OFN_ENABLESIZING | OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY,
		_T("image files (*.bmp; *.png; *.jpg) |*.bmp; *.png; *.jpg; *.jpeg| All Files (*.*) |*.*||"),
		_T("选择图像文件"),
		_T("Imgs\\Left"));

	if (imgFiles.empty())	// 判断输入图片
	{
		AfxMessageBox(_T("请选择图像！"));
		return;
	}
	CString str_first(_T("-"));
	s_a1 = str_first;
	s_a3 = str_first;
	s_a2 = str_first;
	s_a4 = str_first;
	GetDlgItem(IDC_VALUE_A1_M)->SetWindowTextA(s_a1);
	GetDlgItem(IDC_VALUE_A2_M)->SetWindowTextA(s_a2);
	GetDlgItem(IDC_VALUE_A3_M)->SetWindowTextA(s_a3);
	GetDlgItem(IDC_VALUE_A4_M)->SetWindowTextA(s_a4);
	//CTime time;
	//CString str;
	//time = CTime::GetCurrentTime();
	//str.Format(_T("未存储 %s"), time.Format("%H:%M:%S"));
	//GetDlgItem(IDC_STATIC_FINISH2)->ShowWindow(SW_SHOW);
	//GetDlgItem(IDC_STATIC_MESURE2)->ShowWindow(SW_HIDE);
	//GetDlgItem(IDC_STATIC_FINISH1)->ShowWindow(SW_SHOW);
	//GetDlgItem(IDC_STATIC_MESURE1)->ShowWindow(SW_HIDE);
	//GetDlgItem(IDC_STATIC_STATE)->ShowWindow(SW_HIDE);
	//m_bar.SetPaneText(1, str, 1);
	//m_bar.SetPaneText(0, "测试中...", 1);
	img0_filename = imgFiles[0];
	img1_filename = imgFiles[1];
	img2_filename = imgFiles[2];
	img3_filename = imgFiles[3];
	/*img4_filename = imgFiles[4];
	img5_filename = imgFiles[5];*/
	Mat downImg = imread(img0_filename, 1);
	Mat downLayerImg = imread(img1_filename, 1);
	Mat downImg2 = imread(img2_filename, 1);
	Mat downLayerImg2 = imread(img3_filename, 1);
	/*Mat downImg3 = imread(img4_filename, 1);
	Mat downLayerImg3 = imread(img5_filename, 1);*/
	m_measPics.push_back(downImg);
	m_measPics2.push_back(downLayerImg);
	m_measPics.push_back(downImg2);
	m_measPics2.push_back(downLayerImg2);
	/*m_measPics.push_back(downImg3);
	m_measPics.push_back(downLayerImg3);*/


	//imgFiles2 = DoSelectFiles(
	//	_T("*.bmp"),
	//	OFN_ENABLESIZING | OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY,
	//	_T("image files (*.bmp; *.png; *.jpg) |*.bmp; *.png; *.jpg; *.jpeg| All Files (*.*) |*.*||"),
	//	_T("选择图像文件"),
	//	_T("Imgs\\Left"));

	//if (imgFiles2.empty())	// 判断输入图片
	//{
	//	AfxMessageBox(_T("请选择图像！"));
	//	return;
	//}
	//img0_filename = imgFiles2[0];
	//img1_filename = imgFiles2[1];
	//img2_filename = imgFiles2[2];
	//img3_filename = imgFiles2[3];
	//img4_filename = imgFiles2[4];
	//img5_filename = imgFiles2[5];
	//Mat upImg = imread(img0_filename, 1);
	//Mat upLayerImg = imread(img1_filename, 1);
	//Mat upImg2 = imread(img2_filename, 1);
	//Mat upLayerImg2 = imread(img3_filename, 1);
	//Mat upImg3 = imread(img4_filename, 1);
	//Mat upLayerImg3 = imread(img5_filename, 1);
	//m_measPics2.push_back(upImg);
	//m_measPics2.push_back(upLayerImg);
	//m_measPics2.push_back(upImg2);
	//m_measPics2.push_back(upLayerImg2);
	//m_measPics2.push_back(upImg3);
	//m_measPics2.push_back(upLayerImg3);
	m_pThread2->ResumeThread();
	m_pThread1->ResumeThread();
	//m_pThread3->ResumeThread();
}
void CMainDialog::OnBnClickedBnDebug()
{
	//调试界面初始化
	if (NULL == m_stereDlg)
	{
		m_stereDlg = new CStereoCameraDlg();
		m_stereDlg->Create(IDD_STEREOCAMERA_DIALOG, this);
	}
	m_stereDlg->ShowWindow(SW_SHOW);
}
void CMainDialog::OnBnClickedBncaculatemain()
{   //离线测试按钮
	try
	{
		if (openInstructionBOOL)
		{
			m_pInstructionDlg->ShowWindow(SW_HIDE);
			openInstructionBOOL = false;
		}
		if (measurefinish)
		{
			m_pThread->ResumeThread();
			measurefinish = false;
		}

	}
	// 错误信息处理
	catch (...)
	{
		PostMessage(WM_RECVDATA, 0, 2);
		measuredownfinish = false;
		measurefinish = true;
		start_measure = false;
		return;
	}
}

//void CMainDialog::OnBnClickedInstruction()
//{
//	Read_config();
//	// TODO:  在此添加控件通知处理程序代码
//	m_bar.SetPaneText(0, "开始", 1);
//	GetDlgItem(IDC_STATIC_FINISH1)->ShowWindow(SW_HIDE);
//	GetDlgItem(IDC_STATIC_MESURE1)->ShowWindow(SW_HIDE);
//	GetDlgItem(IDC_EARE)->ShowWindow(SW_SHOW);
//	if (standlevel == 1)
//	{
//		float showTrackDis = (sensorData.trackDisRef + sensorData.trackDis) / 2;
//		a2 = a2 - showTrackDis;
//		a3 = a3 - showTrackDis;
//	}
//	else if (standlevel == 2)
//	{
//		a2 = A2 + a2;
//		a3 = A3 + a3;
//		a1_maxy = A1 + a1_maxy;
//		a4 = A4 + a4;
//	}
//	if (a1_maxy == 0 || a2 == 0 || a3 == 0 || a4 == 0)
//	{
//		up_x = 135.1; up_y = 686.5; down_x = 122.3; down_y = -44.0;//默认参数
//
//	}
//	else{
//		up_x = up_x + A2 - a2;
//		up_y = up_y + A1 - a1_maxy;
//		down_x = down_x + A2 - a2_minx;
//		//down_x = down_x + A3 - a3;
//		down_y = down_y + A4 - a4;
//	}
//	CString Temp_up_x, Temp_up_y, Temp_down_x, Temp_down_y;
//	Temp_up_x.Format(" %.1f", up_x);
//	Temp_up_y.Format(" %.1f", up_y);
//	Temp_down_x.Format(" %.1f", down_x);
//	Temp_down_y.Format(" %.1f", down_y);
//	SetPropertyByName("RiviseParams", "up_x", Temp_up_x);
//	SetPropertyByName("RiviseParams", "up_y", Temp_up_y);
//	SetPropertyByName("RiviseParams", "down_x", Temp_down_x);
//	SetPropertyByName("RiviseParams", "down_y", Temp_down_y);
//	sensorData.Dvalue0 = sensorData.Dvalue;
//	SetPropertyByName("sensorData", "Dvalue0", to_string(sensorData.Dvalue0).c_str());
//	sensorData.trackDisRef = trackDis_standard - sensorData.trackDis;
//	SetPropertyByName("sensorData", "trackDisRef", to_string(sensorData.trackDisRef).c_str());
//	calibrating = false;
//	m_bar.SetPaneText(0, "修正完成", 1);
//	//GetDlgItem(IDC_EARE)->ShowWindow(SW_HIDE);
//	Read_config();
//	/*if (m_pInstructionDlg == NULL)
//	{
//		m_pInstructionDlg = new CInstructionDlg();
//		m_pInstructionDlg->Create(IDD_INSTRUCTION_DIALOG, this);	
//	}
//	m_pInstructionDlg->ShowWindow(SW_SHOW);
//	openInstructionBOOL = true;*/
//}

//void CMainDialog::SaveCameraImgs(const vector<Mat>& imgs)
//{
//	if (imgs.empty())
//		return;
//	if (m_save)
//	{
//		CTime t = CTime::GetCurrentTime();
//		CString data = t.Format("%Y-%m-%d");
//		CString time = t.Format("%H-%M-%S");
//		m_savePlace = CreatePicFolder(data, time);
//
//		CString filename;
//		map<string, Mat> savePicsMap;
//		for (int i = 0; i <imgs.size(); i++)
//		{
//			//切分左右视图
//			Size2i img_size(imgs[i].cols / 2, imgs[i].rows);
//			Mat img_left = imgs[i](Rect(0, 0, img_size.width, img_size.height));
//			Mat img_right = imgs[i](Rect(img_size.width, 0, img_size.width, img_size.height));
//
//			if (i == 0)
//				filename.Format("%s\\UpOff", m_savePlace);
//			else if (i == 1)
//				filename.Format("%s\\Up", m_savePlace);
//			else if (i == 2)
//				filename.Format("%s\\DownOff", m_savePlace);
//			else if (i == 3)
//				filename.Format("%s\\Down", m_savePlace);
//			CString fn;
//			fn.Format("%s_L.bmp", filename);
//			savePicsMap.insert(make_pair(fn.GetBuffer(0), img_left));
//			fn.Format("%s_R.bmp", filename);
//			savePicsMap.insert(make_pair(fn.GetBuffer(0), img_right));
//		}
//
//		SaveMeasPics(savePicsMap);
//	}
//}