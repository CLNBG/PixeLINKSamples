// MainFrm.cpp : implementation of the CMainFrame class


#include "stdafx.h"
#include "PxLDevApp.h"
#include "MainFrm.h"
#include "PxLDevAppView.h"
#include <dbt.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/******************************************************************************
* CMainFrame
******************************************************************************/

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)


BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_HELPINFO()
	ON_UPDATE_COMMAND_UI(ID_VIEW_ALWAYSONTOP, OnUpdateViewAlwaysOnTop)
	ON_COMMAND(ID_VIEW_ALWAYSONTOP, OnViewAlwaysOnTop)
END_MESSAGE_MAP()


/**
* Function: CMainFrame
* Purpose:  Constructor
*/
CMainFrame::CMainFrame()
{
}


/**
* Function: ~CMainFrame
* Purpose:  Destructor
*/
CMainFrame::~CMainFrame()
{
}


/**
* Function: OnHelpInfo
* Purpose:  
*/
BOOL CMainFrame::OnHelpInfo(HELPINFO *pHelpInfo)
{
	return DevAppHelp(pHelpInfo, NULL);
}


/**
* Function: OnWndMsg
* Purpose:  Application private messages are sent to the Main Window because
*           the Document can't receive messages (it's not a window). We trap
*           the private messages that we want the Document to handle, and
*           forward them to it.
*/
BOOL CMainFrame::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (message == WM_APP_PREVIEW_CLOSED 
		|| message == WM_APP_PREVIEW_ACTIVATED
		|| message == WM_APP_FEATURE_CHANGED
		|| message == WM_DEVICECHANGE
        || message == WM_APP_DEFERRED_DEVICE_CHANGED
        || message == WM_APP_DEVICE_RESET
        || message == WM_APP_DEVICE_DISCONNECTED)
	{
		CPxLCameraDoc* pDoc = static_cast<CPxLCameraDoc*>(GetActiveDocument());
		if (pDoc != NULL)
			pDoc->HandleMessage(message, wParam, lParam);
		return TRUE;
	}
	
	return CFrameWnd::OnWndMsg(message, wParam, lParam, pResult);
}


/**
* Function: OnCreate
* Purpose:  
*/
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	return CFrameWnd::OnCreate(lpCreateStruct);

	this->LoadAccelTable(MAKEINTRESOURCE(IDR_MAINFRAME));
}


/**
* Function: PreCreateWindow
* Purpose:  Called by MFC before the main frame is created.
*/
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.style = 0
			| WS_OVERLAPPED
            | WS_CAPTION
            | WS_SYSMENU
            | WS_THICKFRAME
			| WS_MINIMIZEBOX
            ;

	cs.dwExStyle |= 0
			//| WS_EX_CONTEXTHELP			// Add the ? button to the title bar.
			| WS_EX_CONTROLPARENT		// Allows tabbing between child windows (tabs).
			;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;		// Remove the sunken border around the view area.
	                                        // This makes it look pretty much like a Dialog-based app.

	// Change the window class to one that does not have CS_HREDRAW|CS_VREDRAW. These
	// cause the entire view to be redrawn when the main frame is resized, which
	// causes extreme flickering and is totally unneccesary for this app.
	cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS,
									   AfxGetApp()->LoadStandardCursor(IDC_ARROW),
									   (HBRUSH)(COLOR_WINDOW+1),
									   AfxGetApp()->LoadIcon(IDR_MAINFRAME));

	return TRUE;
}


/**
* Function: OnGetMinMaxInfo
* Purpose:  Called by MFC when the window is resized, allowing us to limit
*           how big or small it is allowed to be.
*/
void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CRect rctWnd;
	GetWindowRect(&rctWnd);
	
	CPxLDevAppView* pView = (CPxLDevAppView*) this->GetActiveView();
	if (pView == NULL) return;

	ASSERT_KINDOF(CPxLDevAppView, pView);

	// Delegate to the view, which knows best how big its containing window 
	// should be.
	pView->SetMinMaxInfo(lpMMI->ptMinTrackSize, lpMMI->ptMaxTrackSize, rctWnd);

	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}


/**
* Function: OnSize
* Purpose:  Called by MFC when the window is resized. We use it to change the
*           size of the call-log list.
*/
void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);
	
	CPxLDevAppView* pView = (CPxLDevAppView*) this->GetActiveView();
	if (pView == NULL) return;

	ASSERT_KINDOF(CPxLDevAppView, pView);

}


/**
* Function: OnUpdateViewAlwaysOnTop
* Purpose:  
*/
void CMainFrame::OnUpdateViewAlwaysOnTop(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theApp.m_bAlwaysOnTop);
}


/**
* Function: OnViewAlwaysOnTop
* Purpose:  
*/
void CMainFrame::OnViewAlwaysOnTop()
{
	theApp.m_bAlwaysOnTop = !theApp.m_bAlwaysOnTop;
	this->SetWindowPos(theApp.m_bAlwaysOnTop ? &wndTopMost : &wndNoTopMost,
					   0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);
}


#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


















