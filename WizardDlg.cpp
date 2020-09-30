// WizardDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "StereoCamera.h"
#include "WizardDlg.h"
#include "afxdialogex.h"
#include "RegisterDlg.h"
//#include "MainDialog.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif
extern BOOL Flag;

// CWizardDlg �Ի���

IMPLEMENT_DYNAMIC(CWizardDlg, CDialogEx)

CWizardDlg::CWizardDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWizardDlg::IDD, pParent)
{

}

CWizardDlg::~CWizardDlg()
{
}

void CWizardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
}


BEGIN_MESSAGE_MAP(CWizardDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CWizardDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CWizardDlg ��Ϣ�������


BOOL CWizardDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	HKEY key;
	char data[4];
	DWORD size = 4;
	DWORD type = REG_SZ;
	CString skey = "Software\\mingrisoft";
	long iret = RegOpenKeyEx(HKEY_CURRENT_USER, skey,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, &key);
	if (iret == 0)
	{
		CString value;
		iret = RegQueryValueEx(key, "tryout", 0, &type, (BYTE*)data, &size);
		if (iret == 0)
		{
			;
		}
		else
		{
			RegSetValueEx(key, "tryout", 0, REG_SZ, (BYTE*)"30", 2);
			OnCancel();
		}
	}
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}


void CWizardDlg::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	INT_PTR nRes;

	CRegisterDlg registerDlg;
	nRes = registerDlg.DoModal();

	if (IDCANCEL == nRes)
	{
		return;
	}

	CDialogEx::OnOK();
}

