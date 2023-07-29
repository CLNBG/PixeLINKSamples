//
// multiCameraPreview.cpp : Defines the entry point for the console application.
//
// Connects to a number of cameras, starting and stopping the stream as per the
// users wishes
//

#include <PixeLINKApi.h>
#include <conio.h>
#include <stdio.h>
#include <vector>

using namespace std;

typedef struct
{
   ULONG  serialNum;
   HANDLE hCamera;
   bool   previewing;
   HWND   previewWindow;
} CameraInfo, *PCameraInfo;

int main(int argc, char* argv[]) {
    PXL_RETURN_CODE rc;
    ULONG numCameras = 0;

    int   keyPressed;
    bool  done = false;
    HWND  previewWin;

    MSG  msg;
    BOOL bRet;

    //
    // Step 1
    //    Get information on all connected cameras
    rc = PxLGetNumberCamerasEx(NULL, &numCameras);
    if (!API_SUCCESS(rc))
    {
        printf ("Could not even enumerate any cameras!  Rc = 0x%X\n", rc);
        return 1;
    }
    if (numCameras < 1)
    {
	    // No point in continuing
        printf ("No cameras found.\n");
        return 0; // Not really and error, so return 0
    }

    vector<CAMERA_ID_INFO> connectedCameras(numCameras);
    // Let the PixeLINK API know the size of CAMERA_ID_INFO we are using
    if (numCameras) connectedCameras[0].StructSize = sizeof (CAMERA_ID_INFO);
    rc = PxLGetNumberCamerasEx(&connectedCameras[0], &numCameras);
    if (!API_SUCCESS(rc))
    {
        printf ("Could not even enumerate the cameras!  Rc = 0x%X\n", rc);
        return 1;
    }

    //
    // Step 2
    //    Show the camera info for all available cameras
    printf ("Camera #    Serial #      \n");
    for (ULONG i = 0; i < numCameras; i++)
    {
	    printf ("   %2d      %d    \n",
                 i+1, connectedCameras[i].CameraSerialNum);
    }


    //
    // Step 3
    //    Record all of the camera 'state' information
    vector<CameraInfo> myCameras(numCameras);
    char  previewTitle[20];
    for (ULONG i = 0; i < numCameras; i++)
    {
         myCameras[i].serialNum = connectedCameras[i].CameraSerialNum;
         rc = PxLInitializeEx(myCameras[i].serialNum,&(myCameras[i].hCamera),0);
         myCameras[i].previewing = false;
         if (!API_SUCCESS(rc))
         {
             printf ("Could not initialize camera %d (serial number:%d)\n",
                     i+1, myCameras[i].serialNum);
             // We may have sucessfully initialized some cameras.  but, since we
             // are about to exit -- I'll dispense with the uninitialize.
             return 1;
         }

         //
         // Step 4
         //     For each camera, set the preview window to a 'resonable' size 
         PxLSetPreviewSettings (myCameras[i].hCamera,
                                itoa (myCameras[i].serialNum, previewTitle, 10),
                                WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                CW_USEDEFAULT,
                                CW_USEDEFAULT,
                                1024,
                                768);


    }

    //
    // Step 5
    //    Toggle the preview according to the users wishes
    printf ("Press a number key to toggle that cameras preview.  Q to quit\n");
    while (!done)
    {
        //fflush(stdin);
        if (kbhit())
        {
            //keyPressed = getchar();
            keyPressed = getch();
            switch (keyPressed)
            {
            case 'q':
            case 'Q':
                done = true;
                break;
            case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9': 
                ULONG cameraNum = (keyPressed-0x30)-1; // Assumes ASCII character set.
                if (cameraNum >= numCameras) break;  // Ignore numbers not mapped to a camera
                if (!myCameras[cameraNum].previewing)
                {
                    rc = PxLSetStreamState (myCameras[cameraNum].hCamera, START_STREAM);
                    if (API_SUCCESS(rc))
                    {
                        rc = PxLSetPreviewState (myCameras[cameraNum].hCamera, 
                                                 START_PREVIEW, 
                                                 &(myCameras[cameraNum].previewWindow));
                    }
                } else {
                    rc = PxLSetPreviewState (myCameras[cameraNum].hCamera, 
                                             STOP_PREVIEW, 
                                             &(myCameras[cameraNum].previewWindow));
                    if (API_SUCCESS(rc))
                    {
                        rc = PxLSetStreamState (myCameras[cameraNum].hCamera, STOP_STREAM);
                    }
                }
                myCameras[cameraNum].previewing = ! myCameras[cameraNum].previewing;
                if (!API_SUCCESS(rc))
                {
                    printf ("Difficulties setting preview state, rc = 0x%x\n", rc);
                    done = true;
                }
                break;
            }

            //fflush(stdout);
        }

        //
        // Step 6
        //      Scan the message queue, just so that preview events get passeed to the preview windows  
        if( (bRet = PeekMessage( &msg, NULL, 0, 0, PM_REMOVE)) > 0)
        { 
            if (bRet != -1)
            {
                TranslateMessage(&msg); 
                DispatchMessage(&msg); 
            }
        }

        //
        // Step 7
        //      Small delay so that we don't hog all of the CPU
        Sleep (50);  // 50 ms
    }


    //
    // Step 8
    //     Clean up
    for (ULONG i = 0; i < numCameras; i++)
    {
         if (myCameras[i].previewing)
         {
             PxLSetPreviewState (myCameras[i].hCamera, STOP_PREVIEW, &previewWin);
             PxLSetStreamState (myCameras[i].hCamera, STOP_STREAM);
         }
         PxLUninitialize(myCameras[i].hCamera);
     }

    return 0;
}

