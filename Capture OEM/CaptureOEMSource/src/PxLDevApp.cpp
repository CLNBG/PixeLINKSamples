// PxLDevApp.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PxLDevApp.h"

#include "MainFrm.h"
#include "PxLDevAppView.h"
#include "PxlDevApp.h"
#include "SplashWnd.h"
#include "CPxLSubwindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPxLDevApp

BEGIN_MESSAGE_MAP(CPxLDevApp, CWinApp)
	ON_COMMAND(ID_HELP_CONTENTS, OnHelpContents)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPxLDevApp construction

CPxLDevApp::CPxLDevApp()
	: m_bAutoResetPreview(false)
	, m_bAlwaysOnTop(false)
	, m_bAutoPreview(false)
	, m_framerateDivisor(1) // 2006-12-13, default the framerate devisor to 1 (laptop friendly mode == off)
	, m_appIsStartingUp(true)
	, m_bAllowMultipleInstances(false)
	, m_bAllowConnectToMultipleCameras(false)
	, m_bRegistrySettingsLoaded(false)
    , m_bCmdLineHasBadParameters(false)
	, m_bShowCmdLineParameterHelp(false)
	, m_bEnableJumboFrames(false)
	, m_bMonitorAccessOnly(false)
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPxLDevApp object

CPxLDevApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPxLDevApp initialization

#define APP_TITLE "PixeLINK Capture OEM"

class DevAppCmdLineInfo : public CCommandLineInfo
{
public:
	void ParseParam(const TCHAR* pszParam,BOOL bFlag,BOOL bLast)
	{
		if (bFlag && _tcsicmp(_T("preview"), pszParam) == 0) {
			theApp.m_bAutoPreview = true;
		} else if (bFlag && _tcsicmp(_T("jumbo"), pszParam) == 0) {
			theApp.m_bEnableJumboFrames = true;
		} else if (bFlag && _tcsicmp(_T("allowMultipleInstances"), pszParam) == 0) {
			theApp.m_bAllowMultipleInstances = true;
		} else if (bFlag && _tcsicmp(_T("monitorAccessOnly"), pszParam) == 0) {
			theApp.m_bMonitorAccessOnly = true;
		} else if (bFlag && _tcsicmp(_T("allowMultipleConnections"), pszParam) == 0) {
			theApp.m_bAllowConnectToMultipleCameras = true;
		} else if (bFlag && _tcsicmp(_T("help"), pszParam) == 0) {
			theApp.m_bShowCmdLineParameterHelp = true;
		} else if (bFlag && _tcsicmp(_T("?"), pszParam) == 0) {
			theApp.m_bShowCmdLineParameterHelp = true;
		} else {
			theApp.m_bCmdLineHasBadParameters = true;
		}
	}
};

static void 
DisplaySupportedCommandLineParameters(bool cmdLineError)
{
	CString msg(cmdLineError ? "An invalid command-line parameters was detected.\n\n" : "");
	msg = msg + "Supported command line parameters:\n\n" 
	"/help  - Display this help\n"
	"/preview  - Autostart preview on startup\n"
	"/allowMultipleInstances  - Allow more than one instance\n"
	"/allowMultipleConnections - Allow connection to more than one camera at a time\n"
	"/jumbo  - Enable jumbo frames\n"
	"/monitorAccessOnly  - Cameras will be read-only\n";
	AfxMessageBox(msg, cmdLineError ? MB_ICONEXCLAMATION : MB_ICONINFORMATION);
}

BOOL CPxLDevApp::InitInstance()
{
	// Dispatch commands specified on the command line
	// Parse command line for standard shell commands, DDE, file open
	DevAppCmdLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if (m_bCmdLineHasBadParameters) {
		DisplaySupportedCommandLineParameters(true);
		return FALSE;
	}

	if (m_bShowCmdLineParameterHelp) {
		DisplaySupportedCommandLineParameters(false);
		return FALSE;
	}

	if (!m_bAllowMultipleInstances) 
	{
		// Make sure that only one application instance can be opened at a time.
		HANDLE mutex_handle = CreateMutex(NULL, FALSE, _T("PxLDevAppMutexID"));
		if (mutex_handle != NULL && GetLastError() == ERROR_ALREADY_EXISTS)
		{
			// The application is already running.
			HWND existingApp = ::FindWindow(NULL, _T(APP_TITLE));
			if (existingApp != NULL)
			{
				// Bring the already running instance to the front:
				CWnd* pPrevWnd = CWnd::FromHandle(existingApp);
				if (pPrevWnd != NULL)
				{
					if (pPrevWnd->IsIconic()) 
					{
						pPrevWnd->ShowWindow(SW_RESTORE);
					}
					pPrevWnd->SetForegroundWindow();
					pPrevWnd->GetLastActivePopup()->SetForegroundWindow();
				}
			}
			//::SetActiveWindow(existingApp);
			return FALSE;
		}
	}

	if (m_bEnableJumboFrames) {
		if (!CPxLCamera::EnableJumboFrames(true)) {
			AfxMessageBox("ERROR: Unable to enable jumbo frames", MB_ICONEXCLAMATION);
			return FALSE;
		}
	}


	CSplashWnd::EnableSplashScreen(TRUE);

	AfxEnableControlContainer();
	//OleInitialize(NULL);

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
#if _MFC_VER < 0x0700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	// Change the registry key under which our settings are stored.
	SetRegistryKey(_T("PixeLINK"));

	// Load our settings from the registry. The view class, in its 
	// OnInitialUpdate method, will ask for a reference to this object, and
	// then use it to apply whatever settings it needs.
	m_settings.LoadFromRegistry();
	m_bRegistrySettingsLoaded = true;


	// It can take some time to get things initialized and displayed, so 
	// display a splash screen to let the user know we're up and doing something..
	CSplashWnd::ShowSplashScreen(1000, IDB_SPLASH);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CPxLCameraDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CPxLDevAppView));
	AddDocTemplate(pDocTemplate);

	if (!ProcessShellCommand(cmdInfo)) {
		return FALSE;
	}


	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	m_appIsStartingUp = false;
	return TRUE;
}


/**
* Function: GetUserSettings
* Purpose:  
*/
UserSettings& CPxLDevApp::GetUserSettings()
{
	return m_settings;
}


/////////////////////////////////////////////////////////////////////////////
// CPxLDevApp message handlers


/**
* Function: OnHelpContents
* Purpose:  
*/
void CPxLDevApp::OnHelpContents()
{
	ShowHelpContents();
}


/**
* Function: ExitInstance
* Purpose:  
*/
int CPxLDevApp::ExitInstance()
{
	if (m_bRegistrySettingsLoaded) 
	{
		m_settings.SaveToRegistry();
	}

	return CWinApp::ExitInstance();
}


BOOL 
CPxLDevApp::PreTranslateMessage(MSG* pMsg)
{
	return CWinApp::PreTranslateMessage(pMsg);
}
