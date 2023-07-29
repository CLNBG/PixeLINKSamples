//
// LightingGui.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "LightingGui.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/******************************************************************************
 * CRoiGui - Member functions.
 *****************************************************************************/

IMPLEMENT_DYNCREATE(CLightingGui, CDevAppPage)

CLightingGui::CLightingGui(CPxLDevAppView* pView)
	: CDevAppPage(CLightingGui::IDD, pView)
   , m_MyController(0)
   , m_bDontWantController(false)
{
}

CLightingGui::~CLightingGui()
{
}

void CLightingGui::DoDataExchange(CDataExchange* pDX)
{
	CDevAppPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RG_CAMERASELECT_COMBO, m_CameraSelectCombo);
	DDX_Control(pDX, IDC_RG_PLAY_BUTTON, m_PlayButton);
	DDX_Control(pDX, IDC_RG_PAUSE_BUTTON, m_PauseButton);
	DDX_Control(pDX, IDC_RG_STOP_BUTTON, m_StopButton);

   DDX_Control(pDX, IDC_LIGHTING_CONTROLLERSELECT_COMBO, m_ControllerSelectCombo);
   DDX_Control(pDX, IDC_LIGHTING_CANTOPENCOMWARNING_STATIC, m_CantOpenComWarningStatic);

	DDX_SLIDER_SUITE(LIGHTING_BRIGHTNESS,LightingBrightness)

	DDX_Control(pDX, IDC_LIGHTING_BRIGHTNESS_ONOFF_CHECK, m_LightingBrightnessOnOffCheck);

   DDX_Control(pDX, IDC_LIGHTING_CURRENT_EDIT, m_CurrentEdit);
   DDX_Control(pDX, IDC_LIGHTING_VOLTAGE_EDIT, m_VoltageEdit);
   DDX_Control(pDX, IDC_LIGHTING_TEMPERATURE_EDIT, m_TemperatureEdit);
   DDX_Control(pDX, IDC_LIGHTING_UPDATE_BUTTON, m_UpdateButton);
}


BEGIN_MESSAGE_MAP(CLightingGui, CDevAppPage)

	ON_CBN_DROPDOWN(IDC_RG_CAMERASELECT_COMBO, OnDropdownCameraSelectCombo)
	ON_CBN_SELCHANGE(IDC_RG_CAMERASELECT_COMBO, OnSelchangeCameraSelectCombo)
	ON_BN_CLICKED(IDC_RG_PLAY_BUTTON, OnClickPlayButton)
	ON_BN_CLICKED(IDC_RG_PAUSE_BUTTON, OnClickPauseButton)
	ON_BN_CLICKED(IDC_RG_STOP_BUTTON, OnClickStopButton)

   ON_CBN_SELCHANGE(IDC_LIGHTING_CONTROLLERSELECT_COMBO, OnSelchangeControllerSelectCombo)
   ON_CBN_DROPDOWN(IDC_LIGHTING_CONTROLLERSELECT_COMBO, OnDropdownControllerSelectCombo)

   MM_SLIDER_SUITE(LIGHTING_BRIGHTNESS,LightingBrightness)
	ON_BN_CLICKED(IDC_LIGHTING_BRIGHTNESS_ONOFF_CHECK, OnClickLightingBrightnessOnOffCheck)

   ON_BN_CLICKED(IDC_LIGHTING_UPDATE_BUTTON, OnClickUpdateButton)

END_MESSAGE_MAP()


BEGIN_EVENTSINK_MAP(CLightingGui, CDevAppPage)
	EVENTSINK_SLIDER_SUITE(CLightingGui, LIGHTING_BRIGHTNESS, LightingBrightness)
END_EVENTSINK_MAP()


/**
* Function: GetHelpContextId
* Purpose:  
*/
int CLightingGui::GetHelpContextId()
{
	return IDH_LINK_TAB;
}


/**
* Function: OnInitDialog
* Purpose:  
*/
BOOL CLightingGui::OnInitDialog()
{
	CDevAppPage::OnInitDialog();

	m_LightingBrightnessSlider.SetLinkedWindows(&m_LightingBrightnessEdit, &m_LightingBrightnessMinLabel, &m_LightingBrightnessMaxLabel, &m_LightingBrightnessFrame);

	return TRUE;  // return TRUE unless you set the focus to a control
}


/**
* Function: OnSetActive
* Purpose:  
*/
BOOL CLightingGui::OnSetActive()
{
	return CDevAppPage::OnSetActive();
}


/**
* Function: SetGUIState
* Purpose:  Enable / Disable controls, according to the state of the camera.
*           Most controls cannot be used in any meaningful way until a camera
*           is initialized.
*/
void CLightingGui::SetGUIState(eGUIState state)
{
	// The CameraSelect and Play/Pause/Stop button states are set in the base
	// class version of this function.
	CDevAppPage::SetGUIState(state);

	CPxLCamera* cam = GetActiveCamera();

	bool enable = (state != GS_NoCamera);

   m_ControllerSelectCombo.EnableWindow(enable);
   
   bool enableFeature = enable && cam->FeatureSupportsManual(FEATURE_LIGHTING);

	ENABLE_SLIDER_SUITE(LightingBrightness,LIGHTING)
   m_LightingBrightnessOnOffCheck.EnableWindow(enableFeature);
   m_CurrentEdit.EnableWindow(enableFeature);
   m_VoltageEdit.EnableWindow(enableFeature);
   m_TemperatureEdit.EnableWindow(enableFeature);
   m_UpdateButton.EnableWindow(enableFeature);

}


/**
* Function: ConfigureControls
* Purpose:  Set up the controls on the page to customize them according to the
*           currently active camera. This involves things like setting the max
*           and min values for sliders and setting the bounds of the subwindow
*           control.
*/
void CLightingGui::ConfigureControls(void)
{
	SetSliderRange(m_LightingBrightnessSlider, FEATURE_LIGHTING);
}

/**
* Function: PopulateDefaults
* Purpose:  Put some meaningles default values into the controls, for purely
*           aesthetic reasons.
*/
void CLightingGui::PopulateDefaults(void)
{
   m_ControllerSelectCombo.ResetContent();
   m_ControllerSelectCombo.AddString(_T("No Controller Assigned"));
   m_ControllerSelectCombo.SetItemData(0, 0);  // the first item is always no contrller, represented as s/n 0
   m_ControllerSelectCombo.SetCurSel(0);

   m_CantOpenComWarningStatic.ShowWindow(SW_HIDE);

   SetSliderDefault(m_LightingBrightnessSlider);
   m_LightingBrightnessOnOffCheck.SetCheck(BST_UNCHECKED);

   m_MyController = 0;
   m_bDontWantController = false;
}


/**
* Function: PopulateControls
* Purpose:  Make the values displayed in the controls reflect the current 
*           settings of the active camera.  Called immediately after a 
*           camera is initialized.
*/
void CLightingGui::PopulateControls(void)
{
	CDevAppPage::PopulateControls();

   bool haveController = GetActiveCamera()->FeatureSupportedInController(FEATURE_LIGHTING);

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
         for (controller = 0; controller<uMaxNumberOfControllers; controller++) {
            if (tempControllerInfo[controller].CameraSerialNumber == me &&
                tempControllerInfo[controller].TypeMask & CONTROLLER_FLAG_LIGHTING) break;
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
               PXL_ERROR_CHECK(PxLUnassignController(GetActiveCamera()->GetHandle(), m_MyController));
               m_MyController = 0;
            }
         }
      }
   } else {
      // We don't have a controller.  Do we want one?  We dont want one if the camera is already controlling controller type features.
      m_bDontWantController = GetActiveCamera()->FeatureSupported(FEATURE_LIGHTING);
   }

   PopulateControllerSelect();


   // Only populate these controls if they are suported by the camera
   if (GetActiveCamera()->FeatureSupported(FEATURE_LIGHTING))
   {
      PopulateLightingBrightness();
      PopulateLightingBrightnessOnOff();
      PopulateLightingControls();
   }
}

/**
* Function: PopulateControllerSelect
* Purpose:  Set the Controller Selelct dropdown to either 'No Controller Assigned', or to
*           the controller that is assigned to this camera
*/
void CLightingGui::PopulateControllerSelect(void)
{
   //
   // Step 1
   //      Rebuild the camera select with at least the 'No Controller Assigned' option
   m_ControllerSelectCombo.ResetContent();
   m_ControllerSelectCombo.AddString(_T("No Controller Assigned"));
   m_ControllerSelectCombo.SetItemData(0, 0);  // the first item is always no contrller, represented as s/n 0
   m_ControllerSelectCombo.SetCurSel(0);

   //
   // Step 2
   //      If we have a controller, enumerate all of them to get info on the contoller
   int controller = 0;
   U32 me = GetActiveCamera()->GetSerialNumber();
   std::vector<CONTROLLER_INFO> tempControllerInfo;
   U32 uMaxNumberOfControllers;
   PXL_RETURN_CODE rc = PxLGetNumberControllers(NULL, sizeof(CONTROLLER_INFO), &uMaxNumberOfControllers);
   PXL_ERROR_CHECK(rc);
   if (uMaxNumberOfControllers > 0) {
      tempControllerInfo.resize(uMaxNumberOfControllers);
      memset(&tempControllerInfo[0], 0, sizeof(tempControllerInfo[0]) * uMaxNumberOfControllers);
      PXL_ERROR_CHECK(PxLGetNumberControllers(&tempControllerInfo[0], sizeof(tempControllerInfo[0]), &uMaxNumberOfControllers));
   }
   if (m_MyController != 0) {

      // 
      // Step 3
      //      Find our controller
      for (controller = 0; controller<uMaxNumberOfControllers; controller++) {
         if (tempControllerInfo[controller].CameraSerialNumber == me &&
             tempControllerInfo[controller].TypeMask & CONTROLLER_FLAG_LIGHTING) {
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
      ASSERT(controller < uMaxNumberOfControllers);
   }

   m_CantOpenComWarningStatic.ShowWindow(rc == ApiSuccessSerialPortScanSuppressedWarning ? SW_SHOW : SW_HIDE); //Bugzilla.2433

}


/**
* Function: PopulateLightingBrightnessOnOff
* Purpose:  
*/
void CLightingGui::PopulateLightingBrightnessOnOff(void)
{
	if (GetActiveCamera()->FeatureSupported(FEATURE_LIGHTING))
	{
		try
		{
			TempVal<bool> temp(m_bAdjusting[FEATURE_LIGHTING], true);

			bool off = GetActiveCamera()->IsFeatureFlagSet(FEATURE_LIGHTING, FEATURE_FLAG_OFF);
			m_LightingBrightnessOnOffCheck.SetCheck(!off);
			m_LightingBrightnessSlider.EnableWindow(!off);
			m_LightingBrightnessEdit.EnableWindow(!off);
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}

/**
* Function: PopulateLightingControls
* Purpose:
*/
void CLightingGui::PopulateLightingControls(void)
{
   bool enabled = false;
   if (GetActiveCamera()->FeatureSupported(FEATURE_LIGHTING))
   {
      try
      {
         TempVal<bool> temp(m_bAdjusting[FEATURE_LIGHTING], true);

         U32 flags = 0;
         Lighting lighting = GetActiveCamera()->GetLighting(&flags);

         enabled = (flags & FEATURE_FLAG_OFF) == 0;
         if (enabled)
         {
            SetFloatText(m_CurrentEdit, lighting.Current); 
            SetFloatText(m_VoltageEdit, lighting.Voltage);
            SetFloatText(m_TemperatureEdit, lighting.Temperature);
         }
      }
      catch (PxLException const& e)
      {
         OnApiError(e);
      }
   }
   m_CurrentEdit.EnableWindow(enabled);
   m_VoltageEdit.EnableWindow(enabled);
   m_TemperatureEdit.EnableWindow(enabled);
   m_UpdateButton.EnableWindow(enabled);
}

/**
* Function: FeatureChanged
* Purpose:  This method is called by the base class (CDevAppPage) as a result
*           of a call to the View class's OnUpdate method (which is called by
*           the Document class as a result of a change to its data).
*/
void CLightingGui::FeatureChanged(U32 featureId)
{
	switch (featureId)
	{
	case FEATURE_LIGHTING:
		PopulateLightingBrightness();
		PopulateLightingBrightnessOnOff();
      PopulateLightingControls();
		break;
	default:
		// A feature that is not relevant to this page. Ignore the change.
		break;
	}
}


/******************************************************************************
 * CLightingGui message handlers
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

SLIDER_CONTROL_HANDLER_IMPL(CLightingGui,LightingBrightness,LIGHTING)

/**
* Function: OnDropdownControllerSelectCombo
* Purpose:
*/
void CLightingGui::OnDropdownControllerSelectCombo()
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
      selectedItem.Format(_T("No Controller Assigned"));
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
         for (U32 i = 0; i<uMaxNumberOfControllers; i++) {
            // Skip any controllers that cannot access, be it because they are open by another application, or are 
            // are already assigned to another camera
            if (tempControllerInfo[i].ControllerSerialNumber == 0 || // Another App has it open 
               (tempControllerInfo[i].CameraSerialNumber != 0 &&
                tempControllerInfo[i].CameraSerialNumber != me) ||   // someone other than me already has it
                !(tempControllerInfo[i].TypeMask & CONTROLLER_FLAG_LIGHTING)) continue; // not a lighting controller
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
      //      'Highlight' the current controller
      m_ControllerSelectCombo.SetCurSel(m_ControllerSelectCombo.FindString(0, selectedItem));

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
void CLightingGui::OnSelchangeControllerSelectCombo(void)
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
            PXL_ERROR_CHECK(PxLUnassignController(GetActiveCamera()->GetHandle(), m_MyController));
            m_MyController = 0;
            guiUpdateRequired = true;
         }
      } else {
         guiUpdateRequired = m_MyController != selectedController;
         // If we already have a differnt controller, release it.
         if (m_MyController != 0 && m_MyController != selectedController) {
            PXL_ERROR_CHECK(PxLUnassignController(GetActiveCamera()->GetHandle(), m_MyController));
            m_MyController = 0;
         }

         // Grab the selected controller if we don't already have it
         if (m_MyController != selectedController) {
            PXL_ERROR_CHECK(PxLAssignController(GetActiveCamera()->GetHandle(), selectedController));
            m_MyController = selectedController;
         }
      }

      if (guiUpdateRequired) {
         // We need to update the GUI so that the lighting controls will be 
         // updated.  

         // Our controller changed -- reload the camera features as they may have changed.  RE-read the lighting
         // camerea limit, given this feature is 'dynamic', that will flush the cache of the feature, and re-read it.
         float tempMin;
         tempMin = GetActiveCamera()->GetFeatureMinVal(FEATURE_LIGHTING);

         SetGUIState(GetDocument()->GetGUIState());

         PopulateLightingBrightness();
         PopulateLightingControls();
      }
   }
   catch (PxLException const& e)
   {
      OnApiError(e);
   }
}

/**
* Function: OnClickLightingBrightnessOnOffCheck
* Purpose:  
*/
void CLightingGui::OnClickLightingBrightnessOnOffCheck(void)
{
	CWaitCursor wc;
	if (!m_bAdjusting[FEATURE_LIGHTING])
	{
		try
		{
			GetActiveCamera()->SetOnOff(FEATURE_LIGHTING, m_LightingBrightnessOnOffCheck.GetCheck() == BST_CHECKED);
         PopulateLightingControls();
      }
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


/**
* Function: OnClickUpdateButton
* Purpose:
*/
void CLightingGui::OnClickUpdateButton(void)
{
   CWaitCursor wc;
   if (!m_bAdjusting[FEATURE_LIGHTING])
   {
      try
      {
         PopulateLightingControls();
      }
      catch (PxLException const& e)
      {
         OnApiError(e);
      }
   }
}







