#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CSetIPAddressDialog dialog

class CSetIPAddressDialog : public CDialog
{
	DECLARE_DYNAMIC(CSetIPAddressDialog)

public:
	CSetIPAddressDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetIPAddressDialog();

// Dialog Data
	enum { IDD = IDD_SETIPADDRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	CComboBox m_cmbCamera;
	CIPAddressCtrl m_ctlIPAddress;
	CIPAddressCtrl m_ctlSubnetMask;
	CIPAddressCtrl m_ctlGateway;
	CButton m_chkPersistent;
	afx_msg void OnBnClickedBtnSetIPAddress();

private:
	bool	ReadIPCameraIdInfo();
	void	UpdateControls();
	std::vector<CAMERA_ID_INFO> m_ipCameraIdInfo;	// holds only IP cameras
public:
	afx_msg void OnCbnSelchangeCamera();
	CButton m_btnSetIPAddress;
	CStatic m_txtNoCamerasDetected;
	afx_msg void OnBnClickedButtonDetect();
};
