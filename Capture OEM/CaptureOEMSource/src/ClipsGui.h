#pragma once


#include "DevAppPage.h"
#include "CSlider.h"
#include "afxwin.h"


// CClipsGui dialog

class CClipsGui : public CDevAppPage
{
	DECLARE_DYNAMIC(CClipsGui)

	DECLARE_MESSAGE_MAP()

   	DECLARE_EVENTSINK_MAP()

public:
	CClipsGui(CPxLDevAppView* pView = NULL);
	~CClipsGui();
	int GetHelpContextId(void);

	enum { IDD = IDD_CLIPS_GUI };
	UINT GetDlgId() { return IDD; }

	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private:
	bool			m_bCaptureInProgress;

	CEdit			m_NumFramesEdit;
	CEdit			m_CapTimeEdit;
	CComboBox		m_EncodingCombo;
	CEdit			m_ClipDecimationFactorEdit;

	CButton			m_MatchCaptureCheck;
	CEdit			m_PbFrameRateEdit;
	CEdit			m_CameraFrameRateEdit;
	CEdit			m_PbTimeEdit;
	CEdit			m_PbEffectEdit;

	CButton			m_AutoBitRateCheck;
	CSlider			m_BitRateSlider;
	CFeatureEdit	m_BitRateEdit;
	CStatic			m_BitRateMinLabel;
	CStatic			m_BitRateMaxLabel;
    CEdit			m_FileSizeEdit;

    CButton			m_SaveRawDataFileCheck;
	CEdit			m_RawDataFileEdit;
	CButton			m_RawDataFileBrowseButton;
	CButton			m_IncrementRawDataFileCheck;

	CButton			m_SaveFormattedCheck;
	CComboBox		m_ClipFileFormatCombo;
	CEdit			m_ClipFileNameEdit;
	CButton			m_FormattedFileBrowseButton;
	CButton			m_IncrementFormattedFileCheck;

	CButton			m_ViewAfterCaptureCheck;

    CButton			m_BeginCaptureButton;
	CButton			m_CancelCaptureButton;

	float EffectiveFrameRate();
    float RequestedPlaybackFrameRate();
    
    void  PopulateNumFrames(void);
    void  PopulateCaptureTime(void);
    void  PopulatePlaybackRate(void);
    void  PopulateCameraFrameRate(void);
    void  PopulatePlaybackTime(void);
    void  PopulatePlaybackEffect(void);
    void  PopulateBitRate(void);
    void  PopulateFileSize(void);
    
	void SetGUIState(eGUIState state);
	void PopulateDefaults(void);
	void ApplyUserSettings(UserSettings& settings);
	void StoreUserSettings(UserSettings& settings);
	void PopulateControls(void);
	void ConfigureControls(void);
	void FeatureChanged(U32 featureId);

    afx_msg void OnKillFocusNumFramesEdit();
    afx_msg void OnKillFocusCapTimeEdit();
	afx_msg void OnSelchangeEncodingCombo();
	afx_msg void OnKillFocusClipDecimationFactorEdit();

	afx_msg void OnUpdateMatchCaptureCheck();
	afx_msg void OnKillFocusPbFrameRateEdit();

	afx_msg void OnUpdateAutoBitRateCheck();
	afx_msg void OnScrollBitRateSlider(void);
	afx_msg void OnChangeBitRateSlider(void);
	afx_msg void OnKillFocusBitRateEdit(void);

    afx_msg void OnKillFocusRawDataFileEdit();
	afx_msg void OnClickRawDataBrowseButton();

	afx_msg void OnKillFocusClipFileNameEdit();
	afx_msg void OnClickFormattedBrowseButton();
	afx_msg void OnSelchangeClipFormatCombo();

	afx_msg void OnClickBeginCaptureButton();

	afx_msg void OnUpdateRawDataFileEdit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRawDataFileBrowseButton(CCmdUI *pCmdUI);
	afx_msg void OnUpdateIncrementRawDataFileCheck(CCmdUI *pCmdUI);
	afx_msg void OnUpdateClipFormatCombo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileFormatCombo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFormattedFileEdit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateIncrementFormattedFileNameCheck(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewAfterCaptureCheck(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBeginCaptureButton(CCmdUI *pCmdUI);

public:
    afx_msg void OnBnClickedCgSaverawdataCheck();
};
