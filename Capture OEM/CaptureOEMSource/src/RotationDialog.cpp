// RotationDialog.cpp : implementation file
//


#include "stdafx.h"
#include "PxLDevApp.h"
#include "RotationDialog.h"


IMPLEMENT_DYNAMIC(CRotationDialog, CFeatureDialog)


/**
* Function: (
* Purpose:  
*/
CRotationDialog::CRotationDialog(CWnd* pParent /*=NULL*/)
	: CFeatureDialog(CRotationDialog::IDD, pParent)
{
	m_featureId = FEATURE_ROTATE;
}


/**
* Function: CRotationDialog
* Purpose:  
*/
CRotationDialog::~CRotationDialog()
{
}


/**
* Function: DoDataExchange
* Purpose:  
*/
void CRotationDialog::DoDataExchange(CDataExchange* pDX)
{
	CFeatureDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_RD_ROTATE0_RADIO, m_RotateNoneRadio);
	DDX_Control(pDX, IDC_RD_ROTATE90_RADIO, m_Rotate90Radio);
	DDX_Control(pDX, IDC_RD_ROTATE180_RADIO, m_Rotate180Radio);
	DDX_Control(pDX, IDC_RD_ROTATE270_RADIO, m_Rotate270Radio);

}


BEGIN_MESSAGE_MAP(CRotationDialog, CFeatureDialog)

	ON_BN_CLICKED(IDC_RD_ROTATE0_RADIO, OnClickRotateNone)
	ON_BN_CLICKED(IDC_RD_ROTATE90_RADIO, OnClickRotate90)
	ON_BN_CLICKED(IDC_RD_ROTATE180_RADIO, OnClickRotate180)
	ON_BN_CLICKED(IDC_RD_ROTATE270_RADIO, OnClickRotate270)

END_MESSAGE_MAP()


/**
* Function: Populate
* Purpose:  
*/
void CRotationDialog::Populate(void)
{
	try
	{
		if (m_pCam->FeatureSupported(FEATURE_ROTATE))
		{
			int val = static_cast<int>(m_pCam->GetFeatureValue(FEATURE_ROTATE));
			
			m_RotateNoneRadio.SetCheck(val == 0);
			m_Rotate90Radio.SetCheck(val == 90);
			m_Rotate180Radio.SetCheck(val == 180);
			m_Rotate270Radio.SetCheck(val == 270);

		}
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


void CRotationDialog::Configure(void)
{
}


/**
* Function: OnClickRotateNone
* Purpose:  
*/
void CRotationDialog::OnClickRotateNone(void)
{
	try
	{
		m_pCam->SetFeatureValue(FEATURE_ROTATE, 0.0f);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnClickRotate90
* Purpose:  
*/
void CRotationDialog::OnClickRotate90(void)
{
	try
	{
		m_pCam->SetFeatureValue(FEATURE_ROTATE, 90.0f);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnClickRotate180
* Purpose:  
*/
void CRotationDialog::OnClickRotate180(void)
{
	try
	{
		m_pCam->SetFeatureValue(FEATURE_ROTATE, 180.0f);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnClickRotate270
* Purpose:  
*/
void CRotationDialog::OnClickRotate270(void)
{
	try
	{
		m_pCam->SetFeatureValue(FEATURE_ROTATE, 270.0f);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


