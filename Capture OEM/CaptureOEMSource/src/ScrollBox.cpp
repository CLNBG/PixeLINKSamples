// ScrollBox.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "ScrollBox.h"


// CScrollBox

IMPLEMENT_DYNAMIC(CScrollBox, CWnd)

CScrollBox::CScrollBox(CWnd &wnd)
	: CWnd()
	, m_ContainedWnd(wnd)
{
}

CScrollBox::~CScrollBox()
{
}


// CScrollBox message handlers


BEGIN_MESSAGE_MAP(CScrollBox, CWnd)
	ON_WM_VSCROLL()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

void CScrollBox::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
	int curPos = this->GetScrollPos(SB_VERT);
	int newPos = curPos;

	switch (nSBCode)
	{
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		newPos = static_cast<int>(nPos);
		break;
	case SB_LINEDOWN:
		newPos = min(curPos + 10, this->GetScrollLimit(SB_VERT));
		break;
	case SB_LINEUP:
		newPos = max(curPos - 10, 0);
		break;
	case SB_PAGEDOWN:
		newPos = min(curPos + 200, this->GetScrollLimit(SB_VERT));
		break;
	case SB_PAGEUP:
		newPos = max(curPos - 200, 0);
		break;
	case SB_BOTTOM:
	case SB_ENDSCROLL:
	case SB_TOP:
		break;
	}

	this->SetScrollPos(SB_VERT, newPos);
	m_ContainedWnd.SetWindowPos(NULL, 0, -newPos, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

}


BOOL CScrollBox::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


// Ensure that the vertical coordinates in the contained window are in view.
void CScrollBox::ForceIntoView(int vPosTop, int vPosBottom)
{
	int curPos;
	while ((curPos = this->GetScrollPos(SB_VERT)) > vPosTop)
		this->OnVScroll(SB_PAGEUP, 0, NULL);

	CRect r;
	this->GetClientRect(r);

	while ((curPos = this->GetScrollPos(SB_VERT)) < vPosBottom - r.Height())
		this->OnVScroll(SB_PAGEDOWN, 0, NULL);
}


void CScrollBox::OnSetFocus(CWnd* pOldWnd)
{
	::PostMessage(m_ContainedWnd.m_hWnd, WM_SETFOCUS, 0, 0);
}

















