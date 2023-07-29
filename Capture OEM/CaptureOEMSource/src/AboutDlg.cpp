
#include "stdafx.h"
#include "PxLDevApp.h"
#include "AboutDlg.h"
#include "PxLCamera.h"
#include <shlwapi.h>
#include <string>
#include "Common.h"

// CAboutDlg dialog

IMPLEMENT_DYNAMIC(CAboutDlg, CDialog)

CAboutDlg::CAboutDlg(CPxLCameraDoc* pDoc) 
	: CDialog(CAboutDlg::IDD)
	, m_pDoc(pDoc)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

	CStatic m_MacAddress;
	CStatic m_IPAddress;
	CStatic m_IPMask;
	CStatic m_IPEngineFirmware;


void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_AB_RELEASEVERSION_LABEL,		m_ReleaseVersionLabel);
	DDX_Control(pDX, IDC_AB_APPVERSION_LABEL,			m_AppVersionLabel);
	DDX_Control(pDX, IDC_AB_APIVERSION_LABEL,			m_ApiVersionLabel);
	DDX_Control(pDX, IDC_AB_DRIVERVERSION_LABEL,		m_DriverVersionLabel);
	DDX_Control(pDX, IDC_AB_USBDRIVERVERSION_LABEL,		m_UsbDriverVersionLabel);
	DDX_Control(pDX, IDC_AB_USB3DRIVERVERSION_LABEL,	m_Usb3DriverVersionLabel);
	DDX_Control(pDX, IDC_AB_GEVDRIVERVERSION_LABEL,		m_GevDriverVersionLabel);
	DDX_Control(pDX, IDC_AB_SERIALNUMBER_LABEL,			m_SerialNumberLabel);
	DDX_Control(pDX, IDC_AB_VENDORNAME_LABEL,			m_VendorNameLabel);
	DDX_Control(pDX, IDC_AB_MODELNAME_LABEL,			m_ModelNameLabel);
	DDX_Control(pDX, IDC_AB_CAMERANAME_LABEL,			m_CameraNameLabel);
	DDX_Control(pDX, IDC_AB_FWVERSION_LABEL,			m_FWVersionLabel);
	DDX_Control(pDX, IDC_AB_BOOTVERSION_LABEL,			m_BootVersionLabel);
	DDX_Control(pDX, IDC_AB_FPGAVERSION_LABEL,			m_FPGAVersionLabel);
	DDX_Control(pDX, IDC_AB_FFC_LABEL,					m_FFCTypeLabel);
	DDX_Control(pDX, IDC_AB_MAC_ADDRESS_LABEL,			m_MacAddressLabel);
	DDX_Control(pDX, IDC_AB_IP_ADDRESS_LABEL,			m_IPAddressLabel);
	DDX_Control(pDX, IDC_AB_IP_ENGINE_FIRMWARE_LABEL1,	m_IPEngineFirmwareLabel);
	DDX_Control(pDX, IDC_AB_IP_XML_VERSION_LABEL,		m_XmlVersionLabel);
	DDX_Control(pDX, IDC_AB_LENS_DESCRIPTION_LABEL,	    m_LensDescriptionLabel);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_COPY_TO_CLIPBOARD, &CAboutDlg::OnBnClickedBtnCopyToClipboard)
END_MESSAGE_MAP()


CString 
GetFileVersion(const LPTSTR fileName)
{
	DWORD dwSize, dummy;
	dwSize = ::GetFileVersionInfoSize(fileName, &dummy);
	if (dwSize > 0)
	{
		// Call to GetFileVersionInfoSize succeeded
		std::vector<byte> data(dwSize);
		//void* pData = new byte[dwSize];
		if (::GetFileVersionInfo(fileName, dummy, dwSize, &data[0]))
		{
			UINT len = 0;
			VS_FIXEDFILEINFO* pVerInfo;
			if (::VerQueryValue(&data[0], _T("\\"), reinterpret_cast<void**>(&pVerInfo), &len))
			{
				CString s;
				s.Format(_T("%d.%d.%d.%d"), 
						 HIWORD(pVerInfo->dwFileVersionMS), 
						 LOWORD(pVerInfo->dwFileVersionMS), 
						 HIWORD(pVerInfo->dwFileVersionLS),
						 LOWORD(pVerInfo->dwFileVersionLS));
				return s;
			}
		}
	}
	return NOT_INSTALLED;
}

CString 
GetProductVersion(const LPTSTR fileName)
{
	DWORD dwSize, dummy;
	dwSize = ::GetFileVersionInfoSize(fileName, &dummy);
	if (dwSize > 0)
	{
		// Call to GetFileVersionInfoSize succeeded
		std::vector<byte> data(dwSize);
		//void* pData = new byte[dwSize];
		if (::GetFileVersionInfo(fileName, dummy, dwSize, &data[0]))
		{
			UINT len = 0;
			VS_FIXEDFILEINFO* pVerInfo;
			if (::VerQueryValue(&data[0], _T("\\"), reinterpret_cast<void**>(&pVerInfo), &len))
			{
				CString s;
				s.Format(_T("%d.%02d"), 
						 HIWORD(pVerInfo->dwProductVersionMS), 
						 LOWORD(pVerInfo->dwProductVersionMS)); 
				return s;
			}
		}
	}
	return _T("???");
}

// As per Bugzilla.598. look for the Release version is HKEY_LOCAL_MACHINE\SOFTWARE\PixeLINK\Release.  Failing
// that, it will look for the string but in HKEY_CURRENT_USER\Software\PixeLINK\Release.
// If this values aren't present, it must be a pre 7.18 release, in which case we'll read it from Capture OEM
//
CString 
GetPixeLINKReleaseVersion()
{
	CString releaseVersion("Pre 7.18");
	HKEY hKey;
	int keyNum;
	
	long rv = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\PixeLINK"), 0, KEY_QUERY_VALUE, &hKey);
	for (keyNum=0; keyNum<2; keyNum++)
	{
        if (ERROR_SUCCESS == rv) {

	        DWORD numBytes = 256;
	        std::vector<BYTE> buffer(256,0);
	        DWORD dataType;
	        rv = ::RegQueryValueEx(hKey, _T("Release"), 0, &dataType, &buffer[0], &numBytes);
	        if ((ERROR_SUCCESS == rv) && (REG_SZ == dataType)) {
		        releaseVersion = CString(&buffer[0]);
		        break;
	        }
	        rv = ::RegCloseKey(hKey);
	    }
	    // this key did not work, try the next one.
	    rv = ::RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\PixeLINK"), 0, KEY_QUERY_VALUE, &hKey);
	}

	return releaseVersion;
}

void
CAboutDlg::AppendTimeStampToClipboard()
{
	SYSTEMTIME lt;
	GetLocalTime(&lt);

	char buffer[128];
	::sprintf_s(buffer, sizeof(buffer), "%4.4d/%2.2d/%2.2d @ %2.2d:%2.2d:%2.2d\n",
		lt.wYear, lt.wMonth, lt.wDay,
		lt.wHour, lt.wMinute, lt.wSecond);
	AppendClipboardInfo("Current time", CString(buffer));
}

void 
CAboutDlg::AppendOSVersionInfoToClipboard()
{
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(osvi));
	osvi.dwOSVersionInfoSize = sizeof(osvi);
	if (!GetVersionEx((OSVERSIONINFO*)&osvi)) {
		AppendClipboardInfo("Windows Version Info", "Unavailable");
	} else {
		AppendClipboardInfo("Windows Info:\n");
		AppendClipboardInfo("Windows Major Version", osvi.dwMajorVersion);
		AppendClipboardInfo("Windows Minor Version", osvi.dwMinorVersion);
		AppendClipboardInfo("Windows Build Number",  osvi.dwBuildNumber);
		AppendClipboardInfo("Windows Service Pack",  osvi.szCSDVersion);
		AppendClipboardInfo("Windows Service Pack Major Version", osvi.wServicePackMajor);
		AppendClipboardInfo("Windows Service Pack Minor Version", osvi.wServicePackMinor);
		AppendClipboardInfo("Windows Platform ID",	osvi.dwPlatformId);
		AppendClipboardInfo("Windows Product Type", osvi.wProductType);
	}
}

#define INDENT "   "

void
CAboutDlg::AppendFeatureInfo(const U32 featureId)
{
	ASSERT(m_pDoc->HasActiveCamera());
	CPxLCamera* pCamera = m_pDoc->GetActiveCamera();
	ASSERT(NULL != pCamera);
	const HANDLE hCamera(pCamera->GetHandle());
	ASSERT(0 != hCamera);

	CString line;
	U32 bufferSize = 0;
	line.Format("Feature %s (%d):\n", FeatureIdAsString(featureId), featureId);
	AppendClipboardInfo(line);

	PXL_RETURN_CODE rc = PxLGetCameraFeatures(hCamera, featureId, NULL, &bufferSize);
	if (!API_SUCCESS(rc)) {
		line.Format(INDENT "Unable to read info. Error 0x%8.8X\n", rc);
		AppendClipboardInfo(line);
	} else {
		// Allocate the buffer, and read the information
		std::vector<U8> buffer(bufferSize, 0);
		rc = PxLGetCameraFeatures(hCamera, featureId, reinterpret_cast<CAMERA_FEATURES*>(&buffer[0]), &bufferSize);
		CAMERA_FEATURES* pFeatureInfo = reinterpret_cast<CAMERA_FEATURES*>(&buffer[0]);
		ASSERT(1 == pFeatureInfo->uNumberOfFeatures);
		CAMERA_FEATURE* pInfo = pFeatureInfo->pFeatures;

		AppendFlags(pInfo->uFlags);

		// Only print information for this feature if it is supported (exists).
		if (!(pInfo->uFlags & FEATURE_FLAG_PRESENCE)) {
			AppendClipboardInfo(INDENT "Unsupported feature\n");
		} else {
		
			for(U32 i=0; i < pInfo->uNumberOfParameters; i++) {
				line.Format(INDENT "Parameter %d min = %f\n", i, pInfo->pParams[i].fMinValue);
				AppendClipboardInfo(line);
				line.Format(INDENT "Parameter %d max = %f\n", i, pInfo->pParams[i].fMaxValue);
				AppendClipboardInfo(line);
                // for the lookup table, only the first parameter has a valid min and max.
                if (FEATURE_LOOKUP_TABLE == featureId) break;
			}

			// Print the info PxLGetFeature
			// Do we have to do multiple queries?
			// To do a GPO query we have to specify in the get which GPO we want to do
			bool doMultipleQueries = (FEATURE_GPIO == featureId);
			int minIndex = (doMultipleQueries) ? static_cast<int>(pInfo->pParams[0].fMinValue)   : 0;
			int maxIndex = (doMultipleQueries) ? static_cast<int>(pInfo->pParams[0].fMaxValue)+1 : 1;

			AppendClipboardInfo("\n");
			AppendClipboardInfo(INDENT "Current Parameter Values\n");
			for(int i=minIndex; i < maxIndex; i++) {
				U32 numParams = (FEATURE_LOOKUP_TABLE == featureId) ? static_cast<U32>(pInfo->pParams[0].fMaxValue) : pInfo->uNumberOfParameters;
				std::vector<float> params(numParams,0.0f);
				if (doMultipleQueries) {
					params[0] = static_cast<float>(i); // Which gpio one are we asking about?
				}
				U32 flags(0);
				rc = PxLGetFeature(hCamera, featureId, &flags, &numParams, &params[0]);
				if (API_SUCCESS(rc)) {
					if (doMultipleQueries) {
						line.Format("\n" INDENT "PxLGetFeature for param %d returned:\n", i);
						AppendClipboardInfo(line);
					} else {
						line.Format("\n" INDENT "PxLGetFeature returned:\n");
					}
					AppendFlags(flags);
					for(int j = 0; j < numParams; j++) {
						line.Format(INDENT "param[%d] = %f\n", j, params[j]);
						AppendClipboardInfo(line);
					}
				} else {
					line.Format("ERROR: PxLGetFeature returned an error: 0x%8.8X\n", rc);
					AppendClipboardInfo(line);
				}
			}
		}
	}
}

//
// Print the flags returned by PxLGetCameraFeatures or PxLGetFeature. 
//
void
CAboutDlg::AppendFlags(const U32 flags)
{
	CString line;
	line.Format(INDENT "flags = 0x%8.8X\n", flags);
	AppendClipboardInfo(line);
	if (0 == flags) return;

	// Print the flags from most significant bit to least significant bit.
	// This makes it easier to correspond the flag in english with the flag/bit in the hex value.
	for(int i = 31; i >=0; i--) {
		U32 flag = (1 << i);
		if (flags & flag) {
			line.Format(INDENT INDENT "%s (0x%2.2X)\n", FeatureFlagAsString(flag), flag);
			AppendClipboardInfo(line);
		}
	}
}

void
CAboutDlg::AppendCameraFeatures()
{
	AppendClipboardInfo("\nCurrent Feature Information:\n");
	for(U32 i = 0; i < FEATURES_TOTAL; i++) {
		AppendClipboardInfo("\n");
		AppendFeatureInfo(i);
	}
	AppendClipboardInfo("\n");
}

//
// http://msdn.microsoft.com/en-us/library/windows/desktop/ms684139(v=vs.85).aspx
//
typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
BOOL 
Is64BitWindows()
{
	BOOL bIsWow64 = FALSE;

	// IsWow64Process is not available on all supported versions of Windows.
	// Use GetModuleHandle to get a handle to the DLL that contains the function
	// and GetProcAddress to get a pointer to the function if available.

	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(GetModuleHandle(TEXT("kernel32")),"IsWow64Process");
	if(NULL != fnIsWow64Process) {
		if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64)) {
			// Function call failed. 
		}
	}
	return bIsWow64;
}
//
// We don't automatically include the \n here, so you'll have to do it yourself if you need it
//
void
CAboutDlg::AppendClipboardInfo(const CString& msg)
{
	m_infoForClipboard += msg;
	TRACE(msg);
}

void 
CAboutDlg::AppendClipboardInfo(const CString& name, const int value)
{
	char buffer[64];
	::sprintf_s(buffer, sizeof(buffer), "%d", value);
	AppendClipboardInfo(name, CString(buffer));
}

void 
CAboutDlg::AppendClipboardInfo(const CString& name, const CString& value)
{
	m_infoForClipboard += name;
	m_infoForClipboard += ",";
	m_infoForClipboard += value;
	m_infoForClipboard += "\n";
}

BOOL CAboutDlg::OnInitDialog()
{
    CString csApiVersion;
    CString csApi41Version;

	CDialog::OnInitDialog();

	// Put the info about the OS at the start of the info
	AppendTimeStampToClipboard();
	AppendOSVersionInfoToClipboard();
	AppendClipboardInfo("\nPixeLINK Info:\n");

	// Fill in all the fields and prepare the info for the clipboard
	CString temp = GetPixeLINKReleaseVersion();
	m_ReleaseVersionLabel.SetWindowText(GetPixeLINKReleaseVersion());
	AppendClipboardInfo("PixeLINK API Release", temp);

	temp = GetFileVersion(_T("PxLDevApp.exe"));
	m_AppVersionLabel.SetWindowText(temp);
	AppendClipboardInfo("Capture OEM", temp);

    csApiVersion = GetFileVersion(_T("PxLAPI40.dll"));
	AppendClipboardInfo("PxLApi40.dll", csApiVersion);

    csApi41Version = GetFileVersion(_T("PxLAPI41.dll"));
    if (csApi41Version != "???") csApiVersion += " - " + csApi41Version;
	m_ApiVersionLabel.SetWindowText(csApiVersion);
	AppendClipboardInfo("PxLApi41.dll", csApi41Version);


	TCHAR buf[MAX_PATH];
	UINT len = ::GetSystemDirectory(buf, MAX_PATH);
	::PathAppend(buf, _T("drivers\\"));
	CString fwDriverName = (Is64BitWindows()) ? "PxlFW64.sys" : "PxLFW.sys";
	::PathAppend(buf, fwDriverName);
	temp = GetFileVersion(buf);
	m_DriverVersionLabel.SetWindowText(temp);
	AppendClipboardInfo(fwDriverName, temp);

	len = ::GetSystemDirectory(buf, MAX_PATH);
	::PathAppend(buf, _T("drivers\\"));
	CString usbDriverName = (Is64BitWindows()) ? "PxLUSB64.sys" : "PxLUSB.sys";
	::PathAppend(buf, usbDriverName);
	
	temp = GetFileVersion(buf);
	m_UsbDriverVersionLabel.SetWindowText(temp);
	AppendClipboardInfo(usbDriverName, temp);

	len = ::GetSystemDirectory(buf, MAX_PATH);
	::PathAppend(buf, _T("drivers\\"));
	CString usb3DriverName = (Is64BitWindows()) ? "PxLUSB3x64.sys" : "PxLUSB3.sys";
	::PathAppend(buf, usb3DriverName);
	
	temp = GetFileVersion(buf);
	m_Usb3DriverVersionLabel.SetWindowText(temp);
	AppendClipboardInfo(usb3DriverName, temp);

	len = ::GetSystemDirectory(buf, MAX_PATH);
	::PathAppend(buf, _T("drivers\\PxLGevNdis6.sys"));
	temp = GetFileVersion(buf);
	AppendClipboardInfo("PxLGevNdis6.sys", temp);
	// Bugzilla.536 -- check to see if NDIS6 driver is installed.  If not, check on NDIS 5
	if (! temp.Compare (NOT_INSTALLED))
	{
    	len = ::GetSystemDirectory(buf, MAX_PATH);
	    ::PathAppend(buf, _T("drivers\\PxLGEV.sys"));
	    temp = GetFileVersion(buf);
    	AppendClipboardInfo("PxLGEV.sys", temp);
	}
	m_GevDriverVersionLabel.SetWindowText(temp);


	AppendClipboardInfo("\nCamera Info:\n");
	CString notApplicable("Not applicable");

	if (m_pDoc->HasActiveCamera())
	{
		CAMERA_INFO const* pInfo = m_pDoc->GetActiveCamera()->GetCameraInfo();
		ASSERT(NULL != pInfo);

		m_SerialNumberLabel.SetWindowText(reinterpret_cast<LPCSTR>(&pInfo->SerialNumber[0]));
		AppendClipboardInfo("Serial Number", CString("\"") + pInfo->SerialNumber + CString("\""));

		// Enclose these fields in quotes so we can see if there are any superfluous spaces
		m_VendorNameLabel.SetWindowText(reinterpret_cast<LPCSTR>(&pInfo->VendorName[0]));
		AppendClipboardInfo("Vendor Name", CString("\"") + pInfo->VendorName + CString("\""));

		m_ModelNameLabel.SetWindowText(reinterpret_cast<LPCSTR>(&pInfo->ModelName[0]));
		AppendClipboardInfo("Model Name", CString("\"") + pInfo->ModelName + CString("\""));

		m_CameraNameLabel.SetWindowText(reinterpret_cast<LPCSTR>(&pInfo->CameraName[0]));
		AppendClipboardInfo("Camera Name", CString("\"") + pInfo->CameraName + CString("\""));

		// Surround firmware and fpga versions in quotes, because if this is a GigE camera,
		// and there are firewall issues, they could be blank. By surrounding with quotes,
		// we make this a little more obvious.
		m_FWVersionLabel.SetWindowText(reinterpret_cast<LPCSTR>(&pInfo->FirmwareVersion[0]));
		AppendClipboardInfo("Firmware", CString("\"") + pInfo->FirmwareVersion + CString("\""));

		m_FPGAVersionLabel.SetWindowText(reinterpret_cast<LPCSTR>(&pInfo->FPGAVersion[0]));
		AppendClipboardInfo("FPGA", CString("\"") + pInfo->FPGAVersion + CString("\""));

		// Not all camera support a Boot version.  Those that do not, should ne NULL
    	if (pInfo->BootloadVersion[0] == 0)
		{
		    m_BootVersionLabel.SetWindowText(notApplicable);
		} else {
		    m_BootVersionLabel.SetWindowText(reinterpret_cast<LPCSTR>(&pInfo->BootloadVersion[0]));
       		AppendClipboardInfo("BOOT", CString("\"") + pInfo->BootloadVersion + CString("\""));
		}

		// Not all camera support a Lens Description.  Those that do not, should ne NULL
    	if (pInfo->LensDescription[0] == 0)
		{
		    m_LensDescriptionLabel.SetWindowText(notApplicable);
		} else {
		    m_LensDescriptionLabel.SetWindowText(reinterpret_cast<LPCSTR>(&pInfo->LensDescription[0]));
       		AppendClipboardInfo("LENS", CString("\"") + pInfo->LensDescription + CString("\""));
		}

		CString ffcType = m_pDoc->GetActiveCamera()->GetFFCType();
		temp = ffcType.GetString();
		m_FFCTypeLabel.SetWindowText(temp);
		AppendClipboardInfo("FFC Type", temp);

		const CAMERA_ID_INFO* pCameraIdInfo = m_pDoc->GetActiveCamera()->GetCameraIdInfo();
		ASSERT(NULL != pCameraIdInfo);
    	// Bugzilla.1840 - If there is an XML file, record it's info
		if (pInfo->XMLVersion[0] == 0)
		{
		    m_XmlVersionLabel.SetWindowText(notApplicable);
		} else {
		    m_XmlVersionLabel.SetWindowText(reinterpret_cast<LPCSTR>(&pInfo->XMLVersion[0]));
       		AppendClipboardInfo("XML", CString("\"") + pInfo->XMLVersion + CString("\""));
		}
		// If there's no MAC, it's a Firewire or USB2/3 camera
		U32 macTotal = 0;
		for(int i=0; i < 5; i++) {
			macTotal += pCameraIdInfo->CameraMac.MacAddr[i];
		}
		if (0 == macTotal) {
			
			m_MacAddressLabel.SetWindowText(notApplicable);
			m_IPAddressLabel.SetWindowText(notApplicable);
			m_IPEngineFirmwareLabel.SetWindowText(notApplicable);
			AppendClipboardInfo("GigE", "Not applicable");
		} else {
			temp = GetMacAddressAsString(pCameraIdInfo->CameraMac);
			m_MacAddressLabel.SetWindowText(temp);
			AppendClipboardInfo("MAC Address", temp);

			temp = GetIPAddressAsString(pCameraIdInfo->CameraIpAddress);
			m_IPAddressLabel.SetWindowText(temp);
			AppendClipboardInfo("IP Address", temp);

			temp.Format("%d.%d.%d",
				pCameraIdInfo->IpEngineLoadVersionMajor,
				pCameraIdInfo->IpEngineLoadVersionMinor,
				pCameraIdInfo->IpEngineLoadVersionSubminor);
			m_IPEngineFirmwareLabel.SetWindowText(temp);
			AppendClipboardInfo("IP Engine Firmware Version", temp);

			// Stuff we don't display, but can be useful for support
			temp.Format("%d", pCameraIdInfo->StructSize);
			AppendClipboardInfo("StructSize", temp);
			temp.Format("%d", pCameraIdInfo->CameraSerialNum);
			AppendClipboardInfo("CameraSerialNum", temp);
			temp = GetIPAddressAsString(pCameraIdInfo->CameraIpGateway);
			AppendClipboardInfo("Camera IP Gateway", temp);
			temp = GetIPAddressAsString(pCameraIdInfo->NicIpAddress);
			AppendClipboardInfo("NIC IP Address", temp);
			temp = GetIPAddressAsString(pCameraIdInfo->NicIpMask);
			AppendClipboardInfo("NIC IP Mask", temp);
			temp.Format("%d", pCameraIdInfo->NicAccessMode);
			AppendClipboardInfo("NIC Access Mode", temp);
			temp = GetIpAddressAssignmentAsString(pCameraIdInfo->CameraIpAssignmentType);
			AppendClipboardInfo("Camera IP Assignment Type", temp);
			temp.Format("%d", pCameraIdInfo->CameraProperties);
			AppendClipboardInfo("Camera Properties", temp);
			temp = GetIPAddressAsString(pCameraIdInfo->ControllingIpAddress);
			AppendClipboardInfo("Controlling IP Address", temp);
            temp.Format("%d", pCameraIdInfo->CameraLinkSpeed);
            AppendClipboardInfo("Camera Link Speed (mb/s)", temp);
		}
		
        AppendCameraFeatures();

	}
	else
	{
		m_SerialNumberLabel.SetWindowText(notApplicable);
		m_VendorNameLabel.SetWindowText	(notApplicable);
		m_ModelNameLabel.SetWindowText	(notApplicable);
		m_CameraNameLabel.SetWindowText	(notApplicable);
		m_FWVersionLabel.SetWindowText	(notApplicable);
		m_FPGAVersionLabel.SetWindowText(notApplicable);
		m_FFCTypeLabel.SetWindowText	(notApplicable);
		m_MacAddressLabel.SetWindowText	(notApplicable);
		m_IPAddressLabel.SetWindowText	(notApplicable);
		m_XmlVersionLabel.SetWindowText(notApplicable);
		m_IPEngineFirmwareLabel.SetWindowText(notApplicable);
		m_LensDescriptionLabel.SetWindowText(notApplicable);
		AppendClipboardInfo("Camera", "No camera detected");
	}

	return TRUE;
}

#define NORMAL_NEWLINE "\n"
#define CLIPBOARD_NEWLINE "\r\n"


static bool
CopyToClipboard(const std::string& msg)
{
	std::string clipBoardMsg(msg);
	const std::string searchString(NORMAL_NEWLINE);
	const std::string replaceString(CLIPBOARD_NEWLINE);
	// Replace normal newline with clipboard newline
	std::string::size_type pos = 0;
	while ( (pos = clipBoardMsg.find(searchString, pos)) != std::string::npos ) {
        clipBoardMsg.replace( pos, searchString.size(), replaceString );
        pos += replaceString.length();
    }

	bool success = false;
	if (OpenClipboard(0)) {
		EmptyClipboard();
		char const * const pMsg = clipBoardMsg.c_str();
		HGLOBAL hMem = ::GlobalAlloc(GMEM_MOVEABLE, ::strlen(pMsg) + 1);
		if (NULL != hMem) {
			char * const pMem = reinterpret_cast<char * const>(::GlobalLock(hMem));
			::memcpy(pMem, pMsg, ::strlen(pMsg) + 1);
			::GlobalUnlock(hMem);
			if (SetClipboardData(CF_TEXT, hMem)) {
				success = true;
			} else {
				// You must be debugging, 'cause this fails when debugging
			}
		}
		CloseClipboard();
	}
	return success;
}


void 
CAboutDlg::OnBnClickedBtnCopyToClipboard()
{
	CWaitCursor	wait;
	bool success = CopyToClipboard(m_infoForClipboard);
	if (!success) {
		AfxMessageBox("Unable to copy the information to the clipboard.", MB_ICONEXCLAMATION);
	}
}
