// PxLGraph.h : Declaration of the CPxLGraph
#pragma once

#include "resource.h"       // main symbols
#include <atlctl.h>
#include "PxLControls.h"
#include <vector>
#include <map>

// Data structure to hold information about a graph element - that is, one plot-line
// on the graph.  A PxLGraph object contains 0 or more of these.
class GraphElement
{
public:
	GraphElement();
	GraphElement(OLE_COLOR color);
	~GraphElement();

	void SetXYData(double* pXData, double*pYData, long nDataPoints);
	void SetYData(double* pData, long nDataPoints);

	void SetLineColor(OLE_COLOR color);
	void SetLineWidth(long width);
	void ResetPen(void);

	std::vector<double>		m_Ydata;
	std::vector<double>		m_Xdata;

	double					m_Xmin;
	double					m_Xmax;
	double					m_Ymin;
	double					m_Ymax;

	COLORREF				m_lineColor;
	long					m_lineWidth;
	HPEN					m_pen;

};




// CPxLGraph
class ATL_NO_VTABLE CPxLGraph : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CStockPropImpl<CPxLGraph, IPxLGraph>,
	public IPersistStreamInitImpl<CPxLGraph>,
	public IOleControlImpl<CPxLGraph>,
	public IOleObjectImpl<CPxLGraph>,
	public IOleInPlaceActiveObjectImpl<CPxLGraph>,
	public IViewObjectExImpl<CPxLGraph>,
	public IOleInPlaceObjectWindowlessImpl<CPxLGraph>,
	public CComCoClass<CPxLGraph, &CLSID_PxLGraph>,
	public CComControl<CPxLGraph>
{
public:

	CPxLGraph();

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE | 
	OLEMISC_CANTLINKINSIDE | 
	OLEMISC_INSIDEOUT | 
	OLEMISC_ACTIVATEWHENVISIBLE | 
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_PXLGRAPH)

BEGIN_COM_MAP(CPxLGraph)
	COM_INTERFACE_ENTRY(IPxLGraph)
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
END_COM_MAP()

BEGIN_PROP_MAP(CPxLGraph)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	PROP_ENTRY_TYPE("BackColor", DISPID_BACKCOLOR, CLSID_StockColorPage, VT_EMPTY)
	// Example entries
	// PROP_ENTRY("Property Description", dispid, clsid)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()


BEGIN_MSG_MAP(CPxLGraph)
	MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	MESSAGE_HANDLER(WM_SIZE, OnSize)
	MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
	CHAIN_MSG_MAP(CComControl<CPxLGraph>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// IPxLGraph
public:
	HRESULT OnDraw(ATL_DRAWINFO& di);

	OLE_COLOR m_clrBackColor;
	void OnBackColorChanged()
	{
		ATLTRACE(_T("OnBackColorChanged\n"));
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();

	//STDMETHOD(OnWindowMessage)(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);
	//BOOL ProcessWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &lResult, DWORD dwMessageMapID);


private:
	void DrawAxes(RECT const& rcBounds);
	void DrawGrid(RECT const& rcPlot);
	void UpdateExtrema(GraphElement const& elt);
	void RecalcExtrema(void);
	void RecalcScales(void);

private:
	bool		m_bResized;
	bool		m_bDrawAxes;
	bool		m_bTrackMouse;
	long		m_mouseX;
	long		m_mouseY;

	HBITMAP		m_bmp;
	HDC			m_memDC;
	HBRUSH		m_plotBrush;
	HBRUSH		m_bgBrush;
	HPEN		m_gridPen;
	HFONT		m_tickFont;
	HPEN		m_mouseTrackPen;
	HBRUSH		m_toolTipBrush;

	RECT		m_plotRect;
	HRGN		m_plotRegion;

	//std::vector<GraphElement*>		m_Elements;
	typedef std::map<long,GraphElement*> eltMap_t;
	eltMap_t	m_eltMap;

	// Keep track of the largest and smallest data values.
	double		m_xMinVal;
	double		m_xMaxVal;
	double		m_yMinVal;
	double		m_yMaxVal;

	// Axes
	double		m_xAxisMin;
	double		m_xAxisMax;
	double		m_yAxisMin;
	double		m_yAxisMax;

	bool		m_bAutoScaleX;
	bool		m_bAutoScaleY;

	// Number of grid lines to show (can be 0)
	int			m_gridX;
	int			m_gridY;

	OLE_COLOR	m_backColor; // Area outside the acual plot (eg: axes)
	OLE_COLOR	m_plotColor; // The actual plot area
	OLE_COLOR	m_gridColor;



public:
	STDMETHOD(AddElement)(LONG* elementId);
	STDMETHOD(RemoveElement)(LONG elementNumber);
	STDMETHOD(Clear)(void);
	STDMETHOD(GetElementCount)(LONG* elementCount);
	STDMETHOD(SetXRange)(DOUBLE min, DOUBLE max);
	STDMETHOD(SetYRange)(DOUBLE min, DOUBLE max);
	STDMETHOD(SetXYData)(LONG elementNumber, DOUBLE* xData, DOUBLE* yData, LONG nPoints);
	STDMETHOD(SetYData)(LONG elementNumber, DOUBLE* yData, LONG nPoints);
	STDMETHOD(SetLineColor)(LONG elementNumber, OLE_COLOR color);
	STDMETHOD(SetLineWidth)(LONG elementNumber, LONG width);
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	STDMETHOD(get_AutoScaleX)(VARIANT_BOOL* pVal);
	STDMETHOD(put_AutoScaleX)(VARIANT_BOOL newVal);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	STDMETHOD(get_TrackMouse)(VARIANT_BOOL* pVal);
	STDMETHOD(put_TrackMouse)(VARIANT_BOOL newVal);
	STDMETHOD(get_AutoScaleY)(VARIANT_BOOL* pVal);
	STDMETHOD(put_AutoScaleY)(VARIANT_BOOL newVal);
	STDMETHOD(GetXRange)(DOUBLE* pMin, DOUBLE* pMax);
	STDMETHOD(GetYRange)(DOUBLE* pMin, DOUBLE* pMax);
};

OBJECT_ENTRY_AUTO(__uuidof(PxLGraph), CPxLGraph)

