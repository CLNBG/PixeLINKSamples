// AviCaptureDlg.cpp : implementation file
//
//
// A note about the sequence of events
//
// 1) user pressed start to initiate capture of video to pds files
// 2) PxLGetClip is called; the callback function is CaptureTerminatedCallback
// 3) When PxLGetGetClip is done, it calls CaptureTerminatedCallback
//    If we're supposed to keep going, we:
//             send a message to the app that the capture should be updated
//             i.e. send WM_CAPTURE_UPDATE and CAviCaptureDlg::OnCaptureUpdate will be called
// 4) When the user presses Stop, we kick off the stopping process by stopping the camera's stream.
//		This will cause CaptureTerminatedCallback to be called with ApiStoppedStream
//		CaptureTerminatedCallback will send the message WM_CAPTURE_FINISHED to the app, causing
//      CAviCaptureDlg::OnCaptureUpdate to be called. This method cleans up the capture, and then
//      asks if it should start converting. 
// 
// The sequence of events for conversion is similar, but instead of a thread being created in the 
// API by the call to PxLGetClip, we explicitly create our own thread. It will send 
// messages (WM_CONVERSION_FINISHED and WM_CONVERSION_UPDATE) to the app.
// 
// The other events of interest are the timer events. We generate these so that we can do something to the UI
// on a regular basis, to show the user we're not hung.
//

#include "stdafx.h"
#include "AviCapture.h"
#include "AviCaptureDlg.h"
#include ".\avicapturedlg.h"
#include "AviCaptureState.h"
#include "AviConvertState.h"
#include "Utils.h"
#include <vector>
#include <PixeLINKApi.h>

//
// Local helpers
//
static U32 _stdcall		CaptureTerminatedCallback(HANDLE hCamera, U32 numFramesCapture, PXL_RETURN_CODE returnCode);
static UINT		ConvertPdsFilesToAvi(LPVOID lParam);


//
// Want to capture 10 minutes worth of video.
// We'll restrict the individual clips to 30 seconds because it seems there's a problem playing 
// large (>2G) avi files. 
// 
#define SECONDS_PER_CLIP	30
#define NUMBER_OF_CLIPS		20


//
// Recording the state of the capture or conversion
//
static  AviCaptureState*	s_pCurrentCapture		= NULL;
static	AviConvertState*	s_pCurrentConversion	= NULL;


// Events we use to tell the app/doc that something's happened with the capture
#define WM_CAPTURE_FINISHED			(WM_USER+0x101)
#define WM_CAPTURE_UPDATE			(WM_USER+0x102)

// Events we use to tell the app/doc that something's happened with the conversion
#define WM_CONVERSION_FINISHED		(WM_USER+0x103)
#define WM_CONVERSION_UPDATE		(WM_USER+0x104)

// Helper macros for the above message
#define POST_MESSAGE_TO_APP(messageId, wParam, lParam)	do { PostMessage(theApp.GetMainWnd()->GetSafeHwnd(), (messageId), (wParam), (lParam)); } while(0)


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAviCaptureDlg dialog
CAviCaptureDlg::CAviCaptureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAviCaptureDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAviCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CAPTURE, m_btnCapture);
	DDX_Control(pDX, IDC_PROGRESS1, m_ctlProgress);
	DDX_Control(pDX, IDC_TXT_MESSAGE, m_txtMessage);
	DDX_Control(pDX, IDOK, m_btnOK);
}

BEGIN_MESSAGE_MAP(CAviCaptureDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_CAPTURE_UPDATE,		OnCaptureUpdate)
	ON_MESSAGE(WM_CAPTURE_FINISHED,		OnCaptureFinished)
	ON_MESSAGE(WM_CONVERSION_UPDATE,	OnConversionUpdate)
	ON_MESSAGE(WM_CONVERSION_FINISHED,	OnConversionFinished)
	
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CAPTURE, OnBnClickedCapture)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


BOOL 
CAviCaptureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}

	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Initialize our app state to idle
	SetState(STATE_IDLE);

	// Enable the timer which will update the GUI
	SetTimer(1, 250, 0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void 
CAviCaptureDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	} else if ((nID & 0xFFF0) == SC_CLOSE) {
		// User pressed the Close button (X in upper right)
		OnBnClickedOk();
	} else {
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAviCaptureDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAviCaptureDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//
// We've created a timer to generate WM_TIMER events at regular intervals.
// What we do to the UI depends on the current state.
// The main purpose of these events is to change the current message while
// we're doing something, so that the user doesn't think we're hung. 
//
void
CAviCaptureDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString msg;
	static int s_numProgressDots = 0;

	switch(m_state) {
		case STATE_IDLE:
			break;
		case STATE_CAPTURING:	
			ASSERT(NULL != s_pCurrentCapture);
			msg.Format("Capturing to %s ", s_pCurrentCapture->GetCurrentClipFileName());
			break;
		case STATE_CONVERTING:	
			ASSERT(NULL != s_pCurrentConversion);
			if (s_pCurrentConversion->KeepRunning()) {
				msg.Format("Converting %s ", s_pCurrentConversion->GetCurrentFileName());
			} else {
				msg = "Please wait. Cancelling conversion ";
			}
			break;
		default:
			ASSERT(0);
			break;
	}

	if (msg.GetLength() > 0) {
		s_numProgressDots++;
		if (s_numProgressDots > 5) {
			s_numProgressDots = 0;
		}
		for(int i=0; i<s_numProgressDots; i++) {
			msg += ".";
		}
		m_txtMessage.SetWindowText((LPCTSTR)msg);
	}
}

//
// Centralize the changes to the GUI that have to be made when 
// we change from one state to another.
//
void 
CAviCaptureDlg::SetState(State newState)
{
	switch(newState) {
		case STATE_IDLE:
			m_ctlProgress.ShowWindow(SW_HIDE);
			m_txtMessage.SetWindowText("");
			m_txtMessage.ShowWindow(SW_HIDE);
			m_btnCapture.SetWindowText("Start");
			m_btnOK.ShowWindow(SW_SHOWNORMAL);
			m_state = STATE_IDLE;
			break;

		case STATE_CAPTURING:
			ASSERT(NULL != s_pCurrentCapture);
			m_ctlProgress.ShowWindow(SW_SHOWNORMAL);
			m_txtMessage.ShowWindow(SW_SHOWNORMAL);
			m_txtMessage.SetWindowText("Capturing...");
			m_btnCapture.SetWindowText("Stop");
			m_btnOK.ShowWindow(SW_HIDE);
			m_state = STATE_CAPTURING;
			break;

		case STATE_CONVERTING:
			m_ctlProgress.ShowWindow(SW_SHOWNORMAL);
			m_txtMessage.ShowWindow(SW_SHOWNORMAL);
			m_txtMessage.SetWindowText("Converting...");
			m_btnCapture.SetWindowText("Stop");
			m_btnOK.ShowWindow(SW_HIDE);
			m_state = STATE_CONVERTING;
			break;

		default:	
			ASSERT(0);
			break;
	}


}

//
// User pressed the Start/Stop button
//
void 
CAviCaptureDlg::OnBnClickedCapture()
{
	switch(m_state)
	{
		case STATE_IDLE:
			if (StartCapturing()) {
				SetState(STATE_CAPTURING);
				m_ctlProgress.SetRange(0, (short)s_pCurrentCapture->GetNumberOfClips());
				m_ctlProgress.SetPos(1);
			}
			break;

		case STATE_CAPTURING:
			StopCapturing();
			break;

		case STATE_CONVERTING:
			StopConverting();
			break;
		default:
			ASSERT(0);
	}
}

//
// Exit button was pressed. 
//
void 
CAviCaptureDlg::OnBnClickedOk()
{
	switch(m_state)
	{
		case STATE_IDLE:		
			break;

		case STATE_CAPTURING:	
			StopCapturing();
			break;

		case STATE_CONVERTING:	
			StopConverting();
			break;
	}

	OnOK();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//
// Capture-related Methods and Functions
//
//

//
// Initialize, start streaming and previewing, and start capturing to PDS files.
//
//
bool 
CAviCaptureDlg::StartCapturing()
{
	// Delete any existing pds files so we start with a clean slate
	DeleteFiles("*.pds");

	// Initialize a camera
	HANDLE hCamera;
	PXL_RETURN_CODE rc = PxLInitialize(0, &hCamera);
	if (!API_SUCCESS(rc)) {
		AfxMessageBox("Unable to initialize a PixeLINK camera", MB_ICONEXCLAMATION);
		return false;
	}

	// Start streaming and pop up a preview window
	bool initialized = false;
	HWND	tmpHWnd;
	rc = PxLSetStreamState(hCamera, START_STREAM);
	if (API_SUCCESS(rc)) {
		rc = PxLSetPreviewState(hCamera, START_PREVIEW, &tmpHWnd);
		if (API_SUCCESS(rc)) {
			initialized = true;
		}
	}

	if (!initialized) {
		PxLUninitialize(hCamera);
		AfxMessageBox("Unable to start the camera streaming and previewing", MB_ICONEXCLAMATION);
		return false;
	}



	AviCaptureState* pCapture = new AviCaptureState(
		hCamera, 
		DetermineFramesPerSecond(hCamera) * SECONDS_PER_CLIP,
		NUMBER_OF_CLIPS,
		"capture%2.2d.pds",
		CaptureTerminatedCallback);

	if (NULL == pCapture) {
		PxLUninitialize(hCamera);
		AfxMessageBox("Out of memory", MB_ICONEXCLAMATION);
		return false;
	}

	s_pCurrentCapture = pCapture;
	if (pCapture->StartCapturing()) {
		return true;
	}
	
	delete s_pCurrentCapture;
	s_pCurrentCapture = NULL;
	PxLUninitialize(hCamera);
	AfxMessageBox("Unable to start capturing video", MB_ICONEXCLAMATION);
	return false;
}

//
// We begin stopping the capture by stopping previewing and streaming. 
// By stopping the streaming, any capture to PDS in progress will call the termination
// function with ApiStoppedStream. When the clip termination callback function gets this, 
// it will post a message (WM_CAPTURE_FINISHED) to the application. The message handler
// for this message will complete the transition from capturing to the next state.
//
bool
CAviCaptureDlg::StopCapturing()
{
	ASSERT(NULL != s_pCurrentCapture);
	ASSERT(s_pCurrentCapture->KeepRunning());

	s_pCurrentCapture->StopRunning();
	HWND tmpHWnd;
	HANDLE hCamera = s_pCurrentCapture->GetCameraHandle();
	PxLSetPreviewState(hCamera, STOP_PREVIEW, &tmpHWnd);
	PxLSetStreamState(hCamera, STOP_STREAM);
	return true;
}

//
// Notification that the UI should be updated because another clip has finished
// 
// retCode: API_SUCCESS on success
// clipFileIndex : index of file being captured
//
LRESULT	
CAviCaptureDlg::OnCaptureUpdate(WPARAM retCode, LPARAM clipFileIndex)
{
	ASSERT(NULL != s_pCurrentCapture);

	if (API_SUCCESS(retCode)) {
		m_ctlProgress.SetPos((int)clipFileIndex + 1);
	}
	return 0;
}

//
// Notification that the UI should be updated because we're done capturing
// for one reason or another. 
//
// returnCode:     return code - API_SUCCESS on success
// lParam:  unused
//
LRESULT	
CAviCaptureDlg::OnCaptureFinished(WPARAM returnCode, LPARAM lParam)
{
	TRACE("CAviCaptureDlg::OnCaptureFinished");

	ASSERT(NULL != s_pCurrentCapture);
	PxLUninitialize(s_pCurrentCapture->GetCameraHandle());

	if (!API_SUCCESS(returnCode)) {
		CString msg;
		msg.Format("An error was reported while capturing video to a PDS file.\nError 0x%8.8X.", returnCode);
		AfxMessageBox(msg, MB_ICONEXCLAMATION);
	}

	// Set the state to idle until we know what's going to happen
	// And don't delete the s_pCurrentCapture until AFTER setting the state
	SetState(STATE_IDLE);
	delete s_pCurrentCapture;
	s_pCurrentCapture = NULL;

	// If we can convert files, ask if user wants to convert files
	if (AfxMessageBox("Convert PDS files to AVI files?", MB_YESNO | MB_ICONQUESTION) == IDYES) {
		if (StartConverting()) {
			SetState(STATE_CONVERTING);
		} else {
			AfxMessageBox("Unable to start the conversion of PDS to AVI files", MB_ICONEXCLAMATION);
		}
	}
	return 0;
}

//
// Function that's called when PxLGetClip is finished capturing frames, or can't continue
// capturing frames.
//
static U32 _stdcall 
CaptureTerminatedCallback(HANDLE hCamera, U32 numFramesCapture, PXL_RETURN_CODE returnCode)
{
	TRACE("CaptureTerminatedCallback");
	ASSERT(NULL != s_pCurrentCapture);

	HWND hMainAppHWnd = theApp.GetMainWnd()->GetSafeHwnd();

	if (API_SUCCESS(returnCode)) {
		if (!s_pCurrentCapture->KeepRunning()) {
			POST_MESSAGE_TO_APP(WM_CAPTURE_FINISHED, ApiSuccess, 0);
			return ApiSuccess;
		}
		if (!s_pCurrentCapture->CaptureNextClip()) {
			AfxMessageBox("Unable to start capture of next video clip", MB_ICONEXCLAMATION);
			POST_MESSAGE_TO_APP(WM_CAPTURE_FINISHED, ApiUnknownError, 0);
			return ApiSuccess;
		}
		POST_MESSAGE_TO_APP(WM_CAPTURE_UPDATE, ApiSuccess, s_pCurrentCapture->GetCurrentCaptureIndex());
	} else if (ApiStreamStopped == returnCode) {
		// Stream was stopped because we requested it
		POST_MESSAGE_TO_APP(WM_CAPTURE_FINISHED, ApiSuccess, 0);
	} else {
		// Unknown problem - report the problem to the user
		POST_MESSAGE_TO_APP(WM_CAPTURE_FINISHED, returnCode, 0);
	}
	return ApiSuccess;
}



//////////////////////////////////////////////////////////////////////////////////////////////////
//
// Conversion-related Methods and Functions
//
//

//
// PxLFormatClip is a blocking call, and may take quite a bit of time to complete when 
// working on large files. To not tie up the main thread (the UI thread), we'll spawn off
// a thread dedicated to converting all the PDS files to AVI files. 
//
bool
CAviCaptureDlg::StartConverting()
{
	AviConvertState* pConvert = new AviConvertState();
	if (NULL == pConvert) {
		return false;
	}
	ASSERT(NULL == s_pCurrentConversion);
	s_pCurrentConversion = pConvert;

	CWinThread* pThread = AfxBeginThread(ConvertPdsFilesToAvi, NULL, THREAD_PRIORITY_NORMAL, 0, 0);
	if (NULL == pThread) {
		delete s_pCurrentConversion;
		s_pCurrentConversion = NULL;
		return false;
	}

	s_pCurrentConversion->SetThread(pThread);

	return true;
}

//
// The most we can do to stop converting is tell the conversion thread that we want to 
// stop. When it's done the conversion of the current file, it will detect this and then terminate.
//
bool
CAviCaptureDlg::StopConverting()
{
	ASSERT(NULL != s_pCurrentConversion);
	s_pCurrentConversion->StopConverting();
	m_txtMessage.SetWindowText("Please wait. Cancelling conversion");
	return true;
}



//
// Notification that the UI should be updated because we're done converting a file
//
// currFileIndex: index of file currently being converted
// numFilesToConver: total number of files to be converted
//
LRESULT	
CAviCaptureDlg::OnConversionUpdate(WPARAM currFileIndex, LPARAM numFilesToConvert)
{
	ASSERT(NULL != s_pCurrentConversion);
	// Update the progress bar
	m_ctlProgress.SetRange(0,(short)numFilesToConvert);
	m_ctlProgress.SetPos((int)(currFileIndex+1));
	return 0;
}

//
// Notification that the UI should be updated because we're done converting a file
//
// returnCode: API_SUCCEESS on success
// lParam:  not used
//

LRESULT	
CAviCaptureDlg::OnConversionFinished(WPARAM returnCode, LPARAM lParam)
{
	ASSERT(NULL != s_pCurrentConversion);

	SetState(STATE_IDLE);

	ASSERT(NULL != s_pCurrentConversion);
	delete s_pCurrentConversion;
	s_pCurrentConversion = NULL;

	if (!API_SUCCESS(returnCode)) {
		CString msg;
		msg.Format("An error was reported while converting the PDS files to AVIs.\nError 0x%8.8X.\n", returnCode);
		AfxMessageBox(msg, MB_ICONEXCLAMATION);
	}
	
	return 0;
}

//
// The thread that converts PDS files to AVI files
//
static UINT 
ConvertPdsFilesToAvi(LPVOID lParam)
{
	ASSERT(NULL != s_pCurrentConversion);

	char inputFileName[MAX_PATH];
	char outputFileName[MAX_PATH];
	U32 numberOfFiles = s_pCurrentConversion->GetNumberOfFiles();

	// Convert each file
	for(U32 i=0; i < numberOfFiles; i++) {

		// Did the user ask us to stop?
		if (!s_pCurrentConversion->KeepRunning()) {
			break;
		}

		s_pCurrentConversion->SetCurrentIndex(i);
		POST_MESSAGE_TO_APP(WM_CONVERSION_UPDATE, i, numberOfFiles);

		// Create the name of the output file by changing the extension
		strcpy(inputFileName,  s_pCurrentConversion->GetCurrentFileName());
		strcpy(outputFileName, inputFileName);
		char* pExtension=strstr(outputFileName, ".pds");
		ASSERT(NULL != pExtension);
		strcpy(pExtension, ".avi");

		// And convert it
		PXL_RETURN_CODE rc = PxLFormatClip(inputFileName, outputFileName, CLIP_FORMAT_AVI);
		if (!API_SUCCESS(rc)) {
			POST_MESSAGE_TO_APP(WM_CONVERSION_FINISHED, ApiUnknownError, 0);
		}
	}

	POST_MESSAGE_TO_APP(WM_CONVERSION_FINISHED, 0, 0);
	return 0;
}

