// TriggerDialog.cpp : implementation file
//


#include "stdafx.h"
#include "PxLDevApp.h"
#include "TriggerDialog.h"


/******************************************************************************
* CTriggerDialog dialog
******************************************************************************/

IMPLEMENT_DYNAMIC(CTriggerDialog, CFeatureDialog)

/**
* Function: CTriggerDialog
* Purpose:  Constructor
*/
CTriggerDialog::CTriggerDialog(CWnd* pParent /*=NULL*/)
	: CFeatureDialog(CTriggerDialog::IDD, pParent)
{
}


/**
* Function: ~CTriggerDialog
* Purpose:  Destructor
*/
CTriggerDialog::~CTriggerDialog()
{
}


/**
* Function: DoDataExchange
* Purpose:  
*/
void CTriggerDialog::DoDataExchange(CDataExchange* pDX)
{
	CFeatureDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TD_TRIGGERMODE_COMBO, m_TriggerModeCombo);
	DDX_Control(pDX, IDC_TD_TRIGGERTYPE_COMBO, m_TriggerTypeCombo);
	DDX_Control(pDX, IDC_TD_TRIGGERPOLARITY_COMBO, m_TriggerPolarityCombo);
	DDX_Control(pDX, IDC_TD_TRIGGERDELAY_EDIT, m_TriggerDelayEdit);
	DDX_Control(pDX, IDC_TD_TRIGGERPARAMETER_EDIT, m_TriggerParameterEdit);
}


BEGIN_MESSAGE_MAP(CTriggerDialog, CFeatureDialog)
	ON_CBN_SELCHANGE(IDC_TD_TRIGGERMODE_COMBO, OnSelchangeTriggerModeCombo)
END_MESSAGE_MAP()


/**
* Function: Configure
* Purpose:  
*/
void CTriggerDialog::Configure(void)
{
	// Put items in the combo boxes
	SetTriggerModeEntries(m_TriggerModeCombo, m_pCam->GetSupportedTriggerModes());
	SetTriggerTypeEntries(m_TriggerTypeCombo, m_pCam->GetSupportedTriggerTypes());
	SetPolarityEntries(m_TriggerPolarityCombo, m_pCam->GetFeatureLimits(FEATURE_TRIGGER, FEATURE_TRIGGER_PARAM_POLARITY));
}


/**
* Function: Populate
* Purpose:  
*/
void CTriggerDialog::Populate(void)
{
	try
	{
		Trigger trigger = m_pCam->GetTrigger();

		SelectByItemData(m_TriggerModeCombo, trigger.Mode);
		SelectByItemData(m_TriggerTypeCombo, trigger.Type);
		SelectByItemData(m_TriggerPolarityCombo, trigger.Polarity);
		SetFloatText(m_TriggerDelayEdit, trigger.Delay, 3);

		if (trigger.Mode == TRIGGER_MODE_5)
			SetFloatText(m_TriggerParameterEdit, trigger.Parameter);
		else
			SetIntText(m_TriggerParameterEdit, static_cast<int>(trigger.Parameter));
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnSelchangeTriggerModeCombo
* Purpose:  
*/
void CTriggerDialog::OnSelchangeTriggerModeCombo(void)
{
	// Some trigger modes do not use a parameter.
	int mode = ItemData<int>(m_TriggerModeCombo);
	m_TriggerParameterEdit.EnableWindow(TriggerModeTakesParameter(mode));
}


/**
* Function: OnOK
* Purpose:  
*/
void CTriggerDialog::OnOK()
{
	// With this dialog (and ulike many of the other dialogs), we do not set
	// the feature in the camera until OK is clicked.
	Trigger trigger = {-1, -1, -1, 0.0f, 0.0f};

	trigger.Mode = ItemData<int>(m_TriggerModeCombo);
	trigger.Type = ItemData<int>(m_TriggerTypeCombo);
	trigger.Polarity = ItemData<int>(m_TriggerPolarityCombo);
	trigger.Delay = WindowTextToFloat(m_TriggerDelayEdit); // * 1000; // Convert units?

	if (trigger.Mode == TRIGGER_MODE_5)
	{
		// Parameter is a duration in seconds - allow floats.
		trigger.Parameter = WindowTextToFloat(m_TriggerParameterEdit);
	}
	else if (TriggerModeTakesParameter(trigger.Mode))
	{
		// Parameter is a whole number - force to an integer.
		trigger.Parameter = static_cast<float>(WindowTextToInt(m_TriggerParameterEdit));
	}
	// else: leave the parameter as 0.0f

	try
	{
		m_pCam->SetTrigger(trigger);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}

	CFeatureDialog::OnOK();
}

















