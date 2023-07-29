// FlipDialog.cpp : implementation file
//


#include "stdafx.h"
#include "PxLDevApp.h"
#include "FlipDialog.h"


IMPLEMENT_DYNAMIC(CFlipDialog, CFeatureDialog)


/**
* Function: (
* Purpose:  
*/
CFlipDialog::CFlipDialog(CWnd* pParent /*=NULL*/)
	: CFeatureDialog(CFlipDialog::IDD, pParent)
{
	m_featureId = FEATURE_FLIP;
}


/**
* Function: CFlipDialog
* Purpose:  
*/
CFlipDialog::~CFlipDialog()
{
}


/**
* Function: DoDataExchange
* Purpose:  
*/
void CFlipDialog::DoDataExchange(CDataExchange* pDX)
{
	CFeatureDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_IFD_HORIZONTALOFF_RADIO, m_HorizontalOffRadio);
	DDX_Control(pDX, IDC_IFD_HORIZONTALON_RADIO, m_HorizontalOnRadio);
	DDX_Control(pDX, IDC_IFD_VERTICALOFF_RADIO, m_VerticalOffRadio);
	DDX_Control(pDX, IDC_IFD_VERTICALON_RADIO, m_VerticalOnRadio);

}


BEGIN_MESSAGE_MAP(CFlipDialog, CFeatureDialog)

	ON_BN_CLICKED(IDC_IFD_HORIZONTALOFF_RADIO, OnClickHorizontalOff)
	ON_BN_CLICKED(IDC_IFD_HORIZONTALON_RADIO, OnClickHorizontalOn)
	ON_BN_CLICKED(IDC_IFD_VERTICALOFF_RADIO, OnClickVerticalOff)
	ON_BN_CLICKED(IDC_IFD_VERTICALON_RADIO, OnClickVerticalOn)

END_MESSAGE_MAP()


/**
* Function: Populate
* Purpose:  
*/
void CFlipDialog::Populate(void)
{
	try
	{
		if (m_pCam->FeatureSupported(FEATURE_FLIP))
		{
			bool hFlipOn = m_pCam->GetHorizontalFlip();
			m_HorizontalOnRadio.SetCheck(hFlipOn);
			m_HorizontalOffRadio.SetCheck(!hFlipOn);

			bool vFlipOn = m_pCam->GetVerticalFlip();
			m_VerticalOnRadio.SetCheck(vFlipOn);
			m_VerticalOffRadio.SetCheck(!vFlipOn);
		}
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


void CFlipDialog::Configure(void)
{
}


/**
* Function: OnClickHorizontalOff
* Purpose:  
*/
void CFlipDialog::OnClickHorizontalOff(void)
{
	try
	{
		m_pCam->SetHorizontalFlip(false);
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
void CFlipDialog::OnClickHorizontalOn(void)
{
	try
	{
		m_pCam->SetHorizontalFlip(true);
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
void CFlipDialog::OnClickVerticalOff(void)
{
	try
	{
		m_pCam->SetVerticalFlip(false);
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
void CFlipDialog::OnClickVerticalOn(void)
{
	try
	{
		m_pCam->SetVerticalFlip(true);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


