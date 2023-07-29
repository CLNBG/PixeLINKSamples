// TimeLapseCaptureDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "TimeLapseCaptureDialog.h"
#include "ImagesGui.h"
#include "Helpers.h"


// CTimeLapseCaptureDialog dialog

IMPLEMENT_DYNAMIC(CTimeLapseCaptureDialog, CDialog)

CTimeLapseCaptureDialog::CTimeLapseCaptureDialog(CImagesGui* pParent)
	: CDialog(CTimeLapseCaptureDialog::IDD, pParent)
	, m_parent(pParent)
	, m_captured(0)
    , m_capturesInProgress(0)
	, m_tocapture(0)
	, m_starttime(0)
	, m_duration(0)
	, m_lastframetime(0)
	, m_bLimitByFrameCount(false)
	, m_streamstate(NULL)
{
}

CTimeLapseCaptureDialog::~CTimeLapseCaptureDialog()
{
}

void CTimeLapseCaptureDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TLCD_PROGRESS, m_Progress);
	DDX_Control(pDX, IDC_TLCD_LABEL1, m_Label1);
	DDX_Control(pDX, IDC_TLCD_LABEL2, m_Label2);
	DDX_Control(pDX, IDC_TLCD_LABEL3, m_Label3);
}


BEGIN_MESSAGE_MAP(CTimeLapseCaptureDialog, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CTimeLapseCaptureDialog message handlers

BOOL CTimeLapseCaptureDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_Progress.SetRange(0, 100);
	
	// Set up the timers to tick at the right interval.
	int n = WindowTextToInt(m_parent->m_CaptureEveryEdit);
	int m = m_parent->m_CaptureEveryCombo.GetItemData(m_parent->m_CaptureEveryCombo.GetCurSel());

	double check = (double)n * (double)m;
	if (check > 0x7FFFFFFF)
	{
		// Interval needs to fit in a 32 bit number.
		AfxMessageBox(_T("Error: Interval is too long (maximum ~24 days)"));
		this->PostMessage(WM_CLOSE, 0, 0);
		return TRUE;
	}

	m_interval = m * n;
	if (m_interval == 0)
	{
		//AfxMessageBox(_T("Error: Interval cannot be 0."));
		//this->PostMessage(WM_CLOSE, 0, 0);
		//return TRUE;
		m_interval = 1; // 1 ms - i.e. Capture as fast as possible.
	}

	// Figure out how long to go for. This can be either a duration in MS,
	// or a total number of frames to capture.
	n = WindowTextToInt(m_parent->m_CaptureForEdit);
	m = m_parent->m_CaptureForCombo.GetItemData(m_parent->m_CaptureForCombo.GetCurSel());

	m_bLimitByFrameCount = (m == -1); // "Frames" is selected.
	if (m_bLimitByFrameCount)
	{
		// Total number of frames to capture:
		m_tocapture = n;
	}
	else
	{
		m_duration = (double)n * m;
	}

	m_starttime = GetTickCount();

	// Make sure the stream is started. We start it now and leave it running
	// until all the captures are done, rather than re-starting it for each
	// image capture.  This way the user could, for example, use descriptors
	// and Software Trigger mode to cycle through them.
	m_streamstate = std::auto_ptr<CStreamState>(new CStreamState(m_parent->GetActiveCamera(), START_STREAM));


	// Capture the first frame right away.
	this->OnTimer(TIME_LAPSE_TIMER_ID);

	// Now start the timer to capture the second frame after the specified
	// interval has elapsed.
	// The first timer triggers an image capture each time it goes off.
	this->SetTimer(TIME_LAPSE_TIMER_ID, m_interval, NULL);
	// The second timer is a more frequent timer that is used to update
	// the GUI (eg: display "Captured 11 of 20 frames..." or whatever).
	if (m_interval > 1500)
		this->SetTimer(TIME_LAPSE_TIMER2_ID, 1000, NULL); // 1/2 second interval

	return TRUE;
}


void CTimeLapseCaptureDialog::updateGUI()
{
	// Check whether it's time to quit.
	if (m_bLimitByFrameCount && m_captured == m_tocapture)
	{
		// We're done.
		this->PostMessage(WM_CLOSE, 0, 0);
	}
	if (!m_bLimitByFrameCount && ((m_lastframetime + m_interval) > (m_starttime + m_duration)))
	{
		// We won't make it to the next capture before timing out, so quit now.
		this->PostMessage(WM_CLOSE, 0, 0);
	}

	CString str;
	DWORD elapsed; // Bugzilla.643 -- time remaining could be represented as negative
	long time, h, m, s;
	double pct_done(0.0);

	if (m_bLimitByFrameCount)
	{
		str.Format(_T("Captured %d of %d frames."), m_captured, m_tocapture);
		m_Label1.SetWindowText(str);
		m_Label2.SetWindowText(_T(""));
	}
	else
	{
		str.Format(_T("%d"), m_captured);
		m_Label1.SetWindowText(str);

		// Time remaining (seconds)
		elapsed = GetTickCount() - m_starttime;
		time = (long)((m_duration - elapsed) / 1000);
		h = time / (60*60);
		m = (time % (60*60)) / 60;
		s = time % 60;
		str.Format(_T("%02d:%02d:%02d"), h, m, s);
		m_Label2.SetWindowText(str);

		pct_done = 100 * ((m_duration  - (1000 * time)) / m_duration);
	}

	elapsed = GetTickCount() - m_lastframetime;
	time = (m_interval - elapsed) / 1000;
	h = time / (60*60);
	m = (time % (60*60)) / 60;
	s = time % 60;
	str.Format(_T("%02d:%02d:%02d"), h, m, s);
	m_Label3.SetWindowText(str);

	if (m_bLimitByFrameCount)
	{
		double pct_per_frame = 100.0 / m_tocapture;
		pct_done = (m_captured * pct_per_frame)
					+ pct_per_frame * ((m_interval-(1000*time)) / (double)m_interval);
	}

	m_Progress.SetPos((int)pct_done);
}


void CTimeLapseCaptureDialog::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIME_LAPSE_TIMER_ID)
	{
        // Bugzilla.1148
        //      If the DoCapture takes longer than the m_interval, then we will fall behind.  This is
        //      most typically seen if we are also previewing, as launcing the preview task can be 
        //      very time consuming.  In any event, we want to limit how far behing we want to fall, 
        //      as we cannot let it grow indefinitly.
        if (++m_capturesInProgress > 5) {
            // We are falling behind -- skip this interval
            return;
        }
        // Grab the next image
        bool captureOk = m_parent->DoCapture();
        --m_capturesInProgress;
		if (!captureOk)
		{
            // Capture failed. Abort.
			this->PostMessage(WM_CLOSE, 0, 0);
			return;
		}
		m_lastframetime = GetTickCount();
		++m_captured;
		updateGUI();
	}
	else if (nIDEvent == TIME_LAPSE_TIMER2_ID)
	{
		// Do periodic GUI updates.
		updateGUI();
	}
}


void CTimeLapseCaptureDialog::OnCancel()
{
	if (m_interval > 0)
	{
		KillTimer(m_timer1Id);
		KillTimer(m_timer2Id);
	}
	// else: we never started the timers, so don't need to Kill them.

	CDialog::OnCancel();
}




















