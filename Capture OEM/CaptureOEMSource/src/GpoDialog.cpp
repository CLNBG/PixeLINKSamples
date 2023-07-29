// GpoDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "GpoDialog.h"


// CGpoDialog dialog

IMPLEMENT_DYNAMIC(CGpoDialog, CFeatureDialog)


CGpoDialog::CGpoDialog(CWnd* pParent /*=NULL*/)
	: CFeatureDialog(CGpoDialog::IDD, pParent)
{
}

CGpoDialog::~CGpoDialog()
{
}

void CGpoDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GD_GPONNUMBER_COMBO, m_GpoNumberCombo);
	DDX_Control(pDX, IDC_GD_GPOONOFF_CHECK, m_GpoOnOffCheck);
	DDX_Control(pDX, IDC_GD_GPOMODE_COMBO, m_GpoModeCombo);
	DDX_Control(pDX, IDC_GD_GPOPOLARITY_COMBO, m_GpoPolarityCombo);
	DDX_Control(pDX, IDC_GD_GPOPARAMETER1_EDIT, m_GpoParameter1Edit);
	DDX_Control(pDX, IDC_GD_GPOPARAMETER2_EDIT, m_GpoParameter2Edit);
	DDX_Control(pDX, IDC_GD_GPOPARAMETER3_EDIT, m_GpoParameter3Edit);
}


BEGIN_MESSAGE_MAP(CGpoDialog, CDialog)
	ON_CBN_SELCHANGE(IDC_GD_GPONNUMBER_COMBO, OnSelchangeGponNumberCombo)
	ON_BN_CLICKED(IDC_GD_GPOONOFF_CHECK, OnClickGpOonOffCheck)
	ON_CBN_SELCHANGE(IDC_GD_GPOMODE_COMBO, OnSelchangeGpoModeCombo)
	ON_CBN_SELCHANGE(IDC_GD_GPOPOLARITY_COMBO, OnSelchangeGpoPolarityCombo)
	ON_EN_KILLFOCUS(IDC_GD_GPOPARAMETER1_EDIT, OnKillFocusGpoParameter1Edit)
	ON_EN_KILLFOCUS(IDC_GD_GPOPARAMETER2_EDIT, OnKillFocusGpoParameter2Edit)
	ON_EN_KILLFOCUS(IDC_GD_GPOPARAMETER3_EDIT, OnKillFocusGpoParameter3Edit)
END_MESSAGE_MAP()


// CGpoDialog message handlers


void CGpoDialog::Configure(void)
{
	// Gpo Number
	std::pair<float,float> minmax = m_pCam->GetFeatureLimits(FEATURE_GPIO, FEATURE_GPIO_PARAM_GPIO_INDEX);
	int minGpo = static_cast<int>(minmax.first);
	int maxGpo = static_cast<int>(minmax.second);
    bool dedicatedGpi = maxGpo >= 5;  // If there is a GPO#5, there is no GPO#3 and GPO#4

	CString s;
	for (int gpoNum = minGpo; gpoNum <= maxGpo; gpoNum++)
	{
	    if (dedicatedGpi && (gpoNum == 3 || gpoNum == 4)) continue;
		s.Format(_T("%d"), gpoNum);
		int idx = m_GpoNumberCombo.AddString(s);
		m_GpoNumberCombo.SetItemData(idx, static_cast<DWORD_PTR>(gpoNum));
	}

	// Gpo Mode
	SetGpioModeEntries(m_GpoModeCombo, m_pCam->GetSupportedGpioModes());

	// Polarity
	SetPolarityEntries(m_GpoPolarityCombo, m_pCam->GetFeatureLimits(FEATURE_GPIO, FEATURE_GPIO_PARAM_POLARITY));

	// Store all the current Gpo settings in m_gpos.
	m_gpos.resize(maxGpo + 1);
	for (int i = minGpo; i <= maxGpo; i++)
	{
		m_gpos[i] = m_pCam->GetGpo(i);
	}
	m_GpoNumberCombo.SetCurSel(0);
	m_currentGpo = minGpo;
}


void CGpoDialog::Populate(void)
{
	DisplayGpo();
}


void CGpoDialog::DisplayGpo(void)
{
	m_GpoOnOffCheck.SetCheck((FEATURE_FLAG_OFF & m_gpos[m_currentGpo].flags) == 0);
	SelectByItemData(m_GpoModeCombo, m_gpos[m_currentGpo].Mode);
	SelectByItemData(m_GpoPolarityCombo, m_gpos[m_currentGpo].Polarity);
	SetFloatText(m_GpoParameter1Edit, m_gpos[m_currentGpo].Parameter1, 3);
	SetFloatText(m_GpoParameter2Edit, m_gpos[m_currentGpo].Parameter2, 3);
	SetFloatText(m_GpoParameter3Edit, m_gpos[m_currentGpo].Parameter3, 3);
}


void CGpoDialog::OnOK()
{
	std::pair<float,float> minmax = m_pCam->GetFeatureLimits(FEATURE_GPIO, FEATURE_GPIO_PARAM_GPIO_INDEX);
	int minGpo = static_cast<int>(minmax.first);
	int maxGpo = static_cast<int>(minmax.second);
    bool dedicatedGpi = maxGpo >= 5;  // If there is a GPO#5, there is no GPO#3 and GPO#4

    for (int i = minGpo; i <= maxGpo; i++)
	{
	    if (dedicatedGpi && (i == 3 || i == 4)) continue;
		m_pCam->SetGpo(m_gpos[i], i);
	}

	CFeatureDialog::OnOK();
}


void CGpoDialog::OnSelchangeGponNumberCombo()
{
	m_currentGpo = ItemData<int>(m_GpoNumberCombo);
	DisplayGpo();
}

void CGpoDialog::OnClickGpOonOffCheck()
{
	if (m_GpoOnOffCheck.GetCheck() == BST_CHECKED)
	{
		m_gpos[m_currentGpo].flags &= ~FEATURE_FLAG_OFF;
	}
	else
	{
		m_gpos[m_currentGpo].flags |= FEATURE_FLAG_OFF;
	}
}

void CGpoDialog::OnSelchangeGpoModeCombo()
{
	m_gpos[m_currentGpo].Mode = ItemData<int>(m_GpoModeCombo);
}

void CGpoDialog::OnSelchangeGpoPolarityCombo()
{
	m_gpos[m_currentGpo].Polarity = ItemData<int>(m_GpoPolarityCombo);
}

void CGpoDialog::OnKillFocusGpoParameter1Edit()
{
	m_gpos[m_currentGpo].Parameter1 = WindowTextToFloat(m_GpoParameter1Edit);
}

void CGpoDialog::OnKillFocusGpoParameter2Edit()
{
	m_gpos[m_currentGpo].Parameter2 = WindowTextToFloat(m_GpoParameter2Edit);
}

void CGpoDialog::OnKillFocusGpoParameter3Edit()
{
	m_gpos[m_currentGpo].Parameter3 = WindowTextToFloat(m_GpoParameter3Edit);
}



























