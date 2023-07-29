// AviCaptureDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CAviCaptureDlg dialog
class CAviCaptureDlg : public CDialog
{
// Construction
public:
	CAviCaptureDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_AVICAPTURE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT	OnCaptureFinished(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnCaptureUpdate	 (WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnConversionFinished	(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnConversionUpdate		(WPARAM wParam, LPARAM lParam);
	afx_msg void	OnTimer(UINT_PTR nIDEvent);

	enum State {
		STATE_IDLE = 0,
		STATE_CAPTURING = 1,
		STATE_CONVERTING
	} ;


	void	SetState(State newState);
	State m_state;

	bool StartCapturing();
	bool StopCapturing();
	bool StartConverting();
	bool StopConverting();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCapture();
	CButton m_btnCapture;
	CProgressCtrl m_ctlProgress;
	afx_msg void OnBnClickedOk();
	CStatic m_txtMessage;
	CButton m_btnOK;
};
