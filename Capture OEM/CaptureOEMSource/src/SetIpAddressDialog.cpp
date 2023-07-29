// SetIpAddressDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "SetIpAddressDialog.h"
#include "winsock2.h"
#include "setipaddressdialog.h"


// CSetIPAddressDialog dialog

IMPLEMENT_DYNAMIC(CSetIPAddressDialog, CDialog)
CSetIPAddressDialog::CSetIPAddressDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSetIPAddressDialog::IDD, pParent)
{
}

CSetIPAddressDialog::~CSetIPAddressDialog()
{
}

void CSetIPAddressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CAMERA,			m_cmbCamera);
	DDX_Control(pDX, IDC_IPADDRESS,				m_ctlIPAddress);
	DDX_Control(pDX, IDC_IPADDRESS_SUBNET_MASK, m_ctlSubnetMask);
	DDX_Control(pDX, IDC_IPADDRESS_GATEWAY,		m_ctlGateway);
	DDX_Control(pDX, IDC_CHECK_PERSISTENT,		m_chkPersistent);
	DDX_Control(pDX, IDC_BTN_SETIPADDRESS, m_btnSetIPAddress);
	DDX_Control(pDX, IDC_TXT_NO_CAMERAS, m_txtNoCamerasDetected);
}


BEGIN_MESSAGE_MAP(CSetIPAddressDialog, CDialog)
	ON_BN_CLICKED(IDC_BTN_SETIPADDRESS, OnBnClickedBtnSetIPAddress)
	ON_CBN_SELCHANGE(IDC_COMBO_CAMERA, OnCbnSelchangeCamera)
	ON_BN_CLICKED(IDC_BUTTON_DETECT, OnBnClickedButtonDetect)
END_MESSAGE_MAP()


BOOL 
CSetIPAddressDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (!ReadIPCameraIdInfo()) 
	{
		return FALSE;
	}

	return TRUE;
}

bool
CSetIPAddressDialog::ReadIPCameraIdInfo()
{
	try {
		CWaitCursor wc;

		m_ipCameraIdInfo.clear();

		U32 numCameras;
		PXL_ERROR_CHECK(PxLGetNumberCamerasEx(NULL, &numCameras));
		if (numCameras > 0) 
		{
			std::vector<CAMERA_ID_INFO> allCameraIdInfo(numCameras);
			allCameraIdInfo[0].StructSize = sizeof(CAMERA_ID_INFO);
			PXL_ERROR_CHECK(PxLGetNumberCamerasEx(&allCameraIdInfo[0], &numCameras));
			// We're only interested in IP cameras.
			// Q) Which ones are IP cameras?
			// A) The ones with a non-zero MAC.
			for(int i=0; i < allCameraIdInfo.size(); i++)
			{
				U32 total = 0;
				for(int j=0; j < 6; j++)
				{
					total += allCameraIdInfo[i].CameraMac.MacAddr[j];
				}
				if (total != 0)
				{
					m_ipCameraIdInfo.push_back(allCameraIdInfo[i]);
				}
			}
		}

		UpdateControls();

	}
	catch (PxLException const& e) 
	{
		CString msg;
		msg.Format("Error 0x%8.8X (%s) while detecting IP cameras", e.GetReturnCode(), e.what());
		AfxMessageBox(msg, MB_ICONEXCLAMATION);

		UpdateControls();
		return false;
	} 
	return true;
}

// CSetIPAddressDialog message handlers

void CSetIPAddressDialog::OnBnClickedBtnSetIPAddress()
{
	CAMERA_ID_INFO info = m_ipCameraIdInfo[m_cmbCamera.GetCurSel()];
	PXL_MAC_ADDRESS macAddr = info.CameraMac;

	PXL_IP_ADDRESS ipAddr;
	m_ctlIPAddress.GetAddress(ipAddr.U32Address);
	ipAddr.U32Address = htonl(ipAddr.U32Address);

	PXL_IP_ADDRESS subnetMask;
	m_ctlSubnetMask.GetAddress(subnetMask.U32Address);
	subnetMask.U32Address = htonl(subnetMask.U32Address);

	PXL_IP_ADDRESS gatewayAddr;
	m_ctlGateway.GetAddress(gatewayAddr.U32Address);
	gatewayAddr.U32Address = htonl(gatewayAddr.U32Address);

	BOOL32 persistent = (m_chkPersistent.GetCheck() == BST_CHECKED) ? 1 : 0;

	{
		CWaitCursor wc;
		try 
		{
			PXL_ERROR_CHECK(PxLSetCameraIpAddress(&macAddr, &ipAddr, &subnetMask, &gatewayAddr, persistent));
			AfxMessageBox(_T("IP address successfully set.\n\nCameras will be re-detected."), MB_ICONINFORMATION);
		}
		catch (PxLException const& e) 
		{
			CString msg(e.what());
			AfxMessageBox(msg, MB_ICONEXCLAMATION);
		}
	}

	// Read the info again so we're displaying the latest
	ReadIPCameraIdInfo();

}

void CSetIPAddressDialog::OnCbnSelchangeCamera()
{
	UpdateControls();
}

void
CSetIPAddressDialog::UpdateControls()
{
	BOOL enable = (m_ipCameraIdInfo.size() > 0);

	m_txtNoCamerasDetected.ShowWindow(!enable);
	m_cmbCamera.ShowWindow(enable);

	m_ctlIPAddress.EnableWindow(enable);
	m_ctlSubnetMask.EnableWindow(enable);
	m_ctlGateway.EnableWindow(enable);
	m_chkPersistent.EnableWindow(enable);
	m_btnSetIPAddress.EnableWindow(enable);

	m_cmbCamera.ResetContent();
	
	if (!enable) 
	{
		m_ctlIPAddress.ClearAddress();
		m_ctlSubnetMask.ClearAddress();
		m_ctlGateway.ClearAddress();
	}
	else
	{
		// Re-populate the camera dropdown
		if (m_ipCameraIdInfo.size() > 0) 
		{
			for(int i=0; i < m_ipCameraIdInfo.size(); i++) 
			{
				CString desc;
				if (m_ipCameraIdInfo[i].CameraSerialNum != 0) 
				{
					desc.Format("%d", m_ipCameraIdInfo[i].CameraSerialNum);
				}
				else 
				{
					desc = GetMacAddressAsString(m_ipCameraIdInfo[i].CameraMac);
				}
				m_cmbCamera.AddString(desc);
			}
			m_cmbCamera.SetCurSel(0);
		}

		// And populate the IP address controls
		ASSERT(m_cmbCamera.GetCurSel() >= 0);
		ASSERT(m_cmbCamera.GetCurSel() < m_ipCameraIdInfo.size());
		CAMERA_ID_INFO	info = m_ipCameraIdInfo[m_cmbCamera.GetCurSel()];

		m_ctlIPAddress.SetAddress(
			info.CameraIpAddress.U8Address[0],
			info.CameraIpAddress.U8Address[1],
			info.CameraIpAddress.U8Address[2],
			info.CameraIpAddress.U8Address[3]);

		m_ctlSubnetMask.SetAddress(
			info.CameraIpMask.U8Address[0],
			info.CameraIpMask.U8Address[1],
			info.CameraIpMask.U8Address[2],
			info.CameraIpMask.U8Address[3]);

		m_ctlGateway.SetAddress(
			info.CameraIpGateway.U8Address[0],
			info.CameraIpGateway.U8Address[1],
			info.CameraIpGateway.U8Address[2],
			info.CameraIpGateway.U8Address[3]);
	}

}

void CSetIPAddressDialog::OnBnClickedButtonDetect()
{
	CWaitCursor wc;

	if (!ReadIPCameraIdInfo()) 
	{
		return;
	}

}
