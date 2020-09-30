// RegisterDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "StereoCamera.h"
#include "RegisterDlg.h"
#include "afxdialogex.h"
#include "WizardDlg.h"


// CRegisterDlg �Ի���

IMPLEMENT_DYNAMIC(CRegisterDlg, CDialogEx)

CRegisterDlg::CRegisterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRegisterDlg::IDD, pParent)
	, m_Name(_T(""))
	, m_Num1(_T(""))
	, m_Num2(_T(""))
	, m_Num3(_T(""))
	, m_Num4(_T(""))
{

}

CRegisterDlg::~CRegisterDlg()
{
}

void CRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_Name);
	DDX_Text(pDX, IDC_EDIT2, m_Num1);
	DDV_MaxChars(pDX, m_Num1, 5);
	DDX_Text(pDX, IDC_EDIT3, m_Num2);
	DDV_MaxChars(pDX, m_Num2, 5);
	DDX_Text(pDX, IDC_EDIT4, m_Num3);
	DDV_MaxChars(pDX, m_Num3, 5);
	DDX_Text(pDX, IDC_EDIT5, m_Num4);
	DDV_MaxChars(pDX, m_Num4, 5);
	DDX_Control(pDX, IDC_EDIT2, m_Edit2);
	DDX_Control(pDX, IDC_EDIT3, m_Edit3);
	DDX_Control(pDX, IDC_EDIT4, m_Edit4);
	DDX_Control(pDX, IDC_EDIT5, m_Edit5);
	DDX_Control(pDX, IDOK, m_Advance);
	DDX_Control(pDX, IDCANCEL, m_Backoff);
}


BEGIN_MESSAGE_MAP(CRegisterDlg, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CRegisterDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CRegisterDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT2, &CRegisterDlg::OnEnChangeEdit2)
	ON_EN_CHANGE(IDC_EDIT3, &CRegisterDlg::OnEnChangeEdit3)
	ON_EN_CHANGE(IDC_EDIT4, &CRegisterDlg::OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT5, &CRegisterDlg::OnEnChangeEdit5)
	//ON_WM_HOTKEY()
	ON_WM_DRAWCLIPBOARD()
	ON_WM_CHANGECBCHAIN()
	ON_EN_CHANGE(IDC_EDIT1, &CRegisterDlg::OnEnChangeEdit1)
END_MESSAGE_MAP()


// CRegisterDlg ��Ϣ�������


void CRegisterDlg::OnBnClickedCancel()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CDialog::OnOK();
	CWizardDlg dlg;
	dlg.DoModal();

	//CDialogEx::OnCancel();
}


void CRegisterDlg::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	if (m_Name.IsEmpty())
	{
		MessageBox("����д�û�����");
		return;
	}
	if (m_Name.IsEmpty() || m_Num1.IsEmpty() || m_Num2.IsEmpty()
		|| m_Num3.IsEmpty() || m_Num4.IsEmpty())
	{
		MessageBox("ע�������");
		return;
	}
	HKEY key;
	char data[32];
	DWORD size = 32;
	DWORD type = REG_SZ;
	CString skey = "Software\\mingrisoft";
	long iret = RegOpenKeyEx(HKEY_CURRENT_USER, skey,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, &key);
	if (iret == 0)
	{
		CString value;
		iret = RegQueryValueEx(key, "regnum", 0, &type, (BYTE*)data, &size);
		CString text = data;
		int index = text.Find("-");
		if (iret == 0)
		{
			if (text.Left(index) == m_Name && text.Mid(index + 1, 5) == m_Num1 &&
				text.Mid(index + 6, 5) == m_Num2 && text.Mid(index + 11, 5) == m_Num3
				&& text.Mid(index + 16, 5) == m_Num4)
			{
				Flag = TRUE;
				RegSetValueEx(key, "isreg", 0, REG_SZ, (BYTE*)"1", 1);
			}
			else
			{
				MessageBox("�û�����ע�������");
				return;
			}
		}
		else
		{
			RegSetValueEx(key, "regnum", 0, REG_SZ, (BYTE*)"0", 1);
		}
	}
	CDialogEx::OnOK();
}


void CRegisterDlg::OnEnChangeEdit2()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	if (m_Num1.GetLength() == 5)
		GetDlgItem(IDC_EDIT3)->SetFocus();
}


void CRegisterDlg::OnEnChangeEdit3()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	if (m_Num2.GetLength() == 5)
		GetDlgItem(IDC_EDIT4)->SetFocus();
}


void CRegisterDlg::OnEnChangeEdit4()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	if (m_Num3.GetLength() == 5)
		GetDlgItem(IDC_EDIT5)->SetFocus();
}


void CRegisterDlg::OnEnChangeEdit5()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	if (m_Num4.GetLength() >= 5)
	{
		m_Num4 = m_Num4.Left(5);
	}
	UpdateData(FALSE);
}


BOOL CRegisterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	GetDlgItem(IDC_EDIT2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT4)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT5)->ShowWindow(SW_HIDE);
	RegisterHotKey(this->GetSafeHwnd(), HOTKEY_PASTE, MOD_CONTROL, 0x0056);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}


void CRegisterDlg::OnHotKey(WPARAM wParam, LPARAM lParam)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (HOTKEY_PASTE == (int)wParam)
	{
		PasteReg();
	}

	//CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}


BOOL CRegisterDlg::DestroyWindow()
{
	// TODO:  �ڴ����ר�ô����/����û���
	UnregisterHotKey(m_hWnd, HOTKEY_PASTE);
	return 1;

	//return CDialogEx::DestroyWindow();
}

void CRegisterDlg::PasteReg()
{
	hwnd = SetClipboardViewer();
	Sleep(100);
	ChangeClipboardChain(hwnd);
}


void CRegisterDlg::OnDrawClipboard()
{
	CDialogEx::OnDrawClipboard();

	// TODO:  �ڴ˴������Ϣ����������
	CString edit, str;
	OpenClipboard();
	if (IsClipboardFormatAvailable(CF_TEXT))
	{
		HANDLE hmem = ::GetClipboardData(CF_TEXT);
		char*data = (char*)GlobalLock(hmem);
		str = data;
		if (m_Once)
			GetDlgItem(IDC_EDIT1)->SetWindowText(str);
		else
		{
			int index = str.Find("-");
			if (index == -1 || str.GetLength() != 23)
			{
				MessageBox("ע�����ʽ����ȷ��");
				return;
			}
			else
			{
				int i = 0;
				while (index != -1)
				{
					edit = str.Left(5);
					GetDlgItem(IDC_EDIT2 + i++)->SetWindowText(edit);
					str = str.Right(str.GetLength() - 6);
					index = str.Find("-");
				}
				edit = str;
				GetDlgItem(IDC_EDIT2 + i)->SetWindowText(edit);
			}
		}
	}
	CloseClipboard();
	::SendMessage(hwnd, WM_DRAWCLIPBOARD, 0, 0);
}


void CRegisterDlg::OnChangeCbChain(HWND hWndRemove, HWND hWndAfter)
{
	CDialogEx::OnChangeCbChain(hWndRemove, hWndAfter);

	// TODO:  �ڴ˴������Ϣ����������
	if (hwnd == hWndRemove)
		hwnd = hWndAfter;
	::SendMessage(hwnd, WM_CHANGECBCHAIN, (WPARAM)hWndRemove, (LPARAM)hWndAfter);
}


void CRegisterDlg::OnEnChangeEdit1()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	GetDlgItem(IDC_EDIT2)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT3)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT4)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT5)->ShowWindow(SW_SHOW);
	m_Once = FALSE;
}
