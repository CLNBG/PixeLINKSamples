// DevAppTabs.cpp : implementation file

#include "stdafx.h"
#include "PxLDevApp.h"
#include "DevAppPage.h"
#include "DevAppTabs.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/******************************************************************************
* Class CDevAppTabs
******************************************************************************/

IMPLEMENT_DYNAMIC(CDevAppTabs, CPropertySheet)


/**
* Function: CDevAppTabs
* Purpose:  Constructor
*/
CDevAppTabs::CDevAppTabs(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}


/**
* Function: CDevAppTabs
* Purpose:  Constructor
*/
CDevAppTabs::CDevAppTabs(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}


/**
* Function: ~CDevAppTabs
* Purpose:  Destructor
*/
CDevAppTabs::~CDevAppTabs()
{
}


BEGIN_MESSAGE_MAP(CDevAppTabs, CPropertySheet)
END_MESSAGE_MAP()


/**
* Function: SetSingleTabMode
* Purpose:  Disable all tabs except the specified one.
*/
void CDevAppTabs::SetSingleTabMode(CDevAppPage* pPage)
{
	for (int i = 0; i < this->GetPageCount(); i++)
	{
		CDevAppPage* pg = static_cast<CDevAppPage*>(this->GetPage(i));
		ASSERT(pg->IsKindOf(RUNTIME_CLASS(CDevAppPage)));
		pg->SetEnabled(pg == pPage);
	}

	this->SetActivePage(pPage);

	this->GetTabControl()->EnableWindow(FALSE);
}


/**
* Function: SetMultiTabMode
* Purpose:  Enable all tabs.
*/
void CDevAppTabs::SetMultiTabMode(int activeIndex)
{
	this->GetTabControl()->EnableWindow(TRUE);

	for (int i = 0; i < this->GetPageCount(); i++)
	{
		CDevAppPage* pAppPage = static_cast<CDevAppPage*>(this->GetPage(i));
		ASSERT(pAppPage->IsKindOf(RUNTIME_CLASS(CDevAppPage)));
		pAppPage->SetEnabled(true);
		//this->SetActivePage(i);
	}

	ASSERT(activeIndex < this->GetPageCount());
	this->SetActivePage(activeIndex);
}


/**
* Function: OnInitDialog
* Purpose:  
*/
BOOL CDevAppTabs::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	m_hAccellTable = LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));

	return bResult;
}


/**
* Function: PreTranslateMessage
* Purpose:  
*/
BOOL CDevAppTabs::PreTranslateMessage(MSG* pMsg)
{
	// By default, when the focus is in a PropertySheet within a child window
	// or View, accelerator keys are not passed on to the main window.
	// We need to do it explicitly.
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{
		if (::TranslateAccelerator(AfxGetMainWnd()->GetSafeHwnd(), m_hAccellTable, pMsg))
			return TRUE;
	}

	return CPropertySheet::PreTranslateMessage(pMsg);
}













