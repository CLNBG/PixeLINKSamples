
// TesterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Tester.h"
#include "TesterDlg.h"
#include <OverlayEngine.h>
#include <PixeLINKApi.h>

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


// CTesterDlg dialog




CTesterDlg::CTesterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTesterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTesterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTesterDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_RUNTEST, &CTesterDlg::OnBnClickedBtnRuntest)
END_MESSAGE_MAP()


// CTesterDlg message handlers

BOOL CTesterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTesterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTesterDlg::OnPaint()
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
HCURSOR CTesterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

class CameraHandle
{
private:
	const HANDLE m_hCamera;
public:
	CameraHandle (const HANDLE hCamera) : m_hCamera(hCamera) {};
	~CameraHandle() { PxLUninitialize(m_hCamera); }
};

void CTesterDlg::OnBnClickedBtnRuntest()
{
	CWaitCursor wait;

	HANDLE hCamera(0);
	PXL_RETURN_CODE rc = PxLInitialize(0, &hCamera);
	if (!API_SUCCESS(rc)) {
		::AfxMessageBox("Unable to initialize a camera", MB_ICONEXCLAMATION);
		return;
	}
	CameraHandle h(hCamera); // Make sure we uninitialize;

	int rv = OverlayEngineInitialize();
	ASSERT(0 == rv);

	rv = OverlayEngineSetOverlayBitmap(hCamera, "overlay.bmp");
	if (OVERLAY_ENGINE_SUCCESS != rv) {
		::AfxMessageBox("Unable to load the overlay bitmap", MB_ICONEXCLAMATION);
		return;
	}

	// Make sure we can specify another one to replace the old one
	rv = OverlayEngineSetOverlayBitmap(hCamera, "overlay.bmp");
	if (OVERLAY_ENGINE_SUCCESS != rv) {
		::AfxMessageBox("Unable to load the overlay bitmap", MB_ICONEXCLAMATION);
		return;
	}

	rv = OverlayEngineEnableOverlay(hCamera, CALLBACK_PREVIEW, 1);
	if (OVERLAY_ENGINE_SUCCESS != rv) {
		::AfxMessageBox("Unable to enable the preview overlay", MB_ICONEXCLAMATION);
		return;
	}

	rc = PxLSetStreamState(hCamera, START_STREAM);
	if (!API_SUCCESS(rc)) {
		::AfxMessageBox("Unable to enable the start the stream", MB_ICONEXCLAMATION);
		return;
	}

	rc = PxLSetPreviewState(hCamera, START_PREVIEW, NULL);
	if (!API_SUCCESS(rc)) {
		::AfxMessageBox("Unable to start the preview", MB_ICONEXCLAMATION);
		return;
	}

	// Send it across the diagonal
	for(int i = 0; i < 200; i++) {
		rv = OverlayEngineSetOverlayOffset(hCamera, i, i);
		Sleep(10);
	}

	// And back 
	for(int i = 200; i >= 0; i--) {
		rv = OverlayEngineSetOverlayOffset(hCamera, i, i);
		Sleep(10);
	}

	// Now enable and disable a few times, making it blink
	for(int i =0; i < 10; i++) {
		OverlayEngineEnableOverlay(hCamera, CALLBACK_PREVIEW, 1);
		Sleep(250);
		OverlayEngineEnableOverlay(hCamera, CALLBACK_PREVIEW, 0);
		Sleep(250);
	}

	rv = OverlayEngineUninitialize();
	ASSERT(0 == rv);

	PxLSetPreviewState(hCamera, STOP_PREVIEW, NULL);
	PxLSetStreamState(hCamera, STOP_STREAM);
	return;

}
