// GpioGui.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "GpioGui.h"


// CGpioGui dialog

IMPLEMENT_DYNAMIC(CGpioGui, CDevAppPage)


/**
* Function: CGpioGui
* Purpose:  Constructor
*/
CGpioGui::CGpioGui(CPxLDevAppView* pView)
	: CDevAppPage(CGpioGui::IDD, pView)
	, m_pLock(new CMutex)
{
}


/**
* Function: ~CGpioGui
* Purpose:  Destructor
*/
CGpioGui::~CGpioGui()
{
    delete m_pLock;
}


/**
* Function: DoDataExchange
* Purpose:  
*/
void CGpioGui::DoDataExchange(CDataExchange* pDX)
{
	CDevAppPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_GG_CAMERASELECT_COMBO, m_CameraSelectCombo);
	DDX_Control(pDX, IDC_GG_PLAY_BUTTON, m_PlayButton);
	DDX_Control(pDX, IDC_GG_PAUSE_BUTTON, m_PauseButton);
	DDX_Control(pDX, IDC_GG_STOP_BUTTON, m_StopButton);

	DDX_Control(pDX, IDC_GG_TRIGGERONOFF_CHECK, m_TriggerOnOffCheck);
	DDX_Control(pDX, IDC_GG_TRIGGERMODE_COMBO, m_TriggerModeCombo);
	DDX_Control(pDX, IDC_GG_TRIGGERTYPE_COMBO, m_TriggerTypeCombo);
	DDX_Control(pDX, IDC_GG_TRIGGERPOLARITY_COMBO, m_TriggerPolarityCombo);
	DDX_Control(pDX, IDC_GG_TRIGGERDELAY_EDIT, m_TriggerDelayEdit);
	DDX_Control(pDX, IDC_GG_TRIGGERPARAMETER_LABEL, m_TriggerParameterLabel);
	DDX_Control(pDX, IDC_GG_TRIGGERPARAMETER_EDIT, m_TriggerParameterEdit);
	DDX_Control(pDX, IDC_GG_CONTROLLED_LIGHTING_CHECK, m_TriggerControlledLightingCheck);

	DDX_Control(pDX, IDC_GG_GPOONOFF_CHECK, m_GpoOnOffCheck);
	DDX_Control(pDX, IDC_GG_GPONNUMBER_COMBO, m_GpoNumberCombo);
	DDX_Control(pDX, IDC_GG_GPOMODE_COMBO, m_GpoModeCombo);
	DDX_Control(pDX, IDC_GG_GPOPOLARITY_COMBO, m_GpoPolarityCombo);
	DDX_Control(pDX, IDC_GG_GPOPARAMETER1_LABEL, m_GpoParameter1Label);
	DDX_Control(pDX, IDC_GG_GPOPARAMETER2_LABEL, m_GpoParameter2Label);
	DDX_Control(pDX, IDC_GG_GPOPARAMETER3_LABEL, m_GpoParameter3Label);
	DDX_Control(pDX, IDC_GG_GPOPARAMETER1_EDIT, m_GpoParameter1Edit);
	DDX_Control(pDX, IDC_GG_GPOPARAMETER2_EDIT, m_GpoParameter2Edit);
	DDX_Control(pDX, IDC_GG_GPOPARAMETER3_EDIT, m_GpoParameter3Edit);
	DDX_Control(pDX, IDC_GG_GPOPARAMETER1_UNITS, m_GpoParameter1Units);
	DDX_Control(pDX, IDC_GG_GPOPARAMETER2_UNITS, m_GpoParameter2Units);
	DDX_Control(pDX, IDC_GG_GPOPARAMETER3_UNITS, m_GpoParameter3Units);

	DDX_Control(pDX, IDC_GG_UPDATETRIGGER_BUTTON, m_UpdateTriggerButton);
	DDX_Control(pDX, IDC_GG_EXPOSUREWARNING_STATIC, m_ExposureWarningStatic);
	DDX_Control(pDX, IDC_GG_UPDATEGPO_BUTTON, m_UpdateGpoButton);

	DDX_Control(pDX, IDC_GG_TRIGGERDESCRIPTION_EDIT, m_TriggerDescription);
	DDX_Control(pDX, IDC_GG_GPODESCRIPTION_EDIT, m_GpoDescription);

	DDX_Control(pDX, IDC_GG_ACTIONCOMMAND_COMBO, m_ActionCommandCombo);
	DDX_Control(pDX, IDC_GG_ACTIONDELAY_EDIT, m_ActionDelayEdit);
	DDX_Control(pDX, IDC_GG_SENDACTION_BUTTON, m_SendActionButton);

	DDX_Control(pDX, IDC_GG_EVENTS_EDIT, m_EventsEdit);
	DDX_Control(pDX, IDC_GG_CLEARACTIONS_BUTTON, m_ClearActionsButton);
}


BEGIN_MESSAGE_MAP(CGpioGui, CDevAppPage)

	ON_CBN_DROPDOWN(IDC_GG_CAMERASELECT_COMBO, OnDropdownCameraSelectCombo)
	ON_CBN_SELCHANGE(IDC_GG_CAMERASELECT_COMBO, OnSelchangeCameraSelectCombo)
	ON_BN_CLICKED(IDC_GG_PLAY_BUTTON, OnClickPlayButton)
	ON_BN_CLICKED(IDC_GG_PAUSE_BUTTON, OnClickPauseButton)
	ON_BN_CLICKED(IDC_GG_STOP_BUTTON, OnClickStopButton)

	ON_BN_CLICKED(IDC_GG_TRIGGERONOFF_CHECK, OnClickTriggerOnOffCheck)
	ON_CBN_SELCHANGE(IDC_GG_TRIGGERMODE_COMBO, OnSelchangeTriggerModeCombo)
	ON_BN_CLICKED(IDC_GG_UPDATETRIGGER_BUTTON, OnClickUpdateTriggerButton)
    ON_BN_CLICKED(IDC_GG_CONTROLLED_LIGHTING_CHECK, OnBnClickedGgControlledLightingCheck)

	ON_CBN_SELCHANGE(IDC_GG_GPONNUMBER_COMBO, OnSelchangeGpoNumberCombo)
	ON_BN_CLICKED(IDC_GG_GPOONOFF_CHECK, OnClickGpoOnOffCheck)
	ON_CBN_SELCHANGE(IDC_GG_GPOMODE_COMBO, OnSelchangeGpoModeCombo)
	ON_BN_CLICKED(IDC_GG_UPDATEGPO_BUTTON, OnClickUpdateGpoButton)

	ON_UPDATE_COMMAND_UI(IDC_GG_TRIGGERPARAMETER_EDIT, OnUpdateTriggerParameterEdit)
	ON_UPDATE_COMMAND_UI(IDC_GG_GPOPARAMETER1_EDIT, OnUpdateGpoParameter1Edit)
	ON_UPDATE_COMMAND_UI(IDC_GG_GPOPARAMETER2_EDIT, OnUpdateGpoParameter2Edit)
	ON_UPDATE_COMMAND_UI(IDC_GG_GPOPARAMETER3_EDIT, OnUpdateGpoParameter3Edit)
	ON_UPDATE_COMMAND_UI(IDC_GG_UPDATETRIGGER_BUTTON, OnUpdateUpdateTriggerButton)
	ON_UPDATE_COMMAND_UI(IDC_GG_UPDATEGPO_BUTTON, OnUpdateUpdateGpoButton)

	ON_BN_CLICKED(IDC_GG_SENDACTION_BUTTON, OnClickSendActionButton)

    ON_BN_CLICKED(IDC_GG_CLEARACTIONS_BUTTON, OnClickClearActionsButton)

END_MESSAGE_MAP()


/**
* Function: GetHelpContextId
* Purpose:  
*/
int CGpioGui::GetHelpContextId(void)
{
	return IDH_GPIOS_TAB;
}


/**
* Function: OnInitDialog
* Purpose:  
*/
BOOL CGpioGui::OnInitDialog()
{
	CDevAppPage::OnInitDialog();

	return TRUE;
}


/**
* Function: SetGUIState
* Purpose:  Enable / Disable controls, according to the state of the camera.
*           Most controls cannot be used in any meaningful way until a camera
*           is initialized.
*/
void CGpioGui::SetGUIState(eGUIState state)
{
	// The CameraSelect and Play/Pause/Stop button states are set in the base
	// class version of this function.
	CDevAppPage::SetGUIState(state);

	CPxLCamera* cam = GetActiveCamera();

	bool enable = (state != GS_NoCamera);

	bool enableTrigger = enable && cam->FeatureSupportsManual(FEATURE_TRIGGER);
	m_TriggerOnOffCheck.EnableWindow(enableTrigger);
	m_TriggerModeCombo.EnableWindow(enableTrigger);
	m_TriggerTypeCombo.EnableWindow(enableTrigger);
	m_TriggerPolarityCombo.EnableWindow(enableTrigger);
	m_TriggerDelayEdit.EnableWindow(enableTrigger);
	m_UpdateTriggerButton.EnableWindow(enableTrigger);
	// Note: m_TriggerParameterEdit is enabled & disabled by an UPDATE_COMMAND_UI handler.
	m_TriggerControlledLightingCheck.EnableWindow(enableTrigger && 
	                                              cam->FeatureSupportsManual(FEATURE_TRIGGER_WITH_CONTROLLED_LIGHT));

	m_GpoNumberCombo.EnableWindow(enable && cam->FeatureSupported(FEATURE_GPIO));

	bool enableGpo = enable && cam->FeatureSupportsManual(FEATURE_GPIO);
	m_GpoOnOffCheck.EnableWindow(enableGpo);
	m_GpoModeCombo.EnableWindow(enableGpo);
	m_GpoPolarityCombo.EnableWindow(enableGpo);
	m_UpdateGpoButton.EnableWindow(enableGpo);
	// Note: m_GpoParameterEdits are enabled & disabled by an UPDATE_COMMAND_UI handler.

    bool enableActionsCommands = enable && cam->IsActionsSupported();
    m_ActionCommandCombo.EnableWindow(enableActionsCommands);
    m_ActionDelayEdit.EnableWindow(enableActionsCommands);
    m_SendActionButton.EnableWindow(enableActionsCommands);
}


/**
* Function: ConfigureControls
* Purpose:  Set up the controls on the page to customize them according to the
*           currently active camera. This involves things like setting the max
*           and min values for sliders and setting the bounds of the subwindow
*           control.
*/
void CGpioGui::ConfigureControls(void)
{
	SetTriggerModeEntries(m_TriggerModeCombo, GetActiveCamera()->GetSupportedTriggerModes());
	SetTriggerTypeEntries(m_TriggerTypeCombo, GetActiveCamera()->GetSupportedTriggerTypes());
	SetPolarityEntries(m_TriggerPolarityCombo, GetActiveCamera()->GetFeatureLimits(FEATURE_TRIGGER, FEATURE_TRIGGER_PARAM_POLARITY));

    std::vector<int> supportedGpioModes = GetActiveCamera()->GetSupportedGpioModes();
    GpioProfiles gpioProfile = GetActiveCamera()->GetGpioProfile();
    if (contains(supportedGpioModes,GPIO_MODE_INPUT) &&
        gpioProfile == TWO_GPOS_ONE_GPI)
    {
        // The camera supports GPI, but not on the (default) GPIO#1, so don't display it as an option
        remove(supportedGpioModes,GPIO_MODE_INPUT);
    }

	SetGpoNumberEntries(m_GpoNumberCombo);
	SetGpioModeEntries(m_GpoModeCombo, supportedGpioModes);
	SetPolarityEntries(m_GpoPolarityCombo, GetActiveCamera()->GetFeatureLimits(FEATURE_GPIO, FEATURE_GPIO_PARAM_POLARITY));

    std::vector<int> supportedActionCommands;
    if (GetActiveCamera()->IsActionsSupported())
    {
        supportedActionCommands.push_back (ACTION_FRAME_TRIGGER); // If actions are supported -- this one is always supported
        if (gpioProfile == TWO_GPOS_ONE_GPI)
        {
            supportedActionCommands.push_back (ACTION_GPO1);
            supportedActionCommands.push_back (ACTION_GPO2);
        }
    }
    SetActionCommandEntries(m_ActionCommandCombo, supportedActionCommands);
}


/**
* Function: SetGpoNumberEntries
* Purpose:  Add the entries to the "Gpo Number" combo box.
*/
void CGpioGui::SetGpoNumberEntries(CComboBox& cb)
{
	ClearComboBox(cb);

    GpioProfiles gpioProfile = GetActiveCamera()->GetGpioProfile();
    int numGpios = 0;
    switch (gpioProfile)
    {
    case FOUR_GPOS:
        numGpios = 4;
        break;
    case TWO_GPOS_ONE_GPI:
        numGpios = 3;
        break;
    case TWO_GPOS:
    case ONE_GPIO_ONE_GPO:
        numGpios = 2;
        break;
    case ONE_GPIO:
    case ONE_GPO:
        numGpios = 1;
        break;
    default:
        numGpios = 0;
        break;
    }

	CString s;
	for (int gpoNum = 1; gpoNum <= numGpios; gpoNum++)
	{
        s.Format(_T("%d"), gpoNum);
		int idx = cb.AddString(s);
		cb.SetItemData(idx, static_cast<DWORD_PTR>(gpoNum));
	}
}

/**
* Function: PopulateDefaults
* Purpose:  Put some meaningles default values into the controls, for purely
*           aesthetic reasons.
*/
void CGpioGui::PopulateDefaults(void)
{
	ClearTriggerFields();
	m_GpoNumberCombo.SetCurSel(-1);
	ClearGpoFields();
}


/**
* Function: ClearTriggerFields
* Purpose:  
*/
void CGpioGui::ClearTriggerFields(void)
{
	m_TriggerOnOffCheck.SetCheck(BST_UNCHECKED);
	m_TriggerModeCombo.SetCurSel(-1);
	m_TriggerTypeCombo.SetCurSel(-1);
	m_TriggerPolarityCombo.SetCurSel(-1);
	m_TriggerDelayEdit.SetWindowText(_T(""));
	m_TriggerParameterEdit.SetWindowText(_T(""));
	m_TriggerControlledLightingCheck.SetCheck(BST_UNCHECKED);
}


/**
* Function: ClearGpoFields
* Purpose:  
*/
void CGpioGui::ClearGpoFields(void)
{
	m_GpoOnOffCheck.SetCheck(BST_UNCHECKED);
	m_GpoModeCombo.SetCurSel(-1);
	m_GpoPolarityCombo.SetCurSel(-1);
	m_GpoParameter1Edit.SetWindowText(_T(""));
	m_GpoParameter2Edit.SetWindowText(_T(""));
	m_GpoParameter3Edit.SetWindowText(_T(""));
    m_ExposureWarningStatic.ShowWindow(SW_HIDE);
}


/**
* Function: PopulateControls
* Purpose:  Make the values displayed in the controls reflect the current 
*           settings of the active camera.  Called immediately after a 
*           camera is initialized.
*/
void CGpioGui::PopulateControls(void)
{
	CDevAppPage::PopulateControls();

	PopulateTrigger();
	if (m_GpoNumberCombo.GetCurSel() == -1)
		m_GpoNumberCombo.SetCurSel(0);
	PopulateGpo();
	if (m_ActionCommandCombo.GetCurSel() == -1)
		m_ActionCommandCombo.SetCurSel(0);
    PopulateActionCommands();
    try 
    {
        // Report any of the camera events
        for (int i = EVENT_CAMERA_DISCONNECTED+1; i <= EVENT_LAST; i++)
        {
            GetActiveCamera()->SetEventCallback(i, this, EventHandler);
        }
    }
	catch (PxLException const& e)
	{
        if (e.GetReturnCode() != ApiNotSupportedError) OnApiError(e);
	}

}


/**
* Function: PopulateTrigger
* Purpose:  
*/
void CGpioGui::PopulateTrigger(void)
{
	if (!GetActiveCamera()->FeatureSupported(FEATURE_TRIGGER))
		return;

	try
	{
		U32 flags = 0;
		float   controlledLighting;
		Trigger trigger = GetActiveCamera()->GetTrigger(&flags);

		if ((flags & FEATURE_FLAG_OFF) != 0)
		{
			m_TriggerOnOffCheck.SetCheck(BST_UNCHECKED);
			// When the feature is OFF, the values of the parameters are just
			// returned as defaults (not as the last-set values), so it is
			// more user-friendly to *not* re-populate the rest of the Trigger
			// fields with this meaningless data - instead we just leave them
			// populated with the last-used values, so that next time the
			// feature is turned ON, you are setting it to these values.

			// However, we don't want to allow the drop-downs to have *no*
			// selected value, or the text fields to be empty (can happen
			// on app startup).
			if (m_TriggerModeCombo.GetCurSel() == -1)
				m_TriggerModeCombo.SetCurSel(0);
			if (m_TriggerTypeCombo.GetCurSel() == -1)
				m_TriggerTypeCombo.SetCurSel(0);
			if (m_TriggerPolarityCombo.GetCurSel() == -1)
				m_TriggerPolarityCombo.SetCurSel(0);
			if (m_TriggerDelayEdit.GetWindowTextLength() == 0)
				m_TriggerDelayEdit.SetWindowText(_T("0"));
			if (m_TriggerParameterEdit.GetWindowTextLength() == 0)
			{
				// Bugzilla.696 - given that 0 is an invalid value, use a default of 1.
				m_TriggerParameterEdit.SetWindowText(_T("1"));
			}
		}
		else
		{
			m_TriggerOnOffCheck.SetCheck(BST_CHECKED);

			SelectByItemData(m_TriggerModeCombo, trigger.Mode);
			SelectByItemData(m_TriggerTypeCombo, trigger.Type);
			SelectByItemData(m_TriggerPolarityCombo, trigger.Polarity);
			SetFloatText(m_TriggerDelayEdit, trigger.Delay, 5); // Bugzilla.1988

			if (trigger.Mode == TRIGGER_MODE_5)
				SetFloatText(m_TriggerParameterEdit, trigger.Parameter, 5); // Bugzilla.1988
			else if (TriggerModeTakesParameter(trigger.Mode))
				SetIntText(m_TriggerParameterEdit, static_cast<int>(trigger.Parameter));
			else
				m_TriggerParameterEdit.SetWindowText(_T(""));
				
		}

	    if (m_TriggerControlledLightingCheck.IsWindowEnabled()) {
            controlledLighting = GetActiveCamera()->GetFeatureValue(FEATURE_TRIGGER_WITH_CONTROLLED_LIGHT,&flags, 0);	
            m_TriggerControlledLightingCheck.SetCheck((controlledLighting == 1.0f) ?  BST_CHECKED : BST_UNCHECKED);
	    }

		SetLabels();

	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: PopulateGpo
* Purpose:  
*/
void CGpioGui::PopulateGpo(void)
{
	if (!GetActiveCamera()->FeatureSupported(FEATURE_GPIO))
		return;

	int gpoNum = ItemData<int>(m_GpoNumberCombo);
    if (gpoNum < 0) return;  //Bugzilla.2099 -- Don't compplain if we can't fill in GPIO information

	try
	{
		BOOL exposureTooShort = FALSE;

        Gpo gpo = GetActiveCamera()->GetGpo(gpoNum);
		
        // We will need to set the mode, but only allow the user to pick modes that this particular GPIO
		// supports.
		std::vector<int> supportedGpioModes = GetActiveCamera()->GetSupportedGpioModes();        
		GpioProfiles gpioProfile = GetActiveCamera()->GetGpioProfile();
		if (gpoNum == 1 && gpioProfile == TWO_GPOS_ONE_GPI) 
		{
			remove(supportedGpioModes, GPIO_MODE_INPUT); // for PL-X, GPIO#1 is NEVER a GPI
        } else if (gpoNum == 2) {
			remove(supportedGpioModes, GPIO_MODE_INPUT); // GPIO#2 is NEVER a GPI
		} else if (gpoNum == 3 && gpioProfile == TWO_GPOS_ONE_GPI) {
			// For PL-X cameras, GPIO#3 is always a GPI
			supportedGpioModes.clear();
			supportedGpioModes.push_back(GPIO_MODE_INPUT);
		}
		SetGpioModeEntries(m_GpoModeCombo, supportedGpioModes);

		if ((gpo.flags & FEATURE_FLAG_OFF) != 0)
		{
			m_GpoOnOffCheck.SetCheck(BST_UNCHECKED);
			// See comment in PopulateTrigger, above.
			if (m_GpoModeCombo.GetCurSel() == -1)
				m_GpoModeCombo.SetCurSel(0);
			if (m_GpoPolarityCombo.GetCurSel() == -1)
				m_GpoPolarityCombo.SetCurSel(0);
		}
		else
		{
			m_GpoOnOffCheck.SetCheck(BST_CHECKED);

			SelectByItemData(m_GpoModeCombo, gpo.Mode);

			SelectByItemData(m_GpoPolarityCombo, gpo.Polarity);

			m_GpoParameter1Edit.SetWindowText(_T(""));
			m_GpoParameter2Edit.SetWindowText(_T(""));
			m_GpoParameter3Edit.SetWindowText(_T(""));

			if (gpo.Mode == GPIO_MODE_STROBE ||
                gpo.Mode == GPIO_MODE_ACTION_STROBE)
			{
				SetFloatText(m_GpoParameter1Edit, gpo.Parameter1, 5);	// Delay (seconds) (Bugzilla.1988)
				SetFloatText(m_GpoParameter2Edit, gpo.Parameter2, 5);	// Duration (seconds) (Bugzilla.1988)
                if (gpo.Mode == GPIO_MODE_STROBE)
                {
                    // As per Bugzilla.1998, see if the expsoure is too short
                    float exp = (GetActiveCamera()->GetFeatureValue(FEATURE_EXPOSURE)) / 1000.0; // be sure exposure is in secondds
                    if (exp < gpo.Parameter1 + gpo.Parameter2) exposureTooShort = TRUE;

                }
			}
			else if (gpo.Mode == GPIO_MODE_PULSE ||
                     gpo.Mode == GPIO_MODE_ACTION_PULSE)
			{
				SetIntText(m_GpoParameter1Edit, static_cast<int>(gpo.Parameter1));		// Number of pulses
				SetFloatText(m_GpoParameter2Edit, gpo.Parameter2, 5);	// Duration (seconds)
				SetFloatText(m_GpoParameter3Edit, gpo.Parameter3, 5);	// Interval (seconds)
			}
			else if (gpo.Mode == GPIO_MODE_INPUT)
			{
				SetIntText(m_GpoParameter1Edit, static_cast<int>(gpo.Parameter1));	// Signal status
			}
		}

		SetLabels();
        m_ExposureWarningStatic.ShowWindow(exposureTooShort ? SW_SHOW : SW_HIDE);

	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}

/**
* Function: PopulateActionCommands
* Purpose:  Add the entries to the "Action Command" combo box.
*/
void CGpioGui::PopulateActionCommands()
{
    if (!GetActiveCamera()->IsActionsSupported())
		return;

	if (m_ActionDelayEdit.GetWindowTextLength() == 0)
	    m_ActionDelayEdit.SetWindowText(_T("0.0"));
}

/**
* Function: FeatureChanged
* Purpose:  This method is called by the base class (CDevAppPage) as a result
*           of a call to the View class's OnUpdate method (which is called by
*           the Document class as a result of a change to its data).
*/
void CGpioGui::FeatureChanged(U32 featureId)
{
	switch (featureId)
	{
	case FEATURE_SPECIAL_CAMERA_MODE:  // Bugzilla.2293
        // The supported feature set may have changed, treat this like it's a 'new' camera
		SetGUIState(GS_CameraSelected);
		break;
	case FEATURE_TRIGGER:
		PopulateTrigger();
		break;
	case FEATURE_GPIO:
		PopulateGpo();
		break;
	default:
		// A feature that is not relevant to this page. Ignore the change.
		break;
	}
}


/**
* Function: UpdateTrigger
* Purpose:  
*/
void CGpioGui::UpdateTrigger(void)
{
	Trigger trigger = {-1, -1, -1, 0.0f, 0.0f};

	trigger.Mode = ItemData<int>(m_TriggerModeCombo);
	trigger.Type = ItemData<int>(m_TriggerTypeCombo);
	trigger.Polarity = ItemData<int>(m_TriggerPolarityCombo);
	trigger.Delay = WindowTextToFloat(m_TriggerDelayEdit);

	if (trigger.Mode == TRIGGER_MODE_5)
	{
		// Parameter is a duration in seconds - allow floats.
		trigger.Parameter = WindowTextToFloat(m_TriggerParameterEdit);
	}
	else if (TriggerModeTakesParameter(trigger.Mode))
	{
		// Parameter is a whole number - force to an integer.
		trigger.Parameter = static_cast<float>(WindowTextToInt(m_TriggerParameterEdit));
	}
	else
	{
		// Parameter is not used by this mode, but we still have to make sure
		// that it is within the valid range, or we will get an error.
		trigger.Parameter = GetActiveCamera()->GetFeatureMinVal(FEATURE_TRIGGER, FEATURE_TRIGGER_PARAM_PARAMETER);
	}

	try
	{
		GetActiveCamera()->SetTrigger(trigger);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: UpdateGpo
* Purpose:  
*/
void CGpioGui::UpdateGpo(void)
{
	PXL_RETURN_CODE rc;

    Gpo  gpo = {0, -1, -1, 0.0f, 0.0f, 0.0f}; // Flags, Mode, Polarity, Param 1/2/3

	gpo.Mode = ItemData<int>(m_GpoModeCombo);
	gpo.Polarity = ItemData<int>(m_GpoPolarityCombo);

	// Not all of the parameters are used in all of the modes, but we still
	// have to make sure that any unused parameters are within the allowed
	// range, or we will get an error.
	gpo.Parameter1 = GetActiveCamera()->GetFeatureMinVal(FEATURE_GPIO, FEATURE_GPIO_PARAM_PARAM_1);
	gpo.Parameter2 = GetActiveCamera()->GetFeatureMinVal(FEATURE_GPIO, FEATURE_GPIO_PARAM_PARAM_2);
	gpo.Parameter3 = GetActiveCamera()->GetFeatureMinVal(FEATURE_GPIO, FEATURE_GPIO_PARAM_PARAM_3);

	if (gpo.Mode == GPIO_MODE_STROBE ||
        gpo.Mode == GPIO_MODE_ACTION_STROBE)
	{
        // Mark paramter 1 as read/write
        m_GpoParameter1Edit.EnableWindow(1);
        // Overwrite the defaults, if the user has not updated the field
        if (m_GpoParameter1Edit.GetWindowTextLength() != 0)
            gpo.Parameter1 = WindowTextToFloat(m_GpoParameter1Edit); // Delay (seconds)
        if (m_GpoParameter2Edit.GetWindowTextLength() != 0)
		    gpo.Parameter2 = WindowTextToFloat(m_GpoParameter2Edit); // On Time (seconds)
	}
	else if (gpo.Mode == GPIO_MODE_PULSE ||
             gpo.Mode == GPIO_MODE_ACTION_PULSE)
	{
        // Mark paramter 1 as read/write
        m_GpoParameter1Edit.EnableWindow(1);
        // Overwrite the defaults, if the user has not updated the field
        if (m_GpoParameter1Edit.GetWindowTextLength() != 0)
    		gpo.Parameter1 = static_cast<float>(WindowTextToInt(m_GpoParameter1Edit)); // Number of pulses
        if (m_GpoParameter2Edit.GetWindowTextLength() != 0)
    		gpo.Parameter2 = WindowTextToFloat(m_GpoParameter2Edit); // On Time of each pulse (seconds)
        if (m_GpoParameter3Edit.GetWindowTextLength() != 0)
    		gpo.Parameter3 = WindowTextToFloat(m_GpoParameter3Edit); // Off time between pulses (seconds)
	}
	else if (gpo.Mode == GPIO_MODE_INPUT)
	{
        // Mark paramter 1 as read only
        m_GpoParameter1Edit.EnableWindow(0);
        if (m_GpoParameter1Edit.GetWindowTextLength() != 0)
    		gpo.Parameter1 = static_cast<float>(WindowTextToInt(m_GpoParameter1Edit)); // Number of pulses
    }
	// else - no parameters are used.


	try
	{
		rc = GetActiveCamera()->SetGpo(gpo, ItemData<int>(m_GpoNumberCombo));
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
    m_ExposureWarningStatic.ShowWindow(rc ==  ApiSuccessParameterWarning ? SW_SHOW : SW_HIDE);
}


/**
* Function: SendAction
* Purpose:  
*/
void CGpioGui::SendAction(void)
{
	int actionCommand = ItemData<int>(m_ActionCommandCombo);
    double actionDelta = 0.0;
    double actionTime = 0.0;  // Assume the user wants immediate action

    try 
    {

        if (m_ActionDelayEdit.GetWindowTextLength() != 0)
    	    	actionDelta = static_cast<double>(WindowTextToFloat(m_ActionDelayEdit));
        if (actionDelta != 0)
        {
            // Only set a non-zero action time if teh user wants to schedule an action
            actionTime = GetActiveCamera()->GetTimestamp();
            actionTime += actionDelta;
        }

        PXL_ERROR_CHECK(PxLSetActions(actionCommand, actionTime));
    }
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/******************************************************************************
* Message handlers
******************************************************************************/


/**
* Function: OnClickTriggerOnOffCheck
* Purpose:  
*/
void CGpioGui::OnClickTriggerOnOffCheck()
{
	if (m_TriggerOnOffCheck.GetCheck() == BST_UNCHECKED)
	{
		// Turn off
		try
		{
			GetActiveCamera()->SetOnOff(FEATURE_TRIGGER, false); // false == "Off"
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
	else
	{
		// Turn on - set to the currently selected values in the controls.
		UpdateTrigger();
	}
}


/**
* Function: OnClickGpoOnOffCheck
* Purpose:  
*/
void CGpioGui::OnClickGpoOnOffCheck()
{
	if (m_GpoOnOffCheck.GetCheck() == BST_UNCHECKED)
	{
		// Turn off
		try
		{
			GetActiveCamera()->SetGpoFlags(FEATURE_FLAG_OFF, ItemData<int>(m_GpoNumberCombo));
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
	else
	{
		// Turn on - set to the currently selected values in the controls.
		UpdateGpo();
	}
}


/**
* Function: OnSelchangeGpoNumberCombo
* Purpose:  
*/
void CGpioGui::OnSelchangeGpoNumberCombo()
{
	PopulateGpo();
}


/**
* Function: OnSelchangeTriggerModeCombo
* Purpose:  
*/
void CGpioGui::OnSelchangeTriggerModeCombo()
{
	SetLabels();
}


/**
* Function: OnSelchangeGpoModeCombo
* Purpose:  
*/
void CGpioGui::OnSelchangeGpoModeCombo()
{
	SetLabels();
}


/**
* Function: SetLabels
* Purpose:  
*/
void CGpioGui::SetLabels(void)
{
	if (m_TriggerModeCombo.GetCurSel() == -1)
	{
		m_TriggerDescription.SetWindowText(_T(""));
		m_TriggerParameterLabel.SetWindowText(_T("Pa&rameter"));
	}
	else
	{
		TriggerModeDescriptor* ptd = GetTriggerDescriptor(ItemData<int>(m_TriggerModeCombo));
		m_TriggerDescription.SetWindowText(ptd->description);
		m_TriggerParameterLabel.SetWindowText(ptd->paramName);
	}

	if (m_GpoModeCombo.GetCurSel() == -1)
	{
		m_GpoDescription.SetWindowText(_T(""));
		m_GpoParameter1Label.SetWindowText(_T("Parameter 1"));
		m_GpoParameter2Label.SetWindowText(_T("Parameter 1"));
		m_GpoParameter3Label.SetWindowText(_T("Parameter 1"));
	}
	else
	{
		m_GpoDescription.SetWindowText(
			GetGpoDescription(m_GpoModeCombo)
		);
		TCHAR const** paramNames = GetGpoParameterNames(ItemData<int>(m_GpoModeCombo));
		m_GpoParameter1Label.SetWindowText(paramNames[0]);
		m_GpoParameter1Units.SetWindowText(paramNames[1]);
		m_GpoParameter2Label.SetWindowText(paramNames[2]);
		m_GpoParameter2Units.SetWindowText(paramNames[3]);
		m_GpoParameter3Label.SetWindowText(paramNames[4]);
		m_GpoParameter3Units.SetWindowText(paramNames[5]);
	}
}


/**
* Function: OnClickUpdateTriggerButton
* Purpose:  
*/
void CGpioGui::OnClickUpdateTriggerButton()
{
	UpdateTrigger();
}


/**
* Function: OnClickUpdateGpoButton
* Purpose:  
*/
void CGpioGui::OnClickUpdateGpoButton()
{
	UpdateGpo();
}

/**
* Function: OnBnClickedGgControlledLightingCheck
* Purpose:  
*/
void CGpioGui::OnBnClickedGgControlledLightingCheck()
{
	float lightingControlValue = 
	        (m_TriggerControlledLightingCheck.GetCheck() == BST_UNCHECKED) ?
	        0.0f : // We are NOT controlling the lighting
	        1.0f ; // We ARE controlling the lighting
	
	try
	{
			GetActiveCamera()->SetFeatureValue(FEATURE_TRIGGER_WITH_CONTROLLED_LIGHT, lightingControlValue, 0); 
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}

/**
* Function: OnClickSendActionButton
* Purpose:  
*/
void CGpioGui::OnClickSendActionButton()
{
	SendAction();
}

/**
* Function: OnClickClearActionsButton
* Purpose:  
*/
void CGpioGui::OnClickClearActionsButton()
{
	m_EventsEdit.SetWindowText(_T(""));
}


/******************************************************************************
* UPDATE_COMMAND_UI handlers
******************************************************************************/

/**
* Function: OnUpdateTriggerParameterEdit
* Purpose:  
*/
void CGpioGui::OnUpdateTriggerParameterEdit(CCmdUI *pCmdUI)
{
	if (m_TriggerModeCombo.GetCurSel() == -1)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	pCmdUI->Enable(TriggerModeTakesParameter(ItemData<int>(m_TriggerModeCombo)));
}


/**
* Function: OnUpdateGpoParameter1Edit
* Purpose:  
*/
void CGpioGui::OnUpdateGpoParameter1Edit(CCmdUI *pCmdUI)
{
	int idx = m_GpoModeCombo.GetCurSel();
	if (m_GpoNumberCombo.GetCurSel() == -1 || idx == -1)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	int gpoMode = static_cast<int>(m_GpoModeCombo.GetItemData(idx));
	pCmdUI->Enable(GpioModeNumParams(gpoMode) >= 1);
}


/**
* Function: OnUpdateGpoParameter2Edit
* Purpose:  
*/
void CGpioGui::OnUpdateGpoParameter2Edit(CCmdUI *pCmdUI)
{
	int idx = m_GpoModeCombo.GetCurSel();
	if (m_GpoNumberCombo.GetCurSel() == -1 || idx == -1)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	int gpoMode = static_cast<int>(m_GpoModeCombo.GetItemData(idx));
	pCmdUI->Enable(GpioModeNumParams(gpoMode) >= 2);
}


/**
* Function: OnUpdateGpoParameter3Edit
* Purpose:  
*/
void CGpioGui::OnUpdateGpoParameter3Edit(CCmdUI *pCmdUI)
{
	int idx = m_GpoModeCombo.GetCurSel();
	if (m_GpoNumberCombo.GetCurSel() == -1 || idx == -1)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	int gpoMode = static_cast<int>(m_GpoModeCombo.GetItemData(idx));
	pCmdUI->Enable(GpioModeNumParams(gpoMode) >= 3);
}


/**
* Function: OnUpdateUpdateTriggerButton
* Purpose:  
*/
void CGpioGui::OnUpdateUpdateTriggerButton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_TriggerOnOffCheck.GetCheck() == BST_CHECKED);
}


/**
* Function: OnUpdateUpdateGpoButton
* Purpose:  
*/
void CGpioGui::OnUpdateUpdateGpoButton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_GpoOnOffCheck.GetCheck() == BST_CHECKED);
}

/**
* Function: EventCallback
* Purpose:  This function recirds all events in the EVENTS_EDIT
*/
static U32 __stdcall EventHandler(
							HANDLE hCamera,
							U32    eventId,
                            double eventTimestamp,
                            U32    numDataBytes,
                            LPVOID pData,
                            LPVOID pContext)
{
	CGpioGui* pDlg = static_cast<CGpioGui*>(pContext);
	if (!::IsWindow(pDlg->GetSafeHwnd()))
		return ApiSuccess;

	if (!pDlg->m_pLock->Lock(0))
	{
		// We were not able to acquire the lock immediately.
		// That's OK - we just won't process this event.
		return ApiSuccess;
	}
	// else:
	// We have acquired a lock on the Mutex. It is safe to access the members,
	// of the CGpioGui object, because the main thread cannot delete it
	// until after we release the lock.
	

	pDlg->m_pLock->Unlock();
    
    // Format a new string for the event
    CString newLine;
    U32    days = static_cast<U32>(eventTimestamp / (60.0*60.0*24.0));
    eventTimestamp -= days * (60.0*60.0*24.0);
    U32    hours = static_cast<U32>(eventTimestamp / (60.0*60.0));
    eventTimestamp -= hours * (60.0*60.0);
    U32    mins = static_cast<U32>(eventTimestamp / 60.0);
    eventTimestamp -= mins * 60.0;
    newLine.Format(_T("%03d:%02d:%02d:%04.2f - %s (%d)\r\n"), days, hours, mins, eventTimestamp, GetEventNames(eventId), eventId);
    
    // Append the event
    int nLength = pDlg->m_EventsEdit.GetWindowTextLength();
    pDlg->m_EventsEdit.SetSel(nLength, nLength);
    pDlg->m_EventsEdit.ReplaceSel(newLine);

    return ApiSuccess;
}

