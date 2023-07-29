#pragma once


#include "FeatureEdit.h"


class CPxLDevAppView; // forward declaration
class CPxLCameraDoc;
class CSlider;


#include <map>
#include "SliderMacros.h"


// Macro used inside methods to temporarily set one of the m_bAdjusting bools
// to true, inside the scope of a code block.
#define LOCK_FEATURE(id)									\
	TempVal<bool> _temp_val(m_bAdjusting[FEATURE_ ## id], true)	\


/******************************************************************************
* Class CDevAppPage
******************************************************************************/

class CDevAppPage : public CPropertyPage
{
	friend class CRoiDialog2;

	DECLARE_DYNCREATE(CDevAppPage)

	DECLARE_MESSAGE_MAP()

public:
	CDevAppPage(UINT nID = 0, CPxLDevAppView* pView = NULL);
	~CDevAppPage();
	virtual BOOL OnInitDialog();
	virtual int GetHelpContextId();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);

	enum { IDD = 0 };
	virtual UINT GetDlgId() { return IDD; }

	bool GetEnabled(void);
	void SetEnabled(bool bEnabled);
	virtual BOOL OnSetActive();
	virtual void DocumentChanged(int change); // virtual, but you probably don't need to override it
	virtual void ApplyUserSettings(UserSettings& settings);
	virtual void StoreUserSettings(UserSettings& settings);
	virtual void GetConfigFileData(std::vector<byte>& cfgData);
	virtual void SetFromConfigFileData(byte const* pData, int nBytes);
	virtual void ModeChanged(bool advanced);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CPxLCameraDoc* GetDocument(void);
	CPxLCamera* GetActiveCamera(void);
	void PopulateCameraSelect(void);

	// Virtuals that you should override:
	virtual void SetGUIState(eGUIState state);
	virtual void ConfigureControls(void);
	virtual void PopulateControls(void);
	virtual void PopulateDefaults(void);
	virtual void FeatureChanged(U32 featureId);

	// Virtuals that you probably don't need to override:
	virtual void EnableCameraSelect(bool enable);
	virtual void EnablePreviewButtons(bool enablePlay, bool enablePause, bool enableStop);

	void SetSliderRange(CSlider& slider, U32 featureId);

	afx_msg virtual void OnDropdownCameraSelectCombo();
	afx_msg virtual void OnSelchangeCameraSelectCombo();
	afx_msg virtual void OnClickPlayButton(void);
	afx_msg virtual void OnClickPauseButton();
	afx_msg virtual void OnClickStopButton();

	void SetSliderDefault(CSlider& slider, float maxVal = 100.0f);
	void PopulateSlider(CSlider& slider, U32 featureId);
	void SliderScrolled(CSlider& slider, U32 featureId, int paramNo=0);
	void SliderChanged(CSlider& slider, U32 featureId, int paramNo=0);
	void SliderEditFocusKilled(CSlider& slider, U32 featureId, int paramNo=0);

	virtual void OnApiError(PxLException const& e, bool cameraRelated = true);
	virtual void PopulateWindowSize(CComboBox& cb, bool exact=true, U32 featureId=FEATURE_ROI);
	virtual void WindowsizeComboChanged(CComboBox& cb, U32 featureId=FEATURE_ROI, bool disabled=false);

// Member data
protected:
	// Used to prevent unneccesary extra calls to PxLSetFeature. Set to true
	// while a control is being programmatically set to the value read by
	// a call to PxLGetFeature. eg: We read the PixelFormat, then set the
	// selected item in the PixelFormat to display the value. This causes
	// the OnSelChange handler to be called, which normally calls PxLSetFeature.
	// We prevent this by setting m_bAdjusting[FEATURE_PIXEL_FORMAT] to true
	// just before calling SetCurSel, and back to false after. The OnSelChange
	// handler checks m_bAdjusting, and does nothing if it is true.
	std::map<U32, bool>		m_bAdjusting;

	// Used to determine whether a page needs to be reconfigured (eg: set
	// slider ranges, etc.) when it becomes the active page.
	U32						m_lastUsedSerialNumber;

	CPxLDevAppView*			m_pView;
	bool					m_bEnabled;

protected:
	CComboBox		m_CameraSelectCombo;
	CButton			m_PlayButton;
	CButton			m_PauseButton;
	CButton			m_StopButton;

};



