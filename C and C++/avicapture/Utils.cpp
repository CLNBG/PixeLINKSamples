//
// Utils.cpp: A few utility functions. 
//
//
#include "stdafx.h"
#include "Utils.h"


//
// If 'dir' is blank, don't change the directory.
//
bool
GetListOfFilesInDir(const CString& dir, const CString& pattern, std::vector<CString>& files)
{
	CFileFind fileFinder;   // Mfc Class that supports serching for files
	char oldDirectory[512]; // Array big enough to hold
                           // the path of the old current dir
	//Save old current directory as we will change to 'directory'
	//to enumerate its files. We will be reset at the end of this func.
	//CFileFind search only serches files in the current dir.
	GetCurrentDirectory(512, oldDirectory);

	if (dir.GetLength() != 0) {
		if (!SetCurrentDirectory(dir)) {
			return false;
		}
	}
 
	BOOL working = fileFinder.FindFile(pattern);
  
	while(working) {
		working = fileFinder.FindNextFile();
		CString file (fileFinder.GetFileName());
		if (fileFinder.IsDots() || fileFinder.IsDirectory()) {
			continue;
		}
		files.push_back(file);
	}
 
	//Restore old Path
	SetCurrentDirectory(oldDirectory);

	return true;
}




bool
DeletePdsAndAviFiles()
{
	if (!DeleteFiles("*.pds")) {
		AfxMessageBox("Unable to delete one or more PDS files");
		return false;
	}
	if (!DeleteFiles("*.avi")) {
		AfxMessageBox("Unable to delete one or more AVI files");
		return false;
	}
	return true;
}

bool 
DeleteFiles(const CString& pattern)
{
	std::vector<CString> pdsFiles;
	if (!GetListOfFilesInDir("", pattern, pdsFiles)) {
		return false;
	}

	bool success = true;
	for(size_t i=0; i < pdsFiles.size(); i++) {
		if (!DeleteFile(pdsFiles[i])) {
			success = false;
		}
	}

	return success;
}



//
// Read the frame rate to get the number of frames per seconds.
// Can't just read the exposure time and invert because that doesn't take into
// account readout time.
//
U32 
DetermineFramesPerSecond(HANDLE hCamera)
{
	U32 flags;
	U32 numParams = 1;
	float framesPerSecond = 0.0f;	// time in seconds

	PXL_RETURN_CODE rc = PxLGetFeature(hCamera, FEATURE_FRAME_RATE, &flags, &numParams, &framesPerSecond);
	ASSERT(API_SUCCESS(rc));
	ASSERT(0.0 != framesPerSecond);

	// Round up to the next integer.
	U32 fps = static_cast<U32>(framesPerSecond) + 1;

	return fps;
}
