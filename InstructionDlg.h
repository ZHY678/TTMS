#pragma once


// CInstructionDlg �Ի���

class CInstructionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CInstructionDlg)

public:
	CInstructionDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CInstructionDlg();

	// �Ի�������
	enum { IDD = IDD_INSTRUCTION_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnBnClickedBack();
	CInstructionDlg  *m_pInstructionDlg;
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();

	bool m_commStatus = false;

	CStatusBar  m_bar;

	CString m_SDbattery;
};
