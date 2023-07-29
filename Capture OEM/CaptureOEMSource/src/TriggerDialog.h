#pragma once


#include "FeatureDialog.h"


// CTriggerDialog dialog

class CTriggerDialog : public CFeatureDialog
{
	DECLARE_DYNAMIC(CTriggerDialog)

	DECLARE_MESSAGE_MAP()

public:
	CTriggerDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTriggerDialog();

	enum { IDD = IDD_FEATURETRIGGER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual void Configure(void);
	virtual void Populate(void);
	virtual void OnOK();

private:
	CComboBox	m_TriggerModeCombo;
	CComboBox	m_TriggerTypeCombo;
	CComboBox	m_TriggerPolarityCombo;
	CEdit		m_TriggerDelayEdit;
	CEdit		m_TriggerParameterEdit;

	afx_msg void OnSelchangeTriggerModeCombo(void);

};


