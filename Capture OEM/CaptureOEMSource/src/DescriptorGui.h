#pragma once


#include "DevAppPage.h"
#include "DescriptorDialog.h"
#include "ScrollBox.h"
#include "afxwin.h"


/////////////////////////////////////////////////////////////////////////////
// CDescriptorGui dialog

class CDescriptorGui : public CDevAppPage
{
	DECLARE_DYNCREATE(CDescriptorGui)

	DECLARE_MESSAGE_MAP()

public:
	friend class CDescriptorDialog;

	CDescriptorGui(CPxLDevAppView* pView = NULL);
	~CDescriptorGui();
	int GetHelpContextId();
	virtual BOOL OnInitDialog();

	enum { IDD = IDD_DESCRIPTORS_GUI };
	UINT GetDlgId() { return IDD; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private:
	CTreeCtrl			m_CameraTree;

	CButton				m_CreateDescriptorButton;
	CButton				m_DeleteDescriptorButton;
	CButton				m_DeleteAllButton;

	CButton				m_UpdateModeCameraRadio;
	CButton				m_UpdateModeHostRadio;
	CButton				m_UpdateNowButton;

	CStatic				m_DescriptorInFocusLabel;
	CScrollBox			m_DescriptorEditScrollBox;
	CDescriptorDialog	m_DescriptorDlg;

	HTREEITEM			m_hCameraNode;
	HTREEITEM			m_hInfoNode;
	HTREEITEM			m_hCameraIdInfoRootNode;
	HTREEITEM			m_hDescRootNode;

	void SetGUIState(eGUIState state);
	void PopulateDefaults(void);
	void PopulateControls(void);
	void ConfigureControls(void);
	void FeatureChanged(U32 featureId);

	void EmbedDescriptorDialog(void);

	void PopulateDescriptorControls(void);
	void UpdateDescriptorNodes(void);

	void UpdateCameraIdInfoNode(void);

	afx_msg void OnUpdateUpdateNowButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDeleteDescriptorButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDeleteAllButton(CCmdUI* pCmdUI);

	afx_msg void OnClickCreateDescriptorButton();
	afx_msg void OnClickDeleteDescriptorButton();
	afx_msg void OnClickDeleteAllButton();

	afx_msg void OnClickUpdateCameraRadio();
	afx_msg void OnClickUpdateHostRadio();
	afx_msg void OnClickUpdateNowButton();

	afx_msg void OnSelchangedCameraTree(NMHDR *pNMHDR, LRESULT *pResult);

public:
	virtual BOOL OnKillActive();
};



















