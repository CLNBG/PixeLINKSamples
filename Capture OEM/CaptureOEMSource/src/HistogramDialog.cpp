/**********************************************************************************************************
 * COPYRIGHT © 2003 PixeLINK.  ALL RIGHTS RESERVED.                                                       *
 *                                                                                                        *
 * Copyright Notice and Disclaimer of Liability:                                                          *
 *                                                                                                        *
 * Liability, distribution and use of this software is governed by the PixeLINK Software Agreement        *
 * distributed with this product.                                                                         *
 *                                                                                                        *
 * In short:                                                                                              *
 *                                                                                                        *
 * PixeLINK hereby explicitly prohibits any form of reproduction (with the express strict exception       *
 * for backup and archival purposes, which are allowed as stipulated within the License Agreement         *
 * for PixeLINK Software), modification, or distribution of this software or its associated               *
 * documentation unless explicitly specified in a written agreement signed by both parties.               *
 *                                                                                                        *
 * To the extent permitted by law, PixeLINK disclaims all other warranties or conditions of any kind,     *
 * either express or implied, except as stated in the PixeLINK Software Agreement or by written           *
 * consent of all parties.  Other written or oral statements by PixeLINK, its representatives, or         *
 * others do not constitute warranties or conditions of PixeLINK.  The Purchaser hereby agree to rely     *
 * on the software, associated hardware and documentation and results stemming from the use thereof       *
 * solely at their own risk.                                                                              *
 *                                                                                                        *
 * By using the products(s) associated with the software (or the software itself) the Purchaser           *
 * and/or user(s) agree(s) to abide by the terms and conditions set forth within this document, as        *
 * well as, respectively, any and all other documents issued by PixeLINK in relation to the               *
 * product(s).                                                                                            *
 *                                                                                                        *
 * For more information, review the PixeLINK Software Agreement distributed with this product or          *
 * contact sales@pixelink.com.                                                                            *
 *                                                                                                        *
 **********************************************************************************************************/

// HistogramDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include ".\HistogramDialog.h"
#include "Helpers.h"
#include <time.h>


// Swap bytes and shift right to get a value in the range [0,1023]
inline U16 SwapAndShiftU16(U16 val)
{
	return ((val & 0xFF) << 2) | ((val & 0xFF00) >> 14);
}


/**
* Function: HistogramDataCallback
* Purpose:  This function is set as the OVERLAY_FRAME callback while the
*           histogram window is open.
*/
U32 __stdcall HistogramDataCallback(
							HANDLE hCamera,
							LPVOID pFrameData,
							U32 uDataFormat,
							PFRAME_DESC pFrameDesc,
							LPVOID pContext)
{
	CHistogramDialog* pDlg = static_cast<CHistogramDialog*>(pContext);

	if (!pDlg->m_pLock->Lock(0))
	{
		// We were not able to acquire the lock immediately.
		// That's OK - we just won't process this frame.
		return ApiSuccess;
	}
	// else:
	// We have acquired a lock on the Mutex. It is safe to access the members,
	// of the HistogramDialog object, because the main thread cannot delete it
	// until after we release the lock.

	long curtime = clock();
	long elapsedtime = curtime - pDlg->m_startframetime;
	int elapsedframes = pFrameDesc->uFrameNumber - pDlg->m_startframe;
	if (elapsedtime >= 50 && elapsedframes >= 5)
	{
		pDlg->m_rate = static_cast<double>(CLOCKS_PER_SEC) * elapsedframes / elapsedtime;
		pDlg->m_startframetime = curtime;
		pDlg->m_startframe = pFrameDesc->uFrameNumber;
	}
	else if (elapsedframes < 0)
	{
		// Stream has been restarted
		pDlg->m_rate = 0.0;
		pDlg->m_startframetime = curtime;
		pDlg->m_startframe = pFrameDesc->uFrameNumber;
	}


	std::vector<long>& data = pDlg->m_histogramData; 

	int dec = static_cast<int>(pFrameDesc->Decimation.fValue);
	int nPixels = DEC_SIZE(static_cast<int>(pFrameDesc->Roi.fWidth), dec)
				* DEC_SIZE(static_cast<int>(pFrameDesc->Roi.fHeight), dec);

	if (uDataFormat == PIXEL_FORMAT_MONO8)
	{
		data.resize(256);
		std::fill(data.begin(), data.end(), 0L);
		U8* pData = static_cast<U8*>(pFrameData);
		for (int i = 0; i < nPixels; i++)
			++data[pData[i]];
	}
	else if (uDataFormat == PIXEL_FORMAT_MONO16)
	{
		data.resize(1024);
		std::fill(data.begin(), data.end(), 0L);
		U16* pData = static_cast<U16*>(pFrameData);
		for (int i = 0; i < nPixels; i++)
			++data[SwapAndShiftU16(pData[i])];
	}

	pDlg->m_pLock->Unlock();

	return ApiSuccess;
}


// CHistogramDialog dialog

IMPLEMENT_DYNAMIC(CHistogramDialog, CDialog)

/**
* Function: CHistogramDialog
* Purpose:  Constructor
*/
CHistogramDialog::CHistogramDialog(CPxLCamera* pCam)
	: CDialog(CHistogramDialog::IDD)
	, m_pCam(pCam)
	, m_histogramData(256, 0L)
	, m_pLock(new CMutex)
	, m_bDataUpdated(true)
	, m_startframetime(0L)
	, m_rate(0.0)
	, m_startframe(0)
	, m_hScale(256)
{
	m_pCam->m_pHistogramWindow = this;
}


/**
* Function: ~CHistogramDialog
* Purpose:  Destructor
*/
CHistogramDialog::~CHistogramDialog()
{
}


BEGIN_MESSAGE_MAP(CHistogramDialog, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_HSCALEPLUS_BUTTON, OnBnClickedHscaleplusButton)
	ON_BN_CLICKED(IDC_HSCALEMINUS_BUTTON, OnBnClickedHscaleminusButton)
END_MESSAGE_MAP()


/**
* Function: DoDataExchange
* Purpose:  
*/
void CHistogramDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HD_HISTOGRAMCTRL, m_HistogramCtrl);
	DDX_Control(pDX, IDC_HSCALE_EDIT, m_HScaleEdit);
}


/**
* Function: OnInitDialog
* Purpose:  
*/
BOOL CHistogramDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// We will repaint the histogram 10 times per second, regardless of the 
	// frame rate. We could call InvalidateRect in the callback itself, and
	// repaint for each frame, but when the frame rate is very high, this would
	// just be too much - 10 times per second is fast enough.
	if ((m_TimerId = SetTimer(HISTOGRAM_DLG_TIMER_ID, 100, NULL)) == 0)
		return FALSE;

	SetIntText(m_HScaleEdit, m_hScale);

	try
	{
		m_pCam->SetStreamState(START_STREAM);
		m_pCam->SetCallback(OVERLAY_FRAME, this, HistogramDataCallback);
	}
	catch (PxLException const& e)
	{
		m_pCam->OnApiError(e);
		return FALSE;
	}

	// Set the dialog's title bar to show which camera it is connected to.
	CString s;
	s.Format(_T("Histogram - s/n %lu"), m_pCam->GetSerialNumber());
	this->SetWindowText(s);

	return TRUE;
}


/**
* Function: OnCancel
* Purpose:  
*/
void CHistogramDialog::OnCancel()
{
	try
	{
		m_pCam->SetCallback(OVERLAY_FRAME, NULL, NULL);
		m_pCam->SetStreamState(STOP_STREAM);
		m_pCam->m_pHistogramWindow = NULL;
	}
	catch (PxLException const&)
	{
		// Ignore. This happens when this function is called from the
		// camera object's Uninitialize method - m_handle has already been set
		// to NULL, so the call to SetCallback above fails.
		//m_pCam->OnApiError(e);
		//return;
	}

	// Stop updating the screen.
	KillTimer(m_TimerId);

	// Make copies of the member data that we access after "delete this".
	CMutex* pLock = m_pLock;
	//HANDLE quitEventHandle = m_quitEvent;

	// Lock the Mutex, which prevents the callback thread from accessing any
	// data members of this object.
	pLock->Lock();

	// Signal the thread to quit.
	//SetEvent(m_quitEvent);

	CDialog::OnCancel();

	// With the Mutex locked, we can be sure (I think) that the callback thread
	// will not try to access any member data (eg the vector or the histogram
	// control), so it's safe to delete them.
	delete this;

	pLock->Unlock();

	//DeleteCriticalSection(pCritSec);
	delete pLock;
	//CloseHandle(quitEventHandle);

}


/**
* Function: OnTimer
* Purpose:  
*/
void CHistogramDialog::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == HISTOGRAM_DLG_TIMER_ID)
	{
		if (m_hScale > m_histogramData.size())
		{
			m_hScale = m_histogramData.size();
			SetIntText(m_HScaleEdit, m_hScale);
		}

		m_pLock->Lock();
		//m_HistogramCtrl.SetHistogramData(m_histogramData.size(), &m_histogramData[0]);
		m_HistogramCtrl.SetHistogramData(m_hScale, &m_histogramData[0]);

		CString s;
		s.Format(_T("Histogram - s/n %lu (%7.03f fps)"), m_pCam->GetSerialNumber(), this->m_rate);
		this->SetWindowText(s);

		m_pLock->Unlock();
	}

	CDialog::OnTimer(nIDEvent);
}


void CHistogramDialog::OnBnClickedHscaleplusButton()
{
	m_hScale = min(1024, m_hScale*2);
	SetIntText(m_HScaleEdit, m_hScale);
}

void CHistogramDialog::OnBnClickedHscaleminusButton()
{
	m_hScale = max(8, m_hScale/2);
	SetIntText(m_HScaleEdit, m_hScale);
}
