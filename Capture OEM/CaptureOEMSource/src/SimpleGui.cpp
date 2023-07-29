// SimpleGui.cpp : implementation file

#include "stdafx.h"
#include "PxLDevApp.h"
#include "SimpleGui.h"
#include "RoiDialog2.h"

#include "Helpers.h"
#include <algorithm>
#include <functional>

#include "PxLDevAppView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/******************************************************************************
* CSimpleGui - Member functions
******************************************************************************/

IMPLEMENT_DYNCREATE(CSimpleGui, CDevAppPage)

/**
 * Function: CSimpleGui::CSimpleGui
 * Purpose:  Constructor
 */
CSimpleGui::CSimpleGui(CPxLDevAppView* pView) 
	: CDevAppPage(CSimpleGui::IDD, pView)
	, m_exposureTimerId(0)
	, m_gainTimerId(0)
	, m_pRoiDlg(NULL)
    , m_AutoExposeMinStale(false)
    , m_AutoExposeMaxStale(false)
    , m_GainsKnown(false)
{
}


/**
 * Function: CSimpleGui::~CSimpleGui
 * Purpose:  Destructor
 */
CSimpleGui::~CSimpleGui()
{
}


/**
 * Function: DoDataExchange
 * Purpose:  Generated code for associating GUI elements with C++ variables.
 */
void CSimpleGui::DoDataExchange(CDataExchange* pDX)
{
	CDevAppPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SG_CAMERASELECT_COMBO, m_CameraSelectCombo);
	DDX_Control(pDX, IDC_SG_PLAY_BUTTON, m_PlayButton);
	DDX_Control(pDX, IDC_SG_PAUSE_BUTTON, m_PauseButton);
	DDX_Control(pDX, IDC_SG_STOP_BUTTON, m_StopButton);

	DDX_SLIDER_SUITE(SG_EXPOSURE,Exposure)
	DDX_SLIDER_SUITE(SG_GAIN,Gain)
	DDX_SLIDER_SUITE(SG_SATURATION,Saturation)
	DDX_SLIDER_SUITE(SG_BRIGHTNESS,Brightness)
	DDX_SLIDER_SUITE(SG_GAMMA,Gamma)
	DDX_SLIDER_SUITE(SG_FRAMERATE,FrameRate)
	DDX_SLIDER_SUITE(SG_COLORTEMP,ColorTemp)

   	DDX_Control(pDX, IDC_SG_EXPOSUREAUTOMIN_LABEL, m_AutoExposureMin);
	DDX_Control(pDX, IDC_SG_EXPOSUREAUTOMAX_LABEL, m_AutoExposureMax);
	DDX_Control(pDX, IDC_SG_EXPOSUREAUTOMIN_BUTTON, m_AutoExposureMinButton);
	DDX_Control(pDX, IDC_SG_EXPOSUREAUTOMAX_BUTTON, m_AutoExposureMaxButton);

	DDX_Control(pDX, IDC_SG_ACTUALFRAMERATE_EDIT, m_ActualFrameRateEdit);
	DDX_Control(pDX, IDC_SG_FPSWARNING_STATIC, m_FpsWarningStatic);
	DDX_Control(pDX, IDC_SG_LINKSPEEDWARNING_STATIC, m_LinkSpeedWarningStatic);

	DDX_Control(pDX, IDC_SG_EXPOSUREAUTO_CHECK, m_ExposureAutoCheck);
	DDX_Control(pDX, IDC_SG_GAINAUTO_CHECK, m_GainAutoCheck);
	DDX_Control(pDX, IDC_SG_GAMMAONOFF_CHECK, m_GammaOnOffCheck);
	DDX_Control(pDX, IDC_SG_WHITESHADINGAUTO_BUTTON, m_WhiteShadingAutoButton);

	DDX_Control(pDX, IDC_SG_HDR_COMBO, m_HDRCombo);

    DDX_Control(pDX, IDC_SG_WINDOWSIZE_COMBO, m_WindowSizeCombo);

	DDX_Control(pDX, IDC_SG_IMAGEFILENAME_EDIT, m_ImageFileNameEdit);
	DDX_Control(pDX, IDC_SG_IMAGEFILEBROWSE_BUTTON, m_ImageFileBrowseButton);
	DDX_Control(pDX, IDC_SG_IMAGEFILEFORMAT_COMBO, m_ImageFileFormatCombo);
	DDX_Control(pDX, IDC_SG_NUMFRAMES_EDIT, m_NumFramesEdit);
	DDX_Control(pDX, IDC_SG_IMAGECAPTURE_BUTTON, m_ImageCaptureButton);

	DDX_Control(pDX, IDC_SG_INCREMENTFILENAME_CHECK, m_IncrementAfterCaptureCheck);
	DDX_Control(pDX, IDC_SG_PREVIEWAFTERCAPTURE_CHECK, m_PreviewAfterCaptureCheck);
	DDX_Control(pDX, IDC_SG_CAPTUREFULLFRAME_CHECK, m_CaptureFullFrameCheck);
	DDX_Control(pDX, IDC_SG_CAPTUREFULLRES_CHECK, m_CaptureFullResCheck);

	DDX_Control(pDX, IDC_SG_ROIPOPUP_BUTTON, m_RoiPopupButton);

	DDX_Control(pDX, IDC_SG_RED_SLIDER, m_RedSlider);
	DDX_Control(pDX, IDC_SG_RED_EDIT, m_RedEdit);
	DDX_Control(pDX, IDC_SG_GREEN_SLIDER, m_GreenSlider);
	DDX_Control(pDX, IDC_SG_GREEN_EDIT, m_GreenEdit);
	DDX_Control(pDX, IDC_SG_BLUE_SLIDER, m_BlueSlider);
	DDX_Control(pDX, IDC_SG_BLUE_EDIT, m_BlueEdit);
	DDX_Control(pDX, IDC_SG_WHITESHADINGAUTO_BUTTON, m_WhiteShadingAutoButton);
	DDX_Control(pDX, IDC_SG_WHITESHADINGMIN_LABEL, m_WhiteShadingMinLabel);
	DDX_Control(pDX, IDC_SG_WHITESHADINGMAX_LABEL, m_WhiteShadingMaxLabel);
	DDX_Control(pDX, IDC_SG_FRAMERATEONOFF_CHECK, m_FrameRateOnOffCheck);
	DDX_Control(pDX, IDC_SG_COLORTEMPONOFF_CHECK, m_ColorTempOnOffCheck);
}


/**
 * Generated Code.
 */
BEGIN_MESSAGE_MAP(CSimpleGui, CDevAppPage)

	ON_CBN_DROPDOWN(IDC_SG_CAMERASELECT_COMBO, OnDropdownCameraSelectCombo)
	ON_CBN_SELCHANGE(IDC_SG_CAMERASELECT_COMBO, OnSelchangeCameraSelectCombo)
	ON_BN_CLICKED(IDC_SG_PLAY_BUTTON, OnClickPlayButton)
	ON_BN_CLICKED(IDC_SG_PAUSE_BUTTON, OnClickPauseButton)
	ON_BN_CLICKED(IDC_SG_STOP_BUTTON, OnClickStopButton)

	MM_SLIDER_SUITE(SG_EXPOSURE,Exposure)
	MM_SLIDER_SUITE(SG_GAIN,Gain)
	MM_SLIDER_SUITE(SG_SATURATION,Saturation)
	MM_SLIDER_SUITE(SG_BRIGHTNESS,Brightness)
	MM_SLIDER_SUITE(SG_GAMMA,Gamma)
	MM_SLIDER_SUITE(SG_FRAMERATE,FrameRate)
	MM_SLIDER_SUITE(SG_COLORTEMP,ColorTemp)

	ON_BN_CLICKED(IDC_SG_EXPOSUREAUTOMIN_BUTTON, OnClickExposureAutoMinButton)
	ON_BN_CLICKED(IDC_SG_EXPOSUREAUTOMAX_BUTTON, OnClickExposureAutoMaxButton)

    ON_BN_CLICKED(IDC_SG_EXPOSUREAUTO_CHECK, OnClickExposureAutoCheck)
    ON_BN_CLICKED(IDC_SG_GAINAUTO_CHECK, OnClickGainAutoCheck)
	ON_BN_CLICKED(IDC_SG_GAMMAONOFF_CHECK, OnClickGammaOnOffCheck)
	ON_BN_CLICKED(IDC_SG_WHITESHADINGAUTO_BUTTON, OnClickWhiteShadingAutoButton)

	ON_CBN_SELCHANGE(IDC_SG_HDR_COMBO, OnSelchangeHDRCombo)

	ON_CBN_SELCHANGE(IDC_SG_WINDOWSIZE_COMBO, OnSelchangeWindowsizeCombo)

	ON_BN_CLICKED(IDC_SG_WHITESHADINGAUTO_BUTTON, OnClickWhiteShadingAutoButton)
	ON_EN_KILLFOCUS(IDC_SG_RED_EDIT, OnKillFocusRedEdit)
	ON_EN_KILLFOCUS(IDC_SG_GREEN_EDIT, OnKillFocusGreenEdit)
	ON_EN_KILLFOCUS(IDC_SG_BLUE_EDIT, OnKillFocusBlueEdit)

	ON_BN_CLICKED(IDC_SG_COLORTEMPONOFF_CHECK, OnClickColorTempOnOffCheck)
	ON_BN_CLICKED(IDC_SG_FRAMERATEONOFF_CHECK, OnClickFrameRateOnOffCheck)

	ON_BN_CLICKED(IDC_SG_IMAGEFILEBROWSE_BUTTON, OnClickImageFileBrowseButton)
	ON_BN_CLICKED(IDC_SG_IMAGECAPTURE_BUTTON, OnClickImageCaptureButton)
	ON_EN_KILLFOCUS(IDC_SG_IMAGEFILENAME_EDIT, OnKillFocusImageFileNameEdit)
	ON_CBN_SELCHANGE(IDC_SG_IMAGEFILEFORMAT_COMBO, OnSelchangeImageFileFormatCombo)

	ON_UPDATE_COMMAND_UI(IDC_SG_NUMFRAMES_EDIT, OnUpdateNumFramesEdit)

	ON_WM_TIMER()

	ON_BN_CLICKED(IDC_SG_ROIPOPUP_BUTTON, OnClickRoiPopupButton)

END_MESSAGE_MAP()


BEGIN_EVENTSINK_MAP(CSimpleGui, CDevAppPage)

	EVENTSINK_SLIDER_SUITE(CSimpleGui,SG_EXPOSURE,Exposure)
	EVENTSINK_SLIDER_SUITE(CSimpleGui,SG_GAIN,Gain)
	EVENTSINK_SLIDER_SUITE(CSimpleGui,SG_SATURATION,Saturation)
	EVENTSINK_SLIDER_SUITE(CSimpleGui,SG_BRIGHTNESS,Brightness)
	EVENTSINK_SLIDER_SUITE(CSimpleGui,SG_GAMMA,Gamma)
	EVENTSINK_SLIDER_SUITE(CSimpleGui,SG_FRAMERATE,FrameRate)
	EVENTSINK_SLIDER_SUITE(CSimpleGui,SG_COLORTEMP,ColorTemp)

	ON_EVENT(CSimpleGui, IDC_SG_RED_SLIDER, 1, OnScrollRedSlider, VTS_NONE)
	ON_EVENT(CSimpleGui, IDC_SG_RED_SLIDER, 2, OnChangeRedSlider, VTS_NONE)
	ON_EVENT(CSimpleGui, IDC_SG_GREEN_SLIDER, 1, OnScrollGreenSlider, VTS_NONE)
	ON_EVENT(CSimpleGui, IDC_SG_GREEN_SLIDER, 2, OnChangeGreenSlider, VTS_NONE)
	ON_EVENT(CSimpleGui, IDC_SG_BLUE_SLIDER, 1, OnScrollBlueSlider, VTS_NONE)
	ON_EVENT(CSimpleGui, IDC_SG_BLUE_SLIDER, 2, OnChangeBlueSlider, VTS_NONE)

END_EVENTSINK_MAP()


/**
 * Function: GetHelpContextId
 * Purpose:  Used by the help system to determine the help context for the
 *           active tab. Called by the base class (virtually).
 */
int CSimpleGui::GetHelpContextId()
{
	return IDH_SIMPLE_TAB;
}


/**
* Function: OnInitDialog
* Purpose:  
*/
BOOL CSimpleGui::OnInitDialog()
{
	CDevAppPage::OnInitDialog();

	// CSlider is able to exchange data with other controls - an 
	// edit control for the current value, two controls to display the minimum
	// and maximum values of the slider's range, and a frame to display the 
	// multiplication factor.
	m_ExposureSlider.SetLinkedWindows(&m_ExposureEdit, &m_ExposureMinLabel, &m_ExposureMaxLabel, &m_ExposureFrame);
	m_GainSlider.SetLinkedWindows(&m_GainEdit, &m_GainMinLabel, &m_GainMaxLabel, &m_GainFrame);
	m_SaturationSlider.SetLinkedWindows(&m_SaturationEdit, &m_SaturationMinLabel, &m_SaturationMaxLabel, &m_SaturationFrame);
	m_BrightnessSlider.SetLinkedWindows(&m_BrightnessEdit, &m_BrightnessMinLabel, &m_BrightnessMaxLabel, &m_BrightnessFrame);
	m_GammaSlider.SetLinkedWindows(&m_GammaEdit, &m_GammaMinLabel, &m_GammaMaxLabel, &m_GammaFrame);
	m_FrameRateSlider.SetLinkedWindows(&m_FrameRateEdit, &m_FrameRateMinLabel, &m_FrameRateMaxLabel, &m_FrameRateFrame);
	m_ColorTempSlider.SetLinkedWindows(&m_ColorTempEdit, &m_ColorTempMinLabel, &m_ColorTempMaxLabel, &m_ColorTempFrame);

	m_RedSlider.SetLinkedWindows(&m_RedEdit, &m_WhiteShadingMinLabel, &m_WhiteShadingMaxLabel, NULL);
	m_GreenSlider.SetLinkedWindows(&m_GreenEdit, NULL, NULL, NULL);
	m_BlueSlider.SetLinkedWindows(&m_BlueEdit, NULL, NULL, NULL);

	SetCombinedFormatEntries(m_ImageFileFormatCombo);

	m_BrightnessSlider.SetIntegerMode();

	return TRUE;  // return TRUE unless you set the focus to a control
}


/**
* Function: OnSetActive
* Purpose:  
*/
BOOL CSimpleGui::OnSetActive()
{
	// Because this page gets removed while the application is in Advanced
	// mode, it needs to be reconfigured when we go back in to Simple Mode.
	// Setting m_lastUsedSerialNumber to 0 tricks CDevAppPage::OnSetActive
	// into calling ConfigureControls.
	m_lastUsedSerialNumber = 0;

	if (CDevAppPage::OnSetActive())
	{
		// The options pertaining to clip capture and image capture need to be
		// kept synced with the Clips tab and the Images tab. For example, if the
		// user selects a new path to capture clips to on the Clips tab, then comes
		// back to this tab, we want to display that new path here as well. We use
		// the UserSettings to track these values.
		UserSettings& settings = theApp.GetUserSettings();
		ApplyImageSettings(settings);
		ApplyClipSettings(settings);

		ScrollToEnd(m_ImageFileNameEdit);

		if (GetActiveCamera()->IsFeatureFlagSet(FEATURE_SHUTTER, FEATURE_FLAG_AUTO))
		{
			// Start the timer to do continuous updates of the displayed value
			// of the exposure time.
			StartExposureTimer();
		}

		if (GetActiveCamera()->IsFeatureFlagSet(FEATURE_GAIN, FEATURE_FLAG_AUTO))
		{
			// Start the timer to do continuous updates of the displayed value
			// of the gain time.
			StartGainTimer();
		}

        return TRUE;
	}

	return FALSE;
}


/**
* Function: OnKillActive
* Purpose:  
*/
BOOL CSimpleGui::OnKillActive()
{
	// See comment in OnSetActive, above.
	UserSettings& settings = theApp.GetUserSettings();
	StoreImageSettings(settings);
	StoreClipSettings(settings);

	StopExposureTimer();
	StopGainTimer();

	return CDevAppPage::OnKillActive();
}


/**
* Function: StartExposureTimer
* Purpose:  
*/
void CSimpleGui::StartExposureTimer(void)
{
	if (m_exposureTimerId == 0)
		m_exposureTimerId = SetTimer(AUTO_EXPOSURE_TIMER_ID, 2500, NULL);
}


/**
* Function: StopExposureTimer
* Purpose:  
*/
void CSimpleGui::StopExposureTimer(void)
{
	if (m_exposureTimerId != 0)
		KillTimer(m_exposureTimerId);
	m_exposureTimerId = 0;
}


/**
* Function: StartGainTimer
* Purpose:  
*/
void CSimpleGui::StartGainTimer(void)
{
	if (m_gainTimerId == 0)
		m_gainTimerId = SetTimer(AUTO_GAIN_TIMER_ID, 2500, NULL);
}


/**
* Function: StopGainTimer
* Purpose:  
*/
void CSimpleGui::StopGainTimer(void)
{
	if (m_gainTimerId != 0)
		KillTimer(m_gainTimerId);
	m_gainTimerId = 0;
}


/**
* Function: OnTimer
* Purpose:  
*/
void CSimpleGui::OnTimer(UINT_PTR nIDEvent)
{
	// Update the display of the exposure anf gain time.
    switch ((int)nIDEvent)
    {
    case AUTO_EXPOSURE_TIMER_ID:
		this->FeatureChanged(FEATURE_SHUTTER);
        break;
    case AUTO_GAIN_TIMER_ID:
		this->FeatureChanged(FEATURE_GAIN);
        break;
    default:
        break;
    }

    CDevAppPage::OnTimer(nIDEvent);
}


/**
* Function: ApplyUserSettings
* Purpose:  
*/
void CSimpleGui::ApplyUserSettings(UserSettings& settings)
{
	ApplyImageSettings(settings);
	ApplyClipSettings(settings);
}


/**
* Function: ApplyClipSettings
* Purpose:  
*/
void CSimpleGui::ApplyClipSettings(UserSettings& settings)
{
	CString s;
	s.Format(_T("%d"), settings.GetIntSetting(USI_CLIP_NUM_FRAMES));
	m_NumFramesEdit.SetWindowText(s);
}


/**
* Function: ApplyImageSettings
* Purpose:  
*/
void CSimpleGui::ApplyImageSettings(UserSettings& settings)
{
	m_ImageFileNameEdit.SetWindowText(settings.GetStringSetting(USS_IMAGE_FILE_PATH));
	m_ImageFileFormatCombo.SetCurSel(settings.GetIntSetting(USI_IMAGE_FILE_FORMAT_INDEX));

	m_IncrementAfterCaptureCheck.SetCheck(settings.GetBoolSetting(USB_IMAGE_INCREMENT_AFTER_CAPTURE) ? BST_CHECKED : BST_UNCHECKED);
	m_PreviewAfterCaptureCheck.SetCheck(settings.GetBoolSetting(USB_IMAGE_PREVIEW_AFTER_CAPTURE) ? BST_CHECKED : BST_UNCHECKED);
	m_CaptureFullFrameCheck.SetCheck(settings.GetBoolSetting(USB_IMAGE_CAPTURE_FULL_FRAME) ? BST_CHECKED : BST_UNCHECKED);
	m_CaptureFullResCheck.SetCheck(settings.GetBoolSetting(USB_IMAGE_CAPTURE_FULL_RES) ? BST_CHECKED : BST_UNCHECKED);
}


/**
* Function: StoreUserSettings
* Purpose:  
*/
void CSimpleGui::StoreUserSettings(UserSettings& settings)
{
	StoreImageSettings(settings);
	StoreClipSettings(settings);
}


/**
* Function: StoreImageSettings
* Purpose:  
*/
void CSimpleGui::StoreImageSettings(UserSettings& settings)
{
	settings.SetStringSetting(USS_IMAGE_FILE_PATH, WindowTextAsString(m_ImageFileNameEdit));
	settings.SetIntSetting(USI_IMAGE_FILE_FORMAT_INDEX, m_ImageFileFormatCombo.GetCurSel());

	settings.SetBoolSetting(USB_IMAGE_INCREMENT_AFTER_CAPTURE, BST_CHECKED == m_IncrementAfterCaptureCheck.GetCheck());
	settings.SetBoolSetting(USB_IMAGE_PREVIEW_AFTER_CAPTURE, BST_CHECKED == m_PreviewAfterCaptureCheck.GetCheck());
	settings.SetBoolSetting(USB_IMAGE_CAPTURE_FULL_FRAME, BST_CHECKED == m_CaptureFullFrameCheck.GetCheck());
	settings.SetBoolSetting(USB_IMAGE_CAPTURE_FULL_RES, BST_CHECKED == m_CaptureFullResCheck.GetCheck());
}


/**
* Function: StoreClipSettings
* Purpose:  
*/
void CSimpleGui::StoreClipSettings(UserSettings& settings)
{
	int nFrames = WindowTextToInt(m_NumFramesEdit);
	if (nFrames > 0)
		settings.SetIntSetting(USI_CLIP_NUM_FRAMES, nFrames);
}


/**
* Function: SetGUIState
* Purpose:  Enable / Disable controls, according to the state of the camera.
*           Most controls cannot be used in any meaningful way until a camera
*           is initialized.
*/
void CSimpleGui::SetGUIState(eGUIState state)
{
	if (state == GS_NoCamera && m_pRoiDlg != NULL)
		::PostMessage(m_pRoiDlg->GetSafeHwnd(), WM_CLOSE, 0, 0);

	// The CameraSelect and Play/Pause/Stop button states are set in the base
	// class version of this function.
	CDevAppPage::SetGUIState(state);

	CPxLCamera* cam = GetActiveCamera();

    // Bugzilla.2108 -- Don't allow the user to edit features if the app is read-only
    bool enable = (state != GS_NoCamera && !theApp.m_bMonitorAccessOnly);

	ENABLE_SLIDER_SUITE(Exposure,SHUTTER)
	ENABLE_SLIDER_SUITE(Gain,GAIN)
	ENABLE_SLIDER_SUITE(Saturation,SATURATION)
	ENABLE_SLIDER_SUITE(Brightness,BRIGHTNESS)
	ENABLE_SLIDER_SUITE(Gamma,GAMMA)
	ENABLE_SLIDER_SUITE(ColorTemp,WHITE_BAL)
    
	m_AutoExposureMin.EnableWindow (enable && cam->FeatureSupportsAutoLimits(FEATURE_SHUTTER));
  	m_AutoExposureMax.EnableWindow (enable && cam->FeatureSupportsAutoLimits(FEATURE_SHUTTER));
    m_AutoExposureMinButton.EnableWindow (enable && cam->FeatureSupportsAutoLimits(FEATURE_SHUTTER));
    m_AutoExposureMaxButton.EnableWindow (enable && cam->FeatureSupportsAutoLimits(FEATURE_SHUTTER));
    m_ExposureAutoCheck.EnableWindow(enable && cam->FeatureSupportsAuto(FEATURE_SHUTTER));
	m_GainAutoCheck.EnableWindow(enable && cam->FeatureSupportsAuto(FEATURE_GAIN));
	m_GammaOnOffCheck.EnableWindow(enable && cam->FeatureSupportsOnOff(FEATURE_GAMMA));
	m_WhiteShadingAutoButton.EnableWindow(enable && cam->FeatureSupportsOnePush(FEATURE_WHITE_SHADING));
	m_ColorTempOnOffCheck.EnableWindow(enable && cam->FeatureSupportsOnOff(FEATURE_COLOR_TEMP));

	bool enableWS = enable && cam->FeatureSupportsManual(FEATURE_WHITE_SHADING);
	m_RedSlider.put_Enabled(enableWS);
	m_GreenSlider.put_Enabled(enableWS);
	m_BlueSlider.put_Enabled(enableWS);
	m_RedEdit.EnableWindow(enableWS);
	m_GreenEdit.EnableWindow(enableWS);
	m_BlueEdit.EnableWindow(enableWS);

    m_HDRCombo.EnableWindow(enable && cam->FeatureSupportsManual(FEATURE_GAIN_HDR));

	m_WindowSizeCombo.EnableWindow(enable && cam->FeatureSupportsManual(FEATURE_ROI));

	m_ImageFileNameEdit.EnableWindow(enable);
	m_ImageFileBrowseButton.EnableWindow(enable);
	m_ImageFileFormatCombo.EnableWindow(enable);
	m_NumFramesEdit.EnableWindow(enable);
	m_ImageCaptureButton.EnableWindow(enable);

	m_IncrementAfterCaptureCheck.EnableWindow(enable);
	m_PreviewAfterCaptureCheck.EnableWindow(enable);
	m_CaptureFullFrameCheck.EnableWindow(enable);
	m_CaptureFullResCheck.EnableWindow(enable);

    // Bugzilla.1194 -- Don't enable FrameRate Controls if in fixedFramerateMode
    enable = enable & !cam->IsFixedFramerateModeEnabled();
    ENABLE_SLIDER_SUITE(FrameRate,FRAME_RATE)
	m_FrameRateOnOffCheck.EnableWindow(enable && cam->FeatureSupportsOnOff(FEATURE_FRAME_RATE));
	m_ActualFrameRateEdit.EnableWindow(false);  // This is always read only

    ScrollToEnd(m_ImageFileNameEdit);

    // HDR makes some features ineffective.  Signify this by greying out those controls
    if (cam->FeatureSupported(FEATURE_GAIN_HDR))
    {
        U32 hdrMode = (U32)cam->GetFeatureValue(FEATURE_GAIN_HDR);
        if (enable && cam->FeatureSupported(FEATURE_GAIN) && hdrMode != FEATURE_GAIN_HDR_MODE_NONE)
        {
            m_GainSlider.put_Enabled(FALSE);
            m_GainEdit.EnableWindow(FALSE);
            m_GainAutoButton.EnableWindow(FALSE);
    	    m_GainAutoCheck.EnableWindow(FALSE);
        }
        if (enable && 
            cam->FeatureSupported(FEATURE_COLOR_TEMP) && 
            hdrMode == FEATURE_GAIN_HDR_MODE_INTERLEAVED)
        {
            m_ColorTempSlider.put_Enabled(FALSE);
            m_ColorTempEdit.EnableWindow(FALSE);
        }
    }
}


/**
* Function: ModeChanged
* Purpose:  Will be called by the View class after the mode is changed
*           to Advanced mode or to Simple mode.
*/
void CSimpleGui::ModeChanged(bool advanced)
{
	this->ConfigureExposure();
}


/**
* Function: ConfigureControls
* Purpose:  Set up the controls on the page to customize them according to the
*           currently active camera. This involves things like setting the max
*           and min values for sliders and limiting the entries in combo boxes.
*/
void CSimpleGui::ConfigureControls(void)
{
	ConfigureExposure();
	ConfigureGain();
    ConfigureHDR();
	
	SetSliderRange(m_SaturationSlider, FEATURE_SATURATION);
	SetSliderRange(m_BrightnessSlider, FEATURE_BRIGHTNESS);
	SetSliderRange(m_GammaSlider, FEATURE_GAMMA);

	ConfigureWindowSize();

	std::vector<float> const& vals = GetActiveCamera()->GetSupportedColorTempValues();
	if (vals.size() > 0) {
		m_ColorTempSlider.SetMappedMode((int)vals.size(), &vals[0]);	
	} else {
		m_ColorTempSlider.SetMappedMode(0, NULL);
	}

	SetSliderRange(m_FrameRateSlider, FEATURE_FRAME_RATE);
	SetSliderRange(m_ColorTempSlider, FEATURE_COLOR_TEMP);

	SetSliderRange(m_RedSlider, FEATURE_WHITE_SHADING);
	SetSliderRange(m_GreenSlider, FEATURE_WHITE_SHADING);
	SetSliderRange(m_BlueSlider, FEATURE_WHITE_SHADING);
}


/**
* Function: ConfigureExposure
* Purpose:  Initial configuration of the Exposure (FEATURE_SHUTTER) control.
*           Done when a camera is initialized.
*/
void CSimpleGui::ConfigureExposure(void)
{
	try
	{
		if (GetActiveCamera()->FeatureSupported(FEATURE_SHUTTER))
		{
			TempVal<bool> temp(m_bAdjusting[FEATURE_SHUTTER], true);
			if (m_pView->IsAdvancedMode())
			{
				std::pair<float,float> minmax = GetActiveCamera()->GetFeatureLimits(FEATURE_SHUTTER);
				m_ExposureSlider.SetRange(
					minmax.first,
					minmax.second );
			}
			else
			{
				// Simple mode - allow the slider to go to full range.
				m_ExposureSlider.SetRange(
					GetActiveCamera()->GetAbsMinShutter(),
					GetActiveCamera()->GetAbsMaxShutter());
			}
            bool supportsAutolimits = GetActiveCamera()->FeatureSupportsAutoLimits (FEATURE_SHUTTER);
            if (supportsAutolimits)
            {
        	    std::pair<float,float> limits = GetExposureAutoLimits();
    		    SetFloatText(m_AutoExposureMin, limits.first*1000.0f, 2); // convert from seconds to mSec
    		    SetFloatText(m_AutoExposureMax, limits.second*1000.0f, 1); // convert from seconds to mSec
            } else {
                m_AutoExposureMin.SetWindowText ("");
                m_AutoExposureMax.SetWindowText ("");
            }

  			m_AutoExposureMin.EnableWindow (supportsAutolimits);
  			m_AutoExposureMax.EnableWindow (supportsAutolimits);
            m_AutoExposureMinButton.EnableWindow (supportsAutolimits);
            m_AutoExposureMaxButton.EnableWindow (supportsAutolimits);
		}
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: ConfigureGain
* Purpose:  Initial configuration of the Gain (FEATURE_GAIN) control.
*           Done when a camera is initialized.
*/
void CSimpleGui::ConfigureGain(void)
{
	// Bugzilla.1319
    //      THis can be a fairly expensive process, as some cameras can take a while to try a particular gain.  So, we only
    //      try to do this once for a particuar camera.
    if (! m_GainsKnown)
    {
        std::vector<float> const& vals = GetActiveCamera()->GetSupportedGains();
    	
        // As per Bugzilla.1319, we can't test to see which gains are supported if continous is currently on.  In this case, it will
        // look like there is only one gain supported
        if (vals.size() > 0)
        {
		    m_GainSlider.SetMappedMode((int)vals.size(), &vals[0]);
            if (vals.size() > 3) m_GainsKnown = true; // the 3 being min, max, and the current value
        } else {
		    m_GainSlider.SetMappedMode(0, NULL); // Remove mapped mode.
        }

	    SetSliderRange(m_GainSlider, FEATURE_GAIN);
    }
}

/**
* Function: ConfigureHDR
* Purpose:  
*/
void CSimpleGui::ConfigureHDR(void)
{
    int mode;

    m_HDRCombo.ResetContent();
    //
    // The dropdown will always have the 'Disabled' pick
	int idx = m_HDRCombo.AddString(ApiConstAsString(CT_HDR_MODE, FEATURE_GAIN_HDR_MODE_NONE));
	if (idx != CB_ERR)
		m_HDRCombo.SetItemData(idx, FEATURE_GAIN_HDR_MODE_NONE);
    mode = FEATURE_GAIN_HDR_MODE_NONE;

    if (GetActiveCamera()->FeatureSupported(FEATURE_GAIN_HDR))
	{
		try
		{
			//
            // The camera supports HDR.  It MUST therefore support Camera mode, and it MAY support interleved mode
        	idx = m_HDRCombo.AddString(ApiConstAsString(CT_HDR_MODE, FEATURE_GAIN_HDR_MODE_CAMERA));
	        if (idx != CB_ERR)
		        m_HDRCombo.SetItemData(idx, FEATURE_GAIN_HDR_MODE_CAMERA);

            if (GetActiveCamera()->GetFeatureMaxVal(FEATURE_GAIN_HDR) >= FEATURE_GAIN_HDR_MODE_INTERLEAVED)
            {
            	idx = m_HDRCombo.AddString(ApiConstAsString(CT_HDR_MODE, FEATURE_GAIN_HDR_MODE_INTERLEAVED));
	            if (idx != CB_ERR)
		            m_HDRCombo.SetItemData(idx, FEATURE_GAIN_HDR_MODE_INTERLEAVED);
            }


            TempVal<bool> _temp(m_bAdjusting[FEATURE_GAIN_HDR], true);

            //
            // Set the current HDR mode as active in the combo box
			mode = static_cast<int>(GetActiveCamera()->GetFeatureValue(FEATURE_GAIN_HDR));
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}

    SelectByItemData(m_HDRCombo, mode);
}

/**
* Function: ConfigureWindowSize
* Purpose:  Limit the list of standard subwindow sizes to those that are within
*           the capabilities of the camera.
*/
void CSimpleGui::ConfigureWindowSize()
{
	try
	{
		if (!GetActiveCamera()->FeatureSupported(FEATURE_ROI))
			return;

		long maxWidth = static_cast<long>(
			GetActiveCamera()->GetFeatureMaxVal(FEATURE_ROI, FEATURE_ROI_PARAM_WIDTH));
		long maxHeight = static_cast<long>(
			GetActiveCamera()->GetFeatureMaxVal(FEATURE_ROI, FEATURE_ROI_PARAM_HEIGHT));
		long minWidth = static_cast<long>(
			GetActiveCamera()->GetFeatureMinVal(FEATURE_ROI, FEATURE_ROI_PARAM_WIDTH));
		long minHeight = static_cast<long>(
			GetActiveCamera()->GetFeatureMinVal(FEATURE_ROI, FEATURE_ROI_PARAM_HEIGHT));

		int rot = static_cast<int>(GetActiveCamera()->GetFeatureValue(FEATURE_ROTATE));
		if ((rot % 180) != 0)
		{
			std::swap(maxWidth, maxHeight);
			std::swap(minWidth, minHeight);
		}

		TempVal<bool> _temp(m_bAdjusting[FEATURE_ROI], true);

		SetWindowSizeEntries(
			m_WindowSizeCombo,
			static_cast<int>(minWidth),
			static_cast<int>(minHeight),
			static_cast<int>(maxWidth), 
			static_cast<int>(maxHeight));
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: PopulateDefaults
* Purpose:  Put some meaningles default values into the controls, for purely
*           aesthetic reasons.
*/
void CSimpleGui::PopulateDefaults(void)
{
	m_GainsKnown = false;
    
    SetSliderDefault(m_ExposureSlider);
	SetSliderDefault(m_GainSlider);
	SetSliderDefault(m_SaturationSlider);
	SetSliderDefault(m_BrightnessSlider);
	SetSliderDefault(m_GammaSlider);
	SetSliderDefault(m_FrameRateSlider);
	SetSliderDefault(m_ColorTempSlider);
	SetSliderDefault(m_RedSlider);
	SetSliderDefault(m_GreenSlider);
	SetSliderDefault(m_BlueSlider);

	m_ExposureAutoCheck.SetCheck(BST_UNCHECKED);
	m_GainAutoCheck.SetCheck(BST_UNCHECKED);
	m_GammaOnOffCheck.SetCheck(BST_UNCHECKED);
	m_ColorTempOnOffCheck.SetCheck(BST_UNCHECKED);
	m_FrameRateOnOffCheck.SetCheck(BST_UNCHECKED);

    m_FpsWarningStatic.ShowWindow(SW_HIDE);
    m_LinkSpeedWarningStatic.ShowWindow(SW_HIDE);
    
	m_HDRCombo.SetCurSel(-1);

    m_WindowSizeCombo.SetCurSel(-1);

	if (m_ImageFileFormatCombo.GetCurSel() == -1)
		m_ImageFileFormatCombo.SetCurSel(0);

	if (m_NumFramesEdit.GetWindowTextLength() == 0)
		m_NumFramesEdit.SetWindowText(_T("10"));
}


/**
* Function: PopulateControls
* Purpose:  Make the values displayed in the conrols reflect the current 
*           settings of the active camera.  Called immediately after a 
*           camera is initialized.
*/
void CSimpleGui::PopulateControls(void)
{
	CDevAppPage::PopulateControls();

	PopulateExposure();
	PopulateAutoExposure();
	PopulateGain();
	PopulateAutoGain();
    PopulateHDR();

	PopulateSaturation();
	PopulateBrightness();
	PopulateGamma();
	PopulateGammaOnOff();

	PopulateFrameRate();
	PopulateFrameRateOnOff();
	PopulateFpsWarning();
	PopulateLinkSpeedWarning();
	PopulateActualFrameRate();

	PopulateColorTemp();
	PopulateColorTempOnOff();

	PopulateWhiteShading();

	PopulateWindowSize(m_WindowSizeCombo, false);
}


/**
* Function: PopulateAutoExposure
* Purpose:  
*/
void CSimpleGui::PopulateAutoExposure(void)
{
    // Bugzilla.2108 -- Don't allow the user to edit features if the app is read-only
    if (GetActiveCamera()->FeatureSupported(FEATURE_SHUTTER) && !theApp.m_bMonitorAccessOnly)
	{
		try
		{
			TempVal<bool> _temp(m_bAdjusting[FEATURE_SHUTTER], true);

			bool autoOn = GetActiveCamera()->IsFeatureFlagSet(FEATURE_SHUTTER, FEATURE_FLAG_AUTO);
						
    		//Bugzilla.226.  Don't enable the Auto Exposure if kneepoints are enabled
			bool kneepointEnabled = false;
		    if (GetActiveCamera()->FeatureSupported(FEATURE_EXTENDED_SHUTTER))
		    {
    		    std::vector<float> kneePoints = GetActiveCamera()->GetKneePoints();
    		    if (kneePoints.size() > 0) 
    		    {
    		        kneepointEnabled = true;
    		    }    		
		    }
        	// Bugzilla.419 -- don't enable auto exposure if software triggered
        	bool softwareTriggered = false;
            if (GetActiveCamera()->FeatureSupported(FEATURE_TRIGGER))
            {
    	        U32     triggerflags;
	            Trigger trigger = GetActiveCamera()->GetTrigger(&triggerflags);
	            if ((triggerflags & FEATURE_FLAG_OFF) == 0 && trigger.Type == TRIGGER_TYPE_SOFTWARE)
			        softwareTriggered = true;
            }

			m_ExposureAutoCheck.EnableWindow(!kneepointEnabled && !softwareTriggered && GetActiveCamera()->FeatureSupportsAuto(FEATURE_SHUTTER));
			m_ExposureAutoCheck.SetCheck(autoOn);
			m_ExposureSlider.EnableWindow(!autoOn && !m_ExposureSlider.m_bMinEqualsMax);
			m_ExposureEdit.EnableWindow(!autoOn);
			m_ExposureAutoButton.EnableWindow(!autoOn && !kneepointEnabled && !softwareTriggered && GetActiveCamera()->FeatureSupportsOnePush(FEATURE_SHUTTER));
			// Bugzilla.660 - Start the update timer, if necessary.  
			//     Note that this will not start a new timer if one is already running -- nor will it complain if you try to stop a time that is not running
    		if (autoOn)
	    		StartExposureTimer();
		    else
			    StopExposureTimer();

		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


/**
* Function: PopulateAutoGain
* Purpose:  
*/
void CSimpleGui::PopulateAutoGain(void)
{
    // Bugzilla.2108 -- Don't allow the user to edit features if the app is read-only
    if (GetActiveCamera()->FeatureSupported(FEATURE_GAIN) && !theApp.m_bMonitorAccessOnly)
	{
		try
		{
			TempVal<bool> _temp(m_bAdjusting[FEATURE_GAIN], true);

			bool autoOn = GetActiveCamera()->IsFeatureFlagSet(FEATURE_GAIN, FEATURE_FLAG_AUTO);
						
        	// Bugzilla.419 -- don't enable auto gain if software triggered
        	bool softwareTriggered = false;
            if (GetActiveCamera()->FeatureSupported(FEATURE_TRIGGER))
            {
    	        U32     triggerflags;
	            Trigger trigger = GetActiveCamera()->GetTrigger(&triggerflags);
	            if ((triggerflags & FEATURE_FLAG_OFF) == 0 && trigger.Type == TRIGGER_TYPE_SOFTWARE)
			        softwareTriggered = true;
            }

            // If the camera is using HDR mode, then gain is ineffective
            bool hdrEnabled = false;
            if (GetActiveCamera()->FeatureSupported(FEATURE_GAIN_HDR))
            {
                hdrEnabled = GetActiveCamera()->GetFeatureValue(FEATURE_GAIN_HDR) != FEATURE_GAIN_HDR_MODE_NONE;
            }

			m_GainAutoCheck.EnableWindow(!softwareTriggered && !hdrEnabled && GetActiveCamera()->FeatureSupportsAuto(FEATURE_GAIN));
			m_GainAutoCheck.SetCheck(!hdrEnabled && autoOn);
			m_GainSlider.put_Enabled(!hdrEnabled && !autoOn && !m_GainSlider.m_bMinEqualsMax);
			m_GainEdit.EnableWindow(!hdrEnabled && !autoOn);
			m_GainAutoButton.EnableWindow(!hdrEnabled && !autoOn && !softwareTriggered && GetActiveCamera()->FeatureSupportsOnePush(FEATURE_GAIN));
			// Bugzilla.660 - Start the update timer, if necessary.  
			//     Note that this will not start a new timer if one is already running -- nor will it complain if you try to stop a time that is not running
    		if (!hdrEnabled && autoOn)
	    		StartGainTimer();
		    else
			    StopGainTimer();

		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


/**
* Function: PopulateHdr
* Purpose:  
*/
void CSimpleGui::PopulateHDR(void)
{
    int mode = FEATURE_GAIN_HDR_MODE_NONE; // always supported

    if (GetActiveCamera()->FeatureSupported(FEATURE_GAIN_HDR))
	{
		try
		{
            TempVal<bool> _temp(m_bAdjusting[FEATURE_GAIN_HDR], true);

            //
            // Set the current HDR mode as active in the combo box
			mode = static_cast<int>(GetActiveCamera()->GetFeatureValue(FEATURE_GAIN_HDR));
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}

    SelectByItemData(m_HDRCombo, mode);
}

/**
* Function: PopulateGammaOnOff
* Purpose:  
*/
void CSimpleGui::PopulateGammaOnOff(void)
{
	if (GetActiveCamera()->FeatureSupported(FEATURE_GAMMA))
	{
		try
		{
			TempVal<bool> temp(m_bAdjusting[FEATURE_GAMMA], true);

			bool off = GetActiveCamera()->IsFeatureFlagSet(FEATURE_GAMMA, FEATURE_FLAG_OFF);
			m_GammaOnOffCheck.SetCheck(!off);
			m_GammaSlider.EnableWindow(!off);
			m_GammaEdit.EnableWindow(!off);
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


/**
* Function: PopulateFrameRateOnOff
* Purpose:  
*/
void CSimpleGui::PopulateFrameRateOnOff(void)
{
	
	CPxLCamera* cam = GetActiveCamera();
    // Bugzilla.2108 -- Don't allow the user to edit features if the app is read-only
    if (cam->FeatureSupported(FEATURE_FRAME_RATE) && !theApp.m_bMonitorAccessOnly)
	{
		try
		{
			LOCK_FEATURE(FRAME_RATE);

			bool supported = cam->FeatureSupportsOnOff(FEATURE_FRAME_RATE);
			m_FrameRateOnOffCheck.EnableWindow(supported);
			bool off = cam->IsFeatureFlagSet(FEATURE_FRAME_RATE, FEATURE_FLAG_OFF);
			m_FrameRateOnOffCheck.SetCheck(supported && !off);
			m_FrameRateSlider.EnableWindow(supported && !off && !m_FrameRateSlider.m_bMinEqualsMax);
			m_FrameRateEdit.EnableWindow(supported && !off);
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


/**
* Function: PopulateColorTempOnOff
* Purpose:  
*/
void CSimpleGui::PopulateColorTempOnOff(void)
{
    // Bugzilla.2108 -- Don't allow the user to edit features if the app is read-only
    if (GetActiveCamera()->FeatureSupported(FEATURE_COLOR_TEMP) && !theApp.m_bMonitorAccessOnly)
	{
		try
		{
			TempVal<bool> temp(m_bAdjusting[FEATURE_COLOR_TEMP], true);

			bool off = GetActiveCamera()->IsFeatureFlagSet(FEATURE_COLOR_TEMP, FEATURE_FLAG_OFF);
            // If the camera is using HDR interleaved mode, then color temp is ineffective
            bool hdrInterleaved = false;
            if (GetActiveCamera()->FeatureSupported(FEATURE_GAIN_HDR))
            {
                hdrInterleaved = GetActiveCamera()->GetFeatureValue(FEATURE_GAIN_HDR) == FEATURE_GAIN_HDR_MODE_INTERLEAVED;
            }
			m_ColorTempOnOffCheck.EnableWindow(!hdrInterleaved);
			m_ColorTempOnOffCheck.SetCheck(!off);
			m_ColorTempSlider.put_Enabled(!hdrInterleaved && !off);
			m_ColorTempEdit.EnableWindow(!hdrInterleaved && !off);
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


/**
* Function: PopulateWhiteShading
* Purpose:  Display the value of FEATURE_WHITE_SHADING.
*/
void CSimpleGui::PopulateWhiteShading(void)
{
	if (GetActiveCamera()->FeatureSupported(FEATURE_WHITE_SHADING))
	{
		try
		{
			LOCK_FEATURE(WHITE_SHADING);

			if (GetActiveCamera()->IsDynamicFeature(FEATURE_WHITE_SHADING))
			{
				// Reset the range of the sliders - the max and min may have changed
				float min = GetActiveCamera()->GetFeatureMinVal(FEATURE_WHITE_SHADING);
				float max = GetActiveCamera()->GetFeatureMaxVal(FEATURE_WHITE_SHADING);
				m_RedSlider.SetRange(min, max);
				m_GreenSlider.SetRange(min, max);
				m_BlueSlider.SetRange(min, max);
			}

			std::vector<float> vals = GetActiveCamera()->GetWhiteShading();
			m_RedSlider.SetValue(vals[FEATURE_WHITE_SHADING_PARAM_RED]);
			m_GreenSlider.SetValue(vals[FEATURE_WHITE_SHADING_PARAM_GREEN]);
			m_BlueSlider.SetValue(vals[FEATURE_WHITE_SHADING_PARAM_BLUE]);
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


/**
* Function: FeatureChanged
* Purpose:  This method is called by the base class (CDevAppPage) as a result
*           of a call to the View class's OnUpdate method (which is called by
*           the Document class as a result of a change to its data).
*/
void CSimpleGui::FeatureChanged(U32 featureId)
{
	if (m_pRoiDlg != NULL)
	{
		m_pRoiDlg->FeatureChanged(featureId);
		return;
	}

	switch (featureId)
	{
	case FEATURE_SHUTTER:
		PopulateExposure();
		PopulateAutoExposure();
    	PopulateSlider(m_FrameRateSlider, FEATURE_FRAME_RATE);
		//PopulateFrameRate();
		PopulateActualFrameRate();
		PopulateFrameRateOnOff();
		PopulateFpsWarning();
		break;
	case FEATURE_GAIN:
		PopulateGain();
		PopulateAutoGain();
		break;
    case FEATURE_SATURATION:
		PopulateSaturation();
		break;
	case FEATURE_BRIGHTNESS:
		PopulateBrightness();
		break;
	case FEATURE_GAMMA:
		PopulateGamma();
		PopulateGammaOnOff();
		break;
	case FEATURE_ROI:
		PopulateFrameRate();
		PopulateActualFrameRate();
		PopulateFrameRateOnOff();
		PopulateWindowSize(m_WindowSizeCombo, false);
		PopulateFrameRate();
		PopulateFrameRateOnOff();
    	PopulateSlider(m_ExposureSlider, FEATURE_SHUTTER);
		PopulateAutoExposure();
		PopulateAutoGain();
		break;
	case FEATURE_FRAME_RATE:
		PopulateFrameRate();
		PopulateActualFrameRate();
		PopulateFrameRateOnOff();
		ConfigureExposure();
		PopulateExposure();
		PopulateFpsWarning();
		break;
	case FEATURE_COLOR_TEMP:
		PopulateColorTemp();
		PopulateColorTempOnOff();
		break;
	case FEATURE_WHITE_SHADING:
		PopulateWhiteShading();
		break;
	case FEATURE_SPECIAL_CAMERA_MODE:
	    ConfigureWindowSize();
	    PopulateWindowSize(m_WindowSizeCombo);
	    break;
    case FEATURE_GAIN_HDR:
		PopulateFrameRate();
		PopulateActualFrameRate();
        PopulateGain();
        PopulateAutoGain();
        PopulateColorTempOnOff();
        PopulateSaturation();
		ConfigureExposure();  // Bugzilla.1913
	default:
		// A feature that is not relevant to this page. Ignore the change.
		break;
	}
}


/******************************************************************************
 * CSimpleGui message handlers
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

//SLIDER_CONTROL_HANDLER_IMPL(CSimpleGui,Exposure,SHUTTER)
SLIDER_CONTROL_HANDLER_IMPL(CSimpleGui,Gain,GAIN)
//SLIDER_CONTROL_HANDLER_IMPL(CSimpleGui,Saturation,SATURATION)
SLIDER_CONTROL_EVENT_HANDLER_IMPL(CSimpleGui,Saturation,SATURATION)
SLIDER_CONTROL_HANDLER_IMPL(CSimpleGui,Brightness,BRIGHTNESS)
SLIDER_CONTROL_HANDLER_IMPL(CSimpleGui,Gamma,GAMMA)
// 2004-11-20 - Normal handling of OnScroll for FrameRate is not good - the stream
// gets stopped and restarted many times as you drag the slider, which is very slow.
//SLIDER_CONTROL_HANDLER_IMPL(CSimpleGui,FrameRate,FRAME_RATE)
SLIDER_CONTROL_HANDLER_IMPL(CSimpleGui,ColorTemp,WHITE_BAL)



/*
 * Purpose:  Read the Actual Frame Rate (FEATURE_ACTUAL_FRAME_RATE) control.  If this
 *           control is not supported, then FEATURE_FRAME_RATE is used.
 *           Done when a camera is initialized.
 */
void CSimpleGui::PopulateActualFrameRate(void)
{
	try
	{
		float fr;
		U32   fr_flags = 0;
		
		if (GetActiveCamera()->FeatureSupported(FEATURE_ACTUAL_FRAME_RATE))
		{
			fr = GetActiveCamera()->GetFeatureValue(FEATURE_ACTUAL_FRAME_RATE, &fr_flags);
		} else {
			fr = GetActiveCamera()->GetFeatureValue(FEATURE_FRAME_RATE, &fr_flags);
		}
		SetFloatText(m_ActualFrameRateEdit, fr, 3);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}

void CSimpleGui::OnScrollFrameRateSlider() {
	if (STOP_STREAM == GetActiveCamera()->GetStreamState())
		SliderScrolled(m_FrameRateSlider, FEATURE_FRAME_RATE);
}
void CSimpleGui::OnChangeFrameRateSlider() {
	SliderChanged(m_FrameRateSlider, FEATURE_FRAME_RATE);
}
void CSimpleGui::OnKillFocusFrameRateEdit() {
    if (GetActiveCamera()->GetFeatureValue(FEATURE_SPECIAL_CAMERA_MODE) != FEATURE_SPECIAL_CAMERA_MODE_FIXED_FRAME_RATE)
    {
    	SliderEditFocusKilled(m_FrameRateSlider, FEATURE_FRAME_RATE);
    }
}
void CSimpleGui::OnClickFrameRateAutoButton() {
	try { GetActiveCamera()->SetOnePush(FEATURE_FRAME_RATE); }
	catch (PxLException const& e) { OnApiError(e); }
}

void CSimpleGui::PopulateFrameRate (void) {
	PopulateSlider(m_FrameRateSlider, FEATURE_FRAME_RATE);
}

// The following will warn the user if the exposure is limiting the actual frame rate.
void CSimpleGui::PopulateFpsWarning(void)
{
	if (GetActiveCamera()->FeatureSupported(FEATURE_ACTUAL_FRAME_RATE) &&
	    GetActiveCamera()->FeatureSupported(FEATURE_FRAME_RATE) && 
	    GetActiveCamera()->FeatureSupported(FEATURE_EXPOSURE))
	{
		try
		{
			bool warningRequired = false;
      		U32   fr_flags = 0;
      		U32   other_flags = 0;
		    float targetFr = GetActiveCamera()->GetFeatureValue(FEATURE_FRAME_RATE, &fr_flags);
		    float actualFr = GetActiveCamera()->GetFeatureValue(FEATURE_ACTUAL_FRAME_RATE, &other_flags);
			    
			if (! (fr_flags & FEATURE_FLAG_OFF))
			{
		        // Only display the warning if the user is controlling frame rate, 
		        // and it is in fact the exposure that is limiting the frame rate (as opposed to say, 
		        // bandwidth limit).
		        float exposure = GetActiveCamera()->GetFeatureValue(FEATURE_EXPOSURE, &other_flags);
		        exposure = exposure / 1000.0f; // convert from ms to seconds.
		        warningRequired = ((actualFr < targetFr) && (exposure > 1.0f/targetFr));
			}
		    m_FpsWarningStatic.ShowWindow(warningRequired ? SW_SHOW : SW_HIDE);
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}

// The following will warn the user if camera is connected via link operating at a reduced speed (EG, USB3 camera conncted via USB2).
void CSimpleGui::PopulateLinkSpeedWarning(void)
{
    m_LinkSpeedWarningStatic.ShowWindow(GetActiveCamera()->IsLinkSpeedReduced() ? SW_SHOW : SW_HIDE);
}

/******************************************************************************
// The following group of functions are very similar to those that would be
// created by using SLIDER_CONTROL_HANDLER_IMPL(CSimpleGui,Exposure,SHUTTER),
// but they're just a little different. We want the slider to always show the
// full range of shutter values allowed, and when it is moved, we need to
// check whether we need to turn off FrameRate to make the new value legal.
******************************************************************************/

void CSimpleGui::PopulateExposure(void)
{
	if (GetActiveCamera()->FeatureSupported(FEATURE_SHUTTER))
	{
		try
		{
			TempVal<bool> temp(m_bAdjusting[FEATURE_SHUTTER], true);

			float val = GetActiveCamera()->GetFeatureValue(FEATURE_SHUTTER);
			m_ExposureSlider.SetValue(val);		
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


void CSimpleGui::OnScrollExposureSlider()
{
	if (!m_bAdjusting[FEATURE_SHUTTER])
	{
		m_ExposureSlider.OnScroll();
		SetExposure(m_ExposureSlider.GetValue());
	}
}


void CSimpleGui::OnChangeExposureSlider()
{
	if (!m_bAdjusting[FEATURE_SHUTTER])
	{
		m_ExposureSlider.OnChange();
		SetExposure(m_ExposureSlider.GetValue());
		// Bugzilla.614 & Bugzilla.428 -- Re-read the exposure limits, as they may have changed.
		ConfigureExposure();
	}
}


void CSimpleGui::OnKillFocusExposureEdit()
{
	{
		TempVal<bool> temp(m_bAdjusting[FEATURE_SHUTTER], true);
		m_ExposureSlider.UpdateFromWnd();
	}
	SetExposure(m_ExposureSlider.GetValue());
	// Bugzilla.614 & Bugzilla.428 -- Re-read the exposure limits, as they may have changed.
	ConfigureExposure();
}

void CSimpleGui::OnClickExposureAutoMinButton(void)
{
    SetFloatText(m_AutoExposureMin, m_ExposureSlider.GetValue(), 2);
    m_AutoExposeMinStale = true;
}

void CSimpleGui::OnClickExposureAutoMaxButton(void)
{
    SetFloatText(m_AutoExposureMax, m_ExposureSlider.GetValue(), 2);
    m_AutoExposeMaxStale = true;
}

void CSimpleGui::OnClickExposureAutoButton()
{
	try
	{
        std::pair<float,float> limits = GetExposureAutoLimits();
        GetActiveCamera()->SetOnePush(FEATURE_SHUTTER, true, limits);
        m_AutoExposeMinStale = false;
        m_AutoExposeMaxStale = false;
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}

void CSimpleGui::SetExposure(float val)
{
	if (!GetActiveCamera()->FeatureSupportsManual(FEATURE_SHUTTER))
		return;

	// First, determine whether the new value is legal.
	float maxShutter = GetActiveCamera()->GetFeatureMaxVal(FEATURE_SHUTTER);
	if (val > maxShutter)
	{
		GetActiveCamera()->SetOnOff(FEATURE_FRAME_RATE, false);
		// XXX - workaround for [driver|firmware] bug - after turning off framerate,
		//       we still get an OutOfRangeError when trying to set SHUTTER. Doing
		//       a query for the new max value prevents the error.
		GetActiveCamera()->GetFeatureMaxVal(FEATURE_SHUTTER);
	}

	try
	{
		GetActiveCamera()->SetFeatureValue(FEATURE_SHUTTER, val);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}

std::pair<float,float> CSimpleGui::GetExposureAutoLimits()
{
   	std::pair<float,float> limits = GetActiveCamera()->GetFeatureAutoLimits(FEATURE_SHUTTER);
    if (m_AutoExposeMinStale) limits.first = WindowTextToFloat(m_AutoExposureMin) / 1000.0f; // convert mSec to seconds
    if (m_AutoExposeMaxStale) limits.second = WindowTextToFloat(m_AutoExposureMax) / 1000.0f; // convert mSec to seconds

    return limits;
}

/**
* Function: OnClickExposureAutoCheck
* Purpose:  
*/
void CSimpleGui::OnClickExposureAutoCheck(void)
{
	CWaitCursor wc;
	try
	{
		bool autoOn = m_ExposureAutoCheck.GetCheck() == BST_CHECKED;
		if (autoOn)
        {
            std::pair<float,float> limits = GetExposureAutoLimits();
    		GetActiveCamera()->SetAuto(FEATURE_SHUTTER, true, limits);
            m_AutoExposeMinStale = false;
            m_AutoExposeMaxStale = false;
			StartExposureTimer();
        } else {
    		GetActiveCamera()->SetAuto(FEATURE_SHUTTER, false);
			StopExposureTimer();
        }
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}

/**
* Function: OnClickGainAutoCheck
* Purpose:  
*/
void CSimpleGui::OnClickGainAutoCheck(void)
{
	CWaitCursor wc;
	try
	{
		bool autoOn = m_GainAutoCheck.GetCheck() == BST_CHECKED;
		if (autoOn)
        {
    		GetActiveCamera()->SetAuto(FEATURE_GAIN, true);
			StartGainTimer();
        } else {
    		GetActiveCamera()->SetAuto(FEATURE_GAIN, false);
			StopGainTimer();
            // Bugzilla.1319 - We're turing off auto gain -- we may need to figure out the gains.
           	ConfigureGain();
        }
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}

void CSimpleGui::PopulateSaturation(void)
{
    // Bugzilla.2108 -- Don't allow the user to edit features if the app is read-only
    if (GetActiveCamera()->FeatureSupported(FEATURE_SATURATION) && !theApp.m_bMonitorAccessOnly)
	{
		try
		{
			TempVal<bool> temp(m_bAdjusting[FEATURE_SATURATION], true);

			float val = GetActiveCamera()->GetFeatureValue(FEATURE_SATURATION);
			m_SaturationSlider.SetValue(val);		

            // If the camera is using HDR interleaved mode, then saturation is ineffective
            bool hdrInterleaved = false;
            if (GetActiveCamera()->FeatureSupported(FEATURE_GAIN_HDR))
            {
                hdrInterleaved = GetActiveCamera()->GetFeatureValue(FEATURE_GAIN_HDR) == FEATURE_GAIN_HDR_MODE_INTERLEAVED;
            }
			m_SaturationSlider.put_Enabled(!hdrInterleaved);
			m_SaturationEdit.EnableWindow(!hdrInterleaved);
            m_SaturationAutoButton.EnableWindow(!hdrInterleaved && GetActiveCamera()->FeatureSupportsOnePush(FEATURE_SATURATION));
        }
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}

/**
* Function: OnSelchangeHDRCombo
* Purpose:  Event handler for the Change event of the "Standard Sizes" combo
*           box. Sets the PxLSubwindow control to the specified size.
*/
void CSimpleGui::OnSelchangeHDRCombo(void)
{
	if (!m_bAdjusting[FEATURE_GAIN_HDR] && m_HDRCombo.GetCurSel() != -1)
	{
		float mode = ItemData<float>(m_HDRCombo);
		try
		{
			GetActiveCamera()->SetFeatureValue(FEATURE_GAIN_HDR, mode);
            if (theApp.m_bAutoResetPreview)
            {
                GetActiveCamera()->ResetPreviewWindow();
            }
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}




/**
* Function: OnClickGammaOnOffCheck
* Purpose:  
*/
void CSimpleGui::OnClickGammaOnOffCheck(void)
{
	CWaitCursor wc;
	if (!m_bAdjusting[FEATURE_GAMMA])
	{
		try
		{
			GetActiveCamera()->SetOnOff(FEATURE_GAMMA, m_GammaOnOffCheck.GetCheck() == BST_CHECKED);
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


/**
* Function: OnClickFrameRateOnOffCheck
* Purpose:  
*/
void CSimpleGui::OnClickFrameRateOnOffCheck(void)
{
	CWaitCursor wc;
	if (!m_bAdjusting[FEATURE_FRAME_RATE])
	{
		try
		{
			GetActiveCamera()->SetOnOff(FEATURE_FRAME_RATE, m_FrameRateOnOffCheck.GetCheck() == BST_CHECKED);
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


/**
* Function: OnScrollRedSlider
* Purpose:  
*/
void CSimpleGui::OnScrollRedSlider()
{
	SliderScrolled(m_RedSlider, FEATURE_WHITE_SHADING, FEATURE_WHITE_SHADING_PARAM_RED);
}


/**
* Function: OnChangeRedSlider
* Purpose:  
*/
void CSimpleGui::OnChangeRedSlider()
{
	SliderChanged(m_RedSlider, FEATURE_WHITE_SHADING, FEATURE_WHITE_SHADING_PARAM_RED);
}


/**
* Function: OnKillFocusRedEdit
* Purpose:  
*/
void CSimpleGui::OnKillFocusRedEdit()
{
	SliderEditFocusKilled(m_RedSlider, FEATURE_WHITE_SHADING, FEATURE_WHITE_SHADING_PARAM_RED);
}


/**
* Function: OnScrollGreenSlider
* Purpose:  
*/
void CSimpleGui::OnScrollGreenSlider()
{
	SliderScrolled(m_GreenSlider, FEATURE_WHITE_SHADING, FEATURE_WHITE_SHADING_PARAM_GREEN);
}


/**
* Function: OnChangeGreenSlider
* Purpose:  
*/
void CSimpleGui::OnChangeGreenSlider()
{
	SliderChanged(m_GreenSlider, FEATURE_WHITE_SHADING, FEATURE_WHITE_SHADING_PARAM_GREEN);
}


/**
* Function: OnKillFocusGreenEdit
* Purpose:  
*/
void CSimpleGui::OnKillFocusGreenEdit()
{
	SliderEditFocusKilled(m_GreenSlider, FEATURE_WHITE_SHADING, FEATURE_WHITE_SHADING_PARAM_GREEN);
}


/**
* Function: OnScrollBlueSlider
* Purpose:  
*/
void CSimpleGui::OnScrollBlueSlider()
{
	SliderScrolled(m_BlueSlider, FEATURE_WHITE_SHADING, FEATURE_WHITE_SHADING_PARAM_BLUE);
}


/**
* Function: OnChangeBlueSlider
* Purpose:  
*/
void CSimpleGui::OnChangeBlueSlider()
{
	SliderChanged(m_BlueSlider, FEATURE_WHITE_SHADING, FEATURE_WHITE_SHADING_PARAM_BLUE);
}


/**
* Function: OnKillFocusBlueEdit
* Purpose:  
*/
void CSimpleGui::OnKillFocusBlueEdit()
{
	SliderEditFocusKilled(m_BlueSlider, FEATURE_WHITE_SHADING, FEATURE_WHITE_SHADING_PARAM_BLUE);
}


/**
* Function: OnClickWhiteShadingAutoButton
* Purpose:  
*/
void CSimpleGui::OnClickWhiteShadingAutoButton()
{
	try
	{
		GetActiveCamera()->SetOnePush(FEATURE_WHITE_SHADING);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnClickColorTempOnOffCheck
* Purpose:  
*/
void CSimpleGui::OnClickColorTempOnOffCheck(void)
{
	CWaitCursor wc;
	if (!m_bAdjusting[FEATURE_COLOR_TEMP])
	{
		try
		{
			GetActiveCamera()->SetOnOff(FEATURE_COLOR_TEMP, m_ColorTempOnOffCheck.GetCheck() == BST_CHECKED);
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


/**
* Function: OnSelchangeWindowsizeCombo
* Purpose:  Event handler for the Change event of the "Standard Sizes" combo
*           box. Sets the PxLSubwindow control to the specified size.
*/
void CSimpleGui::OnSelchangeWindowsizeCombo()
{
	WindowsizeComboChanged(m_WindowSizeCombo);
}


/**
* Function: OnKillFocusImageFileNameEdit
* Purpose:  Auto-select the entry in the Format combo box that corresponds to
*           the file extension entered, if any.
*/
void CSimpleGui::OnKillFocusImageFileNameEdit()
{
	SetCombinedFormatFromFilename(m_ImageFileNameEdit, m_ImageFileFormatCombo);
	ScrollToEnd(m_ImageFileNameEdit);
}


/**
* Function: OnClickImageFileBrowseButton
* Purpose:  
*/
void CSimpleGui::OnClickImageFileBrowseButton(void)
{
	BrowseForFile(m_ImageFileNameEdit,
				  combinedFormatFilterString,
				  GetCombinedFormatExtension(m_ImageFileFormatCombo.GetCurSel()));

	ScrollToEnd(m_ImageFileNameEdit);

	SetCombinedFormatFromFilename(m_ImageFileNameEdit, m_ImageFileFormatCombo);
}


/**
* Function: OnSelchangeImageFileFormatCombo
* Purpose:  Make the File Extension in the edit box agree with the format
*           selected here.
*/
void CSimpleGui::OnSelchangeImageFileFormatCombo()
{
	SetFileExtensionFromCombinedFormat(m_ImageFileNameEdit, m_ImageFileFormatCombo);
	ScrollToEnd(m_ImageFileNameEdit);
}


void CSimpleGui::OnUpdateNumFramesEdit(CCmdUI* pCmdUI)
{
	int fmt = ItemData<int>(m_ImageFileFormatCombo);
	pCmdUI->Enable(fmt & 0x80); // We add an offset of 0x80 to the Video formats (well, right now there's only one...)
}


/**
* Function: OnClickImageCaptureButton
* Purpose:  
*/
void CSimpleGui::OnClickImageCaptureButton(void)
{
	CWaitCursor wc;
	try
	{
		// Temporarily set the ROI to full frame, if the box is checked.
		std::auto_ptr<CStreamState> ss(NULL);
		std::auto_ptr<CPreviewState> ps(NULL);
		std::auto_ptr<CTempFullFrame> ff(NULL);
		std::auto_ptr<CTempFullResolution> fr(NULL);
		bool discardFrame = false;
		if (m_CaptureFullFrameCheck.GetCheck() == BST_CHECKED)
		{
			//ff.reset(new CTempFullFrame(GetActiveCamera())); // vc7
			ff = std::auto_ptr<CTempFullFrame>(new CTempFullFrame(GetActiveCamera())); // vc6
            // Bugzilla.778 - Pause the preview if we had to change the camera to a full frame state
			if (ff->ChangedValue())
            {
    			if (GetActiveCamera()->GetPreviewState() == START_PREVIEW)
	    			ps = std::auto_ptr<CPreviewState>(new CPreviewState(GetActiveCamera(), PAUSE_PREVIEW));
				discardFrame = true;
            }
		}
		if (m_CaptureFullResCheck.GetCheck() == BST_CHECKED)
		{
			fr = std::auto_ptr<CTempFullResolution>(new CTempFullResolution(GetActiveCamera()));
			if (fr->ChangedValue())
				discardFrame = true;
		}

		int fmt = ItemData<int>(m_ImageFileFormatCombo);
		bool success = false;
		if ((fmt & 0x80) != 0)
		{
			// Video capture
            if (fmt == (0x80 + CLIP_FORMAT_MP4))
            {
			    success = GetDocument()->CaptureEncodedClip(m_NumFramesEdit, 
                                                            1 ,  // No decimation
						  				                    &m_ImageFileNameEdit, 
										                    &m_ImageFileFormatCombo,
                                                            GetActiveCamera()->GetFeatureValue(FEATURE_FRAME_RATE),
                                                            1000000,  // Pick a 'reasonable' bit rate
                                                            CLIP_ENCODING_H264, // Use H.264, as its more commonly available
										                    NULL);
            } else {
			    success = GetDocument()->CaptureClip(m_NumFramesEdit, &m_ImageFileNameEdit, &m_ImageFileFormatCombo);
            }
		}
		else
		{
			// Image Capture
			success = GetDocument()->CaptureImage(m_ImageFileNameEdit, m_ImageFileFormatCombo, discardFrame ? 1 : 0);
		}

		if (success)
		{
			if (m_PreviewAfterCaptureCheck.GetCheck() == BST_CHECKED) {
				CString fileName(WindowTextAsString(m_ImageFileNameEdit));
				SysOpenFile(fileName);
			}
			if (m_IncrementAfterCaptureCheck.GetCheck() == BST_CHECKED)
				IncrementFileName(m_ImageFileNameEdit, "%4.4d");
		}

	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnClickRoiPopupButton
* Purpose:  
*/
void CSimpleGui::OnClickRoiPopupButton()
{
	CWaitCursor wc;

	m_pRoiDlg = new CRoiDialog2(this);
	m_pRoiDlg->DoModal();
	delete m_pRoiDlg;
	m_pRoiDlg = NULL;

	ConfigureWindowSize();
	FeatureChanged(FEATURE_ROI);
}
















