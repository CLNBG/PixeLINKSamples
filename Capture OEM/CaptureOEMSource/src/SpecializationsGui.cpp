// KneePointGui.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "SpecializationsGui.h"
#include "Helpers.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define MIN_INCREMENT	0.01f	// .01 milliseconds


/////////////////////////////////////////////////////////////////////////////
// CKneePointGui property page

IMPLEMENT_DYNCREATE(CKneePointGui, CDevAppPage)

CKneePointGui::CKneePointGui(CPxLDevAppView* pView)
	: CDevAppPage(CKneePointGui::IDD, pView)
	, m_MaxKnees(0)
	, m_timerId(0)
{
}


CKneePointGui::~CKneePointGui()
{
}


void CKneePointGui::DoDataExchange(CDataExchange* pDX)
{
	CDevAppPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_KG_CAMERASELECT_COMBO, m_CameraSelectCombo);
	DDX_Control(pDX, IDC_KG_PLAY_BUTTON, m_PlayButton);
	DDX_Control(pDX, IDC_KG_PAUSE_BUTTON, m_PauseButton);
	DDX_Control(pDX, IDC_KG_STOP_BUTTON, m_StopButton);

	DDX_Control(pDX, IDC_KG_EXPOSURE_SLIDER, m_ExposureSlider);
	DDX_Control(pDX, IDC_KG_EXPOSURE_EDIT, m_ExposureEdit);
	DDX_Control(pDX, IDC_KG_EXPOSUREAUTO_BUTTON, m_ExposureAutoButton);
	DDX_Control(pDX, IDC_KG_EXPOSUREMAX_LABEL, m_ExposureMaxLabel);

	DDX_Control(pDX, IDC_KG_KNEEPOINT1_CHECK, m_KPCheck1);
	DDX_Control(pDX, IDC_KG_KNEEPOINT1_SLIDER, m_KPSlider1);
	DDX_Control(pDX, IDC_KG_KNEEPOINT1_EDIT, m_KPEdit1);
	DDX_Control(pDX, IDC_KG_KNEEPOINTMIN1_LABEL, m_KPMinLabel1);
	DDX_Control(pDX, IDC_KG_KNEEPOINTMAX1_LABEL, m_KPMaxLabel1);

	DDX_Control(pDX, IDC_KG_KNEEPOINT2_CHECK, m_KPCheck2);
	DDX_Control(pDX, IDC_KG_KNEEPOINT2_SLIDER, m_KPSlider2);
	DDX_Control(pDX, IDC_KG_KNEEPOINT2_EDIT, m_KPEdit2);
	DDX_Control(pDX, IDC_KG_KNEEPOINTMIN2_LABEL, m_KPMinLabel2);
	DDX_Control(pDX, IDC_KG_KNEEPOINTMAX2_LABEL, m_KPMaxLabel2);

	DDX_Control(pDX, IDC_KG_KNEEPOINT3_CHECK, m_KPCheck3);
	DDX_Control(pDX, IDC_KG_KNEEPOINT3_SLIDER, m_KPSlider3);
	DDX_Control(pDX, IDC_KG_KNEEPOINT3_EDIT, m_KPEdit3);
	DDX_Control(pDX, IDC_KG_KNEEPOINTMIN3_LABEL, m_KPMinLabel3);
	DDX_Control(pDX, IDC_KG_KNEEPOINTMAX3_LABEL, m_KPMaxLabel3);

	DDX_Control(pDX, IDC_KG_KNEEPOINT4_CHECK, m_KPCheck4);
	DDX_Control(pDX, IDC_KG_KNEEPOINT4_SLIDER, m_KPSlider4);
	DDX_Control(pDX, IDC_KG_KNEEPOINT4_EDIT, m_KPEdit4);
	DDX_Control(pDX, IDC_KG_KNEEPOINTMIN4_LABEL, m_KPMinLabel4);
	DDX_Control(pDX, IDC_KG_KNEEPOINTMAX4_LABEL, m_KPMaxLabel4);

	DDX_Control(pDX, IDC_KG_NOTE, m_KPNote);

	DDX_Control(pDX, IDC_KG_COLORBAR, m_ColorBar);

    DDX_Control(pDX, IDC_KG_PIXELFORMAT_COMBO, m_PixelFormatCombo);
    DDX_Control(pDX, IDC_KG_PIXELFORMATINTERPRETATION_COMBO, m_PixelFormatInterpretationCombo);

	DDX_Control(pDX, IDC_KG_MINALLPOLARS_BUTTON, m_KPMinAllPolarsButton);
	DDX_Control(pDX, IDC_KG_MAXALLPOLARS_BUTTON, m_KPMaxAllPolarsButton);

    DDX_Control(pDX, IDC_KG_POLAR0_SLIDER, m_KPPolar0Slider);
	DDX_Control(pDX, IDC_KG_POLAR0_EDIT, m_KPPolar0Edit);
	DDX_Control(pDX, IDC_KG_POLAR0MIN_LABEL, m_KPPolar0MinLabel);
	DDX_Control(pDX, IDC_KG_POLAR0MAX_LABEL, m_KPPolar0MaxLabel);

	DDX_Control(pDX, IDC_KG_POLAR45_SLIDER, m_KPPolar45Slider);
	DDX_Control(pDX, IDC_KG_POLAR45_EDIT, m_KPPolar45Edit);
	DDX_Control(pDX, IDC_KG_POLAR45MIN_LABEL, m_KPPolar45MinLabel);
	DDX_Control(pDX, IDC_KG_POLAR45MAX_LABEL, m_KPPolar45MaxLabel);

	DDX_Control(pDX, IDC_KG_POLAR90_SLIDER, m_KPPolar90Slider);
	DDX_Control(pDX, IDC_KG_POLAR90_EDIT, m_KPPolar90Edit);
	DDX_Control(pDX, IDC_KG_POLAR90MIN_LABEL, m_KPPolar90MinLabel);
	DDX_Control(pDX, IDC_KG_POLAR90MAX_LABEL, m_KPPolar90MaxLabel);

	DDX_Control(pDX, IDC_KG_POLAR135_SLIDER, m_KPPolar135Slider);
	DDX_Control(pDX, IDC_KG_POLAR135_EDIT, m_KPPolar135Edit);
	DDX_Control(pDX, IDC_KG_POLAR135MIN_LABEL, m_KPPolar135MinLabel);
	DDX_Control(pDX, IDC_KG_POLAR135MAX_LABEL, m_KPPolar135MaxLabel);

	DDX_Control(pDX, IDC_KG_PTPONOFF_CHECK, m_PtpOnOffCheck);
	DDX_Control(pDX, IDC_KG_PTPSLAVEONLY_CHECK, m_PtpSlaveOnlyCheck);
	DDX_Control(pDX, IDC_KG_PTPSTATE_EDIT, m_PtpStateEdit);
	DDX_Control(pDX, IDC_KG_PTPDAYS_EDIT, m_PtpDaysEdit);
	DDX_Control(pDX, IDC_KG_PTPHOURS_EDIT, m_PtpHoursEdit);
	DDX_Control(pDX, IDC_KG_PTPMINS_EDIT, m_PtpMinsEdit);
	DDX_Control(pDX, IDC_KG_PTPSECS_EDIT, m_PtpSecsEdit);

}


BEGIN_MESSAGE_MAP(CKneePointGui, CDevAppPage)

	ON_CBN_DROPDOWN(IDC_KG_CAMERASELECT_COMBO, OnDropdownCameraSelectCombo)
	ON_CBN_SELCHANGE(IDC_KG_CAMERASELECT_COMBO, OnSelchangeCameraSelectCombo)
	ON_BN_CLICKED(IDC_KG_PLAY_BUTTON, OnClickPlayButton)
	ON_BN_CLICKED(IDC_KG_PAUSE_BUTTON, OnClickPauseButton)
	ON_BN_CLICKED(IDC_KG_STOP_BUTTON, OnClickStopButton)

	ON_EN_KILLFOCUS(IDC_KG_EXPOSURE_EDIT, OnKillFocusExposureEdit)
	ON_BN_CLICKED(IDC_KG_EXPOSUREAUTO_BUTTON, OnClickExposureAutoButton)

	ON_BN_CLICKED(IDC_KG_KNEEPOINT1_CHECK, OnClickKneePointCheck1)
	ON_EN_KILLFOCUS(IDC_KG_KNEEPOINT1_EDIT, OnKillFocusKneePointEdit1)

	ON_BN_CLICKED(IDC_KG_KNEEPOINT2_CHECK, OnClickKneePointCheck2)
	ON_EN_KILLFOCUS(IDC_KG_KNEEPOINT2_EDIT, OnKillFocusKneePointEdit2)

	ON_BN_CLICKED(IDC_KG_KNEEPOINT3_CHECK, OnClickKneePointCheck3)
	ON_EN_KILLFOCUS(IDC_KG_KNEEPOINT3_EDIT, OnKillFocusKneePointEdit3)

	ON_BN_CLICKED(IDC_KG_KNEEPOINT4_CHECK, OnClickKneePointCheck4)
	ON_EN_KILLFOCUS(IDC_KG_KNEEPOINT4_EDIT, OnKillFocusKneePointEdit4)

	ON_UPDATE_COMMAND_UI(IDC_KG_KNEEPOINT1_EDIT, OnUpdateKneePoint1)

	ON_UPDATE_COMMAND_UI(IDC_KG_KNEEPOINT2_CHECK, OnUpdateKneePointCheck2)
	ON_UPDATE_COMMAND_UI(IDC_KG_KNEEPOINT2_EDIT, OnUpdateKneePoint2)

	ON_UPDATE_COMMAND_UI(IDC_KG_KNEEPOINT3_CHECK, OnUpdateKneePointCheck3)
	ON_UPDATE_COMMAND_UI(IDC_KG_KNEEPOINT3_EDIT, OnUpdateKneePoint3)

	ON_UPDATE_COMMAND_UI(IDC_KG_KNEEPOINT4_CHECK, OnUpdateKneePointCheck4)
	ON_UPDATE_COMMAND_UI(IDC_KG_KNEEPOINT4_EDIT, OnUpdateKneePoint4)

    ON_CBN_SELCHANGE(IDC_KG_PIXELFORMAT_COMBO, OnSelchangePixelFormatCombo)
    ON_CBN_SELCHANGE(IDC_KG_PIXELFORMATINTERPRETATION_COMBO, OnSelchangePixelFormatInterpretationCombo)

   	ON_BN_CLICKED(IDC_KG_MINALLPOLARS_BUTTON, OnClickMinAllPolarsButton)
    ON_BN_CLICKED(IDC_KG_MAXALLPOLARS_BUTTON, OnClickMaxAllPolarsButton)

   	ON_EN_KILLFOCUS(IDC_KG_POLAR0_EDIT, OnKillFocusPolar0Edit)
   	ON_EN_KILLFOCUS(IDC_KG_POLAR45_EDIT, OnKillFocusPolar45Edit)
   	ON_EN_KILLFOCUS(IDC_KG_POLAR90_EDIT, OnKillFocusPolar90Edit)
   	ON_EN_KILLFOCUS(IDC_KG_POLAR135_EDIT, OnKillFocusPolar135Edit)

    ON_BN_CLICKED(IDC_KG_PTPONOFF_CHECK, OnClickPtpOnOffCheck)
    ON_BN_CLICKED(IDC_KG_PTPSLAVEONLY_CHECK, OnClickPtpSlaveOnlyCheck)

	ON_WM_TIMER()

    ON_WM_PAINT()

END_MESSAGE_MAP()


BEGIN_EVENTSINK_MAP(CKneePointGui, CDevAppPage)

	ON_EVENT(CKneePointGui, IDC_KG_EXPOSURE_SLIDER, 1, OnScrollExposureSlider, VTS_NONE)
	ON_EVENT(CKneePointGui, IDC_KG_EXPOSURE_SLIDER, 2, OnChangeExposureSlider, VTS_NONE)

	ON_EVENT(CKneePointGui, IDC_KG_KNEEPOINT1_SLIDER, 1, OnScrollKneePointSlider1, VTS_NONE)
	ON_EVENT(CKneePointGui, IDC_KG_KNEEPOINT1_SLIDER, 2, OnChangeKneePointSlider1, VTS_NONE)

	ON_EVENT(CKneePointGui, IDC_KG_KNEEPOINT2_SLIDER, 1, OnScrollKneePointSlider2, VTS_NONE)
	ON_EVENT(CKneePointGui, IDC_KG_KNEEPOINT2_SLIDER, 2, OnChangeKneePointSlider2, VTS_NONE)

	ON_EVENT(CKneePointGui, IDC_KG_KNEEPOINT3_SLIDER, 1, OnScrollKneePointSlider3, VTS_NONE)
	ON_EVENT(CKneePointGui, IDC_KG_KNEEPOINT3_SLIDER, 2, OnChangeKneePointSlider3, VTS_NONE)

	ON_EVENT(CKneePointGui, IDC_KG_KNEEPOINT4_SLIDER, 1, OnScrollKneePointSlider4, VTS_NONE)
	ON_EVENT(CKneePointGui, IDC_KG_KNEEPOINT4_SLIDER, 2, OnChangeKneePointSlider4, VTS_NONE)

	ON_EVENT(CKneePointGui, IDC_KG_POLAR0_SLIDER, 1, OnScrollPolar0Slider, VTS_NONE)
	ON_EVENT(CKneePointGui, IDC_KG_POLAR0_SLIDER, 2, OnChangePolar0Slider, VTS_NONE)

	ON_EVENT(CKneePointGui, IDC_KG_POLAR45_SLIDER, 1, OnScrollPolar45Slider, VTS_NONE)
	ON_EVENT(CKneePointGui, IDC_KG_POLAR45_SLIDER, 2, OnChangePolar45Slider, VTS_NONE)

	ON_EVENT(CKneePointGui, IDC_KG_POLAR90_SLIDER, 1, OnScrollPolar90Slider, VTS_NONE)
	ON_EVENT(CKneePointGui, IDC_KG_POLAR90_SLIDER, 2, OnChangePolar90Slider, VTS_NONE)

	ON_EVENT(CKneePointGui, IDC_KG_POLAR135_SLIDER, 1, OnScrollPolar135Slider, VTS_NONE)
	ON_EVENT(CKneePointGui, IDC_KG_POLAR135_SLIDER, 2, OnChangePolar135Slider, VTS_NONE)

END_EVENTSINK_MAP()


/////////////////////////////////////////////////////////////////////////////
// CKneePointGui message handlers

int CKneePointGui::GetHelpContextId()
{
	return IDH_SPECIALIZATIONS_TAB;
}


BOOL CKneePointGui::OnInitDialog()
{
	CDevAppPage::OnInitDialog();

	m_ExposureSlider.SetLinkedWindows(&m_ExposureEdit, NULL, &m_ExposureMaxLabel, NULL);

    m_KPSlider1.SetLinkedWindows(&m_KPEdit1, &m_KPMinLabel1, &m_KPMaxLabel1, NULL);
	m_KPSlider2.SetLinkedWindows(&m_KPEdit2, &m_KPMinLabel2, &m_KPMaxLabel2, NULL);
	m_KPSlider3.SetLinkedWindows(&m_KPEdit3, &m_KPMinLabel3, &m_KPMaxLabel3, NULL);
	m_KPSlider4.SetLinkedWindows(&m_KPEdit4, &m_KPMinLabel4, &m_KPMaxLabel4, NULL);

    m_KPPolar0Slider.SetLinkedWindows(&m_KPPolar0Edit, &m_KPPolar0MinLabel, &m_KPPolar0MaxLabel, NULL);
    m_KPPolar45Slider.SetLinkedWindows(&m_KPPolar45Edit, &m_KPPolar45MinLabel, &m_KPPolar45MaxLabel, NULL);
    m_KPPolar90Slider.SetLinkedWindows(&m_KPPolar90Edit, &m_KPPolar90MinLabel, &m_KPPolar90MaxLabel, NULL);
    m_KPPolar135Slider.SetLinkedWindows(&m_KPPolar135Edit, &m_KPPolar135MinLabel, &m_KPPolar135MaxLabel, NULL);

    return TRUE;
}


/**
* Function: OnSetActive
* Purpose:  
*/
BOOL CKneePointGui::OnSetActive()
{
	if (CDevAppPage::OnSetActive())
	{
        // We always have this timer running while this screen is displayed.
        StartUpdateTimer();

		return TRUE;
	}
	return FALSE;
}


/**
* Function: OnKillActive
* Purpose:  
*/
BOOL CKneePointGui::OnKillActive()
{
	StopUpdateTimer();

	return CDevAppPage::OnKillActive();
}

/**
* Function: SetGUIState
* Purpose:  Enable / Disable controls, according to the state of the camera.
*           Most controls cannot be used in any meaningful way until a camera
*           is initialized.
*/
void CKneePointGui::SetGUIState(eGUIState state)
{
	// The CameraSelect and Play/Pause/Stop button states are set in the base
	// class version of this function.
	CDevAppPage::SetGUIState(state);

	CPxLCamera* cam = GetActiveCamera();

	bool enable = (state != GS_NoCamera);

	bool enableExp = enable && cam->FeatureSupportsManual(FEATURE_SHUTTER);
	// Bugzilla.226 -- don't enable auto exposure if kneepoints are enabled.
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

	m_ExposureSlider.put_Enabled(enableExp);
	m_ExposureEdit.EnableWindow(enableExp);
	m_ExposureAutoButton.EnableWindow(enable && !kneepointEnabled && !softwareTriggered && cam->FeatureSupportsOnePush(FEATURE_SHUTTER));

	bool enableKnees = enable && cam->FeatureSupportsManual(FEATURE_EXTENDED_SHUTTER);
	if (!enableKnees) {
		m_KPCheck1.SetCheck(BST_UNCHECKED);
	}
	m_KPNote.ShowWindow(enableKnees ? SW_HIDE : SW_SHOW);
	m_ColorBar.ShowWindow(enableKnees ? SW_SHOW : SW_HIDE);
	m_KPCheck1.EnableWindow(enableKnees);

	if (cam->FeatureSupportsManual(FEATURE_EXTENDED_SHUTTER))
	{
		m_KPNote.SetWindowText("");
	}
	else
	{
		m_KPNote.SetWindowText(_T("This camera does not support Extended Shutter."));
	}

    bool enablePolar = enable && cam->FeatureSupported(FEATURE_POLAR_WEIGHTINGS);
    m_PixelFormatCombo.EnableWindow(enablePolar);
    // Only enable the pixel format interpretations disalog if the camera is using the HSV pixel format
    bool enableInterpretation = false;
    if (enablePolar)
    {
        enableInterpretation = PIXEL_FORMAT_HSV4_12 == cam->GetFeatureValue(FEATURE_PIXEL_FORMAT);
    }
	m_PixelFormatInterpretationCombo.EnableWindow(enableInterpretation);
    
    m_KPMinAllPolarsButton.EnableWindow(enablePolar);
	m_KPMaxAllPolarsButton.EnableWindow(enablePolar);

    m_KPPolar0Slider.put_Enabled(enablePolar);
    m_KPPolar45Slider.put_Enabled(enablePolar);
    m_KPPolar90Slider.put_Enabled(enablePolar);
    m_KPPolar135Slider.put_Enabled(enablePolar);

    m_KPPolar0Edit.EnableWindow(enablePolar);
    m_KPPolar45Edit.EnableWindow(enablePolar);
    m_KPPolar90Edit.EnableWindow(enablePolar);
    m_KPPolar135Edit.EnableWindow(enablePolar);

    m_KPPolar0MinLabel.EnableWindow(enablePolar);
    m_KPPolar45MinLabel.EnableWindow(enablePolar);
    m_KPPolar90MinLabel.EnableWindow(enablePolar);
    m_KPPolar135MinLabel.EnableWindow(enablePolar);

    m_KPPolar0MaxLabel.EnableWindow(enablePolar);
    m_KPPolar45MaxLabel.EnableWindow(enablePolar);
    m_KPPolar90MaxLabel.EnableWindow(enablePolar);
    m_KPPolar135MaxLabel.EnableWindow(enablePolar);

	m_PtpOnOffCheck.EnableWindow(enable && cam->FeatureSupportsOnOff(FEATURE_PTP));
	m_PtpSlaveOnlyCheck.EnableWindow(enable && cam->FeatureSupportsOnOff(FEATURE_PTP));
    // The other PTP controls are always read-only, no need to reassert that

    // The rest of the controls will be enabled or disabled by
	// UPDATE_COMMAND_UI handlers.

}


/**
* Function: ConfigureControls
* Purpose:  Set up the controls on the page to customize them according to the
*           currently active camera. This involves things like setting the max
*           and min values for sliders and setting the bounds of the subwindow
*           control.
*/
void CKneePointGui::ConfigureControls(void)
{
	SetSliderRange(m_ExposureSlider, FEATURE_SHUTTER);

    SetPixelFormatEntries();
	SetPixelFormatInterpretationEntries();

	SetSliderRange(m_KPPolar0Slider, FEATURE_POLAR_WEIGHTINGS);
	SetSliderRange(m_KPPolar45Slider, FEATURE_POLAR_WEIGHTINGS);
	SetSliderRange(m_KPPolar90Slider, FEATURE_POLAR_WEIGHTINGS);
	SetSliderRange(m_KPPolar135Slider, FEATURE_POLAR_WEIGHTINGS);

}


/**
* Function: PopulateDefaults
* Purpose:  Put some meaningles default values into the controls, for purely
*           aesthetic reasons.
*/
void CKneePointGui::PopulateDefaults(void)
{
	m_KPCheck1.SetCheck(BST_UNCHECKED);

	SetSliderDefault(m_ExposureSlider);
	
    SetSliderDefault(m_KPSlider1);
	SetSliderDefault(m_KPSlider2);
	SetSliderDefault(m_KPSlider3);
	SetSliderDefault(m_KPSlider4);

	m_PixelFormatCombo.SetCurSel(-1);
	m_PixelFormatInterpretationCombo.SetCurSel(-1);

    SetSliderDefault(m_KPPolar0Slider);
    SetSliderDefault(m_KPPolar45Slider);
    SetSliderDefault(m_KPPolar90Slider);
    SetSliderDefault(m_KPPolar135Slider);
	
	m_PtpOnOffCheck.SetCheck(BST_UNCHECKED);
	m_PtpSlaveOnlyCheck.SetCheck(BST_UNCHECKED);
    m_PtpStateEdit.SetWindowText(_T(" "));
    m_PtpDaysEdit.SetWindowText(_T(" "));
    m_PtpHoursEdit.SetWindowText(_T(" "));
    m_PtpMinsEdit.SetWindowText(_T(" "));
    m_PtpSecsEdit.SetWindowText(_T(" "));
}


/**
* Function: PopulateControls
* Purpose:  Make the values displayed in the controls reflect the current 
*           settings of the active camera.  Called immediately after a 
*           camera is initialized.
*/
void CKneePointGui::PopulateControls(void)
{
	CDevAppPage::PopulateControls();

	m_MaxKnees = (GetActiveCamera()->FeatureSupported(FEATURE_EXTENDED_SHUTTER)) 
				? static_cast<int>(GetActiveCamera()->GetFeatureMaxVal(FEATURE_EXTENDED_SHUTTER, FEATURE_EXTENDED_SHUTTER_PARAM_NUM_KNEES))
				: 0;

	PopulateExposure();
	PopulateKneePoints();
    PopulatePixelFormat();
    PopulatePolars();
	
    PopulatePtp ();
}


/**
* Function: PopulateExposure
* Purpose:  
*/
void CKneePointGui::PopulateExposure(void)
{
	try
	{
		PopulateSlider(m_ExposureSlider, FEATURE_SHUTTER);
		//Bugzilla.226.  Don't enable the auto Exposure if kneepoints are enabled
		bool enableAutoExposure = GetActiveCamera()->FeatureSupportsOnePush(FEATURE_SHUTTER);
		if (enableAutoExposure && GetActiveCamera()->FeatureSupported(FEATURE_EXTENDED_SHUTTER))
		{
    		std::vector<float> kneePoints = GetActiveCamera()->GetKneePoints();
    		if (kneePoints.size() > 0) 
    		{
    		    enableAutoExposure = false;
    		}
		}
    	// Bugzilla.419 -- don't enable auto exposure if software triggered
        if (enableAutoExposure && GetActiveCamera()->FeatureSupported(FEATURE_TRIGGER))
        {
    	    U32     triggerflags;
	        Trigger trigger = GetActiveCamera()->GetTrigger(&triggerflags);
	        if ((triggerflags & FEATURE_FLAG_OFF) == 0 && trigger.Type == TRIGGER_TYPE_SOFTWARE)
			    enableAutoExposure = false;
        }
		m_ExposureAutoButton.EnableWindow(enableAutoExposure);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: PopulateKneePoints
* Purpose:  
*/
void CKneePointGui::PopulateKneePoints(void)
{
	if (!(GetActiveCamera()->FeatureSupported(FEATURE_EXTENDED_SHUTTER))) {
		return;
	}
	try
	{
		std::vector<float> kneePoints = GetActiveCamera()->GetKneePoints();

		TempVal<bool> temp(m_bAdjusting[FEATURE_EXTENDED_SHUTTER], true);

		// Bugzilla.353 - Assume none of the kneepoints are enabled, this routine will enable all the ones required.
		m_KPCheck1.SetCheck(BST_UNCHECKED); m_KPCheck1.EnableWindow(false);
		m_KPCheck2.SetCheck(BST_UNCHECKED); m_KPCheck2.EnableWindow(false);
		m_KPCheck3.SetCheck(BST_UNCHECKED); m_KPCheck3.EnableWindow(false);
		m_KPCheck4.SetCheck(BST_UNCHECKED); m_KPCheck4.EnableWindow(false);
		
		// Bugzilla.178 - Limit the kneepoints to the feature limits.  However, not all cameras will set them
		// properly, so for those cameras -- use the shutter as the limit
		std::pair<float,float> kneePointLimits = GetActiveCamera()->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,1);
		float exposure = GetActiveCamera()->GetFeatureValue(FEATURE_SHUTTER);
		bool useCameraKpLimits = (kneePointLimits.second < exposure);
		
    	if (useCameraKpLimits) m_KPSlider1.SetRange(kneePointLimits.first + MIN_INCREMENT, kneePointLimits.second - MIN_INCREMENT); 
		else                   m_KPSlider1.SetRange(kneePointLimits.first + MIN_INCREMENT, exposure - MIN_INCREMENT);
		m_KPCheck1.SetCheck(BST_UNCHECKED); m_KPCheck1.EnableWindow(true);
	
		if (kneePoints.size() > 0)
		{
		    if (useCameraKpLimits) kneePointLimits = GetActiveCamera()->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,2);
			m_KPCheck1.SetCheck(BST_CHECKED);
			m_KPSlider1.SetValue(kneePoints[0]);
			if (useCameraKpLimits) m_KPSlider2.SetRange(kneePointLimits.first + MIN_INCREMENT, kneePointLimits.second - MIN_INCREMENT); 
			else                   m_KPSlider2.SetRange(kneePoints[0] + MIN_INCREMENT, exposure - MIN_INCREMENT);
		}

		if (kneePoints.size() > 1)
		{
		    if (useCameraKpLimits) kneePointLimits = GetActiveCamera()->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,3);
    		m_KPCheck2.SetCheck(BST_CHECKED); m_KPCheck2.EnableWindow(true);
			m_KPSlider2.SetValue(kneePoints[1]);
			if (useCameraKpLimits) m_KPSlider3.SetRange(kneePointLimits.first + MIN_INCREMENT, kneePointLimits.second  - MIN_INCREMENT);
			else                   m_KPSlider3.SetRange(kneePoints[1] + MIN_INCREMENT, exposure - MIN_INCREMENT);
		}

		if (kneePoints.size() > 2)
		{
		    if (useCameraKpLimits) kneePointLimits = GetActiveCamera()->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,4);
    		m_KPCheck3.SetCheck(BST_CHECKED); m_KPCheck3.EnableWindow(true);
			m_KPSlider3.SetValue(kneePoints[2]);
			if (useCameraKpLimits) m_KPSlider4.SetRange(kneePointLimits.first + MIN_INCREMENT, kneePointLimits.second  - MIN_INCREMENT);
			else                   m_KPSlider4.SetRange(kneePoints[2] + MIN_INCREMENT, exposure - MIN_INCREMENT);
		}

		if (kneePoints.size() > 3)
		{
    		m_KPCheck3.SetCheck(BST_CHECKED); m_KPCheck3.EnableWindow(true);
			m_KPSlider4.SetValue(kneePoints[3]);
		}

		// Redraw the color bar rectangle
		CRect rctColorBar;
		m_ColorBar.GetWindowRect(&rctColorBar);
		ScreenToClient(&rctColorBar);
		InvalidateRect(&rctColorBar);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}

/**
* Function: PopulatePixelFormat
* Purpose:  
*/
void CKneePointGui::PopulatePixelFormat(void)
{
	// The pixel format combo box has already been configured - the entries that it contains
	// at this point should be precisely the formats that the camera supports.  Note
    // the combo ONLY ever contains polar pixel formats, it is unpopulated for non-polar
    // cameras.
	if (m_PixelFormatCombo.GetCount() == 0)
	{
		m_PixelFormatCombo.EnableWindow(FALSE);
		m_PixelFormatInterpretationCombo.EnableWindow(FALSE);
		return;
	}

    try
	{
		if (GetActiveCamera()->FeatureSupported(FEATURE_PIXEL_FORMAT))
		{
			int fmt = static_cast<int>(GetActiveCamera()->GetFeatureValue(FEATURE_PIXEL_FORMAT));

			LOCK_FEATURE(PIXEL_FORMAT);
			SelectByItemData(m_PixelFormatCombo, fmt);

            SetPixelFormatInterpretationEntries();
            if (fmt == PIXEL_FORMAT_HSV4_12)
            {
                int HSVIntpretation = static_cast<int>(GetActiveCamera()->GetFeatureValue(FEATURE_POLAR_HSV_INTERPRETATION));
    			SelectByItemData(m_PixelFormatInterpretationCombo, HSVIntpretation);
            }
		}
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: SetPixelFormatEntries
* Purpose:  Fill the Pixel Format combo box with only those values that are
*           supported by the current camera.  NOTE, if this is a non-polar
*           camera, then then set remains NULL
*/
void CKneePointGui::SetPixelFormatEntries(void)
{
	ClearComboBox(m_PixelFormatCombo);
    if (! GetActiveCamera()->FeatureSupported(FEATURE_POLAR_WEIGHTINGS)) return;

	std::vector<int> const& formats = GetActiveCamera()->GetSupportedPixelFormats();

	for (std::vector<int>::const_iterator iter = formats.begin(); iter != formats.end(); iter++)
	{
		int idx = m_PixelFormatCombo.AddString(ApiConstAsString(CT_PIXEL_FORMAT, *iter));
		if (idx != CB_ERR)
			m_PixelFormatCombo.SetItemData(idx, static_cast<DWORD>(*iter));
	}
}

/**
* Function: SetPixelFormatInterpretationEntries
* Purpose:  Fill the Pixel Format Intpretations combo box with only those values that are
*           supported by the current camera and pixel format.
*/
void CKneePointGui::SetPixelFormatInterpretationEntries(void)
{
	ClearComboBox(m_PixelFormatInterpretationCombo);
	if (GetActiveCamera()->FeatureSupported(FEATURE_PIXEL_FORMAT))
    {
        int fmt = static_cast<int>(GetActiveCamera()->GetFeatureValue(FEATURE_PIXEL_FORMAT));

        if (fmt == PIXEL_FORMAT_HSV4_12)
        {
        	for (int iter = CT_HSV_AS_COLOR; iter <= CT_HSV_AS_DEGREE; iter++)
            {
        		int idx = m_PixelFormatInterpretationCombo.AddString(ApiConstAsString(CT_PIXEL_FORMAT_INTERPRETATION, iter));
		        if (idx != CB_ERR)
			        m_PixelFormatInterpretationCombo.SetItemData(idx, iter);
            }
        }
    }
	m_PixelFormatInterpretationCombo.EnableWindow(m_PixelFormatInterpretationCombo.GetCount() != 0);
}

/**
* Function: PopulatePolars
* Purpose:  
*/
void CKneePointGui::PopulatePolars(void)
{
	// Should only be called for polar camera, but just in case...
    if (!(GetActiveCamera()->FeatureSupported(FEATURE_POLAR_WEIGHTINGS))) {
		return;
	}
	try
	{
        std::pair<float,float> polarLimits = GetActiveCamera()->GetFeatureLimits(FEATURE_POLAR_WEIGHTINGS);
        
        std::vector<float> weightings = GetActiveCamera()->GetPolarWeightings();

        m_KPPolar0Slider.SetValue(weightings[FEATURE_POLAR_WEIGHTINGS_0_DEG]);
        m_KPPolar45Slider.SetValue(weightings[FEATURE_POLAR_WEIGHTINGS_45_DEG]);
        m_KPPolar90Slider.SetValue(weightings[FEATURE_POLAR_WEIGHTINGS_90_DEG]);
        m_KPPolar135Slider.SetValue(weightings[FEATURE_POLAR_WEIGHTINGS_135_DEG]);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}

/**
* Function: PopulatePtp
* Purpose:  
*/
void CKneePointGui::PopulatePtp(void)
{
	if (GetActiveCamera()->FeatureSupported(FEATURE_PTP))
	{
		try
		{
			TempVal<bool> temp(m_bAdjusting[FEATURE_PTP], true);

      		U32  ptpFlags = 0;
            U32  ptpStatus = FEATURE_PTP_STATUS_DISABLED; // Assume this
            U32  ptpMode = (U32) GetActiveCamera()->GetFeatureValue(FEATURE_PTP, &ptpFlags, FEATURE_PTP_PARAM_MODE);
            bool off = ((ptpFlags & FEATURE_FLAG_OFF) != 0);
            bool ptpSlaveOnly = ptpMode == FEATURE_PTP_MODE_SLAVE_ONLY;
			
            m_PtpOnOffCheck.SetCheck(!off);
            m_PtpSlaveOnlyCheck.SetCheck(ptpSlaveOnly);
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}

/**
* Function: PopulatePeriodicPtp
* Purpose:  
*/
void CKneePointGui::PopulatePeriodicPtp(void)
{
	BOOL bPTPUpdated = FALSE;
	BOOL bTimeUpdated = FALSE;

    try
	{
        if (! GetDocument()->HasActiveCamera()) return;  // Bugzilla.2024 - No point in checking if I don't have a camera
		
        TempVal<bool> temp(m_bAdjusting[FEATURE_PTP], true);

        U32 ptpFlags = 0;
        if (GetActiveCamera()->FeatureSupported(FEATURE_PTP))
        {
            U32 ptpStatus = (U32) GetActiveCamera()->GetFeatureValue(FEATURE_PTP, &ptpFlags, FEATURE_PTP_PARAM_STATUS);
            // Bugzilla.2007 -- Update the status even if it is disabled (status is valid)
            m_PtpStateEdit.SetWindowText(GetPtpState(ptpStatus));
            bPTPUpdated = TRUE;
        }

        CString s;
        double currentTimeInSecs = GetActiveCamera()->GetTimestamp();

        U32    wholeUnits = static_cast<U32>(currentTimeInSecs / (60.0*60.0*24.0));
        s.Format(_T("%d"), wholeUnits);
        m_PtpDaysEdit.SetWindowText(s);
        currentTimeInSecs -= wholeUnits * (60.0*60.0*24.0);

        wholeUnits = static_cast<U32>(currentTimeInSecs / (60.0*60.0));
        s.Format(_T("%d"), wholeUnits);
        m_PtpHoursEdit.SetWindowText(s);
        currentTimeInSecs -= wholeUnits * (60.0*60.0);

        wholeUnits = static_cast<U32>(currentTimeInSecs / 60.0);
        s.Format(_T("%d"), wholeUnits);
        m_PtpMinsEdit.SetWindowText(s);
        currentTimeInSecs -= wholeUnits * 60.0;

        s.Format(_T("%.2f"), currentTimeInSecs);
        m_PtpSecsEdit.SetWindowText(s);

        bTimeUpdated = TRUE;
    }
	catch (PxLException const& e)
	{
		if (!bPTPUpdated) m_PtpStateEdit.SetWindowText(" ");
        if (!bTimeUpdated)
        {
            m_PtpDaysEdit.SetWindowText(" ");
            m_PtpHoursEdit.SetWindowText(" ");
            m_PtpMinsEdit.SetWindowText(" ");
            m_PtpSecsEdit.SetWindowText(" ");
        }
        if ((e.GetReturnCode() != ApiInvalidHandleError) && // Bugzilla.2024 - Not an error if we already lost the camera
            (e.GetReturnCode() != ApiNotSupportedError))  // It's not an error if the camrea does not support it.
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
void CKneePointGui::FeatureChanged(U32 featureId)
{
	switch (featureId)
	{
	case FEATURE_SHUTTER:
		PopulateExposure();
		PopulateKneePoints(); // Changing Exposure Time may affect Knee Points.
		break;
	case FEATURE_EXTENDED_SHUTTER:
		PopulateKneePoints();
		PopulateExposure(); // Bugzilla.226, enabling/disabling kneepoints may require autoexpsore enable/disable
		break;
	case FEATURE_TRIGGER:
		PopulateExposure(); // Bugzilla.419, enabling/disabling kneepoints may require autoexpsore enable/disable
		break;
	case FEATURE_PIXEL_FORMAT:
		PopulatePixelFormat();
		break;
	case FEATURE_POLAR_WEIGHTINGS:
		PopulatePolars();
		break;
	default:
		// A feature that is not relevant to this page. Ignore the change.
		break;
	}
}


/**
* Function: SetKneePoints
* Purpose:  
*/
void CKneePointGui::SetKneePoints(void)
{
	try
	{
		std::vector<float> knees;
		float val;

		if (m_KPCheck1.GetCheck() == BST_CHECKED)
		{
			val = m_KPSlider1.GetValue();
			knees.push_back(val);

			if (m_KPCheck2.GetCheck() == BST_CHECKED)
			{
				val = m_KPSlider2.GetValue();
				// Bugzilla.631 The slider limits already comensate for MIN_INCREMENT, so no seed to
				// compensate for it again.
				if (val >= m_KPSlider2.GetMin())
				{
					knees.push_back(val);

					if (m_KPCheck3.GetCheck() == BST_CHECKED)
					{
						val = m_KPSlider3.GetValue();
						if (val >= m_KPSlider3.GetMin())
						{
							knees.push_back(val);

							if (m_KPCheck4.GetCheck() == BST_CHECKED)
							{
								val = m_KPSlider4.GetValue();
								if (val >= m_KPSlider4.GetMin())
								{
									knees.push_back(val);
								}
							}
						}
					}
				}
			}
		}

		GetActiveCamera()->SetKneePoints(knees);
	} catch (PxLException const& e) {
		OnApiError(e);
	}
}


/******************************************************************************
 * CKneePointGui message handlers
 *****************************************************************************/

/******************************************************************************
* See "Note on Slider controls" in CSimpleGui.cpp
******************************************************************************/

/**
* Function: OnScrollExposureSlider
* Purpose:  Scroll event handler for the Exposure slider.
*/
void CKneePointGui::OnScrollExposureSlider()
{
	// Remove this handler if you do not want continuous updates to the
	// camera while dragging the slider.
	SliderScrolled(m_ExposureSlider, FEATURE_SHUTTER);
}


/**
* Function: OnChangeExposureSlider
* Purpose:  Change event handler for the Exposure slider.
*/
void CKneePointGui::OnChangeExposureSlider()
{
	SliderChanged(m_ExposureSlider, FEATURE_SHUTTER);
}


/**
* Function: OnKillFocusExposureEdit
* Purpose:  
*/
void CKneePointGui::OnKillFocusExposureEdit()
{
	SliderEditFocusKilled(m_ExposureSlider, FEATURE_SHUTTER);
}


/**
* Function: OnClickExposureAutoButton
* Purpose:  Click Event handler for the Auto Exposure button.
*/
void CKneePointGui::OnClickExposureAutoButton()
{
	try
	{
		GetActiveCamera()->SetOnePush(FEATURE_SHUTTER);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


void CKneePointGui::OnClickKneePointCheck1(void)
{
	if (m_KPCheck1.GetCheck() == BST_CHECKED)
	{
		// Bugzilla.178 - Limit the kneepoints to the feature limits.  However, not all cameras will set them
		// properly, so for those cameras -- use the shutter as the limit
		std::pair<float,float> kneePointLimits = GetActiveCamera()->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,1);
		float exposure = GetActiveCamera()->GetFeatureValue(FEATURE_SHUTTER);
		bool useCameraKpLimits = (kneePointLimits.second < exposure);

		if (useCameraKpLimits) m_KPSlider1.SetRange(kneePointLimits.first + MIN_INCREMENT, kneePointLimits.second - MIN_INCREMENT);
		else                   m_KPSlider1.SetRange(kneePointLimits.first + MIN_INCREMENT, exposure - MIN_INCREMENT);
	}
	SetKneePoints();
}


void CKneePointGui::OnClickKneePointCheck2(void)
{
	if (m_KPCheck2.GetCheck() == BST_CHECKED)
	{
		// Bugzilla.178 - Limit the kneepoints to the feature limits.  However, not all cameras will set them
		// properly, so for those cameras -- use the shutter as the limit
		std::pair<float,float> kneePointLimits = GetActiveCamera()->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,2);
		float exposure = GetActiveCamera()->GetFeatureValue(FEATURE_SHUTTER);
		bool useCameraKpLimits = (kneePointLimits.second < exposure);

		if (useCameraKpLimits) m_KPSlider2.SetRange(kneePointLimits.first + MIN_INCREMENT, kneePointLimits.second - MIN_INCREMENT);
		else                   m_KPSlider2.SetRange(kneePointLimits.first + MIN_INCREMENT, exposure - MIN_INCREMENT);
	}
	SetKneePoints();
}


void CKneePointGui::OnClickKneePointCheck3(void)
{
	if (m_KPCheck3.GetCheck() == BST_CHECKED)
	{
		// Bugzilla.178 - Limit the kneepoints to the feature limits.  However, not all cameras will set them
		// properly, so for those cameras -- use the shutter as the limit
		std::pair<float,float> kneePointLimits = GetActiveCamera()->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,3);
		float exposure = GetActiveCamera()->GetFeatureValue(FEATURE_SHUTTER);
		bool useCameraKpLimits = (kneePointLimits.second < exposure);

		if (useCameraKpLimits) m_KPSlider3.SetRange(kneePointLimits.first + MIN_INCREMENT, kneePointLimits.second - MIN_INCREMENT);
		else                   m_KPSlider3.SetRange(kneePointLimits.first + MIN_INCREMENT, exposure - MIN_INCREMENT);
	}
	SetKneePoints();
}


void CKneePointGui::OnClickKneePointCheck4(void)
{
	if (m_KPCheck4.GetCheck() == BST_CHECKED)
	{
		// Bugzilla.178 - Limit the kneepoints to the feature limits.  However, not all cameras will set them
		// properly, so for those cameras -- use the shutter as the limit
		std::pair<float,float> kneePointLimits = GetActiveCamera()->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,4);
		float exposure = GetActiveCamera()->GetFeatureValue(FEATURE_SHUTTER);
		bool useCameraKpLimits = (kneePointLimits.second < exposure);

		if (useCameraKpLimits) m_KPSlider4.SetRange(kneePointLimits.first + MIN_INCREMENT, kneePointLimits.second - MIN_INCREMENT);
		else                   m_KPSlider4.SetRange(kneePointLimits.first + MIN_INCREMENT, exposure - MIN_INCREMENT);
	}
	SetKneePoints();
}


void CKneePointGui::OnScrollKneePointSlider1(void)
{
	m_KPSlider1.OnScroll();

	if (m_KPSlider1.GetValue() <= m_KPSlider1.GetMin() + MIN_INCREMENT)
	{
		m_KPCheck4.SetCheck(BST_UNCHECKED); m_KPCheck4.EnableWindow(false);
		m_KPCheck3.SetCheck(BST_UNCHECKED); m_KPCheck3.EnableWindow(false);
		m_KPCheck2.SetCheck(BST_UNCHECKED); m_KPCheck2.EnableWindow(false);
	}
	else 
	{
	    if (m_MaxKnees >= 2) m_KPCheck2.EnableWindow(true);
        // Bugzilla.178 - Limit the kneepoints to the feature limits.  However, not all cameras will set them
	    // properly, so for those cameras -- use the shutter as the limit
	    std::pair<float,float> kneePointLimits = GetActiveCamera()->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,2);
	    float exposure = GetActiveCamera()->GetFeatureValue(FEATURE_SHUTTER);
	    bool useCameraKpLimits = (kneePointLimits.second < exposure);

	    if (useCameraKpLimits) 
	        m_KPSlider2.SetRange(m_KPSlider1.GetValue() + MIN_INCREMENT, kneePointLimits.second - MIN_INCREMENT);
	    else                   
	        m_KPSlider2.SetRange(m_KPSlider1.GetValue() + MIN_INCREMENT, exposure - MIN_INCREMENT);
	}
	SetKneePoints();
}


void CKneePointGui::OnChangeKneePointSlider1(void)
{
	m_KPSlider1.OnChange();

    try
    {
	    if (m_bAdjusting[FEATURE_EXTENDED_SHUTTER])
		    return;

	    if (m_KPSlider1.GetValue() <= m_KPSlider1.GetMin() + MIN_INCREMENT)
	    {
		    m_KPCheck4.SetCheck(BST_UNCHECKED); m_KPCheck4.EnableWindow(false);
		    m_KPCheck3.SetCheck(BST_UNCHECKED); m_KPCheck3.EnableWindow(false);
		    m_KPCheck2.SetCheck(BST_UNCHECKED); m_KPCheck2.EnableWindow(false);
	    }
	    else 
	    {
		    if (m_MaxKnees >= 2) m_KPCheck2.EnableWindow(true);
		    // Bugzilla.178 - Limit the kneepoints to the feature limits.  However, not all cameras will set them
		    // properly, so for those cameras -- use the shutter as the limit
		    std::pair<float,float> kneePointLimits = GetActiveCamera()->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,2);
		    float exposure = GetActiveCamera()->GetFeatureValue(FEATURE_SHUTTER);
		    bool useCameraKpLimits = (kneePointLimits.second < exposure);

		    if (useCameraKpLimits) 
		        m_KPSlider2.SetRange(m_KPSlider1.GetValue() + MIN_INCREMENT, kneePointLimits.second - MIN_INCREMENT);
		    else                   
		        m_KPSlider2.SetRange(m_KPSlider1.GetValue() + MIN_INCREMENT, exposure - MIN_INCREMENT);
	    }
	    SetKneePoints();
	} catch (PxLException const& e) {
		OnApiError(e);
	}
}


void CKneePointGui::OnScrollKneePointSlider2(void)
{
	m_KPSlider2.OnScroll();

	if (m_KPSlider2.GetValue() <= m_KPSlider2.GetMin())
	{
		m_KPCheck4.SetCheck(BST_UNCHECKED); m_KPCheck4.EnableWindow(false);
		m_KPCheck3.SetCheck(BST_UNCHECKED); m_KPCheck3.EnableWindow(false);
	}
	else 
	{
		 if (m_MaxKnees >= 3) m_KPCheck3.EnableWindow(false);
		// Bugzilla.178 - Limit the kneepoints to the feature limits.  However, not all cameras will set them
		// properly, so for those cameras -- use the shutter as the limit
		std::pair<float,float> kneePointLimits = GetActiveCamera()->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,3);
		float exposure = GetActiveCamera()->GetFeatureValue(FEATURE_SHUTTER);
		bool useCameraKpLimits = (kneePointLimits.second < exposure);

		if (useCameraKpLimits) m_KPSlider3.SetRange(m_KPSlider2.GetValue() + MIN_INCREMENT, kneePointLimits.second - MIN_INCREMENT);
		else                   m_KPSlider3.SetRange(m_KPSlider2.GetValue() + MIN_INCREMENT, exposure - MIN_INCREMENT);
	}
	SetKneePoints();
}


void CKneePointGui::OnChangeKneePointSlider2(void)
{
	m_KPSlider2.OnChange();

	try 
	{
	    if (m_bAdjusting[FEATURE_EXTENDED_SHUTTER])
		    return;

	    if (m_KPSlider2.GetValue() <= m_KPSlider2.GetMin() + MIN_INCREMENT)
	    {
		    m_KPCheck4.SetCheck(BST_UNCHECKED); m_KPCheck4.EnableWindow(false);
		    m_KPCheck3.SetCheck(BST_UNCHECKED); m_KPCheck3.EnableWindow(false);
	    }
	    else
	    {
		    if (m_MaxKnees >= 3) m_KPCheck3.EnableWindow(true);
		    // Bugzilla.178 - Limit the kneepoints to the feature limits.  However, not all cameras will set them
		    // properly, so for those cameras -- use the shutter as the limit
		    std::pair<float,float> kneePointLimits = GetActiveCamera()->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,3);
		    float exposure = GetActiveCamera()->GetFeatureValue(FEATURE_SHUTTER);
		    bool useCameraKpLimits = (kneePointLimits.second < exposure);

		    if (useCameraKpLimits) m_KPSlider3.SetRange(m_KPSlider2.GetValue() + MIN_INCREMENT, kneePointLimits.second - MIN_INCREMENT);
		    else                   m_KPSlider3.SetRange(m_KPSlider2.GetValue() + MIN_INCREMENT, exposure - MIN_INCREMENT);
	    }
	    SetKneePoints();
	} catch (PxLException const& e) {
		OnApiError(e);
	}
}


void CKneePointGui::OnScrollKneePointSlider3(void)
{
	m_KPSlider3.OnScroll();

	if (m_KPSlider3.GetValue() <= m_KPSlider3.GetMin() + MIN_INCREMENT)
	{
		m_KPCheck4.SetCheck(BST_UNCHECKED); m_KPCheck4.EnableWindow(false);
	}
	else 
	{
		if (m_MaxKnees >= 4) m_KPCheck4.EnableWindow(false);
		// Bugzilla.178 - Limit the kneepoints to the feature limits.  However, not all cameras will set them
		// properly, so for those cameras -- use the shutter as the limit
		std::pair<float,float> kneePointLimits = GetActiveCamera()->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,4);
		float exposure = GetActiveCamera()->GetFeatureValue(FEATURE_SHUTTER);
		bool useCameraKpLimits = (kneePointLimits.second < exposure);

		if (useCameraKpLimits) m_KPSlider4.SetRange(m_KPSlider3.GetValue() + MIN_INCREMENT, kneePointLimits.second - MIN_INCREMENT);
		else                   m_KPSlider4.SetRange(m_KPSlider3.GetValue() + MIN_INCREMENT, exposure - MIN_INCREMENT);
	}
	SetKneePoints();
}


void CKneePointGui::OnChangeKneePointSlider3(void)
{
	m_KPSlider3.OnChange();

	try 
	{
	    if (m_bAdjusting[FEATURE_EXTENDED_SHUTTER])
		    return;

	    if (m_KPSlider3.GetValue() == m_KPSlider3.GetMin())
	    {
		    m_KPCheck4.SetCheck(BST_UNCHECKED); m_KPCheck4.EnableWindow(false);
	    }
	    else 
	    {
		    if (m_MaxKnees >= 4) m_KPCheck4.EnableWindow(true);
		    // Bugzilla.178 - Limit the kneepoints to the feature limits.  However, not all cameras will set them
		    // properly, so for those cameras -- use the shutter as the limit
		    std::pair<float,float> kneePointLimits = GetActiveCamera()->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,4);
		    float exposure = GetActiveCamera()->GetFeatureValue(FEATURE_SHUTTER);
		    bool useCameraKpLimits = (kneePointLimits.second < exposure);

		    if (useCameraKpLimits) m_KPSlider4.SetRange(m_KPSlider3.GetValue() + MIN_INCREMENT, kneePointLimits.second - MIN_INCREMENT);
		    else                   m_KPSlider4.SetRange(m_KPSlider3.GetValue() + MIN_INCREMENT, exposure - MIN_INCREMENT);
	    }
	    SetKneePoints();
	} catch (PxLException const& e) {
		OnApiError(e);
	}
}


void CKneePointGui::OnScrollKneePointSlider4(void)
{
	m_KPSlider4.OnScroll();

	SetKneePoints();
}


void CKneePointGui::OnChangeKneePointSlider4(void)
{
	m_KPSlider4.OnChange();

	try
	{
	    if (m_bAdjusting[FEATURE_EXTENDED_SHUTTER])
		    return;

	    SetKneePoints();
	} catch (PxLException const& e) {
		OnApiError(e);
	}
}


void CKneePointGui::OnKillFocusKneePointEdit1(void)
{
	{
		TempVal<bool> temp(m_bAdjusting[FEATURE_EXTENDED_SHUTTER], true);
		m_KPSlider1.UpdateFromWnd();
	}
	SetKneePoints();
}


void CKneePointGui::OnKillFocusKneePointEdit2(void)
{
	{
		TempVal<bool> temp(m_bAdjusting[FEATURE_EXTENDED_SHUTTER], true);
		m_KPSlider2.UpdateFromWnd();
	}
	SetKneePoints();
}


void CKneePointGui::OnKillFocusKneePointEdit3(void)
{
	{
		TempVal<bool> temp(m_bAdjusting[FEATURE_EXTENDED_SHUTTER], true);
		m_KPSlider3.UpdateFromWnd();
	}
	SetKneePoints();
}


void CKneePointGui::OnKillFocusKneePointEdit4(void)
{
	{
		TempVal<bool> temp(m_bAdjusting[FEATURE_EXTENDED_SHUTTER], true);
		m_KPSlider4.UpdateFromWnd();
	}
	SetKneePoints();
}


/******************************************************************************
 * UPDATE_COMMAND_UI handlers.
 *****************************************************************************/

void CKneePointGui::OnUpdateKneePoint1(CCmdUI *pCmdUI)
{
	BOOL enable = m_MaxKnees >= 1
					&& m_KPCheck1.GetCheck() == BST_CHECKED;
	pCmdUI->Enable(enable);
	if (enable != m_KPSlider1.get_Enabled())
		m_KPSlider1.put_Enabled(enable);
}


void CKneePointGui::OnUpdateKneePointCheck2(CCmdUI *pCmdUI)
{
	BOOL enable = m_MaxKnees >= 2
					&& m_KPCheck1.GetCheck() == BST_CHECKED
					&& m_KPSlider1.GetValue() < m_KPSlider2.GetMin();
	pCmdUI->Enable(enable);
	if (!enable)
		pCmdUI->SetCheck(BST_UNCHECKED);
}


void CKneePointGui::OnUpdateKneePoint2(CCmdUI *pCmdUI)
{
	BOOL enable = m_KPCheck2.GetCheck() == BST_CHECKED;
	pCmdUI->Enable(enable);
	if (enable != m_KPSlider2.get_Enabled())
		m_KPSlider2.put_Enabled(enable);
}


void CKneePointGui::OnUpdateKneePointCheck3(CCmdUI *pCmdUI)
{
	BOOL enable = m_MaxKnees >= 3
					&& m_KPCheck2.GetCheck() == BST_CHECKED
					&& m_KPSlider2.GetValue() < m_KPSlider3.GetMin();
	pCmdUI->Enable(enable);
	if (!enable)
		pCmdUI->SetCheck(BST_UNCHECKED);
}


void CKneePointGui::OnUpdateKneePoint3(CCmdUI *pCmdUI)
{
	BOOL enable = m_KPCheck3.GetCheck() == BST_CHECKED;
	pCmdUI->Enable(enable);
	if (enable != m_KPSlider3.get_Enabled())
		m_KPSlider3.put_Enabled(enable);
}


void CKneePointGui::OnUpdateKneePointCheck4(CCmdUI *pCmdUI)
{
	BOOL enable = m_MaxKnees >= 4
					&& m_KPCheck3.GetCheck() == BST_CHECKED
					&& m_KPSlider3.GetValue() < m_KPSlider4.GetMin();
	pCmdUI->Enable(enable);
	if (!enable)
		pCmdUI->SetCheck(BST_UNCHECKED);
}


void CKneePointGui::OnUpdateKneePoint4(CCmdUI *pCmdUI)
{
	BOOL enable = m_KPCheck4.GetCheck() == BST_CHECKED;
	pCmdUI->Enable(enable);
	if (enable != m_KPSlider4.get_Enabled())
		m_KPSlider4.put_Enabled(enable);
}

/**
* Function: OnSelchangePixelFormatCombo
* Purpose:  
*/
void CKneePointGui::OnSelchangePixelFormatCombo(void)
{
	if (!m_bAdjusting[FEATURE_PIXEL_FORMAT] && m_PixelFormatCombo.GetCurSel() != -1)
	{
		float fmt = ItemData<float>(m_PixelFormatCombo);
		try
		{
			GetActiveCamera()->SetFeatureValue(FEATURE_PIXEL_FORMAT, fmt);
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
        SetPixelFormatInterpretationEntries();
	}
}

/**
* Function: OnSelchangePixelFormatInterpretationsCombo
* Purpose:  
*/
void CKneePointGui::OnSelchangePixelFormatInterpretationCombo(void)
{
	if (m_PixelFormatInterpretationCombo.GetCurSel() != -1)
	{
		std::auto_ptr<CPreviewState> ps(NULL);
    
        // If we are previewing, the stop and restart it, as the preview may have changed between color and mono
        if (GetActiveCamera()->GetPreviewState() == START_PREVIEW)
				ps = std::auto_ptr<CPreviewState>(new CPreviewState(GetActiveCamera(), PAUSE_PREVIEW));

        try
		{
    		float interpretation = ItemData<float>(m_PixelFormatInterpretationCombo);
			GetActiveCamera()->SetFeatureValue(FEATURE_POLAR_HSV_INTERPRETATION, interpretation);
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}

/**
* Function: OnClickMinAllPolarsButton
* Purpose:  Set all polar weightings to their minimum value.
*/
void CKneePointGui::OnClickMinAllPolarsButton(void)
{
	// Should only be called for polar camera, but just in case...
    if (!(GetActiveCamera()->FeatureSupported(FEATURE_POLAR_WEIGHTINGS))) {
		return;
	}
        
	CWaitCursor wc;

    try
	{
        std::pair<float,float> polarLimits = GetActiveCamera()->GetFeatureLimits(FEATURE_POLAR_WEIGHTINGS);
        static const float weightings[] = {polarLimits.first, polarLimits.first, polarLimits.first, polarLimits.first};
        std::vector<float> weights (weightings, weightings + sizeof(weightings) / sizeof(weightings[0]));

        GetActiveCamera()->SetPolarWeightings(weights);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}

/**
* Function: OnClickMaxAllPolarsButton
* Purpose:  Set all polar weightings to their maximum value.
*/
void CKneePointGui::OnClickMaxAllPolarsButton(void)
{
	// Should only be called for polar camera, but just in case...
    if (!(GetActiveCamera()->FeatureSupported(FEATURE_POLAR_WEIGHTINGS))) {
		return;
	}
        
	CWaitCursor wc;

    try
	{
        std::pair<float,float> polarLimits = GetActiveCamera()->GetFeatureLimits(FEATURE_POLAR_WEIGHTINGS);
        float weightings[] = {polarLimits.second, polarLimits.second, polarLimits.second, polarLimits.second};
        std::vector<float> weights (weightings, weightings + sizeof(weightings) / sizeof(weightings[0]));

        GetActiveCamera()->SetPolarWeightings(weights);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}

/**
* Functions: OnScrollPolarSliders
* Purpose:  
*/
void CKneePointGui::OnScrollPolar0Slider()
{
	SliderScrolled(m_KPPolar0Slider, FEATURE_POLAR_WEIGHTINGS, FEATURE_POLAR_WEIGHTINGS_0_DEG);
}
void CKneePointGui::OnScrollPolar45Slider()
{
	SliderScrolled(m_KPPolar45Slider, FEATURE_POLAR_WEIGHTINGS, FEATURE_POLAR_WEIGHTINGS_45_DEG);
}
void CKneePointGui::OnScrollPolar90Slider()
{
	SliderScrolled(m_KPPolar90Slider, FEATURE_POLAR_WEIGHTINGS, FEATURE_POLAR_WEIGHTINGS_90_DEG);
}
void CKneePointGui::OnScrollPolar135Slider()
{
	SliderScrolled(m_KPPolar135Slider, FEATURE_POLAR_WEIGHTINGS, FEATURE_POLAR_WEIGHTINGS_135_DEG);
}

/**
* Functions: OnChangePolarSliders
* Purpose:  
*/
void CKneePointGui::OnChangePolar0Slider()
{
	SliderChanged(m_KPPolar0Slider, FEATURE_POLAR_WEIGHTINGS, FEATURE_POLAR_WEIGHTINGS_0_DEG);
}
void CKneePointGui::OnChangePolar45Slider()
{
	SliderChanged(m_KPPolar45Slider, FEATURE_POLAR_WEIGHTINGS, FEATURE_POLAR_WEIGHTINGS_45_DEG);
}
void CKneePointGui::OnChangePolar90Slider()
{
	SliderChanged(m_KPPolar90Slider, FEATURE_POLAR_WEIGHTINGS, FEATURE_POLAR_WEIGHTINGS_90_DEG);
}
void CKneePointGui::OnChangePolar135Slider()
{
	SliderChanged(m_KPPolar135Slider, FEATURE_POLAR_WEIGHTINGS, FEATURE_POLAR_WEIGHTINGS_135_DEG);
}

/**
* Functions: OnKillFocusPolarEdits
* Purpose:  
*/
void CKneePointGui::OnKillFocusPolar0Edit()
{
	SliderEditFocusKilled(m_KPPolar0Slider, FEATURE_POLAR_WEIGHTINGS, FEATURE_POLAR_WEIGHTINGS_0_DEG);
}
void CKneePointGui::OnKillFocusPolar45Edit()
{
	SliderEditFocusKilled(m_KPPolar45Slider, FEATURE_POLAR_WEIGHTINGS, FEATURE_POLAR_WEIGHTINGS_45_DEG);
}
void CKneePointGui::OnKillFocusPolar90Edit()
{
	SliderEditFocusKilled(m_KPPolar90Slider, FEATURE_POLAR_WEIGHTINGS, FEATURE_POLAR_WEIGHTINGS_90_DEG);
}
void CKneePointGui::OnKillFocusPolar135Edit()
{
	SliderEditFocusKilled(m_KPPolar135Slider, FEATURE_POLAR_WEIGHTINGS, FEATURE_POLAR_WEIGHTINGS_135_DEG);
}

/**
* Function: OnClickPtpOnOffCheck
* Purpose:  
*/
void CKneePointGui::OnClickPtpOnOffCheck(void)
{
	CWaitCursor wc;
	if (!m_bAdjusting[FEATURE_PTP])
	{
		try
		{
			bool turningPtpOn = m_PtpOnOffCheck.GetCheck() == BST_CHECKED;
            if (turningPtpOn)
            {
                // turn it on, we need to set the mode (slave only, or automatic)
                float ptpMode = static_cast<float> (m_PtpSlaveOnlyCheck.GetCheck() == BST_CHECKED ? FEATURE_PTP_MODE_SLAVE_ONLY : FEATURE_PTP_MODE_AUTOMATIC);
                GetActiveCamera()->SetFeatureValue (FEATURE_PTP, ptpMode, FEATURE_PTP_PARAM_MODE);
            } else {
                GetActiveCamera()->SetOnOff(FEATURE_PTP, turningPtpOn);
            }
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}

/**
* Function: OnClickPtpSlaveOnlyCheck
* Purpose:  
*/
void CKneePointGui::OnClickPtpSlaveOnlyCheck(void)
{
	bool ptpEnabled = m_PtpOnOffCheck.GetCheck() == BST_CHECKED;
    
    if (ptpEnabled)
    {
        CWaitCursor wc;
	    if (!m_bAdjusting[FEATURE_PTP])
	    {
		    try
		    {
                // ptp is on, we need to set the mode (slave only, or automatic)
                float ptpMode = static_cast<float> (m_PtpSlaveOnlyCheck.GetCheck() == BST_CHECKED ? FEATURE_PTP_MODE_SLAVE_ONLY : FEATURE_PTP_MODE_AUTOMATIC);
                GetActiveCamera()->SetFeatureValue (FEATURE_PTP, ptpMode, FEATURE_PTP_PARAM_MODE);
		    }
		    catch (PxLException const& e)
		    {
			    OnApiError(e);
		    }
	    }
    }
}

/**
* Function: StartUpdateTimer
* Purpose:  
*/
void CKneePointGui::StartUpdateTimer(void)
{
	m_timerId = SetTimer(PTP_TIMER_ID, 1000, NULL);
}


/**
* Function: StopUpdateTimer
* Purpose:  
*/
void CKneePointGui::StopUpdateTimer(void)
{
	if (m_timerId != 0)
		KillTimer(m_timerId);
	m_timerId = 0;
}


/**
* Function: OnTimer
* Purpose:  
*/
void CKneePointGui::OnTimer(UINT_PTR nIDEvent)
{
	if (PTP_TIMER_ID == nIDEvent)
	{
		PopulatePeriodicPtp();
	}

	CDevAppPage::OnTimer(nIDEvent);
}

void CKneePointGui::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if (m_KPCheck1.GetCheck() == BST_UNCHECKED)
		return;

	COLORREF clr0 = RGB(0xFF, 0xFF, 0xFF);
	COLORREF clr1 = RGB(0xC0, 0xC0, 0xC0);
	COLORREF clr2 = RGB(0x90, 0x90, 0x90);
	COLORREF clr3 = RGB(0x60, 0x60, 0x60);
	COLORREF clr4 = RGB(0x30, 0xB0, 0x30);

	// Get the rectangle of the whole color bar
	CRect r;
	m_ColorBar.GetWindowRect(&r);
	ScreenToClient(&r);
	long fullWidth = r.Width();

	float exposure = m_ExposureSlider.GetValue();

	CBrush brush0(clr0);
	dc.SelectObject(&brush0);
	dc.Rectangle(&r);

	if (m_KPCheck1.GetCheck() == BST_CHECKED)
	{
		CBrush brush1(clr1);
		dc.SelectObject(&brush1);
		r.left = static_cast<long>(r.right - fullWidth * (1.0 - (m_KPSlider1.GetValue() / exposure)));
		dc.Rectangle(&r);
	}
	else return;

	if (m_KPCheck2.GetCheck() == BST_CHECKED)
	{
		CBrush brush2(clr2);
		dc.SelectObject(&brush2);
		r.left = static_cast<long>(r.right - fullWidth * (1.0 - (m_KPSlider2.GetValue() / exposure)));
		dc.Rectangle(&r);
	}
	else return;

	if (m_KPCheck3.GetCheck() == BST_CHECKED)
	{
		CBrush brush3(clr3);
		dc.SelectObject(&brush3);
		r.left = static_cast<long>(r.right - fullWidth * (1.0 - (m_KPSlider3.GetValue() / exposure)));
		dc.Rectangle(&r);
	}
	else return;

	if (m_KPCheck4.GetCheck() == BST_CHECKED)
	{
		CBrush brush4(clr4);
		dc.SelectObject(&brush4);
		r.left = static_cast<long>(r.right - fullWidth * (1.0 - (m_KPSlider4.GetValue() / exposure)));
		dc.Rectangle(&r);
	}
	else return;

	// Do not call CDevAppPage::OnPaint() for painting messages
}


















