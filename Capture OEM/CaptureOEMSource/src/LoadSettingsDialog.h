#pragma once
#include "afxwin.h"


// forward declaration
class CPxLCameraDoc;


// CLoadSettingsDialog dialog

class CLoadSettingsDialog : public CDialog
{
	DECLARE_DYNAMIC(CLoadSettingsDialog)

	DECLARE_MESSAGE_MAP()

public:
	CLoadSettingsDialog(CPxLCamera* pCam);
	virtual ~CLoadSettingsDialog();
	virtual BOOL OnInitDialog();

	int				m_channel;

// Dialog Data
	enum { IDD = IDD_LOADSETTINGS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

private:
	CComboBox		m_MemoryChannelCombo;
	CButton			m_DefaultRadio;
	CButton			m_UserRadio;
	CPxLCamera*		m_pCam;

	afx_msg void OnKickIdle(void);
	afx_msg void OnUpdateMemoryChannelCombo(CCmdUI* pCmdUI);

};
