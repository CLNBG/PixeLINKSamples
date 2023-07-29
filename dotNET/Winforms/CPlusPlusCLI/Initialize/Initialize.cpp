// Initialize.cpp : main project file.

#include "stdafx.h"

using namespace System;
using namespace PixeLINK;

static bool 
IsFeatureSupported(const int hCamera, const Feature featureId)
{
	CameraFeature featureInfo;
	ReturnCode rc = Api::GetCameraFeatures(hCamera, featureId, featureInfo);
	if (!Api::IsSuccess(rc)) 
	{
		return false;
	}

	return featureInfo.IsSupported;
}

int main(array<System::String ^> ^args)
{
	int hCamera;
	ReturnCode rc = Api::Initialize(0, hCamera);
	if (!Api::IsSuccess(rc)) 
	{
		Console::WriteLine("Unable to initialize a camera (Error {0})", rc);
		return 1;
	}

	Console::WriteLine("Camera successfully detected");

	// Do you interaction with the camera here
	CameraInformation ci;
	rc = Api::GetCameraInformation(hCamera, ci);
	if (!Api::IsSuccess(rc)) 
	{
		Console::WriteLine("Unable to read camera information (Error {0})\n", rc);
	}
	else
	{
		Console::WriteLine("Serial # : {0}", ci.SerialNumber);
		Console::WriteLine("Name     : {0}", ci.CameraName);
	}

	// Does this camera support an exposure
	if (IsFeatureSupported(hCamera, Feature::Exposure))
	{
		FeatureFlags flags;
		int numberOfParameters(1);
		array<float>^ parameters = gcnew array<float>(numberOfParameters);
		Api::GetFeature(hCamera, Feature::Exposure, flags, numberOfParameters,parameters);
		if (!Api::IsSuccess(rc)) 
		{
			Console::WriteLine("Unable to read exposure time (Error {0})\n", rc);
		}
		else
		{
			Console::WriteLine("Exposure : {0} ms", parameters[0] * 1000.0f);
		}
	}
	Api::Uninitialize(hCamera);



}
