#pragma once


#include "DevAppPage.h"
#include "CSlider.h"
#include "afxwin.h"


struct CallbackDesc
{
	PxLApiCallback	callback;
	void*			callbackParam;
	TCHAR const*	callbackName;
	bool			requiresFile;
};


/******************************************************************************
* CCallbacksGui dialog
******************************************************************************/

class CCallbacksGui : public CDevAppPage
{
	friend class CLensGui; // Allow the Lens Gui to set callbacks

	DECLARE_DYNAMIC(CCallbacksGui)

	DECLARE_MESSAGE_MAP()

	DECLARE_EVENTSINK_MAP()

public:
	CCallbacksGui(CPxLDevAppView* pView = NULL);
	virtual ~CCallbacksGui();
	int GetHelpContextId(void);
	virtual BOOL OnInitDialog();
	static void EnableSSRoiCallback (bool enable, CCallbacksGui* pCallbackGui);
	static bool SSRoiCallbackEnabled (CCallbacksGui* pCallbackGui);
   static void EnableAutoRoiCallback(bool enable, CCallbacksGui* pCallbackGui);
   static bool AutoRoiCallbackEnabled(CCallbacksGui* pCallbackGui);

	enum { IDD = IDD_CALLBACKS_GUI };
	UINT GetDlgId() { return IDD; }

	virtual void GetConfigFileData(std::vector<byte>& cfgData);
	virtual void SetFromConfigFileData(byte const* pData, int nBytes);

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

	void UpdateCallback(CallbackDesc* pcb, U32 overlayFlags);
	void PopulateCallbackCombo(CComboBox& cb, PxLApiCallback callback);
	void PopulateSharpnessOnOff(void);

//	void PopulateSharpness(void);
	SLIDER_HANDLER_SUITE(Sharpness)

	afx_msg void OnSelchangePreviewFunctionCombo(void);
	afx_msg void OnKillFocusPreviewFileEdit(void);
	afx_msg void OnClickPreviewBrowseButton(void);

	afx_msg void OnSelchangeImageFunctionCombo(void);
	afx_msg void OnKillFocusImageFileEdit(void);
	afx_msg void OnClickImageBrowseButton(void);

	afx_msg void OnSelchangeClipFunctionCombo(void);
	afx_msg void OnKillFocusClipFileEdit(void);
	afx_msg void OnClickClipBrowseButton(void);

	afx_msg void OnUpdatePreviewFileEdit(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImageFileEdit(CCmdUI* pCmdUI);
	afx_msg void OnUpdateClipFileEdit(CCmdUI* pCmdUI);

	afx_msg void OnClickSharpnessOnOffCheck(void);

	CComboBox	m_PreviewFunctionCombo;
	CEdit		m_PreviewFileEdit;
	CButton		m_PreviewBrowseButton;

	CComboBox	m_ImageFunctionCombo;
	CEdit		m_ImageFileEdit;
	CButton		m_ImageBrowseButton;

	CComboBox	m_ClipFunctionCombo;
	CEdit		m_ClipFileEdit;
	CButton		m_ClipBrowseButton;

	SLIDER_CONTROL_SUITE(Sharpness)
	CButton			m_SharpnessOnOffCheck;

public:
    afx_msg void OnEnChangeCbSharpnessEdit();
};
