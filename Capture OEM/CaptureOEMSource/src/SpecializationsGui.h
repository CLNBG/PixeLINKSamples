#pragma once


#include "DevAppPage.h"
#include "CSlider.h"
#include "HistogramCtrl.h"
#include "afxwin.h"


// CKneePointGui dialog

class CKneePointGui : public CDevAppPage
{
	DECLARE_DYNCREATE(CKneePointGui)

	DECLARE_MESSAGE_MAP()

	DECLARE_EVENTSINK_MAP()

public:
	CKneePointGui(CPxLDevAppView* pView = NULL);
	~CKneePointGui();
	int GetHelpContextId();
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();

	enum { IDD = IDD_SPECIALIZATIONS_GUI };
	UINT GetDlgId() { return IDD; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private:
	void SetGUIState(eGUIState state);
	void PopulateDefaults(void);
	void PopulateControls(void);
	void ConfigureControls(void);
	void FeatureChanged(U32 featureId);

	void PopulateExposure(void);
	void PopulateKneePoints(void);
	void PopulatePixelFormat(void);
	void SetPixelFormatEntries(void);
	void SetPixelFormatInterpretationEntries(void);
	void PopulatePolars(void);
	void PopulatePtp(void);
	void PopulatePeriodicPtp(void);

	void SetKneePoints(void);

// data members
	int m_MaxKnees;

	CSlider			m_ExposureSlider;
	CFeatureEdit	m_ExposureEdit;
	CButton			m_ExposureAutoButton;
	CStatic			m_ExposureMaxLabel;

	CButton			m_KPCheck1;
	CSlider			m_KPSlider1;
	CFeatureEdit	m_KPEdit1;
	CStatic			m_KPMinLabel1;
	CStatic			m_KPMaxLabel1;

	CButton			m_KPCheck2;
	CSlider			m_KPSlider2;
	CFeatureEdit	m_KPEdit2;
	CStatic			m_KPMinLabel2;
	CStatic			m_KPMaxLabel2;

	CButton			m_KPCheck3;
	CSlider			m_KPSlider3;
	CFeatureEdit	m_KPEdit3;
	CStatic			m_KPMinLabel3;
	CStatic			m_KPMaxLabel3;

	CButton			m_KPCheck4;
	CSlider			m_KPSlider4;
	CFeatureEdit	m_KPEdit4;
	CStatic			m_KPMinLabel4;
	CStatic			m_KPMaxLabel4;

	CStatic			m_KPNote;
	CWnd			m_ColorBar;

	CComboBox		m_PixelFormatCombo;
    CComboBox       m_PixelFormatInterpretationCombo;

	CButton			m_KPMinAllPolarsButton;
	CButton			m_KPMaxAllPolarsButton;

    CSlider			m_KPPolar0Slider;
	CFeatureEdit	m_KPPolar0Edit;
	CStatic			m_KPPolar0MinLabel;
	CStatic			m_KPPolar0MaxLabel;

	CSlider			m_KPPolar45Slider;
	CFeatureEdit	m_KPPolar45Edit;
	CStatic			m_KPPolar45MinLabel;
	CStatic			m_KPPolar45MaxLabel;

	CSlider			m_KPPolar90Slider;
	CFeatureEdit	m_KPPolar90Edit;
	CStatic			m_KPPolar90MinLabel;
	CStatic			m_KPPolar90MaxLabel;

	CSlider			m_KPPolar135Slider;
	CFeatureEdit	m_KPPolar135Edit;
	CStatic			m_KPPolar135MinLabel;
	CStatic			m_KPPolar135MaxLabel;

   	CButton			m_PtpOnOffCheck;
   	CButton			m_PtpSlaveOnlyCheck;
	CEdit			m_PtpStateEdit;
	CEdit			m_PtpDaysEdit;
	CEdit			m_PtpHoursEdit;
	CEdit			m_PtpMinsEdit;
	CEdit			m_PtpSecsEdit;

	UINT_PTR		m_timerId;

	void StartUpdateTimer(void);
	void StopUpdateTimer(void);

    // message handlers
	afx_msg void OnPaint();

	afx_msg void OnScrollExposureSlider(void);
	afx_msg void OnChangeExposureSlider(void);
	afx_msg void OnKillFocusExposureEdit(void);
	afx_msg void OnClickExposureAutoButton(void);

	afx_msg void OnScrollKneePointSlider1(void);
	afx_msg void OnChangeKneePointSlider1(void);
	afx_msg void OnKillFocusKneePointEdit1(void);
	afx_msg void OnClickKneePointCheck1(void);

	afx_msg void OnScrollKneePointSlider2(void);
	afx_msg void OnChangeKneePointSlider2(void);
	afx_msg void OnKillFocusKneePointEdit2(void);
	afx_msg void OnClickKneePointCheck2(void);

	afx_msg void OnScrollKneePointSlider3(void);
	afx_msg void OnChangeKneePointSlider3(void);
	afx_msg void OnKillFocusKneePointEdit3(void);
	afx_msg void OnClickKneePointCheck3(void);

	afx_msg void OnScrollKneePointSlider4(void);
	afx_msg void OnChangeKneePointSlider4(void);
	afx_msg void OnKillFocusKneePointEdit4(void);
	afx_msg void OnClickKneePointCheck4(void);

	afx_msg void OnUpdateKneePoint1(CCmdUI *pCmdUI);

	afx_msg void OnUpdateKneePointCheck2(CCmdUI *pCmdUI);
	afx_msg void OnUpdateKneePoint2(CCmdUI *pCmdUI);

	afx_msg void OnUpdateKneePointCheck3(CCmdUI *pCmdUI);
	afx_msg void OnUpdateKneePoint3(CCmdUI *pCmdUI);

	afx_msg void OnUpdateKneePointCheck4(CCmdUI *pCmdUI);
	afx_msg void OnUpdateKneePoint4(CCmdUI *pCmdUI);

   	afx_msg void OnSelchangePixelFormatCombo(void);
	afx_msg void OnSelchangePixelFormatInterpretationCombo(void);

   	afx_msg void OnClickMinAllPolarsButton(void);
   	afx_msg void OnClickMaxAllPolarsButton(void);

   	afx_msg void OnScrollPolar0Slider(void);
	afx_msg void OnChangePolar0Slider(void);
	afx_msg void OnKillFocusPolar0Edit(void);
   	afx_msg void OnScrollPolar45Slider(void);
	afx_msg void OnChangePolar45Slider(void);
	afx_msg void OnKillFocusPolar45Edit(void);
   	afx_msg void OnScrollPolar90Slider(void);
	afx_msg void OnChangePolar90Slider(void);
	afx_msg void OnKillFocusPolar90Edit(void);
   	afx_msg void OnScrollPolar135Slider(void);
	afx_msg void OnChangePolar135Slider(void);
	afx_msg void OnKillFocusPolar135Edit(void);

   	afx_msg void OnClickPtpOnOffCheck(void);
   	afx_msg void OnClickPtpSlaveOnlyCheck(void);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

};













