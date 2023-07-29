// PixelFormatDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "PixelFormatDialog.h"
#include "pixelformatdialog.h"


// CPixelFormatDialog dialog

IMPLEMENT_DYNAMIC(CPixelFormatDialog, CFeatureDialog)


/**
* Function: CPixelFormatDialog
* Purpose:  Constructor
*/
CPixelFormatDialog::CPixelFormatDialog(CWnd* pParent /*=NULL*/)
	: CFeatureDialog(CPixelFormatDialog::IDD, pParent)
{
}


/**
* Function: ~CPixelFormatDialog
* Purpose:  Destructor
*/
CPixelFormatDialog::~CPixelFormatDialog()
{
}


/**
* Function: DoDataExchange
* Purpose:  
*/
void CPixelFormatDialog::DoDataExchange(CDataExchange* pDX)
{
	CFeatureDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PFD_PIXELFORMAT_COMBO, m_PixelFormatCombo);
}


BEGIN_MESSAGE_MAP(CPixelFormatDialog, CFeatureDialog)
END_MESSAGE_MAP()


/**
* Function: Configure
* Purpose:  
*/
void CPixelFormatDialog::Configure(void)
{
	std::vector<int> const& formats = m_pCam->GetSupportedPixelFormats();
	ClearComboBox(m_PixelFormatCombo);

	for (std::vector<int>::const_iterator iter = formats.begin(); iter != formats.end(); iter++)
	{
		m_PixelFormatCombo.SetItemData(
				m_PixelFormatCombo.AddString(ApiConstAsString(CT_PIXEL_FORMAT, *iter)),
				static_cast<DWORD_PTR>(*iter));
	}
}


/**
* Function: Populate
* Purpose:  
*/
void CPixelFormatDialog::Populate(void)
{
	int fmt = static_cast<int>(m_pCam->GetFeatureValue(FEATURE_PIXEL_FORMAT));
	SelectByItemData(m_PixelFormatCombo, fmt);
}


/**
* Function: OnOK
* Purpose:  
*/
void CPixelFormatDialog::OnOK()
{
	int fmt = ItemData<int>(m_PixelFormatCombo);

	try
	{
		m_pCam->SetFeatureValue(FEATURE_PIXEL_FORMAT, static_cast<float>(fmt));
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}

	CFeatureDialog::OnOK();
}
