#pragma once


#include "DevAppPage.h"
#include "SubwindowCtrl.h"
#include "CSlider.h"
#include "afxwin.h"


// Default size of the ROI shown in the subwindow control - to display when
// no camera is active.
#define DEFAULT_FOV_WIDTH 1280
#define DEFAULT_FOV_HEIGHT 1024


// CLensGui dialog

class CLensGui : public CDevAppPage
{
	DECLARE_DYNCREATE(CLensGui)

	DECLARE_MESSAGE_MAP()

	DECLARE_EVENTSINK_MAP()

public:
	CLensGui(CPxLDevAppView* pView = NULL);
	~CLensGui(void);
	int GetHelpContextId(void);
	BOOL OnInitDialog(void);
	virtual BOOL OnSetActive();

	enum { IDD = IDD_LENS_GUI };
	UINT GetDlgId() { return IDD; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private:

	SLIDER_HANDLER_SUITE(FocusControl)
	SLIDER_HANDLER_SUITE(ZoomControl)

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

	afx_msg void OnSelchangeWindowsizeCombo(void);
	
	afx_msg void OnClickSharpnessScoreCheck(void);
	afx_msg void OnClickPreviewCheck(void);

   	CComboBox	 m_ControllerSelectCombo;
    afx_msg void OnDropdownControllerSelectCombo(void);
    afx_msg void OnSelchangeControllerSelectCombo(void);
    CStatic      m_CantOpenComWarningStatic;

	SLIDER_CONTROL_SUITE(FocusControl)
	CButton		    m_FocusLowerButton;
	CButton		    m_FocusUpperButton;
    afx_msg void OnClickFocusLowerButton();
    afx_msg void OnClickFocusUpperButton();
	
    SLIDER_CONTROL_SUITE(ZoomControl)
	CButton		    m_ZoomLowerButton;
	CButton		    m_ZoomUpperButton;
    afx_msg void OnClickZoomLowerButton();
    afx_msg void OnClickZoomUpperButton();

	CFeatureEdit	m_RoiWidthEdit;
	CFeatureEdit	m_RoiHeightEdit;
	
	CSubwindowCtrl	m_SubwindowCtrl;
	CFeatureEdit	m_TopEdit;
	CFeatureEdit	m_LeftEdit;
	CFeatureEdit	m_HeightEdit;
	CFeatureEdit	m_WidthEdit;
	CButton			m_CenterButton;
	CComboBox		m_WindowSizeCombo;

	CButton         m_SharpnessScoreCheck;
	CButton         m_PreviewCheck;

	bool			m_bSsRoiConfigured;
    U32             m_MyController; // 0 == I don't have a conttroller assigned
    bool            m_bDontWantController;
    bool            m_DefaultState; // Bugzilla.1246 -- Don't use the Callback tab if it hasn't been initialized

	void SetGUIState(eGUIState state);
	void PopulateDefaults(void);
	void PopulateControls(void);
	void ConfigureControls(void);
	void FeatureChanged(U32 featureId);

	void PopulateSubwindow(void);
    void PopulateControllerSelect(void);
    void ConfigureSsRoi(void);

	void StartUpdateTimer(void);
	void StopUpdateTimer(void);

};











