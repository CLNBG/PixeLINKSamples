#pragma once


#include "FeatureDialog.h"
#include "CSlider.h"


// CSliderDialog dialog

class CSliderDialog : public CFeatureDialog
{
	DECLARE_DYNAMIC(CSliderDialog)

	DECLARE_MESSAGE_MAP()

	DECLARE_EVENTSINK_MAP()

public:
	CSliderDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSliderDialog();

	enum { IDD = IDD_SLIDER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual void Configure(void);
	virtual void Populate(void);

private:
	CSlider			m_FeatureSlider;
	CEdit			m_FeatureEdit;
	CButton			m_FeatureAutoButton;
	CStatic			m_FeatureMinLabel;
	CStatic			m_FeatureMaxLabel;
	CStatic			m_FeatureFrame;

	afx_msg void OnScrollFeatureSlider(void);
	afx_msg void OnChangeFeatureSlider(void);
	afx_msg void OnKillFocusFeatureEdit(void);
	afx_msg void OnClickFeatureAutoButton(void);

};
