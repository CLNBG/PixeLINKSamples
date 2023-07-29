//
// digitalgain.c 
//
// Set linear digital gain between 0 and 60 dB using a lookup table.
// 

#include <PixeLINKAPI.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>


// Local function helpers
static int Initializedigitalgain(HANDLE hCamera, float dB, float* pSlope);
static int PrintCurrentLookupTable(HANDLE hCamera);
static void CreateLookupLine(float* pParams, U32 startX, U32 startY, U32 endX, U32 endY);

int 
main(int argc, char* argv[])
{
    // Initialize any camera
    HANDLE hCamera;
    PXL_RETURN_CODE rc = PxLInitialize(0, &hCamera);
    if (!(API_SUCCESS(rc)))
    {
        printf("Error: Unable to initialize a camera\n");
        return EXIT_FAILURE;
    }

    // Initial digital gain value and gain slope
    float digitalGain = -1.0f;
    float slope = 1.0f;
    
    // Selected digital gain
    if (NULL != argv[1])
    {
        digitalGain = (float)atof(argv[1]);
        printf("\nSelected digital gain is %.2f dB.\n\n", digitalGain);
    }

    // Checks that the selected digital gain is between 0 and 60 dB
    if (digitalGain < 0.0f || digitalGain > 60.0f)
    {
        printf("Provide value of digital gain between 0 and 60 dB.\n");
        return EXIT_FAILURE;
    }
    
    // Print current lookup table settings
    printf("Current lookup table settings:\n");
    if (PrintCurrentLookupTable(hCamera) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    
    // Set digital gain
    if (Initializedigitalgain(hCamera, digitalGain, &slope) == EXIT_FAILURE)
    {
        PxLUninitialize(hCamera);
        return EXIT_FAILURE;
    }

    // Print set digital gain and its slope
    printf("\nDigital gain is set to %.2f dB.\n", digitalGain);
    printf("The slope of this gain is %.3f.\n\n", slope);

    // Print new lookup table settings
    printf("New lookup table settings:\n");
    if (PrintCurrentLookupTable(hCamera) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    PxLUninitialize(hCamera);
    return EXIT_SUCCESS;
}

static int
PrintCurrentLookupTable(HANDLE hCamera)
{
    U32 flags = 0;
    U32 numParms = 0;
    float* pParms = NULL;

    assert(NULL != hCamera);

    // Find out how many parameters there are
    PXL_RETURN_CODE rc = PxLGetFeature(hCamera, FEATURE_LOOKUP_TABLE, &flags, &numParms, NULL);
    if (!(API_SUCCESS(rc))) 
    {
        return EXIT_FAILURE;
    }

    //
    // We have to read the current values in order to read the current flag value.
    // Alloc the array required to hold the current values.
    //
    pParms = (float*)malloc(numParms * sizeof(float));
    if (NULL == pParms)
    {
        return EXIT_FAILURE;
    }

    // Read the current entries and get the flags
    rc = PxLGetFeature(hCamera, FEATURE_LOOKUP_TABLE, &flags, &numParms, pParms);
    if (!(API_SUCCESS(rc)))
    {
        free(pParms);
        return EXIT_FAILURE;
    }

    // Print current lookup table settings
    for (U32 i = 0; i < numParms; i++)
    {
        printf("lookupTable[%4d] = %7.2f\n", i, pParms[i]);
    }

    free(pParms);
    return EXIT_SUCCESS;
}

static int
Initializedigitalgain(HANDLE hCamera, float dB, float* pSlope)
{
    U32 flags = 0;
    U32 numParms = 0;
    float* pParms = NULL;

    assert(0 != hCamera);
    assert(0 <= dB);
    assert(NULL != pSlope);
        
    // Find out how many parameters there are
    PXL_RETURN_CODE rc = PxLGetFeature(hCamera, FEATURE_LOOKUP_TABLE, &flags, &numParms, NULL);
    if (!(API_SUCCESS(rc))) 
    {
        return EXIT_FAILURE;
    }

    //  We have to read the current values in order to read the current flag value. 
    //  Alloc the array required to hold the current values
    pParms = (float*)malloc(numParms * sizeof(float));
    if (NULL == pParms)
    {
        return EXIT_FAILURE;
    }
    
    // Read the current entries and get the flags
    rc = PxLGetFeature(hCamera, FEATURE_LOOKUP_TABLE, &flags, &numParms, pParms);
    if (!(API_SUCCESS(rc))) 
    {
        free(pParms);
        return EXIT_FAILURE;
    }

    // Calculate the lookup table slope according to the selected linear digital gain
    if (0.0f != dB)
    {
        *pSlope = powf(10.0f, dB / 20.0f);
    }

    // Calculate endX coordinate
    int endX = roundf((float)(numParms-1) / *pSlope - 0.5f);

    // Initialize the lookup table
    CreateLookupLine(pParms, 0, 0, endX, numParms - 1);
    if (1.0 != *pSlope)
    {
        CreateLookupLine(pParms, endX, numParms - 1, numParms - 1, numParms - 1);
    }

    // Ensure that the lookup table is enabled
    flags &= ENABLE_FEATURE(flags, 1);

    // Write to camera
    rc = PxLSetFeature(hCamera, FEATURE_LOOKUP_TABLE, flags, numParms, pParms);
    free(pParms);
    
    return (API_SUCCESS(rc)) ? EXIT_SUCCESS : EXIT_FAILURE;
}

//
//  Given a pointer to the entire lookup table, create a straight line 
//  between (startX, startY) and (endX, endY) 
//
static void 
CreateLookupLine(float* pTable, U32 startX, U32 startY, U32 endX, U32 endY)
{
    assert(NULL != pTable);
    assert(0 <= startX);
    assert(0 <= startY);
    assert(0 <= endX);
    assert(0 <= endY);

    float rise = (float)endY - (float)startY;
    float run  = (float)endX - (float)startX;
    float slope = rise / run;
    float yIntercept = startY - (slope * startX);
    float y;
    
    for(U32 x = startX; x <= endX; x++) 
    {
        y = slope * (float)x + yIntercept;
        pTable[x] = y;
    }
}

