//
// initializeex.c
//
// Demonstrates how to call PxLGetNumberCamerasEx.
//

#include <PixeLINKApi.h>
#include <stdio.h>
#include <assert.h>

//
// Some useful defines
//
#define ASSERT(x)	do { assert((x)); } while(0)
#define INDENT	"  "

//
// Defines for use with some PixeLINK IP structs
//
#define	IS_IP_CAMERA(cameraIdInfo)		((cameraIdInfo).CameraIpAddress.U32Address != 0)
#define	IS_LLA_IP_ADDRESS(ipa, mask)	(((ipa).U8Address[0] == 169) && ((ipa).U8Address[1] == 254) && (mask.U8Address[0] == 0xFF) && (mask.U8Address[1] == 0xFF))

// Local function prototypes
static void		ProcessCameras(CAMERA_ID_INFO* pCameraInfo, U32 numberOfCameras);
static void		ProcessCameraWithSerialNumber(CAMERA_ID_INFO* pCameraInfo);
static void		ProcessCameraWithoutSerialNumber(CAMERA_ID_INFO* pIp);
static void		AnalyzeIpCamera(CAMERA_ID_INFO* pCameraInfo);
static void		PrintCameraIdInfo(CAMERA_ID_INFO* pCameraInfo);
static void		PrintCameraInfo(U32 serialNumber);
static void		GetCameraProperties(const U8 props, char * const pBuffer);



int 
main(int argc, char* argv[])
{

	PXL_RETURN_CODE rc;
	U32	numberOfCameras;
	CAMERA_ID_INFO*	pCameraInfo = NULL;

	// 
	// Step 1 - Determine how many cameras there are.
	//
	// NOTE: If you're expecting to see your GigE camera show up here, but it
	// doesn't, you might want to check that you're using the Universal Driver.
	// Go to Start\Programs\Pleora Technologies Inc\eBus Driver Suite\Driver Installation Tool
	// and ensure that the driver for your GigE network adaptor is "eBus Universal Driver".
	// If it isn't, click on the network adapter, then click the Configure button and follow
	// the instructions.
	rc = PxLGetNumberCamerasEx (NULL, &numberOfCameras);
	if ((!(API_SUCCESS(rc))) || (0 == numberOfCameras)) {
	    printf ("No cameras found !!\n");
	    return 1;
	}

	//
	// Step 2 - Set up an array of CAMERA_ID_INFO structs.
	//
	pCameraInfo = (CAMERA_ID_INFO*)malloc(sizeof(CAMERA_ID_INFO) * numberOfCameras);
	if (NULL == pCameraInfo) {
	    printf ("Out of memory error.\n");
	    return 1;
	}
	memset(pCameraInfo, 0, sizeof(CAMERA_ID_INFO));
	// Initialize the StructSize field for the first struct only 
	pCameraInfo[0].StructSize = sizeof(pCameraInfo[0]);

	//
	// Step 3 - Retrieve the information
	//
	rc = PxLGetNumberCamerasEx (pCameraInfo, &numberOfCameras);
	if (!API_SUCCESS(rc)) {
	    printf("Unable to read camera id info (Error 0x%8.8X)\n", rc);
	    return 1;
	}

	// Do whatever it is we're going with the list of cameras
	ProcessCameras(pCameraInfo, numberOfCameras);

	free(pCameraInfo);

	return 0;
}

static void 
ProcessCameras(CAMERA_ID_INFO* pCameraInfo, U32 numberOfCameras)
{
	U32 i;

	ASSERT(NULL != pCameraInfo);
	ASSERT(numberOfCameras > 0);

	for(i=0; i < numberOfCameras; i++) {
		printf("\n/////////// Camera %d /////////////\n", i);

		PrintCameraIdInfo(&pCameraInfo[i]);

		// Is this camera initializable already?
		if (pCameraInfo[i].CameraSerialNum != 0) {
			ProcessCameraWithSerialNumber(&pCameraInfo[i]);
		} else {
			ProcessCameraWithoutSerialNumber(&pCameraInfo[i]);
		}

		if (IS_IP_CAMERA(pCameraInfo[i])) {
			AnalyzeIpCamera(&pCameraInfo[i]);
		}

	}
}



//
// If the camera has a serial number, we can initialize the camera and
// print its CAMERA_INFO.
//
static void
ProcessCameraWithSerialNumber(CAMERA_ID_INFO* pCameraInfo)
{
	ASSERT(NULL != pCameraInfo);
	ASSERT(0 != pCameraInfo->CameraSerialNum);

	PrintCameraInfo(pCameraInfo->CameraSerialNum);
}


//
// See what we can determine about an IP camera based on its CAMERA_ID_INFO
//
static void
AnalyzeIpCamera(CAMERA_ID_INFO* pCameraInfo)
{
	U32 nicSubnet;
	U32 camSubnet;

	printf("\nAnalyzing IP information\n");

	if (IS_LLA_IP_ADDRESS(pCameraInfo->NicIpAddress, pCameraInfo->NicIpMask)) {
		printf(INDENT "The NIC is using an LLA IP address\n");
	}

	if (IS_LLA_IP_ADDRESS(pCameraInfo->CameraIpAddress, pCameraInfo->CameraIpMask)) {
		printf(INDENT "The camera is using an LLA IP address\n");
	}

	if (IS_LLA_IP_ADDRESS(pCameraInfo->NicIpAddress, pCameraInfo->NicIpMask) && (IS_LLA_IP_ADDRESS(pCameraInfo->CameraIpAddress, pCameraInfo->CameraIpMask))) {
		printf(INDENT "NOTE: Both the NIC and the camera have LLA IP addressses\n");
		printf(INDENT "      You may achieve better performance at initialization if you use static IP addressses\n");
	}

	nicSubnet = pCameraInfo->NicIpAddress.U32Address & pCameraInfo->NicIpMask.U32Address;
	camSubnet = pCameraInfo->CameraIpAddress.U32Address & pCameraInfo->CameraIpMask.U32Address;
	if (nicSubnet != camSubnet) {
		printf(INDENT "WARNING: NIC and camera subnets do not match!\n");
	}

	printf("IP analysis done\n");
}


static void 
ProcessCameraWithoutSerialNumber(CAMERA_ID_INFO* pCameraInfo)
{
	// This is where you may want to assign an IP address to the
	// camera using the API function PxLSetCameraIpAddress, thereby making it initializable.
	// See the demo app setipaddress for an example of how to do this.
}


//////////////////////////////////////////////////////////////////////////////
//
// Functions to print the contents of structs
//

static void
PrintCameraIdInfo(CAMERA_ID_INFO* pCameraInfo)
{
	ASSERT(NULL != pCameraInfo);

	printf("\nCAMERA_ID_INFO:\n");
	printf("StructSize ........... = %d\n", pCameraInfo->StructSize);
	printf("CameraSerialNum	...... = %d\n", pCameraInfo->CameraSerialNum);

	printf("CameraMac ............ = %2.2X-%2.2X-%2.2X-%2.2X-%2.2X-%2.2X\n", 
		pCameraInfo->CameraMac.MacAddr[0], pCameraInfo->CameraMac.MacAddr[1], pCameraInfo->CameraMac.MacAddr[2],
		pCameraInfo->CameraMac.MacAddr[3], pCameraInfo->CameraMac.MacAddr[4], pCameraInfo->CameraMac.MacAddr[5]);

	printf("CameraIpAddress ...... = %3d.%3d.%3d.%3d\n", pCameraInfo->CameraIpAddress.U8Address[0],	pCameraInfo->CameraIpAddress.U8Address[1],	pCameraInfo->CameraIpAddress.U8Address[2],	pCameraInfo->CameraIpAddress.U8Address[3]);
	printf("CameraIpMask ......... = %3d.%3d.%3d.%3d\n", pCameraInfo->CameraIpMask.U8Address[0],	pCameraInfo->CameraIpMask.U8Address[1],		pCameraInfo->CameraIpMask.U8Address[2],		pCameraInfo->CameraIpMask.U8Address[3]);
	printf("CameraIpGateway ...... = %3d.%3d.%3d.%3d\n", pCameraInfo->CameraIpGateway.U8Address[0],	pCameraInfo->CameraIpGateway.U8Address[1],	pCameraInfo->CameraIpGateway.U8Address[2],	pCameraInfo->CameraIpGateway.U8Address[3]);

	printf("NicIpAddress ......... = %3d.%3d.%3d.%3d\n", pCameraInfo->NicIpAddress.U8Address[0],		pCameraInfo->NicIpAddress.U8Address[1],		pCameraInfo->NicIpAddress.U8Address[2],		pCameraInfo->NicIpAddress.U8Address[3]);
	printf("NicIpMask ............ = %3d.%3d.%3d.%3d\n", pCameraInfo->NicIpMask.U8Address[0],			pCameraInfo->NicIpMask.U8Address[1],		pCameraInfo->NicIpMask.U8Address[2],		pCameraInfo->NicIpMask.U8Address[3]);

	printf("NicAccessMode ........ = %d\n", pCameraInfo->NicAccessMode);
	printf("CameraIpAssignmentType = %d\n", pCameraInfo->CameraIpAssignmentType);
	printf("XML Version .......... = %d.%d.%d\n", pCameraInfo->XmlVersionMajor,				pCameraInfo->XmlVersionMinor,			pCameraInfo->XmlVersionSubminor);
	printf("IP Engine Version .... = %d.%d.%d\n", pCameraInfo->IpEngineLoadVersionMajor,	pCameraInfo->IpEngineLoadVersionMinor,	pCameraInfo->
IpEngineLoadVersionSubminor);
	printf("Camera Properties .... = 0x%2.2X", pCameraInfo->CameraProperties);
	if (0 != pCameraInfo->CameraProperties) {
		char buffer[512] = "";
		GetCameraProperties(pCameraInfo->CameraProperties, &buffer[0]);
		printf(" (%s)", buffer);
	}
	printf("\n");
	printf("ControllingIpAddress   = %3d.%3d.%3d.%3d\n", pCameraInfo->ControllingIpAddress.U8Address[0],		pCameraInfo->ControllingIpAddress.U8Address[1],		pCameraInfo->ControllingIpAddress.U8Address[2],		pCameraInfo->ControllingIpAddress.U8Address[3]);


}

static void 
PrintCameraInfo(U32 serialNumber)
{
	CAMERA_INFO	cameraInfo;
	PXL_RETURN_CODE rc;
	HANDLE hCamera;

	// Access the camera for read only.
	rc = PxLInitializeEx(serialNumber, &hCamera, CAMERA_INITIALIZE_EX_FLAG_MONITOR_ACCESS_ONLY);
	if (!API_SUCCESS(rc)) {
		printf("Unable to initialize camera %d (Error 0x%8.8X)\n", serialNumber, rc);
		return;
	}

	rc = PxLGetCameraInfo(hCamera, &cameraInfo);
	if (!API_SUCCESS(rc)) {
		printf("Unable to read camera info. (Error 0x%8.8X)\n", rc);
	} else {
		printf("\nCAMERA_INFO:\n");
		printf("VendorName .... = [%s]\n", cameraInfo.VendorName);
		printf("ModelName ..... = [%s]\n", cameraInfo.ModelName);
		printf("Description ... = [%s]\n", cameraInfo.Description);
		printf("SerialNumber .. = [%s]\n", cameraInfo.SerialNumber);
		printf("FirmwareVersion = [%s]\n", cameraInfo.FirmwareVersion);
		printf("FPGAVersion ... = [%s]\n", cameraInfo.FPGAVersion);
		printf("CameraName .... = [%s]\n", cameraInfo.CameraName);
	}

	PxLUninitialize(hCamera);

}

static void
GetCameraProperties(const U8 props, char * const pBuffer)
{
	if (0 != (props & CAMERA_PROPERTY_MONITOR_ACCESS_ONLY)) {
		strcat(pBuffer, "Monitor access only, ");
	}
	if (0 != (props & CAMERA_PROPERTY_NOT_ACCESSIBLE)) {
		strcat(pBuffer, "Not accessible, ");
	}
	if (0 != (props & CAMERA_PROPERTY_IP_UNREACHABLE)) {
		strcat(pBuffer, "Not reachable, ");
	}
}