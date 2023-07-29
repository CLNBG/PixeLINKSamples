#pragma once


#include "FeatureDialog.h"


// CRotationDialog dialog

class CRotationDialog : public CFeatureDialog
{
	DECLARE_DYNAMIC(CRotationDialog)

public:
	CRotationDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRotationDialog();

// Dialog Data
	enum { IDD = IDD_FEATUREROTATE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	virtual void Configure(void);
	virtual void Populate(void);

private:
	CButton			m_RotateNoneRadio;
	CButton			m_Rotate90Radio;
	CButton			m_Rotate180Radio;
	CButton			m_Rotate270Radio;

	afx_msg void OnClickRotateNone(void);
	afx_msg void OnClickRotate90(void);
	afx_msg void OnClickRotate180(void);
	afx_msg void OnClickRotate270(void);

};

