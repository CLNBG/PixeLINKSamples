// PxLHistogram.h : Declaration of the CPxLHistogram

/******************************************************************************
* Updates - April 19, 2004
* Four color channels
* Scales on X and Y axes
* 
******************************************************************************/

#pragma once


#include "resource.h"       // main symbols
#include <atlctl.h>
#include "PxLControls.h"
#include "Common.h"
#include <vector>

// CPxLHistogram
class ATL_NO_VTABLE CPxLHistogram : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CStockPropImpl<CPxLHistogram, IPxLHistogram>,
	public IPersistStreamInitImpl<CPxLHistogram>,
	public IOleControlImpl<CPxLHistogram>,
	public IOleObjectImpl<CPxLHistogram>,
	public IOleInPlaceActiveObjectImpl<CPxLHistogram>,
	public IViewObjectExImpl<CPxLHistogram>,
	public IOleInPlaceObjectWindowlessImpl<CPxLHistogram>,
	public ISupportErrorInfo,
	public IPersistStorageImpl<CPxLHistogram>,
	public ISpecifyPropertyPagesImpl<CPxLHistogram>,
	public IQuickActivateImpl<CPxLHistogram>,
	public IDataObjectImpl<CPxLHistogram>,
	public IProvideClassInfo2Impl<&CLSID_PxLHistogram, NULL, &LIBID_PxLControlsLib>,
	public CComCoClass<CPxLHistogram, &CLSID_PxLHistogram>,
	public CComControl<CPxLHistogram>
{
public:

	CPxLHistogram();


DECLARE_OLEMISC_STATUS(
	OLEMISC_RECOMPOSEONRESIZE | 
	OLEMISC_CANTLINKINSIDE | 
	OLEMISC_INSIDEOUT | 
	OLEMISC_ACTIVATEWHENVISIBLE | 
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_PXLHISTOGRAM)

BEGIN_COM_MAP(CPxLHistogram)
	COM_INTERFACE_ENTRY(IPxLHistogram)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IViewObjectEx)
	COM_INTERFACE_ENTRY(IViewObject2)
	COM_INTERFACE_ENTRY(IViewObject)
	COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceObject)
	COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY(IOleControl)
	COM_INTERFACE_ENTRY(IOleObject)
	COM_INTERFACE_ENTRY(IPersistStreamInit)
	COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
	COM_INTERFACE_ENTRY(IQuickActivate)
	COM_INTERFACE_ENTRY(IPersistStorage)
	COM_INTERFACE_ENTRY(IDataObject)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
END_COM_MAP()

BEGIN_PROP_MAP(CPxLHistogram)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	PROP_ENTRY_TYPE("BackColor", DISPID_BACKCOLOR, CLSID_StockColorPage, VT_NULL)
	PROP_ENTRY_TYPE("BorderColor", DISPID_BORDERCOLOR, CLSID_StockColorPage, VT_NULL)
	PROP_ENTRY_TYPE("BorderWidth", DISPID_BORDERWIDTH, CLSID_NULL, VT_NULL)
	PROP_ENTRY_TYPE("Enabled", DISPID_ENABLED, CLSID_NULL, VT_NULL)
	PROP_ENTRY_TYPE("ForeColor", DISPID_FORECOLOR, CLSID_StockColorPage, VT_NULL)
	// Example entries
	// PROP_ENTRY("Property Description", dispid, clsid)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()


BEGIN_MSG_MAP(CPxLHistogram)
	MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	CHAIN_MSG_MAP(CComControl<CPxLHistogram>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		static const IID* arr[] = 
		{
			&IID_IPxLHistogram,
		};

		for (int i=0; i<sizeof(arr)/sizeof(arr[0]); i++)
		{
			if (InlineIsEqualGUID(*arr[i], riid))
				return S_OK;
		}
		return S_FALSE;
	}

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

private:
	std::vector<int> m_data;
	int m_drawWidth;
	int m_maxCount;
	double m_mean;
	double m_stddev;

	HDC			m_memDC;
	XFORM		m_xform;


// IPxLHistogram
public:
	HRESULT OnDraw(ATL_DRAWINFO& di);

	OLE_COLOR m_clrBackColor;
	void OnBackColorChanged()
	{
		ATLTRACE(_T("OnBackColorChanged\n"));
	}
	OLE_COLOR m_clrBorderColor;
	void OnBorderColorChanged()
	{
		ATLTRACE(_T("OnBorderColorChanged\n"));
	}
	LONG m_nBorderWidth;
	void OnBorderWidthChanged()
	{
		ATLTRACE(_T("OnBorderWidthChanged\n"));
	}
	BOOL m_bEnabled;
	void OnEnabledChanged()
	{
		ATLTRACE(_T("OnEnabledChanged\n"));
	}
	OLE_COLOR m_clrForeColor;
	void OnForeColorChanged()
	{
		ATLTRACE(_T("OnForeColorChanged\n"));
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

	// COM Interface Methods
	STDMETHOD(SetHistogramData)(LONG nDataPoints, LONG* pData);

	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);


};

OBJECT_ENTRY_AUTO(__uuidof(PxLHistogram), CPxLHistogram)












