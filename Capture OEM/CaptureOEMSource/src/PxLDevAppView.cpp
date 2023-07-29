// PxLDevAppView.cpp : implementation of the CPxLDevAppView class

#include "stdafx.h"
#include "PxLDevApp.h"
#include "PxlDevAppView.h"
#include <afxpriv.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#pragma warning(disable:4355) // "this" used in base member initializer list.


/////////////////////////////////////////////////////////////////////////////
// CPxLDevAppView

IMPLEMENT_DYNCREATE(CPxLDevAppView, CFormView)


BEGIN_MESSAGE_MAP(CPxLDevAppView, CFormView)

	ON_WM_DESTROY()
	ON_WM_TIMER()

	ON_MESSAGE_VOID(WM_IDLEUPDATECMDUI, OnIdleUpdate)

	ON_COMMAND(ID_VIEW_ADVANCEDMODE, OnViewAdvancedmode)
	ON_COMMAND(ID_VIEW_RESTOREGUIDEFAULTS, OnViewRestoreGuiDefaults)

	ON_UPDATE_COMMAND_UI(ID_VIEW_ADVANCEDMODE, OnUpdateViewAdvancedMode)

	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


/**
* Function: CPxLDevAppView
* Purpose:  Constructor.
*/
CPxLDevAppView::CPxLDevAppView()
	: CFormView(CPxLDevAppView::IDD)
	, m_Tabs(_T(""))
	, m_SimpleGui(this)
	, m_RoiGui(this)
	, m_LensGui(this)
	, m_DescriptorGui(this)
	, m_LookupTableGui(this)
	, m_KneePointGui(this)
	, m_GpioGui(this)
	, m_LinkGui(this)
   , m_LightingGui(this)
   , m_ImagesGui(this)
	, m_ClipsGui(this)
	, m_CallbackGui(this)
#ifdef SHOW_EXAMPLE_GUI
	, m_ExampleGui(this)
#endif
	, m_bAdvancedMode(true)
	, m_AdvancedModeWindowWidth(0)
	, m_AdvancedModeWindowHeight(0)
	, m_SimpleModeWindowWidth(0)
	, m_SimpleModeWindowHeight(0)
{
	// Add pages in the order that we want them to appear in the tabs.

	// First row
	m_Tabs.AddPage(&m_SimpleGui);
	m_Tabs.AddPage(&m_RoiGui);
	m_Tabs.AddPage(&m_LensGui);
	m_Tabs.AddPage(&m_LookupTableGui);
	m_Tabs.AddPage(&m_ImagesGui);
	m_Tabs.AddPage(&m_ClipsGui);

	// Second row
	m_Tabs.AddPage(&m_GpioGui);
	m_Tabs.AddPage(&m_LinkGui);
   m_Tabs.AddPage(&m_LightingGui);
   m_Tabs.AddPage(&m_CallbackGui);
	m_Tabs.AddPage(&m_KneePointGui);
	m_Tabs.AddPage(&m_DescriptorGui);
#ifdef SHOW_EXAMPLE_GUI
	m_Tabs.AddPage(&m_ExampleGui);
#endif
}


/**
* Function: ~CPxLDevAppView
* Purpose:  Destructor.
*/
CPxLDevAppView::~CPxLDevAppView()
{
}


/**
* Function: GetActiveCamera
* Purpose:  
*/
CPxLCamera* CPxLDevAppView::GetActiveCamera(void)
{
	return GetDocument()->GetActiveCamera();
}


/**
* Function: DoDataExchange
* Purpose:  
*/
void CPxLDevAppView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}


/**
* Function: PreCreateWindow
* Purpose:  
*/
BOOL CPxLDevAppView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~WS_BORDER; // Remove the sunken border around the view area.
	return CFormView::PreCreateWindow(cs);
}


/**
* Function: OnIdleUpdate
* Purpose:  Only the high-level MFC stuff gets the WM_IDLEUPDATECMDUI message -
*           specifically, the main frame, toolbars, etc, and the View class.
*           We handle this message here and pass it along to the active page
*           so that it can do idle-update processing.
*/
void CPxLDevAppView::OnIdleUpdate()
{
	CDevAppPage *pActivePage = static_cast<CDevAppPage*>(m_Tabs.GetActivePage());
	if (pActivePage != NULL)
		pActivePage->UpdateDialogControls(pActivePage, FALSE);
}


/**
* Function: OnInitialUpdate
* Purpose:  One-time initialization stuff.
*/
void CPxLDevAppView::OnInitialUpdate()
{
	// Finish the initialization of the PropertySheet.
	m_Tabs.Create(this, WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0);

	CFormView::OnInitialUpdate();

	// Set the mode (Simple or Advanced)
	UserSettings& settings = theApp.GetUserSettings();
	bool storedMode = settings.GetBoolSetting(USB_ADVANCED_MODE);
	m_bAdvancedMode = !storedMode; // Trick to get SetViewMode to work when the app first starts up.
	this->SetViewMode(storedMode);

	// Apply any persistent settings that are stored in the registry:
	ApplyUserSettings(); // May switch us to Advanced Mode...

	// Start the video preview automatically.
	// Decided not to do this after all - more trouble than it's worth - CP, Oct.3, 2003.
	// Made this active via command line flag "/preview" or "-preview" only.
	if (theApp.m_bAutoPreview && GetDocument()->HasActiveCamera())
	{
		if (GetActiveCamera()->FeatureSupported(FEATURE_TRIGGER) 
			&& TRIGGER_TYPE_HARDWARE == GetActiveCamera()->GetFeatureValue(FEATURE_TRIGGER, NULL, FEATURE_TRIGGER_PARAM_TYPE))
		{
			// The camera is in hardware trigger mode, so we do not want to start the preview.
		}
		else
		{
			try
			{
				GetActiveCamera()->StartPreview();

				// Bring the main application window in front of the preview window.
				CWnd* wnd = theApp.GetMainWnd();
				if (wnd != NULL)
				{
					::SetWindowPos(wnd->GetSafeHwnd(), GetActiveCamera()->GetPreviewHwnd(), 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);
				}
			}
			catch (PxLException const&)
			{
				// Starting the preview on startup is not essential - ignore the
				// error. If something is really wrong, we will get other errors
				// soon enough.
				//OnApiError(e);
			}
		}
	}
}


/**
* Function: ApplyUserSettings
* Purpose:  Restore application settings from the last time this user used it.
*/
void CPxLDevAppView::ApplyUserSettings(void)
{
	UserSettings& settings = theApp.GetUserSettings();

	theApp.m_bAlwaysOnTop = settings.GetBoolSetting(USB_ALWAYS_ON_TOP);

	// Apply general settings, like window size and location.
	// 1) Window position.
	int left = settings.GetIntSetting(USI_FRAMEWINDOW_LEFT);
	int top  = settings.GetIntSetting(USI_FRAMEWINDOW_TOP);
	if (left != -1 && top != -1) // -1 means "not set yet" (first run of app)
		this->GetParentFrame()->SetWindowPos(theApp.m_bAlwaysOnTop ? &wndTopMost : &wndNoTopMost, left, top, 0, 0, SWP_NOSIZE);
		//this->GetParentFrame()->SetWindowPos(NULL, left, top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);

	theApp.m_bAutoResetPreview = settings.GetBoolSetting(USB_AUTO_RESET_PREVIEW);
	theApp.m_framerateDivisor = settings.GetIntSetting(USI_FRAMERATE_DIVISOR);

	// 2) Mode - Simple or Advanced
	bool storedMode = settings.GetBoolSetting(USB_ADVANCED_MODE);
	this->SetViewMode(storedMode);

	// 3) Active tab, if in Advanced Mode.
	if (m_bAdvancedMode)
	{
		int tabNo = settings.GetIntSetting(USI_ACTIVE_TAB);
#if 0
		// 4) Window Height (if in Advanced Mode)
		CRect rcWnd;
		this->GetParentFrame()->GetWindowRect(&rcWnd);
		rcWnd.bottom = rcWnd.top + height;
		this->GetParentFrame()->SetWindowPos(NULL, 0, 0, rcWnd.Width(), rcWnd.Height(), SWP_NOMOVE|SWP_NOZORDER);
#endif
	}

	// For each page in the property sheet, ask the page to apply
	// whatever user settings it needs.
	CDevAppPage *pPage;
	for (int i = 0; i < m_Tabs.GetPageCount(); i++)
	{
		pPage = static_cast<CDevAppPage*>(m_Tabs.GetPage(i));
		pPage->ApplyUserSettings(settings);
	}

	// Try to initialize the last camera that was used by this user, if it
	// is connected. If not, initialize any connected camera.
	try
	{
		int lastUsedSerial = settings.GetIntSetting(USI_LAST_USED_SERIAL);
		if (GetDocument()->IsCameraConnected(lastUsedSerial))
			GetDocument()->SetActiveCamera(lastUsedSerial);
		else if (GetDocument()->IsCameraConnected(0))
			GetDocument()->SetActiveCamera(0); // Initialize any camera
		//else: there are no cameras connected
	}
	catch (PxLException const& e)
	{
		GetDocument()->OnApiError(e);
	}

}


/**
* Function: StoreUserSettings
* Purpose:  Called when the application is exiting. Anything that we want
*           restored to its current state when the application is next
*           started should be stored in the UserSettings object now.
*/
void CPxLDevAppView::StoreUserSettings(void)
{
	UserSettings& settings = theApp.GetUserSettings();

	// Store general settings, like window size and location.
	settings.SetBoolSetting(USB_ADVANCED_MODE, m_bAdvancedMode);
	settings.SetIntSetting(USI_ACTIVE_TAB, m_Tabs.GetActiveIndex());
	settings.SetIntSetting(USI_LAST_USED_SERIAL, static_cast<int>(GetActiveCamera()->GetSerialNumber()));

	if (!this->GetParentFrame()->IsIconic())
	{
		CRect rcWnd;
		this->GetParentFrame()->GetWindowRect(&rcWnd);
		settings.SetIntSetting(USI_FRAMEWINDOW_LEFT, rcWnd.left);
		settings.SetIntSetting(USI_FRAMEWINDOW_TOP, rcWnd.top);
	}

	settings.SetBoolSetting(USB_AUTO_RESET_PREVIEW, theApp.m_bAutoResetPreview);
	settings.SetBoolSetting(USB_ALWAYS_ON_TOP, theApp.m_bAlwaysOnTop);
	settings.SetIntSetting(USI_FRAMERATE_DIVISOR, theApp.m_framerateDivisor);

	// For each page in the property sheet, ask the page to store 
	// whatever user settings it uses.
	// Do the current page last, so that for any settings (eg: Image File Path)
	// that appear on more than one page, the current page's value takes precedence.
	CDevAppPage* pCurrentPage = static_cast<CDevAppPage*>(m_Tabs.GetActivePage());
	for (int i = 0; i < m_Tabs.GetPageCount(); i++)
	{
		CDevAppPage *pPage = static_cast<CDevAppPage*>(m_Tabs.GetPage(i));
		if (pPage == pCurrentPage)
			continue;
		pPage->StoreUserSettings(settings);
	}
	pCurrentPage->StoreUserSettings(settings);
}


/**
* Function: OnDestroy
* Purpose:  Called when the application is about to be closed.
*           We use this to store user settings to the registry.
*/
void CPxLDevAppView::OnDestroy(void)
{
	CFormView::OnDestroy();
	StoreUserSettings();
}


/**
* Function: PostNcDestroy
* Purpose:  Remove listener.
*/
void CPxLDevAppView::PostNcDestroy()
{
	CFormView::PostNcDestroy();
}


/**
* Function: OnUpdate
* Purpose:  Called after the Document has been modified.
*/
void CPxLDevAppView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* /*pHint*/)
{
	// For efficiency, we only update the active tab. Other tabs will be
	// updated only when/if they are activated.
	// This avoids (or at least delays) the overhead of updating a bunch of
	// pages that the user may not ever look at.
	int change = static_cast<int>(lHint);
	CDevAppPage *pActivePage = static_cast<CDevAppPage*>(m_Tabs.GetActivePage());
	if (pActivePage != NULL)
		pActivePage->DocumentChanged(change);

}


/**
* Function: SetAdvancedMode
* Purpose:  Put the GUI into "Advanced Mode" - all tabs visible and enabled.
*/
void CPxLDevAppView::SetAdvancedMode(void)
{
	// We will set the active tab back to the one that it was last time we
	// were in Advanced Mode.
	UserSettings& settings = theApp.GetUserSettings();
	int idx = settings.GetIntSetting(USI_ACTIVE_TAB);
	m_Tabs.SetMultiTabMode(idx);

	// Move the tabs to the top-left corner of the view area.
	m_Tabs.SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// Remove the Simple Mode tab while in Advanced Mode.
	//m_Tabs.RemovePage(&m_SimpleGui);

	SetAdvancedModeWindowLayout();

} // SetAdvancedMode


/**
* Function: SetAdvancedModeWindowLayout
* Purpose:  
*/
void CPxLDevAppView::SetAdvancedModeWindowLayout(void)
{
	// Get the screen rectangle of the property sheet.
	CRect rctPropSheet;
	m_Tabs.GetWindowRect(&rctPropSheet);
	ScreenToClient(&rctPropSheet);

	// We need to find out how much bigger the main frame is than the view area:
	CRect rctWin, rctClient;
	CFrameWnd *pMainFrame = GetParentFrame();
	pMainFrame->GetWindowRect(&rctWin);		// Main Frame's screen rect.
	this->GetWindowRect(&rctClient);		// View Area's screen rect.

	// Determine how much extra space there is around the view area - i.e., we
	// want to determine the height of the menus, the title bar, borders, etc.
	int xExtra = rctWin.Width() - rctClient.Width();
	int yExtra = rctWin.Height() - rctClient.Height();

	// Keep track of some of this size information so we can limit the resizing of the window.
	m_AdvancedModeWindowHeight	= yExtra + rctPropSheet.Height();
	m_AdvancedModeWindowWidth	= rctPropSheet.Width() + xExtra;

	// Resize the main frame so that the view will fit the contained property 
	// sheet, the call log box, and a margin between them. The View itself
	// will resize itself automatically.
	pMainFrame->SetWindowPos(NULL, 0, 0,
							 rctPropSheet.Width() + xExtra,
							 rctPropSheet.Height() + yExtra,
							 SWP_NOMOVE | SWP_NOZORDER);

	pMainFrame->ModifyStyleEx(0, WS_THICKFRAME);

}


/**
* Function: SetSimpleMode
* Purpose:  Put the GUI into "Simple Mode" - all tabs but the first are
*           hidden and disabled.
*/
void CPxLDevAppView::SetSimpleMode(void)
{
	// The trick to simple mode is to move the Property Sheet so that the
	// actual tabs are hidden just off the top of the view area, and resize the
	// main frame so that the border around the Property Sheet is hidden.

	// Keep track of which tab is active now, when we are leaving advanced
	// mode, so that we can restore it next time we switch to advanced mode.
	UserSettings& settings = theApp.GetUserSettings();
	settings.SetIntSetting(USI_ACTIVE_TAB, m_Tabs.GetActiveIndex());

	// Put the Simple Mode tab back in, if it has been removed.
	if (m_Tabs.GetPageIndex(&m_SimpleGui) == -1) {
		m_Tabs.AddPage(&m_SimpleGui);
	}

	// Make the Simple tab active.
	m_Tabs.SetSingleTabMode(&m_SimpleGui);

	SetSimpleModeWindowLayout();
}


/**
* Function: SetSimpleModeWindowLayout
* Purpose:  
*/
void CPxLDevAppView::SetSimpleModeWindowLayout(void)
{
	CRect rctPropSheet;
	CRect rctPage;
	m_Tabs.GetWindowRect(&rctPropSheet);
	m_Tabs.GetActivePage()->GetWindowRect(&rctPage);

	int yoffset = rctPage.top - rctPropSheet.top;
	int xoffset = rctPage.left - rctPropSheet.left;

	int xExtra(0);
	int yExtra(0);
	{
		// We need to find out how much bigger the main frame is than the view area:
		CRect rctWin, rctClient;
		CFrameWnd *pMainFrame = GetParentFrame();
		pMainFrame->GetWindowRect(&rctWin);		// Main Frame's screen rect.
		this->GetWindowRect(&rctClient);		// View Area's screen rect.

		// Determine how much extra space there is around the view area - i.e., we
		// want to determine the height of the menus, the title bar, borders, etc.
		xExtra = rctWin.Width() - rctClient.Width();
		yExtra = rctWin.Height() - rctClient.Height();
	}

	// Move the Property Page up and left far enough that the the tabs and the
	// left-hand border are hidden off the edge of the view area.
	m_Tabs.SetWindowPos(NULL,
						-xoffset,
						-yoffset,
						0, 0,
						SWP_NOSIZE | SWP_NOZORDER);

	// Resize the main frame to fit only the current tab's contents.
	m_SimpleModeWindowHeight	= rctPropSheet.Height();
	m_SimpleModeWindowWidth		= rctPage.Width() + xExtra;

	CFrameWnd *pMainFrame = GetParentFrame();
	pMainFrame->SetWindowPos(NULL, 0, 0,
							 m_SimpleModeWindowWidth,
							 m_SimpleModeWindowHeight,
							 SWP_NOMOVE | SWP_NOZORDER);

	// Remove the WS_THICKFRAME style, since the window is not resizeable in
	// simple mode. NOTE: This doesn't seem to work. Why?
	pMainFrame->ModifyStyleEx(WS_THICKFRAME, 0);

}


/**
* Function: SetViewMode
* Purpose:  Set the GUI mode to either Advanced or Simple.
*/
void CPxLDevAppView::SetViewMode(bool advancedMode)
{
	if (advancedMode == m_bAdvancedMode)
		return; // Nothing needs to be done

	m_bAdvancedMode = advancedMode;

	if (m_bAdvancedMode)
	{
		SetAdvancedMode();
	}
	else
	{
		SetSimpleMode();
	}

	CDevAppPage* pPage = (CDevAppPage*)m_Tabs.GetActivePage();
	pPage->ModeChanged(advancedMode);
}


/**
* Function: IsAdvancedMode
* Purpose:  
*/
bool CPxLDevAppView::IsAdvancedMode(void)
{
	return m_bAdvancedMode;
}

/**
* Function: SetMinMaxInfo
* Purpose:  Called from the main Frame Window's OnGetMinMaxInfo, to determine
*           how big or small it is to be.
*/
void CPxLDevAppView::SetMinMaxInfo(POINT& ptMinSize, POINT& ptMaxSize, CRect& wndRect)
{
	if (m_bAdvancedMode)
	{
		ptMaxSize.x = ptMinSize.x = m_AdvancedModeWindowWidth;
		ptMaxSize.y = ptMinSize.y = m_AdvancedModeWindowHeight;
	}
	else
	{
		ptMaxSize.x = ptMinSize.x = m_SimpleModeWindowWidth;
		ptMaxSize.y = ptMinSize.y = m_SimpleModeWindowHeight;
	}
}

/**
* Function: GetConfigFileData
* Purpose:  Fill the vector with any additional data that we want to have
*           stored in a config file. This data will be passed to the
*           SetFromConfigFileData method when the config file is loaded.
*/
void CPxLDevAppView::GetConfigFileData(std::vector<byte>& cfgData)
{
	for (int i = 0; i < m_Tabs.GetPageCount(); i++)
	{
		CDevAppPage* pPage = static_cast<CDevAppPage*>(m_Tabs.GetPage(i));

		// Make room for the IDD and the number of bytes added by the page.
		const size_t initialCfgDataSize = cfgData.size();
		cfgData.resize(cfgData.size() + 2 * sizeof(int));
		int* iddPos = reinterpret_cast<int*>(&cfgData[initialCfgDataSize]);
		int oldSize = (int)cfgData.size();
		pPage->GetConfigFileData(cfgData);
		if (oldSize < cfgData.size())
		{
			// The page added data
			*iddPos++ = pPage->GetDlgId();
			*iddPos = (int)cfgData.size() - oldSize;
		}
		else
		{
			// The page did not add any data.
			cfgData.resize(cfgData.size() - 2 * sizeof(int));
		}
	}
}


/**
* Function: SetFromConfigFileData
* Purpose:  A block of data that we stored in a previous call to
*           GetConfigFileData is passed back to us here. We are to use it to
*           restore whatever application state we stored in the config file.
*/
void CPxLDevAppView::SetFromConfigFileData(std::vector<byte> const& cfgData, int startPos)
{
	byte const* pdata = &cfgData[startPos];
	byte const * pOnePastEndOfBuffer = &cfgData[cfgData.size() - 1] + 1;
	byte const * const pOnePastEndOfData = pOnePastEndOfBuffer - (2 * sizeof(int));
	

	while (pdata < pOnePastEndOfData)
	{
		int idd = *reinterpret_cast<int const*>(pdata);
		pdata += sizeof(int);
		int nbytes = *reinterpret_cast<int const*>(pdata);
		pdata += sizeof(int);

		if ((pdata + nbytes) > pOnePastEndOfBuffer)
		{
			TRACE("Page with IDD %d stored %d bytes. Only %d available.\n", idd, nbytes, (pOnePastEndOfBuffer - pdata));
			return; // Error
		}

		for (int i = 0; i < m_Tabs.GetPageCount(); i++)
		{
			CDevAppPage* pPage = static_cast<CDevAppPage*>(m_Tabs.GetPage(i));
			if (pPage->GetDlgId() == idd)
			{
				pPage->SetFromConfigFileData(pdata, nbytes);
				break;
			}
		}

		pdata += nbytes;
	}
}


/**
* Function: OnContextMenu
* Purpose:  
*/
void CPxLDevAppView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	// Handle right click
}


/**
* Function: OnViewAdvancedmode
* Purpose:  Event handler for the View->Advanced Mode menu.
*/
void CPxLDevAppView::OnViewAdvancedmode(void)
{
	SetViewMode(!m_bAdvancedMode);
}


/**
* Function: OnUpdateViewAdvancedMode
* Purpose:  Idle-Update handler for the View->Advanced Mode menu. Set the
*           check on or off.
*/
void CPxLDevAppView::OnUpdateViewAdvancedMode(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bAdvancedMode);
}


/**
* Function: OnViewRestoreGuiDefaults
* Purpose:  
*/
void CPxLDevAppView::OnViewRestoreGuiDefaults()
{
	UserSettings& settings = theApp.GetUserSettings();
	settings.LoadDefaults(true); // Specify "reloadOnly".
	this->ApplyUserSettings();
}




/////////////////////////////////////////////////////////////////////////////
// CPxLDevAppView diagnostics

#ifdef _DEBUG
void CPxLDevAppView::AssertValid() const
{
	CFormView::AssertValid();
}

void CPxLDevAppView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CPxLCameraDoc* CPxLDevAppView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPxLCameraDoc)));
	return (CPxLCameraDoc*)m_pDocument;
}
#endif //_DEBUG




