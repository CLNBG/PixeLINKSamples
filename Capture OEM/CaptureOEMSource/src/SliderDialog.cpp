// SliderDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "SliderDialog.h"


// CSliderDialog dialog

IMPLEMENT_DYNAMIC(CSliderDialog, CFeatureDialog)

CSliderDialog::CSliderDialog(CWnd* pParent /*=NULL*/)
	: CFeatureDialog(CSliderDialog::IDD, pParent)
{
}

CSliderDialog::~CSliderDialog()
{
}

void CSliderDialog::DoDataExchange(CDataExchange* pDX)
{
	CFeatureDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SD_FEATURE_SLIDER, m_FeatureSlider);
	DDX_Control(pDX, IDC_SD_FEATURE_EDIT, m_FeatureEdit);
	DDX_Control(pDX, IDC_SD_FEATUREAUTO_BUTTON, m_FeatureAutoButton);
	DDX_Control(pDX, IDC_SD_FEATUREMIN_LABEL, m_FeatureMinLabel);
	DDX_Control(pDX, IDC_SD_FEATUREMAX_LABEL, m_FeatureMaxLabel);
	DDX_Control(pDX, IDC_SD_FEATURE_FRAME, m_FeatureFrame);

}


BEGIN_MESSAGE_MAP(CSliderDialog, CFeatureDialog)

	ON_EN_KILLFOCUS(IDC_SD_FEATURE_EDIT, OnKillFocusFeatureEdit)
	ON_BN_CLICKED(IDC_SD_FEATUREAUTO_BUTTON, OnClickFeatureAutoButton)

END_MESSAGE_MAP()


BEGIN_EVENTSINK_MAP(CSliderDialog, CFeatureDialog)

	ON_EVENT(CSliderDialog, IDC_SD_FEATURE_SLIDER, 1, OnScrollFeatureSlider, VTS_NONE)
	ON_EVENT(CSliderDialog, IDC_SD_FEATURE_SLIDER, 2, OnChangeFeatureSlider, VTS_NONE)

END_EVENTSINK_MAP()


/**
* Function: Configure
* Purpose:  
*/
void CSliderDialog::Configure(void)
{
	m_FeatureSlider.SetLinkedWindows(&m_FeatureEdit, &m_FeatureMinLabel, &m_FeatureMaxLabel, &m_FeatureFrame);
	m_FeatureFrame.SetWindowText(FeatureAsString(m_featureId));

	// Enable the "Auto" button only if the camera supports "One-Push" for
	// for our feature.
	m_FeatureAutoButton.EnableWindow(m_pCam->FeatureSupportsOnePush(m_featureId));

	// Set the range (min and max values) of the slider.
	try
	{
		if (m_featureId == FEATURE_GAIN)
		{
			std::vector<float> const& vals = m_pCam->GetSupportedGains();
			if (vals.size() > 0)
			{
				m_FeatureSlider.SetMappedMode(vals.size(), &vals[0]);
			}
		}
		else if (m_featureId == FEATURE_COLOR_TEMP)
		{
			std::vector<float> const& vals = m_pCam->GetSupportedColorTempValues();
			if (vals.size() > 0)
			{
				m_FeatureSlider.SetMappedMode(vals.size(), &vals[0]);
			}
		}
		else if (m_featureId == FEATURE_BRIGHTNESS)
		{
			m_FeatureSlider.SetIntegerMode();
		}

		std::pair<float,float> range = m_pCam->GetFeatureLimits(m_featureId);
		TempVal<bool> _temp(m_bAdjusting, true);
		m_FeatureSlider.SetRange(range.first, range.second);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: Populate
* Purpose:  
*/
void CSliderDialog::Populate(void)
{
	// Populate the slider with the current value of the feature.
	try
	{
		float val = m_pCam->GetFeatureValue(m_featureId);

		TempVal<bool> temp(m_bAdjusting, true); // Stop the OnChange event of the slider from calling SetFeature.
		m_FeatureSlider.SetValue(val);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
		//throw;
	}
}


/**
* Function: OnScrollFeatureSlider
* Purpose:  
*/
void CSliderDialog::OnScrollFeatureSlider()
{
	// Remove this handler if you do not want continuous updates to the
	// camera while dragging the slider.
	m_FeatureSlider.OnScroll();
	try
	{
		m_pCam->SetFeatureValue(m_featureId, m_FeatureSlider.GetValue());
		Populate();
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
		//throw;
	}
}


/**
* Function: OnChangeFeatureSlider
* Purpose:  
*/
void CSliderDialog::OnChangeFeatureSlider()
{
	if (m_bAdjusting)
		return;

	m_FeatureSlider.OnChange();
	try
	{
		m_pCam->SetFeatureValue(m_featureId, m_FeatureSlider.GetValue());
		Populate();
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
		//throw;
	}
}


/**
* Function: OnKillFocusFeatureEdit
* Purpose:  
*/
void CSliderDialog::OnKillFocusFeatureEdit()
{
	{
		TempVal<bool> temp(m_bAdjusting, true);
		m_FeatureSlider.UpdateFromWnd();
	}
	m_pCam->SetFeatureValue(m_featureId, m_FeatureSlider.GetValue());
	Populate();
}


/**
* Function: OnClickFeatureAutoButton
* Purpose:  
*/
void CSliderDialog::OnClickFeatureAutoButton()
{
	try
	{
		m_pCam->SetOnePush(m_featureId);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}














