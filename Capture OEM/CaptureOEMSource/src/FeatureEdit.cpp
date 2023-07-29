// FeatureEdit.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "FeatureEdit.h"


// CFeatureEdit

IMPLEMENT_DYNAMIC(CFeatureEdit, CEdit)

CFeatureEdit::CFeatureEdit()
{
}

CFeatureEdit::~CFeatureEdit()
{
}


BEGIN_MESSAGE_MAP(CFeatureEdit, CEdit)
END_MESSAGE_MAP()



// CFeatureEdit message handlers

BOOL CFeatureEdit::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		int ss, se;
		this->GetSel(ss,se);
		::SendMessage(this->GetSafeHwnd(), WM_KILLFOCUS, 0, 0);
		::SendMessage(this->GetSafeHwnd(), WM_SETFOCUS, 0, 0);
		this->SetSel(ss,se);
	}

	return CEdit::PreTranslateMessage(pMsg);
}
