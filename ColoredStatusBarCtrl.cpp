// ColoredStatusBarCtrl.cpp : implementation file
//

#include "stdafx.h"
//#include "dlgviewtest.h"
#include "ColoredStatusBarCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColoredStatusBarCtrl

CColoredStatusBarCtrl::CColoredStatusBarCtrl()
{
	m_measStatus = "";
	m_saveResult = "";
	m_commStatus = "";
	m_time = "";
}

CColoredStatusBarCtrl::~CColoredStatusBarCtrl()
{
}


BEGIN_MESSAGE_MAP(CColoredStatusBarCtrl, CStatusBar)
	//{{AFX_MSG_MAP(CColoredStatusBarCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColoredStatusBarCtrl message handlers

void CColoredStatusBarCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// Attach to a CDC object
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);

	dc.SetBkMode(TRANSPARENT);
	//Invalidate();

	// Get the pane rectangle and calculate text coordinates
	CRect rect(&lpDrawItemStruct->rcItem);

	switch (lpDrawItemStruct->itemID) {

	case 0:
		dc.SetTextColor(RGB(0, 0, 0));
		dc.TextOut(rect.left + 2, rect.top, m_measStatus);
		break;

	case 1:
		dc.SetTextColor(RGB(0, 0,0));
		dc.TextOut(rect.left + 2, rect.top, m_commStatus);
		break;

	case 2:
		dc.SetTextColor(RGB(0, 200, 200));
		dc.TextOut(rect.left + 2, rect.top, m_saveResult);
		break;

	case 3:
		dc.SetTextColor(RGB(0, 0, 0));
		dc.TextOut(rect.left + 2, rect.top, m_time);
		break;
	}

	// Detach from the CDC object, otherwise the hDC will be
	// destroyed when the CDC object goes out of scope
	dc.Detach();
}
