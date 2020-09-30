#pragma once


// CInstructionDlg 对话框

class CInstructionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CInstructionDlg)

public:
	CInstructionDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CInstructionDlg();

	// 对话框数据
	enum { IDD = IDD_INSTRUCTION_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
