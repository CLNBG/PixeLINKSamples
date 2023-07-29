#pragma once


#include "afxcmn.h"
#include "afxwin.h"

class CImagesGui;
// CTimeLapseCaptureDialog dialog

class CTimeLapseCaptureDialog : public CDialog
{
	DECLARE_DYNAMIC(CTimeLapseCaptureDialog)

	DECLARE_MESSAGE_MAP()

public:
	CTimeLapseCaptureDialog(CImagesGui* pParent);
	virtual ~CTimeLapseCaptureDialog();

// Dialog Data
	enum { IDD = IDD_TIMELAPSE_DIALOG };

	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private:
	CImagesGui*		m_parent;
	UINT_PTR		m_timer1Id;
	UINT_PTR		m_timer2Id;
	long			m_interval;
	int				m_captured;
    int             m_capturesInProgress; // Bugzilla.1148
	int				m_tocapture;
	DWORD			m_starttime;
	double			m_duration;
	DWORD			m_lastframetime;
	bool			m_bLimitByFrameCount;
	std::auto_ptr<CStreamState>	m_streamstate; // to start and restore stream state
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
	virtual void OnCancel();
private:
	CProgressCtrl m_Progress;
	CStatic m_Label1;
	CStatic m_Label2;
	CStatic m_Label3;

	void updateGUI();
};
