!include $(BUILD_TOOLS)\makefile.vs08

#//
#// Build everything
#//
build:
	@set DEMO_NAME=Initialize
	@$(MAKE) build_demo

#//
#// Set the environment var DEMO_NAME to the name of the demo to build
#//
build_demo:
	@set SOLUTION_DIR="%%DEMO_NAME%%\"
	@set SOLUTION=%DEMO_NAME%\%%DEMO_NAME%%
	@$(MAKE) clean_build_tidyup
