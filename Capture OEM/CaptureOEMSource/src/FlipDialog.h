#pragma once


#include "FeatureDialog.h"


// CFlipDialog dialog

class CFlipDialog : public CFeatureDialog
{
	DECLARE_DYNAMIC(CFlipDialog)

public:
	CFlipDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFlipDialog();

// Dialog Data
	enum { IDD = IDD_FEATUREFLIP_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	virtual void Configure(void);
	virtual void Populate(void);

private:
	CButton			m_HorizontalOffRadio;
	CButton			m_HorizontalOnRadio;
	CButton			m_VerticalOffRadio;
	CButton			m_VerticalOnRadio;

	afx_msg void OnClickHorizontalOff(void);
	afx_msg void OnClickHorizontalOn(void);
	afx_msg void OnClickVerticalOff(void);
	afx_msg void OnClickVerticalOn(void);

};
