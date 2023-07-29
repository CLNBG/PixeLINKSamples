#pragma once

#include "DevAppPage.h"
#include "FeatureDialog.h"
#include "CSlider.h"


// CWhiteShadingDialog dialog

class CWhiteShadingDialog : public CFeatureDialog
{
	DECLARE_DYNAMIC(CWhiteShadingDialog)

	DECLARE_MESSAGE_MAP()

	DECLARE_EVENTSINK_MAP()

public:
	CWhiteShadingDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CWhiteShadingDialog();

// Dialog Data
	enum { IDD = IDD_WHITESHADING_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
	virtual void Configure(void);
	virtual void Populate(void);

private:
	CSlider			m_RedSlider;
	CFeatureEdit	m_RedEdit;

	CSlider			m_GreenSlider;
	CFeatureEdit	m_GreenEdit;

	CSlider			m_BlueSlider;
	CFeatureEdit	m_BlueEdit;

	CButton			m_FeatureAutoButton;
	CStatic			m_FeatureMinLabel;
	CStatic			m_FeatureMaxLabel;

	afx_msg void OnScrollRedSlider(void);
	afx_msg void OnChangeRedSlider(void);
	afx_msg void OnKillFocusRedEdit(void);

	afx_msg void OnScrollGreenSlider(void);
	afx_msg void OnChangeGreenSlider(void);
	afx_msg void OnKillFocusGreenEdit(void);

	afx_msg void OnScrollBlueSlider(void);
	afx_msg void OnChangeBlueSlider(void);
	afx_msg void OnKillFocusBlueEdit(void);

	afx_msg void OnClickFeatureAutoButton(void);

};

