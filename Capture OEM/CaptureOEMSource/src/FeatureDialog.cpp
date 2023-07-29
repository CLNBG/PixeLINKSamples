// FeatureDialog.cpp

#include "stdafx.h"
#include "PxLDevApp.h"
#include "FeatureDialog.h"
#include <afxpriv.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/******************************************************************************
* Class CFeatureDialog
******************************************************************************/


IMPLEMENT_DYNAMIC(CFeatureDialog, CDialog)


/**
* Function: CFeatureDialog
* Purpose:  Constructor
*/
CFeatureDialog::CFeatureDialog(UINT nID, CWnd* pParent /*=NULL*/)
	: CDialog(nID, pParent)
	, m_bAdjusting(false)
{
}


/**
* Function: ~CFeatureDialog
* Purpose:  Destructor
*/
CFeatureDialog::~CFeatureDialog()
{
}


/**
* Function: DoDataExchange
* Purpose:  
*/
void CFeatureDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFeatureDialog, CDialog)
	ON_MESSAGE_VOID(WM_KICKIDLE, OnKickIdle)
END_MESSAGE_MAP()


/**
* Function: OnKickIdle
* Purpose:  
*/
void CFeatureDialog::OnKickIdle(void)
{
	UpdateDialogControls(this, FALSE);
}


/**
* Function: OnInitDialog
* Purpose:  
*/
BOOL CFeatureDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Determine where on the screen to display the dialog.
	CRect rctThis, rctLoc;
	int desktopWidth = ::GetSystemMetrics(SM_CXMAXIMIZED);
	int desktopHeight= ::GetSystemMetrics(SM_CYMAXIMIZED);
	GetWindowRect(&rctThis);
	// The window that defines where on the screen this dialog should appear:
	m_location->GetWindowRect(&rctLoc); 

	// Make sure that we are not partially off-screen.
	CPoint ptInitPos(rctLoc.left, rctLoc.top);
	if (ptInitPos.x + rctThis.Width() > desktopWidth)
		ptInitPos.x = desktopWidth - rctThis.Width();
	if (ptInitPos.y + rctThis.Height() > desktopHeight)
		ptInitPos.y = desktopHeight - rctThis.Height();

	SetWindowPos(NULL, ptInitPos.x, ptInitPos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// Call virtual functions to configure and populate the dialog.
	Configure();
	Populate();

	return TRUE;
}


/**
* Function: OnApiError
* Purpose:  General purpose error handler.
*/
void CFeatureDialog::OnApiError(PxLException const& e, bool cameraRelated)
{
	m_pCam->OnApiError(e, cameraRelated);
}






