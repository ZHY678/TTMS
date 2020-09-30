
// StereoCameraDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "StereoCamera.h"
#include "StereoCameraDlg.h"
#include "afxdialogex.h"
#include "CameraCalib.h"





// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CStereoCameraDlg 对话框

CStereoCameraDlg::CStereoCameraDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CStereoCameraDlg::IDD, pParent)
	, m_nBoardWidth(0)
	, m_nBoardHeight(0)
	, m_nPixelSize(0)
	, m_CameraSelect(_T(""))
	, m_ZmeasRange1(300)
	, m_ZmeasRange2(1000)
	, m_XiuVer(0)
	, m_XiuHor(0)
{


	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CStereoCameraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_BoardWidth, m_nBoardWidth);
	DDX_Text(pDX, IDC_BoardHeight, m_nBoardHeight);
	DDX_Text(pDX, IDC_PixelSize, m_nPixelSize);
	DDX_Text(pDX, IDC_SquareSize, m_nSquareSize);
	DDX_Control(pDX, IDC_PicLfCam, m_PicLfArea);
	DDX_Control(pDX, IDC_PicRtCam, m_PicRtArea);
	DDX_Control(pDX, IDC_LIST_STATUS, m_RunStaInfo);
	DDX_CBString(pDX, IDC_CameraSelect, m_CameraSelect);
	DDX_Control(pDX, IDC_CameraSelect, m_CameraSelectCombox);
	DDX_Control(pDX, IDC_PicDraw, m_picDraw);
	DDX_Text(pDX, IDC_MEAS_RANGE1, m_ZmeasRange1);
	DDX_Text(pDX, IDC_MEAS_RANGE2, m_ZmeasRange2);
	DDX_Text(pDX, IDC_XIU_VER, m_XiuVer);
	DDX_Text(pDX, IDC_XIU_HOR, m_XiuHor);
}

BEGIN_MESSAGE_MAP(CStereoCameraDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BN2StereoCalib, &CStereoCameraDlg::OnBnClickedBn2stereocalib)
	ON_BN_CLICKED(IDC_BNSetLfCam, &CStereoCameraDlg::OnBnClickedBnsetlfcam)
	ON_BN_CLICKED(IDC_BNSetRtCam, &CStereoCameraDlg::OnBnClickedBnsetrtcam)
	ON_BN_CLICKED(IDC_BNRunCam, &CStereoCameraDlg::OnBnClickedBnruncam)
	ON_BN_CLICKED(IDC_BN_DefaultCamCalibParam, &CStereoCameraDlg::OnBnClickedBnDefaultcamcalibparam)
	ON_BN_CLICKED(IDC_BNGrabBothCam, &CStereoCameraDlg::OnBnClickedBngrabbothcam)
	ON_BN_CLICKED(IDC_BNSaveCalibParams, &CStereoCameraDlg::OnBnClickedBnsavecalibparams)
	ON_BN_CLICKED(IDC_BNXiuZheng, &CStereoCameraDlg::OnBnClickedBnxiuzheng)
	ON_BN_CLICKED(IDC_BNReadFromMatlab, &CStereoCameraDlg::OnBnClickedBnreadfrommatlab)
	ON_CBN_SELCHANGE(IDC_CameraSelect, &CStereoCameraDlg::OnCbnSelchangeCameraselect)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BNCalculate, &CStereoCameraDlg::OnBnClickedBncalculate)
	ON_BN_CLICKED(IDC_BTRUNMIND, &CStereoCameraDlg::OnBnClickedBtrunmind)
	ON_BN_CLICKED(IDC_MeasureA1A2A3A4, &CStereoCameraDlg::OnBnClickedMeasurea1a2a3a4)
	ON_BN_CLICKED(IDC_BNRUNUPDOWN, &CStereoCameraDlg::OnBnClickedBnrunupdown)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BNSAVE_CONFIG, &CStereoCameraDlg::OnBnClickedBnsaveConfig)
	ON_BN_CLICKED(IDC_OpenLayerPic, &CStereoCameraDlg::OnBnClickedOpenlayerpic)
	ON_BN_CLICKED(IDC_CloseLayerPic, &CStereoCameraDlg::OnBnClickedCloselayerpic)
	ON_BN_CLICKED(IDC_BNAdjust, &CStereoCameraDlg::OnBnClickedBnadjust)
ON_BN_CLICKED(IDC_BN_RevSet, &CStereoCameraDlg::OnBnClickedBnRevset)
ON_BN_CLICKED(IDC_BUTTON1, &CStereoCameraDlg::OnBnClickedButton1)
ON_BN_CLICKED(IDC_BUTTON2, &CStereoCameraDlg::OnBnClickedButton2)
END_MESSAGE_MAP()

// CStereoCameraDlg 消息处理程序

BOOL CStereoCameraDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	bool bool_MIND = false;


	if (!bool_MIND) //mindvision初始化失败，进行UVC初始化
	{
		m_camType = Camera::CAM_TYPE::UVC;
		CheckRadioButton(IDC_RAD_CalcFromCam, IDC_RAD_CalcFromImg, IDC_RAD_CalcFromCam);
		GetDlgItem(IDC_BNRunCam)->EnableWindow(TRUE); 
		bool bool_UVC = Camera::CameraInit(m_camType);
		if (!bool_UVC)
		{
			CheckRadioButton(IDC_RAD_CalcFromCam, IDC_RAD_CalcFromImg, IDC_RAD_CalcFromImg);
			GetDlgItem(IDC_BNRunCam)->EnableWindow(FALSE);
			//AfxMessageBox(_T("No UVC Camera was found!"));
		}

		//获取所有摄像头的名称
		if (Camera::cam_count > 0)
		{
			for (int i = 0; i != Camera::camDevices.size(); i++)
			{
				string camera_name = Camera::camDevices[i];
				m_CameraSelectCombox.InsertString(i, _T(camera_name.c_str()));
			}
			m_CameraSelectCombox.SetCurSel(0);
		}
	}
	
	CheckRadioButton(IDC_RADA1A2, IDC_RADBOTH, IDC_RADBOTH);
	CheckRadioButton(IDC_RADCAMUP, IDC_RADCAMDOWN, IDC_RADCAMUP);

	//获取工作路径
	m_workDir = GetAppExecPath();
	//初始化标定参数
	InitCalibParams(m_CalibrationResult1, 1);
	InitCalibParams(m_CalibrationResult2, 2);
	//三角测量测试
	//m_SignalCalibrationResult.sCaliResultFilePath.Format(_T("%s\\CalibrationResult"), m_workDir);
	//m_SignalCalibrationResult.bIsBinocularCameraCalibration = 0;
	//m_SignalCalibrationResult.ReadYMLFile();

	if (Camera::cam_count == 1 && m_cameraSupport1.CameraIsOpen())
	{
		SetTimer(1,50,NULL);
		m_cameraStatusOK = true;
		m_cameraBothOpen = false;
	}
	else if (Camera::cam_count == 2 && m_cameraSupport1.CameraIsOpen() && m_cameraSupport2.CameraIsOpen())
	{
		SetTimer(2, 50, NULL);
		m_cameraStatusOK = true;
		m_cameraBothOpen = true;
	}

	UpdateData(FALSE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CStereoCameraDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CStereoCameraDlg::OnPaint()
{
	//ClientToScreen(&m_mousePoint);
	//CRect rect;
	//GetDlgItem(IDC_PicDraw)->GetClientRect(rect);
	//GetDlgItem(IDC_PicDraw)->ClientToScreen(rect);
	//CDC *pDC = GetDlgItem(IDC_PicDraw)->GetDC();
	//DrawPic(pDC, rect, minX + m_margin, maxX - m_margin, minY + m_margin, maxY - m_margin);

	/*if (rect.PtInRect(m_mousePoint))
	{
		CPen newPen;
		CPen* oldPen;
		CBrush newBrush;
		CBrush* pOldBrush;

		newBrush.CreateSolidBrush(RGB(255, 255, 255));
		pOldBrush = pDC->SelectObject(&newBrush);
		pDC->Rectangle(rect);
		pDC->SelectObject(pOldBrush);
		newBrush.DeleteObject();

		newPen.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
		oldPen = pDC->SelectObject(&newPen);

		int x = m_mousePoint.x - rect.left;
		int y = m_mousePoint.y - rect.top;

		pDC->MoveTo(0, y);
		pDC->LineTo(rect.Width(), y);
		pDC->MoveTo(x, 0);
		pDC->LineTo(x, rect.Height());

		pDC->SelectObject(oldPen);
		newPen.DeleteObject();
	}
	*/
	
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CStereoCameraDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CStereoCameraDlg::OnClose()
{
	
	CDialog::OnClose();
}



/*----------------------------
* 功能 : 显示图像
*		 将要绘制的图像 src 显示到控件号为 ID 的 Picture 控件
*----------------------------
* 参数 : src	[in]	待显示图像
* 参数 : ID		[in]	图像窗口控件ID
*/
void CStereoCameraDlg::ShowImage(Mat& src, UINT ID)
{
	if (src.empty())
	{
		return;
	}

	CDC* pDC = GetDlgItem(ID)->GetDC();		// 获得显示控件的 DC
	HDC hDC = pDC->GetSafeHdc();				// 获取 HDC(设备句柄) 来进行绘图操作
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);	// 获取控件尺寸位置
	CvvImage cimg;
	IplImage src_img = src;
	cimg.CopyOf(&src_img);						// 复制图片
	cimg.DrawToHDC(hDC, &rect);				// 将图片绘制到显示控件的指定区域内
	ReleaseDC(pDC);
}

/*----------------------------
* 功能 : 弹出打开文件对话框，选择单个或多个文件
*----------------------------
* 函数 : CStereoVisionDlg::DoSelectFiles
* 访问 : private
* 返回 : selectedFiles	选择的文件的路径序列
*
* 参数 : lpszDefExt			[in]	文件默认格式
* 参数 : dwFlags			[in]	对话框选项
* 参数 : lpszFilter			[in]	文件格式过滤条件
* 参数 : lpstrTitle			[in]	对话框标题
* 参数 : lpstrInitialDir	[in]	对话框的初始路径
*/
vector<CStringA> CStereoCameraDlg::DoSelectFiles(
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


void CStereoCameraDlg::OnBnClickedBn2stereocalib()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (m_cameraStatusOK)
	{
		//暂停摄像机
		if ((m_camType == Camera::CAM_TYPE::UVC))
		{
			KillTimer(1);
			KillTimer(2);
		}
	}
	
	int nImages; //总共的图像张数
	int nImgCount = 0;   //图像计数
	vector<CStringA> img0Files, img1Files;
	const char* img0_file = NULL, *img1_file = NULL;
	cv::Mat frame0, frame1, img0, img1;
	cv::Size imageSize;

	CameraCalib::CornerDatas cornerDatas;
	
	try{
		// 选择从本地图片读入定标图像，首先弹出对话框选择图片文件
		img0Files = DoSelectFiles(
			_T("*.bmp"),
			OFN_ENABLESIZING | OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY,
			_T("image files (*.bmp; *.png; *.jpg) |*.bmp; *.png; *.jpg; *.jpeg| All Files (*.*) |*.*||"),
			_T("选择左视图文件"),
			_T("Imgs\\Left"));
		img1Files = DoSelectFiles(
			_T("*.bmp"),
			OFN_ENABLESIZING | OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY,
			_T("image files (*.bmp; *.png; *.jpg) |*.bmp; *.png; *.jpg; *.jpeg| All Files (*.*) |*.*||"),
			_T("选择右视图文件"),
			_T("Imgs\\Right"));

		if (img0Files.empty() || img1Files.empty())	// 判断是否获得图片
		{
			LPCTSTR errMsg = _T("没有选择到有效的图像文件，请重新选择!");
			throw errMsg;
		}


		img0_file = img0Files[0];
		img0 = imread(img0_file);
		imageSize = img0.size();
		nImages = MIN(img0Files.size(), img1Files.size());
		Size imgSize = img0.size();
		Size boardSize(m_nBoardWidth, m_nBoardHeight);
				
		//图像小于5张 不能标定
		if (nImages < 5)
		{
			LPCTSTR errMsg = _T("图像少于5张，不能标定！");
			throw errMsg;
		}

		RunStateInfoShow(_T("开始进行标定..."));

		//初始化角点数据集
		m_cameraCalibrator.initCornerData(nImages, imgSize, boardSize, m_nSquareSize, cornerDatas);
			
		// 开始检测角点
		MSG msg;
		while (nImgCount < nImages)
		{
			// MFC 窗口消息处理
			if (::PeekMessage(&msg, this->m_hWnd, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					LPCTSTR errMsg = _T("手动退出双目标定!");
					throw errMsg;
				}

				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}

			img0_file = img0Files[nImgCount];
			frame0 = imread(img0_file);
			img1_file = img1Files[nImgCount];
			frame1 = imread(img1_file);

			if (frame0.empty())	break;
			if (frame1.empty())	break;

			// 复制图像
			img0 = frame0.clone();
			img1 = frame1.clone();

			// 检测角点
			bool flag = m_cameraCalibrator.detectCorners(img0, img1, cornerDatas, nImgCount);
			if (flag)
			{
				CString strSucceed;
				strSucceed.Format(_T("第%d张图像角点检测成功!"), nImgCount + 1);
				RunStateInfoShow(strSucceed);

				//使图像反色，表明同时找到完整的棋盘角点
				//bitwise_not(img0, img0);
				//bitwise_not(img1, img1);
				//将图片显示在窗口中;
				ShowImage(img0, IDC_PicLfCam);
				ShowImage(img1, IDC_PicRtCam);

				nImgCount++;
	
			}
			else
			{
				CString strFalseCorners;
				strFalseCorners.Format(_T("第%d张图像角点检测失败！"), nImgCount + 1);
				RunStateInfoShow(strFalseCorners);
				ShowImage(img0, IDC_PicLfCam);
				ShowImage(img1, IDC_PicRtCam);
				return;
			}
		}

		//开始标定计算
		m_CalibrationResult1.dBoardWidth = m_nBoardWidth;
		m_CalibrationResult1.dBoardHeight = m_nBoardHeight;
		m_CalibrationResult1.dPixelSize = m_nPixelSize;
		m_CalibrationResult1.dSquareSize = m_nSquareSize;

		RunStateInfoShow(_T("正在进行标定计算…"));
		m_cameraCalibrator.calibrateStereoCamera(cornerDatas, m_CalibrationResult1);
	
		//获取标定时间;
		CString sCurrentTime;
		CTime ttii;
		ttii = CTime::GetCurrentTime();
		sCurrentTime = ttii.Format(_T("%Y年%m月%d日 %X"));
		m_CalibrationResult1.strCalibrateTime = sCurrentTime;
		//显示标定参数;
		CString str;
		str.Empty();
		str.Format(_T("左相机焦距：%fmm"), m_CalibrationResult1.dLeftCameraFocalLen);
		RunStateInfoShow(str);

		str.Empty();
		str.Format(_T("右相机焦距：%fmm"), m_CalibrationResult1.dRightCameraFocalLen);
		RunStateInfoShow(str);

		str.Empty();
		str.Format(_T("标定误差：%fmm"), m_CalibrationResult1.dCaliratebError);
		RunStateInfoShow(str);

		str.Empty();
		if (m_CalibrationResult1.dCaliratebError < 0.8)
		{
			RunStateInfoShow(_T("标定成功！"));
		}
		else
		{
			RunStateInfoShow(_T("标定误差过大，建议重新标定！"));
		}

		str.Empty();
		str.Format(_T("标定时间：%s"), m_CalibrationResult1.strCalibrateTime);
		RunStateInfoShow(str);

		//保存标定数据;
		m_isWhichCamera = GetCheckedRadioButton(IDC_RADCAMUP, IDC_RADCAMDOWN);
		int isWhichCC = 1;
		m_CalibrationResult1.sCaliResultFilePath.Format(_T("%s\\CalibrationResult"), GetAppExecPath());
		if (m_isWhichCamera == IDC_RADCAMDOWN)
			isWhichCC = 2;
		m_CalibrationResult1.WriteYMLFile(isWhichCC);
		
		
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
	
	//恢复摄像机
	if (m_cameraStatusOK)
	{
		if (m_camType == Camera::CAM_TYPE::UVC)
		{
			if (m_cameraBothOpen)
			{
				SetTimer(2, 50, NULL); //恢复同时显示上下相机的定时器
			}
			else
			{
				SetTimer(1, 50, NULL);	// 50ms 定时显示
			}
		}
	}
}


void CStereoCameraDlg::OnBnClickedBnsetlfcam()
{
}


void CStereoCameraDlg::OnBnClickedBnsetrtcam()
{
}


void CStereoCameraDlg::OnBnClickedBngrabbothcam()
{
	// TODO: Add your control notification handler code here
	if (m_cameraStatusOK)
	{
		if (m_camType == Camera::CAM_TYPE::UVC)
		{
			Mat img_left, img_right;
			Mat frame;
			if (m_cameraSupport1.CameraIsOpen())
			{
				frame = m_cameraSupport1.getOneFrame();
			}
			if (m_cameraSupport2.CameraIsOpen())
			{
				frame = m_cameraSupport2.getOneFrame();
			}
			if (frame.empty())
			{
				return;
			}
			Size2i img_size(frame.cols / 2, frame.rows);
			img_left = frame(Rect(0, 0, img_size.width, img_size.height));
			img_right = frame(Rect(img_size.width, 0, img_size.width, img_size.height));

			CString calibImgPath;
			calibImgPath.Format("%s\\calibrateImg", m_workDir);
			if (!PathFileExists(calibImgPath))  //如果该路径不存在则新建该路径;
			{
				CreateDirectory(calibImgPath, NULL);
			}
			CString sFileName1, sFileName2;
			CString msg;
			sFileName1.Format("%s\\L_%d.jpg", calibImgPath, m_imgCount);//初始化保存文件的文件名
			sFileName2.Format("%s\\R_%d.jpg", calibImgPath, m_imgCount);//初始化保存文件的文件名

			imwrite(sFileName1.GetBuffer(0), img_left);
			imwrite(sFileName2.GetBuffer(0), img_right);

			msg.Format("保存图片成功!");
			RunStateInfoShow(_T(msg));
		}

		m_imgCount++;
	}

}


void CStereoCameraDlg::OnBnClickedBtrunmind()
{
}


void CStereoCameraDlg::OnBnClickedBnruncam()
{
	// TODO: Add your control notification handler code here
	//根据相机所在位置初始化标定参数
	m_isWhichCamera = GetCheckedRadioButton(IDC_RADCAMUP, IDC_RADCAMDOWN);

	//释放定时器
	KillTimer(1);
	KillTimer(2);
	m_cameraSupport1.CameraClose();  //关闭摄像头
	m_cameraSupport2.CameraClose();
	bool cam_bool;
	if (m_isWhichCamera == IDC_RADCAMUP) //打开上部相机
	{
		cam_bool = m_cameraSupport1.CameraOpen(Camera::CAM_TYPE::UVC, m_CameraSelectID);
	}
	else if (m_isWhichCamera == IDC_RADCAMDOWN) //打开下部相机
	{
		cam_bool = m_cameraSupport2.CameraOpen(Camera::CAM_TYPE::UVC, m_CameraSelectID);
	}
	
	if (!cam_bool)
	{
		RunStateInfoShow(_T("启动摄像头失败！"));
		return;
	}

	CWnd *pwd = GetDlgItem(IDC_LeftCenterPic);
	CRect rectPicture; 
	pwd->GetClientRect(&rectPicture);
	pwd->GetDC()->FillSolidRect(&rectPicture, RGB(255, 255, 255));
	
	pwd = GetDlgItem(IDC_RightCenterPic);
	pwd->GetClientRect(&rectPicture);
	pwd->GetDC()->FillSolidRect(&rectPicture, RGB(255, 255, 255));

	m_cameraStatusOK = true;
	m_cameraBothOpen = false;
	SetTimer(1, 50, NULL);	// 50ms 定时显示
	UpdateData(FALSE);
}


void CStereoCameraDlg::OnBnClickedBnrunupdown()
{
	// TODO: Add your control notification handler code here
	if (Camera::cam_count != 2){
		RunStateInfoShow(_T("摄像头数目不等于2！"));
		return;
	}
	
	//释放定时器
	KillTimer(1);
	KillTimer(2);
	m_cameraSupport1.CameraClose();  //关闭摄像头
	m_cameraSupport2.CameraClose();

	string up_camera = GetPropertyByName("camera", "up_camera");
	string down_camera = GetPropertyByName("camera", "down_camera");
	bool cam_bool1 = m_cameraSupport1.CameraOpen(Camera::CAM_TYPE::UVC, atoi(up_camera.c_str())-1); 
	bool cam_bool2 = m_cameraSupport2.CameraOpen(Camera::CAM_TYPE::UVC, atoi(down_camera.c_str())-1);
	if (!cam_bool1 || !cam_bool2)
	{
		RunStateInfoShow(_T("摄像头启动失败！"));
		return;
	}
	m_cameraStatusOK = true;
	m_cameraBothOpen = true;
	SetTimer(2, 50, NULL);	// 50ms 定时显示
	CheckRadioButton(IDC_RADA1A2, IDC_RADBOTH, IDC_RADBOTH);
	UpdateData(FALSE);
}


void CStereoCameraDlg::OnBnClickedBnDefaultcamcalibparam()
{
	// TODO: Add your control notification handler code here
	m_nBoardWidth = 7;
	m_nBoardHeight = 5;
	m_nSquareSize = 17.6;
	m_nPixelSize = 0.003;
	UpdateData(FALSE);
}
    
void CStereoCameraDlg::OnBnClickedBnsavecalibparams()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_CalibrationResult1.dBoardHeight = m_nBoardHeight;
	m_CalibrationResult1.dBoardWidth = m_nBoardWidth;
	m_CalibrationResult1.dSquareSize = m_nSquareSize;
	m_CalibrationResult1.dPixelSize = m_nPixelSize;
	m_isWhichCamera = GetCheckedRadioButton(IDC_RADCAMUP, IDC_RADCAMDOWN);
	int isWhichCC = 1;
	if (m_isWhichCamera == IDC_RADCAMDOWN)
		isWhichCC = 2;
	m_CalibrationResult1.WriteYMLFile(isWhichCC);

	RunStateInfoShow(_T("保存参数成功！"));
}

void CStereoCameraDlg::OnTimer(UINT nIDEvent) //实时绘制场景 
{
	// TODO: Add your message handler code here and/or call default 
	switch (nIDEvent)
	{
	case 1:
		/*if (m_camType == Camera::CAM_TYPE::UVC)
		{
			Mat img_left, img_right;
			Mat frame;
			if (m_cameraSupport1.CameraIsOpen())
			{
				frame = m_cameraSupport1.getOneFrame();
			}
			else if ((m_cameraSupport2.CameraIsOpen()))
			{
				frame = m_cameraSupport2.getOneFrame();
			}

			if (frame.empty())
			{
				return;
			}
			Size2i img_size(frame.cols / 2, frame.rows);
			img_left = frame(Rect(0, 0, img_size.width, img_size.height));
			img_right = frame(Rect(img_size.width, 0, img_size.width, img_size.height));

			ShowImage(img_left, IDC_PicLfCam);
			ShowImage(img_right, IDC_PicRtCam);
		}*/
		break;
	case 2:
		/*Mat img_left1, img_right1, img_left2, img_right2;
		Mat frame1 = m_cameraSupport1.getOneFrame();
		Mat frame2 = m_cameraSupport2.getOneFrame();
		if (frame1.empty()||frame2.empty()) 
		{
			return;
		}
		Size2i img_size1(frame1.cols / 2, frame1.rows);
		img_left1 = frame1(Rect(0, 0, img_size1.width, img_size1.height));
		img_right1 = frame1(Rect(img_size1.width, 0, img_size1.width, img_size1.height));

		Size2i img_size2(frame2.cols / 2, frame2.rows);
		img_left2 = frame2(Rect(0, 0, img_size2.width, img_size2.height));
		img_right2 = frame2(Rect(img_size2.width, 0, img_size2.width, img_size2.height));

		ShowImage(img_left1, IDC_PicLfCam);
		ShowImage(img_right1, IDC_PicRtCam);
		ShowImage(img_left2, IDC_LeftCenterPic);
		ShowImage(img_right2, IDC_RightCenterPic);*/
		break;
	}
	
	CDialogEx::OnTimer(nIDEvent);
}

BEGIN_EVENTSINK_MAP(CStereoCameraDlg, CDialogEx)
END_EVENTSINK_MAP()


void CStereoCameraDlg::OnBnClickedBnxiuzheng()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	vector<CStringA> imgFile;
	imgFile = DoSelectFiles(
		_T("*.bmp"),
		OFN_ENABLESIZING | OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY,
		_T("image files (*.bmp; *.png; *.jpg) |*.bmp; *.png; *.jpg; *.jpeg| All Files (*.*) |*.*||"),
		_T("选择1张图像文件"),
		_T("Imgs\\"));

	if (imgFile.empty())
	{
		return;
	}
	Mat img = imread(imgFile[0].GetBuffer(0));
	m_isWhichCamera = GetCheckedRadioButton(IDC_RADCAMUP, IDC_RADCAMDOWN);

	CameraCalib::CalibrationParames *mC = NULL;
	if (m_isWhichCamera == IDC_RADCAMUP)
	{
		mC = &m_CalibrationResult1;
	}
	if (m_isWhichCamera == IDC_RADCAMDOWN)
	{
		mC = &m_CalibrationResult2;
	}
	if (mC == NULL)
		return;

	vector<cv::Point3f> obj_points;
	for (int i = 0; i < (*mC).dBoardHeight; i++)
	{
		for (int j = 0; j < (*mC).dBoardWidth; j++)
		{
			Point3f realPoint(j*(*mC).dSquareSize, i*(*mC).dSquareSize, 0);
			obj_points.push_back(realPoint);
		}
	}
	
	int board_n = (*mC).dBoardWidth*(*mC).dBoardHeight;
	vector<cv::Point2f> corners((*mC).dBoardWidth*(*mC).dBoardHeight,Point2f(0.0,0.0));
	findChessboardCorners(img, cv::Size2i((*mC).dBoardWidth, (*mC).dBoardHeight), corners);

	if (corners.empty() || int(corners.size()) != board_n)
	{
		RunStateInfoShow(_T("检测角点失败或者角点数量错误..."));
		return;
	}

	CvMat* image_points = cvCreateMat(board_n, 2, CV_32FC1);
	CvMat* object_points = cvCreateMat(board_n, 3, CV_32FC1);
	CvMat intrinsic = (*mC).mLeftCameraInParas;
	CvMat distortion = (*mC).mLeftCameraDistorParas;
	CvMat* rotation_vector = cvCreateMat(3, 1, CV_32FC1);
	CvMat* translation_vector = cvCreateMat(3, 1, CV_32FC1);
	CvMat* rotation_mat = cvCreateMat(3, 3, CV_32FC1);

	for (int i = 0; i < board_n; ++i)
	{
		CV_MAT_ELEM(*image_points, float, i, 0) = corners[i].x;
		CV_MAT_ELEM(*image_points, float, i, 1) = corners[i].y;
		CV_MAT_ELEM(*object_points, float, i, 0) = obj_points[i].x;
		CV_MAT_ELEM(*object_points, float, i, 1) = obj_points[i].y;
		CV_MAT_ELEM(*object_points, float, i, 2) = obj_points[i].z;
	}

	//计算外参
	cvFindExtrinsicCameraParams2(object_points, image_points, &intrinsic, &distortion, rotation_vector, translation_vector);
	cvRodrigues2(rotation_vector, rotation_mat);
	m_rotateMat = Mat(rotation_mat, true);
	m_translateMat = Mat(translation_vector, true);

	(*mC).amendRotMat = m_rotateMat;
	//(*mC).dCameraHeigh = m_CameraHeigh1;
	int isWhichCC = 1;
	if (m_isWhichCamera == IDC_RADCAMDOWN)
		isWhichCC = 2;
	(*mC).WriteYMLFile(isWhichCC);
	RunStateInfoShow(_T("已保存修正参数。"));

}


void CStereoCameraDlg::OnBnClickedBnreadfrommatlab()
{
	// TODO: Add your control notification handler code here
	//读取内部参数   
	CString Left_Intrinsics_File;
	Left_Intrinsics_File.Format(_T("%s\\Intrinsics_Camera_Left.xml"), m_CalibrationResult1.sCaliResultFilePath);
	CString Right_Intrinsics_File;
	Right_Intrinsics_File.Format(_T("%s\\Intrinsics_Camera_Right.xml"), m_CalibrationResult1.sCaliResultFilePath);
	CString Left_Distortion_File;
	Left_Distortion_File.Format(_T("%s\\Distortion_Camera_Left.xml"), m_CalibrationResult1.sCaliResultFilePath);
	CString Right_Distortion_File;
	Right_Distortion_File.Format(_T("%s\\Distortion_Camera_Right.xml"), m_CalibrationResult1.sCaliResultFilePath);
	CString Translation_File;
	Translation_File.Format(_T("%s\\Translation.xml"), m_CalibrationResult1.sCaliResultFilePath);
	CString RotRodrigues_File;
	RotRodrigues_File.Format(_T("%s\\RotRodrigues.xml"), m_CalibrationResult1.sCaliResultFilePath);

	CvMat *Intrinsics_Camera_Left = (CvMat *)cvLoad(Left_Intrinsics_File);
	CvMat *Intrinsics_Camera_Right = (CvMat *)cvLoad(Right_Intrinsics_File);
	CvMat *Distortion_Camera_Left = (CvMat *)cvLoad(Left_Distortion_File);
	CvMat *Distortion_Camera_Right = (CvMat *)cvLoad(Right_Distortion_File);
	CvMat *Translation_matlab = (CvMat *)cvLoad(Translation_File);
	CvMat *RotRodrigues_matlab = (CvMat *)cvLoad(RotRodrigues_File);
	CvMat *R_opencv = cvCreateMat(3, 3, CV_32F);
	cvRodrigues2(RotRodrigues_matlab, R_opencv);

	m_CalibrationResult1.mLeftCameraInParas = Intrinsics_Camera_Left;
	m_CalibrationResult1.mLeftCameraDistorParas = Distortion_Camera_Left;
	m_CalibrationResult1.mRightCameraInParas = Intrinsics_Camera_Right;
	m_CalibrationResult1.mRightCameraDistorParas = Distortion_Camera_Right;
	m_CalibrationResult1.mL2RRotation = RotRodrigues_matlab;
	m_CalibrationResult1.mL2RRotation33 = R_opencv;
	m_CalibrationResult1.mL2RTranslation = Translation_matlab;

	CString leftPointsFile;
	leftPointsFile.Format(_T("%s\\x_left_1.xml"), m_CalibrationResult1.sCaliResultFilePath);
	CString rightPointsFile;
	rightPointsFile.Format(_T("%s\\x_right_1.xml"), m_CalibrationResult1.sCaliResultFilePath);
	CvMat *leftPoints = (CvMat *)cvLoad(leftPointsFile);
	CvMat *rightPoints = (CvMat *)cvLoad(rightPointsFile);
	Mat p1, p2;
	transpose(Mat(leftPoints), p1);
	transpose(Mat(rightPoints), p2);
	Mat F = findFundamentalMat(p1, p2,FM_RANSAC, 1, 0.99);
	m_CalibrationResult1.mFoundational = F;

	m_isWhichCamera = GetCheckedRadioButton(IDC_RADCAMUP, IDC_RADCAMDOWN);
	int isWhichCC = 1;
	if (m_isWhichCamera == IDC_RADCAMDOWN)
		isWhichCC = 2;
	m_CalibrationResult1.WriteYMLFile(isWhichCC);

	RunStateInfoShow(_T("读入matlab标定参数成功！"));
}


void CStereoCameraDlg::OnCbnSelchangeCameraselect()
{
	// TODO: Add your control notification handler code here
	int nsel;
	CString str;
	nsel = m_CameraSelectCombox.GetCurSel();
	m_CameraSelectCombox.GetLBText(nsel, str);
	m_CameraSelect = str;
	m_CameraSelectID = nsel;
}

void CStereoCameraDlg::InitCalibParams(CameraCalib::CalibrationParames& cameraCalibParams, int whichNum)
{
	//读取标定数据;
	cameraCalibParams.sCaliResultFilePath.Format(_T("%s\\CalibrationResult"), m_workDir);
	bool bCaliSucflag = cameraCalibParams.ReadYMLFile(whichNum);
	//默认标定参数，从文件中读取
	m_nBoardHeight = cameraCalibParams.dBoardHeight;
	m_nBoardWidth = cameraCalibParams.dBoardWidth;
	m_nSquareSize = cameraCalibParams.dSquareSize;
	m_nPixelSize = cameraCalibParams.dPixelSize;
	//m_CameraHeigh1 = cameraCalibParams.dCameraHeigh;
	//m_rotateMat = cameraCalibParams.amendRotMat;

	if (bCaliSucflag)
	{
		CString str;
		str.Empty();
		str.Format(_T("左相机焦距：%fmm"), cameraCalibParams.dLeftCameraFocalLen);
		RunStateInfoShow(str);

		str.Empty();
		str.Format(_T("右相机焦距：%fmm"), cameraCalibParams.dRightCameraFocalLen);
		RunStateInfoShow(str);

		str.Empty();
		str.Format(_T("标定误差：%fmm"), cameraCalibParams.dCaliratebError);
		RunStateInfoShow(str);

		str.Empty();
		str.Format(_T("标定时间：%s"), cameraCalibParams.strCalibrateTime);
		RunStateInfoShow(str);

	}
	else
	{
		RunStateInfoShow(_T("标定文件不存在。"));
	}

}

void CStereoCameraDlg::DrawPic(CDC *pDC, CRect &rectPicture, vector<Point2d>Points, double xmin, double xmax, double ymin, double ymax)

{
	//确定图象显示大小
	int width = rectPicture.Width();
	int height = rectPicture.Height();

	if (Points.empty())
		return;

	pDC->FillSolidRect(&rectPicture, RGB(255, 255, 255));
	
	minX = xmin - m_margin;
	maxX = xmax + m_margin;
	minY = ymin - m_margin;
	maxY = ymax + m_margin;
	//minX = 645 - m_margin;
	//maxX = 860 + m_margin;
	//minY = 180 - m_margin;
	//maxY = 380 + m_margin;

	//确定X，Y轴每单位显示宽度 
	intervalX = (width - myleft - myright) / (maxX - minX);
	intervalY = (height - mybottom - mytop) / (maxY - minY);
	//绘制X，Y轴
	//X轴从图形区域最左端到最右端
	pDC->MoveTo(int(myleft), int(height - mybottom));
	pDC->LineTo(int(width - myright), int(height - mybottom));
	//Y轴从图形区域最底端到最顶端
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
		str.Format("%.1f", minX + i*(maxX - minX) / count);
		pDC->MoveTo(int(myleft + spaceX*i), int(height - mybottom));
		pDC->LineTo(int(myleft + spaceX*i), int(height - (mybottom + 5)));
		pDC->TextOut(int(myleft + spaceX*i - 10), int(height - (mybottom - 5)), str);
	}
	//Y轴
	for (int i = 0; i <= count; i++)
	{
		str.Format("%.1f", minY + i*(maxY - minY) / count);
		pDC->MoveTo(int(myleft), int(height - (mybottom + spaceY*i)));
		pDC->LineTo(int(myleft + 5), int(height - (mybottom + spaceY*i)));
		pDC->TextOut(int(myleft - 50), int(height - (mybottom + spaceY*i + 8)), str);
	}

	CPen cu;
	cu.CreatePen(PS_DOT, 3, RGB(255, 0, 0));
	CPen*oldPen = pDC->SelectObject(&cu);

	//绘制数据点
	int x = myleft + (Points[0].x - minX)*intervalX;
	int y = height - (mybottom + (Points[0].y - minY)*intervalY);
	pDC->MoveTo(x, y);

	for (int i = 1; i<Points.size()-1; i++)
	{
		Point2f p = Points[i];
		if (p.x == 0 && p.y == 0)
		{
			i++;
			int x = myleft + (Points[i].x - minX)*intervalX;
			int y = height - (mybottom + (Points[i].y - minY)*intervalY);
			pDC->MoveTo(x, y);
		}

		else{
			CPen*oldPen = pDC->SelectObject(&cu);
			int x = myleft + (p.x - minX)*intervalX;
			int y = height - (mybottom + (p.y - minY)*intervalY);
			pDC->MoveTo(x, y);
			pDC->SetPixel(x, y, RGB(255, 0, 0));
		}
		//pDC->SetPixel(x, y, RGB(255, 0, 0));
	}

	//绘制X，Y轴的变量名
	pDC->TextOut(width / 2, height - 20, "Z轴(深度)");
	pDC->TextOut(0, height / 2, "Y轴(高度)");
}

void CStereoCameraDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	ClientToScreen(&point);
	CRect rect;
	GetDlgItem(IDC_PicDraw)->GetClientRect(rect);
	GetDlgItem(IDC_PicDraw)->ClientToScreen(rect);
	CDC *pDC = GetDlgItem(IDC_PicDraw)->GetDC();
	 
	if (rect.PtInRect(point))
	{
		int x = point.x - rect.left;
		int y = point.y - rect.top;
		float Z = (x - myleft) / intervalX + minX;
		float Y = ((rect.Height() - y) - mybottom) / intervalY + minY;

		CString str;
		str.Format(" z=%f \r\n y=%f", Z, Y);
		GetDlgItem(IDC_VALUE)->SetWindowTextA(_T(str));
	}
	
	CDialogEx::OnMouseMove(nFlags, point);
}


//三维重建
void CStereoCameraDlg::OnBnClickedBncalculate()
{
	//_CrtDumpMemoryLeaks();
	//_CrtSetBreakAlloc(52620);
	UpdateData(TRUE);
	try{
		//计算算法时间
		clock_t start, finish;
		start = clock();
		//vector<Point3d> real_points;
		//重建出来的结构光三维坐标
		vector<Point3d> realPoints1;
		vector<Point3d> realPoints2;
		m_deal2DPoints1.clear();
		m_deal2DPoints2.clear();
		m_deal2DPointsAll.clear();
		m_calRadBN = GetCheckedRadioButton(IDC_RAD_CalcFromCam, IDC_RAD_CalcFromImg);
		Mat leftCenter1, rightCenter1, leftCenter2, rightCenter2;
		Mat matchCenter1, matchCenter2;
		//判断是否是在线模式
		if (m_calRadBN == IDC_RAD_CalcFromCam)
		{
			if (!m_cameraStatusOK)
			{
				RunStateInfoShow(_T("相机状态错误，请确认相机是否打开..."));
				return;
			}
			if ((m_camType == Camera::CAM_TYPE::UVC))
			{
				if (doLayerTest==false) //不控制激光
				{
					//上部相机
					if (m_cameraSupport1.CameraIsOpen())
					{
						Mat frame = m_cameraSupport1.getOneFrame();
						Size2i img_size(frame.cols / 2, frame.rows);
						Mat img_left = frame(Rect(0, 0, img_size.width, img_size.height));
						Mat img_right = frame(Rect(img_size.width, 0, img_size.width, img_size.height));

						RunStateInfoShow(_T("正在进行测距计算..."));
						m_measureDist.caculateDistance(img_left, img_right, m_CalibrationResult1, realPoints1, leftCenter1, rightCenter1, matchCenter1, 0, 2);
					}
					//下部相机
					if (m_cameraSupport2.CameraIsOpen())
					{
						Mat frame = m_cameraSupport2.getOneFrame();
						Size2i img_size(frame.cols / 2, frame.rows);
						Mat img_left = frame(Rect(0, 0, img_size.width, img_size.height));
						Mat img_right = frame(Rect(img_size.width, 0, img_size.width, img_size.height));

						RunStateInfoShow(_T("正在进行测距计算..."));
						m_measureDist.caculateDistance(img_left, img_right, m_CalibrationResult2, realPoints2, leftCenter2, rightCenter2, matchCenter2, 1, 2);
					}
				}
				else  //控制激光
				{
					Mat upMat, downMat;
					//得到带激光相减后的待测量图片
					Camera::GetMeasPic(upLayerMat, upMat);
					Camera::GetMeasPic(downLayerMat, downMat);
					//处理上部相机
					if (!upMat.empty())
					{
						//切分左右视图
						Size2i img_size1(upMat.cols / 2, upMat.rows);
						Mat img_left1 = upMat(Rect(0, 0, img_size1.width, img_size1.height));
						Mat img_right1 = upMat(Rect(img_size1.width, 0, img_size1.width, img_size1.height));
						//开始测量
						RunStateInfoShow(_T("正在进行测距计算..."));
						m_measureDist.caculateDistance(img_left1, img_right1, m_CalibrationResult1, realPoints1, leftCenter1, rightCenter1, matchCenter1, 0, 1, 2);
					}

					//处理下部相机
					if (!downMat.empty())
					{
						downMat = downMat * 2;
						Size2i img_size2(downMat.cols / 2, downMat.rows);
						Mat img_left2 = downMat(Rect(0, 0, img_size2.width, img_size2.height));
						Mat img_right2 = downMat(Rect(img_size2.width, 0, img_size2.width, img_size2.height));
						RunStateInfoShow(_T("正在进行测距计算..."));
						m_measureDist.caculateDistance(img_left2, img_right2, m_CalibrationResult2, realPoints2, leftCenter2, rightCenter2, matchCenter2, 1, 2, 2);
					}
					doLayerTest = false;	
					upLayerMat.clear();
					downLayerMat.clear();
				}
				
			}
		}
		else  //离线测距
		{
			vector<CStringA> imgFiles;
			const char* img0_filename = NULL, *img1_filename = NULL;

			imgFiles = DoSelectFiles(
				_T("*.bmp"),
				OFN_ENABLESIZING | OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY,
				_T("image files (*.bmp; *.png; *.jpg) |*.bmp; *.png; *.jpg; *.jpeg| All Files (*.*) |*.*||"),
				_T("选择两张图像文件"),
				_T("Imgs\\Left"));

			if (imgFiles.empty() || imgFiles.size() < 2 || imgFiles.size() > 2)	// 判断输入图片
			{
				AfxMessageBox(_T("请选择两张图像！"));
				return;
			}
			RunStateInfoShow(_T("正在进行测距计算..."));
			img0_filename = imgFiles[0];
			img1_filename = imgFiles[1];

			Mat frame_left = imread(img0_filename);
			Mat frame_right = imread(img1_filename);

			ShowImage(frame_left, IDC_PicLfCam);
			ShowImage(frame_right, IDC_PicRtCam);
			leftCenter1 = Mat::zeros(frame_left.size(), CV_8UC1);
			rightCenter1 = Mat::zeros(frame_right.size(), CV_8UC1);

			m_isWhichCamera = GetCheckedRadioButton(IDC_RADCAMUP, IDC_RADCAMDOWN);
			if (m_isWhichCamera == IDC_RADCAMUP)
			{
				m_measureDist.caculateDistance(frame_left, frame_right, m_CalibrationResult1, realPoints1, leftCenter1, rightCenter1, matchCenter1, 0, 1, 2);
			}
			else
			{
				//frame_left = 2 * frame_left ;
				//frame_right = 2 * frame_right ;
				m_measureDist.caculateDistance(frame_left, frame_right, m_CalibrationResult2, realPoints2, leftCenter2, rightCenter2, matchCenter2, 1, 2, 2);
			}
		}

		//保存得到的三维坐标
		ofstream fout2(m_workDir+"\\OUT_real_points3D.txt");
		for (int i = 0; i != realPoints1.size(); i++)
		{
			Point3d p = realPoints1[i];
			fout2 << p.x << "," << p.y << "," << p.z << endl;
		}
		fout2 << "-------------------------------------------" << endl << endl << endl;
		for (int i = 0; i != realPoints2.size(); i++)
		{
			Point3d p = realPoints2[i];
			fout2 << p.x << "," << p.y << "," << p.z << endl;
		}

		//显示中间过程
		if (!m_cameraBothOpen)
		{
			if (!matchCenter1.empty())
			{
				ShowImage(leftCenter1, IDC_LeftCenterPic);
				ShowImage(rightCenter1, IDC_RightCenterPic);
				ShowImage(matchCenter1, IDC_MatchPic);
			}
			else if (!matchCenter2.empty())
			{
				ShowImage(leftCenter2, IDC_LeftCenterPic);
				ShowImage(rightCenter2, IDC_RightCenterPic);
				ShowImage(matchCenter2, IDC_MatchPic);
			}
		}

		//修正的摄像机旋转矩阵
		Mat_<double> rotateMat1 = m_CalibrationResult1.amendRotMat; 
		Mat_<double> rotateMat2 = m_CalibrationResult2.amendRotMat;
		//深度范围
		//MEAS_VALI_MIN_Z = m_ZmeasRange1;
		//MEAS_VALI_MAX_Z = m_ZmeasRange2;
		GetRevParam(up_x, up_y, down_x, down_y);

		//判断计算的轨检几何参数
		m_calRadArg = GetCheckedRadioButton(IDC_RADA1A2, IDC_RADBOTH);
		if (m_calRadArg == IDC_RADA1A2)
		{
			m_measureDist.deal3DPoints(realPoints1, m_deal2DPoints1, rotateMat1, 0);
			m_measureDist.deal2DPoints(m_deal2DPoints1, 0, up_x, up_y);
			m_measureDist.deal2DPoints(m_deal2DPoints1, 0, m_XiuHor, m_XiuVer);
		}
		else if (m_calRadArg == IDC_RADA3A4)
		{
			m_measureDist.deal3DPoints(realPoints2, m_deal2DPoints2, rotateMat2, 0);
			m_measureDist.deal2DPoints(m_deal2DPoints2, 0, down_x, down_y);
			m_measureDist.deal2DPoints(m_deal2DPoints2, 0, m_XiuHor, m_XiuVer);
		}
		else //上下同时检测
		{
			m_measureDist.deal3DPoints(realPoints1, m_deal2DPoints1, rotateMat1, 0);
			m_measureDist.deal3DPoints(realPoints2, m_deal2DPoints2, rotateMat2, 0);
			m_measureDist.deal2DPoints(m_deal2DPoints1, 0, up_x, up_y);
			m_measureDist.deal2DPoints(m_deal2DPoints2, 0, down_x, down_y);
			m_measureDist.deal2DPoints(m_deal2DPoints2, 0, m_XiuHor, m_XiuVer);
		}

		ShowRes();
		finish = clock();
		CString stime;
		stime.Format("Time : %f ms", 1000.0*(finish - start) / (double)CLOCKS_PER_SEC);
		RunStateInfoShow(stime);
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


void CStereoCameraDlg::OnBnClickedMeasurea1a2a3a4()
{
	// TODO: Add your control notification handler code here
	double  a3, a4, a11, a3_min, a4_min, a4_max;
	CStringA s_a1, s_a2, s_a3, s_a4;
	a3 = a4 = 0;
	vector<Point2d> pointsA,pointsA3, pointsA4, pointsA5;
	double a1_maxx, a1_maxy, a2, a2_minx, a2_miny;
	a1_maxx = 0, a1_maxy = 0, a2 = 0, a2_minx = 0, a2_miny = 0;
	vector<Point2d> pointsA1, pointsA2;

	m_calRadArg = GetCheckedRadioButton(IDC_RADA1A2, IDC_RADBOTH);
	if (m_calRadArg == IDC_RADA1A2)
	{
		m_measureDist.measureA1A2_NEW(m_deal2DPoints1, a1_maxx, a1_maxy, a2_minx, a2_miny, a2, pointsA1, pointsA2, pointsA);
	}
	else if (m_calRadArg == IDC_RADA3A4)
	{
		m_measureDist.measureA3A4(m_deal2DPoints2, a3, a4, a3_min, a4_min, pointsA3, pointsA4, pointsA5);
	}
	else
	{
		m_measureDist.measureA1A2_NEW(m_deal2DPoints1, a1_maxx, a1_maxy, a2_minx, a2_miny, a2, pointsA1, pointsA2, pointsA);
		m_measureDist.measureA3A4(m_deal2DPoints2, a3, a4, a3_min, a4_min, pointsA3, pointsA4, pointsA5, a2_minx, a2_miny);
	}

	s_a1.Format("%.1f", a1_maxy);
	s_a2.Format("%.1f", a2);
	s_a3.Format("%.1f", a3);
	s_a4.Format("%.1f", a4);

	GetDlgItem(IDC_VALUE_A1)->SetWindowTextA(s_a1);
	GetDlgItem(IDC_VALUE_A2)->SetWindowTextA(s_a2);
	GetDlgItem(IDC_VALUE_A3)->SetWindowTextA(s_a3);
	GetDlgItem(IDC_VALUE_A4)->SetWindowTextA(s_a4);
}


void CStereoCameraDlg::OnBnClickedBnsaveConfig()
{
	LOWCOMDlg * lowComDlg = new LOWCOMDlg();
	lowComDlg->Create(IDD_LOWCOM_DIALOG, this);
	lowComDlg->ShowWindow(SW_SHOW);
}



void CStereoCameraDlg::OnBnClickedOpenlayerpic()
{
	if (m_cameraSupport1.CameraIsOpen())  //打开激光前拍摄照片
	{
		upLayerMat.clear();
		Camera::CameraTakePics(m_cameraSupport1, 2, upLayerMat);
	}
	if (m_cameraSupport2.CameraIsOpen())
	{
		downLayerMat.clear();
		Camera::CameraTakePics(m_cameraSupport2, 2, downLayerMat);
	}

	//do_open_allLayer();
	do_open_topLayer();
}


void CStereoCameraDlg::OnBnClickedCloselayerpic()
{
	if (m_cameraSupport1.CameraIsOpen())
	{ 
		Camera::CameraTakePics(m_cameraSupport1, 2, upLayerMat);
	}
	do_close_allLayer();
	Sleep(1000);
	do_open_belowLayer();
	Sleep(1000);
	if (m_cameraSupport2.CameraIsOpen())
	{
		Camera::CameraTakePics(m_cameraSupport2, 2, downLayerMat);
	}
	do_close_allLayer();
	doLayerTest = true;
}


void CStereoCameraDlg::OnBnClickedBnadjust()
{
	UpdateData(true);
	vector<double> ylist1;
	vector<double> zlist1;
	vector<double> ylist2;
	vector<double> zlist2;

	m_measureDist.deal2DPoints(m_deal2DPoints2, 0, m_XiuHor, m_XiuVer);

	m_measureDist.fillyzList(m_deal2DPoints1, ylist1, zlist1);
	m_measureDist.fillyzList(m_deal2DPoints2, ylist2, zlist2);

	//合并数据点
	m_deal2DPointsAll.clear();
	m_deal2DPointsAll.insert(m_deal2DPointsAll.end(), m_deal2DPoints1.begin(), m_deal2DPoints1.end());
	m_deal2DPointsAll.insert(m_deal2DPointsAll.end(), m_deal2DPoints2.begin(), m_deal2DPoints2.end());
	vector<double> ylist;
	vector<double> zlist;
	ylist.insert(ylist.end(), ylist1.begin(), ylist1.end());
	ylist.insert(ylist.end(), ylist2.begin(), ylist2.end());
	zlist.insert(zlist.end(), zlist1.begin(), zlist1.end());
	zlist.insert(zlist.end(), zlist2.begin(), zlist2.end());
	if (!ylist.empty() && !zlist.empty())
	{
		sort(ylist.begin(), ylist.end());
		sort(zlist.begin(), zlist.end());
		
		CRect rectPicture;
		//	m_picDraw.GetClientRect(&rectPicture);
		//	DrawPic(m_picDraw.GetDC(), rectPicture, m_deal2DPointsAll, zlist[0], zlist[zlist.size() - 1], ylist[0], ylist[ylist.size() - 1]);
	}
}

void CStereoCameraDlg::ShowRes()
{
	vector<double> ylist1;
	vector<double> zlist1;
	vector<double> ylist2;
	vector<double> zlist2;
	vector<double> ylist;
	vector<double> zlist;

	//显示结果
	
	//合并数据点
	m_deal2DPointsAll.insert(m_deal2DPointsAll.end(), m_deal2DPoints1.begin(), m_deal2DPoints1.end());
	m_deal2DPointsAll.insert(m_deal2DPointsAll.end(), m_deal2DPoints2.begin(), m_deal2DPoints2.end());
	m_measureDist.fillyzList(m_deal2DPoints1, ylist1, zlist1);
	m_measureDist.fillyzList(m_deal2DPoints2, ylist2, zlist2);
	ylist.insert(ylist.end(), ylist1.begin(), ylist1.end());
	ylist.insert(ylist.end(), ylist2.begin(), ylist2.end());
	zlist.insert(zlist.end(), zlist1.begin(), zlist1.end());
	zlist.insert(zlist.end(), zlist2.begin(), zlist2.end());
	
	if (!ylist.empty() && !zlist.empty())
	{
		Point2d P(0, 0);
		sort(ylist.begin(), ylist.end());
		sort(zlist.begin(), zlist.end());
		/*if (zlist[0] < MEAS_VALI_MIN_Z)
			MEAS_VALI_MIN_Z = zlist[0] - 50;
		if (zlist[zlist.size() - 1] > MEAS_VALI_MAX_Z)
			MEAS_VALI_MAX_Z = zlist[zlist.size() - 1] + 50;*/

		m_calRadArg = GetCheckedRadioButton(IDC_RADA1A2, IDC_RADBOTH);
		if (m_calRadArg == IDC_RADA1A2)
		{
			double a1_maxx, a1_maxy, a2, a2_minx, a2_miny;
			a1_maxx = 0, a1_maxy = 0, a2 = 0, a2_minx = 0, a2_miny = 0;
			vector<Point2d> pointsA1, pointsA2, pointsA, pointsA4;
			m_measureDist.measureA1A2_NEW(m_deal2DPoints1, a1_maxx, a1_maxy, a2_minx, a2_miny, a2, pointsA1, pointsA2, pointsA);
		/*	pointsA4.insert(pointsA4.end(), pointsA1.begin(), pointsA1.end());
			pointsA4.insert(pointsA4.end(), pointsA2.begin(), pointsA2.end());*/
			CRect rectPicture;
			ofstream fout2("D:\\document\\Debug\\real_point.txt");
			for (int i = 0; i != pointsA.size(); i++)
			{
				Point2d p = pointsA[i];
				fout2 << p.x << "," << p.y << endl;
			}
			m_picDraw.GetClientRect(&rectPicture);
			DrawPic(m_picDraw.GetDC(), rectPicture, m_deal2DPoints1, zlist[0], zlist[zlist.size() - 1], ylist[0], ylist[ylist.size() - 1]);
		}
		else if (m_calRadArg == IDC_RADA3A4)
		{
			double a3, a4, a3_min, a4_min, a4_max;
			vector<Point2d>  pointsA, pointsA3, pointsA4, pointsA5;
			m_measureDist.measureA3A4(m_deal2DPoints2, a3, a4, a3_min, a4_min, pointsA3, pointsA4, pointsA5);
			/*ofstream fout2("D:\\document\\Debug\\real_points3.txt");
			for (int i = 0; i != pointsA5.size(); i++)
			{
				Point2d p = pointsA5[i];
				fout2 << p.x << "," << p.y<< endl;
			}
			ofstream fout3("D:\\document\\Debug\\real_points.txt");
			for (int i = 0; i != m_deal2DPoints2.size(); i++)
			{
				Point2d p = m_deal2DPoints2[i];
				fout3<< p.x << "," << p.y << endl;
			}*/
			pointsA.insert(pointsA.end(), pointsA3.begin(), pointsA3.end());
			pointsA.insert(pointsA.end(), pointsA4.begin(), pointsA4.end());
			vector<double> xlist34;
			vector<double> ylist34;
			for (int i = 0; i != pointsA5.size(); i++)
			{
				xlist34.push_back(pointsA5[i].x);
				ylist34.push_back(pointsA5[i].y);
			}

			if (xlist34.empty() || ylist34.empty())
				return;

			sort(xlist34.begin(), xlist34.end());
			sort(ylist34.begin(), ylist34.end());
			CRect rectPicture;
			m_picDraw.GetClientRect(&rectPicture);
			DrawPic(m_picDraw.GetDC(), rectPicture, pointsA5, xlist34[0], xlist34[xlist34.size() - 1], ylist34[0], ylist34[ylist34.size() - 1]);
		}
		else //上下同时检测
		{
			double a1_maxx, a1_maxy, a2, a2_minx, a2_miny;
			a1_maxx = 0, a1_maxy = 0, a2 = 0, a2_minx = 0, a2_miny = 0;
			vector<Point2d> pointsA1, pointsA2, pointsA, pointsA3, pointsA4, pointsA5, pointsA11;
			m_measureDist.measureA1A2_NEW(m_deal2DPoints1, a1_maxx, a1_maxy, a2_minx, a2_miny, a2, pointsA1, pointsA2, pointsA);
			double a3, a4, a3_min, a4_min, a4_max;
			a2_minx = 710
				; a2_miny = 231;
			m_measureDist.measureA3A4(m_deal2DPoints2, a3, a4, a3_min, a4_min, pointsA3, pointsA4, pointsA5, a2_minx, a2_miny);
			pointsA3.push_back(P);
			pointsA11.insert(pointsA11.end(), pointsA1.begin(), pointsA1.end());
			pointsA11.insert(pointsA11.end(), pointsA2.begin(), pointsA2.end());
			pointsA11.insert(pointsA11.end(), pointsA3.begin(), pointsA3.end());
			pointsA11.insert(pointsA11.end(), pointsA4.begin(), pointsA4.end());
			CRect rectPicture;
			m_picDraw.GetClientRect(&rectPicture);
			/*ofstream fout2("D:\\document\\Debug\\real_point.txt");
			for (int i = 0; i != pointsA5.size(); i++)
			{
				Point2d p = pointsA5[i];
				fout2 << p.x << "," << p.y << endl;
			}*/
			DrawPic(m_picDraw.GetDC(), rectPicture, m_deal2DPoints2, zlist[0], zlist[zlist.size() - 1], ylist[0], ylist[ylist.size() - 1]);
		}
	}
}


void CStereoCameraDlg::OnBnClickedBnRevset()
{
	CParamReviseDlg * paramDlg = new CParamReviseDlg();
	paramDlg->Create(IDD_PARAMRIVISE_DLG, this);
	paramDlg->ShowWindow(SW_SHOW);

}



void CStereoCameraDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	int nImages; //总共的图像张数
	int nImgCount = 0;   //图像计数
	vector<CStringA> img0Files;
	const char* img0_file = NULL;
	cv::Mat frame0, img0;
	cv::Size imageSize;

	CameraCalib::CalibrationParames sigleCalibResult;
	CameraCalib::CornerDatas cornerDatas;

	try{
		// 选择从本地图片读入定标图像，首先弹出对话框选择图片文件
		img0Files = DoSelectFiles(
			_T("*.bmp"),
			OFN_ENABLESIZING | OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY,
			_T("image files (*.bmp; *.png; *.jpg) |*.bmp; *.png; *.jpg; *.jpeg| All Files (*.*) |*.*||"),
			_T("选择左视图文件"),
			_T("Imgs\\Left"));

		if (img0Files.empty())	// 判断是否获得图片
		{
			LPCTSTR errMsg = _T("没有选择到有效的图像文件，请重新选择!");
			throw errMsg;
		}


		img0_file = img0Files[0];
		img0 = imread(img0_file);
		imageSize = img0.size();
		nImages = img0Files.size();
		Size imgSize = img0.size();
		Size boardSize(m_nBoardWidth, m_nBoardHeight);

		//图像小于5张 不能标定
		if (nImages < 5)
		{
			LPCTSTR errMsg = _T("图像少于5张，不能标定！");
			throw errMsg;
		}

		RunStateInfoShow(_T("开始进行标定..."));

		//初始化角点数据集
		//m_cameraCalibrator.initCornerData(nImages, imgSize, boardSize, m_nSquareSize, cornerDatas);
		cornerDatas.nImages = nImages;
		cornerDatas.imageSize = imageSize;
		cornerDatas.boardSize = boardSize;
		cornerDatas.nPointsPerImage = boardSize.width * boardSize.height;
		cornerDatas.objectPoints.resize(nImages, vector<cv::Point3f>(cornerDatas.nPointsPerImage, cv::Point3f(0, 0, 0)));
		cornerDatas.leftImgPoints.resize(nImages, vector<cv::Point2f>(cornerDatas.nPointsPerImage, cv::Point2f(0, 0)));

		//计算棋盘角点的世界坐标值
		int i, j, k, n;
		for (i = 0; i < nImages; i++)
		{
			n = 0;
			for (j = 0; j < boardSize.height; j++)
				for (k = 0; k < boardSize.width; k++)
					cornerDatas.objectPoints[i][n++] = cv::Point3f(k*m_nSquareSize, j*m_nSquareSize, 0);
		}



		// 开始检测角点
		MSG msg;
		while (nImgCount < nImages)
		{
			// MFC 窗口消息处理
			if (::PeekMessage(&msg, this->m_hWnd, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					LPCTSTR errMsg = _T("手动退出双目标定!");
					throw errMsg;
				}

				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}

			img0_file = img0Files[nImgCount];
			frame0 = imread(img0_file);
			if (frame0.empty())	break;
			// 复制图像
			img0 = frame0.clone();

			// 检测角点
			//bool flag = m_cameraCalibrator.detectCorners(img0, img1, cornerDatas, nImgCount);
			// 提取当前棋盘对应的角点坐标子矩阵
			vector<cv::Point2f>& corners1 = cornerDatas.leftImgPoints[nImgCount];

			// 寻找棋盘及其角点
			bool found1;
			int flags = CV_CALIB_CB_ADAPTIVE_THRESH + CV_CALIB_CB_NORMALIZE_IMAGE + CV_CALIB_CB_FAST_CHECK;

			found1 = findChessboardCorners(img0, cornerDatas.boardSize, corners1);

			// 则将检测到的角点坐标精确化
			if (found1)
			{
				//转换为灰度图
				cv::Mat gray1;
				cvtColor(img0, gray1, CV_RGB2GRAY);

				//计算角点的精确坐标
				cv::Size regionSize(11, 11);
				cornerSubPix(gray1, corners1, regionSize, cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 30, 0.05));
			}

			if (found1)
			{
				// 显示检测到的角点
				drawChessboardCorners(img0, cornerDatas.boardSize, corners1, found1);

				CString strSucceed;
				strSucceed.Format(_T("第%d张图像角点检测成功!"), nImgCount + 1);
				RunStateInfoShow(strSucceed);

				//将图片显示在窗口中;
				imshow("1" ,img0);
				waitKey(100);
				nImgCount++;

			}
			else
			{
				CString strFalseCorners;
				strFalseCorners.Format(_T("第%d张图像角点检测失败！"), nImgCount + 1);
				RunStateInfoShow(strFalseCorners);
				imshow("1", img0);
				waitKey(100);
				return;
			}
		}

		cv::destroyAllWindows();

		//开始标定计算
		RunStateInfoShow(_T("正在进行标定计算…"));

		//开始标定计算
		Mat cameraMatrix, distCoeffs;
		vector<Mat> rvecs, tvecs; //旋转矩阵和平移矩阵
		cameraMatrix = Mat(3, 3, CV_32FC1, Scalar::all(0));

		//单目标定
		double rms = calibrateCamera(cornerDatas.objectPoints, cornerDatas.leftImgPoints, cornerDatas.imageSize, cameraMatrix, distCoeffs, rvecs, tvecs, CV_CALIB_FIX_K3);
		RunStateInfoShow(_T("标定计算完成"));

		//标定结果
		sigleCalibResult.dBoardWidth = m_nBoardWidth;
		sigleCalibResult.dBoardHeight = m_nBoardHeight;
		sigleCalibResult.dPixelSize = m_nPixelSize;
		sigleCalibResult.dSquareSize = m_nSquareSize;
		sigleCalibResult.mLeftCameraInParas = cameraMatrix;
		sigleCalibResult.mLeftCameraDistorParas = distCoeffs;
		sigleCalibResult.dLeftCameraFocalLen = (cameraMatrix.at<double>(0, 0) + cameraMatrix.at<double>(1, 1)) / 2 * sigleCalibResult.dPixelSize;
		sigleCalibResult.dCaliratebError = rms;
		//保存最后一张棋盘图的平移旋转矩阵，后面作为世界坐标系 
		if (!rvecs.empty() && !tvecs.empty())
		{
			Mat trainRotationVec = rvecs[rvecs.size() - 1];
			cv::Rodrigues(trainRotationVec, sigleCalibResult.trianRotation);
			sigleCalibResult.trianTanslation = tvecs[tvecs.size() - 1];
		}

		//ofstream fout("test_r_t_vecs.txt");
		//for (int i = 0; i < rvecs.size(); i ++)
		//{
		//	string str1 = string("第") + to_string(i) + "旋转矩阵：";
		//	Mat rMat;
		//	cv::Rodrigues(rvecs[i], rMat);
		//	fout << str1 << rMat << endl;

		//	string str2 = string("第") + to_string(i) + "平移向量：";
		//	fout << str2 << tvecs[i] << endl;
		//}

		//获取标定时间;
		CString sCurrentTime;
		CTime ttii;
		ttii = CTime::GetCurrentTime();
		sCurrentTime = ttii.Format(_T("%Y年%m月%d日 %X"));
		sigleCalibResult.strCalibrateTime = sCurrentTime;

		CString str;
		str.Format(_T("标定时间：%s"), sigleCalibResult.strCalibrateTime);
		RunStateInfoShow(str);

		//保存标定数据;
		sigleCalibResult.sCaliResultFilePath.Format(_T("%s\\CalibrationResult"), GetAppExecPath());
		sigleCalibResult.bIsBinocularCameraCalibration = 0;
		sigleCalibResult.WriteYMLFile();


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


void CStereoCameraDlg::OnBnClickedButton2()
{
	UpdateData(TRUE);
	try{
		//计算算法时间
		clock_t start, finish;
		start = clock();
		//vector<Point3d> real_points;
		//重建出来的结构光三维坐标
		vector<Point3d> realPoints1;
		m_deal2DPoints1.clear();
		m_deal2DPoints2.clear();
		m_deal2DPointsAll.clear();
		m_calRadBN = GetCheckedRadioButton(IDC_RAD_CalcFromCam, IDC_RAD_CalcFromImg);
		Mat center;
		//判断是否是在线模式
		if (m_calRadBN == IDC_RAD_CalcFromCam)
		{

		}
		else  //离线测距
		{
			vector<CStringA> imgFiles;
			const char* img0_filename = NULL, *img1_filename = NULL;

			imgFiles = DoSelectFiles(
				_T("*.bmp"),
				OFN_ENABLESIZING | OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY,
				_T("image files (*.bmp; *.png; *.jpg) |*.bmp; *.png; *.jpg; *.jpeg| All Files (*.*) |*.*||"),
				_T("选择两张图像文件"),
				_T("Imgs\\Left"));

			if (imgFiles.empty() || imgFiles.size() != 1)	// 判断输入图片
			{
				AfxMessageBox(_T("请选择1张图像！"));
				return;
			}
			RunStateInfoShow(_T("正在进行测距计算..."));
			img0_filename = imgFiles[0];

			Mat frame_left = imread(img0_filename);

			ShowImage(frame_left, IDC_PicLfCam);
			center = Mat::zeros(frame_left.size(), CV_8UC1);

			m_isWhichCamera = GetCheckedRadioButton(IDC_RADCAMUP, IDC_RADCAMDOWN);
			m_measureDist.caculateDistance_ARC(frame_left, m_SignalCalibrationResult, realPoints1, center, CENTER_FLAG_STEGER);
		}

		//深度范围
		//MEAS_VALI_MIN_Z = m_ZmeasRange1;
		//MEAS_VALI_MAX_Z = m_ZmeasRange2;
		GetRevParam(up_x, up_y, down_x, down_y);

		//显示中间过程
		if (!center.empty())
		{
			ShowImage(center, IDC_LeftCenterPic);
		}
		for (int i = 0; i < realPoints1.size(); i++)
		{
			m_deal2DPoints1.push_back(Point2d(realPoints1[i].z, realPoints1[i].y));
		}

		ShowRes();
		finish = clock();
		CString stime;
		stime.Format("Time : %f ms", 1000.0*(finish - start) / (double)CLOCKS_PER_SEC);
		RunStateInfoShow(stime);
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
