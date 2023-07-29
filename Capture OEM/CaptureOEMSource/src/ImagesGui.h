#pragma once


#include "DevAppPage.h"
#include "afxwin.h"


// CImagesGui dialog

class CImagesGui : public CDevAppPage
{
	friend class CTimeLapseCaptureDialog;
	friend class CImageCaptureDialog;

	DECLARE_DYNAMIC(CImagesGui)

	DECLARE_MESSAGE_MAP()

public:
	CImagesGui(CPxLDevAppView* pView = NULL);
	virtual ~CImagesGui();
	int GetHelpContextId(void);
	virtual BOOL OnInitDialog(void);
	virtual BOOL OnKillActive(void);
	virtual BOOL OnSetActive(void);

	enum { IDD = IDD_IMAGES_GUI };
	UINT GetDlgId() { return IDD; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private:
	bool			m_bCameraSupportsAutoExposure;

	CEdit			m_FileNameEdit;
	CButton			m_FileBrowseButton;
	CComboBox		m_FileFormatCombo;
	CButton			m_IncrementAfterCaptureCheck;
	CButton			m_PreviewAfterCaptureCheck;
	CButton			m_CaptureFullFrameCheck;
	CButton			m_CaptureFullResCheck;

	CButton			m_TimeLapseCaptureCheck;
	CEdit			m_CaptureEveryEdit;
	CComboBox		m_CaptureEveryCombo;
	CEdit			m_CaptureForEdit;
	CComboBox		m_CaptureForCombo;
	CButton			m_AutoExposeCheck;
	CButton			m_AverageFramesCheck;
	CEdit			m_AverageFramesEdit;

    CButton         m_CaptureButton;

	void SetGUIState(eGUIState state);
	void PopulateDefaults(void);
	void ApplyUserSettings(UserSettings& settings);
	void StoreUserSettings(UserSettings& settings);
	void PopulateControls(void);
	void ConfigureControls(void);
	void FeatureChanged(U32 featureId);

	void SetUnitOfTimeEntries(CComboBox& cb, bool addFramesEntry=false);
	
	bool DoCapture(void);

	afx_msg void OnClickFileBrowseButton(void);
	afx_msg void OnKillFocusFileNameEdit(void);
	afx_msg void OnSelchangeFileFormatCombo(void);
	afx_msg void OnClickCaptureButton(void);

	afx_msg void OnUpdateIncrementFileNameCheck(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCaptureEveryEdit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCaptureEveryCombo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCaptureForEdit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCaptureForCombo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAutoExposeCheck(CCmdUI *pCmdUI);

};














