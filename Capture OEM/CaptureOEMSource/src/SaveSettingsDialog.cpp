// SaveSettingsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "SaveSettingsDialog.h"
#include "PxLCameraDoc.h"


// CSaveSettingsDialog dialog

IMPLEMENT_DYNAMIC(CSaveSettingsDialog, CDialog)

CSaveSettingsDialog::CSaveSettingsDialog(CPxLCamera* pCam)
	: CDialog(CSaveSettingsDialog::IDD)
	, m_pCam(pCam)
	, m_channel(0)
{
}

CSaveSettingsDialog::~CSaveSettingsDialog()
{
}

void CSaveSettingsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCSD_MEMORYCHANNEL_COMBO, m_MemoryChannelCombo);
}


BEGIN_MESSAGE_MAP(CSaveSettingsDialog, CDialog)
END_MESSAGE_MAP()


// CSaveSettingsDialog message handlers

void CSaveSettingsDialog::OnOK()
{
	m_channel = ItemData<int>(m_MemoryChannelCombo);	

	CDialog::OnOK();
}

BOOL CSaveSettingsDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	for (int i = static_cast<int>(m_pCam->GetFeatureMinVal(FEATURE_MEMORY_CHANNEL));
			i <= static_cast<int>(m_pCam->GetFeatureMaxVal(FEATURE_MEMORY_CHANNEL));
			i++)
	{
		if (i == FACTORY_DEFAULTS_MEMORY_CHANNEL)
			continue; // Can't save to this channel.

		CString s;
		s.Format(_T("Channel %d"), i);
		m_MemoryChannelCombo.SetItemData(
				m_MemoryChannelCombo.AddString(s),
				static_cast<DWORD_PTR>(i));
	}

	m_MemoryChannelCombo.SetCurSel(0);

	return TRUE;
}


























