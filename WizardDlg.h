#pragma once
#include "afxwin.h"
#include "RegisterDlg.h"


// CWizardDlg �Ի���

class CWizardDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWizardDlg)

public:
	CWizardDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWizardDlg();

// �Ի�������
	enum { IDD = IDD_WIZARD_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CButton m_OK;
	CButton m_Cancel;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
