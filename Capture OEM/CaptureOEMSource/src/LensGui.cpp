//
// LensGui.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "LensGui.h"
#include "CallbacksGui.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CCallbacksGui* s_CallbackGui;

/******************************************************************************
 * CLensGui - Member functions.
 *****************************************************************************/

IMPLEMENT_DYNCREATE(CLensGui, CDevAppPage)

CLensGui::CLensGui(CPxLDevAppView* pView)
	: CDevAppPage(CLensGui::IDD, pView)
	, m_bSsRoiConfigured(false)
    , m_DefaultState(true)
    , m_MyController(0)
    , m_bDontWantController(false)
{
	//{{AFX_DATA_INIT(CLensGui)
	//}}AFX_DATA_INIT
}

CLensGui::~CLensGui()
{
}

void CLensGui::DoDataExchange(CDataExchange* pDX)
{
	CDevAppPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLensGui)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_LENS_CAMERASELECT_COMBO, m_CameraSelectCombo);
	DDX_Control(pDX, IDC_LENS_PLAY_BUTTON, m_PlayButton);
	DDX_Control(pDX, IDC_LENS_PAUSE_BUTTON, m_PauseButton);
	DDX_Control(pDX, IDC_LENS_STOP_BUTTON, m_StopButton);

	DDX_Control(pDX, IDC_LENS_SHARPNESS_SCOREONOFF_CHECK, m_SharpnessScoreCheck);
	DDX_Control(pDX, IDC_LENS_PREVIEW_CHECK, m_PreviewCheck);

	DDX_Control(pDX, IDC_LENS_ROI_WIDTH_EDIT, m_RoiWidthEdit);
	DDX_Control(pDX, IDC_LENS_ROI_HEIGHT_EDIT, m_RoiHeightEdit);

	DDX_Control(pDX, IDC_LENS_SUBWINDOW_CTRL, m_SubwindowCtrl);
	DDX_Control(pDX, IDC_LENS_WINDOWTOP_EDIT, m_TopEdit);
	DDX_Control(pDX, IDC_LENS_WINDOWLEFT_EDIT, m_LeftEdit);
	DDX_Control(pDX, IDC_LENS_WINDOWHEIGHT_EDIT, m_HeightEdit);
	DDX_Control(pDX, IDC_LENS_WINDOWWIDTH_EDIT, m_WidthEdit);
	DDX_Control(pDX, IDC_LENS_WINDOWSIZE_COMBO, m_WindowSizeCombo);
	DDX_Control(pDX, IDC_LENS_CENTER_BUTTON, m_CenterButton);

	DDX_Control(pDX, IDC_LENS_CONTROLLERSELECT_COMBO, m_ControllerSelectCombo);
	DDX_Control(pDX, IDC_LENS_CANTOPENCOMWARNING_STATIC, m_CantOpenComWarningStatic);

    DDX_SLIDER_SUITE(LENS_FOCUS_CONTROL,FocusControl)
	DDX_SLIDER_SUITE(LENS_ZOOM_CONTROL,ZoomControl)

    DDX_Control(pDX, IDC_LENS_FOCUS_LOWER_BUTTON, m_FocusLowerButton);
    DDX_Control(pDX, IDC_LENS_FOCUS_UPPER_BUTTON, m_FocusUpperButton);
    DDX_Control(pDX, IDC_LENS_ZOOM_LOWER_BUTTON, m_ZoomLowerButton);
    DDX_Control(pDX, IDC_LENS_ZOOM_UPPER_BUTTON, m_ZoomUpperButton);

}


BEGIN_MESSAGE_MAP(CLensGui, CDevAppPage)

	ON_CBN_DROPDOWN(IDC_LENS_CAMERASELECT_COMBO, OnDropdownCameraSelectCombo)
	ON_CBN_SELCHANGE(IDC_LENS_CAMERASELECT_COMBO, OnSelchangeCameraSelectCombo)
	ON_BN_CLICKED(IDC_LENS_PLAY_BUTTON, OnClickPlayButton)
	ON_BN_CLICKED(IDC_LENS_PAUSE_BUTTON, OnClickPauseButton)
	ON_BN_CLICKED(IDC_LENS_STOP_BUTTON, OnClickStopButton)

	ON_BN_CLICKED(IDC_LENS_SHARPNESS_SCOREONOFF_CHECK, OnClickSharpnessScoreCheck)
	ON_BN_CLICKED(IDC_LENS_PREVIEW_CHECK, OnClickPreviewCheck)

	ON_EN_KILLFOCUS(IDC_LENS_WINDOWTOP_EDIT, OnKillFocusSubwindowTopEdit)
	ON_EN_KILLFOCUS(IDC_LENS_WINDOWLEFT_EDIT, OnKillFocusSubwindowLeftEdit)
	ON_EN_KILLFOCUS(IDC_LENS_WINDOWWIDTH_EDIT, OnKillFocusSubwindowWidthEdit)
	ON_EN_KILLFOCUS(IDC_LENS_WINDOWHEIGHT_EDIT, OnKillFocusSubwindowHeightEdit)

	ON_EN_SETFOCUS(IDC_LENS_WINDOWLEFT_EDIT, OnSetFocusWindowLeftEdit)
	ON_EN_SETFOCUS(IDC_LENS_WINDOWTOP_EDIT, OnSetFocusWindowTopEdit)
	ON_EN_SETFOCUS(IDC_LENS_WINDOWWIDTH_EDIT, OnSetFocusWindowWidthEdit)
	ON_EN_SETFOCUS(IDC_LENS_WINDOWHEIGHT_EDIT, OnSetFocusWindowHeightEdit)

	ON_BN_CLICKED(IDC_LENS_CENTER_BUTTON, OnClickCenterButton)

	ON_CBN_SELCHANGE(IDC_LENS_WINDOWSIZE_COMBO, OnSelchangeWindowsizeCombo)

	ON_CBN_SELCHANGE(IDC_LENS_CONTROLLERSELECT_COMBO, OnSelchangeControllerSelectCombo)
	ON_CBN_DROPDOWN(IDC_LENS_CONTROLLERSELECT_COMBO, OnDropdownControllerSelectCombo)

    MM_SLIDER_SUITE(LENS_FOCUS_CONTROL,FocusControl)
	MM_SLIDER_SUITE(LENS_ZOOM_CONTROL,ZoomControl)

   	ON_BN_CLICKED(IDC_LENS_FOCUS_LOWER_BUTTON, OnClickFocusLowerButton)
   	ON_BN_CLICKED(IDC_LENS_FOCUS_UPPER_BUTTON, OnClickFocusUpperButton)
   	ON_BN_CLICKED(IDC_LENS_ZOOM_LOWER_BUTTON, OnClickZoomLowerButton)
   	ON_BN_CLICKED(IDC_LENS_ZOOM_UPPER_BUTTON, OnClickZoomUpperButton)

END_MESSAGE_MAP()


BEGIN_EVENTSINK_MAP(CLensGui, CDevAppPage)
	ON_EVENT(CLensGui, IDC_LENS_SUBWINDOW_CTRL, 1, OnSubwindowChanged, VTS_I4 VTS_I4 VTS_I4 VTS_I4)
	EVENTSINK_SLIDER_SUITE(CLensGui,LENS_FOCUS_CONTROL,FocusControl)
	EVENTSINK_SLIDER_SUITE(CLensGui,LENS_ZOOM_CONTROL,ZoomControl)

END_EVENTSINK_MAP()


/**
* Function: GetHelpContextId
* Purpose:  
*/
int CLensGui::GetHelpContextId()
{
	return IDH_LENS_TAB;
}


/**
* Function: OnInitDialog
* Purpose:  
*/
BOOL CLensGui::OnInitDialog()
{
	CDevAppPage::OnInitDialog();

	// The PxLSubwindow control is able to semi-automatically exchange data
	// with up to four edit controls  - we just need to tell it which ones:
	m_SubwindowCtrl.SetEdits(&m_LeftEdit, &m_TopEdit, &m_WidthEdit, &m_HeightEdit);

	m_FocusControlSlider.SetLinkedWindows(&m_FocusControlEdit, &m_FocusControlMinLabel, &m_FocusControlMaxLabel, &m_FocusControlFrame);
	m_ZoomControlSlider.SetLinkedWindows(&m_ZoomControlEdit, &m_ZoomControlMinLabel, &m_ZoomControlMaxLabel, &m_ZoomControlFrame);

	return TRUE;  // return TRUE unless you set the focus to a control
}


/**
* Function: OnSetActive
* Purpose:  
*/
BOOL CLensGui::OnSetActive()
{
	m_bSsRoiConfigured = false;
	return CDevAppPage::OnSetActive();
}


/**
* Function: SetGUIState
* Purpose:  Enable / Disable controls, according to the state of the camera.
*           Most controls cannot be used in any meaningful way until a camera
*           is initialized.
*/
void CLensGui::SetGUIState(eGUIState state)
{
	// The CameraSelect and Play/Pause/Stop button states are set in the base
	// class version of this function.
	CDevAppPage::SetGUIState(state);

	CPxLCamera* cam = GetActiveCamera();

	bool enable = (state != GS_NoCamera);

    bool ssSupported = enable && cam->FeatureSupported(FEATURE_SHARPNESS_SCORE);
	bool ssEnabled = ssSupported & (! GetActiveCamera()->IsFeatureFlagSet(FEATURE_SHARPNESS_SCORE, FEATURE_FLAG_OFF));
    bool focusSupported = enable && cam->FeatureSupported(FEATURE_FOCUS);
    bool zoomSupported = enable && cam->FeatureSupported(FEATURE_ZOOM);

	m_SharpnessScoreCheck.EnableWindow(ssSupported);
	m_PreviewCheck.EnableWindow(ssSupported);

	m_SubwindowCtrl.put_Enabled(ssEnabled);
	m_TopEdit.EnableWindow(ssEnabled);
	m_LeftEdit.EnableWindow(ssEnabled);
	m_WidthEdit.EnableWindow(ssEnabled);
	m_HeightEdit.EnableWindow(ssEnabled);
	m_WindowSizeCombo.EnableWindow(ssEnabled);
	m_CenterButton.EnableWindow(ssEnabled);

    m_ControllerSelectCombo.EnableWindow(enable);
	
	ENABLE_SLIDER_SUITE(FocusControl,FOCUS)
	ENABLE_SLIDER_SUITE(ZoomControl,ZOOM)

    m_FocusLowerButton.EnableWindow(focusSupported && cam->FeatureSupportsAssertLowerLimit(FEATURE_FOCUS));
    m_FocusUpperButton.EnableWindow(focusSupported && cam->FeatureSupportsAssertUpperLimit(FEATURE_FOCUS));
    m_ZoomLowerButton.EnableWindow(zoomSupported && cam->FeatureSupportsAssertLowerLimit(FEATURE_ZOOM));
    m_ZoomUpperButton.EnableWindow(zoomSupported && cam->FeatureSupportsAssertUpperLimit(FEATURE_ZOOM));

}


/**
* Function: ConfigureControls
* Purpose:  Set up the controls on the page to customize them according to the
*           currently active camera. This involves things like setting the max
*           and min values for sliders and setting the bounds of the subwindow
*           control.
*/
void CLensGui::ConfigureControls(void)
{
	SetSliderRange(m_FocusControlSlider, FEATURE_FOCUS);
	SetSliderRange(m_ZoomControlSlider, FEATURE_ZOOM);

	ConfigureSsRoi();	
}


/**
* Function: ConfigureRoi
* Purpose:  Sets up the ROI window for the sharpness score.  Note that like the ROI, this dialog generally works
*           withe the 'transposed' ROI window, as this is what the user sees and it's more intuative for the 
*           user to work with.  However, like FEATURE_ROI, FEATURE_SHARPNESS_SCORE's ROI parameters that 
*           are supplied to the API, are pre-translosed.  CPxLCamera::SetRoi will take care of this conversion
*           for us.
*/
void CLensGui::ConfigureSsRoi(void)
{
	if (!GetActiveCamera()->FeatureSupported(FEATURE_SHARPNESS_SCORE))
		return;

	try
	{
   		long currentRoiWidth  = static_cast<long>(GetActiveCamera()->GetFeatureValue(FEATURE_ROI, NULL, FEATURE_ROI_PARAM_WIDTH));
   		long currentRoiHeight = static_cast<long>(GetActiveCamera()->GetFeatureValue(FEATURE_ROI, NULL, FEATURE_ROI_PARAM_HEIGHT));
   	    long maxSsRoiWidth  = static_cast<long>(GetActiveCamera()->GetFeatureMaxVal(FEATURE_SHARPNESS_SCORE, FEATURE_SHARPNESS_SCORE_PARAM_WIDTH));
   	    long maxSsRoiHeight = static_cast<long>(GetActiveCamera()->GetFeatureMaxVal(FEATURE_SHARPNESS_SCORE, FEATURE_SHARPNESS_SCORE_PARAM_HEIGHT));

		// We want the SubwindowControl to work in an intuitive way, even in the
		// presence of Flip and Rotation.
		bool sideways = GetActiveCamera()->FeatureSupported(FEATURE_ROTATE)
					&& static_cast<int>(GetActiveCamera()->GetFeatureValue(FEATURE_ROTATE)) % 180 != 0;
		if (sideways)
		{
			std::swap(currentRoiWidth, currentRoiHeight);
			std::swap(maxSsRoiWidth, maxSsRoiHeight);
		}
		long maxWidth  = min(currentRoiWidth, maxSsRoiWidth);
		long maxHeight = min(currentRoiHeight,maxSsRoiHeight);

		TempVal<bool> temp(m_bAdjusting[FEATURE_SHARPNESS_SCORE], true); // SetBounds fires a SubwindowChanged event.

		m_SubwindowCtrl.SetBounds(currentRoiWidth,currentRoiHeight);

		long minSsRoiWidth = static_cast<long>(
			GetActiveCamera()->GetFeatureMinVal(FEATURE_SHARPNESS_SCORE, FEATURE_SHARPNESS_SCORE_PARAM_WIDTH));
		long minSsRoiHeight = static_cast<long>(
			GetActiveCamera()->GetFeatureMinVal(FEATURE_SHARPNESS_SCORE, FEATURE_SHARPNESS_SCORE_PARAM_HEIGHT));

		if (sideways)
		{
			std::swap(minSsRoiWidth, minSsRoiHeight);
		}

		m_SubwindowCtrl.put_MinWidth(minSsRoiWidth);
		m_SubwindowCtrl.put_MinHeight(minSsRoiHeight);

		// Limit the list of standard subwindow sizes to those that are
		// within the capabilities of the camera.
		SetWindowSizeEntries(
			m_WindowSizeCombo,
			static_cast<int>(minSsRoiWidth),
			static_cast<int>(minSsRoiHeight),
			static_cast<int>(maxWidth), 
			static_cast<int>(maxHeight));

		m_bSsRoiConfigured = true;
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: PopulateDefaults
* Purpose:  Put some meaningles default values into the controls, for purely
*           aesthetic reasons.  Called when when we don't have a camera.
*/
void CLensGui::PopulateDefaults(void)
{
    m_RoiWidthEdit.Clear();
    m_RoiHeightEdit.Clear();
    
	{
		TempVal<bool> _temp(m_bAdjusting[FEATURE_SHARPNESS_SCORE], true);
		m_SubwindowCtrl.SetBounds(DEFAULT_FOV_WIDTH,DEFAULT_FOV_HEIGHT);
		m_SubwindowCtrl.SetSubwindow(DEFAULT_FOV_WIDTH/4, DEFAULT_FOV_HEIGHT/4, DEFAULT_FOV_WIDTH/2, DEFAULT_FOV_HEIGHT/2);
		m_SubwindowCtrl.UpdateEdits();
	}
	m_SharpnessScoreCheck.SetCheck(BST_UNCHECKED);

	m_PreviewCheck.SetCheck(BST_UNCHECKED);

    m_ControllerSelectCombo.ResetContent();
    m_ControllerSelectCombo.AddString(_T("No Controller Assigned"));
    m_ControllerSelectCombo.SetItemData(0, 0);  // the first item is always no contrller, represented as s/n 0
    m_ControllerSelectCombo.SetCurSel(0);

    m_CantOpenComWarningStatic.ShowWindow(SW_HIDE);

	SetSliderDefault(m_FocusControlSlider);
	SetSliderDefault(m_ZoomControlSlider);

    m_MyController = 0;
    m_bDontWantController= false;
	
    m_DefaultState = true;
}


/**
* Function: PopulateControls
* Purpose:  Make the values displayed in the controls reflect the current 
*           settings of the active camera.  Called immediately after a 
*           camera is initialized.
*/
void CLensGui::PopulateControls(void)
{
	CDevAppPage::PopulateControls();
	
    bool ssSupported = GetActiveCamera()->FeatureSupported(FEATURE_SHARPNESS_SCORE);
	bool ssEnabled = ssSupported & (! GetActiveCamera()->IsFeatureFlagSet(FEATURE_SHARPNESS_SCORE, FEATURE_FLAG_OFF));

	PopulateSubwindow();
	PopulateWindowSize(m_WindowSizeCombo, true, FEATURE_SHARPNESS_SCORE);

	PopulateFocusControl();
	PopulateZoomControl();

	m_SharpnessScoreCheck.SetCheck(ssEnabled);
    m_PreviewCheck.EnableWindow(ssEnabled);
    // Bugzilla.1246 -- Don't use the Callback tab if it hasn't been initialized
	m_PreviewCheck.SetCheck((! m_DefaultState && CCallbacksGui::SSRoiCallbackEnabled(s_CallbackGui))? BST_CHECKED : BST_UNCHECKED);

    bool haveController = GetActiveCamera()->FeatureSupportedInController(FEATURE_FOCUS) ||
                          GetActiveCamera()->FeatureSupportedInController(FEATURE_ZOOM);
    U32 controller;
    if (haveController)
    {
        // We have a controller.  Enumerate all the controllers so we can find the information on this controller
        std::vector<CONTROLLER_INFO> tempControllerInfo;
        U32 uMaxNumberOfControllers;
	    PXL_ERROR_CHECK(PxLGetNumberControllers(NULL, sizeof(CONTROLLER_INFO), &uMaxNumberOfControllers));
	    if (uMaxNumberOfControllers > 0) {
            tempControllerInfo.resize(uMaxNumberOfControllers);
		    memset(&tempControllerInfo[0], 0, sizeof(tempControllerInfo[0]) * uMaxNumberOfControllers);
		    PXL_ERROR_CHECK(PxLGetNumberControllers(&tempControllerInfo[0], sizeof(tempControllerInfo[0]), &uMaxNumberOfControllers));

            // 
            // Step 2
            //      Find our controller
            U32 me = GetActiveCamera()->GetSerialNumber();
            for (controller=0; controller<uMaxNumberOfControllers; controller++) {
                if (tempControllerInfo[controller].CameraSerialNumber == me &&
                    tempControllerInfo[controller].TypeMask & (CONTROLLER_FLAG_FOCUS | CONTROLLER_FLAG_ZOOM)) break;
            }
            if (controller < uMaxNumberOfControllers)
            {
                m_MyController = tempControllerInfo[controller].ControllerSerialNumber;
            } else {
                // bugzilla.1247
                //      We could not open the controller to get a serial number.  We have lost communications
                //      with our controller.  Release it.
                if (m_MyController)
                {
                    PXL_ERROR_CHECK(PxLUnassignController (GetActiveCamera()->GetHandle(), m_MyController));
                    m_MyController = 0;
                }
            }
        }
    } else {
        // We don't have a controller.  Do we want one?  We dont want one if the camera is already controlling controller type features.
        m_bDontWantController = GetActiveCamera()->FeatureSupported(FEATURE_FOCUS) || GetActiveCamera()->FeatureSupported(FEATURE_ZOOM);
    }

    PopulateControllerSelect();

    m_DefaultState = false;
}

/**
* Function: PopulateSubwindow
* Purpose:  Set the subwindow control to reflect the current value of the ROI
*           in the active camera.
*/
void CLensGui::PopulateSubwindow(void)
{
	if (GetActiveCamera()->FeatureSupported(FEATURE_SHARPNESS_SCORE))
	{
		try
		{
			if (!m_bSsRoiConfigured)
				ConfigureSsRoi();

			CRect ssRoi = GetActiveCamera()->GetRoi(FEATURE_SHARPNESS_SCORE);
			CRect roi   = GetActiveCamera()->GetRoi(FEATURE_ROI);

			// Set the subwindow control to display the correct subwindow.
			TempVal<bool> temp(m_bAdjusting[FEATURE_SHARPNESS_SCORE], true); // SetSubwindow *always* emits an OnChange event.
			m_SubwindowCtrl.SetSubwindow(
									ssRoi.left,
									ssRoi.top,
									ssRoi.right - ssRoi.left,		// width
									ssRoi.bottom - ssRoi.top);		// height

            SetIntText(m_RoiWidthEdit, roi.right - roi.left);	
            SetIntText(m_RoiHeightEdit, roi.bottom - roi.top);	        
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


/**
* Function: PopulateControllerSelect
* Purpose:  Set the Controller Selelct dropdown to either 'No Controller Assigned', or to
*           the controller that is assigned to this camera
*/
void CLensGui::PopulateControllerSelect(void)
{
    //
    // Step 1
    //      Rebuild the camera select with at least the 'No Controller Assigned' option
    m_ControllerSelectCombo.ResetContent();
    m_ControllerSelectCombo.AddString(_T("No Controller Assigned"));
    m_ControllerSelectCombo.SetItemData(0, 0);  // the first item is always no contrller, represented as s/n 0
    m_ControllerSelectCombo.SetCurSel(0);
    m_CantOpenComWarningStatic.ShowWindow(SW_HIDE); //Bugzilla.1312

    //
    // Step 2
    //      If we have a controller, enumerate all of them to get info on the contoller
    int controller = 0;
    U32 me = GetActiveCamera()->GetSerialNumber();
    std::vector<CONTROLLER_INFO> tempControllerInfo;
    U32 uMaxNumberOfControllers;
    PXL_ERROR_CHECK(PxLGetNumberControllers(NULL, sizeof(CONTROLLER_INFO), &uMaxNumberOfControllers));
	if (uMaxNumberOfControllers > 0) {
        tempControllerInfo.resize(uMaxNumberOfControllers);
	    memset(&tempControllerInfo[0], 0, sizeof(tempControllerInfo[0]) * uMaxNumberOfControllers);
	    PXL_ERROR_CHECK(PxLGetNumberControllers(&tempControllerInfo[0], sizeof(tempControllerInfo[0]), &uMaxNumberOfControllers));
    }
    if (m_MyController != 0) {

        // 
        // Step 3
        //      Find our controller
        for (controller=0; controller<uMaxNumberOfControllers; controller++) {
            if (tempControllerInfo[controller].CameraSerialNumber == me &&
                tempControllerInfo[controller].TypeMask & (CONTROLLER_FLAG_FOCUS | CONTROLLER_FLAG_ZOOM)) {
                int j;
                CString s;
	            s.Format(_T("%s - %s  S/N: %lu"), 
                         tempControllerInfo[controller].VendorName,
                         tempControllerInfo[controller].ModelName,
                         tempControllerInfo[controller].ControllerSerialNumber);
                j = m_ControllerSelectCombo.AddString(s);  // its sorted, so it will be placed it the correct location
	            m_ControllerSelectCombo.SetItemData(j, static_cast<DWORD_PTR>(tempControllerInfo[controller].ControllerSerialNumber));
                m_ControllerSelectCombo.SetCurSel(j);
                return;
            }
        }
        ASSERT (controller < uMaxNumberOfControllers);
    } else if (uMaxNumberOfControllers > 0){
        //
        // Step 4
        //     We don't have a controller, yet there is at least one controller.  If one of the controller has a serial number of 0, it's an indication that
        //     the controller could not be opened, this is most certainly because some other applcation has the COM port open.  In that case, display a 
        //     warning
        for (controller=0; controller<uMaxNumberOfControllers; controller++) {
            if (tempControllerInfo[controller].ControllerSerialNumber == 0) {
                m_CantOpenComWarningStatic.ShowWindow(SW_SHOW); //Bugzilla.1312
            }
        }
    }
}


/**
/**
* Function: FeatureChanged
* Purpose:  This method is called by the base class (CDevAppPage) as a result
*           of a call to the View class's OnUpdate method (which is called by
*           the Document class as a result of a change to its data).
*/
void CLensGui::FeatureChanged(U32 featureId)
{
	switch (featureId)
	{
	case FEATURE_ROI:
	case FEATURE_SHARPNESS_SCORE:
		PopulateSubwindow();
		PopulateWindowSize(m_WindowSizeCombo, true, FEATURE_SHARPNESS_SCORE);
		break;
	case FEATURE_ROTATE:
		m_bSsRoiConfigured = false;
		PopulateSubwindow();
		PopulateWindowSize(m_WindowSizeCombo, true, FEATURE_SHARPNESS_SCORE);
		break;
	case FEATURE_FLIP:
		PopulateSubwindow();
		PopulateWindowSize(m_WindowSizeCombo, true, FEATURE_SHARPNESS_SCORE);
		break;
	case FEATURE_FOCUS:
		PopulateFocusControl();
		break;
	case FEATURE_ZOOM:
		PopulateZoomControl();
		break;
	default:
		// A feature that is not relevant to this page. Ignore the change.
		break;
	}
}


/******************************************************************************
 * CLensGui message handlers
 *****************************************************************************/

/**
* Function: OnClickSharpnessScoreCheck
* Purpose:  
*/
void CLensGui::OnClickSharpnessScoreCheck(void)
{
	CWaitCursor wc;
	bool ssEnabled = m_SharpnessScoreCheck.GetCheck() == BST_CHECKED;

	try
	{
		GetActiveCamera()->SetOnOff(FEATURE_SHARPNESS_SCORE, ssEnabled);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}

	m_SubwindowCtrl.put_Enabled(ssEnabled);
	m_TopEdit.EnableWindow(ssEnabled);
	m_LeftEdit.EnableWindow(ssEnabled);
	m_WidthEdit.EnableWindow(ssEnabled);
	m_HeightEdit.EnableWindow(ssEnabled);
	m_WindowSizeCombo.EnableWindow(ssEnabled);
	m_CenterButton.EnableWindow(ssEnabled);
	
	m_PreviewCheck.EnableWindow(ssEnabled);
	
}

/**
* Function: OnClickSharpnessScoreCheck
* Purpose:  
*/
void CLensGui::OnClickPreviewCheck(void)
{
	CWaitCursor wc;
	bool previewEnabled = m_PreviewCheck.GetCheck() == BST_CHECKED;

	try
	{
		CCallbacksGui::EnableSSRoiCallback (previewEnabled, s_CallbackGui);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}

/**
* Function: OnSubwindowChanged
* Purpose:  Event handler for the SubwindowChanged event, which is emitted by
*           the PxLSubwindow control at the completion of a move or resize
*           operation.
*/
void CLensGui::OnSubwindowChanged(long left, long top, long width, long height)
{
	// Make sure the edits are set properly (they should be - OnSubwindowChanging updates them)
	m_SubwindowCtrl.UpdateEdits(left, top, width, height);

	// Update the camera.
	if (!m_bAdjusting[FEATURE_SHARPNESS_SCORE])
	{
		try
		{
			GetActiveCamera()->SetRoi(left, top, width, height,
									  theApp.m_bAutoResetPreview, FEATURE_SHARPNESS_SCORE);
		}
		catch (PxLException const& e)
		{
			if (e.GetReturnCode() == ApiInvalidParameterError)
			{
			    // These are expected, it's quite possible that the user will try to
			    // set the rubwinds larger than possible for SSRoi.  Quietly eat
			    // these errors.
			} else {
			    OnApiError(e);
			}
		}
	}
}


/**
* Function: OnKillFocusSubwindowTopEdit
* Purpose:  
*/
void CLensGui::OnKillFocusSubwindowTopEdit()
{
	m_SubwindowCtrl.UpdateFromEdits(CSubwindowCtrl::WND_TOP);
}


/**
* Function: OnKillFocusSubwindowLeftEdit
* Purpose:  
*/
void CLensGui::OnKillFocusSubwindowLeftEdit()
{
	m_SubwindowCtrl.UpdateFromEdits(CSubwindowCtrl::WND_LEFT);
}


/**
* Function: OnKillFocusSubwindowWidthEdit
* Purpose:  
*/
void CLensGui::OnKillFocusSubwindowWidthEdit()
{
	m_SubwindowCtrl.UpdateFromEdits(CSubwindowCtrl::WND_WIDTH);
}


/**
* Function: OnKillFocusSubwindowHeightEdit
* Purpose:  
*/
void CLensGui::OnKillFocusSubwindowHeightEdit()
{
	m_SubwindowCtrl.UpdateFromEdits(CSubwindowCtrl::WND_HEIGHT);
}


void CLensGui::OnSetFocusWindowLeftEdit()
{
	m_LeftEdit.SetSel(0, 99);
}

void CLensGui::OnSetFocusWindowTopEdit()
{
	m_TopEdit.SetSel(0, 99);
}

void CLensGui::OnSetFocusWindowWidthEdit()
{
	m_WidthEdit.SetSel(0, 99);
}

void CLensGui::OnSetFocusWindowHeightEdit()
{
	m_HeightEdit.SetSel(0, 99);
}


/**
* Function: OnClickCenterButton
* Purpose:  Center the Region of Interest in the available space, without
*           changing its size.
*/
void CLensGui::OnClickCenterButton(void)
{
	CWaitCursor wc;

	long dummy, width, height, maxwidth, maxheight;

	m_SubwindowCtrl.GetSubwindow(&dummy, &dummy, &width, &height);
	m_SubwindowCtrl.GetBounds(&maxwidth, &maxheight);

	if (width < maxwidth || height < maxheight)
	{
		m_SubwindowCtrl.SetSubwindow((maxwidth - width)/2,
									 (maxheight - height)/2,
									 width, height);
	}
}

/**
* Function: OnSelchangeWindowsizeCombo
* Purpose:  Event handler for the Change event of the "Standard Sizes" combo
*           box. Sets the PxLSubwindow control to the specified size.
*/
void CLensGui::OnSelchangeWindowsizeCombo(void)
{
	WindowsizeComboChanged(m_WindowSizeCombo, FEATURE_SHARPNESS_SCORE);
}

/**
* Function: OnDropdownControllerSelectCombo
* Purpose:  
*/
void CLensGui::OnDropdownControllerSelectCombo()
{
	// Refresh the list of controllers.
	try
	{
		CWaitCursor waitForIt;
        
        //
        // Step 1
        //      We may already have a controller -- get some info so we can detect this
        CString  selectedItem;
        U32 me = GetActiveCamera()->GetSerialNumber();

        //
        // Step 2
        //      Clear the list of controllers, and add the No Controller Assigned item (that is always an option)
        m_ControllerSelectCombo.ResetContent();
        selectedItem.Format (_T("No Controller Assigned"));
        m_ControllerSelectCombo.AddString(selectedItem);
        m_ControllerSelectCombo.SetItemData(0, 0);  // no contrller is represented as s/n 0

        //
        // Step 3
        //      Enumerate all controllers
        if (!m_bDontWantController) {
            std::vector<CONTROLLER_INFO> tempControllerInfo;
            U32 uMaxNumberOfControllers;
	        PXL_ERROR_CHECK(PxLGetNumberControllers(NULL, sizeof(CONTROLLER_INFO), &uMaxNumberOfControllers));
	        if (uMaxNumberOfControllers > 0) {
                tempControllerInfo.resize(uMaxNumberOfControllers);
		        memset(&tempControllerInfo[0], 0, sizeof(tempControllerInfo[0]) * uMaxNumberOfControllers);
		        PXL_ERROR_CHECK(PxLGetNumberControllers(&tempControllerInfo[0], sizeof(tempControllerInfo[0]), &uMaxNumberOfControllers));
            }

            //
            // Step 4
            //      Add the controlles to the drop down
            for (U32 i=0; i<uMaxNumberOfControllers; i++) {
                // Skip any controllers that cannot access, be it because they are open by another application, or are 
                // are already assigned to another camera
                if (tempControllerInfo[i].ControllerSerialNumber == 0 || // Another App has it open 
                    (tempControllerInfo[i].CameraSerialNumber != 0 &&
                    tempControllerInfo[i].CameraSerialNumber != me) ||   // Someone else already has it
                    !(tempControllerInfo[i].TypeMask & (CONTROLLER_FLAG_FOCUS | CONTROLLER_FLAG_ZOOM))) continue; // Not a zoom/focus controller
                int j;
                CString s;
		        s.Format(_T("%s - %s  S/N: %lu"), 
                          tempControllerInfo[i].VendorName,
                          tempControllerInfo[i].ModelName,
                          tempControllerInfo[i].ControllerSerialNumber);
                j = m_ControllerSelectCombo.AddString(s);
		        m_ControllerSelectCombo.SetItemData(j, static_cast<DWORD_PTR>(tempControllerInfo[i].ControllerSerialNumber));
                if (tempControllerInfo[i].CameraSerialNumber == me) {
                    selectedItem = s;
                }
            }
        }

        //
        // Step 5
        //      'Highlight' the current camera
        m_ControllerSelectCombo.SetCurSel (m_ControllerSelectCombo.FindString(0, selectedItem));

	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}

/**
* Function: OnSelchangeControllerSelectCombo
* Purpose:  
*/
void CLensGui::OnSelchangeControllerSelectCombo(void)
{
    if (m_ControllerSelectCombo.GetCurSel() == -1)
		return;

	try
	{
        U32 selectedController = ItemData<U32>(m_ControllerSelectCombo);
        bool guiUpdateRequired = false;

        if (selectedController == 0) {
            if (m_MyController != 0) {
                // We no longer need this controller
                PXL_ERROR_CHECK(PxLUnassignController (GetActiveCamera()->GetHandle(), m_MyController));
                m_MyController = 0;
                guiUpdateRequired = true;
            }
        } else {
            guiUpdateRequired = m_MyController != selectedController;
            // If we already have a differnt controller, release it.
            if (m_MyController != 0 && m_MyController != selectedController) {
                PXL_ERROR_CHECK(PxLUnassignController (GetActiveCamera()->GetHandle(), m_MyController));
                m_MyController = 0;
            }

            // Grab the selected controller if we don't already have it
            if (m_MyController != selectedController) {
                PXL_ERROR_CHECK(PxLAssignController (GetActiveCamera()->GetHandle(), selectedController));
                m_MyController = selectedController;
            }
        }

        if (guiUpdateRequired) {
            // We need to update the GUI so that the Focus / Zoom controls will be 
            // updated.  However, even before doing that, read the min value for each
            // of the target features, just so the cache gets updated 
            float tempMin;
            tempMin = GetActiveCamera()->GetFeatureMinVal (FEATURE_FOCUS);
            tempMin = GetActiveCamera()->GetFeatureMinVal (FEATURE_ZOOM);

            SetGUIState (GetDocument()->GetGUIState());   

            PopulateFocusControl();
            PopulateZoomControl();

        }
    }
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}

/**
* Function: OnClickFocusLowerButton
* Purpose:  
*/
void CLensGui::OnClickFocusLowerButton()
{
	CWaitCursor wc;

    GetActiveCamera()->SetLimit (FEATURE_FOCUS, true);
    PopulateFocusControl();
}

/**
* Function: OnClickFocusUpperButton
* Purpose:  
*/
void CLensGui::OnClickFocusUpperButton()
{
	CWaitCursor wc;

    GetActiveCamera()->SetLimit (FEATURE_FOCUS, false);
    PopulateFocusControl();
}

/**
* Function: OnClickZoomLowerButton
* Purpose:  
*/
void CLensGui::OnClickZoomLowerButton()
{
	CWaitCursor wc;

    GetActiveCamera()->SetLimit (FEATURE_ZOOM, true);
    PopulateZoomControl();
}

/**
* Function: OnClickZoomUpperButton
* Purpose:  
*/
void CLensGui::OnClickZoomUpperButton()
{
	CWaitCursor wc;

    GetActiveCamera()->SetLimit (FEATURE_ZOOM, false);
    PopulateZoomControl();
}

/******************************************************************************
* Note on Slider controls:
* The slider control emits a strange combination of Scroll and Change events
* whose logic I haven't entirely figured out yet. For most purposes, I think 
* you could get away with handling only one or the other. 
* Here are the trade-offs:
* Handling OnChange only: You get an event only at the end of a drag operation, 
* instead of continuously throughout it. If this is what you want, OnChange is
* your Huckleberry.
* Handling OnScroll only: Works for almost everything - the exception is
* that you don't get a Scroll event when you press the home or end keys. The
* pointer goes to one end of the slider, but strangly you just get *two*
* Change events, and no Scroll event.
******************************************************************************/

SLIDER_CONTROL_HANDLER_IMPL(CLensGui,FocusControl,FOCUS)
SLIDER_CONTROL_HANDLER_IMPL(CLensGui,ZoomControl,ZOOM)














