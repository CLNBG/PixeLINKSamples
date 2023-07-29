// PxLSubwindow.cpp : Implementation of CPxLSubwindow
#include "stdafx.h"
#include "PxLSubwindow.h"
#include ".\pxlsubwindow.h"
#include <utility>
#include <cmath>


/******************************************************************************
* CPxLSubwindow - Implementation
******************************************************************************/

/**
* Function: 
* Purpose:  
*/
CPxLSubwindow::CPxLSubwindow()
	: m_bMaintainAspectRatio(true)
	, m_ResizeAction(No_Resize)
	, m_bResizing(false)
	, m_MouseOffsetTop(0)
	, m_MouseOffsetLeft(0)
	, m_MouseOffsetRight(0)
	, m_MouseOffsetBottom(0)
{
	// Force the control to have a window, so that we can SetCapture().
	m_bWindowOnly = true;

	// Set some default colors.
	m_clrAmbientColor = static_cast<OLE_COLOR>(RGB(0xC0,0xC0,0xC0));
	m_clrBackColor = static_cast<OLE_COLOR>(RGB(255,255,255));
	m_clrBorderColor = static_cast<OLE_COLOR>(RGB(0,0,0));
	m_clrForeColor = static_cast<OLE_COLOR>(RGB(0xF0,0xF0,0xF0));
	m_clrFillColor = static_cast<OLE_COLOR>(RGB(255,0,0));
}


/**
* Function: 
* Purpose:  
*/
HRESULT CPxLSubwindow::FinalConstruct()
{
	m_bRecalcOuter = true;
	m_rectClient.SetPos(320, 272, 640, 480);
	m_ClientWidth = 1280;
	m_ClientHeight = 1024;
	m_MinWidth = 32;
	m_MinHeight = 32;

	// It appears that by the time FinalConstruct is called, the container has
	// done whatever it is that it needs to do to set the public properties
	// of the object. This means that we can use m_clrXXX here.
	OleTranslateColor(m_clrBackColor, NULL, &m_rectOuter.fillColor);
	OleTranslateColor(m_clrBorderColor, NULL, &m_rectOuter.lineColor);
	OleTranslateColor(m_clrFillColor, NULL, &m_rectInner.fillColor);
	OleTranslateColor(m_clrForeColor, NULL, &m_rectInner.lineColor);
	m_rectMoving.lineColor = m_rectInner.lineColor;


	m_rectOuter.lineWidth = m_rectInner.lineWidth = m_rectMoving.lineWidth = 1;
	m_rectInner.filled = m_rectOuter.filled = true;
	m_rectMoving.filled = false;

	m_clrAmbientColor = static_cast<OLE_COLOR>(GetSysColor(COLOR_ACTIVEBORDER));
	OLE_COLOR temp;
	if (SUCCEEDED(this->GetAmbientBackColor(temp)))
		m_clrAmbientColor = temp;

	return S_OK;
}


/**
* Function: 
* Purpose:  
*/
void CPxLSubwindow::FinalRelease() 
{
}


/**
* Function: AspectRatio
* Purpose:  Helper used by the following function (CalcOuterRect).
*/
template<typename T>
double AspectRatio(Rct<T> const& r)
{
	return static_cast<double>(r.Width()) / r.Height();
}


/**
* Function: CalcOuterRect
* Purpose:  Calculate the size and location of the outer rectangle. We want
*           to size the outer rectangle such that its _interior_ corresponds
*           to the client area.
*/
void CPxLSubwindow::CalcOuterRect(RECT const& r)
{
	// r is the drawing rectangle that we have to work with - it is passed
	// to us in the OnDraw method.

	// Make a Rct object to represent the available drawing area.
	Rct<int> rc;
	rc.SetPos(r);
	rc.lineWidth = 1;

	// To start with, we set the outer rectangle to fit *inside* rc.
	m_rectOuter.SetPos(rc.Inner());

	m_ratioX = static_cast<double>(m_rectOuter.InnerWidth()) / m_ClientWidth;
	m_ratioY = static_cast<double>(m_rectOuter.InnerHeight()) / m_ClientHeight;

	// If we are maintaining aspect ratio, we will probably need to adjust
	// either the X or Y size of the outer rectangle.
	if (m_bMaintainAspectRatio)
	{
		// Determine which dimension we need to shrink.
		double ctlAR = AspectRatio(m_rectOuter.Inner());
		double clientAR = static_cast<double>(m_ClientWidth) / m_ClientHeight;
		if (ctlAR > clientAR)
		{
			// Control is wider (relative to its height) than the client Region.
			// We need to adjust the X-ratio.
			m_ratioX = m_ratioY;
			int outerWidth = roundTo<int>(m_ratioX * m_ClientWidth) + (2 * m_rectOuter.lineWidth);
			int leftOver = rc.Width() - outerWidth;
			m_rectOuter.left = rc.left + (leftOver / 2);
			m_rectOuter.right = m_rectOuter.left + outerWidth;
		}
		else
		{
			// Control is taller (relative to its width) than the camera FOV.
			m_ratioY = m_ratioX;
			int outerHeight = roundTo<int>(m_ratioY * m_ClientHeight) + (2 * m_rectOuter.lineWidth);
			int leftOver = rc.Height() - outerHeight;
			m_rectOuter.top = rc.top + (leftOver / 2);
			m_rectOuter.bottom = m_rectOuter.top + outerHeight;
		}
	}

	// Calculate the subwindow rect, based on the new outer rect size.
	RecalcInnerRect();

	// ReCalculate the Client coordinates based on the new m_rectInner.
	// This is a workaround for the fact that the width and height cannot be exactly
	// represented in screen-pixel coordinates, and this causes them to appear to 
	// change as soon as the subwindow is first moved.
	//SetClientRect(m_rectInner);

	// Mark the outer rect as being properly size - we only need to call this function
	// if the control is resized (which will probably be rare), and on initial creation.
	m_bRecalcOuter = false;
}


/**
* Function: RecalcInnerRect
* Purpose:  Calculate m_rectInner from the m_rectClient.
*           Called when the client calls SetLeft, SetWidth, etc, etc..
*/
void CPxLSubwindow::RecalcInnerRect(void)
{
	m_rectInner.left   = m_rectOuter.InnerLeft() + (m_ratioX * m_rectClient.left);
	m_rectInner.right  = m_rectOuter.InnerLeft() + (m_ratioX * m_rectClient.right);
	m_rectInner.top    = m_rectOuter.InnerTop()  + (m_ratioY * m_rectClient.top);
	m_rectInner.bottom = m_rectOuter.InnerTop()  + (m_ratioY * m_rectClient.bottom);
}


/**
* Function: OnDraw
* Purpose:  Draw the control on the screen.
*/
HRESULT CPxLSubwindow::OnDraw(ATL_DRAWINFO& di)
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

	// Draw the background of the control.
	IRect bg(rc, 0, 0, true, 0);
	OleTranslateColor(m_clrAmbientColor, NULL, &bg.fillColor);
	bg.lineColor = bg.fillColor;

	// We may need to recalculate the outer rectangle (eg: if SetBounds has been
	// called since the last OnDraw.))
	if (m_bRecalcOuter)
	{

		this->GetParent().InvalidateRect(&rc);
		//bg.Draw(di.hdcDraw);
		CalcOuterRect(rc);
	}

	// Draw the outer rectangle
	m_rectOuter.Draw(di.hdcDraw);

	// If we are dragging, the inner rectangle is gray.
	m_rectInner.Draw(di.hdcDraw);

	if (m_bResizing)
	{
		m_rectMoving.Draw(di.hdcDraw);
	}

	if (bSelectOldRgn)
	{
		SelectClipRgn(di.hdcDraw, hRgnOld);
	}
	DeleteObject(hRgnNew);

	return S_OK;
}


LRESULT 
CPxLSubwindow::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	// TODO: Add your message handler code here and/or call default
	if (m_ResizeAction != No_Resize)
	{
		SetCursor(cursors[m_ResizeAction]);
		this->SetCapture();
		m_rectMoving.SetPos(m_rectInner);
		m_rectInner.lineColor = RGB(128,128,128);
		m_MouseOffsetLeft = GET_X_LPARAM(lParam) - m_rectInner.left;
		m_MouseOffsetTop = GET_Y_LPARAM(lParam) - m_rectInner.top;
		m_MouseOffsetRight = GET_X_LPARAM(lParam) - m_rectInner.right;
		m_MouseOffsetBottom = GET_Y_LPARAM(lParam) - m_rectInner.bottom;
		m_bResizing = true;
	}

	return 0;
}


LRESULT 
CPxLSubwindow::OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (!m_bEnabled) return 0;

	double x = GET_X_LPARAM(lParam);
	double y = GET_Y_LPARAM(lParam);

	if (m_bResizing)
	{
		ATLASSERT(m_ResizeAction != No_Resize);

		if (!(wParam & MK_LBUTTON))
		{
			// This should never happen, because we have called SetCapture.
			EndResize();
		}
		else
		{
			double newX, newY;

			if (m_ResizeAction == Move_NoResize)
			{
				// Get new coords for top-left corner
				newX = x - m_MouseOffsetLeft;
				newY = y - m_MouseOffsetTop;
				if (newX < m_rectOuter.InnerLeft())
					newX = m_rectOuter.InnerLeft();
				if (newY < m_rectOuter.InnerTop())
					newY = m_rectOuter.InnerTop();
				if (newX + m_rectMoving.Width() > m_rectOuter.InnerRight())
					newX = m_rectOuter.InnerRight() - m_rectMoving.Width();
				if (newY + m_rectMoving.Height() > m_rectOuter.InnerBottom())
					newY = m_rectOuter.InnerBottom() - m_rectMoving.Height(); // This fucks up - why????????
				m_rectMoving.ShiftTo(newX, newY);
			}
			else
			{
				// Handle Top/Bottom resizing
				switch (m_ResizeAction)
				{
				case TL_Resize:
				case T_Resize:
				case TR_Resize:
					newY = y - m_MouseOffsetTop;
					if (newY > m_rectMoving.bottom)
					{
						std::swap(m_rectMoving.top, m_rectMoving.bottom);
						std::swap(m_MouseOffsetTop, m_MouseOffsetBottom);
						switch (m_ResizeAction)
						{
						case TL_Resize: m_ResizeAction = BL_Resize; break;
						case T_Resize: m_ResizeAction = B_Resize; break;
						case TR_Resize: m_ResizeAction = BR_Resize; break;
						}
					}
					else
					{
						m_rectMoving.top = max(newY, m_rectOuter.InnerTop());
					}
					break;
				case BL_Resize:
				case B_Resize:
				case BR_Resize:
					newY = y - m_MouseOffsetBottom;
					if (newY < m_rectMoving.top)
					{
						std::swap(m_rectMoving.top, m_rectMoving.bottom);
						std::swap(m_MouseOffsetTop, m_MouseOffsetBottom);
						switch (m_ResizeAction)
						{
						case BL_Resize: m_ResizeAction = TL_Resize; break;
						case B_Resize: m_ResizeAction = T_Resize; break;
						case BR_Resize: m_ResizeAction = TR_Resize; break;
						}
					}
					else
					{
						m_rectMoving.bottom = min(newY, m_rectOuter.InnerBottom());
					}
					break;
				}

				// Handle Left/Right resizing
				switch (m_ResizeAction)
				{
				case TL_Resize:
				case L_Resize:
				case BL_Resize:
					newX = x - m_MouseOffsetLeft;
					if (newX > m_rectMoving.right)
					{
						std::swap(m_rectMoving.left, m_rectMoving.right);
						std::swap(m_MouseOffsetLeft, m_MouseOffsetRight);
						switch (m_ResizeAction)
						{
						case TL_Resize: m_ResizeAction = TR_Resize; break;
						case L_Resize: m_ResizeAction = R_Resize; break;
						case BL_Resize: m_ResizeAction = BR_Resize; break;
						}
					}
					else
					{
						m_rectMoving.left = max(newX, m_rectOuter.InnerLeft());
					}
					break;
				case TR_Resize:
				case R_Resize:
				case BR_Resize:
					newX = x - m_MouseOffsetRight;
					if (newX < m_rectMoving.left)
					{
						std::swap(m_rectMoving.left, m_rectMoving.right);
						std::swap(m_MouseOffsetLeft, m_MouseOffsetRight);
						switch (m_ResizeAction)
						{
						case TR_Resize: m_ResizeAction = TL_Resize; break;
						case R_Resize: m_ResizeAction = L_Resize; break;
						case BR_Resize: m_ResizeAction = BL_Resize; break;
						}
					}
					else
					{
						m_rectMoving.right = min(newX, m_rectOuter.InnerRight());
					}
					break;
				}
			}

			FireViewChange();
			SetClientRect(m_rectMoving);
			Fire_SubwindowChanging( roundTo<long>(m_rectClient.left), 
									roundTo<long>(m_rectClient.top), 
									roundTo<long>(m_rectClient.Width()), 
									roundTo<long>(m_rectClient.Height()));
		}
	}
	else
	{
		m_ResizeAction = GetResizeRegion(x, y, m_rectInner, 4);
	}

	SetCursor(cursors[m_ResizeAction]);

	return 0;
}


LRESULT 
CPxLSubwindow::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (m_bResizing)
	{
		ReleaseCapture();
		EndResize();
		m_ResizeAction = GetResizeRegion(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), m_rectInner, 4);
		SetCursor(cursors[m_ResizeAction]);
	}

	return 0;
}


// Called to end a resizing operation
void CPxLSubwindow::EndResize(void)
{
	m_bResizing = false;
	if (!m_rectInner.PosEquals(m_rectMoving))
	{
		m_rectInner.SetPos(m_rectMoving);
		OleTranslateColor(m_clrForeColor, NULL, &m_rectInner.lineColor);
		SetClientRect(m_rectInner);
		FireViewChange();
		FireSubwindowChanged();
	}
}


// Converts the supplied rect into client coordinates
void CPxLSubwindow::SetClientRect(DRect const& rc)
{
	m_rectClient.left = (rc.left - m_rectOuter.InnerLeft()) / m_ratioX;
	m_rectClient.right = m_rectClient.left + (rc.Width() / m_ratioX);
	m_rectClient.top = (rc.top - m_rectOuter.InnerTop()) / m_ratioY;
	m_rectClient.bottom = m_rectClient.top + (rc.Height() / m_ratioY);

	// Check that we are not smaller than the minimum size.
	if (m_rectClient.Width() < m_MinWidth)
	{
		if (m_ResizeAction == TL_Resize || m_ResizeAction == L_Resize || m_ResizeAction == BL_Resize)
		{
			// User was dragging the left side.
			m_rectClient.left = m_rectClient.right - m_MinWidth;
		}
		else if (m_ResizeAction == TR_Resize || m_ResizeAction == R_Resize || m_ResizeAction == BR_Resize)
		{
			m_rectClient.right = m_rectClient.left + m_MinWidth;
		}
	}
	if (m_rectClient.Height() < m_MinHeight)
	{
		if (m_ResizeAction == TL_Resize || m_ResizeAction == T_Resize || m_ResizeAction == TR_Resize)
		{
			// User was dragging the top edge.
			m_rectClient.top = m_rectClient.bottom - m_MinHeight;
		}
		else if (m_ResizeAction == BL_Resize || m_ResizeAction == B_Resize || m_ResizeAction == BR_Resize)
		{
			m_rectClient.bottom = m_rectClient.top + m_MinHeight;
		}
	}

	// Double check that we are not outside the bounds.
	// TODO - I have spent way too much time trying to track down rounding errors - I kept
	//   ending up with, for example, a subwindow width of 1281 when my bounding width is 1280.
	//   In the process, I have made just about every variable in this component a double, 
	//   instead of an int, to try to retain accuracy, but I'm still having problems. Some
	//   day, when I have the time (yeah, right!) I need to nail this down once and for all.
	//   Until then, I'm leaving in the following sanity-check:
	if (m_rectClient.left < 0)
	{
		m_rectClient.right = min(m_rectClient.Width(), m_ClientWidth);
		m_rectClient.left = 0;
	}
	if (m_rectClient.right > m_ClientWidth)
	{
		m_rectClient.left = max(m_ClientWidth - m_rectClient.Width(), 0);
		m_rectClient.right = m_ClientWidth;
	}
	if (m_rectClient.top < 0)
	{
		m_rectClient.bottom = min(m_rectClient.Height(), m_ClientHeight);
		m_rectClient.top = 0;
	}
	if (m_rectClient.bottom > m_ClientHeight)
	{
		m_rectClient.top = max(m_ClientHeight - m_rectClient.Height(), 0);
		m_rectClient.bottom = m_ClientHeight;
	}
}


// Determine which resize region (if any) of the given rectangle the given point is in.
eResizeAction 
CPxLSubwindow::GetResizeRegion(double x, double y, DRect const& rc, int slop)
{
	if (x < rc.left - slop || x > rc.right + slop || y < rc.top - slop || y > rc.bottom + slop)
		return No_Resize;

	// The point is somewhere in the rectangle. Determine the horizontal
	// region (left=1, middle=2, or right=3), and the vertical region (top=1,
	//  middle=2, or bottom=3) that it is in.
	int hReg = abs(x - rc.left) <= slop ? 1 : (abs(x - rc.right) <= slop ? 3 : 2);
	int vReg = abs(y - rc.top) <= slop ? 1 : (abs(y - rc.bottom) <= slop ? 3 : 2);

	if (hReg == 1)
	{
		if (vReg == 1)
			return TL_Resize;
		if (vReg == 3)
			return BL_Resize;
		return L_Resize;
	}
	if (hReg == 3)
	{
		if (vReg == 1)
			return TR_Resize;
		if (vReg == 3)
			return BR_Resize;
		return R_Resize;
	}
	if (vReg == 1)
		return T_Resize;
	if (vReg == 3)
		return B_Resize;

	return Move_NoResize;
}


void CPxLSubwindow::FireSubwindowChanged(void)
{
	Fire_SubwindowChanged(roundTo<long>(m_rectClient.left), 
						  roundTo<long>(m_rectClient.top), 
						  roundTo<long>(m_rectClient.Width()), 
						  roundTo<long>(m_rectClient.Height()));
}


// Setting bHandled to true is supposed to reduce flicker. I don't think it
// works, but I guess it doesn't hurt.
LRESULT CPxLSubwindow::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = TRUE;
	return 0;
}


LRESULT CPxLSubwindow::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_bRecalcOuter = true;
	FireViewChange();
	return 0;
}



STDMETHODIMP CPxLSubwindow::get_AmbientColor(OLE_COLOR* pVal)
{
	*pVal = m_clrAmbientColor;
	return S_OK;
}

STDMETHODIMP CPxLSubwindow::put_AmbientColor(OLE_COLOR newVal)
{
	m_clrAmbientColor = newVal;
	return S_OK;
}


/******************************************************************************
* CPxLSubwindow - Methods exposed by the IPxLSubwindow COM interface.
******************************************************************************/

/**
* Function: SetBounds
* Purpose:  Set the maximum height and width, in client coordinates. These
*           are the maximum values that the client will ever be able to set
*           the subwindow to, by any method (dragging, stretching, etc.).
*/
STDMETHODIMP CPxLSubwindow::SetBounds(LONG width, LONG height)
{
	m_ClientWidth = max(width, 0);
	m_ClientHeight = max(height, 0);
	
	m_MinWidth = min(m_MinWidth, m_ClientWidth);
	m_MinHeight = min(m_MinHeight, m_ClientHeight);

	m_bRecalcOuter = true;
	this->GetParent().Invalidate();
	FireViewChange();
	FireSubwindowChanged();

	return S_OK;
}


/**
* Function: 
* Purpose:  
*/
STDMETHODIMP CPxLSubwindow::GetBounds(LONG* width, LONG* height)
{
	*width = m_ClientWidth;
	*height = m_ClientHeight;

	return S_OK;
}


/**
* Function: 
* Purpose:  
*/
STDMETHODIMP CPxLSubwindow::GetSubwindow(LONG* left, LONG* top, LONG* width, LONG* height)
{
	*left = roundTo<long>(m_rectClient.left);
	*top = roundTo<long>(m_rectClient.top);
	*width = roundTo<long>(m_rectClient.Width());
	*height = roundTo<long>(m_rectClient.Height());

	return S_OK;
}


/**
* Function: 
* Purpose:  
*/
STDMETHODIMP CPxLSubwindow::SetSubwindow(LONG left, LONG top, LONG width, LONG height)
{
	m_rectClient.left = min(max(left, 0), m_ClientWidth - m_MinWidth);
	m_rectClient.top = min(max(top, 0), m_ClientHeight - m_MinHeight);
	m_rectClient.right = min(m_rectClient.left + max(width, m_MinWidth), m_ClientWidth);
	m_rectClient.bottom = min(m_rectClient.top + max(height, m_MinHeight), m_ClientHeight);

	RecalcInnerRect();      // Set the size of m_rectInner, for proper drawing.
	FireViewChange();		// Notify the control that it needs to redraw itself.
	FireSubwindowChanged(); // Notify the container of the new subwindow size.

	return S_OK;
}


/**
* Function: 
* Purpose:  
*/
STDMETHODIMP CPxLSubwindow::SetSize(LONG width, LONG height)
{
	width = min(max(width, m_MinWidth), m_ClientWidth);
	height = min(max(height, m_MinHeight), m_ClientHeight);

	// Leave the left and top as-is, if possible. Move them only if we have to.
	m_rectClient.left = min(m_rectClient.left, m_ClientWidth - width);
	m_rectClient.top = min(m_rectClient.top, m_ClientHeight - height);

	m_rectClient.right = m_rectClient.left + width;
	m_rectClient.bottom = m_rectClient.top + height;

	RecalcInnerRect();      // Set the size of m_rectInner, for proper drawing.
	FireViewChange();
	FireSubwindowChanged(); 

	return S_OK;
}


/**
* Function: 
* Purpose:  
*/
STDMETHODIMP CPxLSubwindow::SetWidth(LONG width)
{
	width = min(max(width, m_MinWidth), m_ClientWidth);

	// Leave the left as-is, if possible. Move it only if we have to.
	m_rectClient.left = min(m_rectClient.left, m_ClientWidth - width);
	m_rectClient.right = m_rectClient.left + width;

	RecalcInnerRect();      // Set the size of m_rectInner, for proper drawing.
	FireViewChange();
	FireSubwindowChanged(); 

	return S_OK;
}


/**
* Function: 
* Purpose:  
*/
STDMETHODIMP CPxLSubwindow::SetHeight(LONG height)
{
	height = min(max(height, m_MinHeight), m_ClientHeight);

	// Leave the top as-is, if possible. Move it only if we have to.
	m_rectClient.top = min(m_rectClient.top, m_ClientHeight - height);
	m_rectClient.bottom = m_rectClient.top + height;

	RecalcInnerRect();      // Set the size of m_rectInner, for proper drawing.
	FireViewChange();
	FireSubwindowChanged(); 

	return S_OK;
}


/**
* Function: 
* Purpose:  
*/
STDMETHODIMP CPxLSubwindow::SetLeft(LONG left)
{
	double width = m_rectClient.Width();
	m_rectClient.left = min(max(left, 0), m_ClientWidth - m_MinWidth);
	m_rectClient.right = min(m_rectClient.left + width, m_ClientWidth);

	RecalcInnerRect();      // Set the size of m_rectInner, for proper drawing.
	FireViewChange();		// Notify the control that it needs to redraw itself.
	FireSubwindowChanged(); // Notify the container of the new subwindow size.

	return S_OK;
}


/**
* Function: 
* Purpose:  
*/
STDMETHODIMP CPxLSubwindow::SetRight(LONG right)
{
	double width = m_rectClient.Width();
	m_rectClient.right = min(max(right, m_MinWidth), m_ClientWidth);
	m_rectClient.left = max(m_rectClient.right - width, 0);

	RecalcInnerRect();      // Set the size of m_rectInner, for proper drawing.
	FireViewChange();		// Notify the control that it needs to redraw itself.
	FireSubwindowChanged(); // Notify the container of the new subwindow size.

	return S_OK;
}


/**
* Function: 
* Purpose:  
*/
STDMETHODIMP CPxLSubwindow::SetTop(LONG top)
{
	double height = m_rectClient.Height();
	m_rectClient.top = min(max(top, 0), m_ClientHeight - m_MinHeight);
	m_rectClient.bottom = min(m_rectClient.top + height, m_ClientHeight);

	RecalcInnerRect();      // Set the size of m_rectInner, for proper drawing.
	FireViewChange();		// Notify the control that it needs to redraw itself.
	FireSubwindowChanged(); // Notify the container of the new subwindow size.

	return S_OK;
}


/**
* Function: 
* Purpose:  
*/
STDMETHODIMP CPxLSubwindow::SetBottom(LONG bottom)
{
	double height = m_rectClient.Height();
	m_rectClient.bottom = min(max(bottom, m_MinHeight), m_ClientHeight);
	m_rectClient.left = max(m_rectClient.bottom - height, 0);

	RecalcInnerRect();      // Set the size of m_rectInner, for proper drawing.
	FireViewChange();		// Notify the control that it needs to redraw itself.
	FireSubwindowChanged(); // Notify the container of the new subwindow size.

	return S_OK;
}


/**
* Function: 
* Purpose:  
*/
STDMETHODIMP CPxLSubwindow::get_MinWidth(LONG* pVal)
{
	*pVal = m_MinWidth;
	return S_OK;
}


/**
* Function: 
* Purpose:  
*/
STDMETHODIMP CPxLSubwindow::put_MinWidth(LONG newVal)
{
	m_MinWidth = min(max(newVal, 0), m_ClientWidth);
	if (m_rectClient.Width() < m_MinWidth)
	{
		this->SetWidth(m_MinWidth);
	}
	return S_OK;
}


/**
* Function: 
* Purpose:  
*/
STDMETHODIMP CPxLSubwindow::get_MinHeight(LONG* pVal)
{
	*pVal = m_MinHeight;
	return S_OK;
}


/**
* Function: 
* Purpose:  
*/
STDMETHODIMP CPxLSubwindow::put_MinHeight(LONG newVal)
{
	m_MinHeight = min(max(newVal, 0), m_ClientHeight);
	if (m_rectClient.Height() < m_MinHeight)
	{
		this->SetHeight(m_MinHeight);
	}
	return S_OK;
}


