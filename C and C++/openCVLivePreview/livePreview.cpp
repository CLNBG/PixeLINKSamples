//
// livePreview.cpp 
//
// A demonstration of how to capture a camera image that can be 'imported' into openCV.
// Be sure to update the project to point to openCV libraries.
//
//  NOTE:
//     You will need to install openCV on your system for this sample application to build/run.
//     This particular sample was written to OpenCV v4.0.1, and assumes you have an environmental
//     variable called OPEN_CV that represents the location where OpenCV was installed.

#include <PixeLINKApi.h>
#include <cstdio>
#include <cassert>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;

// Use a small window size -- something that can easily managmed on pretty much any comptuer 
#define WIDTH 640
#define HEIGHT 480

#define ASSERT(x)	do { assert((x)); } while(0)

typedef enum _PIXEL_TYPE
{
   PT_COLOR,
   PT_MONO,
   PT_OTHERWISE // Special cameras, like polarized and interleaved HDR
} PIXEL_TYPE;

// LOCAL FUNCTIONS
PIXEL_TYPE getPixelType (HANDLE hCamera);

int 
main(const int argc, char const * const argv[])
{
   // 
   // Step 1
   //      Prepare the camera

   // Initialize any camera
   HANDLE hCamera;
   PXL_RETURN_CODE rc = PxLInitialize(0, &hCamera);
   if (!API_SUCCESS(rc)) {
      printf("Error: Unable to initialize a camera\n");
      return EXIT_FAILURE;
   }

   //
   // Step 2
   //      Figure out if this is a mono or color camera, so that we know the type of
   //      image we will be working with. 
   PIXEL_TYPE pixelType = getPixelType(hCamera);
   if (pixelType == PT_OTHERWISE) {
      printf("Error: We can't deal with this type of camera\n");
      PxLUninitialize(hCamera);
      return EXIT_FAILURE;
   }

   //
   // Step 3
   //    Set the ROI to a smaller size, something that can be easily displayed/previewed on small window
   //    on pretty much any system
   float roi[] = {0,0,WIDTH,HEIGHT}; // set size, offest (0,0)
   rc = PxLSetFeature (hCamera, FEATURE_ROI, FEATURE_FLAG_MANUAL, 4, roi);
   if (!API_SUCCESS(rc)) {
      printf("Error: Unable to set the ROI\n");
      PxLUninitialize(hCamera);
      return EXIT_FAILURE;
   }

   // Just going to declare a very large buffer here
   // One that's large enough for for all pixel formats for the given window size
   std::vector<U8> frameBuffer(WIDTH * HEIGHT * 2);
   FRAME_DESC frameDesc;
   frameDesc.uSize = sizeof(FRAME_DESC); // Let the Pixelink API know what version we were compiled against
   // color has 3 channels, mono just 1
   U32 imageBufferSize = (U32)frameBuffer.size() * (pixelType == PT_COLOR ? 3 : 1);
   std::vector<U8> imageBuffer(imageBufferSize); 

    //
    // Step 4
    //      Start the stream and tell the user how to exit the loop
    rc = PxLSetStreamState(hCamera, START_STREAM);
    if (!API_SUCCESS(rc)) {
      printf("Error: Unable to start the stream on the camera\n");
      PxLUninitialize(hCamera);
      return EXIT_FAILURE;
    }

    printf (" Grabbing / previewing images.  Press any key to stop...\n");

    //
    // Step 5
    //      Grab an image from the camera, convert it into a OpenCV mat, and preview it using
    //      openCV.  Pressing a key will exit
    
    while (true) {
    
      rc = PxLGetNextFrame(hCamera, (U32)frameBuffer.size(), &frameBuffer[0], &frameDesc);
      if (API_SUCCESS(rc)) {
        // Convert it to an image buffer.  Note that frame can be in any one of a large number of pixel
        // formats, so we will simplify things by converting all mono to mono8, and all color to rgb24
        if (pixelType == PT_MONO) {
           rc = PxLFormatImage(&frameBuffer[0], &frameDesc, IMAGE_FORMAT_RAW_MONO8, &imageBuffer[0], &imageBufferSize);
        } else {
           rc = PxLFormatImage(&frameBuffer[0], &frameDesc, IMAGE_FORMAT_RAW_BGR24, &imageBuffer[0], &imageBufferSize);
        }
        if (API_SUCCESS(rc)) {

            //
            // Step 5.1
            //      'convert' the image to one that openCV can manipulate
            Mat  openCVImage((int)(frameDesc.Roi.fHeight/frameDesc.PixelAddressingValue.fVertical),
                             (int)(frameDesc.Roi.fWidth/ frameDesc.PixelAddressingValue.fHorizontal),
                             pixelType == PT_MONO ? CV_8UC1 : CV_8UC3,
                             &imageBuffer[0]);
            if (openCVImage.data) {
                // 
                //  Step 5.2
                //      Do openCV manipulations on the matrix here.  
                //      We will simply preview it
                imshow("Live", openCVImage);
            }
         }
      } else {
            printf("Error: Could not grab an image from the camera\n");
        }

        if (waitKey(5) >= 0) break;
   }


   //
   // Step 6
   //      Cleanup
   rc = PxLSetStreamState(hCamera, STOP_STREAM);
   ASSERT(API_SUCCESS(rc));
   rc = PxLUninitialize(hCamera);
   ASSERT(API_SUCCESS(rc));

   return EXIT_SUCCESS;
}

// IMPORTANT NOTE:
//    This function will only return a meaningful value, if called while NOT streaming
PIXEL_TYPE getPixelType(HANDLE hCamera)
{
   PIXEL_TYPE pixelType = PT_OTHERWISE;
   // Take a simple minded approach; All Pixelink monochrome cameras support PIXEL_FORMAT_MONO8, and all
   // Pixelink color cameas support PIXEL_FORMAT_BAYER8.  So, try setting each of these to see if it 
   // works.

   //  However, we should take care to restore the current pixel format.
   U32 flags = 0;
   U32 numParams = 1;
   float savedPixelFormat;
   float newPixelFormat;
   PXL_RETURN_CODE rc = PxLGetFeature (hCamera, FEATURE_PIXEL_FORMAT, &flags, &numParams, &savedPixelFormat);
   if (!API_SUCCESS(rc)) return pixelType;

   // Is it mono??
   newPixelFormat = PIXEL_FORMAT_MONO8;
   rc = PxLSetFeature (hCamera, FEATURE_PIXEL_FORMAT, FEATURE_FLAG_MANUAL, 1, &newPixelFormat);
   if (!API_SUCCESS(rc)) {
      // Nope, so is it color?
      newPixelFormat = PIXEL_FORMAT_BAYER8;
      rc = PxLSetFeature(hCamera, FEATURE_PIXEL_FORMAT, FEATURE_FLAG_MANUAL, 1, &newPixelFormat);
      if (API_SUCCESS(rc)) {
         // Yes, it IS color
         pixelType = PT_COLOR;
      }
   } else {
      // Yes, it IS mono
      pixelType = PT_MONO;
   }

   // Restore the saved pixel format
   PxLSetFeature(hCamera, FEATURE_PIXEL_FORMAT, FEATURE_FLAG_MANUAL, 1, &savedPixelFormat);

   return pixelType;
}

