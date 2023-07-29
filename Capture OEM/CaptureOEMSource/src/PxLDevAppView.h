#pragma once

#include "PxLCameraDoc.h"
#include "DevAppTabs.h"
#include "SimpleGui.h"
#include "RoiGui.h"
#include "DescriptorGui.h"
#include "LookupTableGui.h"
#include "SpecializationsGui.h"
#include "GpioGui.h"
#include "ImagesGui.h"
#include "ClipsGui.h"
#include "CallbacksGui.h"
#include "LinkGui.h"
#include "LightingGui.h"
#include "LensGui.h"


class CApiTextLogger;


class CPxLDevAppView : public CFormView
{
	DECLARE_DYNCREATE(CPxLDevAppView)

	DECLARE_MESSAGE_MAP()

protected: // create from serialization only
	CPxLDevAppView();

public:
	enum{ IDD = IDD_PXLDEMO_FORM };

	virtual ~CPxLDevAppView();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	CPxLCameraDoc* GetDocument();
	CPxLCamera* GetActiveCamera(void);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	virtual void PostNcDestroy();

public:
	void SetMinMaxInfo(POINT& ptMinSize, POINT& ptMaxSize, CRect& wndRect);
	void GetConfigFileData(std::vector<byte>& cfgData);
	void SetFromConfigFileData(std::vector<byte> const& cfgData, int startPos);

	bool IsAdvancedMode(void);

private:
	afx_msg void OnDestroy();
	afx_msg void OnIdleUpdate(void);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);

	// Menu handlers
	afx_msg void OnViewRestoreGuiDefaults();
	afx_msg void OnViewAdvancedmode();

	// Idle update handlers for menu items
	afx_msg void OnUpdateViewAdvancedMode(CCmdUI *pCmdUI);

private:

	CDevAppTabs		m_Tabs;

	//CApiTextLogger*		m_pCallLogger;
	//CListBox			m_CallLog;

	CSimpleGui			m_SimpleGui;
	CRoiGui				m_RoiGui;
	CLensGui           m_LensGui;
	CDescriptorGui		m_DescriptorGui;
	CLookupTableGui	m_LookupTableGui;
	CKneePointGui		m_KneePointGui;
	CGpioGui			   m_GpioGui;
	CLinkGui          m_LinkGui;
   CLightingGui      m_LightingGui;
   CImagesGui			m_ImagesGui;
	CClipsGui			m_ClipsGui;
	CCallbacksGui		m_CallbackGui;
#ifdef SHOW_EXAMPLE_GUI
	CExampleGui			m_ExampleGui;
#endif

	bool			m_bAdvancedMode;
	long			m_AdvancedModeWindowWidth;
	long			m_AdvancedModeWindowHeight;

	long			m_SimpleModeWindowWidth;
	long			m_SimpleModeWindowHeight;

	void SetViewMode(bool advancedMode);
	void SetSimpleMode(void);
	void SetAdvancedMode(void);
	void SetSimpleModeWindowLayout(void);
	void SetAdvancedModeWindowLayout(void);
	void ApplyUserSettings(void);
	void StoreUserSettings(void);

};

#ifndef _DEBUG  // debug version in PxLDevAppView.cpp
inline CPxLCameraDoc* CPxLDevAppView::GetDocument()
   { return (CPxLCameraDoc*)m_pDocument; }
#endif














