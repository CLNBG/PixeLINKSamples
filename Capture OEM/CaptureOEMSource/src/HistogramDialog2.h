#pragma once


//#include "NTGraphCtrl.h"
#include <afxmt.h>
#include "afxwin.h"
#include "PxLGraphCtrl.h"


class HistogramControlPanel;

// We need to keep track of when we have not we have not yet specified a pixel format.  We will 'redefine' a
// a  pixel format, choosing one that will never be  supported in camera.
#define UNINTIALIZED_PIXEL_FORMAT PIXEL_FORMAT_RGB48_DIB


enum
{
	ALL_PIXELS,
	EVEN_COLUMN_PIXELS,
	ODD_COLUMN_PIXELS,
    // For the 'alpha' pixel formats (RGBA, BGRA, ARGB, ABGR -- we re-use the 4 Bayer pixels
	BAYER_GREEN1_PIXELS,    BAYER_FIRST_CHANNEL = BAYER_GREEN1_PIXELS,
	BAYER_RED_PIXELS,   
	BAYER_BLUE_PIXELS,      ALPHA_PIXELS = BAYER_BLUE_PIXELS, // The Blue column is reused for alpha value
	BAYER_GREEN2_PIXELS,    BAYER_LAST_CHANNEL = BAYER_GREEN2_PIXELS, 
    // The following three 'generic' channel lidentifiers are used for YUV, HSV, Stokes, RGB24 and the Alpha.  The channels
    // are mapped to the pixel components as follows:
    //     Channel Id    YUV     HSV    Stokes RGB24  Alpha
    //         0          Y       H       S0    R       R
    //         1          U       S       S1    G       G
    //         2          V       V       S2    B       B
	CHANNEL_0,
	CHANNEL_1,
	CHANNEL_2,
	PIXEL_TYPES_TOTAL
};

typedef enum _PixelChannels
{
    PIXEL_CHANNELS_NONE,   // the pixels are not channelized (they are 'raw')
    PIXEL_CHANNELS_YUV,    // A pixel has a Y, U, and V channel
    PIXEL_CHANNELS_HSV,    // Pixels have a H, S, and V channel
    PIXEL_CHANNELS_STOKES, // Pixels have S0, S1, and S2 channels
    PIXEL_CHANNELS_RGB,    // Pixels have Red, Green, and Blue component
    PIXEL_CHANNELS_ALPHA,  // Pixels have Reg, Green, Blue, and Alpha component
} PixelChannels;


struct HistElt
{
	bool		bShow;
	int			eltNo;
	COLORREF	color;
	int			lineWidth;
	std::vector<double> data;
	double		mean;
	double		stddev;

	HistElt() : bShow(false), eltNo(-1), color(RGB(0,0,0)), lineWidth(0), mean(0), stddev(0) {}
};


// CHistogramDialog2 dialog

class CHistogramDialog2 : public CDialog
{
	friend class CHistogramControlPanel;
	friend class CHistogramControlPanel_Eng;
	friend class CHistogramControlPanel_Simple;
	friend U32 __stdcall HistogramDataCallback(HANDLE,LPVOID,U32,FRAME_DESC const * ,LPVOID);

	DECLARE_DYNAMIC(CHistogramDialog2)

	DECLARE_MESSAGE_MAP()

public:
	CHistogramDialog2(CPxLCamera* pCam);   // standard constructor
	virtual ~CHistogramDialog2();
	void ForgetCamera();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnKickIdle(void);

// Dialog Data
	enum { IDD = IDD_HISTOGRAM_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void DataUpdated(ULONG uDataFormat);
	void SetDataFormat(ULONG uDataFormat);
	void SetControlPanel(void);

private:
	CPxLCamera*			m_pCam;
	CMutex*				m_pLock;

	CPxLGraphCtrl		m_Graph;
	//CRect				m_RectMargins;

	CHistogramControlPanel* m_ControlPanel;
	int m_MinHeight;
	int m_MinWidth;

	// Stuff for calculating actual frame rate:
	double				m_cameraRate;
	double				m_hostRate;
	long				m_intervalframetime;
	int					m_intervalframe;
   int                 m_numFramesRxInInterval;

	bool				m_bPaused;
	int					m_currentPixelFormat;
	bool				m_elementsChanged;
	bool				m_bAutoFormatMode; // If true, decide which plot lines to show based on pixel format.

	HistElt				m_elts[PIXEL_TYPES_TOTAL];

	bool				m_bSimpleMode;
    PixelChannels       m_PixelChannels;
	U32					m_uDataFormat;

	int					m_cumulativeMode; // 0 == None, 1 == bottom-up, 2 == top-down
	
	int                 m_maxPixelSize; // The number of bits used to represent a pixel (should be 10 or 12).

	double				m_sharpnessScore;
	double				m_maxSharpnessScore;

private:
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	void FeatureChanged(U32 featureId);

protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

private:
	HACCEL m_hAccellTable, m_hAccellTable2;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void SwitchMode();
};















