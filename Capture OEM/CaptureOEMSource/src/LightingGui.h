#pragma once


#include "DevAppPage.h"
#include "SubwindowCtrl.h"
#include "CSlider.h"
#include "afxwin.h"



// CLightingGui dialog

class CLightingGui : public CDevAppPage
{
	DECLARE_DYNCREATE(CLightingGui)

	DECLARE_MESSAGE_MAP()

	DECLARE_EVENTSINK_MAP()

public:
	CLightingGui(CPxLDevAppView* pView = NULL);
	~CLightingGui(void);
	int GetHelpContextId(void);
	BOOL OnInitDialog(void);
	virtual BOOL OnSetActive();

	enum { IDD = IDD_LIGHTING_GUI };
	UINT GetDlgId() { return IDD; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private:
	void SetGUIState(eGUIState state);
	
	void PopulateDefaults(void);
	void PopulateControls(void);
	void ConfigureControls(void);
	void FeatureChanged(U32 featureId);

   void PopulateControllerSelect(void);
   void PopulateLightingBrightnessOnOff(void);
   void PopulateLightingControls(void);

   CComboBox	 m_ControllerSelectCombo;
   afx_msg void OnDropdownControllerSelectCombo(void);
   afx_msg void OnSelchangeControllerSelectCombo(void);
   CStatic      m_CantOpenComWarningStatic;
   
   CButton      m_LightingBrightnessOnOffCheck;
   SLIDER_HANDLER_SUITE(LightingBrightness)
   afx_msg void OnClickLightingBrightnessOnOffCheck(void);
   SLIDER_CONTROL_SUITE(LightingBrightness)

	CFeatureEdit    m_CurrentEdit;
   CFeatureEdit    m_VoltageEdit;
   CFeatureEdit    m_TemperatureEdit;
   CButton		    m_UpdateButton;
   afx_msg void OnClickUpdateButton();

   U32             m_MyController; // 0 == I don't have a controller assigned
   bool            m_bDontWantController;

};











