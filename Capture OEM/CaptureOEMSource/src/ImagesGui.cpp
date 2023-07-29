// ImagesGui.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "ImagesGui.h"
#include "Helpers.h"
#include "TimeLapseCaptureDialog.h"
#include "ImageCaptureDialog.h"


IMPLEMENT_DYNAMIC(CImagesGui, CDevAppPage)


/**
* Function: CImagesGui
* Purpose:  Constructor
*/
CImagesGui::CImagesGui(CPxLDevAppView* pView)
	: CDevAppPage(CImagesGui::IDD, pView)
	, m_bCameraSupportsAutoExposure(false)
{
}


/**
* Function: CImagesGui
* Purpose:  Destructor
*/
CImagesGui::~CImagesGui()
{
}


/**
* Function: DoDataExchange
* Purpose:  
*/
void CImagesGui::DoDataExchange(CDataExchange* pDX)
{
	CDevAppPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_IG_CAMERASELECT_COMBO, m_CameraSelectCombo);
	DDX_Control(pDX, IDC_IG_PLAY_BUTTON, m_PlayButton);
	DDX_Control(pDX, IDC_IG_PAUSE_BUTTON, m_PauseButton);
	DDX_Control(pDX, IDC_IG_STOP_BUTTON, m_StopButton);

	DDX_Control(pDX, IDC_IG_FILENAME_EDIT, m_FileNameEdit);
	DDX_Control(pDX, IDC_IG_FILEBROWSE_BUTTON, m_FileBrowseButton);
	DDX_Control(pDX, IDC_IG_FILEFORMAT_COMBO, m_FileFormatCombo);
	DDX_Control(pDX, IDC_IG_INCREMENTFILENAME_CHECK, m_IncrementAfterCaptureCheck);
	DDX_Control(pDX, IDC_IG_PREVIEWAFTERCAPTURE_CHECK, m_PreviewAfterCaptureCheck);
	DDX_Control(pDX, IDC_IG_CAPTUREFULLFRAME_CHECK, m_CaptureFullFrameCheck);
	DDX_Control(pDX, IDC_IG_CAPTUREFULLRES_CHECK, m_CaptureFullResCheck);

	DDX_Control(pDX, IDC_IG_TIMELAPSECAPTURE_CHECK, m_TimeLapseCaptureCheck);
	DDX_Control(pDX, IDC_IG_CAPTUREEVERY_EDIT, m_CaptureEveryEdit);
	DDX_Control(pDX, IDC_IG_CAPTUREEVERY_COMBO, m_CaptureEveryCombo);
	DDX_Control(pDX, IDC_IG_CAPTUREFOR_EDIT, m_CaptureForEdit);
	DDX_Control(pDX, IDC_IG_CAPTUREFOR_COMBO, m_CaptureForCombo);
	DDX_Control(pDX, IDC_IG_AUTOEXPOSE_CHECK, m_AutoExposeCheck);
	DDX_Control(pDX, IDC_IG_AVERAGE_CHECK, m_AverageFramesCheck);
	DDX_Control(pDX, IDC_IG_AVERAGEOVER_EDIT, m_AverageFramesEdit);

	DDX_Control(pDX, IDC_IG_CAPTURE_BUTTON, m_CaptureButton);
}


BEGIN_MESSAGE_MAP(CImagesGui, CDevAppPage)
	ON_CBN_DROPDOWN(IDC_IG_CAMERASELECT_COMBO, OnDropdownCameraSelectCombo)
	ON_CBN_SELCHANGE(IDC_IG_CAMERASELECT_COMBO, OnSelchangeCameraSelectCombo)
	ON_BN_CLICKED(IDC_IG_PLAY_BUTTON, OnClickPlayButton)
	ON_BN_CLICKED(IDC_IG_PAUSE_BUTTON, OnClickPauseButton)
	ON_BN_CLICKED(IDC_IG_STOP_BUTTON, OnClickStopButton)

	ON_EN_KILLFOCUS(IDC_IG_FILENAME_EDIT, OnKillFocusFileNameEdit)
	ON_BN_CLICKED(IDC_IG_FILEBROWSE_BUTTON, OnClickFileBrowseButton)
	ON_CBN_SELCHANGE(IDC_IG_FILEFORMAT_COMBO, OnSelchangeFileFormatCombo)
//	ON_BN_CLICKED(IDC_IG_TIMELAPSECAPTURE_CHECK, OnClickTimeLapseCaptureCheck)
	ON_BN_CLICKED(IDC_IG_CAPTURE_BUTTON, OnClickCaptureButton)

	ON_UPDATE_COMMAND_UI(IDC_IG_INCREMENTFILENAME_CHECK, OnUpdateIncrementFileNameCheck)
	ON_UPDATE_COMMAND_UI(IDC_IG_CAPTUREEVERY_EDIT, OnUpdateCaptureEveryEdit)
	ON_UPDATE_COMMAND_UI(IDC_IG_CAPTUREEVERY_COMBO, OnUpdateCaptureEveryCombo)
	ON_UPDATE_COMMAND_UI(IDC_IG_CAPTUREFOR_EDIT, OnUpdateCaptureForEdit)
	ON_UPDATE_COMMAND_UI(IDC_IG_CAPTUREFOR_COMBO, OnUpdateCaptureForCombo)
	ON_UPDATE_COMMAND_UI(IDC_IG_AUTOEXPOSE_CHECK, OnUpdateAutoExposeCheck)

END_MESSAGE_MAP()


/**
* Function: GetHelpContextId
* Purpose:  
*/
int CImagesGui::GetHelpContextId(void)
{
	return IDH_IMAGES_TAB;
}


/**
* Function: OnInitDialog
* Purpose:  
*/
BOOL CImagesGui::OnInitDialog(void)
{
	CDevAppPage::OnInitDialog();

	SetUnitOfTimeEntries(m_CaptureEveryCombo);
	SetUnitOfTimeEntries(m_CaptureForCombo, true);
	SetImageFormatEntries(m_FileFormatCombo);

	return TRUE;
}


static enum
{
	TIME_UNIT_MILLISECONDS	= 1,
	TIME_UNIT_SECONDS		= 1000,
	TIME_UNIT_MINUTES		= 60 * TIME_UNIT_SECONDS,
	TIME_UNIT_HOURS			= 60 * TIME_UNIT_MINUTES,
	TIME_UNIT_DAYS			= 24 * TIME_UNIT_HOURS,
};


void CImagesGui::SetUnitOfTimeEntries(CComboBox& cb, bool addFramesEntry /*=false*/)
{
	ClearComboBox(cb);
	int idx = 0;
	cb.AddString(_T("Milliseconds"));
	cb.SetItemData(idx++, TIME_UNIT_MILLISECONDS);
	cb.AddString(_T("Seconds"));
	cb.SetItemData(idx++, TIME_UNIT_SECONDS);
	cb.AddString(_T("Minutes"));
	cb.SetItemData(idx++, TIME_UNIT_MINUTES);
	cb.AddString(_T("Hours"));
	cb.SetItemData(idx++, TIME_UNIT_HOURS);
	cb.AddString(_T("Days"));
	cb.SetItemData(idx++, TIME_UNIT_DAYS);

	if (addFramesEntry)
	{
		cb.AddString(_T("Frames"));
		cb.SetItemData(idx++, -1); // Flag value
	}
}


/**
* Function: OnSetActive
* Purpose:  
*/
BOOL CImagesGui::OnSetActive()
{
	if (CDevAppPage::OnSetActive())
	{
		// The options need to be kept synced with Simple mode. For example, if the
		// user selects a new path to capture clips to in Simple mode, then comes
		// back to this tab, we want to display that new path here as well. We use
		// the UserSettings to track these values.
		UserSettings& settings = theApp.GetUserSettings();
		ApplyUserSettings(settings);

		ScrollToEnd(m_FileNameEdit);

		return TRUE;
	}

	return FALSE;
}


/**
* Function: OnKillActive
* Purpose:  
*/
BOOL CImagesGui::OnKillActive()
{
	// See comment in OnSetActive, above.
	UserSettings& settings = theApp.GetUserSettings();
	StoreUserSettings(settings);

	return CDevAppPage::OnKillActive();
}


/**
* Function: ApplyUserSettings
* Purpose:  
*/
void CImagesGui::ApplyUserSettings(UserSettings& settings)
{
	CString s;

	m_FileNameEdit.SetWindowText(settings.GetStringSetting(USS_IMAGE_FILE_PATH));
	m_FileFormatCombo.SetCurSel(settings.GetIntSetting(USI_IMAGE_FILE_FORMAT_INDEX));
	m_IncrementAfterCaptureCheck.SetCheck(settings.GetBoolSetting(USB_IMAGE_INCREMENT_AFTER_CAPTURE) ? BST_CHECKED : BST_UNCHECKED);
	m_PreviewAfterCaptureCheck.SetCheck(settings.GetBoolSetting(USB_IMAGE_PREVIEW_AFTER_CAPTURE) ? BST_CHECKED : BST_UNCHECKED);
	m_CaptureFullFrameCheck.SetCheck(settings.GetBoolSetting(USB_IMAGE_CAPTURE_FULL_FRAME) ? BST_CHECKED : BST_UNCHECKED);
	m_CaptureFullResCheck.SetCheck(settings.GetBoolSetting(USB_IMAGE_CAPTURE_FULL_RES) ? BST_CHECKED : BST_UNCHECKED);
	m_AverageFramesCheck.SetCheck(settings.GetBoolSetting(USB_IMAGE_CAPTURE_AVERAGE_FRAMES) ? BST_CHECKED : BST_UNCHECKED);

	s.Format(_T("%d"), settings.GetIntSetting(USI_IMAGE_CAPTURE_AVERAGE_FRAMES));
	m_AverageFramesEdit.SetWindowText(s);

	m_TimeLapseCaptureCheck.SetCheck(settings.GetBoolSetting(USB_IMAGE_TIME_LAPSE));

	s.Format(_T("%d"), settings.GetIntSetting(USI_IMAGE_CAPTURE_EVERY_VALUE));
	m_CaptureEveryEdit.SetWindowText(s);
	m_CaptureEveryCombo.SetCurSel(settings.GetIntSetting(USI_IMAGE_CAPTURE_EVERY_UNIT));

	s.Format(_T("%d"), settings.GetIntSetting(USI_IMAGE_CAPTURE_FOR_VALUE));
	m_CaptureForEdit.SetWindowText(s);
	m_CaptureForCombo.SetCurSel(settings.GetIntSetting(USI_IMAGE_CAPTURE_FOR_UNIT));

	m_AutoExposeCheck.SetCheck(settings.GetBoolSetting(USB_IMAGE_AUTOEXPOSE_EACH));
}


/**
* Function: StoreUserSettings
* Purpose:  
*/
void CImagesGui::StoreUserSettings(UserSettings& settings)
{
	settings.SetStringSetting(USS_IMAGE_FILE_PATH, WindowTextAsString(m_FileNameEdit));
	settings.SetIntSetting(USI_IMAGE_FILE_FORMAT_INDEX, m_FileFormatCombo.GetCurSel());

	settings.SetBoolSetting(USB_IMAGE_INCREMENT_AFTER_CAPTURE, BST_CHECKED == m_IncrementAfterCaptureCheck.GetCheck());
	settings.SetBoolSetting(USB_IMAGE_PREVIEW_AFTER_CAPTURE, BST_CHECKED == m_PreviewAfterCaptureCheck.GetCheck());
	settings.SetBoolSetting(USB_IMAGE_CAPTURE_FULL_FRAME, BST_CHECKED == m_CaptureFullFrameCheck.GetCheck());
	settings.SetBoolSetting(USB_IMAGE_CAPTURE_FULL_RES, BST_CHECKED == m_CaptureFullResCheck.GetCheck());
	settings.SetBoolSetting(USB_IMAGE_CAPTURE_AVERAGE_FRAMES, BST_CHECKED == m_AverageFramesCheck.GetCheck());

	int val = WindowTextToInt(m_AverageFramesEdit);
	if (val > 0)
		settings.SetIntSetting(USI_IMAGE_CAPTURE_AVERAGE_FRAMES, val);

	settings.SetBoolSetting(USB_IMAGE_TIME_LAPSE, BST_CHECKED == m_TimeLapseCaptureCheck.GetCheck());

	val = WindowTextToInt(m_CaptureEveryEdit);
	settings.SetIntSetting(USI_IMAGE_CAPTURE_EVERY_VALUE, val);
	settings.SetIntSetting(USI_IMAGE_CAPTURE_EVERY_UNIT, m_CaptureEveryCombo.GetCurSel());

	val = WindowTextToInt(m_CaptureForEdit);
	if (val > 0)
		settings.SetIntSetting(USI_IMAGE_CAPTURE_FOR_VALUE, val);
	settings.SetIntSetting(USI_IMAGE_CAPTURE_FOR_UNIT, m_CaptureForCombo.GetCurSel());

	settings.SetBoolSetting(USB_IMAGE_AUTOEXPOSE_EACH, BST_CHECKED == m_AutoExposeCheck.GetCheck());
}


/**
* Function: SetGUIState
* Purpose:  Enable / Disable controls, according to the state of the camera.
*           Most controls cannot be used in any meaningful way until a camera
*           is initialized.
*/
void CImagesGui::SetGUIState(eGUIState state)
{
	// The CameraSelect and Play/Pause/Stop button states are set in the base
	// class version of this function.
	CDevAppPage::SetGUIState(state);

	bool haveCamera = (state != GS_NoCamera);

	m_FileNameEdit.EnableWindow(haveCamera);
	m_FileBrowseButton.EnableWindow(haveCamera);
	m_FileFormatCombo.EnableWindow(haveCamera);
	m_IncrementAfterCaptureCheck.EnableWindow(haveCamera);
	m_PreviewAfterCaptureCheck.EnableWindow(haveCamera);
	m_CaptureFullFrameCheck.EnableWindow(haveCamera);
	m_CaptureFullResCheck.EnableWindow(haveCamera);
	m_AverageFramesCheck.EnableWindow(haveCamera);
	m_AverageFramesEdit.EnableWindow(haveCamera);
	m_TimeLapseCaptureCheck.EnableWindow(haveCamera);
	m_CaptureButton.EnableWindow(haveCamera);

	ScrollToEnd(m_FileNameEdit);
}


/**
* Function: ConfigureControls
* Purpose:  Set up the controls on the page to customize them according to the
*           currently active camera. This involves things like setting the max
*           and min values for sliders and setting the bounds of the subwindow
*           control.
*/
void CImagesGui::ConfigureControls(void)
{
	m_bCameraSupportsAutoExposure = GetActiveCamera()->FeatureSupportsOnePush(FEATURE_SHUTTER);
	//...
}


/**
* Function: PopulateDefaults
* Purpose:  Put some meaningles default values into the controls, for purely
*           aesthetic reasons.
*/
void CImagesGui::PopulateDefaults(void)
{
	// Nothing to do here. We don't want to clear out the values of any of
	// the controls on this page when the camera is de-selected, because
	// none of the controls display a value that is read from the camera.
	// We just disable everything when there is no active camera, but leave
	// all the actual values intact.
}


/**
* Function: PopulateControls
* Purpose:  Make the values displayed in the controls reflect the current 
*           settings of the active camera.  Called immediately after a 
*           camera is initialized.
*/
void CImagesGui::PopulateControls(void)
{
	CDevAppPage::PopulateControls();

	// Nothing else to do here. There are no controls on this page that
	// display the value of any camera setting.
}


/**
* Function: FeatureChanged
* Purpose:  This method is called by the base class (CDevAppPage) as a result
*           of a call to the View class's OnUpdate method (which is called by
*           the Document class as a result of a change to its data).
*/
void CImagesGui::FeatureChanged(U32 featureId)
{
	// Once again, there is nothing to do here on this particular page.
}


/******************************************************************************
* CImagesGui message handlers
******************************************************************************/

/**
* Function: OnClickFileBrowseButton
* Purpose:  
*/
void CImagesGui::OnClickFileBrowseButton()
{
	BrowseForFile(m_FileNameEdit,
				  imageFormatFilterString,
				  GetImageFormatExtension(m_FileFormatCombo.GetCurSel()));

	ScrollToEnd(m_FileNameEdit);

	SetImageFormatFromFilename(m_FileNameEdit, m_FileFormatCombo);
}


/*
* Function: OnKillFocusFileNameEdit
* Purpose:  Auto-select the entry in the Format combo box that corresponds to
*           the file extension entered, if any.
*/
void CImagesGui::OnKillFocusFileNameEdit()
{
	SetImageFormatFromFilename(m_FileNameEdit, m_FileFormatCombo);
	ScrollToEnd(m_FileNameEdit);
}


/**
* Function: OnSelchangeImageFileFormatCombo
* Purpose:  Make the File Extension in the edit box agree with the format
*           selected here.
*/
void CImagesGui::OnSelchangeFileFormatCombo()
{
	SetFileExtensionFromImageFormat(m_FileNameEdit, m_FileFormatCombo);
	ScrollToEnd(m_FileNameEdit);
}


/**
* Function: OnClickCaptureButton
* Purpose:  
*/
void CImagesGui::OnClickCaptureButton()
{
	CWaitCursor wc;

	if (m_TimeLapseCaptureCheck.GetCheck() == BST_CHECKED)
	{
		//if (IDNO == AfxMessageBox(_T("Time Lapse capture is not implemented yet.\nCapture one frame?"), MB_YESNO))
		//	return;

		CTimeLapseCaptureDialog dlg(this);
		dlg.DoModal();
	}
	else
	{
		// Do all of the work in ImageCaptureDialog
		//
		CImageCaptureDialog dlg(this);
		dlg.DoModal();
	}
}

bool CImagesGui::DoCapture(void)
{
	int nframes = 1; // default
	if (m_AverageFramesCheck.GetCheck() == BST_CHECKED)
	{
		nframes = WindowTextToInt(m_AverageFramesEdit);
		if (nframes < 1)
		{
			AfxMessageBox(_T("Invalid number of frames to average."));
			m_AverageFramesEdit.SetFocus();
			m_AverageFramesEdit.SetSel(0, 100);
			return false;
		}
	}

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
			//ff.reset(new CTempFullFrame(GetActiveCamera(), false)); // vc7
			ff = std::auto_ptr<CTempFullFrame>(new CTempFullFrame(GetActiveCamera(), false)); // vc6
            // Bugzilla.778 - Pause the preview if we had to change the camera to a full frame state
			if (ff->ChangedValue())
            {
			    if (GetActiveCamera()->GetPreviewState() == START_PREVIEW)
                {
				    ps = std::auto_ptr<CPreviewState>(new CPreviewState(GetActiveCamera(), PAUSE_PREVIEW));
                }
				discardFrame = true;
            }
		}
		if (m_CaptureFullResCheck.GetCheck() == BST_CHECKED)
		{
			fr = std::auto_ptr<CTempFullResolution>(new CTempFullResolution(GetActiveCamera()));
			if (fr->ChangedValue())
				discardFrame = true;
		}

		bool autoExpose = (m_TimeLapseCaptureCheck.GetCheck() == BST_CHECKED
							&& m_AutoExposeCheck.GetCheck() == BST_CHECKED);

		// Never discard the first frame when in Hardware Trigger mode.
		U32 trigger_flags = 0;
		Trigger trigger = GetActiveCamera()->GetTrigger(&trigger_flags);
		if ((trigger_flags & FEATURE_FLAG_OFF) == 0 && trigger.Type == TRIGGER_TYPE_HARDWARE)
			discardFrame = false;

        if (GetDocument()->CaptureImage(m_FileNameEdit, m_FileFormatCombo, discardFrame ? 1 : 0, nframes, autoExpose))
		{
            // The call to SysOpenFile can take a while -- so be sure we update our camera name for the next thread
            // that may end up here.
			CString fileName(WindowTextAsString(m_FileNameEdit));
            if (m_IncrementAfterCaptureCheck.GetCheck() == BST_CHECKED) {
				IncrementFileName(m_FileNameEdit, "%4.4d");
            }
			if (m_PreviewAfterCaptureCheck.GetCheck() == BST_CHECKED) {
				SysOpenFile(fileName);
			}
		}
		else
			return false;
	}
	catch (PxLException const& e)
	{
        int rc = e.GetReturnCode();
        // ApiStremStopped simply means a cancel, and ApiNoCameraError simple means the camera was disconnected -- don't treat 
        // these as errors
		if (rc != ApiStreamStopped &&
		    rc != ApiNoCameraError)
		{
		    OnApiError(e);
		    return false;
		}
	}
	return true;
}


/******************************************************************************
* UPDATE_COMMAND_UI handlers.
******************************************************************************/

/**
* Function: OnUpdateIncrementFileNameCheck
* Purpose:  
*/
void CImagesGui::OnUpdateIncrementFileNameCheck(CCmdUI *pCmdUI)
{
	// 2004-07-27 - Commented out (CP). People may want to time-lapse capture
	//              to the same file every time.
	//if (m_TimeLapseCaptureCheck.GetCheck() == BST_CHECKED)
	//{
	//	pCmdUI->SetCheck(BST_CHECKED);
	//	pCmdUI->Enable(FALSE);
	//}
	//else
	//{
	//	pCmdUI->Enable(GetDocument()->HasActiveCamera());
	//}
}


/**
* Function: OnUpdateCaptureEveryEdit
* Purpose:  
*/
void CImagesGui::OnUpdateCaptureEveryEdit(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->HasActiveCamera()
					&& m_TimeLapseCaptureCheck.GetCheck() == BST_CHECKED);
}


/**
* Function: OnUpdateCaptureEveryCombo
* Purpose:  
*/
void CImagesGui::OnUpdateCaptureEveryCombo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->HasActiveCamera() 
					&& m_TimeLapseCaptureCheck.GetCheck() == BST_CHECKED);
}


/**
* Function: OnUpdateCaptureForEdit
* Purpose:  
*/
void CImagesGui::OnUpdateCaptureForEdit(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->HasActiveCamera() 
					&& m_TimeLapseCaptureCheck.GetCheck() == BST_CHECKED);
}


/**
* Function: OnUpdateCaptureForCombo
* Purpose:  
*/
void CImagesGui::OnUpdateCaptureForCombo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->HasActiveCamera() 
					&& m_TimeLapseCaptureCheck.GetCheck() == BST_CHECKED);
}


/**
* Function: OnUpdateAutoExposeCheck
* Purpose:  
*/
void CImagesGui::OnUpdateAutoExposeCheck(CCmdUI *pCmdUI)
{
    U32 trigger_flags = 0;
    Trigger trigger;
    BOOL softwareTriggering = FALSE;
    BOOL haveCamera = GetDocument()->HasActiveCamera();

    BOOL moitorAutoExposeCheck = FALSE;

    if (haveCamera && m_bCameraSupportsAutoExposure && m_TimeLapseCaptureCheck.GetCheck() == BST_CHECKED)
    {
        // Candidate for moitoring.  But as per Bugzilla.563, don't allow auto-exposure for software triggering
	    trigger = GetActiveCamera()->GetTrigger(&trigger_flags);
	    softwareTriggering = ((trigger_flags & FEATURE_FLAG_OFF) == 0 && trigger.Type == TRIGGER_TYPE_SOFTWARE);

        if (!softwareTriggering) moitorAutoExposeCheck = TRUE;
    }

	pCmdUI->Enable(moitorAutoExposeCheck);
}













