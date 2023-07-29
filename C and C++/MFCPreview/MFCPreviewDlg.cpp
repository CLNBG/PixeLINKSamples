// MFCPreviewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFCPreview.h"
#include "MFCPreviewDlg.h"
#include "mfcpreviewdlg.h"

#include <PixeLINKApi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCPreviewDlg dialog



CMFCPreviewDlg::CMFCPreviewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMFCPreviewDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCPreviewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCPreviewDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CMFCPreviewDlg message handlers
static HANDLE s_hCamera;

BOOL CMFCPreviewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	PXL_RETURN_CODE rc = PxLInitialize(0, &s_hCamera);

	CWnd* pChildWnd = GetDlgItem(IDC_STATIC_PREVIEW);

	RECT rectChildWindow;
	pChildWnd->GetWindowRect(&rectChildWindow);
	U32 childId = pChildWnd->GetDlgCtrlID();

	// Account for the border of the picture control
	// There's a border on each size
	int pictureBorderWidth = ::GetSystemMetrics(SM_CXEDGE);
	U32 width = rectChildWindow.right  - rectChildWindow.left - 2 * pictureBorderWidth; 
	U32 height = rectChildWindow.bottom - rectChildWindow.top - 2 * pictureBorderWidth;

	rc = PxLSetPreviewSettings(s_hCamera, 
        "HOWDY",	// Not displayed at all 
        WS_CHILD | WS_VISIBLE,
        0, 0, width, height,
		pChildWnd->m_hWnd,
        childId);

	HWND hWndPreview;
	rc = PxLSetStreamState(s_hCamera, START_STREAM);
	rc = PxLSetPreviewState(s_hCamera, START_PREVIEW, &hWndPreview);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMFCPreviewDlg::OnPaint() 
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
HCURSOR CMFCPreviewDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMFCPreviewDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}
