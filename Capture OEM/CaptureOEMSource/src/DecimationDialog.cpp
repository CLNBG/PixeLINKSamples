// DecimationDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "DecimationDialog.h"


// CDecimationDialog dialog

IMPLEMENT_DYNAMIC(CDecimationDialog, CFeatureDialog)


/**
* Function: CDecimationDialog
* Purpose:  Constructor
*/
CDecimationDialog::CDecimationDialog(CWnd* pParent /*=NULL*/)
	: CFeatureDialog(CDecimationDialog::IDD, pParent)
{
}


/**
* Function: ~CDecimationDialog
* Purpose:  Destructor
*/
CDecimationDialog::~CDecimationDialog()
{
}


/**
* Function: DoDataExchange
* Purpose:  
*/
void CDecimationDialog::DoDataExchange(CDataExchange* pDX)
{
	CFeatureDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DD_DECIMATIONMODE_COMBO, m_DecimationModeCombo);
	DDX_Control(pDX, IDC_DD_DECIMATION_COMBO, m_DecimationCombo);
}


BEGIN_MESSAGE_MAP(CDecimationDialog, CFeatureDialog)
END_MESSAGE_MAP()


/**
* Function: Configure
* Purpose:  
*/
void CDecimationDialog::Configure(void)
{
	try
	{
		SetDecimationModeEntries(m_DecimationModeCombo, m_pCam->GetSupportedDecimationModeValues());
		SetDecimationEntries(m_DecimationCombo, 
		                     m_pCam->GetSupportedXDecimationValues(),
		                     m_pCam->GetSupportedYDecimationValues(), 
		                     m_pCam->SupportsAsymmetricPixelAddressing() );
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
void CDecimationDialog::Populate(void)
{
	int dec = static_cast<int>(m_pCam->GetFeatureValue(FEATURE_PIXEL_ADDRESSING));
	SelectByItemData(m_DecimationCombo, dec);
	if (m_pCam->GetFeatureParamCount(FEATURE_PIXEL_ADDRESSING) >= 2)
	{
		int mode = static_cast<int>(m_pCam->GetFeatureValue(FEATURE_PIXEL_ADDRESSING, NULL, FEATURE_PIXEL_ADDRESSING_PARAM_MODE));
		SelectByItemData(m_DecimationModeCombo, mode);
	}
	else
	{
		m_DecimationModeCombo.SetCurSel(0);
	}
}


/**
* Function: OnOK
* Purpose:  
*/
void CDecimationDialog::OnOK()
{
	int dec = ItemData<int>(m_DecimationCombo);
	int mode = ItemData<int>(m_DecimationModeCombo);

	try
	{
		m_pCam->SetFeatureValue(FEATURE_PIXEL_ADDRESSING,
								static_cast<float>(dec),
								FEATURE_PIXEL_ADDRESSING_PARAM_VALUE,
								theApp.m_bAutoResetPreview);
		if (m_pCam->GetFeatureParamCount(FEATURE_PIXEL_ADDRESSING) >= 2)
		{
			m_pCam->SetFeatureValue(FEATURE_PIXEL_ADDRESSING,
									static_cast<float>(mode),
									FEATURE_PIXEL_ADDRESSING_PARAM_MODE,
									theApp.m_bAutoResetPreview);
		}
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}

	CFeatureDialog::OnOK();
}
