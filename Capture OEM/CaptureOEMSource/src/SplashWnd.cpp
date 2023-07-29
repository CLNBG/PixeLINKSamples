//
// SplashWnd.cpp
// Based on http://www.microsoft.com/msj/1099/c/c1099.aspx
//
#include "stdafx.h"
#include "SplashWnd.h"

BOOL        CSplashWnd::s_bShowSplashWnd;
CSplashWnd* CSplashWnd::s_pSplashWnd;

CSplashWnd::CSplashWnd()
{
}

CSplashWnd::~CSplashWnd()
{
	ASSERT(s_pSplashWnd == this);
	s_pSplashWnd = NULL;
}

BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
	//{{AFX_MSG_MAP(CSplashWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSplashWnd::EnableSplashScreen(BOOL bEnable /*= TRUE*/)
{
	s_bShowSplashWnd = bEnable;
}

BOOL CSplashWnd::ShowSplashScreen(UINT uTimeOut, UINT uBitmapID, CWnd* pParentWnd /*= NULL*/)
{
	ASSERT(uTimeOut && uBitmapID);
	
	if (!s_bShowSplashWnd || s_pSplashWnd != NULL) {
		return FALSE;
	}

	s_pSplashWnd = new CSplashWnd;

	if (!s_pSplashWnd->m_bitmap.LoadBitmap(uBitmapID)) {
		return FALSE;
	}

	BITMAP bm;
	s_pSplashWnd->m_bitmap.GetBitmap(&bm);

	CString strWndClass = AfxRegisterWndClass(0,
		AfxGetApp()->LoadStandardCursor(IDC_ARROW));

	if (!s_pSplashWnd->CreateEx(0, strWndClass, NULL, WS_POPUP | WS_VISIBLE,
		0, 0, bm.bmWidth, bm.bmHeight, pParentWnd->GetSafeHwnd(), NULL))
	{
		TRACE("Failed to create splash screen.\n");
		delete s_pSplashWnd;
		return FALSE;
	}

	s_pSplashWnd->CenterWindow();
	s_pSplashWnd->UpdateWindow();
	// If this is the release build, make the window topmost so that it doesn't get overdrawn.
#ifndef _DEBUG
	s_pSplashWnd->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
#endif


	// Set a timer to destroy the splash screen.
	s_pSplashWnd->SetTimer(1, uTimeOut, NULL);

	return TRUE;
}

BOOL CSplashWnd::PreTranslateAppMessage(MSG* pMsg)
{
	if (s_pSplashWnd == NULL)
		return FALSE;

	// If we get a keyboard or mouse message, hide the splash screen.
	if (pMsg->message == WM_KEYDOWN ||
	    pMsg->message == WM_SYSKEYDOWN ||
	    pMsg->message == WM_LBUTTONDOWN ||
	    pMsg->message == WM_RBUTTONDOWN ||
	    pMsg->message == WM_MBUTTONDOWN ||
	    pMsg->message == WM_NCLBUTTONDOWN ||
	    pMsg->message == WM_NCRBUTTONDOWN ||
	    pMsg->message == WM_NCMBUTTONDOWN)
	{
		s_pSplashWnd->HideSplashScreen();
		return TRUE;	// message handled here
	}

	return FALSE;	// message not handled
}

void CSplashWnd::HideSplashScreen()
{
	DestroyWindow();
	AfxGetMainWnd()->UpdateWindow();
}

void CSplashWnd::PostNcDestroy()
{
	delete this;
}

void CSplashWnd::OnPaint()
{
	CPaintDC dc(this);

	CDC dcImage;
	if (dcImage.CreateCompatibleDC(&dc))
	{
		BITMAP bm;
		m_bitmap.GetBitmap(&bm);

		CBitmap* pOldBitmap = dcImage.SelectObject(&m_bitmap);
		dc.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcImage, 0, 0, SRCCOPY);
		dcImage.SelectObject(pOldBitmap);
	}
}

void CSplashWnd::OnTimer(UINT_PTR nIDEvent)
{
	HideSplashScreen();
}
