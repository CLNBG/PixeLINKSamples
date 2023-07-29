#pragma once
#include "afxwin.h"


// forward declaration
class CPxLCameraDoc;


// CSaveSettingsDialog dialog

class CSaveSettingsDialog : public CDialog
{
	DECLARE_DYNAMIC(CSaveSettingsDialog)

	DECLARE_MESSAGE_MAP()

public:
	CSaveSettingsDialog(CPxLCamera* pCam);
	virtual ~CSaveSettingsDialog();
	virtual BOOL OnInitDialog();

	int				m_channel;

// Dialog Data
	enum { IDD = IDD_SAVESETTINGS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

private:
	CComboBox		m_MemoryChannelCombo;
	CPxLCamera*		m_pCam;

};
