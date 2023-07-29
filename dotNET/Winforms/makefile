
!include $(BUILD_TOOLS)\makefile.vs08

build: build_cpluspluscli build_csharp build_vb



build_cpluspluscli:
	@echo.
	@echo Building C++ CLI demo applications...
	@echo.
	@cd CPlusPlusCLI
	@$(MAKE) build
	@cd ..


build_csharp:
	@echo.
	@echo Building C# demo applications ...
	@echo.
	@cd CSharp
	@$(MAKE) build
	@cd ..

build_vb:
	@echo.
	@echo Building VB demo applications ...
	@echo.
	@cd VB
	@$(MAKE) build
	@cd ..


all: build release

release: