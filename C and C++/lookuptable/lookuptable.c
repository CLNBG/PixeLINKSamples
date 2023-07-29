/*
 * lookuptable.c 
 *
 * Display, and optionally initialize and enable a lookup table
 */

#include <PixeLINKAPI.h>
#include <stdio.h>
#include <assert.h>


/* Local function helpers */
static int PrintCurrentLookupTable(HANDLE hCamera);
static int InitializeLookupTable(HANDLE hCamera);
static void	CreateLookupLine(float* pParams, U32 startX, U32 startY, U32 endX, U32 endY);

/* Enable this define if you want to change the value of the lookup table. */
#define CREATE_NEW_LOOKUP_TABLE


int 
main(int argc, char* argv[])
{
	HANDLE hCamera;
	PXL_RETURN_CODE rc = PxLInitialize(0, &hCamera);
	if (!(API_SUCCESS(rc))) {
		return EXIT_FAILURE;
	}

	printf("Current lookup table settings\n");
	PrintCurrentLookupTable(hCamera);

#ifdef CREATE_NEW_LOOKUP_TABLE
	printf("\nCreating a new lookup table...\n");
	InitializeLookupTable(hCamera);

	printf("\nCurrent lookup table settings\n");
	PrintCurrentLookupTable(hCamera);
#endif

	PxLUninitialize(hCamera);
	return EXIT_SUCCESS;
}

static int
PrintCurrentLookupTable(HANDLE hCamera)
{
	U32 flags = 0;
	U32 numParms = 0;
	float* pParms = NULL;
	U32 i;

	/* Find out how many parameters there are */
	PXL_RETURN_CODE rc = PxLGetFeature(hCamera, FEATURE_LOOKUP_TABLE, &flags, &numParms, NULL);
	if (!(API_SUCCESS(rc))) {
		return 0;
	}

	/*
	 * We have to read the current values in order to read the current flag value. 
	 * Alloc the array required to hold the current values.
	 */
	pParms = (float*)malloc(numParms * sizeof(float));
	if (NULL == pParms) {
		return 0;
	}
	
	/* Read the current entries and get the flags */
	rc = PxLGetFeature(hCamera, FEATURE_LOOKUP_TABLE, &flags, &numParms, pParms);
	if (!(API_SUCCESS(rc))) {
		free(pParms);
		return 0;
	}

	for(i = 0; i < numParms; i++) {
		printf("lookupTable[%4d] = %7.2f\n", i, pParms[i]);
	}

	free(pParms);
	return 1;
}

static int
InitializeLookupTable(HANDLE hCamera)
{
	U32 flags = 0;
	U32 numParms = 0;
	float* pParms = NULL;

	/* Find out how many parameters there are */
	PXL_RETURN_CODE rc = PxLGetFeature(hCamera, FEATURE_LOOKUP_TABLE, &flags, &numParms, NULL);
	if (!(API_SUCCESS(rc))) {
		return 0;
	}

	/*
	 * We have to read the current values in order to read the current flag value. 
	 * Alloc the array required to hold the current values
	 */
	pParms = (float*)malloc(numParms * sizeof(float));
	if (NULL == pParms) {
		return 0;
	}
	
	/* Read the current entries and get the flags */
	rc = PxLGetFeature(hCamera, FEATURE_LOOKUP_TABLE, &flags, &numParms, pParms);
	if (!(API_SUCCESS(rc))) {
		free(pParms);
		return 0;
	}

	/* Initialize the lookup table */
	CreateLookupLine(pParms,     0,    0,  400,  100);
	CreateLookupLine(pParms,   400,  100,  623,  923);
	CreateLookupLine(pParms,   623,  923, 1023, 1023);
	if (numParms > 1024) {
		CreateLookupLine(pParms,   1024,  1024, numParms-1, numParms-1);
	}

	/* Ensure that the lookup table is enabled */
	flags &= ENABLE_FEATURE(flags, 1);

	/* Write to camera */
	rc = PxLSetFeature(hCamera, FEATURE_LOOKUP_TABLE, flags, numParms, pParms);
	free(pParms);

	return (API_SUCCESS(rc)) ? 1 : 0;
}

/*
 * Given a pointer to the entire lookup table, create a straight line 
 * between (startX,startY) and (endX,endY) 
 */
static void 
CreateLookupLine(float* pTable, U32 startX, U32 startY, U32 endX, U32 endY)
{
	float rise = (float)endY - (float)startY;
	float run  = (float)endX - (float)startX;
	float slope = rise / run;
	float yIntercept = startY - (slope * startX);
	float y;
	U32 x;

	for(x = startX; x <= endX; x++) {
		y = slope * (float)x + yIntercept;
		pTable[x] = y;
	}
}

