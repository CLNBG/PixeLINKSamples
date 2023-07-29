#pragma once

#include "afxcmn.h"
#include "afxwin.h"


class CPxLCamera;

// COneTimeAutoDialog dialog

class COneTimeAutoDialog : public CDialog
{
	DECLARE_DYNAMIC(COneTimeAutoDialog)

	DECLARE_MESSAGE_MAP()

public:
	COneTimeAutoDialog(CPxLCamera* pParent);
	virtual ~COneTimeAutoDialog();

// Dialog Data
	enum { IDD = IDD_ONETIMEAUTO_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();

private:
    TCHAR* FeatureIdStr(U32 featureID);
    float  FeatureValue(U32 featureID,  U32* pFlags = NULL);

	CPxLCamera*		m_parent;
	UINT_PTR		m_timerId;
	U32             m_pollNumber;

	CStatic         m_OneTimeAutoFeatureLabel;
	CStatic         m_OneTimeAutoValueLabel;
	CStatic         m_OneTimeAutoPollNumberLabel;
	
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
