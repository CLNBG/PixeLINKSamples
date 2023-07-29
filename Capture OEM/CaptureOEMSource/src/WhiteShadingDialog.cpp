// WhiteShadingDialog.cpp : implementation file
//


#include "stdafx.h"
#include "PxLDevApp.h"
#include "WhiteShadingDialog.h"


IMPLEMENT_DYNAMIC(CWhiteShadingDialog, CFeatureDialog)


/**
* Function: (
* Purpose:  
*/
CWhiteShadingDialog::CWhiteShadingDialog(CWnd* pParent /*=NULL*/)
	: CFeatureDialog(CWhiteShadingDialog::IDD, pParent)
{
}


/**
* Function: CWhiteShadingDialog
* Purpose:  
*/
CWhiteShadingDialog::~CWhiteShadingDialog()
{
}


/**
* Function: DoDataExchange
* Purpose:  
*/
void CWhiteShadingDialog::DoDataExchange(CDataExchange* pDX)
{
	CFeatureDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_WSD_RED_SLIDER, m_RedSlider);
	DDX_Control(pDX, IDC_WSD_RED_EDIT, m_RedEdit);
	DDX_Control(pDX, IDC_WSD_GREEN_SLIDER, m_GreenSlider);
	DDX_Control(pDX, IDC_WSD_GREEN_EDIT, m_GreenEdit);
	DDX_Control(pDX, IDC_WSD_BLUE_SLIDER, m_BlueSlider);
	DDX_Control(pDX, IDC_WSD_BLUE_EDIT, m_BlueEdit);

	DDX_Control(pDX, IDC_WSD_FEATUREAUTO_BUTTON, m_FeatureAutoButton);
	DDX_Control(pDX, IDC_WSD_FEATUREMIN_LABEL, m_FeatureMinLabel);
	DDX_Control(pDX, IDC_WSD_FEATUREMAX_LABEL, m_FeatureMaxLabel);

}


BEGIN_MESSAGE_MAP(CWhiteShadingDialog, CFeatureDialog)

	ON_BN_CLICKED(IDC_WSD_FEATUREAUTO_BUTTON, OnClickFeatureAutoButton)

	ON_EN_KILLFOCUS(IDC_WSD_RED_EDIT, OnKillFocusRedEdit)
	ON_EN_KILLFOCUS(IDC_WSD_GREEN_EDIT, OnKillFocusGreenEdit)
	ON_EN_KILLFOCUS(IDC_WSD_BLUE_EDIT, OnKillFocusBlueEdit)

END_MESSAGE_MAP()


BEGIN_EVENTSINK_MAP(CWhiteShadingDialog, CFeatureDialog)

	ON_EVENT(CWhiteShadingDialog, IDC_WSD_RED_SLIDER, 1, OnScrollRedSlider, VTS_NONE)
	ON_EVENT(CWhiteShadingDialog, IDC_WSD_RED_SLIDER, 2, OnChangeRedSlider, VTS_NONE)
	ON_EVENT(CWhiteShadingDialog, IDC_WSD_GREEN_SLIDER, 1, OnScrollGreenSlider, VTS_NONE)
	ON_EVENT(CWhiteShadingDialog, IDC_WSD_GREEN_SLIDER, 2, OnChangeGreenSlider, VTS_NONE)
	ON_EVENT(CWhiteShadingDialog, IDC_WSD_BLUE_SLIDER, 1, OnScrollBlueSlider, VTS_NONE)
	ON_EVENT(CWhiteShadingDialog, IDC_WSD_BLUE_SLIDER, 2, OnChangeBlueSlider, VTS_NONE)

END_EVENTSINK_MAP()


/**
* Function: Configure
* Purpose:  
*/
void CWhiteShadingDialog::Configure(void)
{
	m_RedSlider.SetLinkedWindows(&m_RedEdit, &m_FeatureMinLabel, &m_FeatureMaxLabel, NULL);
	m_GreenSlider.SetLinkedWindows(&m_GreenEdit, NULL, NULL, NULL);
	m_BlueSlider.SetLinkedWindows(&m_BlueEdit, NULL, NULL, NULL);

	// Enable the "Auto" button only if the camera supports "One-Push" for
	// for our feature.
	m_FeatureAutoButton.EnableWindow(m_pCam->FeatureSupportsOnePush(FEATURE_WHITE_SHADING));

	// Set the range (min and max values) of the sliders.
	try
	{
		std::pair<float,float> limits = m_pCam->GetFeatureLimits(FEATURE_WHITE_SHADING);
		float min = limits.first;
		float max = limits.second;
		m_RedSlider.SetRange(min, max);
		m_GreenSlider.SetRange(min, max);
		m_BlueSlider.SetRange(min, max);
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
void CWhiteShadingDialog::Populate(void)
{
	try
	{
		if (m_pCam->FeatureSupported(FEATURE_WHITE_SHADING))
		{
			std::vector<float> gains = m_pCam->GetWhiteShading();
			
			m_RedSlider.SetValue(gains[FEATURE_WHITE_SHADING_PARAM_RED]);
			m_GreenSlider.SetValue(gains[FEATURE_WHITE_SHADING_PARAM_GREEN]);
			m_BlueSlider.SetValue(gains[FEATURE_WHITE_SHADING_PARAM_BLUE]);

		}
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnScrollRedSlider
* Purpose:  
*/
void CWhiteShadingDialog::OnScrollRedSlider()
{
	m_RedSlider.OnScroll();
	try
	{
		m_pCam->SetFeatureValue(FEATURE_WHITE_SHADING,
								m_RedSlider.GetValue(),
								FEATURE_WHITE_SHADING_PARAM_RED);
		Populate();
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnChangeRedSlider
* Purpose:  
*/
void CWhiteShadingDialog::OnChangeRedSlider()
{
	if (m_bAdjusting)
		return;

	m_RedSlider.OnChange();
	try
	{
		m_pCam->SetFeatureValue(FEATURE_WHITE_SHADING,
								m_RedSlider.GetValue(),
								FEATURE_WHITE_SHADING_PARAM_RED);
		Populate();
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnKillFocusRedEdit
* Purpose:  
*/
void CWhiteShadingDialog::OnKillFocusRedEdit()
{
	m_RedSlider.UpdateFromWnd();
}


/**
* Function: OnScrollGreenSlider
* Purpose:  
*/
void CWhiteShadingDialog::OnScrollGreenSlider()
{
	m_GreenSlider.OnScroll();
	try
	{
		m_pCam->SetFeatureValue(FEATURE_WHITE_SHADING,
								m_GreenSlider.GetValue(),
								FEATURE_WHITE_SHADING_PARAM_GREEN);
		Populate();
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnChangeGreenSlider
* Purpose:  
*/
void CWhiteShadingDialog::OnChangeGreenSlider()
{
	if (m_bAdjusting)
		return;

	m_GreenSlider.OnChange();
	try
	{
		m_pCam->SetFeatureValue(FEATURE_WHITE_SHADING,
								m_GreenSlider.GetValue(),
								FEATURE_WHITE_SHADING_PARAM_GREEN);
		Populate();
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnKillFocusGreenEdit
* Purpose:  
*/
void CWhiteShadingDialog::OnKillFocusGreenEdit()
{
	m_GreenSlider.UpdateFromWnd();
}


/**
* Function: OnScrollBlueSlider
* Purpose:  
*/
void CWhiteShadingDialog::OnScrollBlueSlider()
{
	m_BlueSlider.OnScroll();
	try
	{
		m_pCam->SetFeatureValue(FEATURE_WHITE_SHADING,
								m_BlueSlider.GetValue(),
								FEATURE_WHITE_SHADING_PARAM_BLUE);
		Populate();
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnChangeBlueSlider
* Purpose:  
*/
void CWhiteShadingDialog::OnChangeBlueSlider()
{
	if (m_bAdjusting)
		return;

	m_BlueSlider.OnChange();
	try
	{
		m_pCam->SetFeatureValue(FEATURE_WHITE_SHADING,
								m_BlueSlider.GetValue(),
								FEATURE_WHITE_SHADING_PARAM_BLUE);
		Populate();
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnKillFocusBlueEdit
* Purpose:  
*/
void CWhiteShadingDialog::OnKillFocusBlueEdit()
{
	m_BlueSlider.UpdateFromWnd();
}


/**
* Function: OnClickFeatureAutoButton
* Purpose:  
*/
void CWhiteShadingDialog::OnClickFeatureAutoButton()
{
	try
	{
		m_pCam->SetOnePush(FEATURE_WHITE_SHADING);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


