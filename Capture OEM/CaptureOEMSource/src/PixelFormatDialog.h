#pragma once


#include "FeatureDialog.h"


// CPixelFormatDialog dialog

class CPixelFormatDialog : public CFeatureDialog
{
	DECLARE_DYNAMIC(CPixelFormatDialog)

	DECLARE_MESSAGE_MAP()

public:
	CPixelFormatDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPixelFormatDialog();

	enum { IDD = IDD_FEATUREPIXELFORMAT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	virtual void Configure(void);
	virtual void Populate(void);

private:
	CComboBox m_PixelFormatCombo;

};
