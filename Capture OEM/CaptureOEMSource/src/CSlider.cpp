// CSlider.cpp  : Definition of ActiveX Control wrapper class(es) created by Microsoft Visual C++

/**********************************************************************************************************
 *
 *  Design Notes:
 *      Date:      Author:     Note:
 *      01-22-29   PEC         - Changed the slider control to support sliders where min=max.  Note that the
 *                               underlying Active X control (supplied by MS) does not support min=max.  Rather
 *                               it requires min<max.  Consequently, if a client of this module attempted to set
 *                               min<max, the request would be ignored, and the slider would continue to use the 
 *                               old range.
 *                              
 *                               To add the the support for min=max, we simply 'fake' the control.  More precisely, 
 *                               we don't actually change the underlying slider range values (they stay at the old
 *                               values), however, we remove the min and max value displayed on the control, set the 
 *                               control value to the min, and disable the control.  Once the client set a range 
 *                               again where min<max, the control returns to its original behavior.
 *
 *************************************************************************************************************/
 


#include "stdafx.h"
#include "PxLDevApp.h"
#include "DevAppPage.h"
#include "CSlider.h"
#include <math.h>
#include <algorithm>


/////////////////////////////////////////////////////////////////////////////
// CSlider

IMPLEMENT_DYNCREATE(CSlider, CWnd)


/**
* Function: SetLinkedWindows
* Purpose:  
*/
void CSlider::SetLinkedWindows(CWnd* valueDisplay, 
							   CWnd* minValDisplay, 
							   CWnd* maxValDisplay,
							   CWnd* titleDisplay)
{
	m_pValueDisplay = valueDisplay;
	m_pMinValDisplay = minValDisplay;
	m_pMaxValDisplay = maxValDisplay;
	m_pTitleDisplay = titleDisplay;
}


/**
* Function: EnableLinkedWindows
* Purpose:  
*/
void CSlider::EnableLinkedWindows(BOOL enable)
{
	if (m_pMinValDisplay != NULL)
		m_pMinValDisplay->EnableWindow(enable);
	if (m_pMaxValDisplay != NULL)
		m_pMaxValDisplay->EnableWindow(enable);
}


void CSlider::SetMappedMode(int nValues, float const* values)
{
	m_mappedValues.resize(nValues, 0.0f);
	for (int i = 0; i < nValues; i++)
		m_mappedValues[i] = values[i];
	m_bMappedMode = (nValues != 0);
}


void CSlider::SetIntegerMode(bool intMode /*=true*/)
{
	m_bIntMode = intMode;
}


/**
* Function: SetRange
* Purpose:  Set the Min and Max values for this Slider.
*           Because of the way the Windows slider control is implemented, we
*           can only set the Min, the Max, and the Value to integral values.
*           However, we often need a slider that ranges from 0.00 to 1.00 in 
*           intervals of, say, 0.01. To achieve this, we multiply the internal
*           values by a power of 10 to get a reasonable range of available
*           values. For example, to get a range of 0.00 to 1.00, we set the
*           range to be 0 to 100, but display the Min, the Max, and the Value
*           as a decimal number - the real, internal value divided by 100, in 
*           this case.
*/
void CSlider::SetRange(float minVal, float maxVal)
{
	CString s;

	if (m_bMappedMode)
	{
		put_Min(0);
		put_Max(m_mappedValues.size() - 1);
		put_TickFrequency(1);
		put_LargeChange(1);

		if (m_pMinValDisplay != NULL)
		{
			s.Format(_T("%g"), m_mappedValues[0]);
			m_pMinValDisplay->SetWindowText(s);
		}
		if (m_pMaxValDisplay != NULL)
		{
			s.Format(_T("%g"), m_mappedValues.back());
			m_pMaxValDisplay->SetWindowText(s);
		}

		return; // In mapped mode, the range is fixed, and defined by the number of mapped values.
	}


	if (maxVal < minVal)
		return;
		
	if (maxVal == minVal)
	{
	    //
	    // Special case this.  See Design Note at the top of this module
	    //
	    
	    m_bMinEqualsMax = true;
	    
    	m_actualMin = minVal;
	    m_actualMax = maxVal;
	    // Move the slider to its min value
		SetValue(GetMin());
		
		// Display the range values
        s.Format(_T("%g"), minVal);

	    if (m_pMinValDisplay != NULL)
	    {
		    m_pMinValDisplay->SetWindowText(s);
	    }

	    if (m_pMaxValDisplay != NULL)
	    {
		    m_pMaxValDisplay->SetWindowText(s);
	    }

	    // Now, disable the slider so the user can't touch it
	    EnableWindow(false);
	    
	    return;
	}
	    
	// Enable the slider - if it was disabled by the maxVal == minVal special case above
	if (m_bMinEqualsMax)
	{
        EnableWindow(true);
    }
    m_bMinEqualsMax = false;
	
	m_actualMin = minVal;
	m_actualMax = maxVal;

	if (minVal < 0)
	{
		// Slider control doesn't deal well with negatives. Shift the range.
		m_offset = -minVal;
		minVal = 0.0f;
		maxVal += m_offset;
	}
	else
		m_offset = 0.0f;

	// Before changing the range, cache the current value
	float curVal = GetValue();

	// Make sure that we have a reasonable range of available values.
	m_exponent = 0;
	long tick = 1;
	if (!m_bIntMode)
	{
		while (maxVal - minVal < 50)
		{
			maxVal *= 10;
			minVal *= 10;
			m_exponent += 1;
		}
		while (maxVal - minVal > 500)
		{
			maxVal /= 10;
			minVal /= 10;
			m_exponent -= 1;
		}

		tick = static_cast<long>((maxVal-minVal)/100 + 0.5) * 5;
	}

	// Temporarily disable the ticks in case we have
	// to make a significant change in range.
	// See Bugzilla Issue 80
	put_TickFrequency(0);
	put_LargeChange(0);

	if (get_Max() <= minVal) {
		put_Max(static_cast<long>(1 + max(get_Min(), minVal)));
	}
	put_Min(static_cast<long>(minVal));
	put_Max(static_cast<long>(maxVal + 0.5f));

	// Now specify the tick frequencies
	put_TickFrequency(tick);
	put_LargeChange(tick);

	
	// Keep the current value only if it is still in range.
	if (curVal < GetMin())
		SetValue(GetMin());
	else if (curVal > GetMax())
		SetValue(GetMax());
	else
		SetValue(curVal);

	// Display the labels for the min/max range of the slider.
	if (m_pMinValDisplay != NULL)
	{
		s.Format(_T("%g"), m_actualMin);
		m_pMinValDisplay->SetWindowText(s);
	}

	if (m_pMaxValDisplay != NULL)
	{
		s.Format(_T("%g"), m_actualMax);
		m_pMaxValDisplay->SetWindowText(s);
	}

}


/**
* Function: OnScroll
* Purpose:  
*/
void CSlider::OnScroll()
{
	if (m_bMappedMode)
		m_actualValue = m_mappedValues.at(get_Value());
	else
		m_actualValue = min(m_actualMax, max(m_actualMin, static_cast<float>(get_Value() * pow(10.0,-m_exponent) - m_offset)));
	UpdateValueDisplay();
}


/**
* Function: OnChange
* Purpose:  
*/
void CSlider::OnChange()
{
	if (m_bMappedMode)
		m_actualValue = m_mappedValues.at(get_Value());
	else
		m_actualValue = min(m_actualMax, max(m_actualMin, static_cast<float>(get_Value() * pow(10.0,-m_exponent) - m_offset)));
	UpdateValueDisplay();
}


/**
* Function: GetValue
* Purpose:  
*/
float CSlider::GetValue(void)
{
	return m_actualValue;
}


/**
* Function: SetValue
* Purpose:  
*/
void CSlider::SetValue(float newVal)
{
	if (m_bMappedMode)
	{
		// Find the value in the map
		float mindif = 99999.0f;
		int idx = 0;
		for (int i = 0; i < m_mappedValues.size(); i++)
		{
			if (abs(m_mappedValues[i] - newVal) < mindif)
			{
				mindif = fabs(m_mappedValues[i] - newVal);
				idx = i;
			}
		}
		m_actualValue = m_mappedValues[idx];
		put_Value(idx);
	}
	else
	{
		m_actualValue = min(m_actualMax, max(m_actualMin, newVal));
		put_Value(static_cast<long>((m_actualValue + m_offset) * pow(10.0, m_exponent) + 0.5));
	}
	UpdateValueDisplay();
}


/**
* Function: GetMin
* Purpose:  
*/
float CSlider::GetMin(void)
{
	if (m_bMappedMode)
		return m_mappedValues[0];

	return static_cast<float>(max(m_actualMin, get_Min() * pow(10.0,-m_exponent) - m_offset));
}


/**
* Function: GetMax
* Purpose:  
*/
float CSlider::GetMax(void)
{
	if (m_bMappedMode)
		return m_mappedValues.back();

	return static_cast<float>(min(m_actualMax, get_Max() * pow(10.0,-m_exponent) - m_offset));
}


/**
* Function: UpdateFromWnd
* Purpose:  Extract the numeric value from the associated "Value Display" 
*           window (usually an edit box), and set the value of the slider to
*           that value.
*/
void CSlider::UpdateFromWnd(void)
{
	if (m_pValueDisplay == NULL)
		return;

	CString s;
	m_pValueDisplay->GetWindowText(s);
	float val;
	if (s.IsEmpty() || !_stscanf_s(s, _T("%f"), &val, sizeof(val)))
	{
		// invalid number - reset the window
		UpdateValueDisplay();
		return;
	}

	if (val > GetMax())
		val = GetMax();
	if (val < GetMin())
		val = GetMin();

	SetValue(val);

}


/**
* Function: UpdateValueDisplay
* Purpose:  
*/
void CSlider::UpdateValueDisplay(void)
{
	float val = GetValue();
	if (m_pValueDisplay != NULL)
	{
		CString s;
		s.Format(_T("%g"), val);
		m_pValueDisplay->SetWindowText(s);
	}
}


BEGIN_MESSAGE_MAP(CSlider, CWnd)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()


BOOL CSlider::OnHelpInfo(HELPINFO* pHelpInfo)
{
	CDevAppPage* pPage = static_cast<CDevAppPage*>(this->GetParent());
	if (pPage->IsKindOf(RUNTIME_CLASS(CDevAppPage)))
		return pPage->OnHelpInfo(pHelpInfo);

	return FALSE;
}
