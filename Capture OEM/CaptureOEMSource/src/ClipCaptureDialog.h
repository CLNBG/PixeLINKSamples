#pragma once
#include "afxcmn.h"


// CClipCaptureDialog dialog

class CClipCaptureDialog : public CDialog
{
	DECLARE_DYNAMIC(CClipCaptureDialog)

	DECLARE_MESSAGE_MAP()

public:
	CClipCaptureDialog(CPxLCameraDoc* pDoc,
					   HANDLE captureDoneEventHandle);
	virtual ~CClipCaptureDialog();

// Dialog Data
	enum { IDD = IDD_CLIPCAPTURE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();

public:
	void SetFrameRate(float rate) { m_FrameRate = rate; }
	void SetNumFrames(int num) { m_NumFrames = num; }
	bool CancelledByUser(void) { return m_bClipCancelledByUser; }

private:
	CProgressCtrl	m_ClipCaptureProgressBar;

	CPxLCameraDoc*	m_pDoc;
	HANDLE			m_CaptureDoneEventHandle;
	HANDLE			m_DialogClosedEventHandle;
	float			m_FrameRate;
	int				m_NumFrames;

	UINT_PTR		m_TimerId;
	DWORD			m_StartTime;
	bool			m_bClipCancelledByUser;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
