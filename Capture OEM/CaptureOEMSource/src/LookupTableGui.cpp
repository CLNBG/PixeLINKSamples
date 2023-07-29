// LookupTableGui.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "LookupTableGui.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
CReadOnlyColorEdit::CReadOnlyColorEdit()
{
	m_defaultBackgroundColor = GetSysColor(COLOR_BTNFACE);
	m_backgroundColor = m_defaultBackgroundColor;
}

CReadOnlyColorEdit::~CReadOnlyColorEdit()
{
	if (m_brushBackground.GetSafeHandle()) {
       m_brushBackground.DeleteObject();
	}
}


BEGIN_MESSAGE_MAP(CReadOnlyColorEdit, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

HBRUSH 
CReadOnlyColorEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	pDC->SetBkColor(m_backgroundColor);

	return m_brushBackground;
}

void 
CReadOnlyColorEdit::SetBackgroundColor(COLORREF newBackgroundColor)
{
	m_backgroundColor = newBackgroundColor;
	
	if (m_brushBackground.GetSafeHandle()) {
       m_brushBackground.DeleteObject();
	}
	m_brushBackground.CreateSolidBrush(newBackgroundColor);
	
	Invalidate(TRUE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////








/////////////////////////////////////////////////////////////////////////////
// CLookupTableGui property page

IMPLEMENT_DYNCREATE(CLookupTableGui, CDevAppPage)


CLookupTableGui::CLookupTableGui(CPxLDevAppView* pView) 
	: CDevAppPage(CLookupTableGui::IDD, pView)
	, m_bDialogCreated(false)
	, m_bAutoUpdate(true)
	, m_lookupTableSize(256)
	, m_timerId(0)
{
}


CLookupTableGui::~CLookupTableGui()
{
}


void CLookupTableGui::DoDataExchange(CDataExchange* pDX)
{
	CDevAppPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LT_CAMERASELECT_COMBO, m_CameraSelectCombo);
	DDX_Control(pDX, IDC_LT_PLAY_BUTTON, m_PlayButton);
	DDX_Control(pDX, IDC_LT_PAUSE_BUTTON, m_PauseButton);
	DDX_Control(pDX, IDC_LT_STOP_BUTTON, m_StopButton);

	DDX_Control(pDX, IDC_LT_ENABLELOOKUP_CHECK, m_EnableLookupCheck);
	DDX_Control(pDX, IDC_LT_AUTOUPDATE_CHECK, m_AutoUpdateCheck);
	DDX_Control(pDX, IDC_LT_UPDATECAMERA_BUTTON, m_UpdateCameraButton);
	DDX_Control(pDX, IDC_LT_LOOKUPTABLE_LIST, m_LookupTableList);
	DDX_Control(pDX, IDC_LT_LOOKUP_GRAPH, m_Graph);
	DDX_Control(pDX, IDC_LT_FFCGAIN_EDIT, m_FfcGainEdit);
	DDX_Control(pDX, IDC_LT_ENABLEFFC_CHECK, m_FfcEnableCheck);

	DDX_Control(pDX, IDC_LT_SENSOR_TEMPERATURE_EDIT, m_sensorTemperatureEdit);
	DDX_Control(pDX, IDC_LT_BODY_TEMPERATURE_EDIT, m_bodyTemperatureEdit);
}


BEGIN_MESSAGE_MAP(CLookupTableGui, CDevAppPage)

	ON_CBN_DROPDOWN(IDC_LT_CAMERASELECT_COMBO, OnDropdownCameraSelectCombo)
	ON_CBN_SELCHANGE(IDC_LT_CAMERASELECT_COMBO, OnSelchangeCameraSelectCombo)
	ON_BN_CLICKED(IDC_LT_PLAY_BUTTON, OnClickPlayButton)
	ON_BN_CLICKED(IDC_LT_PAUSE_BUTTON, OnClickPauseButton)
	ON_BN_CLICKED(IDC_LT_STOP_BUTTON, OnClickStopButton)

	ON_BN_CLICKED(IDC_LT_ENABLELOOKUP_CHECK, OnClickEnableLookupCheck)
	ON_BN_CLICKED(IDC_LT_AUTOUPDATE_CHECK, OnClickAutoUpdateCheck)
	ON_BN_CLICKED(IDC_LT_UPDATECAMERA_BUTTON, OnClickUpdateCameraButton)
	ON_BN_CLICKED(IDC_LT_ENABLEFFC_CHECK, OnClickEnableFfcCheck)

	ON_UPDATE_COMMAND_UI(IDC_LT_AUTOUPDATE_CHECK, OnUpdateAutoUpdateCheck)
	ON_UPDATE_COMMAND_UI(IDC_LT_UPDATECAMERA_BUTTON, OnUpdateUpdateCameraButton)

	ON_WM_TIMER()

END_MESSAGE_MAP()


BEGIN_EVENTSINK_MAP(CLookupTableGui, CDevAppPage)
	ON_EVENT(CLookupTableGui, IDC_LT_LOOKUP_GRAPH, 2, OnGraphChanged, VTS_NONE)
END_EVENTSINK_MAP()


/**
* Function: OnInitDialog
* Purpose:  
*/
BOOL CLookupTableGui::OnInitDialog()
{
	CDevAppPage::OnInitDialog();

	// I use this bool to ensure that we don't try to call a method on the ActiveX
	// control until after it is fully created - otherwise we get a crash.
	m_bDialogCreated = true;

	// We just do this so that the listbox is never empty - we want it to
	// reflect what's shown in the graph (which is the identity transform
	// at the moment).
	m_table.resize(m_lookupTableSize);
	m_Graph.FillTable(m_table.size(), &m_table[0]);
	PopulateListBox(m_table);

	return TRUE;
}


/**
* Function: OnSetActive
* Purpose:  
*/
BOOL CLookupTableGui::OnSetActive()
{
	if (CDevAppPage::OnSetActive())
	{
		// The value of the temperature feature can change continuously, without
		// user intervention, unlike other features. Set up a timer to update the
		// temperature display at regular intervals.
		StartUpdateTimer();

		return TRUE;
	}
	return FALSE;
}


/**
* Function: OnKillActive
* Purpose:  
*/
BOOL CLookupTableGui::OnKillActive()
{
	StopUpdateTimer();

	return CDevAppPage::OnKillActive();
}


/**
* Function: GetHelpContextId
* Purpose:  
*/
int CLookupTableGui::GetHelpContextId()
{
	return IDH_LOOKUPTABLES_TAB;
}


/**
* Function: SetGUIState
* Purpose:  Enable / Disable controls, according to the state of the camera.
*           Most controls cannot be used in any meaningful way until a camera
*           is initialized.
*/
void CLookupTableGui::SetGUIState(eGUIState state)
{
	// The CameraSelect and Play/Pause/Stop button states are set in the base
	// class version of this function.
	CDevAppPage::SetGUIState(state);

	CPxLCamera* cam = GetActiveCamera();

	bool enable = (state != GS_NoCamera);

	bool enableLookup = enable && cam->FeatureSupportsManual(FEATURE_LOOKUP_TABLE);
	m_EnableLookupCheck.EnableWindow(enableLookup);
	m_AutoUpdateCheck.EnableWindow(enableLookup);
	m_UpdateCameraButton.EnableWindow(enableLookup);
	m_Graph.put_Enabled(enableLookup && !cam->IsFeatureFlagSet(FEATURE_LOOKUP_TABLE, FEATURE_FLAG_OFF));

	if (GS_NoCamera == state) { 
		m_FfcEnableCheck.EnableWindow(false);
	} else {
		const bool manualFfcControl = cam->SupportsManualFFC();
		m_FfcEnableCheck.EnableWindow(manualFfcControl);
	}
}


/**
* Function: ConfigureControls
* Purpose:  Set up the controls on the page to customize them according to the
*           currently active camera. This involves things like setting the max
*           and min values for sliders and setting the bounds of the subwindow
*           control.
*/
void CLookupTableGui::ConfigureControls(void)
{
	m_lookupTableSize = static_cast<long>(GetActiveCamera()->GetFeatureMaxVal(FEATURE_LOOKUP_TABLE));
	m_table.resize(m_lookupTableSize, 0);
}


/**
* Function: PopulateDefaults
* Purpose:  Put some meaningles default values into the controls, for purely
*           aesthetic reasons.
*/
void CLookupTableGui::PopulateDefaults(void)
{
	m_LookupTableList.SetTopIndex(0);

	m_FfcEnableCheck.SetCheck(BST_UNCHECKED);
	m_FfcGainEdit.SetWindowText(_T(""));

	m_sensorTemperatureEdit.Clear();
	m_bodyTemperatureEdit.Clear();
}


/**
* Function: PopulateControls
* Purpose:  Make the values displayed in the controls reflect the current 
*           settings of the active camera.  Called immediately after a 
*           camera is initialized.
*/
void CLookupTableGui::PopulateControls(void)
{
	CDevAppPage::PopulateControls();

	PopulateGraph();

	PopulateListBox(m_table);

	PopulateFFC();

	PopulateTemperatures();
}


/**
* Function: PopulateListBox
* Purpose:  Fill the list box with the mappings of values defined by the graph
*           control.
*/
void CLookupTableGui::PopulateListBox(std::vector<long> const& values)
{
	if (!::IsWindow(m_LookupTableList.m_hWnd))
		return;

	int scrollPos = m_LookupTableList.GetTopIndex();

	CString item;;
	m_LookupTableList.ResetContent();
	m_LookupTableList.InitStorage(values.size(), 11 * sizeof(TCHAR)); // Preallocate memory for a bunch of 11-character strings.
	for ( int i = 0 ; i < values.size() ; i++ )
	{
		item.Format(_T("%4d\t%ld"), i, values[i]);
		m_LookupTableList.AddString(item);
	}

	m_LookupTableList.SetTopIndex(scrollPos);
}


/**
* Function: PopulateGraph
* Purpose:  Set the graph control to reflect the current value of the
*           LOOKUP_TABLE feature in the active camera.
*/
void CLookupTableGui::PopulateGraph(void)
{
	if (!GetActiveCamera()->FeatureSupported(FEATURE_LOOKUP_TABLE))
		return;
	// Get Lookup Table values and flags from the camera:
	U32 flags = 0;
	std::vector<float> table(GetActiveCamera()->GetLookupTable(&flags));

	// Set the "Enable Lookup Table" check box, which reflects the state of
	// FEATURE_FLAG_OFF for FEATURE_LOOKUP_TABLE (unchecked == Off), and
	// enable or disable the lookup control accordingly.
	if ((flags & FEATURE_FLAG_OFF) != 0)
	{
		m_EnableLookupCheck.SetCheck(BST_UNCHECKED);
		m_Graph.put_Enabled(FALSE);
	}
	else
	{
		m_EnableLookupCheck.SetCheck(BST_CHECKED);
		m_Graph.put_Enabled(TRUE);
	}
	
	// Bugzilla.565.  we have alrady done the expensive operation of reading the LUT from the camera.  Invalidate the
	// graphs cached values, just to ensure the graph matches what we just read from the camera.
	GetActiveCamera()->InvalidateLookupGraphCache();

	// Re-Draw the graph control only when neccesary:
	if (!GetActiveCamera()->PopulateGraph(m_Graph))
	{
		// Vertices are not yet cached in camera object - read from the
		// actual camera instead.
		// Convert table values to long to pass to the ActiveX control:
		//std::vector<long> lngTable(table.begin(), table.end()); // vc6 won't accept this
		m_table.clear();
		m_table.resize(table.size());
		for (int i = 0; i < table.size(); i++)
		{
			m_table[i] = static_cast<long>(table[i]);
		}
		// Ask the ActiveX control try to determine the minimal set of line
		// segments that represents the camera's lookup table with reasonable
		// precision.
		m_Graph.SetFromTable(m_table.size(), &m_table[0]);
		
		m_Graph.Invalidate(); // Repaint
	}

	return;
}


/**
* Function: PopulateFFC
* Purpose:  
*/
void CLookupTableGui::PopulateFFC(void)
{
	CPxLCamera* cam = GetActiveCamera();

	try
	{
		if (cam->SupportsManualFFC())
		{
			m_FfcEnableCheck.SetCheck(cam->IsFfcOn());
			SetFloatText(m_FfcGainEdit, cam->GetFfcCalibratedGain(), 1);
		}
		else
		{
			m_FfcEnableCheck.SetCheck(BST_UNCHECKED);
			m_FfcGainEdit.SetWindowText(_T(""));
		}
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: PopulateTemperatures
* Purpose:  Display the values of temperatures supported by the camera.
*/
void 
CLookupTableGui::PopulateTemperatures(void)
{
	PopulateTemperature(FEATURE_SENSOR_TEMPERATURE, m_sensorTemperatureEdit, 45.0f, 50.0f);
	PopulateTemperature(FEATURE_BODY_TEMPERATURE,	m_bodyTemperatureEdit, 62.0f, 70.0f);
}

void 
CLookupTableGui::PopulateTemperature(const U32 temperatureFeatureId, 
                                     CReadOnlyColorEdit& control, 
                                     const float warmThreshold,  // Degrees C; above this are shown yellow
                                     const float hotThreshold )  // Degrees C; above this are shown red.
{
	try
	{
		if (GetActiveCamera()->FeatureSupported(temperatureFeatureId))
		{
			// The temperature is a Signed 8.1 bit value
			// e.g. 23.5 => 23.5 degrees Celsius
			//     247.0 => -9.0 degrees Celsius
			//     247.5 => -8.5 degrees Celsius
			float temperature = GetActiveCamera()->GetFeatureValue(temperatureFeatureId);
			if (temperature >= 128.0f) {
				temperature -= 256.0f;
			}

			CString s;
			s.Format(_T("%2.1f"), temperature);
			control.SetWindowText(s);

			// Report the temperature, but provide a bit of colour information about the nature of the temperature
			COLORREF backgroundColor = control.GetDefaultBackgroundColor();
			if (temperature >= hotThreshold) {
				backgroundColor = RGB(0xFF,0x00,0x00);	// Red
			} else if (temperature >= warmThreshold) {
				backgroundColor = RGB(0xFF,0xFC,0x17);	// Warning yellow
			}
			control.SetBackgroundColor(backgroundColor);
			control.EnableWindow(TRUE);
		} else {
			COLORREF backgroundColor = control.GetDefaultBackgroundColor();
			control.SetBackgroundColor(backgroundColor);
			control.SetWindowText("Unsupported");
			control.EnableWindow(FALSE);
		}
	}
	catch (PxLException const& e)
	{
        OnApiError(e);
	}
}


/**
* Function: FeatureChanged
* Purpose:  This method is called by the base class (CDevAppPage) as a result
*           of a call to the View class's OnUpdate method (which is called by
*           the Document class as a result of a change to its data).
*/
void CLookupTableGui::FeatureChanged(U32 featureId)
{
	switch (featureId)
	{
	case FEATURE_LOOKUP_TABLE:
		PopulateGraph();
		break;
	case FEATURE_GAMMA:
		PopulateGraph();
		break;
	case FEATURE_BODY_TEMPERATURE:
	case FEATURE_SENSOR_TEMPERATURE:
		PopulateTemperatures();
		break;
	default:
		// A feature that is not relevant to this page. Ignore the change.
		break;
	}
}


/**
* Function: UpdateLookupTable
* Purpose:  Put the values defined by the graph into the camera's lookup table.
*/
void CLookupTableGui::UpdateLookupTable(U32 flags /*=0*/)
{
	CWaitCursor wc;

	// Copy the values from the vector of longs (which we get from the Lookup
	// table ActiveX control) into the vector of floats (which we pass to the
	// API).
	m_floatTable.resize(m_table.size(), 0.0);
	//cast_copy(m_table.begin(), m_table.end(), m_floatTable.begin()); // vc7
	for (int i = 0; i < m_table.size(); i++)
	{
		m_floatTable[i] = static_cast<float>(m_table[i]);
	}

	try
	{
		// Store the points of the Lookup table graph, so that if we switch
		// between two or more cameras, we can display the correct lookup
		// table for the active camera.
		long nPts = m_Graph.GetPointCount();
		std::vector<long> pts(nPts * 2);
		std::vector<BYTE> types(nPts);
		m_Graph.GetPoints(&pts[0], &types[0], nPts);
		GetActiveCamera()->CacheLookupGraphPoints(pts, types);

		// Send the values to the camera.
		GetActiveCamera()->SetLookupTable(m_floatTable, flags);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/******************************************************************************
* Message Handlers
******************************************************************************/

/**
* Function: OnGraphChanged
* Purpose:  Event handler for the GraphChanged event of the PxLLookup control.
*           This event is fired at the *end* of a drag operation - when you
*           release the mouse button.
*           We use this event to update the list box, and, if the AutoUpdate
*           box is checked, to update the LOOKUP_TABLE feature of the camera.
*/
void CLookupTableGui::OnGraphChanged()
{
	// I was getting an error due to this event being fired before the control
	// was fully created (i.e. while hWnd == NULL). This is my work-around.
	if (!m_bDialogCreated)
		return;

	m_Graph.FillTable(m_table.size(), &m_table[0]);
	PopulateListBox(m_table);

	if (m_bAutoUpdate)
		UpdateLookupTable();
}


/**
* Function: OnClickEnableLookupCheck
* Purpose:  
*/
void CLookupTableGui::OnClickEnableLookupCheck(void)
{
	if (m_EnableLookupCheck.GetCheck() == BST_UNCHECKED)
	{
		// Turn off FEATURE_LOOKUP_TABLE.
		UpdateLookupTable(FEATURE_FLAG_OFF);
	}
	else
	{
		// Turn on (set to the values determined by the graph).
		UpdateLookupTable();
	}
}


/**
* Function: OnClickAutoUpdateCheck
* Purpose:  
*/
void CLookupTableGui::OnClickAutoUpdateCheck()
{
	m_bAutoUpdate = !m_bAutoUpdate;
	if (m_bAutoUpdate)
		UpdateLookupTable();
}


/**
* Function: OnClickUpdateCameraButton
* Purpose:  
*/
void CLookupTableGui::OnClickUpdateCameraButton()
{
	UpdateLookupTable();
}


//
// This is called after the state of the control has already changed.
// 
void CLookupTableGui::OnClickEnableFfcCheck(void)
{
	const bool enable = m_FfcEnableCheck.GetCheck() == BST_CHECKED;
	try {
		GetActiveCamera()->EnableFfc(enable);
	} catch (PxLException const& e) {
		// Something wrong.. but what? First, restore the control to its old value.
		m_FfcEnableCheck.SetCheck(!enable);

		// Can we still communicate with the camera?
		try {
			GetActiveCamera()->GetFeatureValue(FEATURE_EXPOSURE, NULL);
		} catch(const PxLException&) {
			// Something wrong with the camera.
			// Report the original error.
			OnApiError(e);
			return;
		}

		// Probably due to the camera disabling FFC when pixel addressing is > 1
		// Mono 950 cameras do this.
		PixelAddressing pa = GetActiveCamera()->GetPixelAddressing();
		if (pa.x > 1 || pa.y > 1) {
			this->MessageBox("FFC has been disabled by the camera because, on this camera, it is\nnot supported when the pixel addressing value is greater than 1.");
		}
	}
}


/**
* Function: OnUpdateAutoUpdateCheck
* Purpose:  
*/
void CLookupTableGui::OnUpdateAutoUpdateCheck(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bAutoUpdate ? BST_CHECKED : BST_UNCHECKED);
	pCmdUI->Enable(m_EnableLookupCheck.GetCheck() == BST_CHECKED);
}


/**
* Function: OnUpdateUpdateCameraButton
* Purpose:  
*/
void CLookupTableGui::OnUpdateUpdateCameraButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_EnableLookupCheck.GetCheck() == BST_CHECKED && !m_bAutoUpdate);
}


/**
* Function: StartUpdateTimer
* Purpose:  
*/
void CLookupTableGui::StartUpdateTimer(void)
{
	m_timerId = SetTimer(TEMPERATURE_TIMER_ID, 5000, NULL);
}


/**
* Function: StopUpdateTimer
* Purpose:  
*/
void CLookupTableGui::StopUpdateTimer(void)
{
	if (m_timerId != 0)
		KillTimer(m_timerId);
	m_timerId = 0;
}


/**
* Function: OnTimer
* Purpose:  
*/
void CLookupTableGui::OnTimer(UINT_PTR nIDEvent)
{
	if (TEMPERATURE_TIMER_ID == nIDEvent)
	{
		PopulateTemperatures();
	}

	CDevAppPage::OnTimer(nIDEvent);
}












