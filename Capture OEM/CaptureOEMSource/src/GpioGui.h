#pragma once


#include <afxmt.h>
#include "DevAppPage.h"
#include "afxwin.h"


// CGpioGui dialog

class CGpioGui : public CDevAppPage
{
	friend U32 __stdcall EventHandler(HANDLE,U32, double, U32, LPVOID, LPVOID);

    DECLARE_DYNAMIC(CGpioGui)

	DECLARE_MESSAGE_MAP()

public:
	CGpioGui(CPxLDevAppView* pView = NULL);
	virtual ~CGpioGui();

	enum { IDD = IDD_GPIOS_GUI };
	UINT GetDlgId() { return IDD; }
	virtual BOOL OnInitDialog(void);
	int GetHelpContextId(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private:
	void SetGUIState(eGUIState state);
	void PopulateDefaults(void);
	void PopulateControls(void);
	void ConfigureControls(void);
	void FeatureChanged(U32 featureId);

	void PopulateTrigger(void);
	void PopulateGpo(void);
	void ClearTriggerFields(void);
	void ClearGpoFields(void);
	void SetGpoNumberEntries(CComboBox& cb);
	void SetLabels(void);

	void UpdateTrigger(void);
	void UpdateGpo(void);

	void PopulateActionCommands();
    void SendAction(void);

	CMutex*		m_pLock;

    CButton		m_TriggerOnOffCheck;
	CComboBox	m_TriggerModeCombo;
	CComboBox	m_TriggerTypeCombo;
	CComboBox	m_TriggerPolarityCombo;
	CEdit		m_TriggerDelayEdit;
	CStatic		m_TriggerParameterLabel;
	CEdit		m_TriggerParameterEdit;
	CButton	    m_TriggerControlledLightingCheck;

	CButton		m_GpoOnOffCheck;
	CComboBox	m_GpoNumberCombo;
	CComboBox	m_GpoModeCombo;
	CComboBox	m_GpoPolarityCombo;
	CEdit		m_GpoParameter1Label;
	CEdit		m_GpoParameter2Label;
	CEdit		m_GpoParameter3Label;
	CEdit		m_GpoParameter1Edit;
	CEdit		m_GpoParameter2Edit;
	CEdit		m_GpoParameter3Edit;
	CEdit		m_GpoParameter1Units;
	CEdit		m_GpoParameter2Units;
	CEdit		m_GpoParameter3Units;

	CButton		m_UpdateTriggerButton;
    CStatic     m_ExposureWarningStatic;
	CButton		m_UpdateGpoButton;

	CEdit		m_TriggerDescription;
	CEdit		m_GpoDescription;

	CComboBox	m_ActionCommandCombo;
	CEdit		m_ActionDelayEdit;
	CButton		m_SendActionButton;

	CEdit		m_EventsEdit;
	CButton		m_ClearActionsButton;

    afx_msg void OnSelchangeGpoNumberCombo(void);

	afx_msg void OnUpdateTriggerParameterEdit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGpoParameter1Edit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGpoParameter2Edit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGpoParameter3Edit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateUpdateTriggerButton(CCmdUI *pCmdUI);
	afx_msg void OnUpdateUpdateGpoButton(CCmdUI *pCmdUI);

	afx_msg void OnClickUpdateTriggerButton();
	afx_msg void OnClickUpdateGpoButton();

	afx_msg void OnSelchangeTriggerModeCombo();
	afx_msg void OnSelchangeGpoModeCombo();

	afx_msg void OnClickSendActionButton();

	afx_msg void OnClickClearActionsButton();

public:
	afx_msg void OnClickTriggerOnOffCheck();
	afx_msg void OnClickGpoOnOffCheck();
    afx_msg void OnBnClickedGgControlledLightingCheck();
};
