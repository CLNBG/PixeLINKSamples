#pragma once


#include "DevAppPage.h"
#include "LookupGraph.h"
#include <vector>
#include "afxwin.h"

//
// Simple class that supports setting the background color of a text edit control.
// If you use this again, pull this out of this LookupTable
//
class CReadOnlyColorEdit : public CEdit
{
public:
	CReadOnlyColorEdit();
	~CReadOnlyColorEdit();

	COLORREF GetDefaultBackgroundColor() const { return m_defaultBackgroundColor; }
	void	SetBackgroundColor(COLORREF rgb);

protected:
	COLORREF m_defaultBackgroundColor;
	COLORREF m_backgroundColor;
	CBrush m_brushBackground;

	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

// CLookupTableGui dialog

class CLookupTableGui : public CDevAppPage
{
	DECLARE_DYNCREATE(CLookupTableGui)

	DECLARE_MESSAGE_MAP()

	DECLARE_EVENTSINK_MAP()

public:
	CLookupTableGui(CPxLDevAppView* pView = NULL);
	~CLookupTableGui();
	virtual BOOL OnInitDialog(void);
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	int GetHelpContextId(void);

	enum { IDD = IDD_LOOKUPTABLES_GUI };
	UINT GetDlgId() { return IDD; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private:
	void SetGUIState(eGUIState state);
	void PopulateDefaults(void);
	void PopulateControls(void);
	void ConfigureControls(void);
	void FeatureChanged(U32 featureId);

	void PopulateListBox(std::vector<long> const& values);
	void PopulateGraph(void);
	void UpdateLookupTable(U32 flags = 0);
	void PopulateFFC(void);
	void PopulateTemperatures(void);
	void PopulateTemperature(U32 temperatureFeatureId, CReadOnlyColorEdit& control, float warmThreshold, float hotThreshold);

	afx_msg void OnClickEnableLookupCheck(void);
	afx_msg void OnClickShowHistogramCheck(void);
	afx_msg void OnClickAutoUpdateCheck(void);
	afx_msg void OnClickUpdateCameraButton(void);
	afx_msg void OnGraphChanged(void);
	afx_msg void OnClickEnableFfcCheck(void);

	afx_msg void OnUpdateAutoUpdateCheck(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUpdateCameraButton(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	CButton				m_EnableLookupCheck;
	CButton				m_AutoUpdateCheck;
	CButton				m_UpdateCameraButton;
	CListBox			m_LookupTableList;
	CLookupGraph		m_Graph;
	CEdit				m_FfcGainEdit;
	CButton				m_FfcEnableCheck;
	CReadOnlyColorEdit	m_sensorTemperatureEdit;
	CReadOnlyColorEdit	m_bodyTemperatureEdit;

	bool				m_bDialogCreated;
	bool				m_bAutoUpdate;
	std::vector<long>	m_table;
	std::vector<float>	m_floatTable;
	int					m_lookupTableSize;

	UINT_PTR		m_timerId;

	void StartUpdateTimer(void);
	void StopUpdateTimer(void);

};












