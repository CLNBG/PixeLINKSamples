// SetCameraNameDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "SetCameraNameDialog.h"
#include "PxLCameraDoc.h"


// CSetCameraNameDialog dialog

IMPLEMENT_DYNAMIC(CSetCameraNameDialog, CDialog)


/**
* Function: CSetCameraNameDialog
* Purpose:  
*/
CSetCameraNameDialog::CSetCameraNameDialog(CPxLCameraDoc* pDoc)
	: CDialog(CSetCameraNameDialog::IDD)
	, m_pDoc(pDoc)
{
}

/**
* Function: CSetCameraNameDialog
* Purpose:  
*/
CSetCameraNameDialog::~CSetCameraNameDialog()
{
}


/**
* Function: DoDataExchange
* Purpose:  
*/
void CSetCameraNameDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SCND_SERIALNUMBER_LABEL, m_SerialNumberLabel);
	DDX_Control(pDX, IDC_SCND_VENDORNAME_LABEL, m_VendorNameLabel);
	DDX_Control(pDX, IDC_SCND_MODELNAME_LABEL, m_ModelNameLabel);
	DDX_Control(pDX, IDC_SCND_CAMERANAME_EDIT, m_CameraNameEdit);
	DDX_Control(pDX, IDOK, m_OKButton);
}


BEGIN_MESSAGE_MAP(CSetCameraNameDialog, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()



/**
* Function: OnInitDialog
* Purpose:  
*/
BOOL CSetCameraNameDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_pDoc->HasActiveCamera())
	{
		CAMERA_INFO const* pInfo = m_pDoc->GetActiveCamera()->GetCameraInfo();

		m_SerialNumberLabel.SetWindowText(reinterpret_cast<LPCSTR>(&pInfo->SerialNumber[0]));
		m_VendorNameLabel.SetWindowText(reinterpret_cast<LPCSTR>(&pInfo->VendorName[0]));
		m_ModelNameLabel.SetWindowText(reinterpret_cast<LPCSTR>(&pInfo->ModelName[0]));
		m_CameraNameEdit.SetWindowText(reinterpret_cast<LPCSTR>(&pInfo->CameraName[0]));
	}
	else
	{
		// This *should* never happen...
		TCHAR const* na = _T("n/a");
		m_SerialNumberLabel.SetWindowText(na);
		m_VendorNameLabel.SetWindowText(na);
		m_ModelNameLabel.SetWindowText(na);
		m_CameraNameEdit.SetWindowText(_T("Error: No camera selected"));

		m_CameraNameEdit.EnableWindow(FALSE);
		m_OKButton.EnableWindow(FALSE);
	}

	return TRUE;
}


/**
* Function: OnBnClickedOk
* Purpose:  
*/
void CSetCameraNameDialog::OnBnClickedOk()
{
	m_CameraNameEdit.GetWindowText(m_newName);

	if (m_newName.GetLength() > 32)
	{
		ErrorMessage(_T("Camera name must be 32 or fewer characters."));
		m_CameraNameEdit.SetFocus();
		return;
	}
	else if (m_newName.GetLength() == 0)
	{
		if (IDCANCEL == AfxMessageBox(_T("Are you sure you want to set the camera name to a zero-length string?"), MB_OKCANCEL))
		{
			m_CameraNameEdit.SetFocus();
			return;
		}
	}

	OnOK();
}























