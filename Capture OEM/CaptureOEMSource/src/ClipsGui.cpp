// ClipsGui.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "ClipsGui.h"
#include "Helpers.h"

// Compression levels
#define COMPRESSION_LOW_QUALITY   2000.0f // 2000:1 compression
#define COMPRESSION_HIGH_QUALITY    10.0f // 10:1 compression
#define COMPRESSION_TARGET         100.0f // 100:1 compression

// Bitrate levels (specificed in bits per second
#define BITRATE_FLOOR            10000.0f
#define BITRATE_CEILING       50000000.0f // Larger values will crash older versions of FFMEPEG library

#define CAP_TIME_MIN                 1.0f
#define CAP_TIME_MAX (60.0f * 60.0f * 24.0f) // Limit the captuer to one day
#define CAP_TIME_DEFAULT            10.0f  

// CClipsGui dialog

IMPLEMENT_DYNAMIC(CClipsGui, CDevAppPage)

CClipsGui::CClipsGui(CPxLDevAppView* pView)
	: CDevAppPage(CClipsGui::IDD, pView)
	, m_bCaptureInProgress(false)
{
}

CClipsGui::~CClipsGui()
{
}

void CClipsGui::DoDataExchange(CDataExchange* pDX)
{
	CDevAppPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CG_CAMERASELECT_COMBO, m_CameraSelectCombo);
	DDX_Control(pDX, IDC_CG_PLAY_BUTTON, m_PlayButton);
	DDX_Control(pDX, IDC_CG_PAUSE_BUTTON, m_PauseButton);
	DDX_Control(pDX, IDC_CG_STOP_BUTTON, m_StopButton);

	DDX_Control(pDX, IDC_CG_NUMFRAMES_EDIT, m_NumFramesEdit);
	DDX_Control(pDX, IDC_CG_CAPTIME_EDIT, m_CapTimeEdit);
	DDX_Control(pDX, IDC_CG_ENCODING_COMBO, m_EncodingCombo);
	DDX_Control(pDX, IDC_CG_CLIPDECIMATIONFACTOR_EDIT, m_ClipDecimationFactorEdit);
	
	DDX_Control(pDX, IDC_CG_MATCHCAPTURE_CHECK, m_MatchCaptureCheck);
	DDX_Control(pDX, IDC_CG_PBFRAMERATE_EDIT, m_PbFrameRateEdit);
	DDX_Control(pDX, IDC_CG_CAMERAFRAMERATE_EDIT, m_CameraFrameRateEdit);
	DDX_Control(pDX, IDC_CG_PBTIME_EDIT, m_PbTimeEdit);
	DDX_Control(pDX, IDC_CG_PBEFFECT_EDIT, m_PbEffectEdit);

	DDX_Control(pDX, IDC_CG_AUTOBITRATE_CHECK, m_AutoBitRateCheck);
	DDX_Control(pDX, IDC_CG_BITRATE_SLIDER, m_BitRateSlider);
	DDX_Control(pDX, IDC_CG_BITRATE_EDIT, m_BitRateEdit);
	DDX_Control(pDX, IDC_CG_BITRATEMIN_LABEL, m_BitRateMinLabel);
	DDX_Control(pDX, IDC_CG_BITRATEMAX_LABEL, m_BitRateMaxLabel);
	DDX_Control(pDX, IDC_CG_FILESIZE_EDIT, m_FileSizeEdit);

    DDX_Control(pDX, IDC_CG_SAVERAWDATA_CHECK, m_SaveRawDataFileCheck);
	DDX_Control(pDX, IDC_CG_RAWDATAFILE_EDIT, m_RawDataFileEdit);
	DDX_Control(pDX, IDC_CG_RAWDATABROWSE_BUTTON, m_RawDataFileBrowseButton);
	DDX_Control(pDX, IDC_CG_INCREMENTRAWDATAFILENAME_CHECK, m_IncrementRawDataFileCheck);

	DDX_Control(pDX, IDC_CG_SAVEFORMATTED_CHECK, m_SaveFormattedCheck);
	DDX_Control(pDX, IDC_CG_CLIPFORMAT_COMBO, m_ClipFileFormatCombo);
	DDX_Control(pDX, IDC_CG_FORMATTEDFILE_EDIT, m_ClipFileNameEdit);
	DDX_Control(pDX, IDC_CG_FORMATTEDBROWSE_BUTTON, m_FormattedFileBrowseButton);
	DDX_Control(pDX, IDC_CG_INCREMENTFORMATTEDFILENAME_CHECK, m_IncrementFormattedFileCheck);
	DDX_Control(pDX, IDC_CG_VIEWAFTERCAPTURE_CHECK, m_ViewAfterCaptureCheck);

	DDX_Control(pDX, IDC_CG_BEGINCAPTURE_BUTTON, m_BeginCaptureButton);
}


BEGIN_MESSAGE_MAP(CClipsGui, CDevAppPage)

	ON_CBN_DROPDOWN(IDC_CG_CAMERASELECT_COMBO, OnDropdownCameraSelectCombo)
	ON_CBN_SELCHANGE(IDC_CG_CAMERASELECT_COMBO, OnSelchangeCameraSelectCombo)
	ON_BN_CLICKED(IDC_CG_PLAY_BUTTON, OnClickPlayButton)
	ON_BN_CLICKED(IDC_CG_PAUSE_BUTTON, OnClickPauseButton)
	ON_BN_CLICKED(IDC_CG_STOP_BUTTON, OnClickStopButton)

    ON_EN_KILLFOCUS(IDC_CG_NUMFRAMES_EDIT, OnKillFocusNumFramesEdit)
    ON_EN_KILLFOCUS(IDC_CG_CAPTIME_EDIT, OnKillFocusCapTimeEdit)
	ON_CBN_SELCHANGE(IDC_CG_ENCODING_COMBO, OnSelchangeEncodingCombo)
    ON_EN_KILLFOCUS(IDC_CG_CLIPDECIMATIONFACTOR_EDIT, OnKillFocusClipDecimationFactorEdit)

	ON_BN_CLICKED(IDC_CG_MATCHCAPTURE_CHECK, OnUpdateMatchCaptureCheck)
    ON_EN_KILLFOCUS(IDC_CG_PBFRAMERATE_EDIT, OnKillFocusPbFrameRateEdit)

	ON_BN_CLICKED(IDC_CG_AUTOBITRATE_CHECK, OnUpdateAutoBitRateCheck)
	ON_EN_KILLFOCUS(IDC_CG_BITRATE_EDIT, OnKillFocusBitRateEdit)

    ON_EN_KILLFOCUS(IDC_CG_RAWDATAFILE_EDIT, OnKillFocusRawDataFileEdit)
	ON_BN_CLICKED(IDC_CG_RAWDATABROWSE_BUTTON, OnClickRawDataBrowseButton)

	ON_EN_KILLFOCUS(IDC_CG_FORMATTEDFILE_EDIT, OnKillFocusClipFileNameEdit)
	ON_BN_CLICKED(IDC_CG_FORMATTEDBROWSE_BUTTON, OnClickFormattedBrowseButton)
	ON_CBN_SELCHANGE(IDC_CG_CLIPFORMAT_COMBO, OnSelchangeClipFormatCombo)

	ON_BN_CLICKED(IDC_CG_BEGINCAPTURE_BUTTON, OnClickBeginCaptureButton)

	ON_UPDATE_COMMAND_UI(IDC_CG_RAWDATAFILE_EDIT, OnUpdateRawDataFileEdit)
	ON_UPDATE_COMMAND_UI(IDC_CG_RAWDATABROWSE_BUTTON, OnUpdateRawDataFileBrowseButton)
	ON_UPDATE_COMMAND_UI(IDC_CG_INCREMENTRAWDATAFILENAME_CHECK, OnUpdateIncrementRawDataFileCheck)

	ON_UPDATE_COMMAND_UI(IDC_CG_CLIPFORMAT_COMBO, OnUpdateClipFormatCombo)
	ON_UPDATE_COMMAND_UI(IDC_CG_FORMATTEDFILE_EDIT, OnUpdateFileFormatCombo)
	ON_UPDATE_COMMAND_UI(IDC_CG_FORMATTEDBROWSE_BUTTON, OnUpdateFormattedFileEdit)
	ON_UPDATE_COMMAND_UI(IDC_CG_INCREMENTFORMATTEDFILENAME_CHECK, OnUpdateIncrementFormattedFileNameCheck)
	ON_UPDATE_COMMAND_UI(IDC_CG_VIEWAFTERCAPTURE_CHECK, OnUpdateViewAfterCaptureCheck)

	ON_UPDATE_COMMAND_UI(IDC_CG_BEGINCAPTURE_BUTTON, OnUpdateBeginCaptureButton)

END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CClipsGui, CDevAppPage)
	ON_EVENT(CClipsGui, IDC_CG_BITRATE_SLIDER, 1, OnScrollBitRateSlider, VTS_NONE)
	ON_EVENT(CClipsGui, IDC_CG_BITRATE_SLIDER, 2, OnChangeBitRateSlider, VTS_NONE)
END_EVENTSINK_MAP()



/**
* Function: EffectiveFrameRate
* Purpose:  Queries the camera to see what the camera is using for it's frame rate
*/
float CClipsGui::EffectiveFrameRate()
{
    try 
    {
        if (GetActiveCamera()->FeatureSupported(FEATURE_ACTUAL_FRAME_RATE))
        {
            return GetActiveCamera()->GetFeatureValue(FEATURE_ACTUAL_FRAME_RATE);
        } else {
            return GetActiveCamera()->GetFeatureValue(FEATURE_FRAME_RATE);
        }
    }

    catch (PxLException const& e)
	{
		OnApiError(e);
	}

    return 1.0; // error case -- good value as any
}

/**
* Function: RequestedPlaybackFrameRate
* Purpose:  What the user has specified for clip playback frame rate
*/
float CClipsGui::RequestedPlaybackFrameRate()
{
	float playbackRate;
	CString s;

    playbackRate = WindowTextToFloat(m_PbFrameRateEdit);

    // Unfortunatly, the above call will undo our formatting.  Reassert the formatting
    s.Format(_T("%.1f"), playbackRate);
	m_PbFrameRateEdit.SetWindowText(s);

    // If the user specifies a value of 0 or smaller, then assume thay want to match the camera's frame rate.
    if (playbackRate <= 0.0) playbackRate = EffectiveFrameRate();

    return playbackRate;
}

void CClipsGui::PopulateNumFrames(void)
{
    float capTime = WindowTextToFloat(m_CapTimeEdit);
    float decimation = WindowTextToFloat(m_ClipDecimationFactorEdit); // Bugzilla.2329
    int   numFrames = (int) (capTime * (EffectiveFrameRate() / decimation));
	CString s;

	s.Format(_T("%d"), numFrames);
	m_NumFramesEdit.SetWindowText(s);
}

void CClipsGui::PopulateCaptureTime(void)
{
    float captureTime;  // The video capture time in seconds
	CString s;

    captureTime = ((float)(WindowTextToInt(m_NumFramesEdit) * WindowTextToInt(m_ClipDecimationFactorEdit))) / EffectiveFrameRate();
    s.Format(_T("%.1f"), captureTime);
	m_CapTimeEdit.SetWindowText(s);
}

void CClipsGui::PopulatePlaybackRate(void)
{
	bool matchFrameRate = m_MatchCaptureCheck.GetCheck() == BST_CHECKED;

    m_PbFrameRateEdit.EnableWindow(!matchFrameRate);
    if (matchFrameRate)
    {
        float playbackRate = EffectiveFrameRate();
        // adjust the frame rate to account for clip decimation
        playbackRate = playbackRate / (float)WindowTextToInt(m_ClipDecimationFactorEdit);
	    CString s;

        s.Format(_T("%.1f"), playbackRate);
	    m_PbFrameRateEdit.SetWindowText(s);
    }
}

void CClipsGui::PopulateCameraFrameRate(void)
{
	CString s;

    s.Format(_T("%.1f"), EffectiveFrameRate());
    m_CameraFrameRateEdit.SetWindowText(s);
}

void CClipsGui::PopulatePlaybackTime(void)
{
    float playbackTime;  // The video playback time in seconds
	CString s;

    // There are several cases to consider:
    //     A/ Uncompressed
    //     B/ Compressed where the playback rate matches the capture rate.
    //     C/ Compressed with a user specifed playback frame rate
   	bool compressed = m_EncodingCombo.GetCurSel() != CLIP_ENCODING_PDS; // PDS is the only uncompressed type
    bool matchCapture = m_MatchCaptureCheck.GetCheck() == BST_CHECKED;

    if (!compressed)
    {
        // Case A 
        //      In this case, the plaback time is the same as the capture time (but compute
        //      from first principles, so that the ivoker does not need to update capture time
        //      first.
        playbackTime = (float)(WindowTextToInt(m_NumFramesEdit)) / EffectiveFrameRate();
    } else {
        if (matchCapture)
        {
            // Case B
            //      The playback rate matches capture rate.  The playbackTime needs to account for the decimation factor, as the
            //      number of frames represent the number of frames in the capture, not the number of frames from the camera
            playbackTime = (float) (WindowTextToInt(m_NumFramesEdit) * WindowTextToInt(m_ClipDecimationFactorEdit)) / EffectiveFrameRate();
        } else {
            // Case C
            //      User is controlling the plyback rate, so clearly it has not yet factored for Clip decimation
            playbackTime = ((float)WindowTextToInt(m_NumFramesEdit)) / RequestedPlaybackFrameRate();
        }
    }
    s.Format(_T("%.1f"), playbackTime);
	m_PbTimeEdit.SetWindowText(s);
}

void CClipsGui::PopulatePlaybackEffect(void)
{
	float ratio = WindowTextToFloat (m_PbTimeEdit) / WindowTextToFloat (m_CapTimeEdit);

    if (ratio < 0.95f)
    {
        m_PbEffectEdit.SetWindowText (_T("Fast Motion"));
    } else if (ratio > 1.05f) {
        m_PbEffectEdit.SetWindowText (_T("Slow Motion"));
    } else {
        m_PbEffectEdit.SetWindowText (_T("None"));
    }
}

void CClipsGui::PopulateBitRate(void)
{
	bool compressed = m_EncodingCombo.GetCurSel() != CLIP_ENCODING_PDS; // PDS is the only uncompressed type
    bool autoBitRate = m_AutoBitRateCheck.GetCheck() == BST_CHECKED;

    m_BitRateSlider.EnableWindow(compressed && !autoBitRate);
    m_BitRateEdit.EnableWindow(compressed && !autoBitRate);
    m_BitRateMinLabel.EnableWindow(compressed && !autoBitRate);
    m_BitRateMaxLabel.EnableWindow(compressed && !autoBitRate);
    
    if (!compressed) return;  // The values are meaningless if we arn't using compression

    CRect Roi = GetActiveCamera()->GetFeatureRect(FEATURE_ROI);
    U32   other_flags = 0;
    int PixelFormat = static_cast<int>(GetActiveCamera()->GetFeatureValue(FEATURE_PIXEL_FORMAT, &other_flags));
    float PixelSize = GetPixelFormatSize(PixelFormat);
    PixelAddressing Pa = GetActiveCamera()->GetPixelAddressing();
		        
    float bytesPerFrame = (static_cast<float>((Roi.Width()/Pa.x) * (Roi.Height()/Pa.y))) * PixelSize;
    // The 'Effective' bit rate must compensate for the clip decimation
    float effectiveBitRate = (bytesPerFrame * WindowTextToFloat(m_PbFrameRateEdit) * 8.0f) * WindowTextToFloat(m_ClipDecimationFactorEdit);
    float lowQuality = max (effectiveBitRate / COMPRESSION_LOW_QUALITY, BITRATE_FLOOR);
    lowQuality /= (1000.0f * 1000.0f); // Convert to Mbps (note that FFMPEGlibrary uses 1000, not 1024)
    float highQuality = min (effectiveBitRate / COMPRESSION_HIGH_QUALITY, BITRATE_CEILING);
    highQuality /= (1000.0f * 1000.0f); // Convert to Mbps (note that FFMPEGlibrary uses 1000, not 1024)

    m_BitRateSlider.SetRange (lowQuality, highQuality);

    float target;
    if (autoBitRate)
    {
        target = min (max (effectiveBitRate / COMPRESSION_TARGET, BITRATE_FLOOR), BITRATE_CEILING);
        target /= (1000.0f * 1000.0f); // Vonvert ro Mbps (note that FFMPEGlibrary uses 1000, not 1024)
    } else {
        target = m_BitRateSlider.GetValue();
    }
    if (target > highQuality) target = highQuality;
    if (target < lowQuality)  target = lowQuality;
    m_BitRateSlider.SetValue(target);
}

void CClipsGui::PopulateFileSize(void)
{
	bool compressed = m_EncodingCombo.GetCurSel() != CLIP_ENCODING_PDS; // PDS is the only uncompressed type
    float fileSize;  // In MBytes
	CString s;

    if (compressed)
    {
        // ((frames / frames/second) * Mbits/second) / bits/byte == MBytes
        fileSize = (((float)(WindowTextToInt(m_NumFramesEdit)) / WindowTextToFloat(m_PbFrameRateEdit)) * m_BitRateSlider.GetValue()) / 8.0f;
    } else {
        CRect Roi = GetActiveCamera()->GetFeatureRect(FEATURE_ROI);
        U32   other_flags = 0;
        int PixelFormat = static_cast<int>(GetActiveCamera()->GetFeatureValue(FEATURE_PIXEL_FORMAT, &other_flags));
        float PixelSize = GetPixelFormatSize(PixelFormat);
        PixelAddressing Pa = GetActiveCamera()->GetPixelAddressing();
		        
        float bytesPerFrame = (static_cast<float>((Roi.Width()/Pa.x) * (Roi.Height()/Pa.y))) * PixelSize;
        
        fileSize = ((float)(WindowTextToInt(m_NumFramesEdit))) * bytesPerFrame;
        fileSize /= (1024.0f * 1024.0f); // Convert to MBytes
    }
    s.Format(_T("%.1f"), fileSize);
	m_FileSizeEdit.SetWindowText(s);
}

/**
* Function: GetHelpContextId
* Purpose:  
*/
int CClipsGui::GetHelpContextId(void)
{
	return IDH_CLIPS_TAB;
}


/**
* Function: OnInitDialog
* Purpose:  
*/
BOOL CClipsGui::OnInitDialog()
{
	CDevAppPage::OnInitDialog();

	m_BitRateSlider.SetLinkedWindows(&m_BitRateEdit, &m_BitRateMinLabel, &m_BitRateMaxLabel, NULL);

    SetClipFormatEntries(m_ClipFileFormatCombo);
    SetClipEncodingEntries(m_EncodingCombo);

	return TRUE;
}


/**
* Function: OnSetActive
* Purpose:  
*/
BOOL CClipsGui::OnSetActive()
{
	if (CDevAppPage::OnSetActive())
	{
		// The options pertaining to clip capture need to be kept synced with
		// the Simple tab - that is, if the user selects a new path to capture clips
		// to on the Simple tab, then comes back to this tab, we want to display
		// that new path here as well. We use the UserSettings to track these
		// values.
        // Bugzilla.1949 -- Don't do this,as it will override userdefined values for other settings
		//UserSettings& settings = theApp.GetUserSettings();
		//ApplyUserSettings(settings);

		ScrollToEnd(m_RawDataFileEdit);
		ScrollToEnd(m_ClipFileNameEdit);

		return TRUE;
	}

	return FALSE;
}


/**
* Function: OnKillActive
* Purpose:  
*/
BOOL CClipsGui::OnKillActive()
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
void CClipsGui::ApplyUserSettings(UserSettings& settings)
{
	CString s;
    bool    pbMatchCapture;
    bool    autoBitRate;
    int     idx;

	s.Format(_T("%d"), settings.GetIntSetting(USI_CLIP_NUM_FRAMES));
	m_NumFramesEdit.SetWindowText(s);
	idx = settings.GetIntSetting(USI_CLIP_ENCODING_INDEX);
	if (idx < 0) idx = 0;
	m_EncodingCombo.SetCurSel(idx);
	s.Format(_T("%d"), settings.GetIntSetting(USI_CLIP_DECIMATION_FACTOR));
	m_ClipDecimationFactorEdit.SetWindowText(s);

    pbMatchCapture = settings.GetBoolSetting(USB_CLIP_MATCH_CAPTURE);
    m_MatchCaptureCheck.SetCheck(pbMatchCapture ? BST_CHECKED : BST_UNCHECKED);
    /*
    if (!pbMatchCapture)
    {
    	s.Format(_T("%.1f"), settings.GetFloatSetting(USF_CLIP_PB_FRAME_RATE));
	    m_PbFrameRateEdit.SetWindowText(s);
    }
    */

    autoBitRate = settings.GetBoolSetting(USB_CLIP_AUTO_BIT_RATE);
    m_AutoBitRateCheck.SetCheck(autoBitRate ? BST_CHECKED : BST_UNCHECKED);
    /*
    if (! autoBitRate)
    {
        m_BitRateSlider.SetValue ((float) settings.GetIntSetting (USI_CLIP_PB_BIT_RATE));
    }
    */
	
    m_ViewAfterCaptureCheck.SetCheck(settings.GetBoolSetting(USB_CLIP_VIEW_AFTER_CAPTURE) ? BST_CHECKED : BST_UNCHECKED);

	m_SaveRawDataFileCheck.SetCheck(settings.GetBoolSetting(USB_RAW_SAVE_FILE) ? BST_CHECKED : BST_UNCHECKED);
	m_RawDataFileEdit.SetWindowText(settings.GetStringSetting(USS_RAW_FILE_PATH));
	m_IncrementRawDataFileCheck.SetCheck(settings.GetBoolSetting(USB_RAW_INCREMENT_AFTER_CAPTURE) ? BST_CHECKED : BST_UNCHECKED);

	m_SaveFormattedCheck.SetCheck(settings.GetBoolSetting(USB_CLIP_SAVE_FILE) ? BST_CHECKED : BST_UNCHECKED);
	idx = settings.GetIntSetting(USI_CLIP_FILE_FORMAT_INDEX);
	if (idx < 0) idx = 0;
	m_ClipFileFormatCombo.SetCurSel(idx);
	m_ClipFileNameEdit.SetWindowText(settings.GetStringSetting(USS_CLIP_FILE_PATH));
	m_IncrementFormattedFileCheck.SetCheck(settings.GetBoolSetting(USB_CLIP_INCREMENT_AFTER_CAPTURE) ? BST_CHECKED : BST_UNCHECKED);

}


/**
* Function: StoreUserSettings
* Purpose:  
*/
void CClipsGui::StoreUserSettings(UserSettings& settings)
{
    bool pbMatchCapture = (m_MatchCaptureCheck.GetCheck() == BST_CHECKED); 
    bool autoBitRate = (m_AutoBitRateCheck.GetCheck() == BST_CHECKED); 

	settings.SetFloatSetting (USF_CLIP_CAPTURE_TIME, WindowTextToFloat(m_CapTimeEdit));
    settings.SetIntSetting (USI_CLIP_ENCODING_INDEX, m_EncodingCombo.GetCurSel());
	settings.SetIntSetting (USI_CLIP_DECIMATION_FACTOR, WindowTextToInt(m_ClipDecimationFactorEdit));
    
    settings.SetBoolSetting (USB_CLIP_MATCH_CAPTURE, pbMatchCapture);
    if (!pbMatchCapture)
    {
        settings.SetFloatSetting (USF_CLIP_PB_FRAME_RATE, WindowTextToFloat(m_PbFrameRateEdit));
    }

    settings.SetBoolSetting (USB_CLIP_AUTO_BIT_RATE, autoBitRate);
    if (!autoBitRate)
    {
        settings.SetIntSetting (USI_CLIP_PB_BIT_RATE, (int)m_BitRateSlider.GetValue ());
    }

    settings.SetBoolSetting(USB_CLIP_VIEW_AFTER_CAPTURE, m_ViewAfterCaptureCheck.GetCheck() == BST_CHECKED);

	settings.SetBoolSetting(USB_RAW_SAVE_FILE, m_SaveRawDataFileCheck.GetCheck() == BST_CHECKED);
	settings.SetStringSetting(USS_RAW_FILE_PATH, WindowTextAsString(m_RawDataFileEdit));
	settings.SetBoolSetting(USB_RAW_INCREMENT_AFTER_CAPTURE, m_IncrementRawDataFileCheck.GetCheck() == BST_CHECKED);

	settings.SetBoolSetting(USB_CLIP_SAVE_FILE, m_SaveFormattedCheck.GetCheck() == BST_CHECKED);
	settings.SetIntSetting(USI_CLIP_FILE_FORMAT_INDEX, m_ClipFileFormatCombo.GetCurSel());
	settings.SetStringSetting(USS_CLIP_FILE_PATH, WindowTextAsString(m_ClipFileNameEdit));
	settings.SetBoolSetting(USB_CLIP_INCREMENT_AFTER_CAPTURE, m_IncrementFormattedFileCheck.GetCheck() == BST_CHECKED);
}


/**
* Function: SetGUIState
* Purpose:  Enable / Disable controls, according to the state of the camera.
*           Most controls cannot be used in any meaningful way until a camera
*           is initialized.
*/
void CClipsGui::SetGUIState(eGUIState state)
{
	// The CameraSelect and Play/Pause/Stop button states are set in the base
	// class version of this function.
	CDevAppPage::SetGUIState(state);

	bool enable = (state != GS_NoCamera);
    bool matchFrameRate = m_MatchCaptureCheck.GetCheck() == BST_CHECKED;
    bool autoBitRate = m_AutoBitRateCheck.GetCheck() == BST_CHECKED;
    bool compressed = m_EncodingCombo.GetCurSel() != CLIP_ENCODING_PDS; // PDS is the only uncompressed type

	m_NumFramesEdit.EnableWindow(enable);
	m_CapTimeEdit.EnableWindow(enable);
    m_EncodingCombo.EnableWindow(enable);
	m_ClipDecimationFactorEdit.EnableWindow(enable && compressed);

    m_MatchCaptureCheck.EnableWindow(enable);
    m_PbFrameRateEdit.EnableWindow(enable && matchFrameRate);

    m_AutoBitRateCheck.EnableWindow(enable && compressed);
    m_BitRateSlider.put_Enabled(enable && compressed && !autoBitRate);
    m_BitRateEdit.EnableWindow(enable && compressed && !autoBitRate);
    m_BitRateMinLabel.EnableWindow(enable && compressed && !autoBitRate);
    m_BitRateMaxLabel.EnableWindow(enable && compressed && !autoBitRate);

    m_ViewAfterCaptureCheck.EnableWindow(enable);
	m_SaveRawDataFileCheck.EnableWindow(enable);
	m_SaveFormattedCheck.EnableWindow(enable);

	ScrollToEnd(m_RawDataFileEdit);
	ScrollToEnd(m_ClipFileNameEdit);
}


/**
* Function: ConfigureControls
* Purpose:  Set up the controls on the page to customize them according to the
*           currently active camera. This involves things like setting the max
*           and min values for sliders and setting the bounds of the subwindow
*           control.
*/
void CClipsGui::ConfigureControls(void)
{
	// For this page, there is nothing that needs to be done here.
}


/**
* Function: PopulateDefaults
* Purpose:  Put some meaningles default values into the controls, for purely
*           aesthetic reasons.
*/
void CClipsGui::PopulateDefaults(void)
{
	SetSliderDefault(m_BitRateSlider);
}


/**
* Function: PopulateControls
* Purpose:  Make the values displayed in the controls reflect the current 
*           settings of the active camera.  Called immediately after a 
*           camera is initialized.
*/
void CClipsGui::PopulateControls(void)
{
	CDevAppPage::PopulateControls();
    //PopulateNumFrames();  // Use the settings default
    PopulateCaptureTime();
    PopulateCameraFrameRate();
    PopulatePlaybackRate();
    PopulatePlaybackTime();
    PopulatePlaybackEffect();
    PopulateBitRate();
    PopulateFileSize();
}


/**
* Function: FeatureChanged
* Purpose:  This method is called by the base class (CDevAppPage) as a result
*           of a call to the View class's OnUpdate method (which is called by
*           the Document class as a result of a change to its data).
*/
void CClipsGui::FeatureChanged(U32 featureId)
{
	switch (featureId)
	{
    case FEATURE_ACTUAL_FRAME_RATE:
    case FEATURE_FRAME_RATE:
        PopulateCameraFrameRate();
        PopulateCaptureTime();
        PopulatePlaybackTime();
        PopulatePlaybackRate();
        PopulateFileSize();
        break;
    case FEATURE_ROI:
    case FEATURE_PIXEL_FORMAT:
    case FEATURE_PIXEL_ADDRESSING:
        PopulateBitRate();
        PopulateFileSize();
        break;
    }
}


/******************************************************************************
* Message handlers.
******************************************************************************/

/**
* Function: OnKillFocusNumFramesEdit
* Purpose:  
*/
void CClipsGui::OnKillFocusNumFramesEdit(void)
{
    PopulateCaptureTime();
    PopulatePlaybackTime();
    PopulateFileSize();
}

/**
* Function: OnKillFocusCapTimeEdit
* Purpose:  
*/
void CClipsGui::OnKillFocusCapTimeEdit(void)
{
    float capTime = WindowTextToFloat(m_CapTimeEdit);
    if (capTime < CAP_TIME_MIN ||
        capTime > CAP_TIME_MAX)
    {
        CString s;

        capTime = CAP_TIME_DEFAULT;
        s.Format(_T("%.1f"), capTime);
	    m_CapTimeEdit.SetWindowText(s);
    }
    PopulateNumFrames();
    PopulatePlaybackTime();
    PopulateFileSize();
}

/**
* Function: OnSelchangeEncodingCombo
* Purpose:  
*/
void CClipsGui::OnSelchangeEncodingCombo(void)
{
    bool bCompression = m_EncodingCombo.GetCurSel() != CLIP_ENCODING_PDS;

    // Clip Decimpation does not apply to non-copressed, so set it to 1 so as to not
    // mislead the user.
    if (!bCompression)
    {
        CString s;

        s.Format(_T("%.1f"), 1.0f);
	    m_ClipDecimationFactorEdit.SetWindowText(s);
    }

    m_MatchCaptureCheck.EnableWindow (bCompression);
    m_AutoBitRateCheck.EnableWindow (bCompression);
    m_ClipDecimationFactorEdit.EnableWindow (bCompression);

    if (!bCompression)
    {
        CString s;

        // Clip Decimpation does not apply to non-copressed, so set it to 1 so as to not
        // mislead the user.
        s.Format(_T("%.1f"), 1.0f);
	    m_ClipDecimationFactorEdit.SetWindowText(s);
        PopulateCaptureTime();

        // Also, uncompressed images are always .avi files
		m_ClipFileFormatCombo.SetCurSel(CLIP_FORMAT_AVI);
        OnSelchangeClipFormatCombo();
    }

    PopulatePlaybackRate();
    PopulatePlaybackTime();
    PopulateBitRate();
    PopulateFileSize();

    SetFileExtensionFromClipEncoding(m_RawDataFileEdit, m_EncodingCombo);
	ScrollToEnd(m_RawDataFileEdit);
}

/**
* Function: OnKillFocusClipDecimationFactorEdit
* Purpose:  
*/
void CClipsGui::OnKillFocusClipDecimationFactorEdit(void)
{
    PopulatePlaybackRate();
    PopulateCaptureTime();
    PopulatePlaybackTime();
    PopulateBitRate();
    PopulateFileSize();
}

/**
* Function: OnUpdateMatchCaptureCheck
* Purpose:  Called when m_MatchCaptureCheck is checked/unchecked
*/
void CClipsGui::OnUpdateMatchCaptureCheck(void)
{
    bool matchCapture = m_MatchCaptureCheck.GetCheck() == BST_CHECKED;

    m_PbFrameRateEdit.EnableWindow   (!matchCapture);

    if (matchCapture)
    {
        PopulatePlaybackRate();
        PopulateCaptureTime();
        PopulatePlaybackTime();
        PopulatePlaybackEffect();
    }
}

/**
* Function: OnKillFocusPbFrameRateEdit
* Purpose:  
*/
void CClipsGui::OnKillFocusPbFrameRateEdit(void)
{
	// Ensure the user specified Playback frame rate is between 0.1, and 10000 fps.  Any faster than than is not really
    // visible.
    float playbackRate = WindowTextToFloat(m_PbFrameRateEdit);

    if (playbackRate < 0.1f) playbackRate = 0.1f;
    if (playbackRate > 100.0f) playbackRate = 10000.0f;

    CString s;
    s.Format(_T("%.1f"), playbackRate);
	m_PbFrameRateEdit.SetWindowText(s);

    PopulatePlaybackTime();
    PopulatePlaybackEffect();
    PopulateFileSize();
}

/**
* Function: OnUpdateAutoBitRateCheck
* Purpose:  Called when m_AutoBitRateCheck is checked/unchecked
*/
void CClipsGui::OnUpdateAutoBitRateCheck(void)
{
    PopulateBitRate();
    PopulateFileSize();
}

/**
* Function: OnKillFocusBitRateEdit
* Purpose:  
*/
void CClipsGui::OnKillFocusBitRateEdit(void)
{
    m_BitRateSlider.SetValue(WindowTextToFloat(m_BitRateEdit));
    PopulateFileSize();
}

/**
* Function: OnScrollBitRateSlider
* Purpose:  Scroll event handler for the BitRate slider.
*/
void CClipsGui::OnScrollBitRateSlider()
{
    CString s;

	m_BitRateSlider.OnScroll();

    s.Format(_T("%.1f"), m_BitRateSlider.GetValue());
	m_BitRateEdit.SetWindowText(s);

    PopulateFileSize();
}


/**
* Function: OnChangeBitRateSlider
* Purpose:  Change event handler for the BitRate slider.
*/
void CClipsGui::OnChangeBitRateSlider()
{
    CString s;

    m_BitRateSlider.OnChange();

    s.Format(_T("%.1f"), m_BitRateSlider.GetValue());
	m_BitRateEdit.SetWindowText(s);
    PopulateFileSize();
}

void CClipsGui::OnKillFocusRawDataFileEdit(void)
{
	SetClipEncodingFromFilename(m_RawDataFileEdit, m_EncodingCombo);
	ScrollToEnd(m_RawDataFileEdit);
}

/**
* Function: OnClickRawDataBrowseButton
* Purpose:  
*/
void CClipsGui::OnClickRawDataBrowseButton(void)
{
    BrowseForFile(m_RawDataFileEdit,
				  pdsFormatFilterString,
                  GetClipEncodingExtension (m_EncodingCombo.GetCurSel()));

	ScrollToEnd(m_RawDataFileEdit);
}


/**
* Function: OnKillFocusClipFileNameEdit
* Purpose:  
*/
void CClipsGui::OnKillFocusClipFileNameEdit(void)
{
	SetClipFormatFromFilename(m_ClipFileNameEdit, m_ClipFileFormatCombo);
	ScrollToEnd(m_ClipFileNameEdit);
}


/**
* Function: OnClickFormattedBrowseButton
* Purpose:  
*/
void CClipsGui::OnClickFormattedBrowseButton(void)
{
	BrowseForFile(m_ClipFileNameEdit,
				  clipFormatFilterString,
				  GetClipFormatExtension(m_ClipFileFormatCombo.GetCurSel()));

	ScrollToEnd(m_ClipFileNameEdit);

	SetClipFormatFromFilename(m_ClipFileNameEdit, m_ClipFileFormatCombo);
}

/**
* Function: OnSelchangeClipFormatCombo
* Purpose:  
*/
void CClipsGui::OnSelchangeClipFormatCombo(void)
{
	SetFileExtensionFromClipFormat(m_ClipFileNameEdit, m_ClipFileFormatCombo);
	ScrollToEnd(m_ClipFileNameEdit);
}

/**
* Function: OnClickBeginCaptureButton
* Purpose:  
*/
void CClipsGui::OnClickBeginCaptureButton(void)
{
	CWaitCursor wc;

	bool saveRaw = m_SaveRawDataFileCheck.GetCheck() == BST_CHECKED;
	bool saveClip = m_SaveFormattedCheck.GetCheck() == BST_CHECKED;
    bool compressed = m_EncodingCombo.GetCurSel() != CLIP_ENCODING_PDS; // PDS is the only uncompressed type
    bool captureOK = false;

	try
	{
        if (compressed)
        {
            captureOK = GetDocument()->CaptureEncodedClip( m_NumFramesEdit, 
                                                           WindowTextToInt(m_ClipDecimationFactorEdit),
						  				                   saveClip ? &m_ClipFileNameEdit : NULL, 
										                   saveClip ? &m_ClipFileFormatCombo : NULL,
                                                           RequestedPlaybackFrameRate(),
                                                           (int)(WindowTextToFloat (m_BitRateEdit) * 1000.0f * 1000.0f), // Convert from Mbps to bps (library uses 1000, not 1024)
                                                           (U32)m_EncodingCombo.GetCurSel(),
										                   saveRaw ? &m_RawDataFileEdit : NULL );
        } else {
            captureOK = GetDocument()->CaptureClip( m_NumFramesEdit, 
						  				            saveClip ? &m_ClipFileNameEdit : NULL, 
										            saveClip ? &m_ClipFileFormatCombo : NULL,
										            saveRaw ? &m_RawDataFileEdit : NULL );
        }
    }
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
    try
    {
        if (captureOK)
		{
			if (saveClip && m_ViewAfterCaptureCheck.GetCheck() == BST_CHECKED)
				SysOpenFile(WindowTextAsString(m_ClipFileNameEdit));
			if (saveClip && m_IncrementFormattedFileCheck.GetCheck() == BST_CHECKED)
				IncrementFileName(m_ClipFileNameEdit);
			if (saveRaw && m_IncrementRawDataFileCheck.GetCheck() == BST_CHECKED)
				IncrementFileName(m_RawDataFileEdit);
		}
	}
	catch (PxLException const& e)
	{
		OnApiError(e, false);
	}
}


/******************************************************************************
* UPDATE_COMMAND_UI handlers.
******************************************************************************/

/**
* Function: OnUpdateRawDataFileEdit
* Purpose:  
*/
void CClipsGui::OnUpdateRawDataFileEdit(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->HasActiveCamera() 
					&& m_SaveRawDataFileCheck.GetCheck() == BST_CHECKED);
}


/**
* Function: OnUpdateRawDataFileBrowseButton
* Purpose:  
*/
void CClipsGui::OnUpdateRawDataFileBrowseButton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->HasActiveCamera() 
					&& m_SaveRawDataFileCheck.GetCheck() == BST_CHECKED);
}


/**
* Function: OnUpdateIncrementRawDataFileCheck
* Purpose:  
*/
void CClipsGui::OnUpdateIncrementRawDataFileCheck(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->HasActiveCamera() 
					&& m_SaveRawDataFileCheck.GetCheck() == BST_CHECKED);
}


/**
* Function: OnUpdateClipFormatCombo
* Purpose:  
*/
void CClipsGui::OnUpdateClipFormatCombo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->HasActiveCamera() 
					&& m_SaveFormattedCheck.GetCheck() == BST_CHECKED);
}


/**
* Function: OnUpdateFileFormatCombo
* Purpose:  
*/
void CClipsGui::OnUpdateFileFormatCombo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->HasActiveCamera() 
					&& m_SaveFormattedCheck.GetCheck() == BST_CHECKED);
}


/**
* Function: OnUpdateFormattedFileEdit
* Purpose:  
*/
void CClipsGui::OnUpdateFormattedFileEdit(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->HasActiveCamera() 
					&& m_SaveFormattedCheck.GetCheck() == BST_CHECKED);
}


/**
* Function: OnUpdateIncrementFormattedFileNameCheck
* Purpose:  
*/
void CClipsGui::OnUpdateIncrementFormattedFileNameCheck(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->HasActiveCamera() 
					&& m_SaveFormattedCheck.GetCheck() == BST_CHECKED);
}


/**
* Function: OnUpdateViewAfterCaptureCheck
* Purpose:  
*/
void CClipsGui::OnUpdateViewAfterCaptureCheck(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->HasActiveCamera() 
					&& m_SaveFormattedCheck.GetCheck() == BST_CHECKED);
}


/**
* Function: OnUpdateBeginCaptureButton
* Purpose:  
*/
void CClipsGui::OnUpdateBeginCaptureButton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->HasActiveCamera()
					&& ( m_SaveRawDataFileCheck.GetCheck() == BST_CHECKED
						|| m_SaveFormattedCheck.GetCheck() == BST_CHECKED));
}














