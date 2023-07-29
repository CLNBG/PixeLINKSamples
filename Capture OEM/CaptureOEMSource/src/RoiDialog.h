#pragma once

#include "DevAppPage.h"
#include "FeatureDialog.h"
#include "SubwindowCtrl.h"


// CRoiDialog dialog

class CRoiDialog : public CFeatureDialog
{
	DECLARE_DYNAMIC(CRoiDialog)

	DECLARE_MESSAGE_MAP()

	DECLARE_EVENTSINK_MAP()

public:
	CRoiDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRoiDialog();

	enum { IDD = IDD_FEATUREROI_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual void Configure(void);
	virtual void Populate(void);

private:
	CSubwindowCtrl	m_SubwindowCtrl;
	CFeatureEdit	m_TopEdit;
	CFeatureEdit	m_LeftEdit;
	CFeatureEdit	m_HeightEdit;
	CFeatureEdit	m_WidthEdit;

	afx_msg void OnSubwindowChanged(long left, long top, long width, long height);
	afx_msg void OnKillFocusSubwindowTopEdit(void);
	afx_msg void OnKillFocusSubwindowLeftEdit(void);
	afx_msg void OnKillFocusSubwindowWidthEdit(void);
	afx_msg void OnKillFocusSubwindowHeightEdit(void);

};











