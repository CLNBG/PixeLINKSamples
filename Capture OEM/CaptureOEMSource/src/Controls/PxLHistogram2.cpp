// PxLHistogram2.cpp : Implementation of CPxLHistogram2
#include "stdafx.h"
#include "PxLHistogram2.h"


// CPxLHistogram2


CPxLHistogram2::CPxLHistogram2()
{
}


// IPxLHistogram2
public:
HRESULT CPxLHistogram2::OnDrawAdvanced(ATL_DRAWINFO& di)
{
	RECT& rc = *(RECT*)di.prcBounds;
	// Set Clip region to the rectangle specified by di.prcBounds
	HRGN hRgnOld = NULL;
	if (GetClipRgn(di.hdcDraw, hRgnOld) != 1)
		hRgnOld = NULL;
	bool bSelectOldRgn = false;

	HRGN hRgnNew = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);

	if (hRgnNew != NULL)
	{
		bSelectOldRgn = (SelectClipRgn(di.hdcDraw, hRgnNew) != ERROR);
	}

	Rectangle(di.hdcDraw, rc.left, rc.top, rc.right, rc.bottom);
	SetTextAlign(di.hdcDraw, TA_CENTER|TA_BASELINE);
	LPCTSTR pszText = _T("ATL 7.0 : PxLHistogram2");
	TextOut(di.hdcDraw, 
		(rc.left + rc.right) / 2, 
		(rc.top + rc.bottom) / 2, 
		pszText, 
		lstrlen(pszText));

	if (bSelectOldRgn)
		SelectClipRgn(di.hdcDraw, hRgnOld);
	DeleteObject(hRgnNew);

	return S_OK;
}


void CPxLHistogram2::OnBackColorChanged()
{
	ATLTRACE(_T("OnBackColorChanged\n"));
}


HRESULT CPxLHistogram2::FinalConstruct()
{
	return S_OK;
}


void CPxLHistogram2::FinalRelease() 
{
}











