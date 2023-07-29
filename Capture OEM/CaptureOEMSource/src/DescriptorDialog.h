#pragma once


#include <vector>


class CDescriptorGui;


// Struct used to associate an edit box and a button with the feature that
// they display / control.
struct ControlSet
{
	CEdit*		pEdit;
	CButton*	pButton;
	U32			featureId;
	ControlSet(CEdit* pe, CButton* pb, U32 fid)
		: pEdit(pe), pButton(pb), featureId(fid)
	{}
};


class CDescriptorGui;


class CDescriptorDialog : public CDialog
{
	DECLARE_DYNAMIC(CDescriptorDialog)

public:
	CDescriptorDialog(CDescriptorGui* pPage, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDescriptorDialog();
	virtual BOOL OnInitDialog();

	void OnApiError(PxLException const& e, bool cameraRelated = true);

	void EnableControls(bool enable);
	void ClearControls(void);
	void PopulateControls(void);
	void FeatureChanged(U32 featureId);

	void KillPopup();

// Dialog Data
	enum { IDD = IDD_DESCRIPTOR_EDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CDescriptorGui* m_pPage;
	std::vector<ControlSet> m_controls;
	bool m_bAdjusting;
	CWnd*	m_pCurrentPopup;

	CPxLCamera*		GetActiveCamera(void);

	void ShowFeatureDlg(U32 featureId, CWnd& position);
	void ShowSliderDlg(U32 featureId, CWnd& position);
	void ShowFlipDlg(CWnd& position);
	void ShowRoiDlg(U32 featureId, CWnd& position);
	void ShowTriggerDlg(CWnd& position);
	void ShowGpoDialog(CWnd& position);
	void ShowPixelFormatDialog(CWnd& position);
	void ShowDecimationDialog(CWnd& position);
	void ShowExtendedShutterDialog(CWnd& position);
	void ShowWhiteShadingDialog(CWnd& position);
	void ShowRotationDialog(CWnd& position);

	void ScrollToControl(CWnd& wnd);

	ControlSet* FindControlSet(U32 featureId);

	void PopulateEditCtrl(U32 featureId, CWnd* wnd = NULL);
	void PopulateFloatField(U32 featureId, CWnd* wnd);
	void PopulateRectField(U32  featureId, CWnd* wnd);
	void PopulateTriggerField(CWnd* wnd);
	void PopulateGpoField(CWnd* wnd);
	void PopulateFlipField(CWnd* wnd);
	void PopulatePixelFormatField(CWnd* wnd);
	void PopulateDecimation(CWnd* wnd);
	void PopulateExtendedShutter(CWnd* wnd);
	void PopulateWhiteShading(CWnd* wnd);

	afx_msg void OnSetFocusBrightnessEdit();
	afx_msg void OnSetFocusExposureEdit();
	afx_msg void OnSetFocusSharpnessEdit();
	afx_msg void OnSetFocusColorTempEdit();
	afx_msg void OnSetFocusHueEdit();
	afx_msg void OnSetFocusSaturationEdit();
	afx_msg void OnSetFocusGammaEdit();
	afx_msg void OnSetFocusShutterEdit();
	afx_msg void OnSetFocusGainEdit();
	afx_msg void OnSetFocusIrisEdit();
	afx_msg void OnSetFocusFocusEdit();
	afx_msg void OnSetFocusTriggerEdit();
	afx_msg void OnSetFocusZoomEdit();
	afx_msg void OnSetFocusPanEdit();
	afx_msg void OnSetFocusTiltEdit();
	afx_msg void OnSetFocusOpticalFilterEdit();
	afx_msg void OnSetFocusGpioEdit();
	afx_msg void OnSetFocusFrameRateEdit();
	afx_msg void OnSetFocusRoiEdit();
	afx_msg void OnSetFocusFlipEdit();
	afx_msg void OnSetFocusDecimationEdit();
	afx_msg void OnSetFocusPixelFormatEdit();
	afx_msg void OnSetFocusExtendedShutterEdit();
	afx_msg void OnSetFocusWhiteShadingEdit();
	afx_msg void OnSetFocusRotationEdit();

	afx_msg void OnSetFocusBrightnessButton();
	afx_msg void OnSetFocusExposureButton();
	afx_msg void OnSetFocusSharpnessButton();
	afx_msg void OnSetFocusColorTempButton();
	afx_msg void OnSetFocusHueButton();
	afx_msg void OnSetFocusSaturationButton();
	afx_msg void OnSetFocusGammaButton();
	afx_msg void OnSetFocusShutterButton();
	afx_msg void OnSetFocusGainButton();
	afx_msg void OnSetFocusIrisButton();
	afx_msg void OnSetFocusFocusButton();
	afx_msg void OnSetFocusTriggerButton();
	afx_msg void OnSetFocusZoomButton();
	afx_msg void OnSetFocusPanButton();
	afx_msg void OnSetFocusTiltButton();
	afx_msg void OnSetFocusOpticalFilterButton();
	afx_msg void OnSetFocusGpioButton();
	afx_msg void OnSetFocusFrameRateButton();
	afx_msg void OnSetFocusRoiButton();
	afx_msg void OnSetFocusFlipButton();
	afx_msg void OnSetFocusDecimationButton();
	afx_msg void OnSetFocusPixelFormatButton();
	afx_msg void OnSetFocusExtendedShutterButton();
	afx_msg void OnSetFocusWhiteShadingButton();
	afx_msg void OnSetFocusRotationButton();

	afx_msg void OnClickBrightnessButton();
	afx_msg void OnClickExposureButton();
	afx_msg void OnClickSharpnessButton();
	afx_msg void OnClickColorTempButton();
	afx_msg void OnClickHueButton();
	afx_msg void OnClickSaturationButton();
	afx_msg void OnClickGammaButton();
	afx_msg void OnClickShutterButton();
	afx_msg void OnClickGainButton();
	afx_msg void OnClickIrisButton();
	afx_msg void OnClickFocusButton();
	afx_msg void OnClickTriggerButton();
	afx_msg void OnClickZoomButton();
	afx_msg void OnClickPanButton();
	afx_msg void OnClickTiltButton();
	afx_msg void OnClickOpticalFilterButton();
	afx_msg void OnClickGpioButton();
	afx_msg void OnClickFrameRateButton();
	afx_msg void OnClickRoiButton();
	afx_msg void OnClickFlipButton();
	afx_msg void OnClickDecimationButton();
	afx_msg void OnClickPixelFormatButton();
	afx_msg void OnClickExtendedShutterButton();
	afx_msg void OnClickWhiteShadingButton();
	afx_msg void OnClickRotationButton();

	CEdit m_BrightnessEdit;
	CEdit m_ExposureEdit;
	CEdit m_SharpnessEdit;
	CEdit m_ColorTempEdit;
	CEdit m_HueEdit;
	CEdit m_SaturationEdit;
	CEdit m_GammaEdit;
	CEdit m_ShutterEdit;
	CEdit m_GainEdit;
	CEdit m_IrisEdit;
	CEdit m_FocusEdit;
	CEdit m_TriggerEdit;
	CEdit m_ZoomEdit;
	CEdit m_PanEdit;
	CEdit m_TiltEdit;
	CEdit m_OpticalFilterEdit;
	CEdit m_GpioEdit;
	CEdit m_FrameRateEdit;
	CEdit m_RoiEdit;
	CEdit m_FlipEdit;
	CEdit m_DecimationEdit;
	CEdit m_PixelFormatEdit;
	CEdit m_ExtendedShutterEdit;
	CEdit m_WhiteShadingEdit;
	CEdit m_RotationEdit;

	CButton m_BrightnessButton;
	CButton m_ExposureButton;
	CButton m_SharpnessButton;
	CButton m_ColorTempButton;
	CButton m_HueButton;
	CButton m_SaturationButton;
	CButton m_GammaButton;
	CButton m_ShutterButton;
	CButton m_GainButton;
	CButton m_IrisButton;
	CButton m_FocusButton;
	CButton m_TriggerButton;
	CButton m_ZoomButton;
	CButton m_PanButton;
	CButton m_TiltButton;
	CButton m_OpticalFilterButton;
	CButton m_GpioButton;
	CButton m_FrameRateButton;
	CButton m_RoiButton;
	CButton m_FlipButton;
	CButton m_DecimationButton;
	CButton m_PixelFormatButton;
	CButton m_ExtendedShutterButton;
	CButton m_WhiteShadingButton;
	CButton m_RotationButton;

};
