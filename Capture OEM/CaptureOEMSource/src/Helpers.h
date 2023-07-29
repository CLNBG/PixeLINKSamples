#pragma once


#include "PxLCamera.h"


/**
* Class:   CPreviewState
* Purpose: Declare one of theses on the stack to temporarily change the state
*          of the video preview within the scope of a code block.
*/
class CPreviewState
{
public:
	CPreviewState(CPxLCamera* pCam, U32 newState)
		: m_pCam(pCam)
	{
		m_oldState = pCam->GetPreviewState();
		if (newState != m_oldState)
		{
			m_pCam->SetPreviewState(newState);
		}
	}
	~CPreviewState()
	{
		if (m_pCam->GetPreviewState() != m_oldState)
		{
			try { m_pCam->SetPreviewState(m_oldState); }
			catch (PxLException const&) {}
		}
	}
private:
	CPxLCamera*		m_pCam;
	U32				m_oldState;
};


/**
* Class:   CStreamState
* Purpose: Declare one of these on the stack to temporarily change the state
*          of the video stream within the scope of a code block.
*/
class CStreamState
{
public:
	CStreamState(CPxLCamera* pCam, U32 newState, bool forceChange = false)
		: m_pCam(pCam)
		, m_PreviewStatePtr(NULL)
	{
		m_oldState = pCam->GetStreamState();
		if (newState != m_oldState)
		{
			if (newState != START_STREAM 
				&& pCam->GetPreviewState() == START_PREVIEW)
			{
				// Preview is running - pause it while the stream is stopped or
				// paused. This way we get a freeze-frame, rather than the
				// "Input unavailable" message.
				//m_PreviewStatePtr.reset(new CPreviewState(pCam, PAUSE_PREVIEW)); // vc7
				m_PreviewStatePtr = std::auto_ptr<CPreviewState>(new CPreviewState(pCam, PAUSE_PREVIEW)); // vc6
			}
			m_pCam->SetStreamState(newState, forceChange);
		}
	}
	~CStreamState()
	{
		if (m_pCam->GetStreamState() != m_oldState)
		{
			try { 
				m_pCam->SetStreamState(m_oldState); 
			} catch (PxLException const&) {
				// We have to sink this exception because destructors musn't throw
				TRACE("Caught an exception while trying to restore the stream state\n");
			}
		}
	}
private:
	CPxLCamera*						m_pCam;
	U32								m_oldState;
	std::auto_ptr<CPreviewState>	m_PreviewStatePtr;
};


/**
* Class:   CTempFullFrame
* Purpose: Declare one of these on the stack to temporarily change the ROI
*          to its maximum size within the scope of a code block.
*/
class CTempFullFrame
{
public:
	CTempFullFrame(CPxLCamera* pCam, bool fullRezToo=false)
		: m_pCam(pCam)
		, m_bRestore(false)
	{
		m_oldRoi = m_pCam->GetFeatureRect(FEATURE_ROI);
		m_oldPa  = m_pCam->GetPixelAddressing();
		int maxWidth = static_cast<int>(m_pCam->GetFeatureMaxVal(FEATURE_ROI, FEATURE_ROI_PARAM_WIDTH));
		int maxHeight = static_cast<int>(m_pCam->GetFeatureMaxVal(FEATURE_ROI, FEATURE_ROI_PARAM_HEIGHT));
		if (m_oldRoi.Width() != maxWidth || m_oldRoi.Height() != maxHeight)
		{
			m_pCam->SetFeatureRect(FEATURE_ROI, 0, 0, maxWidth, maxHeight, false);
			m_bRestore = true;
		}
		if (fullRezToo && m_oldPa.x != 1 && m_oldPa.y != 1 )
		{
			PixelAddressing newPa = m_oldPa;
			newPa.x = 1;
			newPa.y = 1;
			m_pCam->SetPixelAddressing(newPa);
			m_bRestore = true;
		}
	}
	~CTempFullFrame()
	{
		if (m_bRestore)
		{
			try { m_pCam->SetFeatureRect(FEATURE_ROI, m_oldRoi.left, m_oldRoi.top, m_oldRoi.Width(), m_oldRoi.Height(), false); }
			catch (PxLException const&) {}
			try { m_pCam->SetPixelAddressing(m_oldPa); }
			catch (PxLException const&) {}
		}
	}
	bool ChangedValue(void) { return m_bRestore; }
private:
	CPxLCamera*			m_pCam;
	CRect				m_oldRoi;
	PixelAddressing		m_oldPa;
	bool				m_bRestore;
};


/**
* Class:   CTempFullResolution
* Purpose: Declare one of these on the stack to temporarily change the Pixel addressing
*          to its maximum size (Pixel addressing off) within the scope of a code block.
*/
class CTempFullResolution
{
public:
	CTempFullResolution(CPxLCamera* pCam)
		: m_pCam(pCam)
		, m_bRestore(false)
	{
		
		m_oldPa = m_pCam->GetPixelAddressing();
		if (1 != m_oldPa.x || 1 != m_oldPa.y)
		{
		    PixelAddressing newPa = m_oldPa;
		    newPa.x = 1;
		    newPa.y = 1;
		    m_pCam->SetPixelAddressing(newPa);
			m_bRestore = true;
		}
	}
	~CTempFullResolution()
	{
		if (m_bRestore)
		{
		    try { m_pCam->SetPixelAddressing(m_oldPa); }
			catch (PxLException const&) {}
		}
	}
	bool ChangedValue(void) { return m_bRestore; }
private:
	CPxLCamera*			m_pCam;
	PixelAddressing		m_oldPa;
	bool				m_bRestore;
};


/**
* Class:   CTempFeature
* Purpose: Declare one of these on the stack to temporarily change the value
*          of a feature.
*/
class CTempFeature
{
public:
	CTempFeature(CPxLCamera* pCam, U32 featureId, float val, U32 parmNum=0)
		: m_pCam(pCam)
		, m_featureId(featureId)
		, m_paramNum(parmNum)
		, m_bRestore(false)
	{
		m_oldVal = m_pCam->GetFeatureValue(m_featureId, NULL, parmNum);
		if (m_oldVal != val)
		{
			m_pCam->SetFeatureValue(m_featureId, val, parmNum);
			m_bRestore = true;
		}
	}
	~CTempFeature()
	{
		if (m_bRestore)
		{
			try { m_pCam->SetFeatureValue(m_featureId, m_oldVal, m_paramNum); }
			catch (PxLException const&) {}
		}
	}
	bool ChangedValue(void) { return m_bRestore; }
private:
	CPxLCamera*			m_pCam;
	U32					m_featureId;
	float				m_oldVal;
	U32                 m_paramNum;
	bool				m_bRestore;
};


