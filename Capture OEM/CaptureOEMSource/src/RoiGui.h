#pragma once


#include "DevAppPage.h"
#include "SubwindowCtrl.h"
#include "afxwin.h"


// Default size of the ROI shown in the subwindow control - to display when
// no camera is active.
#define DEFAULT_FOV_WIDTH 1280
#define DEFAULT_FOV_HEIGHT 1024


// CRoiGui dialog

class CRoiGui : public CDevAppPage
{
	DECLARE_DYNCREATE(CRoiGui)

	DECLARE_MESSAGE_MAP()

	DECLARE_EVENTSINK_MAP()

public:
	CRoiGui(CPxLDevAppView* pView = NULL);
	~CRoiGui(void);
	int GetHelpContextId(void);
	BOOL OnInitDialog(void);
	virtual BOOL OnSetActive();

	enum { IDD = IDD_ROI_GUI };
	UINT GetDlgId() { return IDD; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private:
	afx_msg void OnClickApplyToRoiRadio(void);
	afx_msg void OnClickApplyToAutoRoiRadio(void);
	afx_msg void OnClickAutoRoiEnableCheck(void);

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

	afx_msg void OnSelchangePixelFormatCombo(void);
	afx_msg void OnSelchangePixelFormatInterpretationCombo(void);
	afx_msg void OnKillFocusAlphaEdit();
	
	afx_msg void OnClickPacketSizeAutoCheck(void);

	//afx_msg void OnUpdateApplyToRoiRadio(CCmdUI* pCmdUI);
	//afx_msg void OnUpdateApplyToAutoRoiRadio(CCmdUI* pCmdUI);
	//afx_msg void OnUpdateAutoRoiEnableButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDecimationModeCombo(CCmdUI* pCmdUI);

	CButton			m_ApplyToRoiRadio;
	CButton			m_ApplyToAutoRoiRadio;
	CButton			m_AutoRoiEnableCheck;

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

	CStatic        m_OnCameraFlipStatic; 
   CButton			m_ImageFlipHorizontalOffRadio;
	CButton			m_ImageFlipHorizontalOnRadio;
	CButton			m_ImageFlipVerticalOffRadio;
	CButton			m_ImageFlipVerticalOnRadio;

	CComboBox		m_PixelFormatCombo;
	CComboBox		m_PixelFormatInterpretationCombo;
	CFeatureEdit	m_AlphaEdit;
	
	CButton			m_PacketSizeAutoCheck;

	U32				m_ControlledFeature;
	bool			m_bRoiConfigured;

	void SetGUIState(eGUIState state);
	void PopulateDefaults(void);
	void PopulateControls(void);
	void ConfigureControls(void);
	void FeatureChanged(U32 featureId);

	void PopulateSubwindow(void);
	void PopulateDecimation(void);
	void PopulateRotation(void);
	void PopulateFlip(void);
	void PopulatePixelFormat(void);
	void PopulatePacketSize(void);

	void SetPixelFormatEntries(void);
	void SetPixelFormatInterpretationEntries(void);
	void SetPacketSizeEntries(void);

	void ConfigureRoi(void);
	void ConfigureDecimation(void);

	void SetControlledFeature(U32 featureId);

	void StartUpdateTimer(void);
	void StopUpdateTimer(void);

};











