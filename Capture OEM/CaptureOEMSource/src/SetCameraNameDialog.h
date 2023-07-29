#pragma once
#include "afxwin.h"

// forward declaration
class CPxLCameraDoc;

// CSetCameraNameDialog dialog

class CSetCameraNameDialog : public CDialog
{
	DECLARE_DYNAMIC(CSetCameraNameDialog)

	DECLARE_MESSAGE_MAP()

public:
	CSetCameraNameDialog(CPxLCameraDoc* pDoc);   // standard constructor
	virtual ~CSetCameraNameDialog();
	virtual BOOL OnInitDialog();

	CString		m_newName;

// Dialog Data
	enum { IDD = IDD_SETCAMERANAME_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private:
	CPxLCameraDoc* m_pDoc;

	CStatic		m_SerialNumberLabel;
	CStatic		m_VendorNameLabel;
	CStatic		m_ModelNameLabel;
	CEdit		m_CameraNameEdit;
	CButton		m_OKButton;

	afx_msg void OnBnClickedOk();

};
