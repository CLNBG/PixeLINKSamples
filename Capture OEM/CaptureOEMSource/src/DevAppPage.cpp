// DevAppPage.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "PxLDevAppView.h"
#include "DevAppPage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CDevAppPage, CPropertyPage)


/**
* Function: CDevAppPage
* Purpose:  Constructor
*/
CDevAppPage::CDevAppPage(UINT nID, CPxLDevAppView* pView)
	: CPropertyPage(nID)
	, m_pView(pView)
	, m_bEnabled(false)
	, m_lastUsedSerialNumber(0)
{
}


/**
* Function: ~CDevAppPage
* Purpose:  Destructor
*/
CDevAppPage::~CDevAppPage()
{
}


/**
* Function: DoDataExchange
* Purpose:  
*/
void CDevAppPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDevAppPage, CPropertyPage)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()


/**
* Function: OnInitDialog
* Purpose:  
*/
BOOL CDevAppPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// Note: MFC does the following for us, kind of by accident:
	//       CView::OnInitialUpdate calls OnUpdate (virual) with a hard-coded
	//       0 as the lHint parameter - and luckily 0 == GS_NoCamera. :-)
	if (GS_NoCamera != 0)
		this->SetGUIState(GS_NoCamera); // Playing it safe. This will be optimized away anyway.

	return TRUE;
}


/**
* Function: OnSetActive
* Purpose:  Make sure the Camera Select combo is showing the active camera.
*/
BOOL CDevAppPage::OnSetActive()
{
	CWaitCursor wc;

    try
    {
	    if (this->m_bEnabled && CPropertyPage::OnSetActive())
	    {
		    // Network camers do not have the benefit of receiving WM_APP_DEFERRED_DEVICE_CHANGED messags
            // when a camera is disconnected.  So, in order to improve the look and feel when working
            // with network cameras, each time the tab is changed, we will ping the camera to ensure that
            // it is still there.
            try
            {
                GetDocument()->PingCamera();
            }
			catch (PxLException const& e)
			{
			    OnApiError(e);
            }
            // ----
            // Set the Camera Select combo to show the correct entry.
		    PopulateCameraSelect();

		    // Enable / Disable controls according to the current state of the
		    // camera and the preview window.
		    eGUIState gs = GetDocument()->GetGUIState();
		    SetGUIState(gs);

		    // Configure the GUI.
		    if (gs == GS_NoCamera)
		    {
			    PopulateDefaults();
		    }
		    else
		    {
			    if (m_lastUsedSerialNumber != GetActiveCamera()->GetSerialNumber())
			    {
				    //The camera has been changed since the last time the page was activated.
				    ConfigureControls();
			    }
			    // Populate (or Re-Populate) the controls. Some relevant camera 
			    // settings may have been changed since this page was last activated.
			    PopulateControls();
		    }

		    // Keep track of which camera this page is currently configured for.
		    m_lastUsedSerialNumber = GetActiveCamera()->GetSerialNumber();

		    return TRUE;
	    }
    } catch (PxLException const&) {
		TRACE("Caught an exception while trying to set Active Camera\n");
    }
    return FALSE;
}


/**
* Function: OnHelpInfo
* Purpose:  Method to implement tab-specific help.
*/
BOOL 
CDevAppPage::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	return DevAppHelp(pHelpInfo, this);
}


/**
* Function: GetHelpContextId
* Purpose:  
*/
int CDevAppPage::GetHelpContextId()
{
	// Returns 0, indicating no particular help context.
	// Derived classes should override this to return the context ID
	// of their own help page.
	return 0;
}


/**
* Function: OnApiError
* Purpose:  General purpose error handler for API errors.
*/
void CDevAppPage::OnApiError(PxLException const& e, bool cameraRelated)
{
	GetDocument()->OnApiError(e, cameraRelated);
}


/**
* Function: GetEnabled
* Purpose:  
*/
bool CDevAppPage::GetEnabled(void)
{
	return m_bEnabled;
}


/**
* Function: SetEnabled
* Purpose:  
*/
void CDevAppPage::SetEnabled(bool bEnabled)
{
	m_bEnabled = bEnabled;
}


/**
* Function: GetDocument
* Purpose:  
*/
CPxLCameraDoc* CDevAppPage::GetDocument(void)
{
	return m_pView->GetDocument();
}


/**
* Function: GetActiveCamera
* Purpose:  
*/
CPxLCamera* CDevAppPage::GetActiveCamera(void)
{
	return GetDocument()->GetActiveCamera();
}


// Helper function:
void EnableIfWnd(CWnd& pwnd, bool enable)
{
	if (::IsWindow(pwnd.GetSafeHwnd()))
	{
		pwnd.EnableWindow(enable);
	}
}


/**
* Function: EnableCameraSelect
* Purpose:  
*/
void CDevAppPage::EnableCameraSelect(bool enable)
{
	EnableIfWnd(m_CameraSelectCombo, enable);
}


/**
* Function: PopulateCameraSelect
* Purpose:  
*/
void CDevAppPage::PopulateCameraSelect(void)
{
	if (!SelectByItemData(m_CameraSelectCombo, GetActiveCamera()->GetSerialNumber()))
	{
		// Couldn't set the combo box to the right serial number - maybe it's
		// not in the list. Update the entries and try again.
		GetDocument()->UpdateCameraList(m_CameraSelectCombo);
		if (!SelectByItemData(m_CameraSelectCombo, GetActiveCamera()->GetSerialNumber()))
		{
			// Still couldn't select the right entry - select the first one.
			m_CameraSelectCombo.SetCurSel(-1); // Force an OnSelchange event...
			m_CameraSelectCombo.SetCurSel(0);
		}
	}
}


/**
* Function: EnablePreviewButtons
* Purpose:  
*/
void CDevAppPage::EnablePreviewButtons(bool enablePlay, bool enablePause, bool enableStop)
{
	EnableIfWnd(m_PlayButton, enablePlay);
	EnableIfWnd(m_PauseButton, enablePause);
	EnableIfWnd(m_StopButton, enableStop);
}


/**
* Function: SetSliderDefault
* Purpose:  
*/
void CDevAppPage::SetSliderDefault(CSlider& slider, float maxVal /*=100.0f*/)
{
	slider.SetRange(0, maxVal);
	slider.SetValue(0);
}


/**
* Function: PopulateSlider
* Purpose:  
*/
void CDevAppPage::PopulateSlider(CSlider& slider, U32 featureId)
{
	if (GetActiveCamera()->FeatureSupported(featureId))
	{
		try
		{
			TempVal<bool> temp(m_bAdjusting[featureId], true);

			if (GetActiveCamera()->IsDynamicFeature(featureId))
			{
				// Reset the range of the slider - the max and min may have changed
				SetSliderRange(slider, featureId);
			}

			float val = GetActiveCamera()->GetFeatureValue(featureId);
			slider.SetValue(val);
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


/**
* Function: SliderScrolled
* Purpose:  
*/
void CDevAppPage::SliderScrolled(CSlider& slider, U32 featureId, int paramNo /*=0*/)
{
	if (!m_bAdjusting[featureId])
	{
		slider.OnScroll();
		try
		{
			GetActiveCamera()->SetFeatureValue(featureId, slider.GetValue(), paramNo);
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


/**
* Function: SliderChanged
* Purpose:  
*/
void CDevAppPage::SliderChanged(CSlider& slider, U32 featureId, int paramNo /*=0*/)
{
	if (!m_bAdjusting[featureId])
	{
		slider.OnChange();
		try
		{
			GetActiveCamera()->SetFeatureValue(featureId, slider.GetValue(), paramNo);
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


/**
* Function: SliderEditFocusKilled
* Purpose:  
*/
void CDevAppPage::SliderEditFocusKilled(CSlider& slider, U32 featureId, int paramNo /*=0*/)
{
	{
		TempVal<bool> temp(m_bAdjusting[featureId], true);
		slider.UpdateFromWnd();
	}
	try
	{
		GetActiveCamera()->SetFeatureValue(featureId, slider.GetValue(), paramNo);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: SetGUIState
* Purpose:  
*/
void CDevAppPage::SetGUIState(eGUIState state)
{
	switch (state)
	{
	case GS_PreviewPlaying:
		EnableCameraSelect(true);
		EnablePreviewButtons(false, true, true);
		break;
	case GS_PreviewPaused:
		EnableCameraSelect(true);
		EnablePreviewButtons(true, false, true);
		break;
	case GS_CameraSelected:
		PopulateCameraSelect();
		EnableCameraSelect(true);
		EnablePreviewButtons(true, false, false);
		break;
	case GS_NoCamera:
		PopulateCameraSelect();
		EnableCameraSelect(true);
		EnablePreviewButtons(false, false, false);
		break;
	default:
		TRACE("Missing a case label in CDevAppPage::SetGUIState\n");
		break;
	}
}


/**
* Function: PopulateDefaults
* Purpose:  
*/
void CDevAppPage::PopulateDefaults()
{
	// Override this to populate controls with the values you want
	// to see displayed when no camera is selected. eg: cleare edits, etc.
	TRACE("CDevAppPage::PopulateDefaults called. You very probably need to override this function.\n");
}


/**
* Function: ApplyUserSettings
* Purpose:  Take whatever data is needed for this page from the UserSettings
*           object in order to populate the GUI.
*           This base class version does nothing - some pages have no settings
*           to load, so they don't need to override this function.
*/
void CDevAppPage::ApplyUserSettings(UserSettings& settings)
{
}


/**
* Function: StoreUserSettings
* Purpose:  Put whatever data is used by this page into the UserSettings
*           object, so that it can be saved in the registry.
*           This base class version does nothing - some pages have no settings
*           to store, so they don't need to override this function.
*/
void CDevAppPage::StoreUserSettings(UserSettings& settings)
{
}


/**
* Function: GetConfigFileData
* Purpose:  Any page that wants to store data in config files should override
*           this function and append that data to the given vector in the
*           following format:
*           1) this->IDD as an int (identifier for the page that wrote the data).
*           2) int N, number of bytes of config data appended.
*           3) exactly N bytes of arbitrary data.
*           This base class version does nothing to the vector, indicating
*           that the page does not need to store anything in config files.
*/
void CDevAppPage::GetConfigFileData(std::vector<byte>& cfgData)
{
}


/**
* Function: SetFromConfigFileData
* Purpose:  This is the analogue to GetConfigFileData, above. This function
*           will be called for each page when a config file is loaded. Any page
*           that writes data to a config file in GetConfigFileData needs to be
*           prepared to receive and interpret that same data here.
*           This base class version does nothing.
*/
void CDevAppPage::SetFromConfigFileData(byte const* pData, int nBytes)
{
}

/**
* Function: ModeChanged
* Purpose:  Called when the app switches into or out of Advanced mode, to
*           allow the current tab to do any updates that it wants to.
*/
void CDevAppPage::ModeChanged(bool advanced)
{
}

/**
* Function: ConfigureControls
* Purpose:  
*/
void CDevAppPage::ConfigureControls()
{
	// Override this to set the limits of controls. eg: set range for
	// sliders, bounds of subwindow control, etc.
	TRACE("CDevAppPage::FeatureChanged called. You very probably need to override this function.\n");
}


/**
* Function: PopulateControls
* Purpose:  
*/
void CDevAppPage::PopulateControls()
{
	// Override this to fill in all controls with values that you
	// retrieve from the Document object.
	// Make sure to call the base class version too.
	PopulateCameraSelect();
}


/**
* Function: FeatureChanged
* Purpose:  
*/
void CDevAppPage::FeatureChanged(U32 featureId)
{
	// Override this to populate parts of your page in response to a change
	// in the value of the specified feature.
	TRACE("CDevAppPage::FeatureChanged called. You very probably need to override this function.\n");
}


/**
* Function: DocumentChanged
* Purpose:  
*/
void CDevAppPage::DocumentChanged(int /*eChangeType*/ change)
{
	// No Active Camera (disable everything)
	if (change == CT_NoCamera)
	{
		this->SetGUIState(GS_NoCamera);
		this->PopulateDefaults();
		return;
	}

	// Camera feature changed. Re-Populate controls if required.
	if (change >= CT_FeatureChanged)
	{
		ASSERT(change < CT_FeatureChanged + FEATURES_TOTAL);
		this->FeatureChanged(static_cast<U32>(change - CT_FeatureChanged));
		return;
	}

	// Otherwise - possibly a combination of flags.
	// eg: CT_NewCamera | CT_PreviewStarted would mean: Different camera has
	//     been selected, and its preview window is already playing.
	if ((change & CT_PreviewStarted) != 0)
	{
		this->SetGUIState(GS_PreviewPlaying);
	}
	else if ((change & CT_PreviewPaused) != 0)
	{
		this->SetGUIState(GS_PreviewPaused);
	}
	else if (change < CT_FeatureChanged)
	{
		this->SetGUIState(GS_CameraSelected);
	}

	if ((change & CT_NewCamera) != 0)
	{
		this->ConfigureControls();
		this->PopulateControls();
	}
}


/**
* Function: SetSliderRange
* Purpose:  Subclassses use this method to set the Min and Max values of
*           Slider controls.
*/
void CDevAppPage::SetSliderRange(CSlider& slider, U32 featureId)
{
	try
	{
		if (GetActiveCamera()->FeatureSupported(featureId))
		{
			TempVal<bool> temp(m_bAdjusting[featureId], true);
			slider.SetRange(
				GetActiveCamera()->GetFeatureMinVal(featureId),
				GetActiveCamera()->GetFeatureMaxVal(featureId));
		}
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnClickPlayButton
* Purpose:  
*/
void CDevAppPage::OnClickPlayButton()
{
	CWaitCursor wc;
	try
	{
		GetActiveCamera()->StartPreview();
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnClickPauseButton
* Purpose:  
*/
void CDevAppPage::OnClickPauseButton()
{
	CWaitCursor wc;
	try
	{
		GetActiveCamera()->PausePreview();
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnClickStopButton
* Purpose:  
*/
void CDevAppPage::OnClickStopButton()
{
	CWaitCursor wc;
	try
	{
		GetActiveCamera()->StopPreview();
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnDropdownCameraSelectCombo
* Purpose:  
*/
void CDevAppPage::OnDropdownCameraSelectCombo()
{
	// Refresh the list of cameras.
	try
	{
		CWaitCursor waitForIt;
		GetDocument()->UpdateCameraList(m_CameraSelectCombo, true); // Force reload of serial numbers
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnSelchangeCameraSelectCombo
* Purpose:  
*/
void CDevAppPage::OnSelchangeCameraSelectCombo()
{
	CWaitCursor wc;
	U32 sn = ItemData<U32>(m_CameraSelectCombo);
	if (sn == 0)
		return; // Must be the "No Cameras Connected" entry.

	try
	{
		GetDocument()->SetActiveCamera(sn);
		m_lastUsedSerialNumber = sn;
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: PopulateWindowSize
* Purpose:  Set the "Standard Sizes" combo box to reflect the current value
*           of the ROI in the active camera.
*           I put this here, in the base class, because it is used on more
*           than one tab.
*/
void CDevAppPage::PopulateWindowSize(CComboBox& cb, bool exact /* =true */, U32 featureId /* =FEATURE_ROI */)
{
	try
	{
		if (GetActiveCamera()->FeatureSupported(featureId))
		{
			CRect r = GetActiveCamera()->GetRoi(featureId);
			int width = static_cast<int>(r.right - r.left);
			int height = static_cast<int>(r.bottom - r.top);

			// Look for a match of width and height
			for (int idx = 0; idx < cb.GetCount(); idx++)
			{
				int *pDims = reinterpret_cast<int*>(cb.GetItemDataPtr(idx));
				if (pDims == NULL)
					continue;
				if ((exact
					 && pDims[0] == width
					 && pDims[1] == height)
					||
					(!exact
					 && abs(pDims[0] - width) < 24
					 && abs(pDims[1] - height) < 24))
				{
					TempVal<bool> temp(m_bAdjusting[featureId], true);
					cb.SetCurSel(idx);
					return;
				}
			}

			// If we get here, the ROI does not match one of the standard sizes.
			// Insert an item that describes the current ROI, so that the combo
			// box isn't just empty (it doesn't look good that way - I tried it).
			CString s;
			// "User defined doen't fit in the new, smaller combo box in Simple Mode.
			//s.Format(_T("User defined: %d x %d"), width, height);
			s.Format(_T("%d x %d"), width, height);
			if (cb.GetItemDataPtr(0) == NULL) // There is already a "User defined:..." entry.
				cb.DeleteString(0);
			cb.InsertString(0, s);
			cb.SetItemDataPtr(0, NULL); // Mark this as being not one of the standard entries.
			cb.SetCurSel(0); // Select it.
		}
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: WindowsizeComboChanged
* Purpose:  Event handler for the Change event of the "Standard Sizes" combo
*           box. Sets the PxLSubwindow control to the specified size.
*           As with the previous function, I have put this here in the base
*           class because it is used by more than one page.
*/
void CDevAppPage::WindowsizeComboChanged(CComboBox& cb, U32 featureId /* =FEATURE_ROI */, bool disabled /* = false */)
{
	if (m_bAdjusting[featureId] || cb.GetCurSel() == -1)
		return;

	int* pDims = ItemDataPtr<int*>(cb);
	if (pDims == NULL)
		return;

	try
	{
		// Remove the "User defined: ..." entry, if there is one.
		if (cb.GetItemDataPtr(0) == NULL)
			cb.DeleteString(0);
	    

        GetActiveCamera()->SetRoiSize(pDims[0], pDims[1], theApp.m_bAutoResetPreview, featureId, disabled);
	}
	catch (PxLException const& e)
	{
		this->OnApiError(e);
	}
}




