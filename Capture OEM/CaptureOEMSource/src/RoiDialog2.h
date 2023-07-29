#pragma once

#include "DevAppPage.h"
#include "SubwindowCtrl.h"
#include "afxwin.h"


// CRoiDialog2 dialog

class CRoiDialog2 : public CDialog
{
	DECLARE_DYNAMIC(CRoiDialog2)

	DECLARE_MESSAGE_MAP()

	DECLARE_EVENTSINK_MAP()

public:
	CRoiDialog2(CDevAppPage* pParent);
	virtual ~CRoiDialog2();

	BOOL OnInitDialog(void);
	afx_msg void OnKickIdle(void);

	CPxLCamera* GetActiveCamera(void);
	void OnApiError(PxLException const& e, bool cameraRelated = true);

private:
	CDevAppPage* m_parent;
	std::map<U32,bool>& m_bAdjusting;

public:

// Dialog Data
	enum { IDD = IDD_ROI_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
	void FeatureChanged(U32 featureId);

private:
	void SetGUIState(eGUIState state);
	void PopulateControls(void);
	void ConfigureControls(void);

	void PopulateSubwindow(void);
	void PopulateDecimation(void);
	void PopulateRotation(void);
	void PopulateFlip(void);

	void ConfigureRoi(void);
	void ConfigureDecimation(void);

private:
	afx_msg void OnSubwindowChanged(long left, long top, long width, long height);

	afx_msg void OnKillFocusSubwindowTopEdit(void);
	afx_msg void OnKillFocusSubwindowLeftEdit(void);
	afx_msg void OnKillFocusSubwindowWidthEdit(void);
	afx_msg void OnKillFocusSubwindowHeightEdit(void);

	afx_msg void OnSetFocusWindowLeftEdit();
	afx_msg void OnSetFocusWindowTopEdit();
	afx_msg void OnSetFocusWindowWidthEdit();
	afx_msg void OnSetFocusWindowHeightEdit();

	afx_msg void OnClickCenterButton(void);

	afx_msg void OnSelchangeDecimationCombo(void);
	afx_msg void OnSelchangeDecimationModeCombo(void);
	afx_msg void OnSelchangeWindowsizeCombo(void);

	afx_msg void OnClickRotateNoneRadio();
	afx_msg void OnClickRotate90Radio();
	afx_msg void OnClickRotate180Radio();
	afx_msg void OnClickRotate270Radio();

	afx_msg void OnClickHorizontalOff(void);
	afx_msg void OnClickHorizontalOn(void);
	afx_msg void OnClickVerticalOff(void);
	afx_msg void OnClickVerticalOn(void);

	afx_msg void OnUpdateDecimationModeCombo(CCmdUI* pCmdUI);

	CSubwindowCtrl	m_SubwindowCtrl;
	CFeatureEdit	m_TopEdit;
	CFeatureEdit	m_LeftEdit;
	CFeatureEdit	m_HeightEdit;
	CFeatureEdit	m_WidthEdit;
	CButton			m_CenterButton;
	CComboBox		m_DecimationCombo;
	CComboBox		m_DecimationModeCombo;
	CComboBox		m_WindowSizeCombo;

	CButton			m_RotateNoneRadio;
	CButton			m_Rotate90Radio;
	CButton			m_Rotate180Radio;
	CButton			m_Rotate270Radio;

	CButton			m_ImageFlipHorizontalOffRadio;
	CButton			m_ImageFlipHorizontalOnRadio;
	CButton			m_ImageFlipVerticalOffRadio;
	CButton			m_ImageFlipVerticalOnRadio;

};



