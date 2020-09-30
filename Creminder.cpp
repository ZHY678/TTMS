// Creminder.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "StereoCamera.h"
#include "Creminder.h"
#include "afxdialogex.h"


// Creminder �Ի���

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


// Creminder ��Ϣ�������


BOOL Creminder::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CEdit *m_reminder = (CEdit*)GetDlgItem(IDC_REMINDER);
	font.CreatePointFont(180, "����");
	m_reminder->SetFont(&font);
	SetTimer(20, 2000, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}


void Creminder::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	this->PostMessage(WM_CLOSE, NULL, NULL);
	/*CWnd* hWnd = FindWindow(NULL, _T(""));
	if (hWnd)
	{
		this->PostMessage(WM_CLOSE, NULL, NULL);
	}*/
	KillTimer(20);

	CDialogEx::OnTimer(nIDEvent);
}
