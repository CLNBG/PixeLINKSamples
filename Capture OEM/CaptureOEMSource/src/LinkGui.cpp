//
// RoiGui.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "LinkGui.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/******************************************************************************
 * CRoiGui - Member functions.
 *****************************************************************************/

IMPLEMENT_DYNCREATE(CLinkGui, CDevAppPage)

CLinkGui::CLinkGui(CPxLDevAppView* pView)
	: CDevAppPage(CLinkGui::IDD, pView)
{
}

CLinkGui::~CLinkGui()
{
}

void CLinkGui::DoDataExchange(CDataExchange* pDX)
{
	CDevAppPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RG_CAMERASELECT_COMBO, m_CameraSelectCombo);
	DDX_Control(pDX, IDC_RG_PLAY_BUTTON, m_PlayButton);
	DDX_Control(pDX, IDC_RG_PAUSE_BUTTON, m_PauseButton);
	DDX_Control(pDX, IDC_RG_STOP_BUTTON, m_StopButton);
	
	DDX_Control(pDX, IDC_SG_FIREWALLWARNING_STATIC, m_FirewallWarningStatic);
	DDX_Control(pDX, IDC_SG_MAXPACKETSIZEAUTO_BUTTON, m_MaxPacketSizeAutoButton);
	DDX_Control(pDX, IDC_SG_MAXPACKETSIZE_EDIT, m_MaxPacketSizeEdit);

	DDX_SLIDER_SUITE(SG_BANDWIDTH_LIMIT,BandwidthLimit)

	DDX_Control(pDX, IDC_SG_BANDWIDTH_LIMIT_ONOFF_CHECK, m_BandwidthLimitOnOffCheck);
	DDX_Control(pDX, IDC_SG_FPSWARNING_BW_STATIC, m_FpsWarningBwStatic);
	DDX_Control(pDX, IDC_SG_LINKSPEEDWARNING_BW_STATIC, m_LinkSpeedWarningBwStatic);
}


BEGIN_MESSAGE_MAP(CLinkGui, CDevAppPage)

	ON_CBN_DROPDOWN(IDC_RG_CAMERASELECT_COMBO, OnDropdownCameraSelectCombo)
	ON_CBN_SELCHANGE(IDC_RG_CAMERASELECT_COMBO, OnSelchangeCameraSelectCombo)
	ON_BN_CLICKED(IDC_RG_PLAY_BUTTON, OnClickPlayButton)
	ON_BN_CLICKED(IDC_RG_PAUSE_BUTTON, OnClickPauseButton)
	ON_BN_CLICKED(IDC_RG_STOP_BUTTON, OnClickStopButton)

	ON_EN_KILLFOCUS(IDC_SG_MAXPACKETSIZE_EDIT, OnKillFocusMaxPacketSizeEdit)
	ON_BN_CLICKED(IDC_SG_MAXPACKETSIZEAUTO_BUTTON, OnClickMaxPacketSizeAutoButton)

	MM_SLIDER_SUITE(SG_BANDWIDTH_LIMIT,BandwidthLimit)
	ON_BN_CLICKED(IDC_SG_BANDWIDTH_LIMIT_ONOFF_CHECK, OnClickBandwidthLimitOnOffCheck)

END_MESSAGE_MAP()


BEGIN_EVENTSINK_MAP(CLinkGui, CDevAppPage)
	EVENTSINK_SLIDER_SUITE(CLinkGui,SG_BANDWIDTH_LIMIT,BandwidthLimit)
END_EVENTSINK_MAP()


/**
* Function: GetHelpContextId
* Purpose:  
*/
int CLinkGui::GetHelpContextId()
{
	return IDH_LINK_TAB;
}


/**
* Function: OnInitDialog
* Purpose:  
*/
BOOL CLinkGui::OnInitDialog()
{
	CDevAppPage::OnInitDialog();

	m_BandwidthLimitSlider.SetLinkedWindows(&m_BandwidthLimitEdit, &m_BandwidthLimitMinLabel, &m_BandwidthLimitMaxLabel, &m_BandwidthLimitFrame);

	return TRUE;  // return TRUE unless you set the focus to a control
}


/**
* Function: OnSetActive
* Purpose:  
*/
BOOL CLinkGui::OnSetActive()
{
	return CDevAppPage::OnSetActive();
}


/**
* Function: SetGUIState
* Purpose:  Enable / Disable controls, according to the state of the camera.
*           Most controls cannot be used in any meaningful way until a camera
*           is initialized.
*/
void CLinkGui::SetGUIState(eGUIState state)
{
	// The CameraSelect and Play/Pause/Stop button states are set in the base
	// class version of this function.
	CDevAppPage::SetGUIState(state);

	CPxLCamera* cam = GetActiveCamera();

	bool enable = (state != GS_NoCamera);

	bool enableFeature = enable && cam->FeatureSupportsManual(FEATURE_MAX_PACKET_SIZE);
	m_MaxPacketSizeAutoButton.EnableWindow(enableFeature && 
                                           cam->FeatureSupportsOnePush(FEATURE_MAX_PACKET_SIZE) &&
                                           ! cam->IsFirewallBlocked()); // Don't allow auto adjustments if we have a firewall issue, as it won't work.
	m_MaxPacketSizeEdit.EnableWindow(enableFeature);

	ENABLE_SLIDER_SUITE(BandwidthLimit,BANDWIDTH_LIMIT)
	m_BandwidthLimitOnOffCheck.EnableWindow(enable && cam->FeatureSupportsOnOff(FEATURE_BANDWIDTH_LIMIT));
}


/**
* Function: ConfigureControls
* Purpose:  Set up the controls on the page to customize them according to the
*           currently active camera. This involves things like setting the max
*           and min values for sliders and setting the bounds of the subwindow
*           control.
*/
void CLinkGui::ConfigureControls(void)
{
	SetSliderRange(m_BandwidthLimitSlider, FEATURE_BANDWIDTH_LIMIT);
}

/**
* Function: PopulateDefaults
* Purpose:  Put some meaningles default values into the controls, for purely
*           aesthetic reasons.
*/
void CLinkGui::PopulateDefaults(void)
{
    m_MaxPacketSizeEdit.Clear();

    m_FirewallWarningStatic.ShowWindow(SW_HIDE);
    
    SetSliderDefault(m_BandwidthLimitSlider);
	m_BandwidthLimitOnOffCheck.SetCheck(BST_UNCHECKED);

    m_FpsWarningBwStatic.ShowWindow(SW_HIDE);
    m_LinkSpeedWarningBwStatic.ShowWindow(SW_HIDE);
}


/**
* Function: PopulateControls
* Purpose:  Make the values displayed in the controls reflect the current 
*           settings of the active camera.  Called immediately after a 
*           camera is initialized.
*/
void CLinkGui::PopulateControls(void)
{
	CDevAppPage::PopulateControls();

	PopulateFirewallWarning();
	PopulatePacketSize();

    // Only populate these controls if they are suported by the camera
    if (GetActiveCamera()->FeatureSupported(FEATURE_BANDWIDTH_LIMIT))
    {
	    PopulateBandwidthLimit();
	    PopulateBandwidthLimitOnOff();
	    PopulateFpsBwWarning();
	}
}

/**
* Function: PopulatePacketSize
* Purpose:  
*/
void CLinkGui::PopulatePacketSize(void)
{
	try
	{
		if (GetActiveCamera()->FeatureSupported(FEATURE_MAX_PACKET_SIZE))
		{
			int maxPacketSize = static_cast<int>(GetActiveCamera()->GetFeatureValue(FEATURE_MAX_PACKET_SIZE));

            SetIntText(m_MaxPacketSizeEdit, maxPacketSize);	

		}
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}

/**
* Function: PopulateBandwidthlimitOnOff
* Purpose:  
*/
void CLinkGui::PopulateBandwidthLimitOnOff(void)
{
	if (GetActiveCamera()->FeatureSupported(FEATURE_BANDWIDTH_LIMIT))
	{
		try
		{
			TempVal<bool> temp(m_bAdjusting[FEATURE_BANDWIDTH_LIMIT], true);

			bool off = GetActiveCamera()->IsFeatureFlagSet(FEATURE_BANDWIDTH_LIMIT, FEATURE_FLAG_OFF);
			m_BandwidthLimitOnOffCheck.SetCheck(!off);
			m_BandwidthLimitSlider.EnableWindow(!off);
			m_BandwidthLimitEdit.EnableWindow(!off);
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}

// The following will warn the user if the bandwidth limit is limiting the actual frame rate.
void CLinkGui::PopulateFpsBwWarning(void)
{
	if (GetActiveCamera()->FeatureSupported(FEATURE_ACTUAL_FRAME_RATE) &&
	    GetActiveCamera()->FeatureSupported(FEATURE_FRAME_RATE) && 
	    GetActiveCamera()->FeatureSupported(FEATURE_BANDWIDTH_LIMIT))
	{
		try
		{
			bool warningRequired = false;
      		U32   fr_flags = 0;
      		U32   other_flags = 0;
		    float targetFr = GetActiveCamera()->GetFeatureValue(FEATURE_FRAME_RATE, &fr_flags);
		    float actualFr = GetActiveCamera()->GetFeatureValue(FEATURE_ACTUAL_FRAME_RATE, &other_flags);
			    
			if (fr_flags | FEATURE_FLAG_OFF)
			{
		        // Only display the warning if the user is controlling frame rate, 
		        // and it is in fact the bandwidth limit that is limiting the frame rate (as opposed to say, 
		        // the exposure).
		        float BandWidthLimit = GetActiveCamera()->GetFeatureValue(FEATURE_BANDWIDTH_LIMIT, &other_flags);
		        BandWidthLimit = BandWidthLimit * (1000000.0f / 8.0f); // convert from mbps to Bps
		        CRect Roi = GetActiveCamera()->GetFeatureRect(FEATURE_ROI);
		        int PixelFormat = static_cast<int>(GetActiveCamera()->GetFeatureValue(FEATURE_PIXEL_FORMAT, &other_flags));
		        float PixelSize = GetPixelFormatSize(PixelFormat);
		        PixelAddressing Pa = GetActiveCamera()->GetPixelAddressing();
		        
		        float ImageSize = (static_cast<float>((Roi.Width()/Pa.x) * (Roi.Height()/Pa.y))) * PixelSize;
		        warningRequired = ((actualFr < targetFr) && (BandWidthLimit < (targetFr*ImageSize)));
			}
		    m_FpsWarningBwStatic.ShowWindow(warningRequired ? SW_SHOW : SW_HIDE);
		    m_LinkSpeedWarningBwStatic.ShowWindow(GetActiveCamera()->IsLinkSpeedReduced() ? SW_SHOW : SW_HIDE);
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}

// The following will warn the user if camera is connected on the otherside of a firewall (IE, being blocked by a firewall).
void CLinkGui::PopulateFirewallWarning(void)
{
    m_FirewallWarningStatic.ShowWindow(GetActiveCamera()->IsFirewallBlocked() ? SW_SHOW : SW_HIDE);
}

/**
* Function: FeatureChanged
* Purpose:  This method is called by the base class (CDevAppPage) as a result
*           of a call to the View class's OnUpdate method (which is called by
*           the Document class as a result of a change to its data).
*/
void CLinkGui::FeatureChanged(U32 featureId)
{
	switch (featureId)
	{
	case FEATURE_MAX_PACKET_SIZE:
		PopulatePacketSize();
		break;
	case FEATURE_BANDWIDTH_LIMIT:
		PopulateBandwidthLimit();
		PopulateBandwidthLimitOnOff();
		PopulateFpsBwWarning();
		break;
	case FEATURE_SHUTTER:
		PopulateFpsBwWarning();
        break;		
	default:
		// A feature that is not relevant to this page. Ignore the change.
		break;
	}
}


/**
* Function: OnSetFocusMaxPacketSizeEdit
* Purpose:  
*/
void CLinkGui::OnSetFocusMaxPacketSizeEdit(void)
{
	m_MaxPacketSizeEdit.SetSel(0, 99);
}

/**
* Function: OnKillFocusMaxPacketSizeEdit
* Purpose:  
*/
void CLinkGui::OnKillFocusMaxPacketSizeEdit(void)
{
	int val = WindowTextToInt(m_MaxPacketSizeEdit);
	if (val > 0)
    {
	    try
	    {
		    if (GetActiveCamera()->FeatureSupported(FEATURE_MAX_PACKET_SIZE))
		    {
			    GetActiveCamera()->SetFeatureValue(FEATURE_MAX_PACKET_SIZE, static_cast<float> (val));
		    }
	    }
	    catch (PxLException const& e)
	    {
		    OnApiError(e);
	    }
    }
}

/**
* Function: OnClickMaxPacketSizeAutoButton
* Purpose:  
*/
void CLinkGui::OnClickMaxPacketSizeAutoButton(void)
{
	CWaitCursor wc;
	try
	{
		GetActiveCamera()->SetOnePush(FEATURE_MAX_PACKET_SIZE);
		PopulatePacketSize();
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}

/******************************************************************************
 * CLinkGui message handlers
 *****************************************************************************/

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

SLIDER_CONTROL_HANDLER_IMPL(CLinkGui,BandwidthLimit,BANDWIDTH_LIMIT)

/**
* Function: OnClickBandwidthLimitOnOffCheck
* Purpose:  
*/
void CLinkGui::OnClickBandwidthLimitOnOffCheck(void)
{
	CWaitCursor wc;
	if (!m_bAdjusting[FEATURE_BANDWIDTH_LIMIT])
	{
		try
		{
			GetActiveCamera()->SetOnOff(FEATURE_BANDWIDTH_LIMIT, m_BandwidthLimitOnOffCheck.GetCheck() == BST_CHECKED);
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}








