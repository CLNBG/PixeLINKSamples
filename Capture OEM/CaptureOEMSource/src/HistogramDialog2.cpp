// HistogramDialog2.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "HistogramDialog2.h"
#include "HistogramControlPanel.h"
#include "HistogramControlPanel_Simple.h"
#include "HistogramControlPanel_Eng.h"
#include "Helpers.h"
#include <time.h>
#include <math.h>
#include <afxpriv.h>


inline U16 SwapAndShiftU16(U16 val, int pixelSize)
{
	// Use a simple jump table to minimize calculations
	switch (pixelSize)
	{
	    case 10:
	        return ((val & 0xFF) << 2) | ((val & 0xFF00) >> 14);
	    case 12:
    	    return ((val & 0xFF) << 4) | ((val & 0xFF00) >> 12);
	    case 14:
    	    return ((val & 0xFF) << 6) | ((val & 0xFF00) >> 10);
	    case 16:
	        return ((val & 0xFF) << 8) | ((val & 0xFF00) >> 8);
	    default:
	        // Same as 10 bits
	        return ((val & 0xFF) << 2) | ((val & 0xFF00) >> 14);
	}
}

// Notes on 12-bit packing....
//   We have two different flavors of 12-bit packing:
//       1. XXXX_PACKED  AKA GigE Vision 12-bit packed.  It is packed as follows:
//             11 12 22 33 34 44 55 56 66
//       2. XXXX_PACKED_MSFIRST, AKA Pixel Fromat Naming Convention (PFNC) YYYY12g.  It is packed as follows:
//             11 22 12 33 44 34 55 66 56
inline U16 Unpack12Bit(U16 val, bool bigEndian, int pixelSize)
{
	if (bigEndian) // Big Endian has the bytes backwards (for this Little Endian machine)
   	            //    MS 8 bits              LS 4 bits
	            //  ----------------      -------------------
	    if (pixelSize == 10)
	       return ((val & 0xFF) << 2   | (val & 0x0F00) >> 10);
	    else
	       return ((val & 0xFF) << 4   | (val & 0x0F00) >> 8);
	else
	    if (pixelSize == 10)
	       return ((val & 0xFFF0) >> 6);
	    else
	       return ((val & 0xFFF0) >> 4);
}

inline U16 
Unpack12Bitg(const U8* const data, const bool even, int pixelSize)
{
	// even is refernce to an even (first) or odd (second) byte in the packed sequence
	if (even) {
	               //    MS 8 bits              LS 4 bits
	               //  ----------------      -------------------
	    if (pixelSize == 10)
	        return (((U16)data[0]) << 2   | (((U16)data[2]) & 0x000F) >> 2);
	    else
	        return (((U16)data[0]) << 4   | ((U16)data[2]) & 0x000F);
	} else {
	    if (pixelSize == 10)
	        return (((U16)data[1]) << 2   | ((U16)data[2]) >> 6);
	    else
	        return (((U16)data[1]) << 4   | ((U16)data[2]) >> 4);
	}
}

inline U16 
Unpack10Bitg(const U8* const data, const int pixel)
{
	// pixel is a value of 0..3, identifying the which of the 4 pixels in the the packing
    // sequence, to use
    switch (pixel)
    {
            //        MS 8 bits              LS 2 bits
            //  --------------------    --------------------
    case 0:
        return (((U16)data[0]) << 2) | ((U16)data[4] & 0x03);
    case 1:
        return (((U16)data[1]) << 2) | (((U16)data[4] & 0x0C) >> 2);
    case 2:
        return (((U16)data[2]) << 2) | (((U16)data[4] & 0x30) >> 4);
    case 3:
    default:
        return (((U16)data[3]) << 2) | (((U16)data[4] & 0xC0) >> 6);
    }
}

inline bool Is8BitFormat(ULONG fmt)
{
	return (fmt == PIXEL_FORMAT_BAYER8_BGGR
		|| fmt == PIXEL_FORMAT_BAYER8_GBRG
		|| fmt == PIXEL_FORMAT_BAYER8_GRBG
		|| fmt == PIXEL_FORMAT_BAYER8_RGGB
		|| fmt == PIXEL_FORMAT_MONO8
		|| fmt == PIXEL_FORMAT_RGB24
		|| fmt == PIXEL_FORMAT_RGB24_NON_DIB
		|| fmt == PIXEL_FORMAT_BGR24_NON_DIB
		|| fmt == PIXEL_FORMAT_YUV422
        || fmt == PIXEL_FORMAT_RGBA
        || fmt == PIXEL_FORMAT_BGRA
        || fmt == PIXEL_FORMAT_ARGB
        || fmt == PIXEL_FORMAT_ABGR);
}

inline bool Is12BitPackedFormat(ULONG fmt)
{
	return (fmt == PIXEL_FORMAT_BAYER12_BGGR_PACKED
		|| fmt == PIXEL_FORMAT_BAYER12_GBRG_PACKED
		|| fmt == PIXEL_FORMAT_BAYER12_GRBG_PACKED
		|| fmt == PIXEL_FORMAT_BAYER12_RGGB_PACKED
		|| fmt == PIXEL_FORMAT_MONO12_PACKED);
}

inline bool Is12BitgPackedFormat(ULONG fmt)
{
	return (fmt == PIXEL_FORMAT_BAYER12_BGGR_PACKED_MSFIRST
		|| fmt == PIXEL_FORMAT_BAYER12_GBRG_PACKED_MSFIRST
		|| fmt == PIXEL_FORMAT_BAYER12_GRBG_PACKED_MSFIRST
		|| fmt == PIXEL_FORMAT_BAYER12_RGGB_PACKED_MSFIRST
		|| fmt == PIXEL_FORMAT_MONO12_PACKED_MSFIRST);
}

inline bool Is10BitgPackedFormat(ULONG fmt)
{
	return (fmt == PIXEL_FORMAT_BAYER10_BGGR_PACKED_MSFIRST
		|| fmt == PIXEL_FORMAT_BAYER10_GBRG_PACKED_MSFIRST
		|| fmt == PIXEL_FORMAT_BAYER10_GRBG_PACKED_MSFIRST
		|| fmt == PIXEL_FORMAT_BAYER10_RGGB_PACKED_MSFIRST
		|| fmt == PIXEL_FORMAT_MONO10_PACKED_MSFIRST);
}

inline bool IsBayerFormat(ULONG fmt)
{
	return (fmt == PIXEL_FORMAT_BAYER16_BGGR
		|| fmt == PIXEL_FORMAT_BAYER16_GBRG
		|| fmt == PIXEL_FORMAT_BAYER16_GRBG
		|| fmt == PIXEL_FORMAT_BAYER16_RGGB
		|| fmt == PIXEL_FORMAT_BAYER8_BGGR
		|| fmt == PIXEL_FORMAT_BAYER8_GBRG
		|| fmt == PIXEL_FORMAT_BAYER8_GRBG
		|| fmt == PIXEL_FORMAT_BAYER8_RGGB
	    || fmt == PIXEL_FORMAT_BAYER12_BGGR_PACKED
		|| fmt == PIXEL_FORMAT_BAYER12_GBRG_PACKED
		|| fmt == PIXEL_FORMAT_BAYER12_GRBG_PACKED
		|| fmt == PIXEL_FORMAT_BAYER12_RGGB_PACKED
	    || fmt == PIXEL_FORMAT_BAYER12_BGGR_PACKED_MSFIRST
		|| fmt == PIXEL_FORMAT_BAYER12_GBRG_PACKED_MSFIRST
		|| fmt == PIXEL_FORMAT_BAYER12_GRBG_PACKED_MSFIRST
		|| fmt == PIXEL_FORMAT_BAYER12_RGGB_PACKED_MSFIRST
	    || fmt == PIXEL_FORMAT_BAYER10_BGGR_PACKED_MSFIRST
		|| fmt == PIXEL_FORMAT_BAYER10_GBRG_PACKED_MSFIRST
		|| fmt == PIXEL_FORMAT_BAYER10_GRBG_PACKED_MSFIRST
		|| fmt == PIXEL_FORMAT_BAYER10_RGGB_PACKED_MSFIRST
		);
		
}

inline bool IsAlphaFormat(ULONG fmt)
{
	return (fmt == PIXEL_FORMAT_RGBA
		|| fmt == PIXEL_FORMAT_BGRA
		|| fmt == PIXEL_FORMAT_ARGB
		|| fmt == PIXEL_FORMAT_ABGR);
}

inline bool UsesGenericChannels(ULONG fmt) // See HistogramDialog2.h
{
    return (fmt == PIXEL_FORMAT_YUV422 
        || fmt == PIXEL_FORMAT_HSV4_12
        || fmt == PIXEL_FORMAT_STOKES4_12
        || fmt == PIXEL_FORMAT_RGB24_NON_DIB
        || fmt == PIXEL_FORMAT_BGR24_NON_DIB
        || IsAlphaFormat(fmt)
        );
}

enum BayerChannel
{
	GREEN1,
	RED,
	BLUE,
	GREEN2,
};

std::pair<int,int> Green1Index(ULONG dataFormat)
{
	// Return (row#, col#) of Green1 (that's the green on the red row, and the blue column)
	switch (dataFormat)
	{
	case PIXEL_FORMAT_BAYER8_GRBG:
	case PIXEL_FORMAT_BAYER16_GRBG:
	case PIXEL_FORMAT_BAYER12_GRBG_PACKED:
	case PIXEL_FORMAT_BAYER12_GRBG_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER10_GRBG_PACKED_MSFIRST:
		return std::make_pair(0,0);
	case PIXEL_FORMAT_BAYER8_RGGB:
	case PIXEL_FORMAT_BAYER16_RGGB:
	case PIXEL_FORMAT_BAYER12_RGGB_PACKED:
	case PIXEL_FORMAT_BAYER12_RGGB_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER10_RGGB_PACKED_MSFIRST:
		return std::make_pair(0,1);
	case PIXEL_FORMAT_BAYER8_BGGR:
	case PIXEL_FORMAT_BAYER16_BGGR:
	case PIXEL_FORMAT_BAYER12_BGGR_PACKED:
	case PIXEL_FORMAT_BAYER12_BGGR_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER10_BGGR_PACKED_MSFIRST:
		return std::make_pair(1,0);
	case PIXEL_FORMAT_BAYER8_GBRG:
	case PIXEL_FORMAT_BAYER16_GBRG:
	case PIXEL_FORMAT_BAYER12_GBRG_PACKED:
	case PIXEL_FORMAT_BAYER12_GBRG_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER10_GBRG_PACKED_MSFIRST:
		return std::make_pair(1,1);
	}
	return std::make_pair(0,0); // Treat MONO as GRBG (the default for the PL-A682).
}

std::pair<int,int> GetChannelOffset(int /*BayerChannel*/ channel, ULONG dataFormat)
{
	// Return the row# and col# of the given channel, when the data is in the
	// given format.
	std::pair<int,int> channelCoords(channel/2, channel%2);
	std::pair<int,int> offset = Green1Index(dataFormat);
	return std::make_pair((channelCoords.first+offset.first)%2, (channelCoords.second+offset.second)%2);
}

//template<typename T, typename U>
//T round_to(U val)
//{
//	return static_cast<T>(val + 0.5);
//}


/**
* Function: HistogramDataCallback
* Purpose:  This function is set as the OVERLAY_FRAME callback while the
*           histogram window is open.
*/
static U32 __stdcall HistogramDataCallback(
							HANDLE hCamera,
							LPVOID pFrameData,
							U32 uDataFormat,
							FRAME_DESC const * pFrameDesc,
							LPVOID pContext)
{
	CHistogramDialog2* pDlg = static_cast<CHistogramDialog2*>(pContext);


   pDlg->m_numFramesRxInInterval++;  // Be sure we count themn all, even if we can't get the lock

    if (!::IsWindow(pDlg->GetSafeHwnd()))
		return ApiSuccess;

	if (!pDlg->m_pLock->Lock(0))
	{
		// We were not able to acquire the lock immediately.
		// That's OK - we just won't process this frame.
		return ApiSuccess;
	}
   // else:
	// We have acquired a lock on the Mutex. It is safe to access the members,
	// of the HistogramDialog object, because the main thread cannot delete it
	// until after we release the lock.
	

	// Calculate actual framerate.
	long curtime = static_cast<long>(pFrameDesc->fFrameTime * 1000);
	long elapsedtime = curtime - pDlg->m_intervalframetime;
	int elapsedframes = pFrameDesc->uFrameNumber - pDlg->m_intervalframe;

	if (elapsedtime >= 50 && elapsedframes >= 5)
	{
		// enough time and enough frames have elapsed to calculate a reasonably
		// accurate frame rate.
		pDlg->m_cameraRate = static_cast<double>(CLOCKS_PER_SEC) * elapsedframes / elapsedtime;
		pDlg->m_hostRate   = static_cast<double>(CLOCKS_PER_SEC) * pDlg->m_numFramesRxInInterval / elapsedtime;
		pDlg->m_intervalframetime = curtime;
		pDlg->m_intervalframe = pFrameDesc->uFrameNumber;
        pDlg->m_numFramesRxInInterval = 0;
	}
	// Check to see if the stream restarted (frame count and frame time started over again.).  
	// Bugzilla.165/202 - Check to see if the frameTime / frameNumber were reset because of a 
	// stream stop/start.  the most reliable way to identify this, is lookin at the frameTime, as it
	// is always reset for those types of cameras that do a reset (1394 resets both, Gige resets frameTime
	// and USB does not reset either).
	// Issue.Software.233 - Use the elapsed time too, just so we don't confuse the ocassional 
	// temporal mis-ording of frames for a restart.
	else if (elapsedframes < 0 || elapsedtime < 0) 
	{
		// Stream has been restarted. Reset our start values.
		pDlg->m_intervalframetime = curtime;
		pDlg->m_intervalframe = pFrameDesc->uFrameNumber;
        pDlg->m_numFramesRxInInterval = 0;
    }

	if (pDlg->m_bPaused || (elapsedtime < 50) || (elapsedframes < 0)) {
		// The rest of this function calculates the histogram data, and then
		// sends a message to update the GUI. Do not do this more than 20 times
		// per second - that would be a waste of processor power, since users
		// can't tell the difference between a GUI that is updating 20 times
		// per second and one that is updating 1000 times per second.
		//
		// The frame should also be ignored if the frame is older than 
		// the most recent frame we've seen. (i.e. elapsedframes < 0)

		pDlg->m_pLock->Unlock();
		return ApiSuccess;
	}

   int decX = static_cast<int>(pFrameDesc->PixelAddressingValue.fHorizontal);
	int decY = static_cast<int>(pFrameDesc->PixelAddressingValue.fVertical);
	int frameCols = DEC_SIZE(static_cast<int>(pFrameDesc->Roi.fWidth), decX);
	int frameRows = DEC_SIZE(static_cast<int>(pFrameDesc->Roi.fHeight), decY);
    // If the image is interleaved, then the width is doubled
    if (pFrameDesc->HDRInfo.uMode == FEATURE_GAIN_HDR_MODE_INTERLEAVED) frameCols *= 2;
	int nPixels = frameRows * frameCols;

   // Inform the dialog of the data format (which may have been changed).
	pDlg->SetDataFormat(uDataFormat);

   int nValues;
	if (Is8BitFormat(uDataFormat)) nValues = 0x100;
    else if (Is10BitgPackedFormat(uDataFormat)) nValues = 0x400;
	else nValues = 1 << pDlg->m_maxPixelSize;  // This will include HSV, Stokes, and Polar formats

   int i;
	for (i = 0; i < PIXEL_TYPES_TOTAL; i++)
	{
		if (pDlg->m_elts[i].data.size() != nValues)
			pDlg->m_elts[i].data.resize(nValues);
	}

    pDlg->m_PixelChannels = PIXEL_CHANNELS_NONE;

    switch(uDataFormat)
	{
    case PIXEL_FORMAT_YUV422:
    {
        pDlg->m_PixelChannels = PIXEL_CHANNELS_YUV;

		// NOTE: RGB values are only shown in simple mode
        for (i = CHANNEL_0; i <= CHANNEL_2; i++)
			std::fill(pDlg->m_elts[i].data.begin(), pDlg->m_elts[i].data.end(), 0.0);

		// Format of YUV422 is:
		//     * 2 bytes per pixel
		//     * First pixel on each line has Y and U
		//     * Second pixel on each line has Y and V
		U8* pData = static_cast<U8*>(pFrameData);

		double* pYvals = &pDlg->m_elts[CHANNEL_0].data[0];
		double* pUvals = &pDlg->m_elts[CHANNEL_1].data[0];
		double* pVvals = &pDlg->m_elts[CHANNEL_2].data[0];

		for (int y = 0; y < frameRows; y++)
		{
			U8* row = pData + (y * 2 * frameCols);
			for (int x = 0; x < (2*frameCols)-3; x+=4)
			{
				pUvals[row[x+0]] += 2;
				pYvals[row[x+1]] += 1;
				pVvals[row[x+2]] += 2;
				pYvals[row[x+3]] += 1;
			}
		}
        break;
    }
    case PIXEL_FORMAT_RGB24_NON_DIB:
    case PIXEL_FORMAT_BGR24_NON_DIB:
    {
        pDlg->m_PixelChannels = PIXEL_CHANNELS_RGB;

		U8* pData = static_cast<U8*>(pFrameData);
		int bytesPerRow = frameCols*3;
        U8 R,G,B;

		for (i = CHANNEL_0; i <= CHANNEL_2; i++)
			std::fill(pDlg->m_elts[i].data.begin(), pDlg->m_elts[i].data.end(), 0.0);

      double* pRvals = &pDlg->m_elts[CHANNEL_0].data[0];
		double* pGvals = &pDlg->m_elts[CHANNEL_1].data[0];
		double* pBvals = &pDlg->m_elts[CHANNEL_2].data[0];

		for (int y = 0; y < frameRows; y++)
		{
			for (int x = 0; x < bytesPerRow; x+=3)
			{
                R = *(pData + y*bytesPerRow + x + (uDataFormat == PIXEL_FORMAT_RGB24_NON_DIB ? 0 : 2));
				G = *(pData + y*bytesPerRow + x + 1);
				B = *(pData + y*bytesPerRow + x + (uDataFormat == PIXEL_FORMAT_RGB24_NON_DIB ? 2 : 0));
				++pRvals[R];
				++pGvals[G];
				++pBvals[B];
			}
		}
        break;
    }
    case PIXEL_FORMAT_RGB24_DIB:
    case PIXEL_FORMAT_RGB48:
    {
		// XXX - this should never happen, at the moment, because the cameras don't
		//       output these directly (and this is an OVERLAY_FRAME callback).
		//       If the cameras grow the ability to output them, we need to add code here.
		ASSERT(FALSE);
        break;
    }
    case PIXEL_FORMAT_HSV4_12:
    {
        pDlg->m_PixelChannels = PIXEL_CHANNELS_HSV;

		U8* pData = static_cast<U8*>(pFrameData);
		int bytesPerRow = frameCols*3;
        U16 H,S,V;

		for (i = CHANNEL_0; i <= CHANNEL_2; i++)
			std::fill(pDlg->m_elts[i].data.begin(), pDlg->m_elts[i].data.end(), 0.0);

        double* pHvals = &pDlg->m_elts[CHANNEL_0].data[0];
		double* pSvals = &pDlg->m_elts[CHANNEL_1].data[0];
		double* pVvals = &pDlg->m_elts[CHANNEL_2].data[0];

		// Get histogram data for the HSV channels.  It is packed as follows:
        //    even rows --> H V H V H V
        //    odd rows  --> V S V S V S
        // where each of H, S, and V are 12 bit quantities packed using the 12g algorithm
		for (int y = 0; y < frameRows*2-1; y+=2)
		{
			for (int x = 0; x < bytesPerRow-1; x+=3)
			{
			    H = Unpack12Bitg (pData + y*bytesPerRow + x, true, 12);
				V = Unpack12Bitg (pData + y*bytesPerRow + x, false, 12);
				S = Unpack12Bitg (pData + (y+1)*bytesPerRow + x, false, 12);
				++pHvals[H];
				++pSvals[S];
				++pVvals[V];
			}
		}
        break;
    }
    case PIXEL_FORMAT_STOKES4_12:
    {
        pDlg->m_PixelChannels = PIXEL_CHANNELS_STOKES;

		U8* pData = static_cast<U8*>(pFrameData);
		int bytesPerRow = frameCols*3;
        U16 S0;
        U16 S1, S2;

		for (i = CHANNEL_0; i <= CHANNEL_2; i++)
			std::fill(pDlg->m_elts[i].data.begin(), pDlg->m_elts[i].data.end(), 0.0);

        double* pS0vals = &pDlg->m_elts[CHANNEL_0].data[0];
		double* pS1vals = &pDlg->m_elts[CHANNEL_1].data[0];
		double* pS2vals = &pDlg->m_elts[CHANNEL_2].data[0];

		// Get histogram data for the Stokes channels.  It is packed as follows:
        //    even rows --> S1 S0 S1 S0
        //    odd rows  --> S0 S2 S0 S2
        // where each of S0, S1, and S2 are 12 bit quantities packed using the 12g algorithm
        // Note that S0 is unsigned, while S1 and S2 are signed.
		for (int y = 0; y < frameRows*2-1; y+=2)
		{
			for (int x = 0; x < bytesPerRow-1; x+=3)
			{
			    // S1 and S2 are actually signed.  But that's OK, as we want to 'somehow' show the bias of
                // the polarity.  So, light that is NOT very polarized, will appear on the histogram edges, while
                // light that is very polarized will drift towards the center.  Furthermore, which side of center
                // will be an indication of the bias; less than 2048 will be a large positive number, while larger than 2048
                // will be a large negative number.
                S1 = Unpack12Bitg (pData + y*bytesPerRow + x, true, 12);
				S0 = Unpack12Bitg (pData + y*bytesPerRow + x, false, 12);
				S2 = Unpack12Bitg (pData + (y+1)*bytesPerRow + x, false, 12);
				++pS0vals[S0];
				++pS1vals[S1];
				++pS2vals[S2];
			}
		}
        break;
    }
    case PIXEL_FORMAT_POLAR4_12:
    case PIXEL_FORMAT_POLAR_RAW4_12:
    {
        pDlg->m_PixelChannels = PIXEL_CHANNELS_NONE;

        U8* pData = static_cast<U8*>(pFrameData);
		int bytesPerRow = frameCols*3;
        U16 P;

		std::fill(pDlg->m_elts[0].data.begin(), pDlg->m_elts[0].data.end(), 0.0);

        double* pPvals = &pDlg->m_elts[ALL_PIXELS].data[0];

		// Get histogram data for the Polar pixel.  It is packed as follows:
        //    even rows --> P0 P0 P1 P1 P2 P2  // Yes, each pixle value is repeated.
        //    odd rows  --> P0 P0 P1 P1 P2 P2  // Yes, this entire line is the same as the preceeding line.
        // where each Px is a 12 bit quantity packed using the 12g algorithm
        // OR, it's a PolarRaw, in which case it is packed as follows
        //    even rows --> 90   45  90   45  90   45
        //    odd rows  --> 135  0   135  0   135  0
        // Is either case, we use the first sub-pixel value as our (mono) pixel value.
		for (int y = 0; y < (frameRows*2)-1; y+=2)
		{
			for (int x = 0; x < bytesPerRow-1; x+=3)
			{
			    P = Unpack12Bitg (pData + y*bytesPerRow + x, false, 12);
				++pPvals[P];
			}
		}
        break;
    }
    case PIXEL_FORMAT_RGBA:
    case PIXEL_FORMAT_BGRA:
    case PIXEL_FORMAT_ARGB:
    case PIXEL_FORMAT_ABGR:
    {
       pDlg->m_PixelChannels = PIXEL_CHANNELS_RGB; // Alpha and RGB use the same channels
       
       U8* pData = static_cast<U8*>(pFrameData);
        U8 R,G,B,A;
        U8 RPos, GPos, BPos, APos;

        if (uDataFormat == PIXEL_FORMAT_RGBA)
        {
            RPos = 0; GPos = 1; BPos = 2; APos = 3;
        } else if (uDataFormat == PIXEL_FORMAT_BGRA) {
            RPos = 2; GPos = 1; BPos = 0; APos = 3;
        } else if (uDataFormat == PIXEL_FORMAT_ARGB) {
            RPos = 1; GPos = 2; BPos = 3; APos = 0;
        } else { // ABGR
            RPos = 3; GPos = 2; BPos = 1; APos = 0;
        }

        for (i = CHANNEL_0; i <= CHANNEL_2; i++)
          std::fill(pDlg->m_elts[i].data.begin(), pDlg->m_elts[i].data.end(), 0.0);
        std::fill(pDlg->m_elts[ALPHA_PIXELS].data.begin(), pDlg->m_elts[ALPHA_PIXELS].data.end(), 0.0);

      double* pRvals = &pDlg->m_elts[CHANNEL_0].data[0];
		double* pGvals = &pDlg->m_elts[CHANNEL_1].data[0];
		double* pBvals = &pDlg->m_elts[CHANNEL_2].data[0];
		double* pAvals = &pDlg->m_elts[ALPHA_PIXELS].data[0];

      //S8 printBuffer[100] = "";
      //sprintf_s (&printBuffer[0], sizeof (printBuffer), "NUM_PIXELS:%d", nPixels);
      //OutputDebugString(printBuffer);
      for (int x=0; x < nPixels; x++)
        {
			R = *(pData + RPos);
			G = *(pData + GPos);
			B = *(pData + BPos);
			A = *(pData + APos);

			++pRvals[R];
			++pGvals[G];
			++pBvals[B];
			++pAvals[A];
            
            pData += 4;
        }
        
        break;
    }
    default:
    {
        pDlg->m_PixelChannels = PIXEL_CHANNELS_NONE;
		// BAYER (or MONO) format

		// Set up an array of four pointers to doubles, ordered according to the
		// data format. That is, we set up the pointers such that the first one
		// points to the block of histogram data for RED if we are in RGGB format,
		// BLUE if we are in BGGR format, etc.
		double* pcounts[4];
		for (i = BAYER_FIRST_CHANNEL; i <= BAYER_LAST_CHANNEL; i++)
		{
			std::fill(pDlg->m_elts[i].data.begin(), pDlg->m_elts[i].data.end(), 0.0);
			std::pair<int,int> offsetYX = GetChannelOffset(i-BAYER_FIRST_CHANNEL, uDataFormat);
			pcounts[2*offsetYX.first + offsetYX.second] = &pDlg->m_elts[i].data[0];
		}

		for (i = 0; i < 4; i++)
			for (int j = i+1; j < 4; j++)
				ASSERT(pcounts[i] != pcounts[j]);

		if (Is8BitFormat(uDataFormat))
		{
			U8* pData = static_cast<U8*>(pFrameData);

			// Get histogram data for the bayer channels
			for (int y = 0; y < frameRows-1; y+=2)
			{
				for (int x = 0; x < frameCols-1; x+=2)
				{
					++pcounts[0][pData[y*frameCols + x]];
					++pcounts[1][pData[y*frameCols + x+1]];
					++pcounts[2][pData[(y+1)*frameCols + x]];
					++pcounts[3][pData[(y+1)*frameCols + x+1]];
				}
			}
		} else if (Is12BitPackedFormat(uDataFormat)) {
			U8* pData = static_cast<U8*>(pFrameData);
			U16 channelValues[4];
			U16 packedPixel;
			int bytesPerRow = frameCols + frameCols/2;

			// Get histogram data for the bayer channels
			for (int y = 0; y < frameRows-1; y+=2)
			{
				for (int x = 0; x < bytesPerRow-1; x+=3)
				{
				    packedPixel = *(reinterpret_cast<U16*>(pData + y*bytesPerRow + x));
				    channelValues[0] = Unpack12Bit (packedPixel, true, pDlg->m_maxPixelSize);
				    packedPixel = *(reinterpret_cast<U16*>(pData + y*bytesPerRow + x+1));
				    channelValues[1] = Unpack12Bit (packedPixel, false, pDlg->m_maxPixelSize);
				    packedPixel = *(reinterpret_cast<U16*>(pData + (y+1)*bytesPerRow + x));
				    channelValues[2] = Unpack12Bit (packedPixel, true, pDlg->m_maxPixelSize);
				    packedPixel = *(reinterpret_cast<U16*>(pData + (y+1)*bytesPerRow + x+1));
				    channelValues[3] = Unpack12Bit (packedPixel, false, pDlg->m_maxPixelSize);
					++pcounts[0][channelValues[0]];
					++pcounts[1][channelValues[1]];
					++pcounts[2][channelValues[2]];
					++pcounts[3][channelValues[3]];
				}
			}
		} else if (Is12BitgPackedFormat(uDataFormat)) {
			U8* pData = static_cast<U8*>(pFrameData);
			int bytesPerRow = frameCols + frameCols/2;
			U16 channelValues[4];

			// Get histogram data for the bayer channels
			for (int y = 0; y < frameRows-1; y+=2)
			{
				for (int x = 0; x < bytesPerRow-1; x+=3)
				{
				    channelValues[0] = Unpack12Bitg (pData + y*bytesPerRow + x, true, pDlg->m_maxPixelSize);
				    channelValues[1] = Unpack12Bitg (pData + y*bytesPerRow + x, false, pDlg->m_maxPixelSize);
				    channelValues[2] = Unpack12Bitg (pData + (y+1)*bytesPerRow + x, true, pDlg->m_maxPixelSize);
				    channelValues[3] = Unpack12Bitg (pData + (y+1)*bytesPerRow + x, false, pDlg->m_maxPixelSize);
					++pcounts[0][channelValues[0]];
					++pcounts[1][channelValues[1]];
					++pcounts[2][channelValues[2]];
					++pcounts[3][channelValues[3]];
				}
			}
		} else if (Is10BitgPackedFormat(uDataFormat)) {
			U8* pData = static_cast<U8*>(pFrameData);
			int bytesPerRow = frameCols + frameCols/4;
			U16 channelValues[4];

			// Get histogram data for the bayer channels
			for (int y = 0; y < frameRows-1; y+=2)
			{
				for (int x = 0; x < bytesPerRow-1; x+=5) // 5 == the 4 pixels we just completed + the 'g' byte
				{
                    // Do 4 pixels on each of the 2 rows each time
                    channelValues[0] = Unpack10Bitg (pData + y*bytesPerRow + x, 0);
				    channelValues[1] = Unpack10Bitg (pData + y*bytesPerRow + x, 1);
				    channelValues[2] = Unpack10Bitg (pData + (y+1)*bytesPerRow + x, 0);
				    channelValues[3] = Unpack10Bitg (pData + (y+1)*bytesPerRow + x, 1);
					++pcounts[0][channelValues[0]];
					++pcounts[1][channelValues[1]];
					++pcounts[2][channelValues[2]];
					++pcounts[3][channelValues[3]];
                    channelValues[0] = Unpack10Bitg (pData + y*bytesPerRow + x, 2);
				    channelValues[1] = Unpack10Bitg (pData + y*bytesPerRow + x, 3);
				    channelValues[2] = Unpack10Bitg (pData + (y+1)*bytesPerRow + x, 2);
				    channelValues[3] = Unpack10Bitg (pData + (y+1)*bytesPerRow + x, 3);
					++pcounts[0][channelValues[0]];
					++pcounts[1][channelValues[1]];
					++pcounts[2][channelValues[2]];
					++pcounts[3][channelValues[3]];
				}
			}
		} else { // 16 Bit data format
			U16* pData = static_cast<U16*>(pFrameData);

			// Get histogram data for the bayer channels
			for (int y = 0; y < frameRows-1; y+=2)
			{
				for (int x = 0; x < frameCols-1; x+=2)
				{
					++pcounts[0][SwapAndShiftU16(pData[y*frameCols + x],pDlg->m_maxPixelSize)];
					++pcounts[1][SwapAndShiftU16(pData[y*frameCols + x+1],pDlg->m_maxPixelSize)];
					++pcounts[2][SwapAndShiftU16(pData[(y+1)*frameCols + x],pDlg->m_maxPixelSize)];
					++pcounts[3][SwapAndShiftU16(pData[(y+1)*frameCols + x+1],pDlg->m_maxPixelSize)];
				}
			}
		}

		// Use the four arrays of Bayer totals to update the EvenCol, OddCol, and All totals.
		for (i = 0; i < nValues; i++)
		{
			pDlg->m_elts[EVEN_COLUMN_PIXELS].data[i] = pcounts[0][i] + pcounts[2][i];
			pDlg->m_elts[ODD_COLUMN_PIXELS].data[i] = pcounts[1][i] + pcounts[3][i];
			pDlg->m_elts[ALL_PIXELS].data[i] = pcounts[0][i] + pcounts[1][i] + pcounts[2][i] + pcounts[3][i];
		}

		// Create another array of four double-pointers that point to the 4 bayer
		// data blocks, but this time put them in the order GRBG.
		double *pcountsGRBG[4];
		for (i = GREEN1; i <= GREEN2; i++)
		{
			std::pair<int,int> offsetYX = GetChannelOffset(i, uDataFormat);
			pcountsGRBG[i] = pcounts[2*offsetYX.first + offsetYX.second];
		}

    }}

	if (pDlg->m_cumulativeMode == 1)
	{
		// Bottom-up mode
		for (i = 0; i < PIXEL_TYPES_TOTAL; i++)
		{
			for (int j = 1; j < pDlg->m_elts[i].data.size(); j++)
				pDlg->m_elts[i].data[j] += pDlg->m_elts[i].data[j-1];
		}
	}
	else if (pDlg->m_cumulativeMode == 2)
	{
		// Top-Down mode
		for (i = 0; i < PIXEL_TYPES_TOTAL; i++)
		{
			for (int j = pDlg->m_elts[i].data.size() - 2; j >= 0; --j)
				pDlg->m_elts[i].data[j] += pDlg->m_elts[i].data[j+1];
		}
	}

	pDlg->DataUpdated(uDataFormat);
	
	pDlg->m_sharpnessScore = pFrameDesc->SharpnessScore.fValue;

	pDlg->m_pLock->Unlock();

	::PostMessage(pDlg->m_hWnd, WM_APP_HISTOGRAM_DATA, (WPARAM)uDataFormat, 0);

	return ApiSuccess;
}


// CHistogramDialog2 dialog

IMPLEMENT_DYNAMIC(CHistogramDialog2, CDialog)


// Used when resizing the dialog.
static int margin_size = 7;


/**
* Function: CHistogramDialog2
* Purpose:  Constructor
*/
CHistogramDialog2::CHistogramDialog2(CPxLCamera* pCam)
	: CDialog(CHistogramDialog2::IDD)
	, m_ControlPanel(NULL)
	, m_pCam(pCam)
	, m_pLock(new CMutex)
	, m_intervalframetime(0x7FFFFFFFL) // Bugzilla.202
	, m_cameraRate(0.0)
	, m_hostRate(0.0)
	, m_intervalframe(0)
    , m_numFramesRxInInterval(0)
	, m_bPaused(false)
	, m_currentPixelFormat(PIXEL_FORMAT_MONO8)
	, m_elementsChanged(false)
	, m_bAutoFormatMode(true)
	, m_bSimpleMode(false)
    , m_PixelChannels(PIXEL_CHANNELS_NONE)
    , m_uDataFormat(UNINTIALIZED_PIXEL_FORMAT)
	, m_cumulativeMode(0)
{
	m_pCam->m_pHistogramWindow = this;
}


/**
* Function: ~CHistogramDialog2
* Purpose:  Destructor
*/
CHistogramDialog2::~CHistogramDialog2()
{
	delete m_ControlPanel;
	::DestroyAcceleratorTable(m_hAccellTable);
	::DestroyAcceleratorTable(m_hAccellTable2);
}


/**
* Function: DoDataExchange
* Purpose:  
*/
void CHistogramDialog2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PXLGRAPH1, m_Graph);
}


BEGIN_MESSAGE_MAP(CHistogramDialog2, CDialog)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_MESSAGE_VOID(WM_KICKIDLE, OnKickIdle)
END_MESSAGE_MAP()



// CHistogramDialog2 message handlers

/**
* Function: OnInitDialog
* Purpose:  
*/
BOOL CHistogramDialog2::OnInitDialog()
{
	CDialog::OnInitDialog();
	this->SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);


	this->SetControlPanel();

	m_Graph.Clear(); // Remove the default (design-time) element from the graph

	m_elts[ALL_PIXELS].color			= RGB(0,    0,    0);
	m_elts[EVEN_COLUMN_PIXELS].color	= RGB(0xFF, 0,    0xFF);
	m_elts[ODD_COLUMN_PIXELS].color		= RGB(0,    0xFF, 0xFF);
	m_elts[BAYER_GREEN1_PIXELS].color	= RGB(0,    0xFF, 0);
	m_elts[BAYER_RED_PIXELS].color		= RGB(0xFF, 0,    0);
	m_elts[BAYER_BLUE_PIXELS].color		= RGB(0,    0,    0xFF);
	m_elts[BAYER_GREEN2_PIXELS].color	= RGB(0,    0x80, 0);

	m_elts[CHANNEL_0].color		= RGB(0xFF,  0,    0);
	m_elts[CHANNEL_1].color		= RGB(0,     0xFF, 0);
	m_elts[CHANNEL_2].color		= RGB(0,     0,    0xFF);

	try
	{
		m_pCam->SetCallback(OVERLAY_FRAME, this, HistogramDataCallback);
		m_pCam->SetStreamState(START_STREAM);
	}
	catch (PxLException const& e)
	{
		m_pCam->OnApiError(e);
		return FALSE;
	}

	// Set the dialog's title bar to show which camera it is connected to.
	CString s;
	s.Format(_T("Histogram - s/n %lu"), m_pCam->GetSerialNumber());
	this->SetWindowText(s);

	m_hAccellTable = LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));

	ACCEL accel;
	accel.cmd = 1492;
	accel.fVirt = FSHIFT | FCONTROL | FVIRTKEY;
	accel.key = VK_F9;
	m_hAccellTable2 = ::CreateAcceleratorTable(&accel, 1);

    // Determine the max pixel size, using the old value as the default
    m_maxPixelSize = 10;
    if (m_pCam->FeatureSupported(FEATURE_MAX_PIXEL_SIZE))        
        m_maxPixelSize = static_cast<int>(m_pCam->GetFeatureValue(FEATURE_MAX_PIXEL_SIZE));
        
    // Determine the max SharpnessScore.  Set this to 0 if not eupported/enabled.
    m_maxSharpnessScore = 0.0;
    if (m_pCam->FeatureSupported(FEATURE_SHARPNESS_SCORE))
        m_maxSharpnessScore = m_pCam->GetFeatureMaxVal(FEATURE_SHARPNESS_SCORE, FEATURE_SHARPNESS_SCORE_MAX_VALUE);
        
	return TRUE;
}


void CHistogramDialog2::SetControlPanel(void)
{
	if (m_ControlPanel != NULL)
	{
		delete m_ControlPanel;
		m_ControlPanel = NULL;
	}

	if (m_bSimpleMode)
	{
		m_ControlPanel = new CHistogramControlPanel_Simple(this);
		m_ControlPanel->Create(IDD_HISTOGRAM_CONTROL_PANEL_SIMPLE, this);
	}
	else
	{
		m_ControlPanel = new CHistogramControlPanel_Eng(this);
		m_ControlPanel->Create(IDD_HISTOGRAM_CONTROL_PANEL, this);
	}

	m_ControlPanel->ModifyStyleEx(0, WS_CHILD|WS_VISIBLE);
	m_ControlPanel->ShowWindow(SW_SHOW);

	CRect rcCtrlPanel, rcGraph;
	this->m_Graph.GetWindowRect(&rcGraph);
	this->ScreenToClient(&rcGraph);
	this->m_ControlPanel->GetWindowRect(&rcCtrlPanel);
	this->ScreenToClient(&rcCtrlPanel);

	margin_size = rcGraph.left;

	int graph_min_height = 250;
	m_MinWidth = rcCtrlPanel.Width() + margin_size;
	m_MinHeight = rcCtrlPanel.Height() + margin_size + graph_min_height;

	this->SetWindowPos(NULL, 0, 0, rcGraph.Width() + 2*margin_size, rcGraph.Height() + rcCtrlPanel.Height() + margin_size, SWP_NOMOVE | SWP_NOZORDER);
}


void CHistogramDialog2::OnKickIdle(void)
{
	m_ControlPanel->UpdateDialogControls(m_ControlPanel, FALSE);
}


void CHistogramDialog2::SetDataFormat(ULONG uDataFormat)
{
   if (m_uDataFormat == uDataFormat)
    {
        // It's the same data pormat as last time, so there is nothing
        // for us to do.
        return;
    }
    
   m_uDataFormat = uDataFormat;
    m_ControlPanel->UpdateChannelLabels (uDataFormat);

    if (UsesGenericChannels (uDataFormat))
	{
      // If we using the Geneneric controls, make sure they are the only ones enabled
    	for (int i = 0; i < PIXEL_TYPES_TOTAL; i++)
		{
			m_elts[i].bShow = (i >= CHANNEL_0 && i <= CHANNEL_2);
		}

      if (IsAlphaFormat(uDataFormat))
      {
         m_elts[ALPHA_PIXELS].bShow = TRUE; // Alpha formats use an additional channel for the aplha value
      }
		return;
	}

	if (!m_bAutoFormatMode)
		return;

	if (uDataFormat == PIXEL_FORMAT_MONO8 
	    || uDataFormat == PIXEL_FORMAT_MONO16
	    || uDataFormat == PIXEL_FORMAT_MONO12_PACKED
	    || uDataFormat == PIXEL_FORMAT_MONO12_PACKED_MSFIRST
        || uDataFormat == PIXEL_FORMAT_MONO10_PACKED_MSFIRST
        || uDataFormat == PIXEL_FORMAT_POLAR4_12
        || uDataFormat == PIXEL_FORMAT_POLAR_RAW4_12)
	{
		for (int i = 0; i < PIXEL_TYPES_TOTAL; i++)
		{
			m_elts[i].bShow = (i == ALL_PIXELS);
		}
	}
	else if (IsBayerFormat(uDataFormat))
	{
		// Show 4 bayer channels
		for (int i = 0; i < PIXEL_TYPES_TOTAL; i++)
		{
			m_elts[i].bShow = (i >= BAYER_FIRST_CHANNEL&& i <= BAYER_LAST_CHANNEL);
		}
	}

}

template<typename T>
static inline T SQUARE(T val)
{
	return val * val;
}

template<typename T>
std::pair<double,double>
GetMeanAndSD(T const* pdata, int len)
{
	double total = 0;
	long count = 0;
	int i;
	for (i = 0; i < len; i++)
	{
		total += i * pdata[i];
		count += static_cast<long>(pdata[i]);
	}
	double mean = total / count;

	total = 0;
	for (i = 0; i < len; i++)
	{
		total += pdata[i] * SQUARE(mean - i);
	}
	double sd = sqrt(total / (count-1));

	return std::make_pair(mean, sd);
}

void CHistogramDialog2::DataUpdated(ULONG uDataFormat)
{
	// Transfer the updated histogram data into the GraphCtrl.
	for (int i = 0; i < PIXEL_TYPES_TOTAL; i++)
	{
		if (m_elts[i].bShow)
		{
			// This element is to be displayed.
			// Check whether it has already been added to the GraphCtrl object.
			if (m_elts[i].eltNo == -1)
			{
				m_elts[i].eltNo = m_Graph.AddElement();
				m_Graph.SetLineColor(m_elts[i].eltNo, m_elts[i].color);
				m_Graph.SetLineWidth(m_elts[i].eltNo, m_elts[i].lineWidth);
			}
			m_Graph.SetYData(m_elts[i].eltNo, &m_elts[i].data[0], m_elts[i].data.size());

			// Calculate Mean and StdDev for this element.
			std::pair<double,double> msd = GetMeanAndSD(&m_elts[i].data[0], m_elts[i].data.size());
			m_elts[i].mean = msd.first;
			m_elts[i].stddev = msd.second;

		}
		else
		{
			// Don't show this element.
			// If the element has only just had its bShow member set to false
			// (by unchecking its checkbox), then we need to remove its
			// corresponding element from the GraphCtrl object.
			if (m_elts[i].eltNo != -1)
			{
				m_Graph.RemoveElement(m_elts[i].eltNo);
				m_elts[i].eltNo = -1;
			}
			m_elts[i].mean = m_elts[i].stddev = 0;
		}
	}
}


/**
* Function: OnCancel
* Purpose:  
*/
void CHistogramDialog2::OnCancel()
{
	CWaitCursor wc;

	// bugzilla.232 -- dont try to perform camera operations on a a camera that is gone.
	if (m_pCam)
	{
	    try
    	{
	    	m_pCam->SetStreamState(STOP_STREAM);
		    m_pCam->SetCallback(OVERLAY_FRAME, NULL, NULL);
    	}
	    catch (PxLException const&)
	    {
    		// Ignore. This happens when this function is called from the
	    	// camera object's Uninitialize method - m_handle has already been set
		    // to NULL, so the call to SetCallback above fails.
    		//m_pCam->OnApiError(e);
	    	//return;
	    }
	    m_pCam->m_pHistogramWindow = NULL;
	}

	// Make a copy of the member data that we access after "delete this".
	CMutex* pLock = m_pLock;

	// Lock the Mutex, which prevents the callback thread from accessing any
	// data members of this object.
	BOOL locked = pLock->Lock(3200); // Try to lock the Mutex, but use a timeout.
	// When we get here, one of two things has happened - either we have
	// succeded in acquiring the Mutex, or we timed out, and can be pretty
	// sure that the callback thread is dead. Either way, we don't
	// need to worry about the callback trying to access deleted data structures,
	// so it's safe to go ahead and delete the dialog object.
	

	CDialog::OnCancel();

	// With the Mutex locked (or the callback threa dead), we can be sure (I think)
	// that the callback thread will not try to access any member data (eg the
	// vector or the histogram control), so it's safe to delete them.
	delete this;

	if (locked)
		pLock->Unlock();

	delete pLock;

}


void CHistogramDialog2::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// Resize the Graph to fill the new window.
	if (m_Graph.m_hWnd != NULL && m_ControlPanel->m_hWnd != NULL)
	{
		CRect rcCtrlPanel;
		m_ControlPanel->GetWindowRect(&rcCtrlPanel);
		m_Graph.SetWindowPos(
					NULL,
					margin_size,
					margin_size,
					cx - (2 * margin_size),
					cy - (margin_size + rcCtrlPanel.Height()),
					SWP_NOZORDER);
		m_ControlPanel->SetWindowPos(
					NULL,
					0,
					cy - rcCtrlPanel.Height(),
					0,
					0,
					SWP_NOSIZE | SWP_NOZORDER);
	}
}


void CHistogramDialog2::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = m_MinWidth;
	lpMMI->ptMinTrackSize.y = m_MinHeight;

	CDialog::OnGetMinMaxInfo(lpMMI);
}


BOOL CHistogramDialog2::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (message == WM_APP_HISTOGRAM_DATA)
	{
		// Notification that the GraphCtrl has updated data. Tell it to redraw itself.
		m_Graph.Invalidate(FALSE);
		m_ControlPanel->OnIdleUpdateControls(NULL);
		return TRUE;
	}

	return CDialog::OnWndMsg(message, wParam, lParam, pResult);
}


BOOL CHistogramDialog2::PreTranslateMessage(MSG* pMsg)
{
	// This is an attempt to be able to switch between "Simple" mode and 
	// and "Engineering" mode (with even/odd, bayer, etc) using a "secret"
	// accelerator key (Ctrl-Shift-F9).
	// It only seems to work once, just after opening the histogram. I don't know why yet.

	// Pass accelerators onto the main window explicitly.
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{
		if (::TranslateAccelerator(AfxGetMainWnd()->GetSafeHwnd(), m_hAccellTable, pMsg))
			return TRUE;

		if (::TranslateAccelerator(this->GetSafeHwnd(), m_hAccellTable2, pMsg))
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}


BOOL CHistogramDialog2::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// Bugzilla.2117
    //     We no longer support the simple mode of the histogram -- the all of the newer items have been added to the
    //     the 'advanced (or engineering) varaint.  So, don't allow the user to switch from the default (advanced mode).
    //if (LOWORD(wParam) == 1492)
	//{
	//	this->SwitchMode();
	//}

	return CDialog::OnCommand(wParam, lParam);
}


void CHistogramDialog2::SwitchMode(void)
{
	m_bSimpleMode = !m_bSimpleMode;
	this->SetControlPanel();
}

void CHistogramDialog2::ForgetCamera(void)
{
	m_pCam = NULL;
}

// Bugzilla.1253 - Update the max SharpnessScore if the sharpness score ROI is set.
/**
* Function: FeatureChanged
*/
void CHistogramDialog2::FeatureChanged(U32 featureId)
{
	switch (featureId)
	{
	case FEATURE_SHARPNESS_SCORE:
        m_maxSharpnessScore = m_pCam->GetFeatureMaxVal(FEATURE_SHARPNESS_SCORE, FEATURE_SHARPNESS_SCORE_MAX_VALUE);
		break;
	default:
		// A feature that is not relevant to this page. Ignore the change.
		break;
	}
}





















