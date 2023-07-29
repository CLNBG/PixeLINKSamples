#pragma once


#include "FeatureDialog.h"
#include "afxwin.h"


// CGpoDialog dialog

class CGpoDialog : public CFeatureDialog
{
	DECLARE_DYNAMIC(CGpoDialog)

	DECLARE_MESSAGE_MAP()

public:
	CGpoDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGpoDialog();

// Dialog Data
	enum { IDD = IDD_FEATUREGPO_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual void Configure(void);
	virtual void Populate(void);
	virtual void OnOK();

private:
	CComboBox m_GpoNumberCombo;
	CButton m_GpoOnOffCheck;
	CComboBox m_GpoModeCombo;
	CComboBox m_GpoPolarityCombo;
	CEdit m_GpoParameter1Edit;
	CEdit m_GpoParameter2Edit;
	CEdit m_GpoParameter3Edit;

	std::vector<Gpo> m_gpos;
	int m_currentGpo;

	void DisplayGpo();

public:
	afx_msg void OnSelchangeGponNumberCombo();
	afx_msg void OnClickGpOonOffCheck();
	afx_msg void OnSelchangeGpoModeCombo();
	afx_msg void OnSelchangeGpoPolarityCombo();
	afx_msg void OnKillFocusGpoParameter1Edit();
	afx_msg void OnKillFocusGpoParameter2Edit();
	afx_msg void OnKillFocusGpoParameter3Edit();
};




