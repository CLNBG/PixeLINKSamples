

#pragma once

#include <vector>
#include <PixeLINKApi.h>

U32		DetermineFramesPerSecond(HANDLE hCamera);
bool	GetListOfFilesInDir(const CString& dir, const CString& pattern, std::vector<CString>& files);
bool	DeletePdsAndAviFiles();
bool	DeleteFiles(const CString& pattern);

