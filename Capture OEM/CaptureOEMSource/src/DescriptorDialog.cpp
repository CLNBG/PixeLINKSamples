// DescriptorDialog.cpp : implementation file


#include "stdafx.h"
#include "PxLDevApp.h"
#include "ScrollBox.h"
#include "DescriptorGui.h"
#include "DescriptorDialog.h"

#include "SliderDialog.h"
#include "RoiDialog.h"
#include "TriggerDialog.h"
#include "GpoDialog.h"
#include "FlipDialog.h"
#include "PixelFormatDialog.h"
#include "DecimationDialog.h"
#include "ExtendedShutterDialog.h"
#include "WhiteShadingDialog.h"
#include "RotationDialog.h"


/******************************************************************************
* Class CDescriptorDialog
******************************************************************************/

IMPLEMENT_DYNAMIC(CDescriptorDialog, CDialog)


/**
* Function: CDescriptorDialog
* Purpose:  Constructor
*/
CDescriptorDialog::CDescriptorDialog(CDescriptorGui* pPage, CWnd* pParent /*=NULL*/)
	: CDialog(CDescriptorDialog::IDD, pParent)
	, m_pPage(pPage)
	, m_bAdjusting(false)
	, m_pCurrentPopup(NULL)
{
}


/**
* Function: ~CDescriptorDialog
* Purpose:  Destructor
*/
CDescriptorDialog::~CDescriptorDialog()
{
}


/**
* Function: OnApiError
* Purpose:  
*/
void CDescriptorDialog::OnApiError(PxLException const& e, bool cameraRelated)
{
	m_pPage->OnApiError(e, cameraRelated);
}


void CDescriptorDialog::KillPopup(void)
{
	if (m_pCurrentPopup != NULL)
		::PostMessage(m_pCurrentPopup->GetSafeHwnd(), WM_CLOSE, 0, 0);
}


/**
* Function: OnInitDialog
* Purpose:  
*/
BOOL CDescriptorDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_controls.push_back(ControlSet(&m_BrightnessEdit, &m_BrightnessButton, FEATURE_BRIGHTNESS));
	m_controls.push_back(ControlSet(&m_ExposureEdit, &m_ExposureButton, FEATURE_EXPOSURE));
	m_controls.push_back(ControlSet(&m_SharpnessEdit, &m_SharpnessButton, FEATURE_SHARPNESS));
	m_controls.push_back(ControlSet(&m_ColorTempEdit, &m_ColorTempButton, FEATURE_COLOR_TEMP));
	m_controls.push_back(ControlSet(&m_HueEdit, &m_HueButton, FEATURE_HUE));
	m_controls.push_back(ControlSet(&m_SaturationEdit, &m_SaturationButton, FEATURE_SATURATION));
	m_controls.push_back(ControlSet(&m_GammaEdit, &m_GammaButton, FEATURE_GAMMA));
	m_controls.push_back(ControlSet(&m_ShutterEdit, &m_ShutterButton, FEATURE_SHUTTER));
	m_controls.push_back(ControlSet(&m_GainEdit, &m_GainButton, FEATURE_GAIN));
	m_controls.push_back(ControlSet(&m_IrisEdit, &m_IrisButton, FEATURE_IRIS));
	m_controls.push_back(ControlSet(&m_FocusEdit, &m_FocusButton, FEATURE_FOCUS));
	m_controls.push_back(ControlSet(&m_TriggerEdit, &m_TriggerButton, FEATURE_TRIGGER));
	m_controls.push_back(ControlSet(&m_ZoomEdit, &m_ZoomButton, FEATURE_ZOOM));
	m_controls.push_back(ControlSet(&m_PanEdit, &m_PanButton, FEATURE_PAN));
	m_controls.push_back(ControlSet(&m_TiltEdit, &m_TiltButton, FEATURE_TILT));
	m_controls.push_back(ControlSet(&m_OpticalFilterEdit, &m_OpticalFilterButton, FEATURE_OPT_FILTER));
	m_controls.push_back(ControlSet(&m_GpioEdit, &m_GpioButton, FEATURE_GPIO));
	m_controls.push_back(ControlSet(&m_FrameRateEdit, &m_FrameRateButton, FEATURE_FRAME_RATE));
	m_controls.push_back(ControlSet(&m_RoiEdit, &m_RoiButton, FEATURE_ROI));
	m_controls.push_back(ControlSet(&m_FlipEdit, &m_FlipButton, FEATURE_FLIP));
	m_controls.push_back(ControlSet(&m_DecimationEdit, &m_DecimationButton, FEATURE_PIXEL_ADDRESSING));
	m_controls.push_back(ControlSet(&m_PixelFormatEdit, &m_PixelFormatButton, FEATURE_PIXEL_FORMAT));
	m_controls.push_back(ControlSet(&m_ExtendedShutterEdit, &m_ExtendedShutterButton, FEATURE_EXTENDED_SHUTTER));
	m_controls.push_back(ControlSet(&m_WhiteShadingEdit, &m_WhiteShadingButton, FEATURE_WHITE_SHADING));
	m_controls.push_back(ControlSet(&m_RotationEdit, &m_RotationButton, FEATURE_ROTATE));

	return TRUE;
}


/**
* Function: DoDataExchange
* Purpose:  
*/
void CDescriptorDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_DG_BRIGHTNESS_EDIT, m_BrightnessEdit);
	DDX_Control(pDX, IDC_DG_EXPOSURE_EDIT, m_ExposureEdit);
	DDX_Control(pDX, IDC_DG_SHARPNESS_EDIT, m_SharpnessEdit);
	DDX_Control(pDX, IDC_DG_COLORTEMP_EDIT, m_ColorTempEdit);
	DDX_Control(pDX, IDC_DG_HUE_EDIT, m_HueEdit);
	DDX_Control(pDX, IDC_DG_SATURATION_EDIT, m_SaturationEdit);
	DDX_Control(pDX, IDC_DG_GAMMA_EDIT, m_GammaEdit);
	DDX_Control(pDX, IDC_DG_SHUTTER_EDIT, m_ShutterEdit);
	DDX_Control(pDX, IDC_DG_GAIN_EDIT, m_GainEdit);
	DDX_Control(pDX, IDC_DG_IRIS_EDIT, m_IrisEdit);
	DDX_Control(pDX, IDC_DG_FOCUS_EDIT, m_FocusEdit);
	DDX_Control(pDX, IDC_DG_TRIGGER_EDIT, m_TriggerEdit);
	DDX_Control(pDX, IDC_DG_ZOOM_EDIT, m_ZoomEdit);
	DDX_Control(pDX, IDC_DG_PAN_EDIT, m_PanEdit);
	DDX_Control(pDX, IDC_DG_TILT_EDIT, m_TiltEdit);
	DDX_Control(pDX, IDC_DG_OPTICALFILTER_EDIT, m_OpticalFilterEdit);
	DDX_Control(pDX, IDC_DG_GPIO_EDIT, m_GpioEdit);
	DDX_Control(pDX, IDC_DG_FRAMERATE_EDIT, m_FrameRateEdit);
	DDX_Control(pDX, IDC_DG_ROI_EDIT, m_RoiEdit);
	DDX_Control(pDX, IDC_DG_FLIP_EDIT, m_FlipEdit);
	DDX_Control(pDX, IDC_DG_DECIMATION_EDIT, m_DecimationEdit);
	DDX_Control(pDX, IDC_DG_PIXELFORMAT_EDIT, m_PixelFormatEdit);
	DDX_Control(pDX, IDC_DG_EXTENDEDSHUTTER_EDIT, m_ExtendedShutterEdit);
	DDX_Control(pDX, IDC_DG_WHITESHADING_EDIT, m_WhiteShadingEdit);
	DDX_Control(pDX, IDC_DG_ROTATION_EDIT, m_RotationEdit);

	DDX_Control(pDX, IDC_DG_BRIGHTNESS_BUTTON, m_BrightnessButton);
	DDX_Control(pDX, IDC_DG_EXPOSURE_BUTTON, m_ExposureButton);
	DDX_Control(pDX, IDC_DG_SHARPNESS_BUTTON, m_SharpnessButton);
	DDX_Control(pDX, IDC_DG_COLORTEMP_BUTTON, m_ColorTempButton);
	DDX_Control(pDX, IDC_DG_HUE_BUTTON, m_HueButton);
	DDX_Control(pDX, IDC_DG_SATURATION_BUTTON, m_SaturationButton);
	DDX_Control(pDX, IDC_DG_GAMMA_BUTTON, m_GammaButton);
	DDX_Control(pDX, IDC_DG_SHUTTER_BUTTON, m_ShutterButton);
	DDX_Control(pDX, IDC_DG_GAIN_BUTTON, m_GainButton);
	DDX_Control(pDX, IDC_DG_IRIS_BUTTON, m_IrisButton);
	DDX_Control(pDX, IDC_DG_FOCUS_BUTTON, m_FocusButton);
	DDX_Control(pDX, IDC_DG_TRIGGER_BUTTON, m_TriggerButton);
	DDX_Control(pDX, IDC_DG_ZOOM_BUTTON, m_ZoomButton);
	DDX_Control(pDX, IDC_DG_PAN_BUTTON, m_PanButton);
	DDX_Control(pDX, IDC_DG_TILT_BUTTON, m_TiltButton);
	DDX_Control(pDX, IDC_DG_OPTICALFILTER_BUTTON, m_OpticalFilterButton);
	DDX_Control(pDX, IDC_DG_GPIO_BUTTON, m_GpioButton);
	DDX_Control(pDX, IDC_DG_FRAMERATE_BUTTON, m_FrameRateButton);
	DDX_Control(pDX, IDC_DG_ROI_BUTTON, m_RoiButton);
	DDX_Control(pDX, IDC_DG_FLIP_BUTTON, m_FlipButton);
	DDX_Control(pDX, IDC_DG_DECIMATION_BUTTON, m_DecimationButton);
	DDX_Control(pDX, IDC_DG_PIXELFORMAT_BUTTON, m_PixelFormatButton);
	DDX_Control(pDX, IDC_DG_EXTENDEDSHUTTER_BUTTON, m_ExtendedShutterButton);
	DDX_Control(pDX, IDC_DG_WHITESHADING_BUTTON, m_WhiteShadingButton);
	DDX_Control(pDX, IDC_DG_ROTATION_BUTTON, m_RotationButton);

}


BEGIN_MESSAGE_MAP(CDescriptorDialog, CDialog)

	ON_EN_SETFOCUS(IDC_DG_BRIGHTNESS_EDIT, OnSetFocusBrightnessEdit)
	ON_EN_SETFOCUS(IDC_DG_EXPOSURE_EDIT, OnSetFocusExposureEdit)
	ON_EN_SETFOCUS(IDC_DG_SHARPNESS_EDIT, OnSetFocusSharpnessEdit)
	ON_EN_SETFOCUS(IDC_DG_COLORTEMP_EDIT, OnSetFocusColorTempEdit)
	ON_EN_SETFOCUS(IDC_DG_HUE_EDIT, OnSetFocusHueEdit)
	ON_EN_SETFOCUS(IDC_DG_SATURATION_EDIT, OnSetFocusSaturationEdit)
	ON_EN_SETFOCUS(IDC_DG_GAMMA_EDIT, OnSetFocusGammaEdit)
	ON_EN_SETFOCUS(IDC_DG_SHUTTER_EDIT, OnSetFocusShutterEdit)
	ON_EN_SETFOCUS(IDC_DG_GAIN_EDIT, OnSetFocusGainEdit)
	ON_EN_SETFOCUS(IDC_DG_IRIS_EDIT, OnSetFocusIrisEdit)
	ON_EN_SETFOCUS(IDC_DG_FOCUS_EDIT, OnSetFocusFocusEdit)
	ON_EN_SETFOCUS(IDC_DG_TRIGGER_EDIT, OnSetFocusTriggerEdit)
	ON_EN_SETFOCUS(IDC_DG_ZOOM_EDIT, OnSetFocusZoomEdit)
	ON_EN_SETFOCUS(IDC_DG_PAN_EDIT, OnSetFocusPanEdit)
	ON_EN_SETFOCUS(IDC_DG_TILT_EDIT, OnSetFocusTiltEdit)
	ON_EN_SETFOCUS(IDC_DG_OPTICALFILTER_EDIT, OnSetFocusOpticalFilterEdit)
	ON_EN_SETFOCUS(IDC_DG_GPIO_EDIT, OnSetFocusGpioEdit)
	ON_EN_SETFOCUS(IDC_DG_FRAMERATE_EDIT, OnSetFocusFrameRateEdit)
	ON_EN_SETFOCUS(IDC_DG_ROI_EDIT, OnSetFocusRoiEdit)
	ON_EN_SETFOCUS(IDC_DG_FLIP_EDIT, OnSetFocusFlipEdit)
	ON_EN_SETFOCUS(IDC_DG_DECIMATION_EDIT, OnSetFocusDecimationEdit)
	ON_EN_SETFOCUS(IDC_DG_PIXELFORMAT_EDIT, OnSetFocusPixelFormatEdit)
	ON_EN_SETFOCUS(IDC_DG_EXTENDEDSHUTTER_EDIT, OnSetFocusExtendedShutterEdit)
	ON_EN_SETFOCUS(IDC_DG_WHITESHADING_EDIT, OnSetFocusWhiteShadingEdit)
	ON_EN_SETFOCUS(IDC_DG_ROTATION_EDIT, OnSetFocusRotationEdit)

	ON_BN_SETFOCUS(IDC_DG_BRIGHTNESS_BUTTON, OnSetFocusBrightnessEdit)
	ON_BN_SETFOCUS(IDC_DG_EXPOSURE_BUTTON, OnSetFocusExposureEdit)
	ON_BN_SETFOCUS(IDC_DG_SHARPNESS_BUTTON, OnSetFocusSharpnessEdit)
	ON_BN_SETFOCUS(IDC_DG_COLORTEMP_BUTTON, OnSetFocusColorTempEdit)
	ON_BN_SETFOCUS(IDC_DG_HUE_BUTTON, OnSetFocusHueEdit)
	ON_BN_SETFOCUS(IDC_DG_SATURATION_BUTTON, OnSetFocusSaturationEdit)
	ON_BN_SETFOCUS(IDC_DG_GAMMA_BUTTON, OnSetFocusGammaEdit)
	ON_BN_SETFOCUS(IDC_DG_SHUTTER_BUTTON, OnSetFocusShutterEdit)
	ON_BN_SETFOCUS(IDC_DG_GAIN_BUTTON, OnSetFocusGainEdit)
	ON_BN_SETFOCUS(IDC_DG_IRIS_BUTTON, OnSetFocusIrisEdit)
	ON_BN_SETFOCUS(IDC_DG_FOCUS_BUTTON, OnSetFocusFocusEdit)
	ON_BN_SETFOCUS(IDC_DG_TRIGGER_BUTTON, OnSetFocusTriggerEdit)
	ON_BN_SETFOCUS(IDC_DG_ZOOM_BUTTON, OnSetFocusZoomEdit)
	ON_BN_SETFOCUS(IDC_DG_PAN_BUTTON, OnSetFocusPanEdit)
	ON_BN_SETFOCUS(IDC_DG_TILT_BUTTON, OnSetFocusTiltEdit)
	ON_BN_SETFOCUS(IDC_DG_OPTICALFILTER_BUTTON, OnSetFocusOpticalFilterEdit)
	ON_BN_SETFOCUS(IDC_DG_GPIO_BUTTON, OnSetFocusGpioEdit)
	ON_BN_SETFOCUS(IDC_DG_FRAMERATE_BUTTON, OnSetFocusFrameRateEdit)
	ON_BN_SETFOCUS(IDC_DG_ROI_BUTTON, OnSetFocusRoiEdit)
	ON_BN_SETFOCUS(IDC_DG_FLIP_BUTTON, OnSetFocusFlipEdit)
	ON_BN_SETFOCUS(IDC_DG_DECIMATION_BUTTON, OnSetFocusDecimationEdit)
	ON_BN_SETFOCUS(IDC_DG_PIXELFORMAT_BUTTON, OnSetFocusPixelFormatEdit)
	ON_BN_SETFOCUS(IDC_DG_EXTENDEDSHUTTER_BUTTON, OnSetFocusExtendedShutterEdit)
	ON_BN_SETFOCUS(IDC_DG_WHITESHADING_BUTTON, OnSetFocusWhiteShadingEdit)
	ON_BN_SETFOCUS(IDC_DG_ROTATION_BUTTON, OnSetFocusRotationEdit)

	ON_BN_CLICKED(IDC_DG_BRIGHTNESS_BUTTON, OnClickBrightnessButton)
	ON_BN_CLICKED(IDC_DG_EXPOSURE_BUTTON, OnClickExposureButton)
	ON_BN_CLICKED(IDC_DG_SHARPNESS_BUTTON, OnClickSharpnessButton)
	ON_BN_CLICKED(IDC_DG_COLORTEMP_BUTTON, OnClickColorTempButton)
	ON_BN_CLICKED(IDC_DG_HUE_BUTTON, OnClickHueButton)
	ON_BN_CLICKED(IDC_DG_SATURATION_BUTTON, OnClickSaturationButton)
	ON_BN_CLICKED(IDC_DG_GAMMA_BUTTON, OnClickGammaButton)
	ON_BN_CLICKED(IDC_DG_SHUTTER_BUTTON, OnClickShutterButton)
	ON_BN_CLICKED(IDC_DG_GAIN_BUTTON, OnClickGainButton)
	ON_BN_CLICKED(IDC_DG_IRIS_BUTTON, OnClickIrisButton)
	ON_BN_CLICKED(IDC_DG_FOCUS_BUTTON, OnClickFocusButton)
	ON_BN_CLICKED(IDC_DG_TRIGGER_BUTTON, OnClickTriggerButton)
	ON_BN_CLICKED(IDC_DG_ZOOM_BUTTON, OnClickZoomButton)
	ON_BN_CLICKED(IDC_DG_PAN_BUTTON, OnClickPanButton)
	ON_BN_CLICKED(IDC_DG_TILT_BUTTON, OnClickTiltButton)
	ON_BN_CLICKED(IDC_DG_OPTICALFILTER_BUTTON, OnClickOpticalFilterButton)
	ON_BN_CLICKED(IDC_DG_GPIO_BUTTON, OnClickGpioButton)
	ON_BN_CLICKED(IDC_DG_FRAMERATE_BUTTON, OnClickFrameRateButton)
	ON_BN_CLICKED(IDC_DG_ROI_BUTTON, OnClickRoiButton)
	ON_BN_CLICKED(IDC_DG_FLIP_BUTTON, OnClickFlipButton)
	ON_BN_CLICKED(IDC_DG_DECIMATION_BUTTON, OnClickDecimationButton)
	ON_BN_CLICKED(IDC_DG_PIXELFORMAT_BUTTON, OnClickPixelFormatButton)
	ON_BN_CLICKED(IDC_DG_EXTENDEDSHUTTER_BUTTON, OnClickExtendedShutterButton)
	ON_BN_CLICKED(IDC_DG_WHITESHADING_BUTTON, OnClickWhiteShadingButton)
	ON_BN_CLICKED(IDC_DG_ROTATION_BUTTON, OnClickRotationButton)

END_MESSAGE_MAP()


/**
* Function: ScrollToControl
* Purpose:  
*/
void CDescriptorDialog::ScrollToControl(CWnd& wnd)
{
	CRect r;
	wnd.GetWindowRect(&r);
	ScreenToClient(&r);
	CScrollBox *pSB = static_cast<CScrollBox*>(this->GetParent());
	pSB->ForceIntoView(r.top, r.bottom);
}


/**
* Function: GetActiveCamera
* Purpose:  
*/
CPxLCamera* CDescriptorDialog::GetActiveCamera(void)
{
	return m_pPage->GetActiveCamera();
}


/**
* Function: EnableControls
* Purpose:  
*/
void CDescriptorDialog::EnableControls(bool enable)
{
	for (std::vector<ControlSet>::iterator iter = m_controls.begin();
			iter != m_controls.end(); iter++)
	{
		bool enableCtrl = enable && GetActiveCamera()->FeatureSupportsDescriptor(iter->featureId);
		iter->pEdit->EnableWindow(enableCtrl);
		iter->pButton->EnableWindow(enableCtrl);
	}
}


/**
* Function: ClearControls
* Purpose:  
*/
void CDescriptorDialog::ClearControls(void)
{
	for (std::vector<ControlSet>::iterator iter = m_controls.begin()
			; iter != m_controls.end(); iter++)
	{
		iter->pEdit->SetWindowText(_T(""));
	}
}


/**
* Function: PopulateControls
* Purpose:  
*/
void CDescriptorDialog::PopulateControls(void)
{
	for (std::vector<ControlSet>::iterator iter = m_controls.begin()
			; iter != m_controls.end(); iter++)
	{
		try
		{
			PopulateEditCtrl(iter->featureId, iter->pEdit);
		}
		catch (PxLException const&)
		{
			iter->pEdit->SetWindowText(_T("error"));
			iter->pEdit->EnableWindow(FALSE);
			iter->pButton->EnableWindow(FALSE);
		}
	}
}


/**
* Function: PopulateEditCtrl
* Purpose:  
*/
void CDescriptorDialog::PopulateEditCtrl(U32 featureId, CWnd* wnd /*=NULL*/)
{
	if (wnd == NULL)
	{
		ControlSet* pCS = FindControlSet(featureId);
		if (pCS == NULL)
			return;
		wnd = pCS->pEdit;
	}

	if (!GetActiveCamera()->FeatureSupportsDescriptor(featureId))
	{
		wnd->SetWindowText(_T("n/a")); // The edit boxes are too small for "not supported"...
		return;
	}

	// Display the feature's value, formatted appropriately.
	switch (featureId)
	{
	case FEATURE_TRIGGER:
		PopulateTriggerField(wnd);
		break;
	case FEATURE_GPIO:
		PopulateGpoField(wnd);
		break;
	case FEATURE_ROI:
		PopulateRectField(featureId, wnd);
		break;
	case FEATURE_FLIP:
		PopulateFlipField(wnd);
		break;
	case FEATURE_PIXEL_ADDRESSING:
		PopulateDecimation(wnd);
		break;
	case FEATURE_PIXEL_FORMAT:
		PopulatePixelFormatField(wnd);
		break;
	case FEATURE_EXTENDED_SHUTTER:
		PopulateExtendedShutter(wnd);
		break;
	case FEATURE_WHITE_SHADING:
		PopulateWhiteShading(wnd);
		break;
	default:
		// The rest of the features can be displayed as a single, floating point value.
		PopulateFloatField(featureId, wnd);
		break;
	}
}


/**
* Function: PopulateFloatField
* Purpose:  
*/
void CDescriptorDialog::PopulateFloatField(U32 featureId, CWnd* wnd)
{
	try
	{
		float val = GetActiveCamera()->GetFeatureValue(featureId);
		int mag = 0, temp = static_cast<int>(val); while(++mag,temp/=10);
		SetFloatText(*wnd, val, max(0, 3-mag));
	}
	catch (PxLException const& e)
	{
		m_pPage->OnApiError(e);
	}
}


/**
* Function: PopulateRectField	
* Purpose:  
*/
void CDescriptorDialog::PopulateRectField(U32  featureId, CWnd* wnd)
{
	try{
		CRect r = featureId == FEATURE_ROI
					? GetActiveCamera()->GetRoi()
					: GetActiveCamera()->GetFeatureRect(featureId);
		CString s;
		s.Format(_T("%d, %d, %d, %d"), r.left, r.top, r.Width(), r.Height());
		wnd->SetWindowText(s);
	}
	catch (PxLException const& e)
	{
		m_pPage->OnApiError(e);
	}
}


/**
* Function: PopulateFlipField
* Purpose:  
*/
void CDescriptorDialog::PopulateFlipField(CWnd* wnd)
{
	try
	{
		bool hOn = GetActiveCamera()->GetHorizontalFlip();
		bool vOn = GetActiveCamera()->GetVerticalFlip();
		CString s;
		s.Format(_T("H:%s, V:%s"), hOn ? _T("On") : _T("Off"), vOn ? _T("On") : _T("Off"));
		wnd->SetWindowText(s);
	}
	catch (PxLException const& e)
	{
		m_pPage->OnApiError(e);
	}

}


/**
* Function: PopulateTriggerField
* Purpose:  
*/
void CDescriptorDialog::PopulateTriggerField(CWnd* wnd)
{
	try
	{
		U32 flags = 0;
		Trigger trigger = GetActiveCamera()->GetTrigger(&flags);
		CString s, param(_T(""));
		if (trigger.Mode == TRIGGER_MODE_0 || trigger.Mode == TRIGGER_MODE_1)
			param.Format(_T(", %f"), trigger.Parameter);
		if (flags & FEATURE_FLAG_OFF)
		{
			s = _T("Off");
		}
		else
		{
			s.Format(_T("Mode %d, %s, %s, Delay %dms%s"), 
					trigger.Mode,
					ApiConstAsString(CT_TRIGGER_TYPE, trigger.Type),
					trigger.Polarity ? _T("+'ve") : _T("-'ve"),
					static_cast<int>(trigger.Delay / 1000),
					param);
		}
		wnd->SetWindowText(s);
	}
	catch (PxLException const& e) 
	{
		m_pPage->OnApiError(e);
	}
}


/**
* Function: PopulateGpoField
* Purpose:  
*/
void CDescriptorDialog::PopulateGpoField(CWnd* wnd)
{
	try
	{
		CString gpoString;

		int maxGpo = static_cast<int>(GetActiveCamera()->GetFeatureMaxVal(FEATURE_GPIO, FEATURE_GPIO_PARAM_GPIO_INDEX));
		for (int gpoNum = static_cast<int>(GetActiveCamera()->GetFeatureMinVal(FEATURE_GPIO, FEATURE_GPIO_PARAM_GPIO_INDEX));
				gpoNum <= maxGpo; ++gpoNum)
		{

			Gpo gpo = GetActiveCamera()->GetGpo(gpoNum);
			CString s, param1(_T("")), param2(_T("")), param3(_T(""));
			if (gpo.Mode == GPIO_MODE_STROBE ||
                gpo.Mode == GPIO_MODE_ACTION_STROBE)
			{
				param1.Format(_T(", %f"), gpo.Parameter1);
				param2.Format(_T(", %f"), gpo.Parameter2);
			}
			if (gpo.Mode == GPIO_MODE_PULSE ||
                gpo.Mode == GPIO_MODE_ACTION_PULSE)
			{
				param1.Format(_T(", %d"), static_cast<int>(gpo.Parameter1));
				param2.Format(_T(", %f"), gpo.Parameter2);
				param3.Format(_T(", %f"), gpo.Parameter3);
			}
			s.Format(_T("Gpo #%d: Mode %d, %s%s%s%s,  "), 
					gpoNum,
					gpo.Mode,
					gpo.Polarity ? _T("+'ve") : _T("-'ve"),
					param1,
					param2,
					param3);

			gpoString += s;
		}

		wnd->SetWindowText(gpoString);
	}
	catch (PxLException const& e) 
	{
		m_pPage->OnApiError(e);
	}
}


/**
* Function: PopulatePixelFormat
* Purpose:  
*/
void CDescriptorDialog::PopulatePixelFormatField(CWnd* wnd)
{
	try
	{
		int fmt = static_cast<int>(GetActiveCamera()->GetFeatureValue(FEATURE_PIXEL_FORMAT));
		wnd->SetWindowText(ApiConstAsString(CT_PIXEL_FORMAT, fmt));
	}
	catch (PxLException const& e)
	{
		m_pPage->OnApiError(e);
	}
}


/**
* Function: PopulateDecimation
* Purpose:  
*/
void CDescriptorDialog::PopulateDecimation(CWnd* wnd)
{
	try
	{
		int dec = static_cast<int>(GetActiveCamera()->GetFeatureValue(FEATURE_PIXEL_ADDRESSING));
		if (dec == 1)
			wnd->SetWindowText(_T("None"));
		else
		{
			int mode = GetActiveCamera()->GetFeatureParamCount(FEATURE_PIXEL_ADDRESSING) >= 2
						? static_cast<int>(GetActiveCamera()->GetFeatureValue(FEATURE_PIXEL_ADDRESSING, NULL, 
																			  FEATURE_PIXEL_ADDRESSING_PARAM_MODE))
						: PIXEL_ADDRESSING_MODE_DECIMATE;
			CString s;
			s.Format(_T("%s by %d"), ApiConstAsString(CT_DECIMATION_MODE, mode), dec);
			wnd->SetWindowText(s);
		}
	}
	catch (PxLException const& e)
	{
		m_pPage->OnApiError(e);
	}
}


/**
* Function: PopulateExtendedShutter
* Purpose:  
*/
void CDescriptorDialog::PopulateExtendedShutter(CWnd* wnd)
{
	try
	{
		std::vector<float> knees = GetActiveCamera()->GetKneePoints();
		float exposure = GetActiveCamera()->GetFeatureValue(FEATURE_SHUTTER);

		CString s(_T(""));
		for (int i = 0; i < knees.size(); i++)
		{
			CString temp;
			temp.Format(i == 0 ? _T("%g") : _T(", %g"), exposure - knees[i]);
			s += temp;
		}
		wnd->SetWindowText(knees.size() == 0 ? _T("n/a") : s);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: PopulateWhiteShading
* Purpose:  
*/
void CDescriptorDialog::PopulateWhiteShading(CWnd* wnd)
{
	try
	{
		std::vector<float> gains = GetActiveCamera()->GetWhiteShading();

		CString s;
		s.Format(_T("R:%.2f  G:%.2f  B:%.2f"), gains[0], gains[1], gains[2]);
		wnd->SetWindowText(s);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: ShowFeatureDlg
* Purpose:  
*/
void CDescriptorDialog::ShowFeatureDlg(U32 featureId, CWnd& position)
{
	if (!GetActiveCamera()->FeatureSupportsDescriptor(featureId))
		return; // Shouldn't happen - the button *should* be disabled, but just to be safe...

	// Display an appropriate dialog to get user input of a value for the
	// specified feature.
	switch (featureId)
	{
	case FEATURE_TRIGGER:
		ShowTriggerDlg(position);
		break;

	case FEATURE_GPIO:
		ShowGpoDialog(position);
		break;

	case FEATURE_ROI:
	case FEATURE_AUTO_ROI:
		ShowRoiDlg(featureId, position);
		break;

	case FEATURE_FLIP:
		ShowFlipDlg(position);
		break;

	case FEATURE_PIXEL_ADDRESSING:
		ShowDecimationDialog(position);
		break;

	case FEATURE_PIXEL_FORMAT:
		ShowPixelFormatDialog(position);
		break;

	case FEATURE_EXTENDED_SHUTTER:
		ShowExtendedShutterDialog(position);
		break;

	case FEATURE_WHITE_SHADING:
		ShowWhiteShadingDialog(position);
		break;

	case FEATURE_ROTATE:
		ShowRotationDialog(position);
		break;

	default:
		// The rest of the features can be set using the slider dialog.
		ShowSliderDlg(featureId, position);
		break;
	}
}


/**
* Function: ShowSliderDlg
* Purpose:  
*/
void CDescriptorDialog::ShowSliderDlg(U32 featureId, CWnd& position)
{
	CSliderDialog dlg;
	m_pCurrentPopup = &dlg;

	dlg.m_featureId = featureId;
	dlg.m_pCam = GetActiveCamera();
	dlg.m_location = &position;

	try
	{
		float currentValue = GetActiveCamera()->GetFeatureValue(featureId);

		TempVal<bool> temp(m_bAdjusting, true); // So that the main window will not update itself while the dialog is open.
		if (dlg.DoModal() == IDCANCEL)
		{
			GetActiveCamera()->SetFeatureValue(featureId, currentValue);
		}
		else
		{
			PopulateEditCtrl(featureId);
		}
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
	m_pCurrentPopup = NULL;
}


/**
* Function: ShowRoiDlg
* Purpose:  
*/
void CDescriptorDialog::ShowRoiDlg(U32 featureId, CWnd& position)
{
	CRoiDialog dlg;
	m_pCurrentPopup = &dlg;

	dlg.m_featureId = featureId;
	dlg.m_pCam = GetActiveCamera();
	dlg.m_location = &position;

	try
	{
		CRect r = GetActiveCamera()->GetFeatureRect(featureId);

		TempVal<bool> temp(m_bAdjusting, true); // So that the main window will not update itself while the dialog is open.
		if (dlg.DoModal() == IDCANCEL)
		{
			// Cancelled - set back to original value
			GetActiveCamera()->SetFeatureRect(featureId,
											  r.left, r.top, r.right - r.left, r.bottom - r.top,
											  theApp.m_bAutoResetPreview);
		}
		else
		{
			PopulateEditCtrl(featureId);
		}
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
	m_pCurrentPopup = NULL;
}


/**
* Function: ShowFlipDlg
* Purpose:  
*/
void CDescriptorDialog::ShowFlipDlg(CWnd& position)
{
	CFlipDialog dlg;
	m_pCurrentPopup = &dlg;

	dlg.m_pCam = GetActiveCamera();
	dlg.m_location = &position;

	try
	{
		bool hOn = GetActiveCamera()->GetHorizontalFlip();
		bool vOn = GetActiveCamera()->GetVerticalFlip();

		TempVal<bool> temp(m_bAdjusting, true); // So that the main window will not update itself while the dialog is open.
		if (dlg.DoModal() == IDCANCEL)
		{
			GetActiveCamera()->SetHorizontalFlip(hOn);
			GetActiveCamera()->SetVerticalFlip(vOn);
		}
		else
		{
			PopulateEditCtrl(FEATURE_FLIP);
		}
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
	m_pCurrentPopup = NULL;
}


/**
* Function: ShowExtendedShutterDialog
* Purpose:  
*/
void CDescriptorDialog::ShowExtendedShutterDialog(CWnd& position)
{
	CExtendedShutterDialog dlg;
	m_pCurrentPopup = &dlg;

	dlg.m_pCam = GetActiveCamera();
	dlg.m_location = &position;

	try
	{
		std::vector<float> currentVal = GetActiveCamera()->GetKneePoints();

		TempVal<bool> temp(m_bAdjusting, true); // So that the main window will not update itself while the dialog is open.

		if (dlg.DoModal() == IDCANCEL)
		{
			GetActiveCamera()->SetKneePoints(currentVal);
		}
		else
		{
			PopulateEditCtrl(FEATURE_EXTENDED_SHUTTER);
		}
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
	m_pCurrentPopup = NULL;
}


/**
* Function: ShowTriggerDlg
* Purpose:  
*/
void CDescriptorDialog::ShowTriggerDlg(CWnd& position)
{
	CTriggerDialog dlg;
	m_pCurrentPopup = &dlg;

	dlg.m_pCam = GetActiveCamera();
	dlg.m_location = &position;

	// The Trigger dialog, unlike some of the other dialogs, does not do
	// live updates to the camera - it only updates the camera when the
	// "OK" button is clicked, so we don't need to store the current trigger
	// settings and restore them in the event of a "Cancel", as we do for 
	// most other features.
	// The reasoning is that with a feature like, say, Brightness, the user may
	// want to see the effect of his changes (in the preview window) while the
	// dialog is open, in order to decide on what value to set. With the trigger
	// feature, however, I don't think there's anything to see, so there's no
	// need to update the camera until the dialog is closed with an "OK".
	try
	{
		dlg.DoModal();
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
	m_pCurrentPopup = NULL;
}


/**
* Function: ShowGpoDialog
* Purpose:  
*/
void CDescriptorDialog::ShowGpoDialog(CWnd& position)
{
	CGpoDialog dlg;
	m_pCurrentPopup = &dlg;

	dlg.m_pCam = GetActiveCamera();
	dlg.m_location = &position;

	// See comment in ShowTriggerDialog, above.
	try
	{
		dlg.DoModal();
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
	m_pCurrentPopup = NULL;
}


/**
* Function: ShowPixelFormatDialog
* Purpose:  
*/
void CDescriptorDialog::ShowPixelFormatDialog(CWnd& position)
{
	CPixelFormatDialog dlg;
	m_pCurrentPopup = &dlg;

	dlg.m_pCam = GetActiveCamera();
	dlg.m_location = &position;

	// See the comment in ShowTriggerDialog, above.
	try
	{
		dlg.DoModal();
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
	m_pCurrentPopup = NULL;
}


/**
* Function: ShowDecimationDialog
* Purpose:  
*/
void CDescriptorDialog::ShowDecimationDialog(CWnd& position)
{
	CDecimationDialog dlg;
	m_pCurrentPopup = &dlg;

	dlg.m_pCam = GetActiveCamera();
	dlg.m_location = &position;

	TempVal<bool> temp(m_bAdjusting, true); // So that the main window will not update itself while the dialog is open.

	// See the comment in ShowTriggerDialog, above.
	try
	{
		dlg.DoModal();
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
	m_pCurrentPopup = NULL;
}


/**
* Function: ShowWhiteShadingDialog
* Purpose:  
*/
void CDescriptorDialog::ShowWhiteShadingDialog(CWnd& position)
{
	CWhiteShadingDialog dlg;
	m_pCurrentPopup = &dlg;

	dlg.m_pCam = GetActiveCamera();
	dlg.m_location = &position;

	try
	{
		std::vector<float> currentVal = GetActiveCamera()->GetWhiteShading();

		TempVal<bool> temp(m_bAdjusting, true); // So that the main window will not update itself while the dialog is open.

		if (dlg.DoModal() == IDCANCEL)
		{
			GetActiveCamera()->SetWhiteShading(currentVal);
		}
		else
		{
			PopulateEditCtrl(FEATURE_WHITE_SHADING);
		}
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
	m_pCurrentPopup = NULL;
}


/**
* Function: ShowRotationDialog
* Purpose:  
*/
void CDescriptorDialog::ShowRotationDialog(CWnd& position)
{
	CRotationDialog dlg;
	m_pCurrentPopup = &dlg;

	dlg.m_pCam = GetActiveCamera();
	dlg.m_location = &position;

	try
	{
		float currentValue = GetActiveCamera()->GetFeatureValue(FEATURE_ROTATE);

		TempVal<bool> temp(m_bAdjusting, true); // So that the main window will not update itself while the dialog is open.
		if (dlg.DoModal() == IDCANCEL)
		{
			GetActiveCamera()->SetFeatureValue(FEATURE_ROTATE, currentValue);
		}
		else
		{
			PopulateEditCtrl(FEATURE_ROTATE);
		}
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
	m_pCurrentPopup = NULL;
}


/**
* Function: FeatureChanged
* Purpose:  
*/
void CDescriptorDialog::FeatureChanged(U32 featureId)
{
	if (!m_bAdjusting)
		PopulateEditCtrl(featureId);
}


/**
* Function: FindControlSet
* Purpose:  
*/
ControlSet* CDescriptorDialog::FindControlSet(U32 featureId)
{
	std::vector<ControlSet>::iterator iter = m_controls.begin();
	for ( ; iter != m_controls.end(); iter++)
	{
		if (iter->featureId == featureId)
			return &*iter;
	}
	return NULL;
}


void CDescriptorDialog::OnSetFocusBrightnessEdit()		{ ScrollToControl(m_BrightnessEdit); }
void CDescriptorDialog::OnSetFocusExposureEdit()		{ ScrollToControl(m_ExposureEdit); }
void CDescriptorDialog::OnSetFocusSharpnessEdit()		{ ScrollToControl(m_SharpnessEdit); }
void CDescriptorDialog::OnSetFocusColorTempEdit()	{ ScrollToControl(m_ColorTempEdit); }
void CDescriptorDialog::OnSetFocusHueEdit()				{ ScrollToControl(m_HueEdit); }
void CDescriptorDialog::OnSetFocusSaturationEdit()		{ ScrollToControl(m_SaturationEdit); }
void CDescriptorDialog::OnSetFocusGammaEdit()			{ ScrollToControl(m_GammaEdit); }
void CDescriptorDialog::OnSetFocusShutterEdit()			{ ScrollToControl(m_ShutterEdit); }
void CDescriptorDialog::OnSetFocusGainEdit()			{ ScrollToControl(m_GainEdit); }
void CDescriptorDialog::OnSetFocusIrisEdit()			{ ScrollToControl(m_IrisEdit); }
void CDescriptorDialog::OnSetFocusFocusEdit()			{ ScrollToControl(m_FocusEdit); }
void CDescriptorDialog::OnSetFocusTriggerEdit()			{ ScrollToControl(m_TriggerEdit); }
void CDescriptorDialog::OnSetFocusZoomEdit()			{ ScrollToControl(m_ZoomEdit); }
void CDescriptorDialog::OnSetFocusPanEdit()				{ ScrollToControl(m_PanEdit); }
void CDescriptorDialog::OnSetFocusTiltEdit()			{ ScrollToControl(m_TiltEdit); }
void CDescriptorDialog::OnSetFocusOpticalFilterEdit()	{ ScrollToControl(m_OpticalFilterEdit); }
void CDescriptorDialog::OnSetFocusGpioEdit()			{ ScrollToControl(m_GpioEdit); }
void CDescriptorDialog::OnSetFocusFrameRateEdit()		{ ScrollToControl(m_FrameRateEdit); }
void CDescriptorDialog::OnSetFocusRoiEdit()				{ ScrollToControl(m_RoiEdit); }
void CDescriptorDialog::OnSetFocusFlipEdit()			{ ScrollToControl(m_FlipEdit); }
void CDescriptorDialog::OnSetFocusDecimationEdit()		{ ScrollToControl(m_DecimationEdit); }
void CDescriptorDialog::OnSetFocusPixelFormatEdit()		{ ScrollToControl(m_PixelFormatEdit); }
void CDescriptorDialog::OnSetFocusExtendedShutterEdit()	{ ScrollToControl(m_ExtendedShutterEdit); }
void CDescriptorDialog::OnSetFocusWhiteShadingEdit()	{ ScrollToControl(m_WhiteShadingEdit); }
void CDescriptorDialog::OnSetFocusRotationEdit()		{ ScrollToControl(m_RotationEdit); }


void CDescriptorDialog::OnClickBrightnessButton()		{ ShowFeatureDlg(FEATURE_BRIGHTNESS, m_BrightnessButton); }
void CDescriptorDialog::OnClickExposureButton()			{ ShowFeatureDlg(FEATURE_EXPOSURE, m_ExposureButton); }
void CDescriptorDialog::OnClickSharpnessButton()		{ ShowFeatureDlg(FEATURE_SHARPNESS, m_SharpnessButton); }
void CDescriptorDialog::OnClickColorTempButton()		{ ShowFeatureDlg(FEATURE_COLOR_TEMP, m_ColorTempButton); }
void CDescriptorDialog::OnClickHueButton()				{ ShowFeatureDlg(FEATURE_HUE, m_HueButton); }
void CDescriptorDialog::OnClickSaturationButton()		{ ShowFeatureDlg(FEATURE_SATURATION, m_SaturationButton); }
void CDescriptorDialog::OnClickGammaButton()			{ ShowFeatureDlg(FEATURE_GAMMA, m_GammaButton); }
void CDescriptorDialog::OnClickShutterButton()			{ ShowFeatureDlg(FEATURE_SHUTTER, m_ShutterButton); }
void CDescriptorDialog::OnClickGainButton()				{ ShowFeatureDlg(FEATURE_GAIN, m_GainButton); }
void CDescriptorDialog::OnClickIrisButton()				{ ShowFeatureDlg(FEATURE_IRIS, m_IrisButton); }
void CDescriptorDialog::OnClickFocusButton()			{ ShowFeatureDlg(FEATURE_FOCUS, m_FocusButton); }
void CDescriptorDialog::OnClickTriggerButton()			{ ShowFeatureDlg(FEATURE_TRIGGER, m_TriggerButton); }
void CDescriptorDialog::OnClickZoomButton()				{ ShowFeatureDlg(FEATURE_ZOOM, m_ZoomButton); }
void CDescriptorDialog::OnClickPanButton()				{ ShowFeatureDlg(FEATURE_PAN, m_PanButton); }
void CDescriptorDialog::OnClickTiltButton()				{ ShowFeatureDlg(FEATURE_TILT, m_TiltButton); }
void CDescriptorDialog::OnClickOpticalFilterButton()	{ ShowFeatureDlg(FEATURE_OPT_FILTER, m_OpticalFilterButton); }
void CDescriptorDialog::OnClickGpioButton()				{ ShowFeatureDlg(FEATURE_GPIO, m_GpioButton); }
void CDescriptorDialog::OnClickFrameRateButton()		{ ShowFeatureDlg(FEATURE_FRAME_RATE, m_FrameRateButton); }
void CDescriptorDialog::OnClickRoiButton()				{ ShowFeatureDlg(FEATURE_ROI, m_RoiButton); }
void CDescriptorDialog::OnClickFlipButton()				{ ShowFeatureDlg(FEATURE_FLIP, m_FlipButton); }
void CDescriptorDialog::OnClickDecimationButton()		{ ShowFeatureDlg(FEATURE_PIXEL_ADDRESSING, m_DecimationButton); }
void CDescriptorDialog::OnClickPixelFormatButton()		{ ShowFeatureDlg(FEATURE_PIXEL_FORMAT, m_PixelFormatButton); }
void CDescriptorDialog::OnClickExtendedShutterButton()	{ ShowFeatureDlg(FEATURE_EXTENDED_SHUTTER, m_ExtendedShutterButton); }
void CDescriptorDialog::OnClickWhiteShadingButton()		{ ShowFeatureDlg(FEATURE_WHITE_SHADING, m_WhiteShadingButton); }
void CDescriptorDialog::OnClickRotationButton()			{ ShowFeatureDlg(FEATURE_ROTATE, m_RotationButton); }



















