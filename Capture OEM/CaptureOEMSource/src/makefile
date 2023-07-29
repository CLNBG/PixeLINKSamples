#//
#// Capture OEM
#//
#// Build Capture OEM, and when done, copy the files and binaries to
#// the release dir for packaging with the API installation.
#//
SOLUTION=PxLDevApp
!include $(BUILD_TOOLS)\makefile.vs08

#//
#// Build everything
#//
build:
	@$(MAKE) clean_build_tidyup
	@cd Controls
	@$(MAKE) build
	@cd ..
	

#//
#// Prepare for release 
#//
#// NOTE: 
#// $(CD) is 'cd /d' and 'cd /d ..' doesn't work, so just use 'cd ..'
#//
release: make_temp_copy add_legal_headers test_compile create_source_zip delete_temp_copy

release_binaries:

#// Assumes current dir is host\CaptureOEM
#// Leave current dir to be host\temp
make_temp_copy:
	echo Creating Capture OEM source zip file...
	cd ..
	$(MKDIR) temp
	$(MKDIR) temp\CaptureOEM
	$(CPDIR) CaptureOEM temp\CaptureOEM
	cd temp

#// Assumes current dir is host\temp, and leaves it there
add_legal_headers:
	cd CaptureOEM
	perl $(BUILD_TOOLS)\prependtofiles.pl $(BUILD_TOOLS)\legalheader.txt .\ "*.c*"
	perl $(BUILD_TOOLS)\prependtofiles.pl $(BUILD_TOOLS)\legalheader.txt .\ "*.h"
	cd ..

#//
#// After addding the legal headers, make sure we can still compile everything
#//
#// Assumes current dir is host\temp, and leaves it there
test_compile:
	cd CaptureOEM
	$(MAKE) clean_build_tidyup
	cd ..


#// Assumes current dir is host\temp, and leaves it there
create_source_zip:
	cd CaptureOEM
	$(RM) makefile
	$(RMDIR) Release
	$(RMDIR) Debug
	$(RMDIR) Controls
	cd .. 
	python $(BUILD_TOOLS)\zipdir.py CaptureOEM CaptureOEMSource.zip
	$(MV) CaptureOEMSource.zip ..\CaptureOEM\Release


#// Assumes current dir is host\temp, and leaves it as host\CaptureOEM
delete_temp_copy:
	cd ..
	$(RMDIR) temp
	$(CD) CaptureOEM 
	

