// ImageCaptureDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "ImageCaptureDialog.h"
#include "ImagesGui.h"
#include "Helpers.h"


// CClipCaptureDialog dialog

IMPLEMENT_DYNAMIC(CImageCaptureDialog, CDialog)


/**
* Function: CImageCaptureDialog
* Purpose:  
*/
CImageCaptureDialog::CImageCaptureDialog(CImagesGui* pParent)
	: CDialog(CImageCaptureDialog::IDD)
	, m_parent(pParent)
	, m_ImageCaptureThread(NULL)
	, m_ImageCaptured(FALSE)
{
}


/**
* Function: CImageCaptureDialog
* Purpose:  
*/
CImageCaptureDialog::~CImageCaptureDialog()
{
    if (NULL != m_ImageCaptureThread)
    {
        CloseHandle(m_ImageCaptureThread);  
    }
}


BEGIN_MESSAGE_MAP(CImageCaptureDialog, CDialog)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


/**
* Function: DoDataExchange
* Purpose:  
*/
void CImageCaptureDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


/**
* Function: OnInitDialog
* Purpose:  
*/
BOOL CImageCaptureDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

    m_SavedStreamState = m_parent->GetActiveCamera()->GetStreamState();
    m_ImageCaptureThread = CreateThread(NULL, 0, ImageCaptureThread, this, 0, NULL);
	
	return TRUE;
}


// CImageCaptureDialog message handlers

/**
* Function: OnBnClickedCancel
* Purpose:  
*/
void CImageCaptureDialog::OnBnClickedCancel()
{
	OnCancel();
}


/**
* Function: OnCancel
* Purpose:  
*/
void CImageCaptureDialog::OnCancel()
{
     int currentState = m_parent->GetActiveCamera()->GetStreamState();
    // If we have not captured an image, but the camera is still streaming, then we need to stop CaptureImageThread
    if (! m_ImageCaptured && (STOP_STREAM != currentState))
    {
        // Stop the stream to cause CaptureImage to exit (with a apiStreamStopped return code).  Note that we cannot
        // use the CStreamState class in this instance, as this stopping of the stream will trigger the ImageCaptreThread
        // to run, and it uses CStreamState within it's CaptureImage->GetNextFrame -- we can't control which order
        // these will occur, so we do one final set of the stream state (if necessary) once the thread is done.
        m_parent->GetActiveCamera()->SetStreamState(STOP_STREAM);
   	    // Wait until the thread has stopped, indicating that we have actually returned from our CaptureImage
   	    WaitForSingleObject(m_ImageCaptureThread, INFINITE);
        if (m_parent->GetActiveCamera()->GetStreamState() != m_SavedStreamState)
        {
   	        m_parent->GetActiveCamera()->SetStreamState(m_SavedStreamState);
   	    }
   	}
   	
	// Return IDCANCEL, indicating that the user is attempting to cancel the
	// image capture.
	CDialog::OnCancel();
}


BOOL CImageCaptureDialog::PreTranslateMessage(MSG* pMsg)
{
	// Prevent Esc key from closing the dialog - you have to use the "Cancel" button.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

// 
// This thread does the actual capture.  It will self terminate once the capture is done (captured an image, or
// DoCapture returns false because the stream was stopped (which may have been induced by the cancel button).
ULONG __stdcall CImageCaptureDialog::ImageCaptureThread(LPVOID context)
{
    CImageCaptureDialog* myClass = (CImageCaptureDialog*)context;

    myClass->m_ImageCaptured = myClass->m_parent->DoCapture();
	myClass->PostMessage(WM_CLOSE,0,0);

    return myClass->m_ImageCaptured;
}











