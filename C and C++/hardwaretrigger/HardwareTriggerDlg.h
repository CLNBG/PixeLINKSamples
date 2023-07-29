// HardwareTriggerDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxmt.h" // For the critical section
#include "WorkerThread.h"



// CHardwareTriggerDlg dialog
class CHardwareTriggerDlg : public CDialog
{
// Construction
public:
	CHardwareTriggerDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CHardwareTriggerDlg();

// Dialog Data
	enum { IDD = IDD_HARDWARETRIGGER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	virtual afx_msg LRESULT	OnWorkerThreadUpdate	(WPARAM wParam, LPARAM lParam);
	virtual afx_msg LRESULT	OnWorkerThreadFinished	(WPARAM wParam, LPARAM lParam);

public:
	afx_msg void OnBnClickedCapture();
	CStatic m_txtMessage;
	CButton m_btnCapture;
	CButton m_btnOK;

	void SetMessage(const CString& msg);

protected:
	BOOL InitializeCamera();
	void UninitializeCamera();
	BOOL IsTriggeringSupported(HANDLE hCamera);
	BOOL EnableHardwareTriggering(HANDLE hCamera);
	void StartCapturing();
	void StopCapturing();

	CCriticalSection	m_critSection;
	WorkerThread*		m_pWorkerThread;
	int					m_numImagesCaptured;

	static PXL_RETURN_CODE CaptureImagesToDisk(WorkerThread* pParam);

	HANDLE m_hCamera;
public:
	CComboBox m_cmbImageFormat;
};
