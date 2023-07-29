#pragma once


#include "DevAppPage.h"
#include "SubwindowCtrl.h"
#include "CSlider.h"
#include "afxwin.h"


// CSimpleGui dialog

class CSimpleGui : public CDevAppPage
{
	DECLARE_DYNCREATE(CSimpleGui)

	DECLARE_MESSAGE_MAP()

	DECLARE_EVENTSINK_MAP()

public:
	CSimpleGui(CPxLDevAppView* pView = NULL);
	~CSimpleGui(void);

	int GetHelpContextId(void);
	BOOL OnInitDialog(void);
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();

	enum { IDD = IDD_SIMPLE_GUI };
	UINT GetDlgId() { return IDD; }

	virtual void ModeChanged(bool advanced);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private:
	void SetGUIState(eGUIState state);
	void PopulateDefaults(void);
	void ApplyUserSettings(UserSettings& settings);
	void StoreUserSettings(UserSettings& settings);
	void PopulateControls(void);
	void ConfigureControls(void);
	void FeatureChanged(U32 featureId);

	void PopulateActualFrameRate(void);
    void PopulateFpsWarning(void);
    void PopulateLinkSpeedWarning(void);
    
	void PopulateAutoExposure(void);
	void PopulateAutoGain(void);
    void PopulateSaturation(void);
	void PopulateGammaOnOff(void);
    void PopulateHDR(void);

	void PopulateWhiteShading(void);
	void PopulateFrameRateOnOff(void);
	void PopulateColorTempOnOff(void);

	void ConfigureExposure(void);
	void ConfigureGain(void);
    void ConfigureHDR(void);
	void ConfigureWindowSize(void);

	void ApplyClipSettings(UserSettings& settings);
	void ApplyImageSettings(UserSettings& settings);
	void StoreClipSettings(UserSettings& settings);
	void StoreImageSettings(UserSettings& settings);

	SLIDER_HANDLER_SUITE(Exposure)
	SLIDER_HANDLER_SUITE(Gain)
	SLIDER_EVENT_HANDLER_SUITE(Saturation)
	SLIDER_HANDLER_SUITE(Brightness)
	SLIDER_HANDLER_SUITE(Gamma)
	SLIDER_HANDLER_SUITE(FrameRate)
	SLIDER_HANDLER_SUITE(ColorTemp)

	afx_msg void OnScrollRedSlider(void);
	afx_msg void OnChangeRedSlider(void);
	afx_msg void OnKillFocusRedEdit(void);
	afx_msg void OnScrollGreenSlider(void);
	afx_msg void OnChangeGreenSlider(void);
	afx_msg void OnKillFocusGreenEdit(void);
	afx_msg void OnScrollBlueSlider(void);
	afx_msg void OnChangeBlueSlider(void);
	afx_msg void OnKillFocusBlueEdit(void);
	afx_msg void OnClickWhiteShadingAutoButton(void);

	afx_msg void OnClickColorTempOnOffCheck(void);
	afx_msg void OnClickFrameRateOnOffCheck(void);

	afx_msg void OnClickExposureAutoMinButton(void);
	afx_msg void OnClickExposureAutoMaxButton(void);

    afx_msg void OnClickExposureAutoCheck(void);
    afx_msg void OnClickGainAutoCheck(void);
	afx_msg void OnClickGammaOnOffCheck(void);

	afx_msg void OnSelchangeHDRCombo(void);
	afx_msg void OnSelchangeWindowsizeCombo(void);

	afx_msg void OnKillFocusImageFileNameEdit(void);
	afx_msg void OnClickImageFileBrowseButton(void);
	afx_msg void OnSelchangeImageFileFormatCombo(void);
	afx_msg void OnClickImageCaptureButton(void);

	afx_msg void OnUpdateNumFramesEdit(CCmdUI* pCmdUI);

	SLIDER_CONTROL_SUITE(Exposure)
	SLIDER_CONTROL_SUITE(Gain)
	SLIDER_CONTROL_SUITE(Saturation)
	SLIDER_CONTROL_SUITE(Brightness)
	SLIDER_CONTROL_SUITE(Gamma)
	SLIDER_CONTROL_SUITE(FrameRate)
	SLIDER_CONTROL_SUITE(ColorTemp)

    // The autoExpose limits are a little unusual, in that we can read the values the camera will
    // use for the auto expose limits at any time.  However, we can only write them when we perfrom
    // an auto expose operation.  So, in order to allow the user to set these values, we need to be
    // be able to 'remember' the user set value to be used for the next auto expose operation.  We
    // use the m_AutoExposeMinStale/m_AutoExposeMacStale for this; they are true if the user has changed 
    // it's value, and that value will be written to the camera with the next auto expose.
    bool            m_AutoExposeMinStale;
    bool            m_AutoExposeMaxStale;
    bool            m_GainsKnown;
    CStatic         m_AutoExposureMin;
    CStatic         m_AutoExposureMax;
	CButton			m_AutoExposureMinButton;
	CButton			m_AutoExposureMaxButton;

	CEdit			m_ActualFrameRateEdit;
    CStatic         m_FpsWarningStatic;
    CStatic         m_LinkSpeedWarningStatic;

	CSlider			m_RedSlider;
	CFeatureEdit	m_RedEdit;
	CSlider			m_GreenSlider;
	CFeatureEdit	m_GreenEdit;
	CSlider			m_BlueSlider;
	CFeatureEdit	m_BlueEdit;
	CButton			m_WhiteShadingAutoButton;
	CStatic			m_WhiteShadingMinLabel;
	CStatic			m_WhiteShadingMaxLabel;

	CButton			m_ExposureAutoCheck;
	CButton			m_GainAutoCheck;
	CButton			m_GammaOnOffCheck;
	CButton			m_ColorTempOnOffCheck;
	CButton			m_FrameRateOnOffCheck;

    CComboBox		m_HDRCombo;

	CComboBox		m_WindowSizeCombo;

	CEdit			m_ImageFileNameEdit;
	CButton			m_ImageFileBrowseButton;
	CComboBox		m_ImageFileFormatCombo;
	CEdit			m_NumFramesEdit;
	CButton			m_ImageCaptureButton;

	CButton			m_IncrementAfterCaptureCheck;
	CButton			m_PreviewAfterCaptureCheck;
	CButton			m_CaptureFullFrameCheck;
	CButton			m_CaptureFullResCheck;

	UINT_PTR		m_exposureTimerId;
	UINT_PTR		m_gainTimerId;

	void StartExposureTimer(void);
	void StopExposureTimer(void);
	void StartGainTimer(void);
	void StopGainTimer(void);

	void SetExposure(float val);
   	std::pair<float,float> GetExposureAutoLimits();

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
private:
	CButton m_RoiPopupButton;
public:
	afx_msg void OnClickRoiPopupButton();

private:
	CRoiDialog2*		m_pRoiDlg;
};









