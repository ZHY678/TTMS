#if !defined(AFX_COLOREDSTATUSBARCTRL_H__4D49433F_617D_45A5_A207_5655A0BE779F__INCLUDED_)
#define AFX_COLOREDSTATUSBARCTRL_H__4D49433F_617D_45A5_A207_5655A0BE779F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColoredStatusBarCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColoredStatusBarCtrl window
/*m_bar.m_saveResult = str;
m_bar.GetStatusBarCtrl().SetText("", 1, SBT_OWNERDRAW);*/
class CColoredStatusBarCtrl : public CStatusBar
{
	// Construction
public:
	CColoredStatusBarCtrl();

	// Attributes
public:
	CString m_measStatus;
	CString m_commStatus;
	CString m_saveResult;
	CString m_time;
	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColoredStatusBarCtrl)
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CColoredStatusBarCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColoredStatusBarCtrl)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLOREDSTATUSBARCTRL_H__4D49433F_617D_45A5_A207_5655A0BE779F__INCLUDED_)
