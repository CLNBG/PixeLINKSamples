#pragma once


#include "PxLDevApp.h"

/******************************************************************************
* CFeatureDialog dialog
******************************************************************************/

class CFeatureDialog : public CDialog
{
	DECLARE_DYNAMIC(CFeatureDialog)

	DECLARE_MESSAGE_MAP()

public:
	friend class CDescriptorDialog;
	
	CFeatureDialog(UINT nID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CFeatureDialog();
	virtual BOOL OnInitDialog();
	afx_msg void OnKickIdle(void);

	enum { IDD = 0 }; // This is only a base class

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
	U32				m_featureId;
	CPxLCamera*		m_pCam;
	bool			m_bAdjusting;
	CWnd*			m_location;

	virtual void Configure(void) = 0;
	virtual void Populate(void) = 0;

	virtual void OnApiError(PxLException const& e, bool cameraRelated = true);

};
