// Creminder.cpp : 实现文件
//

#include "stdafx.h"
#include "StereoCamera.h"
#include "Creminder.h"
#include "afxdialogex.h"


// Creminder 对话框

IMPLEMENT_DYNAMIC(Creminder, CDialogEx)

Creminder::Creminder(CWnd* pParent /*=NULL*/)
	: CDialogEx(Creminder::IDD, pParent)
{

}

Creminder::~Creminder()
{
}

void Creminder::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Creminder, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// Creminder 消息处理程序


BOOL Creminder::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CEdit *m_reminder = (CEdit*)GetDlgItem(IDC_REMINDER);
	font.CreatePointFont(180, "黑体");
	m_reminder->SetFont(&font);
	SetTimer(20, 2000, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void Creminder::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	this->PostMessage(WM_CLOSE, NULL, NULL);
	/*CWnd* hWnd = FindWindow(NULL, _T(""));
	if (hWnd)
	{
		this->PostMessage(WM_CLOSE, NULL, NULL);
	}*/
	KillTimer(20);

	CDialogEx::OnTimer(nIDEvent);
}
