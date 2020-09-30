// InstructionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "StereoCamera.h"
#include "InstructionDlg.h"
#include "afxdialogex.h"


// CInstructionDlg 对话框

IMPLEMENT_DYNAMIC(CInstructionDlg, CDialogEx)

static UINT indicators[] = { IDS_INDICATOR_MESSAGE, IDS_INDICATOR_COMMSTATUS, IDS_INDICATOR_TIME };

CInstructionDlg::CInstructionDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CInstructionDlg::IDD, pParent)
{

}

CInstructionDlg::~CInstructionDlg()
{
}

void CInstructionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInstructionDlg, CDialogEx)
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDC_BACK, &CInstructionDlg::OnBnClickedBack)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CInstructionDlg 消息处理程序

BOOL CInstructionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	m_bar.Create(this);
	m_bar.SetIndicators((unsigned int *)indicators, sizeof(indicators) / sizeof(UINT));
	CRect rect;
	GetClientRect(&rect);
	m_bar.SetPaneInfo(0, IDS_INDICATOR_MESSAGE, SBPS_NORMAL, 155);
	m_bar.SetPaneInfo(1, IDS_INDICATOR_COMMSTATUS, SBPS_NORMAL, 180);
	m_bar.SetPaneInfo(2, IDS_INDICATOR_TIME, SBPS_STRETCH, 0);
	//m_bar.SetPaneText(0, "您好！", RGB(255, 255, 255));

	LOGFONT logfont;
	GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(logfont), &logfont);
	logfont.lfCharSet = GB2312_CHARSET;
	logfont.lfWeight = FW_NORMAL;
	logfont.lfHeight = 30;
	lstrcpy(logfont.lfFaceName, _T("微软雅黑"));//微软雅黑
	static CFont sfontStatusBar;
	sfontStatusBar.CreateFontIndirect(&logfont);
	m_bar.SetFont(&sfontStatusBar);

	CTime time;
	time = CTime::GetCurrentTime();
	m_bar.SetPaneText(2, time.Format("%H:%M:%S"), RGB(255, 255, 255));
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, ID_INDICATOR_REC);
	//m_bar.GetStatusBarCtrl().SetBkColor(RGB(68, 68, 68));

	SetTimer(1, 1000, NULL);
	SetTimer(2, 2500, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CInstructionDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	lpMMI->ptMinTrackSize.x = 480; // 设定最小跟踪宽度
	lpMMI->ptMinTrackSize.y = 800; // 设定最小跟踪高度
	lpMMI->ptMaxTrackSize.x = 480; // 设定最大跟踪宽度
	lpMMI->ptMaxTrackSize.y = 800; // 设定最大跟踪高度
	CDialogEx::OnGetMinMaxInfo(lpMMI);
}


void CInstructionDlg::OnBnClickedBack()
{
	// TODO:  在此添加控件通知处理程序代码

	this->ShowWindow(SW_HIDE);
}


void CInstructionDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO:  在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()

	CBitmap   bitmap;
	bitmap.LoadBitmap(IDB_BACKGROUND1);
	CBrush   brush;
	brush.CreatePatternBrush(&bitmap);
	CBrush*   pOldBrush = dc.SelectObject(&brush);
	dc.Rectangle(0, 0, 480, 800);
	dc.SelectObject(pOldBrush);
	SetWindowPos(&wndTopMost, 0, 0, 480, 800, SWP_SHOWWINDOW);
}


void CInstructionDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CTime time;
	switch (nIDEvent)
	{
	case 1:
		time = CTime::GetCurrentTime();
		m_bar.SetPaneText(2, time.Format("%H:%M:%S"), RGB(255, 255, 255));
		break;
	case 2:
		if (m_commStatus)
		{
			m_bar.SetPaneText(1, "串口通讯正常", RGB(255, 255, 255));
			m_commStatus = false;
		}
		else
		{
			m_bar.SetPaneText(1, "串口通讯异常", RGB(255, 255, 255));
		}
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}
