//
// Camera-specific funtions
//
// Note that most of these are taken from other PixeLINK demo apps
//
#ifndef CAMERAUTILS_H
#define CAMERAUTILS_H

PXL_RETURN_CODE EnableSoftwareTriggering(HANDLE hCamera);
PXL_RETURN_CODE EnableHardwareTriggering(HANDLE hCamera);
PXL_RETURN_CODE EnableGpoForStrobe(HANDLE hCamera, int gpoNumber);

#endif // CAMERAUTILS_H