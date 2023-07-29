// PxLSubwindow.h : Declaration of the CPxLSubwindow
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "PxLControls.h"
#include "_IPxLSubwindowEvents_CP.h"
#include "Common.h"
#include "Rct.h"


template<typename T>
T roundTo(double val)
{
	return static_cast<T>(val + 0.5);
}


typedef Rct<int> IRect;
typedef Rct<double> DRect;

enum eResizeAction
{
	No_Resize,
	Move_NoResize,
	TL_Resize,
	T_Resize,
	TR_Resize,
	R_Resize,
	BR_Resize,
	B_Resize,
	BL_Resize,
	L_Resize
};

const HCURSOR cursors[] = 
{
	::LoadCursor(NULL, IDC_ARROW),
	::LoadCursor(NULL, IDC_SIZEALL),
	::LoadCursor(NULL, IDC_SIZENWSE),
	::LoadCursor(NULL, IDC_SIZENS),
	::LoadCursor(NULL, IDC_SIZENESW),
	::LoadCursor(NULL, IDC_SIZEWE),
	::LoadCursor(NULL, IDC_SIZENWSE),
	::LoadCursor(NULL, IDC_SIZENS),
	::LoadCursor(NULL, IDC_SIZENESW),
	::LoadCursor(NULL, IDC_SIZEWE)
};


// CPxLSubwindow
class ATL_NO_VTABLE CPxLSubwindow : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CStockPropImpl<CPxLSubwindow, IPxLSubwindow>,
	public IPersistStreamInitImpl<CPxLSubwindow>,
	public IOleControlImpl<CPxLSubwindow>,
	public IOleObjectImpl<CPxLSubwindow>,
	public IOleInPlaceActiveObjectImpl<CPxLSubwindow>,
	public IViewObjectExImpl<CPxLSubwindow>,
	public IOleInPlaceObjectWindowlessImpl<CPxLSubwindow>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CPxLSubwindow>,
	public CProxy_IPxLSubwindowEvents<CPxLSubwindow>, 
	public IPersistStorageImpl<CPxLSubwindow>,
	public ISpecifyPropertyPagesImpl<CPxLSubwindow>,
	public IQuickActivateImpl<CPxLSubwindow>,
	public IDataObjectImpl<CPxLSubwindow>,
	public IProvideClassInfo2Impl<&CLSID_PxLSubwindow, &__uuidof(_IPxLSubwindowEvents), &LIBID_PxLControlsLib>,
	public IPropertyNotifySinkCP<CPxLSubwindow>,
	public CComCoClass<CPxLSubwindow, &CLSID_PxLSubwindow>,
	public CComControl<CPxLSubwindow>
{
public:

	CPxLSubwindow();

DECLARE_OLEMISC_STATUS(
	OLEMISC_RECOMPOSEONRESIZE | 
	OLEMISC_CANTLINKINSIDE | 
	OLEMISC_INSIDEOUT | 
	OLEMISC_ACTIVATEWHENVISIBLE | 
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_PXLSUBWINDOW)

BEGIN_COM_MAP(CPxLSubwindow)
	COM_INTERFACE_ENTRY(IPxLSubwindow)
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
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
	COM_INTERFACE_ENTRY(IQuickActivate)
	COM_INTERFACE_ENTRY(IPersistStorage)
	COM_INTERFACE_ENTRY(IDataObject)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
END_COM_MAP()

BEGIN_PROP_MAP(CPxLSubwindow)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	PROP_ENTRY_TYPE("BackColor", DISPID_BACKCOLOR, CLSID_StockColorPage, VT_EMPTY)
	PROP_ENTRY_TYPE("BorderColor", DISPID_BORDERCOLOR, CLSID_StockColorPage, VT_EMPTY)
	PROP_ENTRY_TYPE("Enabled", DISPID_ENABLED, CLSID_NULL, VT_EMPTY)
	PROP_ENTRY_TYPE("FillColor", DISPID_FILLCOLOR, CLSID_StockColorPage, VT_EMPTY)
	PROP_ENTRY_TYPE("ForeColor", DISPID_FORECOLOR, CLSID_StockColorPage, VT_EMPTY)
	// Example entries
	// PROP_ENTRY("Property Description", dispid, clsid)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CPxLSubwindow)
	CONNECTION_POINT_ENTRY(IID_IPropertyNotifySink)
	CONNECTION_POINT_ENTRY(__uuidof(_IPxLSubwindowEvents))
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CPxLSubwindow)
	MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
	MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
	MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	MESSAGE_HANDLER(WM_SIZE, OnSize)
	CHAIN_MSG_MAP(CComControl<CPxLSubwindow>)
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
			&IID_IPxLSubwindow,
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

// IPxLSubwindow
public:
	HRESULT OnDraw(ATL_DRAWINFO& di);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	HRESULT FinalConstruct();
	void FinalRelease();

	BOOL m_bEnabled;
	void OnEnabledChanged()
	{
		ATLTRACE(_T("OnEnabledChanged\n"));
		if (m_bEnabled)
		{
			OleTranslateColor(m_clrBackColor, NULL, &m_rectOuter.fillColor);
			OleTranslateColor(m_clrForeColor, NULL, &m_rectInner.lineColor);
			OleTranslateColor(m_clrFillColor, NULL, &m_rectInner.fillColor);
		}
		else
		{
			m_ResizeAction = No_Resize;
			m_bResizing = false;
			m_rectOuter.fillColor = RGB(0xF0,0xF0,0xF0);
			m_rectInner.lineColor = RGB(0x80,0x80,0x80);
			m_rectInner.fillColor = RGB(0xD0,0xD0,0xD0);
		}
	}

	OLE_COLOR m_clrBackColor;
	void OnBackColorChanged()
	{
		ATLTRACE(_T("OnBackColorChanged\n"));
		OleTranslateColor(m_clrBackColor, NULL, &m_rectOuter.fillColor);
	}

	OLE_COLOR m_clrBorderColor;
	void OnBorderColorChanged()
	{
		ATLTRACE(_T("OnBorderColorChanged\n"));
		OleTranslateColor(m_clrBorderColor, NULL, &m_rectOuter.lineColor);
	}

	OLE_COLOR m_clrFillColor;
	void OnFillColorChanged()
	{
		ATLTRACE(_T("OnFillColorChanged\n"));
		OleTranslateColor(m_clrFillColor, NULL, &m_rectInner.fillColor);
	}

	OLE_COLOR m_clrForeColor;
	void OnForeColorChanged()
	{
		ATLTRACE(_T("OnForeColorChanged\n"));
		OleTranslateColor(m_clrForeColor, NULL, &m_rectInner.lineColor);
		m_rectMoving.lineColor = m_rectInner.lineColor;
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

protected:
	// The background of the background...
	OLE_COLOR m_clrAmbientColor;
	// Ctrl coords - inner border represents the limits of the subwindow.
	IRect m_rectOuter;
	// Ctrl coords - outer border represents the subwindow. Stored as double for accuracy.
	DRect m_rectInner;
	// Ctrl coords - the rectangle displayed during a drag operation
	DRect m_rectMoving;
	// Defines the dimensions that the outer rectangle represents in the client's
	// coordinate system. (default 1280 * 1024)
	LONG m_ClientWidth;
	LONG m_ClientHeight;
	// This is the data that the client is interested in - the size and location of the
	// inner rectangle in the client-defined coordinates.
	DRect m_rectClient;

	// If true, the Camera FOV rect and the Subwindow rect will be rendered with 
	// their real-world aspect ratio. Otherwise, they will be stretched to fit 
	// the screen size of the control.
	bool m_bMaintainAspectRatio;
	// Ratio of the size of this control, in pixels, to the size of the maximum field of view, in pixels.
	double m_ratioX; // == m_rectOuter.InnerWidth() / m_ClientWidth;
	double m_ratioY;
	// Specifies what sort of resizing we are currently doing (if any).
	eResizeAction m_ResizeAction;
	// If true, we are currently doing a move or resize operation.
	bool m_bResizing;
	// While dragging, stores the offset from the edges of the subwindow.
	double m_MouseOffsetTop;
	double m_MouseOffsetLeft;
	double m_MouseOffsetRight;
	double m_MouseOffsetBottom;
	// If true, the dimensions of the rectangles need to be recalculated
	bool m_bRecalcOuter;

	// Calculate the size and location of the outer rect to make it fit in the control's drawing
	// area, and, if m_bMaintainAspectRatio is true, to make its aspect ratio the same as the 
	// client's aspect ratio (m_ClientWidth / m_ClientHeight).
	void CalcOuterRect(RECT const& rc);
	// Determine which resize region (if any) of the given rectangle the given point is in.
	eResizeAction GetResizeRegion(double x, double y, DRect const& rc, int slop);
	// Converts the inner rect into client coordinates
	void SetClientRect(DRect const& rc);
	// Called to end a resizing operation
	void EndResize(void);
	void FireSubwindowChanged(void);

public:
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	STDMETHOD(GetBounds)(LONG* width, LONG* height);
	STDMETHOD(SetBounds)(LONG Width, LONG Height);
	STDMETHOD(GetSubwindow)(LONG* left, LONG* top, LONG* width, LONG* height);
	STDMETHOD(SetSubwindow)(LONG left, LONG top, LONG width, LONG height);
	LONG m_MinWidth;
	LONG m_MinHeight;

public:
	STDMETHOD(SetSize)(LONG width, LONG height);
	STDMETHOD(get_MinWidth)(LONG* pVal);
	STDMETHOD(put_MinWidth)(LONG newVal);
	STDMETHOD(get_MinHeight)(LONG* pVal);
	STDMETHOD(put_MinHeight)(LONG newVal);
	STDMETHOD(SetLeft)(LONG left);
	STDMETHOD(SetTop)(LONG top);
	STDMETHOD(SetBottom)(LONG bottom);
	STDMETHOD(SetRight)(LONG right);
	STDMETHOD(SetWidth)(LONG width);
	STDMETHOD(SetHeight)(LONG height);
	STDMETHOD(get_AmbientColor)(OLE_COLOR* pVal);
	STDMETHOD(put_AmbientColor)(OLE_COLOR newVal);
private:
	void RecalcInnerRect(void);
};

OBJECT_ENTRY_AUTO(__uuidof(PxLSubwindow), CPxLSubwindow)


