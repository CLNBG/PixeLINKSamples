#pragma once


#include "DevAppPage.h"


// CExampleGui dialog

class CExampleGui : public CDevAppPage
{
	DECLARE_DYNCREATE(CExampleGui)

	DECLARE_MESSAGE_MAP()

public:
	CExampleGui(CPxLDevAppView* pView = NULL);
	~CExampleGui();
	int GetHelpContextId();

	enum { IDD = IDD_EXAMPLE_GUI };
	UINT GetDlgId() { return IDD; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private:
	void SetGUIState(eGUIState state);
	void PopulateDefaults(void);
	void ApplyUserSettings(UserSettings& settings);
	void StoreUserSettings(UserSettings& settings);
	void PopulateControls(void);
	void ConfigureControls(void);
	void FeatureChanged(U32 featureId);

};








