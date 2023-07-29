// ClipCaptureDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "ClipCaptureDialog.h"
#include "Helpers.h"


// CClipCaptureDialog dialog

IMPLEMENT_DYNAMIC(CClipCaptureDialog, CDialog)


/**
* Function: CClipCaptureDialog
* Purpose:  
*/
CClipCaptureDialog::CClipCaptureDialog(CPxLCameraDoc* pDoc,
									   HANDLE captureDoneEventHandle)
	: CDialog(CClipCaptureDialog::IDD)
	, m_pDoc(pDoc)
	, m_CaptureDoneEventHandle(captureDoneEventHandle)
	, m_FrameRate(0)
	, m_NumFrames(0)
{
}


/**
* Function: CClipCaptureDialog
* Purpose:  
*/
CClipCaptureDialog::~CClipCaptureDialog()
{
	SetEvent(m_DialogClosedEventHandle);
}


BEGIN_MESSAGE_MAP(CClipCaptureDialog, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


/**
* Function: DoDataExchange
* Purpose:  
*/
void CClipCaptureDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLIPCAPTURE_PROGRESS, m_ClipCaptureProgressBar);
}


/**
* Function: OnInitDialog
* Purpose:  
*/
BOOL CClipCaptureDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_StartTime = ::GetTickCount();
	m_TimerId = SetTimer(CLIP_CAPTURE_DLG_TIMER_ID, 100, NULL);
	m_ClipCaptureProgressBar.SetRange(0, m_NumFrames);
	m_bClipCancelledByUser = false;

	return TRUE;
}


// CClipCaptureDialog message handlers

/**
* Function: OnBnClickedCancel
* Purpose:  
*/
void CClipCaptureDialog::OnBnClickedCancel()
{
	// Make a note that the clip capture has been explicitly cancelled
	// by the user. This way, the document knows that the capture was aborted
	// intentionally, and will not display an error message about the stream
	// having stopped unexpectedly.
	m_bClipCancelledByUser = true;

	OnCancel();
}


/**
* Function: OnCancel
* Purpose:  
*/
void CClipCaptureDialog::OnCancel()
{
	// Stop the timer that is used to update the progress bar.
	if (m_TimerId != 0)
		KillTimer(m_TimerId);
	m_TimerId = 0;

	// Return IDCANCEL, indicating that the user is attempting to cancel the
	// clip capture.
	CDialog::OnCancel();
}


/**
* Function: OnTimer
* Purpose:  Update the progress bar.
*           We only show an estimate of the progress, based on the frame rate,
*           the time elapsed, and the number of frames to be captured.
*/
void CClipCaptureDialog::OnTimer(UINT_PTR nIDEvent)
{
	const double correction_factor = 0.75;

	if (CLIP_CAPTURE_DLG_TIMER_ID == nIDEvent)
	{
		// Update the progress bar.
		DWORD curTime = ::GetTickCount();
		int estCaptured = static_cast<int>(((curTime - m_StartTime)/1000.0) * m_FrameRate * correction_factor);
		m_ClipCaptureProgressBar.SetPos(min(estCaptured, m_NumFrames - 1));

		// Check to see whether the termination function has set the event
		// to signal us that the clip capture has completed.
		if (WAIT_OBJECT_0 == ::WaitForSingleObject(m_CaptureDoneEventHandle, 0))
		{
			// Set the progress to 100% (although I doubt anyone will have time to see it..)
			m_ClipCaptureProgressBar.SetPos(m_NumFrames);
			// Close this dialog, returning IDOK.
			this->OnOK();
		}
	}

	CDialog::OnTimer(nIDEvent);
}


BOOL CClipCaptureDialog::PreTranslateMessage(MSG* pMsg)
{
	// Prevent Esc key from closing the dialog - you have to use the "Cancel" button.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}









