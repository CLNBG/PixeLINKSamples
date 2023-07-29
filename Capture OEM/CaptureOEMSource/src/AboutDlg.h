#pragma once
#include "afxwin.h"
#include <string>

#define NOT_INSTALLED _T("Not Installed")

// Forward declaration
class CPxLCameraDoc;


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
	DECLARE_DYNAMIC(CAboutDlg)

	DECLARE_MESSAGE_MAP()

public:
	CAboutDlg(CPxLCameraDoc* pDoc);

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG

private:
	CPxLCameraDoc* m_pDoc;

	void AppendTimeStampToClipboard();
	void AppendOSVersionInfoToClipboard();
	void AppendClipboardInfo(const CString& msg);
	void AppendClipboardInfo(const CString& name, const CString& value);
	void AppendClipboardInfo(const CString& name, int value);

	void AppendCameraFeatures();
	void AppendFeatureInfo(const U32 featureId);
	void AppendFlags(const U32 flags);



	std::string m_infoForClipboard;

public:
	virtual BOOL OnInitDialog();
private:
	CStatic m_ReleaseVersionLabel;
	CStatic m_AppVersionLabel;
	CStatic m_ApiVersionLabel;
	CStatic m_DriverVersionLabel;
	CStatic m_UsbDriverVersionLabel;
	CStatic m_Usb3DriverVersionLabel;
	CStatic m_GevDriverVersionLabel;
	CStatic m_SerialNumberLabel;
	CStatic m_VendorNameLabel;
	CStatic m_ModelNameLabel;
	CStatic m_CameraNameLabel;
	CStatic m_FWVersionLabel;
	CStatic m_BootVersionLabel;
	CStatic m_FPGAVersionLabel;
	CStatic m_FFCTypeLabel;
	CStatic m_MacAddressLabel;
	CStatic m_IPAddressLabel;
	CStatic m_IPEngineFirmwareLabel;
	CStatic m_XmlVersionLabel;
	CStatic m_LensDescriptionLabel;
public:
	afx_msg void OnBnClickedBtnCopyToClipboard();
};

