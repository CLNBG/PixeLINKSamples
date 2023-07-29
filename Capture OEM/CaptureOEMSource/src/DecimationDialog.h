#pragma once


#include "FeatureDialog.h"


// CDecimationDialog dialog

class CDecimationDialog : public CFeatureDialog
{
	DECLARE_DYNAMIC(CDecimationDialog)

	DECLARE_MESSAGE_MAP()

public:
	CDecimationDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDecimationDialog();

	enum { IDD = IDD_FEATUREDECIMATION_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	virtual void Configure(void);
	virtual void Populate(void);

private:
	CComboBox m_DecimationModeCombo;
	CComboBox m_DecimationCombo;
};
