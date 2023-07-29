//
// NavitarController.cpp 
//
// Demonstrates how to find and use external Navitar Controllers in the context of the Pixelink API.  Example controllers
// are:
//    - Navitar Zoom/Focus Controller
//    - Navitar Resolv LED Lighting Controller
// 
// These controllers connect via a COM port on the computer; with with a serial cable (or more typically), via a USB 
// connection mapped to a COM port
//
#include <PixeLINKAPI.h>
#include <stdio.h>

int
main(int argc, char* argv[])
{
   PXL_RETURN_CODE rc;

   U32             numCameras = 0;
   PCAMERA_ID_INFO cameraIds = NULL;
   U32              numControllers = 0;
   PCONTROLLER_INFO controllerIds = NULL;

   //
   // Step 1
   //    Enumerate all of the connected cameras and controllers
   PxLGetNumberCamerasEx (NULL, &numCameras);
   cameraIds = new CAMERA_ID_INFO[numCameras];
   PxLGetNumberControllers (NULL, sizeof(CONTROLLER_INFO), &numControllers);
   controllerIds = new CONTROLLER_INFO[numControllers];

   rc = PxLGetNumberCamerasEx (cameraIds, &numCameras);
   if (API_SUCCESS(rc))
   {
      rc = PxLGetNumberControllers (controllerIds, sizeof(CONTROLLER_INFO), &numControllers);
   }

   if (!API_SUCCESS(rc))
   {
      printf ("ERROR -- Could not enumerate cameras/controllers.  RC:0x%x\n", rc);
   }

   // 
   // Step 2
   //      Print out information on all of the cameras found:
   printf ("Cameras found:\n");
   for (U32 camera = 0; camera < numCameras; camera++)
   {
      printf ("  S/N: %d \n", cameraIds[camera].CameraSerialNum);
   }
   if (numCameras == 0) printf("  None\n");

   //
   // Step 3
   //      Print out information on all of the controllers found:
   printf("Controllers found:\n");
   char strCameraSN[16];
   U32 cameraSN;
   for (U32 controller = 0; controller < numControllers; controller++)
   {
      cameraSN = controllerIds[controller].CameraSerialNumber;
      sprintf (strCameraSN, "%d", cameraSN);
      printf("  S/N: %d, Type: 0x%x, connected to camera:%s, Port:%s\n", 
               controllerIds[controller].ControllerSerialNumber,
               controllerIds[controller].TypeMask,
               cameraSN == 0 ? "None" : strCameraSN,
               controllerIds[controller].COMPort);
   }
   if (numCameras == 0) printf("  None\n");
   printf ("\n");

   // Step 4
   //      Offer some hints on the typical use case
   HANDLE myCamera;
   if (numCameras == 1 && numControllers == 1)
   {
      // 
      // Step 4.1
      //    If there is just a single camera and a single controller, then the controller should 
      //    be auto assigned to the camera, when the camera is initialized.  Check this
      printf ("Checking to see if the controller is auto assigned to the camera..\n");
      rc = PxLInitializeEx (0, &myCamera, 0);
      if (API_SUCCESS(rc))
      {
         rc = PxLGetNumberControllers(controllerIds, sizeof(CONTROLLER_INFO), &numControllers);
         if (API_SUCCESS(rc))
         {
            cameraSN = controllerIds->CameraSerialNumber;
            sprintf(strCameraSN, "%d", cameraSN);
            printf("  S/N: %d, Type: 0x%x, connected to camera:%s, Port:%s\n",
               controllerIds->ControllerSerialNumber,
               controllerIds->TypeMask,
               cameraSN == 0 ? "None" : strCameraSN,
               controllerIds->COMPort);
         }
         if (cameraSN != cameraIds->CameraSerialNum)
         {
            printf ("  Error -- the controller was not auto-assigned to the camera!!\n");
         }

         // 
         // Step 4.2
         //       If it's a Lighting contoller, then try to read it's value
         if (controllerIds->TypeMask & CONTROLLER_FLAG_LIGHTING)
         {
            float lightingParams[FEATURE_LIGHTING_NUM_PARAMS];
            U32 flags = 0;
            U32 numParams = FEATURE_LIGHTING_NUM_PARAMS;
            rc = PxLGetFeature(myCamera, FEATURE_LIGHTING, &flags, &numParams, lightingParams);
            if (API_SUCCESS(rc))
            {
               printf("  Lighting Enabled:%s Brightness:%.1f Current:%.3fA Voltage: %.1fV Temp:%.1fC\n",
                      flags & FEATURE_FLAG_OFF ? "Off" : "On",
                      lightingParams[0], lightingParams[1],
                      lightingParams[2], lightingParams[3]);
            }

         }
         PxLUninitialize (myCamera);
      }
      printf ("\n");
      printf ("Given that there is exactly one camera and one controller, the controller\n");
      printf ("   was automatically assigned to the (only) camera.  Had this not been the \n");
      printf ("   case, PxLAssignController must be used to assign specific controllers to\n");
      printf ("   specific cameras.\n");
   } else if (numCameras > 1 || numControllers > 1) {
      printf("\n");
      printf("There is more than one camera or controller.  The Pixelink API cannot determine\n");
      printf ("  which controller is associated with which camera.  You must use\n");
      printf ("  PxLAssignController to supply this information to the API, before\n");
      printf ("  you will be able use controller based features.\n");
   }

   delete [] cameraIds;
   delete [] controllerIds;
}
