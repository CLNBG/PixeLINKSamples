#pragma once
#include "afxwin.h"

//   Splash Screen class
class CSplashWnd : public CWnd
{
protected:
	CSplashWnd();
	virtual ~CSplashWnd();

protected:
	static BOOL        s_bShowSplashWnd; // TRUE if the splash screen is enabled.
	static CSplashWnd* s_pSplashWnd;     // Points to the splash screen.
	CBitmap            m_bitmap;         // Splash screen image.
public:

	static BOOL ShowSplashScreen(UINT uTimeOut, UINT uBitmapID, CWnd* pParentWnd = NULL);
	static void EnableSplashScreen(BOOL bEnable = TRUE);
	static BOOL PreTranslateAppMessage(MSG* pMsg);

protected:

	void HideSplashScreen();

	//{{AFX_VIRTUAL(CSplashWnd)
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CSplashWnd)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
