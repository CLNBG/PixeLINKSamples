//
// RoiGui.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "RoiGui.h"
#include "Helpers.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/******************************************************************************
 * CRoiGui - Member functions.
 *****************************************************************************/

IMPLEMENT_DYNCREATE(CRoiGui, CDevAppPage)

CRoiGui::CRoiGui(CPxLDevAppView* pView)
	: CDevAppPage(CRoiGui::IDD, pView)
	, m_ControlledFeature(FEATURE_ROI)
	, m_bRoiConfigured(false)
{
	//{{AFX_DATA_INIT(CRoiGui)
	//}}AFX_DATA_INIT
}

CRoiGui::~CRoiGui()
{
}

void CRoiGui::DoDataExchange(CDataExchange* pDX)
{
	CDevAppPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRoiGui)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_RG_CAMERASELECT_COMBO, m_CameraSelectCombo);
	DDX_Control(pDX, IDC_RG_PLAY_BUTTON, m_PlayButton);
	DDX_Control(pDX, IDC_RG_PAUSE_BUTTON, m_PauseButton);
	DDX_Control(pDX, IDC_RG_STOP_BUTTON, m_StopButton);

	DDX_Control(pDX, IDC_RG_APPLYTOROI_RADIO, m_ApplyToRoiRadio);
	DDX_Control(pDX, IDC_RG_APPLYTOAUTOROI_RADIO, m_ApplyToAutoRoiRadio);
	DDX_Control(pDX, IDC_RG_AUTOROIONOFF_CHECK, m_AutoRoiEnableCheck);

	DDX_Control(pDX, IDC_RG_SUBWINDOW_CTRL, m_SubwindowCtrl);
	DDX_Control(pDX, IDC_RG_WINDOWTOP_EDIT, m_TopEdit);
	DDX_Control(pDX, IDC_RG_WINDOWLEFT_EDIT, m_LeftEdit);
	DDX_Control(pDX, IDC_RG_WINDOWHEIGHT_EDIT, m_HeightEdit);
	DDX_Control(pDX, IDC_RG_WINDOWWIDTH_EDIT, m_WidthEdit);
	DDX_Control(pDX, IDC_RG_DECIMATION_COMBO, m_DecimationCombo);
	DDX_Control(pDX, IDC_RG_DECIMATIONMODE_COMBO, m_DecimationModeCombo);
	DDX_Control(pDX, IDC_RG_WINDOWSIZE_COMBO, m_WindowSizeCombo);
	DDX_Control(pDX, IDC_RG_CENTER_BUTTON, m_CenterButton);

	DDX_Control(pDX, IDC_RG_ROTATE0_RADIO, m_RotateNoneRadio);
	DDX_Control(pDX, IDC_RG_ROTATE90_RADIO, m_Rotate90Radio);
	DDX_Control(pDX, IDC_RG_ROTATE180_RADIO, m_Rotate180Radio);
	DDX_Control(pDX, IDC_RG_ROTATE270_RADIO, m_Rotate270Radio);

   DDX_Control(pDX, IDC_RG_ONCAMERAFLIP_STATIC, m_OnCameraFlipStatic);
   DDX_Control(pDX, IDC_RG_IMAGEFLIPHORIZONTALOFF_RADIO, m_ImageFlipHorizontalOffRadio);
	DDX_Control(pDX, IDC_RG_IMAGEFLIPHORIZONTALON_RADIO, m_ImageFlipHorizontalOnRadio);
	DDX_Control(pDX, IDC_RG_IMAGEFLIPVERTICALOFF_RADIO, m_ImageFlipVerticalOffRadio);
	DDX_Control(pDX, IDC_RG_IMAGEFLIPVERTICALON_RADIO, m_ImageFlipVerticalOnRadio);

	DDX_Control(pDX, IDC_RG_PIXELFORMAT_COMBO, m_PixelFormatCombo);
	DDX_Control(pDX, IDC_RG_PIXELFORMATINTERPRETATION_COMBO, m_PixelFormatInterpretationCombo);
	DDX_Control(pDX, IDC_RG_ALPHA_EDIT, m_AlphaEdit);
	
}


BEGIN_MESSAGE_MAP(CRoiGui, CDevAppPage)

	ON_CBN_DROPDOWN(IDC_RG_CAMERASELECT_COMBO, OnDropdownCameraSelectCombo)
	ON_CBN_SELCHANGE(IDC_RG_CAMERASELECT_COMBO, OnSelchangeCameraSelectCombo)
	ON_BN_CLICKED(IDC_RG_PLAY_BUTTON, OnClickPlayButton)
	ON_BN_CLICKED(IDC_RG_PAUSE_BUTTON, OnClickPauseButton)
	ON_BN_CLICKED(IDC_RG_STOP_BUTTON, OnClickStopButton)

	ON_BN_CLICKED(IDC_RG_APPLYTOAUTOROI_RADIO, OnClickApplyToAutoRoiRadio)
	ON_BN_CLICKED(IDC_RG_APPLYTOROI_RADIO, OnClickApplyToRoiRadio)
	ON_BN_CLICKED(IDC_RG_AUTOROIONOFF_CHECK, OnClickAutoRoiEnableCheck)

	ON_EN_KILLFOCUS(IDC_RG_WINDOWTOP_EDIT, OnKillFocusSubwindowTopEdit)
	ON_EN_KILLFOCUS(IDC_RG_WINDOWLEFT_EDIT, OnKillFocusSubwindowLeftEdit)
	ON_EN_KILLFOCUS(IDC_RG_WINDOWWIDTH_EDIT, OnKillFocusSubwindowWidthEdit)
	ON_EN_KILLFOCUS(IDC_RG_WINDOWHEIGHT_EDIT, OnKillFocusSubwindowHeightEdit)

	ON_EN_SETFOCUS(IDC_RG_WINDOWLEFT_EDIT, OnSetFocusWindowLeftEdit)
	ON_EN_SETFOCUS(IDC_RG_WINDOWTOP_EDIT, OnSetFocusWindowTopEdit)
	ON_EN_SETFOCUS(IDC_RG_WINDOWWIDTH_EDIT, OnSetFocusWindowWidthEdit)
	ON_EN_SETFOCUS(IDC_RG_WINDOWHEIGHT_EDIT, OnSetFocusWindowHeightEdit)

	ON_BN_CLICKED(IDC_RG_CENTER_BUTTON, OnClickCenterButton)

	ON_CBN_SELCHANGE(IDC_RG_DECIMATION_COMBO, OnSelchangeDecimationCombo)
	ON_CBN_SELCHANGE(IDC_RG_DECIMATIONMODE_COMBO, OnSelchangeDecimationModeCombo)
	ON_CBN_SELCHANGE(IDC_RG_WINDOWSIZE_COMBO, OnSelchangeWindowsizeCombo)

	ON_UPDATE_COMMAND_UI(IDC_RG_DECIMATIONMODE_COMBO, OnUpdateDecimationModeCombo)

	ON_BN_CLICKED(IDC_RG_ROTATE0_RADIO, OnClickRotateNoneRadio)
	ON_BN_CLICKED(IDC_RG_ROTATE90_RADIO, OnClickRotate90Radio)
	ON_BN_CLICKED(IDC_RG_ROTATE180_RADIO, OnClickRotate180Radio)
	ON_BN_CLICKED(IDC_RG_ROTATE270_RADIO, OnClickRotate270Radio)

	ON_BN_CLICKED(IDC_RG_IMAGEFLIPHORIZONTALOFF_RADIO, OnClickHorizontalOff)
	ON_BN_CLICKED(IDC_RG_IMAGEFLIPHORIZONTALON_RADIO, OnClickHorizontalOn)
	ON_BN_CLICKED(IDC_RG_IMAGEFLIPVERTICALOFF_RADIO, OnClickVerticalOff)
	ON_BN_CLICKED(IDC_RG_IMAGEFLIPVERTICALON_RADIO, OnClickVerticalOn)

	ON_CBN_SELCHANGE(IDC_RG_PIXELFORMAT_COMBO, OnSelchangePixelFormatCombo)
	ON_CBN_SELCHANGE(IDC_RG_PIXELFORMATINTERPRETATION_COMBO, OnSelchangePixelFormatInterpretationCombo)
	ON_EN_KILLFOCUS(IDC_RG_ALPHA_EDIT, OnKillFocusAlphaEdit)
END_MESSAGE_MAP()


BEGIN_EVENTSINK_MAP(CRoiGui, CDevAppPage)
	ON_EVENT(CRoiGui, IDC_RG_SUBWINDOW_CTRL, 1, OnSubwindowChanged, VTS_I4 VTS_I4 VTS_I4 VTS_I4)
END_EVENTSINK_MAP()


/**
* Function: GetHelpContextId
* Purpose:  
*/
int CRoiGui::GetHelpContextId()
{
	return IDH_ROI_TAB;
}


/**
* Function: OnInitDialog
* Purpose:  
*/
BOOL CRoiGui::OnInitDialog()
{
	CDevAppPage::OnInitDialog();

	// The PxLSubwindow control is able to semi-automatically exchange data
	// with up to four edit controls  - we just need to tell it which ones:
	m_SubwindowCtrl.SetEdits(&m_LeftEdit, &m_TopEdit, &m_WidthEdit, &m_HeightEdit);

	return TRUE;  // return TRUE unless you set the focus to a control
}


/**
* Function: OnSetActive
* Purpose:  
*/
BOOL CRoiGui::OnSetActive()
{
	m_bRoiConfigured = false;
	return CDevAppPage::OnSetActive();
}


/**
* Function: SetGUIState
* Purpose:  Enable / Disable controls, according to the state of the camera.
*           Most controls cannot be used in any meaningful way until a camera
*           is initialized.
*/
void CRoiGui::SetGUIState(eGUIState state)
{
	// The CameraSelect and Play/Pause/Stop button states are set in the base
	// class version of this function.
	CDevAppPage::SetGUIState(state);

	CPxLCamera* cam = GetActiveCamera();

	bool enable = (state != GS_NoCamera);

	m_ApplyToRoiRadio.EnableWindow(enable && cam->FeatureSupported(FEATURE_ROI));
	m_ApplyToAutoRoiRadio.EnableWindow(enable && 
                                      cam->FeatureSupported(FEATURE_AUTO_ROI) &&
                                      !cam->FeatureEmulated(FEATURE_AUTO_ROI));
	m_AutoRoiEnableCheck.EnableWindow(enable && 
                                      cam->FeatureSupported(FEATURE_AUTO_ROI) &&
                                      m_ApplyToAutoRoiRadio.GetCheck());

	bool enableRoi = enable && cam->FeatureSupportsManual(m_ControlledFeature); // Can be ROI or AUTO_ROI
	m_SubwindowCtrl.put_Enabled(enableRoi);
	m_TopEdit.EnableWindow(enableRoi);
	m_LeftEdit.EnableWindow(enableRoi);
	m_WidthEdit.EnableWindow(enableRoi);
	m_HeightEdit.EnableWindow(enableRoi);
	m_WindowSizeCombo.EnableWindow(enableRoi);
	m_CenterButton.EnableWindow(enableRoi);

	m_DecimationCombo.EnableWindow(enable && cam->FeatureSupportsManual(FEATURE_PIXEL_ADDRESSING));
	m_DecimationModeCombo.EnableWindow(enable  
									&& cam->FeatureSupportsManual(FEATURE_PIXEL_ADDRESSING)
									&& cam->GetFeatureParamCount(FEATURE_PIXEL_ADDRESSING) >= 2);

	bool enableRot = enable && cam->FeatureSupportsManual(FEATURE_ROTATE);
	m_RotateNoneRadio.EnableWindow(enableRot);
	m_Rotate90Radio.EnableWindow(enableRot);
	m_Rotate180Radio.EnableWindow(enableRot);
	m_Rotate270Radio.EnableWindow(enableRot);

	bool enableFeature = enable 
					&& cam->FeatureSupportsManual(FEATURE_FLIP)
					&& static_cast<int>(cam->GetFeatureMaxVal(FEATURE_FLIP, FEATURE_FLIP_PARAM_HORIZONTAL)) >= 1;
	m_ImageFlipHorizontalOffRadio.EnableWindow(enableFeature);
	m_ImageFlipHorizontalOnRadio.EnableWindow(enableFeature);

	enableFeature = enable 
					&& cam->FeatureSupportsManual(FEATURE_FLIP)
					&& static_cast<int>(cam->GetFeatureMaxVal(FEATURE_FLIP, FEATURE_FLIP_PARAM_VERTICAL)) >= 1;
	m_ImageFlipVerticalOffRadio.EnableWindow(enableFeature);
	m_ImageFlipVerticalOnRadio.EnableWindow(enableFeature);

	enableFeature = enable && cam->FeatureSupportsManual(FEATURE_PIXEL_FORMAT);
	m_PixelFormatCombo.EnableWindow(enableFeature);
    // Only enable the pixel format interpretations dialog if the camera is using the HSV pixel format.  Also, 
    // only enable the Aplha value control if we are using a pixel format that supports alpha
    bool enableInterpretation = false;
    bool enableAplhaValue = false;
    if (enableFeature)
    {
        float currentFormat = cam->GetFeatureValue(FEATURE_PIXEL_FORMAT);
        enableInterpretation = PIXEL_FORMAT_HSV4_12 == currentFormat;
        enableAplhaValue = currentFormat >= PIXEL_FORMAT_RGBA && currentFormat <= PIXEL_FORMAT_ABGR;
    }
	m_PixelFormatInterpretationCombo.EnableWindow(enableInterpretation);
    m_AlphaEdit.EnableWindow(enableAplhaValue);

}


/**
* Function: ConfigureControls
* Purpose:  Set up the controls on the page to customize them according to the
*           currently active camera. This involves things like setting the max
*           and min values for sliders and setting the bounds of the subwindow
*           control.
*/
void CRoiGui::ConfigureControls(void)
{
    if (m_ControlledFeature == FEATURE_AUTO_ROI)
    {
        m_ApplyToRoiRadio.SetCheck(BST_UNCHECKED);
        m_ApplyToAutoRoiRadio.SetCheck(BST_CHECKED);
    } else {
        m_ApplyToRoiRadio.SetCheck(BST_CHECKED);
        m_ApplyToAutoRoiRadio.SetCheck(BST_UNCHECKED);
    }
    m_AutoRoiEnableCheck.EnableWindow (m_ApplyToAutoRoiRadio.GetCheck());
    m_AutoRoiEnableCheck.SetCheck (GetActiveCamera()->FeatureEnabled(FEATURE_AUTO_ROI));

	ConfigureRoi();
	ConfigureDecimation();

	SetPixelFormatEntries();
	SetPixelFormatInterpretationEntries();
	
}


/**
* Function: ConfigureRoi
* Purpose:  
*/
void CRoiGui::ConfigureRoi(void)
{
	if (!GetActiveCamera()->FeatureSupported(m_ControlledFeature))
		return;

	try
	{
		long maxWidth = static_cast<long>(
			GetActiveCamera()->GetFeatureMaxVal(m_ControlledFeature, FEATURE_ROI_PARAM_WIDTH));
		long maxHeight = static_cast<long>(
			GetActiveCamera()->GetFeatureMaxVal(m_ControlledFeature, FEATURE_ROI_PARAM_HEIGHT));

		// We want the SubwindowControl to work in an intuitive way, even in the
		// presence of Flip and Rotation.
		bool sideways = GetActiveCamera()->FeatureSupported(FEATURE_ROTATE)
					&& static_cast<int>(GetActiveCamera()->GetFeatureValue(FEATURE_ROTATE)) % 180 != 0;
		if (sideways)
			std::swap(maxWidth, maxHeight);

		TempVal<bool> temp(m_bAdjusting[m_ControlledFeature], true); // SetBounds fires a SubwindowChanged event.

		m_SubwindowCtrl.SetBounds(maxWidth, maxHeight);

		long minWidth = static_cast<long>(
			GetActiveCamera()->GetFeatureMinVal(m_ControlledFeature, FEATURE_ROI_PARAM_WIDTH));
		long minHeight = static_cast<long>(
			GetActiveCamera()->GetFeatureMinVal(m_ControlledFeature, FEATURE_ROI_PARAM_HEIGHT));

		if (sideways)
			std::swap(minWidth, minHeight);

		//minWidth = max(minWidth, 24);
		//minHeight = max(minHeight, 24);
		m_SubwindowCtrl.put_MinWidth(minWidth);
		m_SubwindowCtrl.put_MinHeight(minHeight);

		// Limit the list of standard subwindow sizes to those that are
		// within the capabilities of the camera.
		SetWindowSizeEntries(
			m_WindowSizeCombo,
			static_cast<int>(minWidth),
			static_cast<int>(minHeight),
			static_cast<int>(maxWidth), 
			static_cast<int>(maxHeight));

		m_bRoiConfigured = true;
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: ConfigureDecimation
* Purpose:  
*/
void CRoiGui::ConfigureDecimation(void)
{
	try
	{
		SetDecimationModeEntries(m_DecimationModeCombo, GetActiveCamera()->GetSupportedDecimationModeValues());
		SetDecimationEntries(m_DecimationCombo, 
		                     GetActiveCamera()->GetSupportedXDecimationValues(),
		                     GetActiveCamera()->GetSupportedYDecimationValues(),
		                     GetActiveCamera()->SupportsAsymmetricPixelAddressing());
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
void CRoiGui::PopulateDefaults(void)
{
    m_ApplyToRoiRadio.SetCheck(BST_CHECKED);

	{
		TempVal<bool> _temp(m_bAdjusting[m_ControlledFeature], true);
		m_SubwindowCtrl.SetBounds(DEFAULT_FOV_WIDTH,DEFAULT_FOV_HEIGHT);
		m_SubwindowCtrl.SetSubwindow(DEFAULT_FOV_WIDTH/4, DEFAULT_FOV_HEIGHT/4, DEFAULT_FOV_WIDTH/2, DEFAULT_FOV_HEIGHT/2);
		m_SubwindowCtrl.UpdateEdits();
	}

	m_DecimationModeCombo.SetCurSel(-1);
	m_DecimationCombo.SetCurSel(-1);

	m_RotateNoneRadio.SetCheck(BST_CHECKED);

   m_OnCameraFlipStatic.ShowWindow(SW_HIDE);
   m_ImageFlipHorizontalOffRadio.SetCheck(BST_CHECKED);
	m_ImageFlipVerticalOffRadio.SetCheck(BST_CHECKED);

	m_PixelFormatCombo.SetCurSel(-1);
	m_PixelFormatInterpretationCombo.SetCurSel(-1);
}


/**
* Function: PopulateControls
* Purpose:  Make the values displayed in the controls reflect the current 
*           settings of the active camera.  Called immediately after a 
*           camera is initialized.
*/
void CRoiGui::PopulateControls(void)
{
	CDevAppPage::PopulateControls();

	PopulateSubwindow();
	PopulateDecimation();
	PopulateWindowSize(m_WindowSizeCombo, true, m_ControlledFeature);
	PopulateRotation();
	PopulateFlip();
	PopulatePixelFormat();
}


/**
* Function: PopulateSubwindow
* Purpose:  Set the subwindow control to reflect the current value of the ROI
*           in the active camera.
*/
void CRoiGui::PopulateSubwindow(void)
{
	if (GetActiveCamera()->FeatureSupported(m_ControlledFeature))
	{
		try
		{
			if (!m_bRoiConfigured)
				ConfigureRoi();

			CRect r = GetActiveCamera()->GetRoi(m_ControlledFeature);

			// Set the subwindow control to display the correct subwindow.
			TempVal<bool> temp(m_bAdjusting[m_ControlledFeature], true); // SetSubwindow *always* emits an OnChange event.
			m_SubwindowCtrl.SetSubwindow(
									r.left,
									r.top,
									r.right - r.left,		// width
									r.bottom - r.top);		// height
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


/**
* Function: PopulateDecimation
* Purpose:  Set the decimation radio buttons to reflect the state of the 
*           active camera.
*/
void CRoiGui::PopulateDecimation(void)
{
	if (GetActiveCamera()->FeatureSupported(FEATURE_PIXEL_ADDRESSING))
	{
		try
		{
			int selector = 1;
			PixelAddressing pa = GetActiveCamera()->GetPixelAddressing();
    	    std::vector<int> const& xPas = GetActiveCamera()->GetSupportedXDecimationValues();
			
			int x,y;
			if (pa.supportAsymmetry)
			{
    	        std::vector<int> const& yPas = GetActiveCamera()->GetSupportedYDecimationValues();
    	        // covert the x and y pa factor, into offsets into the supported pa vectors
    	        for (y=0; y<yPas.size(); y++)
    	            if (yPas[y] == pa.y) break;
    	        for (x=0; x<xPas.size(); x++)
    	            if (xPas[x] == pa.x) break;
                selector = x*yPas.size() + y + 1;
			} else {
    	        // covert the x pa factor, into offsets into the supported pa vector
    	        for (x=0; x<xPas.size(); x++)
    	            if (xPas[x] == pa.x) break;
			    selector = x + 1;
			}

            TempVal<bool> temp(m_bAdjusting[FEATURE_PIXEL_ADDRESSING], true);
			SelectByItemData(m_DecimationCombo, selector);
			// If the camera knows about Decimation Mode, populate the combo box:
			if (GetActiveCamera()->GetFeatureParamCount(FEATURE_PIXEL_ADDRESSING) >= 2)
			{
				int mode = static_cast<int>(GetActiveCamera()->GetFeatureValue(FEATURE_PIXEL_ADDRESSING, NULL, 1));
				SelectByItemData(m_DecimationModeCombo, mode);
			}
			else
			{
				m_DecimationModeCombo.SetCurSel(0);
			}
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


/**
* Function: PopulateRotation
* Purpose:  
*/
void CRoiGui::PopulateRotation(void)
{
	if (GetActiveCamera()->FeatureSupported(FEATURE_ROTATE))
	{
		try
		{
			U32 flags = 0;
			float rot = GetActiveCamera()->GetFeatureValue(FEATURE_ROTATE, &flags);

			TempVal<bool> _temp(m_bAdjusting[FEATURE_ROTATE], true);

			m_RotateNoneRadio.SetCheck(BST_UNCHECKED);
			m_Rotate90Radio.SetCheck(BST_UNCHECKED);
			m_Rotate180Radio.SetCheck(BST_UNCHECKED);
			m_Rotate270Radio.SetCheck(BST_UNCHECKED);

			if (rot == 0.0f || (flags & FEATURE_FLAG_OFF) != 0)
				m_RotateNoneRadio.SetCheck(BST_CHECKED);
			else if (rot == 90.0f)
				m_Rotate90Radio.SetCheck(BST_CHECKED);
			else if (rot == 180.0f)
				m_Rotate180Radio.SetCheck(BST_CHECKED);
			else if (rot == 270.0f)
				m_Rotate270Radio.SetCheck(BST_CHECKED);
			// else ???
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


/**
* Function: PopulateFlip
* Purpose:  
*/
void CRoiGui::PopulateFlip(void)
{
	try
	{
		if (GetActiveCamera()->FeatureSupported(FEATURE_FLIP))
		{
			bool onCameraFlipping = ! GetActiveCamera()->FeatureEmulated(FEATURE_FLIP);
         m_OnCameraFlipStatic.ShowWindow(onCameraFlipping ? SW_SHOW : SW_HIDE);

         bool hFlipOn = GetActiveCamera()->GetHorizontalFlip();
			m_ImageFlipHorizontalOnRadio.SetCheck(hFlipOn);
			m_ImageFlipHorizontalOffRadio.SetCheck(!hFlipOn);

			bool vFlipOn = GetActiveCamera()->GetVerticalFlip();
			m_ImageFlipVerticalOnRadio.SetCheck(vFlipOn);
			m_ImageFlipVerticalOffRadio.SetCheck(!vFlipOn);
		}
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
void CRoiGui::PopulatePixelFormat(void)
{
    // The pixel format combo box has already been configured - the entries that it contains
	// at this point should be precisely the formats that the camera supports.
	if (m_PixelFormatCombo.GetCount() == 0)
	{
		m_PixelFormatCombo.EnableWindow(FALSE);
		m_PixelFormatInterpretationCombo.EnableWindow(FALSE);
      m_AlphaEdit.EnableWindow(FALSE);
      return;
	}

	try
	{
		if (GetActiveCamera()->FeatureSupported(FEATURE_PIXEL_FORMAT))
		{
			int fmt = static_cast<int>(GetActiveCamera()->GetFeatureValue(FEATURE_PIXEL_FORMAT));

			// Pixel Format is one of the features that is a bit of a special case.
			// It behaves as follows when dealing with BAYER formats:
			//   - The four specific bayer formats are interchangeable for the purposes
			//     of calling PxLSetFeature - you can use any of them.
			//   - PxLGetFeature will always report the actual, specific bayer pattern
			//     the is being output (eg: PIXEL_FORMAT_BAYER8_RGGB)
			// We only put one (non-specific) bayer format in the combo box, and we need
			// to show this as the current format, even if GetFeature returned a different
			// value.
			if (fmt == PIXEL_FORMAT_BAYER8_BGGR
					|| fmt == PIXEL_FORMAT_BAYER8_GBRG
					|| fmt == PIXEL_FORMAT_BAYER8_RGGB)
				fmt = PIXEL_FORMAT_BAYER8;
			else if (fmt == PIXEL_FORMAT_BAYER16_BGGR
					|| fmt == PIXEL_FORMAT_BAYER16_GBRG
					|| fmt == PIXEL_FORMAT_BAYER16_RGGB)
				fmt = PIXEL_FORMAT_BAYER16;
			else if (fmt == PIXEL_FORMAT_BAYER12_BGGR_PACKED
					|| fmt == PIXEL_FORMAT_BAYER12_GBRG_PACKED
					|| fmt == PIXEL_FORMAT_BAYER12_RGGB_PACKED)
				fmt = PIXEL_FORMAT_BAYER12_PACKED;
			else if (fmt == PIXEL_FORMAT_BAYER12_BGGR_PACKED_MSFIRST
					|| fmt == PIXEL_FORMAT_BAYER12_GBRG_PACKED_MSFIRST
					|| fmt == PIXEL_FORMAT_BAYER12_RGGB_PACKED_MSFIRST)
				fmt = PIXEL_FORMAT_BAYER12_PACKED_MSFIRST;
			else if (fmt == PIXEL_FORMAT_BAYER10_BGGR_PACKED_MSFIRST
					|| fmt == PIXEL_FORMAT_BAYER10_GBRG_PACKED_MSFIRST
					|| fmt == PIXEL_FORMAT_BAYER10_RGGB_PACKED_MSFIRST)
				fmt = PIXEL_FORMAT_BAYER10_PACKED_MSFIRST;

			LOCK_FEATURE(PIXEL_FORMAT);
			SelectByItemData(m_PixelFormatCombo, fmt);
            SetPixelFormatInterpretationEntries();
            if (fmt == PIXEL_FORMAT_HSV4_12)
            {
                int HSVIntpretation = static_cast<int>(GetActiveCamera()->GetFeatureValue(FEATURE_POLAR_HSV_INTERPRETATION));
    			SelectByItemData(m_PixelFormatInterpretationCombo, HSVIntpretation);
            }

            // Only read the alpha value, if the camera supports it (the window is enabledif it does)
            if (m_AlphaEdit.IsWindowEnabled())
            {
                U32 flags = 0;
                U32 alpha = (U32) GetActiveCamera()->GetFeatureValue (FEATURE_PIXEL_FORMAT, &flags, 1);
		        SetIntText(m_AlphaEdit, alpha);
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
*           supported by the current camera.
*/
void CRoiGui::SetPixelFormatEntries(void)
{
	std::vector<int> const& formats = GetActiveCamera()->GetSupportedPixelFormats();
	ClearComboBox(m_PixelFormatCombo);

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
void CRoiGui::SetPixelFormatInterpretationEntries(void)
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
/**
* Function: FeatureChanged
* Purpose:  This method is called by the base class (CDevAppPage) as a result
*           of a call to the View class's OnUpdate method (which is called by
*           the Document class as a result of a change to its data).
*/
void CRoiGui::FeatureChanged(U32 featureId)
{
	switch (featureId)
	{
	case FEATURE_ROI:
		if (m_ControlledFeature == FEATURE_ROI)
		{
			PopulateSubwindow();
			PopulateWindowSize(m_WindowSizeCombo);
		}
		break;
	case FEATURE_AUTO_ROI:
		if (m_ControlledFeature == FEATURE_AUTO_ROI)
		{
			PopulateSubwindow();
			PopulateWindowSize(m_WindowSizeCombo, true, m_ControlledFeature);
		}
		break;
	case FEATURE_PIXEL_ADDRESSING:
    	PopulateDecimation();
		break;
	case FEATURE_ROTATE:
		m_bRoiConfigured = false;
		PopulateRotation();
		PopulateSubwindow();
		PopulateWindowSize(m_WindowSizeCombo, true, m_ControlledFeature);
		break;
	case FEATURE_FLIP:
		PopulateFlip();
		PopulateSubwindow();
		PopulateWindowSize(m_WindowSizeCombo, true, m_ControlledFeature);
		break;
	case FEATURE_PIXEL_FORMAT:
		PopulatePixelFormat();
		break;
	case FEATURE_SPECIAL_CAMERA_MODE:
	    ConfigureRoi();
	    PopulateWindowSize(m_WindowSizeCombo, true, m_ControlledFeature);
	    break;
	default:
		// A feature that is not relevant to this page. Ignore the change.
		break;
	}
}


/**
* Function: SetControlledFeature
* Purpose:  
*/
void CRoiGui::SetControlledFeature(U32 featureId)
{
	if (featureId == m_ControlledFeature)
		return;

	m_ControlledFeature = featureId;
	this->ConfigureControls();
	this->PopulateControls();
}


/******************************************************************************
 * CRoiGui message handlers
 *****************************************************************************/

/**
* Function: OnClickApplyToRoiRadio
* Purpose:  
*/
void CRoiGui::OnClickApplyToRoiRadio(void)
{
	SetControlledFeature(FEATURE_ROI);
}


/**
* Function: OnClickApplyToAutoRoiRadio
* Purpose:  
*/
void CRoiGui::OnClickApplyToAutoRoiRadio(void)
{
	SetControlledFeature(FEATURE_AUTO_ROI);
}

/**
* Function: OnClickAutoRoiEnableButton
* Purpose:  
*/
void CRoiGui::OnClickAutoRoiEnableCheck(void)
{
	if (GetActiveCamera()->FeatureSupported(FEATURE_AUTO_ROI))
	{
		try
		{
            bool disabling = ! m_AutoRoiEnableCheck.GetCheck ();

            CRect r = GetActiveCamera()->GetRoi(FEATURE_AUTO_ROI);
            GetActiveCamera()->SetRoi(r.left, r.top, r.Width(), r.Height(), false, FEATURE_AUTO_ROI, disabling);
        }
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


/**
* Function: OnSubwindowChanged
* Purpose:  Event handler for the SubwindowChanged event, which is emitted by
*           the PxLSubwindow control at the completion of a move or resize
*           operation.
*/
void CRoiGui::OnSubwindowChanged(long left, long top, long width, long height)
{
	// Make sure the edits are set properly (they should be - OnSubwindowChanging updates them)
	m_SubwindowCtrl.UpdateEdits(left, top, width, height);

	// Update the camera.
	if (!m_bAdjusting[m_ControlledFeature])
	{
		try
		{
          bool disable = false;

          if (m_ControlledFeature == FEATURE_AUTO_ROI && !m_AutoRoiEnableCheck.GetCheck()) disable = true;
          GetActiveCamera()->SetRoi(left, top, width, height,
									  theApp.m_bAutoResetPreview, m_ControlledFeature, disable);
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


/**
* Function: OnKillFocusSubwindowTopEdit
* Purpose:  
*/
void CRoiGui::OnKillFocusSubwindowTopEdit()
{
	m_SubwindowCtrl.UpdateFromEdits(CSubwindowCtrl::WND_TOP);
}


/**
* Function: OnKillFocusSubwindowLeftEdit
* Purpose:  
*/
void CRoiGui::OnKillFocusSubwindowLeftEdit()
{
	m_SubwindowCtrl.UpdateFromEdits(CSubwindowCtrl::WND_LEFT);
}


/**
* Function: OnKillFocusSubwindowWidthEdit
* Purpose:  
*/
void CRoiGui::OnKillFocusSubwindowWidthEdit()
{
	m_SubwindowCtrl.UpdateFromEdits(CSubwindowCtrl::WND_WIDTH);
}


/**
* Function: OnKillFocusSubwindowHeightEdit
* Purpose:  
*/
void CRoiGui::OnKillFocusSubwindowHeightEdit()
{
	m_SubwindowCtrl.UpdateFromEdits(CSubwindowCtrl::WND_HEIGHT);
}


void CRoiGui::OnSetFocusWindowLeftEdit()
{
	m_LeftEdit.SetSel(0, 99);
}

void CRoiGui::OnSetFocusWindowTopEdit()
{
	m_TopEdit.SetSel(0, 99);
}

void CRoiGui::OnSetFocusWindowWidthEdit()
{
	m_WidthEdit.SetSel(0, 99);
}

void CRoiGui::OnSetFocusWindowHeightEdit()
{
	m_HeightEdit.SetSel(0, 99);
}


/**
* Function: OnClickCenterButton
* Purpose:  Center the Region of Interest in the available space, without
*           changing its size.
*/
void CRoiGui::OnClickCenterButton(void)
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
* Function: OnSelchangeDecimationModeCombo
* Purpose:  
*/
void CRoiGui::OnSelchangeDecimationModeCombo(void)
{
	if (m_DecimationModeCombo.GetCurSel() == -1)
		return;

	try
	{
		GetActiveCamera()->SetFeatureValue(FEATURE_PIXEL_ADDRESSING,
										   ItemData<float>(m_DecimationModeCombo),
										   FEATURE_PIXEL_ADDRESSING_PARAM_MODE,
										   theApp.m_bAutoResetPreview);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnSelchangeDecimationCombo
* Purpose:  
*/
void CRoiGui::OnSelchangeDecimationCombo(void)
{
	if (m_DecimationCombo.GetCurSel() == -1)
		return;

	try
	{
	    PixelAddressing pa = GetActiveCamera()->GetPixelAddressing();
	    int selector = ItemData<int>(m_DecimationCombo);
	    std::vector<int> const& xPas = GetActiveCamera()->GetSupportedXDecimationValues();
	    
	    if (pa.supportAsymmetry) 
	    {
    	    std::vector<int> const& yPas = GetActiveCamera()->GetSupportedYDecimationValues();
    	    int yOffset = ((selector-1) / yPas.size());
            pa.x = xPas[yOffset];
            pa.y = yPas[(selector-1) - (yOffset * yPas.size())];
	    } else {
	        pa.x = pa.y = xPas[selector-1];
	    }
		GetActiveCamera()->SetPixelAddressing(pa, 0, theApp.m_bAutoResetPreview);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnSelchangeWindowsizeCombo
* Purpose:  Event handler for the Change event of the "Standard Sizes" combo
*           box. Sets the PxLSubwindow control to the specified size.
*/
void CRoiGui::OnSelchangeWindowsizeCombo(void)
{
   bool disable = false;

   if (m_ControlledFeature == FEATURE_AUTO_ROI && !m_AutoRoiEnableCheck.GetCheck()) disable = true;
   WindowsizeComboChanged(m_WindowSizeCombo, m_ControlledFeature, disable);
}


/**
* Function: OnClickRotateNoneRadio
* Purpose:  
*/
void CRoiGui::OnClickRotateNoneRadio()
{
	try
	{
		GetActiveCamera()->SetFeatureValue(FEATURE_ROTATE, 0.0f, 0, theApp.m_bAutoResetPreview);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnClickRotate90Radio
* Purpose:  
*/
void CRoiGui::OnClickRotate90Radio()
{
	try
	{
		GetActiveCamera()->SetFeatureValue(FEATURE_ROTATE, 90.0f, 0, theApp.m_bAutoResetPreview);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnClickRotate180Radio
* Purpose:  
*/
void CRoiGui::OnClickRotate180Radio()
{
	try
	{
		GetActiveCamera()->SetFeatureValue(FEATURE_ROTATE, 180.0f, 0, theApp.m_bAutoResetPreview);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnClickRotate270Radio
* Purpose:  
*/
void CRoiGui::OnClickRotate270Radio()
{
	try
	{
		GetActiveCamera()->SetFeatureValue(FEATURE_ROTATE, 270.0f, 0, theApp.m_bAutoResetPreview);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnClickHorizontalOff
* Purpose:  
*/
void CRoiGui::OnClickHorizontalOff(void)
{
	try
	{
		GetActiveCamera()->SetHorizontalFlip(false);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnClickHorizontalOn
* Purpose:  
*/
void CRoiGui::OnClickHorizontalOn(void)
{
	try
	{
		GetActiveCamera()->SetHorizontalFlip(true);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnClickVerticalOff
* Purpose:  
*/
void CRoiGui::OnClickVerticalOff(void)
{
	try
	{
		GetActiveCamera()->SetVerticalFlip(false);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnClickVerticalOn
* Purpose:  
*/
void CRoiGui::OnClickVerticalOn(void)
{
	try
	{
		GetActiveCamera()->SetVerticalFlip(true);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnSelchangePixelFormatCombo
* Purpose:  
*/
void CRoiGui::OnSelchangePixelFormatCombo(void)
{
	if (!m_bAdjusting[FEATURE_PIXEL_FORMAT] && m_PixelFormatCombo.GetCurSel() != -1)
	{
		float fmt = ItemData<float>(m_PixelFormatCombo);
		try
		{
         // Stop the strem (if it is streaming), as we only want to do it once while we change
         // both FEATURE_PIXEL_FORMAT and alpha value (if necessary).
         CStreamState ss(GetActiveCamera(), STOP_STREAM, true); 

         GetActiveCamera()->SetFeatureValue(FEATURE_PIXEL_FORMAT, fmt);
         // If the user is setting the pixel format to one of the alpha values, then set it again but
         // this time, provide the new alpha value
         if (fmt >= PIXEL_FORMAT_RGBA && fmt <= PIXEL_FORMAT_ABGR)
         {
            U32 flags = 0;
            U32 alpha = (U32)GetActiveCamera()->GetFeatureValue(FEATURE_PIXEL_FORMAT, &flags, 1);
            SetIntText(m_AlphaEdit, alpha);
             m_AlphaEdit.EnableWindow(true);
         } else {
            m_AlphaEdit.EnableWindow(false);
         }
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
void CRoiGui::OnSelchangePixelFormatInterpretationCombo(void)
{
	if (m_PixelFormatInterpretationCombo.GetCurSel() != -1)
	{
		std::auto_ptr<CPreviewState> ps(NULL);
    
      // If we are previewing, the stop and restart it, as the preview may have changed between color and mono
      if (GetActiveCamera()->GetPreviewState() == START_PREVIEW)
      {
		  ps = std::auto_ptr<CPreviewState>(new CPreviewState(GetActiveCamera(), PAUSE_PREVIEW));
      }
		
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
* Function: OnKillFocusAlphaEdit
* Purpose:  
*/
void CRoiGui::OnKillFocusAlphaEdit(void)
{
	int val = WindowTextToInt(m_AlphaEdit);
   try
	{
	   U32 currentPixelFormat = (U32) GetActiveCamera()->GetFeatureValue(FEATURE_PIXEL_FORMAT);
      if (currentPixelFormat >= PIXEL_FORMAT_RGBA && currentPixelFormat <= PIXEL_FORMAT_ABGR)
      {
         GetActiveCamera()->SetFeatureValue(FEATURE_PIXEL_FORMAT,  static_cast<float> (val), 1);
       }
	}
	catch (PxLException const& e)
	{
	   OnApiError(e);
	}

   // Read back the value, just to see if the camera accepted it
   PopulatePixelFormat();
}


/**
* Function: OnUpdateDecimationModeCombo
* Purpose:  
*/
void CRoiGui::OnUpdateDecimationModeCombo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_DecimationCombo.GetCurSel() > 0);
}











