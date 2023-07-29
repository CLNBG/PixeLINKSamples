// LoadSettingsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "LoadSettingsDialog.h"
#include "PxLCameraDoc.h"
#include <afxpriv.h>


// CLoadSettingsDialog dialog

IMPLEMENT_DYNAMIC(CLoadSettingsDialog, CDialog)


/**
* Function: CLoadSettingsDialog
* Purpose:  Constructor
*/
CLoadSettingsDialog::CLoadSettingsDialog(CPxLCamera* pCam)
	: CDialog(CLoadSettingsDialog::IDD)
	, m_pCam(pCam)
	, m_channel(0)
{
}


/**
* Function: ~CLoadSettingsDialog
* Purpose:  Destructor
*/
CLoadSettingsDialog::~CLoadSettingsDialog()
{
}


/**
* Function: DoDataExchange
* Purpose:  
*/
void CLoadSettingsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LCSD_MEMORYCHANNEL_COMBO, m_MemoryChannelCombo);
	DDX_Control(pDX, IDC_LCSD_DEFAULT_RADIO, m_DefaultRadio);
	DDX_Control(pDX, IDC_LCSD_USER_RADIO, m_UserRadio);
}


BEGIN_MESSAGE_MAP(CLoadSettingsDialog, CDialog)
	ON_MESSAGE_VOID(WM_KICKIDLE, OnKickIdle)
	ON_UPDATE_COMMAND_UI(IDC_LCSD_MEMORYCHANNEL_COMBO, OnUpdateMemoryChannelCombo)
END_MESSAGE_MAP()


// CLoadSettingsDialog message handlers

/**
* Function: OnOK
* Purpose:  
*/
void CLoadSettingsDialog::OnOK()
{
	if (m_DefaultRadio.GetCheck() == BST_CHECKED)
		m_channel = FACTORY_DEFAULTS_MEMORY_CHANNEL;
	else
		m_channel = ItemData<int>(m_MemoryChannelCombo);	

	CDialog::OnOK();
}


/**
* Function: OnInitDialog
* Purpose:  
*/
BOOL CLoadSettingsDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	for (int i = static_cast<int>(m_pCam->GetFeatureMinVal(FEATURE_MEMORY_CHANNEL));
			i <= static_cast<int>(m_pCam->GetFeatureMaxVal(FEATURE_MEMORY_CHANNEL));
			i++)
	{
		if (i == FACTORY_DEFAULTS_MEMORY_CHANNEL)
			continue; // We have radio button for this.

		CString s;
		s.Format(_T("Channel %d"), i);
		m_MemoryChannelCombo.SetItemData(
				m_MemoryChannelCombo.AddString(s),
				static_cast<DWORD_PTR>(i));
	}

	m_MemoryChannelCombo.SetCurSel(0);
	m_DefaultRadio.SetCheck(BST_CHECKED);

	return TRUE;
}


/**
* Function: OnKickIdle
* Purpose:  
*/
void CLoadSettingsDialog::OnKickIdle(void)
{
	UpdateDialogControls(this, FALSE);
}


/**
* Function: OnUpdateMemoryChannelCombo
* Purpose:  
*/
void CLoadSettingsDialog::OnUpdateMemoryChannelCombo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_UserRadio.GetCheck() == BST_CHECKED);
}







