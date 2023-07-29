/*
 * initialize.c
 *
 * The absolute minimal interaction with a PixeLINK camera.
 * 
 * Initialize a camera for use, and if that's successful, 
 * uninitialize it and exit.
 *
 * If there are no cameras, the program will report an error being returned by PxLInitialize.
 *
 * If you DO have a camera connected and it's not being found by PxLInitialize, you could check 
 * to ensure that:
 * - the camera is connected to the host.
 * - the camera's LED is solid green. 
 *        Flashing green indicates the camera is streaming, meaning another app may be controlling it.
 *        Solid red indicates a problem, and the camera should be rebooted by removing power.
 * - if it's a GigE camera, that it has been configured properly and is visible to the host. 
 *        Use the Camera IP Address Tool (PxLIpAddresses.exe in Program Files\PixeLINK\Tools) to confirm 
 *        that the camera is configured properly.
 *        You should be able to ping the camera. 
 */
#include <PixeLINKAPI.h>
#include <stdio.h>

int 
main(int argc, char* argv[])
{
	/* Handle with which we interact with the camera */
	HANDLE hCamera;

	/*
	 * Call PxLInitialize, passing in a serial number of 0, 
	 * meaning we want to talk to a (any) camera connected 
	 * to the system. If there's only one camera, we'll be 
	 * connected to it. If there's more than one camera, we 
	 * cannot know deterministically which camera we will be 
	 * connected to. 
	 * See the demo app getcamerainfo for an example of how to 
	 * enumerate all cameras and connect to a specific camera.
	 */
	PXL_RETURN_CODE rc = PxLInitialize(0, &hCamera);

	printf("PxLInitialize   returned 0x%8.8X (%s)\n", rc,  (API_SUCCESS(rc)) ? "Success" : "Error");

	/*
	 * If the initialization was successful, we need to call PxLUninitialize
	 * to tell the camera we're done with it.
	 * Note the use of the API_SUCCESS macro to check for success. 
	 * This is better than comparing to ApiSuccess.
	 */
	if (API_SUCCESS(rc)) {
		rc = PxLUninitialize(hCamera);
		printf("PxLUninitialize returned 0x%8.8X (%s)\n",  rc,  (API_SUCCESS(rc)) ? "Success" : "Error");
	}

	return (API_SUCCESS(rc)) ? EXIT_SUCCESS : EXIT_FAILURE;
}

