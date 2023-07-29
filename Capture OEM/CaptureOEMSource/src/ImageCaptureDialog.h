#pragma once

#include "afxcmn.h"
#include "afxwin.h"


class CImagesGui;

// CImageCaptureDialog dialog

class CImageCaptureDialog : public CDialog
{
	DECLARE_DYNAMIC(CImageCaptureDialog)

	DECLARE_MESSAGE_MAP()

public:
	CImageCaptureDialog(CImagesGui* pParent);
	virtual ~CImageCaptureDialog();

// Dialog Data
	enum { IDD = IDD_IMAGECAPTURE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();

private:
	CImagesGui*		m_parent;
    HANDLE          m_ImageCaptureThread;
    int             m_SavedStreamState;
    BOOL            m_ImageCaptured;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
    static ULONG __stdcall ImageCaptureThread(LPVOID context);
};
