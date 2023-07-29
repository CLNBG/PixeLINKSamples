/**********************************************************************************************************
 *
 * Design Notes:
 *  o packed data                                                                          
 *       When dealing with packed data, we always ignore the least significant pixel data.  That we only use
 *       the most significant 8 bits of pixel data, and ignore the 'backed' byte.  Where that packed byte
 *       appears in the image data, depends on the type of packing.
 *
 *       With 12 bit packed format, a pixel share's their least significant 4 bits with the pixel 
 *       beside it (thus packed).  As a simplification, these routines will ignore these bits; only 
 *       using the most significant 8 bits of each pixel.  In other words, a Red Green row from a (Bayer) 
 *       color image would look like this.
 *            R RG G R RG G R RG G R RG G R RG G
 *          Were R = most significant 8 bits of a red pixel
 *               G = most significant 8 bits of a green pixel
 *               RG = the least significant 4 bits of the red pixel to the left * 16 ORed with
 *                    the least significant 4 bits of the green pixel to the right.
 *      Filtered image will always have 0's for these RG bytes.
 *    
 *      HOWEVER, The _MSFIRST 12 bit packed formats are similiar, but the location of the byte containing the
 *      least significant bits, is in a differn tlocation.  Specifically, the color image from above would
 *      look as follows:
 *            R G RG R G RG R G RG R G RG R G RG
 *      The callbacks can deal with either packing scheme, using the BOOLEAN msFirst to indicate which packing
 *      scheme is being used.
 *
 *      For 10 bit packed, we only support one type of packing, where the image looks as follows:
 *            R G R G RGRG R G R G RGRG
 *                                                                                                        
 **********************************************************************************************************/

// CallbacksGui.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "CallbacksGui.h"

/******************************************************************************
* Callback Functions
******************************************************************************/

#define DCAM16_TO_TENBIT(x) ((((x) & 0x00FF) << 2) | ((x) >> 14))
#define TENBIT_TO_DCAM16(x) ((((x) & 0x03FC) >> 2) | ((x) << 14))

CCallbacksGui* s_CallbackGui; // Allow Lens gui to access this

struct RGBPixel
{
	U8 R,G,B;
	RGBPixel() : R(0),G(0),B(0) {}
	RGBPixel(U8 r, U8 g, U8 b) : R(r),G(g),B(b) {}
};
RGBPixel operator+(RGBPixel const& rgb, int val) { return RGBPixel(rgb.R+val, rgb.G+val, rgb.B+val); }
RGBPixel operator+(int val, RGBPixel const& rgb) { return RGBPixel(rgb.R+val, rgb.G+val, rgb.B+val); }
RGBPixel operator+(RGBPixel const& rgb1, RGBPixel const& rgb2) { return RGBPixel(rgb1.R+rgb2.R, rgb1.G+rgb2.G, rgb1.B+rgb2.B); }
RGBPixel operator*(RGBPixel const& rgb, int val) { return RGBPixel(rgb.R*val, rgb.G*val, rgb.B*val); }
RGBPixel operator*(int val, RGBPixel const& rgb) { return RGBPixel(rgb.R*val, rgb.G*val, rgb.B*val); }
RGBPixel operator/(RGBPixel const& rgb, int val) { return RGBPixel(rgb.R/val, rgb.G/val, rgb.B/val); }

RGBPixel 
abs(RGBPixel const& rgb)
{
	return RGBPixel(abs(rgb.R), abs(rgb.G), abs(rgb.B));
}

template<typename T>
void Convolution_3x3(int const* kernel, T* pData, const int width, const int height)
{
	int normalizer = 0;
	for (int i = 0; i < 9; i++) 
	{
		normalizer += kernel[i];
	}
	normalizer = abs(normalizer);
	if (normalizer == 0) 
	{
		normalizer = 1;
	}


	// Make a buffer to hold the unaltered values of the previous row.
	std::vector<T> rowBuffer(width);
	T prevVal;

	// Copy row 0 into the rowBuffer.
	memcpy(&rowBuffer[0], pData, width * sizeof(T));

	for (int y = 1; y < height-1; y++)
	{
		// Copy the "about-to-be-altered" row into the rowBuffer
		memcpy(&rowBuffer[0], pData + (y*width), width * sizeof(T));
		T* prevRow = &rowBuffer[0];
		T* thisRow = pData + (y * width);
		T* nextRow = pData + ((y+1) * width);
		prevVal = thisRow[0];
		for (int x = 1; x < width-1; x++)
		{
			T temp = thisRow[x];
			thisRow[x] = abs(
					kernel[0] * prevRow[x-1] +
					kernel[1] * prevRow[x] +
					kernel[2] * prevRow[x+1] +
					kernel[3] * prevVal +
					kernel[4] * thisRow[x] +
					kernel[5] * thisRow[x+1] +
					kernel[6] * nextRow[x-1] +
					kernel[7] * nextRow[x] +
					kernel[8] * nextRow[x+1]
				) / normalizer;
			prevVal = temp;
		}
	}
}

void 
Convolution_3x3_12Bit_Packed(int const* kernel, U8* pData, const int width, const int height, BOOL msFirst)
// see Design Notes above on special handling of 12 bit packed formats.
{
	int normalizer = 0;
	int widthPlusHalf = width + width/2;
	for (int i = 0; i < 9; i++) 
	{
		normalizer += kernel[i];
	}
	normalizer = abs(normalizer);
	if (normalizer == 0) 
	{
		normalizer = 1;
	}


	// Make a buffer to hold the unaltered values of the previous row.
	std::vector<U8> rowBuffer(widthPlusHalf);
	U8              prevVal;
	int             skipByte = msFirst ? 2 : 1;  //Are we to skip every 3rd (msFirst) or 2nd (normal) byte?

	// Copy row 0 into the rowBuffer.
	memcpy(&rowBuffer[0], pData, widthPlusHalf);

	for (int y = 1; y < height-1; y++)
	{
		// Copy the "about-to-be-altered" row into the rowBuffer
		memcpy(&rowBuffer[0], pData + (y*widthPlusHalf), widthPlusHalf);
		U8* prevRow = &rowBuffer[0];
		U8* thisRow = pData + (y * widthPlusHalf);
		U8* nextRow = pData + ((y+1) * widthPlusHalf);
		prevVal = thisRow[0];
		for (int x = 1; x < widthPlusHalf-1; x++) 
		{
			if (x % 3 == skipByte) 
			{
			    // Ignore the LS bits
    			thisRow[x] = 0;
			    continue;  
			}
			U8 temp = thisRow[x];
			if ((x & 1) != msFirst)
			{
			    // Ignore the previous 'column' -- only has LS bits
    			thisRow[x] = abs(
					    kernel[1] * prevRow[x] +
					    kernel[2] * prevRow[x+1] +
					    kernel[4] * thisRow[x] +
					    kernel[5] * thisRow[x+1] +
					    kernel[7] * nextRow[x] +
					    kernel[8] * nextRow[x+1]
				) / normalizer;
			} else {
                // Ignore the next 'column' -- only has LS bits
    			thisRow[x] = abs(
					    kernel[0] * prevRow[x-1] +
					    kernel[1] * prevRow[x] +
					    kernel[3] * prevVal +
					    kernel[4] * thisRow[x] +
					    kernel[6] * nextRow[x-1] +
					    kernel[7] * nextRow[x]
				    ) / normalizer;
			}
			prevVal = temp;
		}
	}
}

void 
Convolution_3x3_10Bit_Packed(int const* kernel, U8* pData, const int width, const int height)
// see Design Notes above on special handling of 10 bit packed formats.
{
	int normalizer = 0;
	int widthPlusQuarter = width + width/4;
	for (int i = 0; i < 9; i++) 
	{
		normalizer += kernel[i];
	}
	normalizer = abs(normalizer);
	if (normalizer == 0) 
	{
		normalizer = 1;
	}


	// Make a buffer to hold the unaltered values of the previous row.
	std::vector<U8> rowBuffer(widthPlusQuarter);
	U8              prevVal;
	int             div5;  //We to skip every 5th byte

	// Copy row 0 into the rowBuffer.
	memcpy(&rowBuffer[0], pData, widthPlusQuarter);

	for (int y = 1; y < height-1; y++)
	{
		// Copy the "about-to-be-altered" row into the rowBuffer
		memcpy(&rowBuffer[0], pData + (y*widthPlusQuarter), widthPlusQuarter);
		U8* prevRow = &rowBuffer[0];
		U8* thisRow = pData + (y * widthPlusQuarter);
		U8* nextRow = pData + ((y+1) * widthPlusQuarter);
		prevVal = thisRow[0];
		for (int x = 1; x < widthPlusQuarter-1; x++) 
		{
			div5 = x % 5;
			U8 temp = thisRow[x];
            switch (div5) {
            case 0:
			    // Ignore the previous 'column' -- only has LS bits
    			thisRow[x] = abs(
					    kernel[1] * prevRow[x] +
					    kernel[2] * prevRow[x+1] +
					    kernel[4] * thisRow[x] +
					    kernel[5] * thisRow[x+1] +
					    kernel[7] * nextRow[x] +
					    kernel[8] * nextRow[x+1]
				) / normalizer;
                break;
            case 3:
                // Ignore the next 'column' -- only has LS bits
    			thisRow[x] = abs(
					    kernel[0] * prevRow[x-1] +
					    kernel[1] * prevRow[x] +
					    kernel[3] * prevVal +
					    kernel[4] * thisRow[x] +
					    kernel[6] * nextRow[x-1] +
					    kernel[7] * nextRow[x]
				    ) / normalizer;
                break;
            case 4:
                // Ignore this 'column' -- it only has LS bits
                thisRow[x] = 0;
                break;
            default:
			    thisRow[x] = abs(
					    kernel[0] * prevRow[x-1] +
					    kernel[1] * prevRow[x] +
					    kernel[2] * prevRow[x+1] +
					    kernel[3] * prevVal +
					    kernel[4] * thisRow[x] +
					    kernel[5] * thisRow[x+1] +
					    kernel[6] * nextRow[x-1] +
					    kernel[7] * nextRow[x] +
					    kernel[8] * nextRow[x+1]
				    ) / normalizer;
                break;
            }
			prevVal = temp;
		}
	}
}

void 
Convolution_3x3_RGB(int const * const kernel, U8* pData, const int width, const int height)
{
	int normalizer = 0;
	for (int i = 0; i < 9; i++) 
	{
		normalizer += kernel[i];
	}
	normalizer = abs(normalizer);
	if (normalizer == 0) 
	{
		normalizer = 1;
	}

	int bytewidth = width * 3;

	// Make a buffer to hold the unaltered values of the previous row and the
	// current row.
	U8* buffer = new U8[bytewidth * 2];

	// Copy row 0 into the buffer.
	memcpy(buffer, pData, bytewidth * sizeof(U8));

	for (int y = 1; y < height-1; y++)
	{
		// Copy the "about-to-be-altered" row into the buffer
		memcpy(&buffer[bytewidth*(y%2)], &pData[y*bytewidth], bytewidth);
		for (int x = 1; x < width-1; x++)
		{
			U8* prevRow = &buffer[bytewidth * ((y+1)%2)];
			U8* thisRow = &buffer[bytewidth * (y%2)];
			for (int rgb = 0; rgb < 3; rgb++)
			{
				int newVal = 0;
				for (int i = 0; i < 3; i++)
				{
					// Get the first two rows from the buffer;
					newVal += kernel[i] * prevRow[3*(x+i-1) + rgb];
					newVal += kernel[3+i] * thisRow[3*(x+i-1) + rgb];
					// Get the last row from the actual data - it hasn't been overwritten yet.
					newVal += kernel[6+i] * pData[(y+1)*bytewidth + 3*(x+i-1) + rgb];
				}
				pData[y*bytewidth + 3*x + rgb] = static_cast<U8>(abs(newVal)/normalizer);
			}
		}
	}
}

void Convolution_3x3_YUV(int const* kernel, U8* pData, const int width, const int height)
{
	int normalizer = 0;
    int widthInBytes = width*2;
	for (int i = 0; i < 9; i++) 
	{
		normalizer += kernel[i];
	}
	normalizer = abs(normalizer);
	if (normalizer == 0) 
	{
		normalizer = 1;
	}

	// Make a buffer to hold the unaltered values of the previous and the current row.
    //    Note that we try to be clever here, for 'odd' rows of the original, the first 
    //    and second rows of rowBuffer are previous and current respectivly.  However, the
    //    opposite is true for 'even' rows
	std::vector<U8> rowBuffer(widthInBytes * 2);

	// Copy row 0 into the rowBuffer (previous row).
	memcpy(&rowBuffer[0], pData, widthInBytes);

	for (int y = 1; y < height-1; y++)
	{
		// Copy the "about-to-be-altered" row (this row) into the rowBuffer
		memcpy(&rowBuffer[widthInBytes*(y%2)], pData + (widthInBytes*y), widthInBytes);
		U8* prevRow = &rowBuffer[widthInBytes*((y+1)%2)];
		U8* thisRow = &rowBuffer[widthInBytes*(y%2)];
		U8* nextRow = pData + (widthInBytes* (y+1));
		for (int x = 1; x < width-1; x++)
		{
			int newVal = 0; 
			for (int i = 0; i < 3; i++)
			{
				newVal += kernel[i] * prevRow[2*(x+i-1)+1];
				newVal += kernel[3+i] * thisRow[2*(x+i-1)+1];
				newVal += kernel[6+i] * nextRow[2*(x+i-1)+1];
            }
            pData[(widthInBytes*y) + (2*x) + 1] = static_cast<U8>(abs(newVal) / normalizer);
		}
	}
}

const int highpass_kernel_3x3[] = 
{
	-1,		-1,		-1,
	-1,		9,		-1,
	-1,		-1,		-1
};

const int lowpass_kernel_3x3[] = 
{
	1,		2,		1,
	2,		4,		2,
	1,		2,		1
};

const int sobel_vertical_3x3[] = 
{
	-1,		0,		1,
	-2,		0,		2,
	-1,		0,		1
};

const int sobel_horizontal_3x3[] = 
{
	-1,		-2,		-1,
	0,		0,		0,
	1,		2,		1
};

#define PXLAPI_CALLBACK(funcname)						\
	U32 __stdcall funcname( HANDLE hCamera,				\
							LPVOID pFrameData,			\
							U32 uDataFormat,			\
							FRAME_DESC const * pFrameDesc,		\
							LPVOID pContext)			\

template<typename T>
void MedianFilter_3x3_Impl(T* const pData, T const * const pCopy, const int width, const int height)
{
	int buf[9];
	for (int y = 1; y < height-1; y++)
	{
		for (int x = 1; x < width-1; x++)
		{
			for (int yy = -1; yy <= 1; yy++)
			{
				for (int xx = -1; xx <= 1; xx++)
				{
					// This could be made faster - too much going on in the inner loop here...
					buf[3*(yy+1) + (xx+1)] = pCopy[(y+yy)*width + (x+xx)];
				}
			}
			std::sort(&buf[0], &buf[9]);
			pData[y*width + x] = static_cast<T>(buf[4]);
		}
	}
}

// See Design Notes above on special handling of 12-bit packed format
void 
MedianFilter_3x3_12Bit_Packed_Impl(U8* const pData, U8 const * const pCopy, const int width, const int height, BOOL msFirst)
{
	int  buf[6];
	int* pBuf;
	int widthPlusHalf = width + width/2;
	int skipByte = msFirst ? 2 : 1;  //Are we to skip every 3rd (msFirst) or 2nd (normal) byte?

	for (int y = 1; y < height-1; y++)
	{
		for (int x = 1; x < widthPlusHalf-1; x++)
		{
			if (x % 3 == skipByte) 
			{
			    // Ignore the LS bits
    			pData[y*widthPlusHalf + x] = 0;
			    continue;  
			}
			
			pBuf = &buf[0];
			for (int yy = -1; yy <= 1; yy++)
			{
				for (int xx = -1; xx <= 1; xx++)
				{
                    if ( (x & !msFirst) && xx ==  1) continue; // Skip bytes containing LS bits (col 1)
                    if (!(x & !msFirst) && xx == -1) continue; // Skip bytes containing LS bits (col -1)
				    *pBuf++ = pCopy[(y+yy)*widthPlusHalf + (x+xx)];
				}
			}
			std::sort(&buf[0], &buf[5]);
			pData[y*widthPlusHalf + x] = static_cast<U8>((buf[2] + buf[3])/2);  // average the middle two
		}
	}
}

// See Design Notes above on special handling of 10-bit packed format
void 
MedianFilter_3x3_10Bit_Packed_Impl(U8* const pData, U8 const * const pCopy, const int width, const int height)
{
	int  buf[6];
	int* pBuf;
	int widthPlusQuarter = width + width/4;
	int div5;  //we to skip every 5th byte

	for (int y = 1; y < height-1; y++)
	{
		for (int x = 1; x < widthPlusQuarter-1; x++)
		{
			div5 = x % 5;
            if (div5 == 4) 
			{
			    // Ignore the LS bits
    			pData[y*widthPlusQuarter + x] = 0;
			    continue;  
			}
			
			pBuf = &buf[0];
			for (int yy = -1; yy <= 1; yy++)
			{
				for (int xx = -1; xx <= 1; xx++)
				{
                    if (div5 == 3 && xx ==  1) continue; // Skip bytes containing LS bits (col 1)
                    if (div5 == 0 && xx == -1) continue; // Skip bytes containing LS bits (col -1)
				    *pBuf++ = pCopy[(y+yy)*widthPlusQuarter + (x+xx)];
				}
			}
			std::sort(&buf[0], &buf[5]);
			pData[y*widthPlusQuarter + x] = static_cast<U8>((buf[2] + buf[3])/2);  // average the middle two
		}
	}
}

void 
MedianFilter_3x3_RGB_Impl(U8* const pData, U8 const * const pCopy, const int width, const int height)
{
	int buf[9];
	for (int y = 1; y < height-1; y++)
	{
		for (int x = 1; x < width-1; x++)
		{
			for (int rgb = 0; rgb < 3; rgb++)
			{
				for (int yy = -1; yy <= 1; yy++)
				{
					for (int xx = -1; xx <= 1; xx++)
					{
						// This could be made faster - too much going on in the inner loop here...
						buf[3*(yy+1) + (xx+1)] = pCopy[3*(y+yy)*width + 3*(x+xx) + rgb];
					}
				}
				std::sort(&buf[0], &buf[9]);
				pData[3*(y*width + x) + rgb] = static_cast<U8>(buf[4]);
			}
		}
	}
}

PXLAPI_CALLBACK(MedianFilter_3x3)
{
	int decX = max(1, static_cast<int>(pFrameDesc->PixelAddressingValue.fHorizontal));
	int decY = max(1, static_cast<int>(pFrameDesc->PixelAddressingValue.fVertical));
    int hdrWidth = pFrameDesc->HDRInfo.uMode == FEATURE_GAIN_HDR_MODE_INTERLEAVED ? 
        static_cast<int>(pFrameDesc->Roi.fWidth*2.0) : static_cast<int>(pFrameDesc->Roi.fWidth); // Bugzilla.2059
	int decWidth = DEC_SIZE(hdrWidth, decX);
	int decHeight = DEC_SIZE(static_cast<int>(pFrameDesc->Roi.fHeight), decY);
	float pixelSize = GetPixelFormatSize(static_cast<int>(uDataFormat));
	int bufferSize = static_cast<int> (static_cast<float>(decWidth) * static_cast<float>(decHeight) * pixelSize);

	// Allocate enough memory to hold a copy of the frame.
	std::vector<byte> buffer(bufferSize);
	memcpy(&buffer[0], pFrameData, bufferSize);

	switch (uDataFormat)
	{
	case PIXEL_FORMAT_MONO8:
	case PIXEL_FORMAT_BAYER8_BGGR:
	case PIXEL_FORMAT_BAYER8_GBRG:
	case PIXEL_FORMAT_BAYER8_GRBG:
	case PIXEL_FORMAT_BAYER8_RGGB:
		MedianFilter_3x3_Impl<U8>(static_cast<U8*>(pFrameData),
								  static_cast<U8*>(&buffer[0]), 
								  decWidth, 
								  decHeight);
		break;

	case PIXEL_FORMAT_MONO16:
	case PIXEL_FORMAT_BAYER16_BGGR:
	case PIXEL_FORMAT_BAYER16_GBRG:
	case PIXEL_FORMAT_BAYER16_GRBG:
	case PIXEL_FORMAT_BAYER16_RGGB:
		MedianFilter_3x3_Impl<U16>(static_cast<U16*>(pFrameData),
								   reinterpret_cast<U16*>(&buffer[0]), 
								   decWidth, 
								   decHeight);
		break;

	case PIXEL_FORMAT_MONO12_PACKED:
	case PIXEL_FORMAT_BAYER12_BGGR_PACKED:
	case PIXEL_FORMAT_BAYER12_GBRG_PACKED:
	case PIXEL_FORMAT_BAYER12_GRBG_PACKED:
	case PIXEL_FORMAT_BAYER12_RGGB_PACKED:
		MedianFilter_3x3_12Bit_Packed_Impl(static_cast<U8*>(pFrameData),
								   reinterpret_cast<U8*>(&buffer[0]), 
								   decWidth, 
								   decHeight,
								   FALSE);
		break;

	case PIXEL_FORMAT_MONO12_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER12_BGGR_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER12_GBRG_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER12_GRBG_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER12_RGGB_PACKED_MSFIRST:
		MedianFilter_3x3_12Bit_Packed_Impl(static_cast<U8*>(pFrameData),
								   reinterpret_cast<U8*>(&buffer[0]), 
								   decWidth, 
								   decHeight,
								   TRUE);
		break;

	case PIXEL_FORMAT_MONO10_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER10_BGGR_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER10_GBRG_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER10_GRBG_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER10_RGGB_PACKED_MSFIRST:
		MedianFilter_3x3_10Bit_Packed_Impl(static_cast<U8*>(pFrameData),
								   reinterpret_cast<U8*>(&buffer[0]), 
								   decWidth, 
								   decHeight);
		break;

	case PIXEL_FORMAT_RGB24:
		MedianFilter_3x3_RGB_Impl(static_cast<U8*>(pFrameData),
								  static_cast<U8*>(&buffer[0]), 
								  decWidth, 
								  decHeight);

	default:
		return ApiInvalidParameterError;
	}
	
	return ApiSuccess;
}

PXLAPI_CALLBACK(LowPassFilter_3x3)
{
	int decX = static_cast<int>(pFrameDesc->PixelAddressingValue.fHorizontal);
	int decY = static_cast<int>(pFrameDesc->PixelAddressingValue.fVertical);
    int hdrWidth = pFrameDesc->HDRInfo.uMode == FEATURE_GAIN_HDR_MODE_INTERLEAVED ? 
        static_cast<int>(pFrameDesc->Roi.fWidth*2.0) : static_cast<int>(pFrameDesc->Roi.fWidth); // Bugzilla.2059
	int width = DEC_SIZE(hdrWidth, decX);
	int height = DEC_SIZE(static_cast<int>(pFrameDesc->Roi.fHeight), decY);

	switch (uDataFormat)
	{
	case PIXEL_FORMAT_MONO8:
	case PIXEL_FORMAT_BAYER8_BGGR:
	case PIXEL_FORMAT_BAYER8_GBRG:
	case PIXEL_FORMAT_BAYER8_GRBG:
	case PIXEL_FORMAT_BAYER8_RGGB:
		Convolution_3x3<U8>(&lowpass_kernel_3x3[0], 
							static_cast<U8*>(pFrameData), 
							width, 
							height);
		break;

	case PIXEL_FORMAT_MONO16:
	case PIXEL_FORMAT_BAYER16_BGGR:
	case PIXEL_FORMAT_BAYER16_GBRG:
	case PIXEL_FORMAT_BAYER16_GRBG:
	case PIXEL_FORMAT_BAYER16_RGGB:
		Convolution_3x3<U16>(&lowpass_kernel_3x3[0],
							 static_cast<U16*>(pFrameData), 
							 width, 
							 height);
		break;

	case PIXEL_FORMAT_MONO12_PACKED:
	case PIXEL_FORMAT_BAYER12_BGGR_PACKED:
	case PIXEL_FORMAT_BAYER12_GBRG_PACKED:
	case PIXEL_FORMAT_BAYER12_GRBG_PACKED:
	case PIXEL_FORMAT_BAYER12_RGGB_PACKED:
		Convolution_3x3_12Bit_Packed(&lowpass_kernel_3x3[0],
							 static_cast<U8*>(pFrameData), 
							 width, 
							 height,
							 FALSE);
		break;

	case PIXEL_FORMAT_MONO12_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER12_BGGR_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER12_GBRG_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER12_GRBG_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER12_RGGB_PACKED_MSFIRST:
		Convolution_3x3_12Bit_Packed(&lowpass_kernel_3x3[0],
							 static_cast<U8*>(pFrameData), 
							 width, 
							 height,
							 TRUE);
		break;

	case PIXEL_FORMAT_MONO10_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER10_BGGR_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER10_GBRG_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER10_GRBG_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER10_RGGB_PACKED_MSFIRST:
		Convolution_3x3_10Bit_Packed(&lowpass_kernel_3x3[0],
							 static_cast<U8*>(pFrameData), 
							 width, 
							 height);
		break;

	case PIXEL_FORMAT_RGB24:
		Convolution_3x3_RGB(&lowpass_kernel_3x3[0],
							static_cast<U8*>(pFrameData),
							width,
							height);
        break;

	case PIXEL_FORMAT_YUV422:
		Convolution_3x3_YUV(&lowpass_kernel_3x3[0],
							static_cast<U8*>(pFrameData),
							width,
							height);
        break;

	default:
		return ApiInvalidParameterError;
	}

	return ApiSuccess;
}

PXLAPI_CALLBACK(HighPassFilter_3x3)
{
	int decX = static_cast<int>(pFrameDesc->PixelAddressingValue.fHorizontal);
	int decY = static_cast<int>(pFrameDesc->PixelAddressingValue.fVertical);
    int hdrWidth = pFrameDesc->HDRInfo.uMode == FEATURE_GAIN_HDR_MODE_INTERLEAVED ? 
        static_cast<int>(pFrameDesc->Roi.fWidth*2.0) : static_cast<int>(pFrameDesc->Roi.fWidth); // Bugzilla.2059
	int width = DEC_SIZE(hdrWidth, decX);
	int height = DEC_SIZE(static_cast<int>(pFrameDesc->Roi.fHeight), decY);

	switch (uDataFormat)
	{
	case PIXEL_FORMAT_MONO8:
	case PIXEL_FORMAT_BAYER8_BGGR:
	case PIXEL_FORMAT_BAYER8_GBRG:
	case PIXEL_FORMAT_BAYER8_GRBG:
	case PIXEL_FORMAT_BAYER8_RGGB:
		Convolution_3x3<U8>(&highpass_kernel_3x3[0], 
							static_cast<U8*>(pFrameData), 
							width, 
							height);
		break;

	case PIXEL_FORMAT_MONO16:
	case PIXEL_FORMAT_BAYER16_BGGR:
	case PIXEL_FORMAT_BAYER16_GBRG:
	case PIXEL_FORMAT_BAYER16_GRBG:
	case PIXEL_FORMAT_BAYER16_RGGB:
		Convolution_3x3<U16>(&highpass_kernel_3x3[0],
							 static_cast<U16*>(pFrameData), 
							 width, 
							 height);
		break;

	case PIXEL_FORMAT_MONO12_PACKED:
	case PIXEL_FORMAT_BAYER12_BGGR_PACKED:
	case PIXEL_FORMAT_BAYER12_GBRG_PACKED:
	case PIXEL_FORMAT_BAYER12_GRBG_PACKED:
	case PIXEL_FORMAT_BAYER12_RGGB_PACKED:
		Convolution_3x3_12Bit_Packed(&highpass_kernel_3x3[0],
							 static_cast<U8*>(pFrameData), 
							 width, 
							 height,
							 FALSE);
		break;

	case PIXEL_FORMAT_MONO12_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER12_BGGR_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER12_GBRG_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER12_GRBG_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER12_RGGB_PACKED_MSFIRST:
		Convolution_3x3_12Bit_Packed(&highpass_kernel_3x3[0],
							 static_cast<U8*>(pFrameData), 
							 width, 
							 height,
							 TRUE);
		break;

	case PIXEL_FORMAT_MONO10_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER10_BGGR_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER10_GBRG_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER10_GRBG_PACKED_MSFIRST:
	case PIXEL_FORMAT_BAYER10_RGGB_PACKED_MSFIRST:
		Convolution_3x3_10Bit_Packed(&highpass_kernel_3x3[0],
							 static_cast<U8*>(pFrameData), 
							 width, 
							 height);
		break;

	case PIXEL_FORMAT_RGB24:
		Convolution_3x3_RGB(&highpass_kernel_3x3[0],
							static_cast<U8*>(pFrameData),
							width,
							height);
        break;

	case PIXEL_FORMAT_YUV422:
		Convolution_3x3_YUV(&highpass_kernel_3x3[0],
							static_cast<U8*>(pFrameData),
							width,
							height);
        break;

	default:
		return ApiInvalidParameterError;
	}

	return ApiSuccess;
}

template<typename T>
void 
SobelFilter_3x3_Impl(T* pData, T* pCopy, int width, int height)
{
	Convolution_3x3<T>(	&sobel_vertical_3x3[0],
						pData,
						width,
						height );
	Convolution_3x3<T>( &sobel_horizontal_3x3[0],
						pCopy,
						width,
						height );
	for (int i = 0; i < width*height; i++)
	{
		// Should really be: sqrt(sqr(pData[i])+sqr(pCopy[i])), but this is faster and close enough:
		pData[i] = (pData[i] + pCopy[i]) / 2 ;
	}
}

void 
SobelFilter_3x3_12Bit_Packed_Impl(U8* const pData, U8 * const pCopy, const int width, const int height)
{
	Convolution_3x3<U8>( &sobel_vertical_3x3[0],
						pData,
						width,
						height );
	Convolution_3x3<U8>( &sobel_horizontal_3x3[0],
						pCopy,
						width,
						height );
	int widthPlusHalf = width + width/2;
	for (int i = 0; i < widthPlusHalf*height; i++)
	{
		// Should really be: sqrt(sqr(pData[i])+sqr(pCopy[i])), but this is faster and close enough:
		pData[i] = (pData[i] + pCopy[i]) / 2 ;
	}
}

void 
SobelFilter_3x3_RGB_Impl(U8* const pData, U8* const pCopy,const int width, const int height)
{
	Convolution_3x3_RGB(&sobel_vertical_3x3[0],
						pData,
						width,
						height );
    Convolution_3x3_RGB(&sobel_horizontal_3x3[0],  
						pCopy,
						width,
						height );
	for (int i = 0; i < width*height*3; i++)
	{
		// Should really be: sqrt(sqr(pData[i])+sqr(pCopy[i])), but this is faster and close enough:
		pData[i] = (pData[i] + pCopy[i]) / 2 ;
	}
}

void 
SobelFilter_3x3_YUV_Impl(U8* const pData, U8* const pCopy,const int width, const int height)
{
	Convolution_3x3_YUV(&sobel_vertical_3x3[0],
						pData,
						width,
						height );
	Convolution_3x3_YUV(&sobel_horizontal_3x3[0],
						pCopy,
						width,
						height );
	for (int i = 0; i < width*height*2; i+=2)
	{
		// Should really be: sqrt(sqr(pData[i])+sqr(pCopy[i])), but this is faster and close enough:
		pData[i+1] = (pData[i+1] + pCopy[i+1]) / 2 ;
	}
}

PXLAPI_CALLBACK(SobelFilter_3x3)
{
	int decX = static_cast<int>(pFrameDesc->PixelAddressingValue.fHorizontal);
	int decY = static_cast<int>(pFrameDesc->PixelAddressingValue.fVertical);
    int hdrWidth = pFrameDesc->HDRInfo.uMode == FEATURE_GAIN_HDR_MODE_INTERLEAVED ? 
        static_cast<int>(pFrameDesc->Roi.fWidth*2.0) : static_cast<int>(pFrameDesc->Roi.fWidth); // Bugzilla.2059
	int width = DEC_SIZE(hdrWidth, decX);
	int height = DEC_SIZE(static_cast<int>(pFrameDesc->Roi.fHeight), decY);
	float pixelSize = GetPixelFormatSize(static_cast<int>(uDataFormat));
	int bufferSize = static_cast<int> (static_cast<float>(width) 
	                                   * static_cast<float>(height) 
	                                   * pixelSize);
	std::vector<byte> buffer(bufferSize);
	memcpy(&buffer[0], pFrameData, bufferSize);

	switch (uDataFormat)
	{
	case PIXEL_FORMAT_MONO8:
	case PIXEL_FORMAT_BAYER8_BGGR:
	case PIXEL_FORMAT_BAYER8_GBRG:
	case PIXEL_FORMAT_BAYER8_GRBG:
	case PIXEL_FORMAT_BAYER8_RGGB:
		SobelFilter_3x3_Impl<U8>(static_cast<U8*>(pFrameData),
								 static_cast<U8*>(&buffer[0]),
								 width,
								 height);
		break;

	case PIXEL_FORMAT_MONO16:
	case PIXEL_FORMAT_BAYER16_BGGR:
	case PIXEL_FORMAT_BAYER16_GBRG:
	case PIXEL_FORMAT_BAYER16_GRBG:
	case PIXEL_FORMAT_BAYER16_RGGB:
		SobelFilter_3x3_Impl<U16>(static_cast<U16*>(pFrameData),
								  reinterpret_cast<U16*>(&buffer[0]),
								  width,
								  height);
		break;

	case PIXEL_FORMAT_MONO12_PACKED:
	case PIXEL_FORMAT_BAYER12_BGGR_PACKED:
	case PIXEL_FORMAT_BAYER12_GBRG_PACKED:
	case PIXEL_FORMAT_BAYER12_GRBG_PACKED:
	case PIXEL_FORMAT_BAYER12_RGGB_PACKED:
		SobelFilter_3x3_12Bit_Packed_Impl(static_cast<U8*>(pFrameData),
							              reinterpret_cast<U8*>(&buffer[0]),
								          width,
								          height);
		break;

	case PIXEL_FORMAT_RGB24:
	case PIXEL_FORMAT_BGR24:
		SobelFilter_3x3_RGB_Impl(static_cast<U8*>(pFrameData),
								 static_cast<U8*>(&buffer[0]),
								 width,
								 height);
        break;

	case PIXEL_FORMAT_YUV422:
		SobelFilter_3x3_YUV_Impl(static_cast<U8*>(pFrameData),
								 static_cast<U8*>(&buffer[0]),
								 width,
								 height);
        break;

	default:
		return ApiInvalidParameterError;
	}

	return ApiSuccess;
}


//
// Cheap 'n' Easy
// Quick 'n' Dirty
//
PXLAPI_CALLBACK(Ascii)
{
	const int decX = static_cast<int>(pFrameDesc->PixelAddressingValue.fHorizontal);
	const int decY = static_cast<int>(pFrameDesc->PixelAddressingValue.fVertical);
    const int hdrWidth = pFrameDesc->HDRInfo.uMode == FEATURE_GAIN_HDR_MODE_INTERLEAVED ? 
        static_cast<int>(pFrameDesc->Roi.fWidth*2.0) : static_cast<int>(pFrameDesc->Roi.fWidth); // Bugzilla.2059
	const int width = DEC_SIZE(hdrWidth, decX);
	const int height = DEC_SIZE(static_cast<int>(pFrameDesc->Roi.fHeight), decY);

	int asciiChars[][64] = { 
		// #
    	{	
			0,0,0,0,0,0,0,0,
			0,0,1,0,0,1,0,0,
			0,1,1,1,1,1,1,0,
			0,0,1,0,0,1,0,0,
			0,0,1,0,0,1,0,0,
			0,1,1,1,1,1,1,0,
			0,0,1,0,0,1,0,0,
			0,0,0,0,0,0,0,0, 
		},
		// '$'
		{	
			0,0,0,1,0,0,0,0,
			0,0,0,1,1,0,0,0,
			0,0,1,0,0,0,0,0,
			0,0,1,1,0,0,0,0,
			0,0,0,1,1,0,0,0,
			0,0,0,0,1,0,0,0,
			0,0,1,1,0,0,0,0,
			0,0,0,1,0,0,0,0, 
		},
		// 'O'
		{	
			0,0,0,0,0,0,0,0,
			0,0,1,1,0,0,0,0,
			0,1,0,0,1,0,0,0,
			1,0,0,0,0,1,0,0,
			1,0,0,0,0,1,0,0,
			0,1,0,0,1,0,0,0,
			0,0,1,1,0,0,0,0,
			0,0,0,0,0,0,0,0, 
		},
		// '='
		{	
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			1,1,1,1,1,1,1,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			1,1,1,1,1,1,1,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0, 
		},
		// '+'
		{	
			0,0,0,0,0,0,0,0,
			0,0,0,1,0,0,0,0,
			0,0,0,1,0,0,0,0,
			0,0,0,1,0,0,0,0,
			1,1,1,1,1,1,1,0,
			0,0,0,1,0,0,0,0,
			0,0,0,1,0,0,0,0,
			0,0,0,1,0,0,0,0, 
		},
		// '|'
		{	
			0,0,0,0,0,0,0,0,
			0,0,0,1,0,0,0,0,
			0,0,0,1,0,0,0,0,
			0,0,0,1,0,0,0,0,
			0,0,0,1,0,0,0,0,
			0,0,0,1,0,0,0,0,
			0,0,0,1,0,0,0,0,
			0,0,0,1,0,0,0,0, 
		},
		// '^'
		{	
			0,0,0,1,0,0,0,0,
			0,0,1,0,1,0,0,0,
			0,1,0,0,0,1,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0, 
		},
		// '.'
		{	
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,1,0,0,0,0,
			0,0,0,0,0,0,0,0, 
		},
		{	
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0, 
		},

	};

	const int numRows = height / 8;
	const int numCols = width / 8;

	if (uDataFormat == PIXEL_FORMAT_MONO8) 
	{
		U8* pFirstPixel = (U8*)pFrameData;
		const int maxTotal = 255 * 64;
		for(int row = 0; row < numRows; row++) 
		{
			for(int col = 0; col < numCols; col++) 
			{
				// Convert an 8x8 section to an ASCII char
				U8* pAsciiPixel = &pFirstPixel[row*8*width + col*8];
				U8* pPixel = pAsciiPixel;
				int total = 0;
				for(int r = 0; r < 8; r++, pPixel += (width-8)) 
				{
					for(int c = 0; c < 8; c++, pPixel++) 
					{
						total += *pPixel;
					}
				}
				// Now program in the ascii character
				int asciiCharIndex = (int) (((float)total / (float)maxTotal) * 9);
				if (9 == asciiCharIndex) 
				{
					asciiCharIndex = 8;
				}
				pPixel = pAsciiPixel;
				int bitIndex = 0;
				for(int r = 0; r < 8; r++, pPixel += (width-8)) 
				{
					for(int c = 0; c < 8; c++, pPixel++) 
					{
//						*pPixel = 255 - asciiChars[asciiCharIndex][bitIndex++] * 255;
						*pPixel = (1 - asciiChars[asciiCharIndex][bitIndex++]) * 255;
					}
				}
			}
		}
	} 
	else if (PIXEL_FORMAT_RGB24 == uDataFormat ||
             PIXEL_FORMAT_BGR24 == uDataFormat) 
	{
		RGBPixel* pFirstPixel = (RGBPixel*)pFrameData;
		const int maxColorTotal = 255 * 64;
		const int maxPixelTotal = maxColorTotal * 3;
		for(int row = 0; row < numRows; row++) 
		{
			for(int col = 0; col < numCols; col++) 
			{
				// Convert an 8x8 section of RGB24 pixels to an ASCII char
                RGBPixel* pAsciiPixel = &pFirstPixel[row*8*width + col*8];
				RGBPixel* pPixel = pAsciiPixel;
				int totals[3] = { 0, 0, 0};
				for(int r = 0; r < 8; r++, pPixel += (width-8)) 
				{
					for(int c = 0; c < 8; c++, pPixel++) 
					{
						totals[0] += pPixel->R;
						totals[1] += pPixel->G;
						totals[2] += pPixel->B;
					}
				}

				// Calculate the average colour for the region
				int total = 0;
				int colours[3];
				for(int i = 0; i < 3; i++) 
				{
					colours[i] = (int)(255.0f * ((float)totals[i] / (float)maxColorTotal));
					total += totals[i];
				}

				// Now program in the ascii character
				int asciiCharIndex = (int) (((float)total / (float)maxPixelTotal)* 9);
				if (9 == asciiCharIndex) 
				{
					asciiCharIndex = 8;
				}
				pPixel = pAsciiPixel;
				int bitIndex = 64-8;	// Remember that RGB24 has the rows flipped
				for(int r = 0; r < 8; r++, pPixel += (width-8), bitIndex -= 8*2) 
				{
					for(int c = 0; c < 8; c++, pPixel++, bitIndex++) 
					{
						int bit = asciiChars[asciiCharIndex][bitIndex];
						if (bit) 
						{
							pPixel->R = colours[0];
							pPixel->G = colours[1];
							pPixel->B = colours[2];
						} 
						else 
						{
							// Colour shows up best against a black background
							pPixel->R = 0;
							pPixel->G = 0;
							pPixel->B = 0;
						}
						;
					}
				}
			}
		}
	}
	return 0;
}


PXLAPI_CALLBACK(Threshold)
{
	const int decX = static_cast<int>(pFrameDesc->PixelAddressingValue.fHorizontal);
	const int decY = static_cast<int>(pFrameDesc->PixelAddressingValue.fVertical);
    const int hdrWidth = pFrameDesc->HDRInfo.uMode == FEATURE_GAIN_HDR_MODE_INTERLEAVED ? 
        static_cast<int>(pFrameDesc->Roi.fWidth*2.0) : static_cast<int>(pFrameDesc->Roi.fWidth); // Bugzilla.2059
	const int width = DEC_SIZE(hdrWidth, decX);
	const int height = DEC_SIZE(static_cast<int>(pFrameDesc->Roi.fHeight), decY);
	const int numPixels = width * height;

	if (uDataFormat == PIXEL_FORMAT_MONO8) {
		U8* pPixel = (U8*)pFrameData;
		U8* pLastPixel = &pPixel[numPixels];
		while(pPixel != pLastPixel) 
		{
			U8 value = *pPixel;
			value = (value < 128) ? 0 : 255;
			*pPixel = value;
			pPixel++;
		}
	} else if (uDataFormat == PIXEL_FORMAT_RGB24 ||
               uDataFormat == PIXEL_FORMAT_BGR24)	{
		RGBPixel* pPixel = (RGBPixel*)pFrameData;
		RGBPixel* pLastPixel = &pPixel[numPixels];
		while(pPixel != pLastPixel) 
		{
			// We don't really need to descern between Red and blue pixels for RGB24 vs BGR24
            int total = pPixel->R + pPixel->G + pPixel->B;
			int newValue = (total < 128*3) ? 0 : 255;
			pPixel->R = newValue;
			pPixel->G = newValue;
			pPixel->B = newValue;
			pPixel++;
		}
    } else if (PIXEL_FORMAT_YUV422 == uDataFormat) { // Bugzilla.2196
		U8* pPixelComponent = ((U8*)pFrameData) + 1; // Y component is in the MSB
		U8* pLastPixelComponent = &pPixelComponent[(numPixels-1)*2];
        for (;pPixelComponent <= pLastPixelComponent; pPixelComponent+=2) 
        {
			U8 value = *pPixelComponent;
			value = (value < 128) ? 0 : 255;
			*pPixelComponent = value;
		}
    } else {
		return ApiInvalidParameterError;
	}

	return 0;
	
}


PXLAPI_CALLBACK(HistogramEqualization)
{
	int decX = static_cast<int>(pFrameDesc->PixelAddressingValue.fHorizontal);
	int decY = static_cast<int>(pFrameDesc->PixelAddressingValue.fVertical);
    int hdrWidth = pFrameDesc->HDRInfo.uMode == FEATURE_GAIN_HDR_MODE_INTERLEAVED ? 
        static_cast<int>(pFrameDesc->Roi.fWidth*2.0) : static_cast<int>(pFrameDesc->Roi.fWidth); // Bugzilla.2059
	int width = DEC_SIZE(hdrWidth, decX);
	int height = DEC_SIZE(static_cast<int>(pFrameDesc->Roi.fHeight), decY);
	int nPixels = width * height;

	if (uDataFormat == PIXEL_FORMAT_MONO8)
	{
		const int NUM_PIXEL_VALUES = 256;
		U8* pData = static_cast<U8*>(pFrameData);
		std::vector<int> map(NUM_PIXEL_VALUES, 0);
		int i;
		for (i = 0; i < nPixels; i++) 
		{
			++map[pData[i]];
		}
		for (i = 1; i < NUM_PIXEL_VALUES; i++) 
		{
			map[i] += map[i-1];
		}
		for (i = 0; i < NUM_PIXEL_VALUES; i++)
		{
			map[i] = (NUM_PIXEL_VALUES-1) * map[i] / nPixels;	
		}

		for (i = 0; i < nPixels; i++)
		{
			pData[i] = map[pData[i]];
		}
	}
	else if (uDataFormat == PIXEL_FORMAT_RGB24 ||
             uDataFormat == PIXEL_FORMAT_BGR24)
	{
		// Equalize the Y part of YUV data, then convert back to RGB.
		// Remember that PIXEL_FORMAT_RGB24 really means that the data is
		// in "BGR" format - that is, the first byte of each triplet is
		// the blue, not the red.  BGR24 obviously have a b-g-r byte order
        // as well (it's non-DIB).
		const int NUM_COLOUR_VALUES = 256;
		U8* pData = static_cast<U8*>(pFrameData);
		std::vector<int> map(NUM_COLOUR_VALUES, 0);
		int i;
		U8 yuv[3] = {0,0,0};

		for (i = 0; i < nPixels; i++)
		{
			BGRtoYUV(&pData[3*i], &yuv[0]);
			map[yuv[0]]++;
		}

		for (i = 1; i < NUM_COLOUR_VALUES; i++) 
		{
			map[i] += map[i-1];
		}

		for (i = 0; i < NUM_COLOUR_VALUES; i++) 
		{
			map[i] = (NUM_COLOUR_VALUES-1) * map[i] / nPixels;	
		}

		for (i = 0; i < nPixels; i++) 
		{
			BGRtoYUV(&pData[3*i], &yuv[0]);
			yuv[0] = map[yuv[0]];
			YUVtoBGR(&yuv[0], &pData[3*i]);
		}
	}
	else if (uDataFormat == PIXEL_FORMAT_MONO16)
	{
		// We work only with 10 bits. 
		// Pixels with more bits will be truncated.
		const int NUM_PIXEL_VALUES = 1024;
		U16* pData = static_cast<U16*>(pFrameData);
		std::vector<int> map(NUM_PIXEL_VALUES, 0);
		int i;
		for (i = 0; i < nPixels; i++) 
		{
			++map[ DCAM16_TO_TENBIT(pData[i]) ];
		}
		for (i = 1; i < NUM_PIXEL_VALUES; i++)
		{
			map[i] += map[i-1];
		}

		for (i = 0; i < NUM_PIXEL_VALUES; i++)
		{
			map[i] = (NUM_PIXEL_VALUES-1) * map[i] / nPixels;	
		}
		for (i = 0; i < nPixels; i++)
		{
			pData[i] = TENBIT_TO_DCAM16( map[ DCAM16_TO_TENBIT(pData[i]) ] );
		}
	}
	else if (uDataFormat == PIXEL_FORMAT_MONO12_PACKED)
	{
		// as per Design Notes above -- we only use the MS 8 bits of data
		const int NUM_PIXEL_VALUES = 256;
		int nPixelsPlusHalf = nPixels + nPixels/2;
		U8* pData = static_cast<U8*>(pFrameData);
		std::vector<int> map(NUM_PIXEL_VALUES, 0);
		int i;
		for (i = 0; i < nPixelsPlusHalf; i++) 
		{
			if (i % 3 == 1) continue;
			++map[pData[i]];
		}
		for (i = 1; i < NUM_PIXEL_VALUES; i++) 
		{
			map[i] += map[i-1];
		}
		for (i = 0; i < NUM_PIXEL_VALUES; i++)
		{
			map[i] = (NUM_PIXEL_VALUES-1) * map[i] / nPixels;	
		}

		for (i = 0; i < nPixelsPlusHalf; i++)
		{
			if (i % 3 == 1) continue;
			pData[i] = map[pData[i]];
		}
	}
	else if (uDataFormat == PIXEL_FORMAT_MONO12_PACKED_MSFIRST)
	{
		// as per Design Notes above -- we only use the MS 8 bits of data
		const int NUM_PIXEL_VALUES = 256;
		int nPixelsPlusHalf = nPixels + nPixels/2;
		U8* pData = static_cast<U8*>(pFrameData);
		std::vector<int> map(NUM_PIXEL_VALUES, 0);
		int i;
		for (i = 0; i < nPixelsPlusHalf; i++) 
		{
			if (i % 3 == 2) continue;
			++map[pData[i]];
		}
		for (i = 1; i < NUM_PIXEL_VALUES; i++) 
		{
			map[i] += map[i-1];
		}
		for (i = 0; i < NUM_PIXEL_VALUES; i++)
		{
			map[i] = (NUM_PIXEL_VALUES-1) * map[i] / nPixels;	
		}

		for (i = 0; i < nPixelsPlusHalf; i++)
		{
			if (i % 3 == 2) continue;
			pData[i] = map[pData[i]];
		}
	}
	else if (uDataFormat == PIXEL_FORMAT_MONO10_PACKED_MSFIRST)
	{
		// as per Design Notes above -- we only use the MS 8 bits of data
		const int NUM_PIXEL_VALUES = 256;
		int nPixelsPlusQuarter = nPixels + nPixels/4;
		U8* pData = static_cast<U8*>(pFrameData);
		std::vector<int> map(NUM_PIXEL_VALUES, 0);
		int i;
		for (i = 0; i < nPixelsPlusQuarter; i++) 
		{
			if (i % 5 == 4) continue;
			++map[pData[i]];
		}
		for (i = 1; i < NUM_PIXEL_VALUES; i++) 
		{
			map[i] += map[i-1];
		}
		for (i = 0; i < NUM_PIXEL_VALUES; i++)
		{
			map[i] = (NUM_PIXEL_VALUES-1) * map[i] / nPixels;	
		}

		for (i = 0; i < nPixelsPlusQuarter; i++)
		{
			if (i % 5 == 4) continue;
			pData[i] = map[pData[i]];
		}
	}
	else
	{
		return ApiInvalidParameterError;
	}

	return ApiSuccess;
}


/**
* Class:   BmpOverlayObj
* Purpose: Used to share data safely between the main thread and the callback
*          thread. This is a reference-counted object
*/
class BmpOverlayObj
{
public:
	BmpOverlayObj(CBitmap* pbmp)
		: m_pBmp(pbmp), m_refcount(1)
	{
	}
	CBitmap* GetBitmap() { return m_pBmp; }
	void Lock()
	{
		InterlockedIncrement(&m_refcount);
	}
	void Unlock()
	{
		InterlockedDecrement(&m_refcount);
		if (m_refcount <= 0)
		{
			m_pBmp->DeleteObject();
			m_pBmp->Detach();
			delete m_pBmp;
			delete this;
		}
	}
private:
	long m_refcount;
	CBitmap* m_pBmp;
};


PXLAPI_CALLBACK(BitmapOverlay)
{
	BmpOverlayObj* pboo = static_cast<BmpOverlayObj*>(pContext);
	if (pboo == NULL || pboo->GetBitmap() == NULL) {
		return ApiInvalidParameterError;
	}

	pboo->Lock();

	BITMAP bmp;
	if (0 == pboo->GetBitmap()->GetBitmap(&bmp))
	{
		pboo->Unlock();
		return ApiInvalidParameterError;
	}

	int decX = static_cast<int>(pFrameDesc->PixelAddressingValue.fHorizontal);
	int decY = static_cast<int>(pFrameDesc->PixelAddressingValue.fVertical);
	U8* pData = static_cast<U8*>(pFrameData);
    int hdrWidth = pFrameDesc->HDRInfo.uMode == FEATURE_GAIN_HDR_MODE_INTERLEAVED ? 
        static_cast<int>(pFrameDesc->Roi.fWidth*2.0) : static_cast<int>(pFrameDesc->Roi.fWidth); // Bugzilla.2059
	int frameWidth = hdrWidth / decX;
	int frameHeight = static_cast<int>(pFrameDesc->Roi.fHeight) / decY;
	float bytesPerPixel = GetPixelFormatSize(uDataFormat);

	int nRows = min(frameHeight, bmp.bmHeight);
	int nCols = min(frameWidth, bmp.bmWidth);

	int y;

	// RGB frames are bottom-up, but MONO frames are top-down:
	bool inverted = (uDataFormat == PIXEL_FORMAT_RGB24);

	std::vector<UCHAR> buffer(sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD), 0);
	BITMAPINFO* pbmi = reinterpret_cast<BITMAPINFO*>(&buffer[0]);
	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = nCols;
	pbmi->bmiHeader.biHeight = nRows;
	pbmi->bmiHeader.biPlanes = 1;
	pbmi->bmiHeader.biBitCount = static_cast<int> (bytesPerPixel * 8.0f);
	pbmi->bmiHeader.biCompression = BI_RGB;

	// If it's a MONO8 image, initialize the bitmap's palette to a simple
	// monochrome image
	if (PIXEL_FORMAT_MONO8 == uDataFormat) {
		RGBQUAD* pPalette = (RGBQUAD*)((U32)pbmi + sizeof(BITMAPINFOHEADER));
		for(int i=0; i < 256; i++, pPalette++) {
			pPalette->rgbBlue		= i;
			pPalette->rgbGreen		= i;
			pPalette->rgbRed		= i;
			pPalette->rgbReserved	= 0;
		}
	}

	void* pBmpData = NULL;
	HBITMAP hbmp = ::CreateDIBSection(NULL, pbmi, DIB_RGB_COLORS, &pBmpData, NULL, 0);
	int pitch = (static_cast<int>(static_cast<float>(nCols) * bytesPerPixel) + 3) / 4 * 4;
	// Copy the required portion of the frame data into the Bitmap.
	for (y = 0; y < nRows; y++)
	{
		int dataRow = inverted ? (frameHeight - y - 1) : y;
		U8* pDest = (U8*)pBmpData + ((nRows-y-1) * pitch);
		U8* pSource = pData + (dataRow 
		                       * static_cast<int>(static_cast<float>(frameWidth) * bytesPerPixel)); 
		memcpy(pDest, pSource, static_cast<int>(static_cast<float>(nCols) * bytesPerPixel));
	}
	// Create DCs for the Overlay and the Bitmap
	CDC overlayDC;
	overlayDC.CreateCompatibleDC(NULL);
	overlayDC.SelectObject(pboo->GetBitmap()->m_hObject);
	CDC frameDC;
	frameDC.CreateCompatibleDC(NULL);
	frameDC.SelectObject(hbmp);
	// Now do a transparent blit of the Overlay onto the Frame. (white is the color to be transparent)
	::TransparentBlt(frameDC, 0, 0, nCols, nRows, overlayDC.m_hDC, 0, 0, nCols, nRows, RGB(0xFF,0xFF,0xFF));
	// Copy the data back into the frame.
	for (y = 0; y < nRows; y++)
	{
		int dataRow = inverted ? (frameHeight - y - 1) : y;
		U8* pSource = (U8*)pBmpData + ((nRows-y-1) * pitch);
		U8* pDest = pData + (dataRow * static_cast<int>(static_cast<float>(frameWidth) 
		                                                * bytesPerPixel)); 
		memcpy(pDest, pSource, static_cast<int>(static_cast<float>(nCols) * bytesPerPixel));
	}
	::DeleteObject(hbmp);

	pboo->Unlock();
	return ApiSuccess;
}

TCHAR const* bitmapFilterString = 
	_T("Bitmap Files (*.bmp)|*.bmp|")
	_T("All Files (*.*)|*.*||");



//
// Displaying areas of saturated pixels and pure black pixels, thereby
// highlighting areas that have lost detail because it's too dark or too bright
//
PXLAPI_CALLBACK(MaxAndMinPixels)
{
	const int decX = static_cast<int>(pFrameDesc->PixelAddressingValue.fHorizontal);
	const int decY = static_cast<int>(pFrameDesc->PixelAddressingValue.fVertical);
    const int hdrWidth = pFrameDesc->HDRInfo.uMode == FEATURE_GAIN_HDR_MODE_INTERLEAVED ? 
        static_cast<int>(pFrameDesc->Roi.fWidth*2.0) : static_cast<int>(pFrameDesc->Roi.fWidth); // Bugzilla.2059
	const int width = DEC_SIZE(hdrWidth, decX);
	const int height = DEC_SIZE(static_cast<int>(pFrameDesc->Roi.fHeight), decY);
	const int numPixels = width * height;

	// If this isn't one of the formats we support, we're out of here. 
	switch(uDataFormat) {
		case PIXEL_FORMAT_RGB24:
		case PIXEL_FORMAT_BGR24:
		case PIXEL_FORMAT_MONO8:
        case PIXEL_FORMAT_YUV422: // Bugzilla.2196
			break;
		default:
			return ApiInvalidParameterError;
	}

	unsigned int TOLERANCE = 5; // +- 5 from pure black or pure white.

	if (PIXEL_FORMAT_RGB24 == uDataFormat ||
        PIXEL_FORMAT_BGR24 == uDataFormat) {
		U8* pPixel = (U8*)pFrameData;
		U8* pLastPixel = &pPixel[(numPixels-1)*3];
		for(; pPixel <= pLastPixel; pPixel+=3) {
			U32 total = pPixel[0] + pPixel[1] + pPixel[2];
            // Both RGB24_DIB and BGR24 have the colors sequenced as b-g-r
			if (total <= (3 * (0x00 + TOLERANCE))) {
				// Make an all- or near-black pixel blue (cold)
				pPixel[0] = 0xFF; // B
				pPixel[1] = 0x00; // G
				pPixel[2] = 0x00; // R
			} else if (total >= (3 * (0xFF - TOLERANCE))) {
				// Make an all- or near-white pixel red (hot)
				pPixel[0] = 0x00; // B
				pPixel[1] = 0x00; // G
				pPixel[2] = 0xFF; // R
			}
		}
	} else if (PIXEL_FORMAT_MONO8 == uDataFormat) {
		U8* pPixel = (U8*)pFrameData;
		U8* pLastPixel = &pPixel[numPixels-1];
		for(; pPixel <= pLastPixel; pPixel++) {
			const U8 pixelVal = *pPixel;
			if (0x00 == pixelVal) {
				*pPixel = 0xFF;	// Make an all black pixel white
			}
			if (0xFF == pixelVal) {
				*pPixel = 0x00;	// Make an all white pixel black
			}
		}
    } else if (PIXEL_FORMAT_YUV422 == uDataFormat) { // Bugzilla.2196
		U8* pPixelComponent = ((U8*)pFrameData) + 1;  // The Y componnet is in MSB
		U8* pLastPixelComponent = &pPixelComponent[(numPixels-1)*2];
		for(; pPixelComponent <= pLastPixelComponent; pPixelComponent+=2) {
			const U8 pixelVal = *pPixelComponent;
			if (0x00 == pixelVal) {
				*pPixelComponent = 0xFF;	// Make an all black pixel saturated
			}
			if (0xFF == pixelVal) {
				*pPixelComponent = 0x00;	// Make a saturated pixel black
			}
		}
    }

	return ApiSuccess;

}

//
// Displaying areas of saturated pixels and pure black pixels, thereby
// highlighting areas that have lost detail because it's too dark or too bright
//
PXLAPI_CALLBACK(Negative)
{
	const int decX = static_cast<int>(pFrameDesc->PixelAddressingValue.fHorizontal);
	const int decY = static_cast<int>(pFrameDesc->PixelAddressingValue.fVertical);
    const int hdrWidth = pFrameDesc->HDRInfo.uMode == FEATURE_GAIN_HDR_MODE_INTERLEAVED ? 
        static_cast<int>(pFrameDesc->Roi.fWidth*2.0) : static_cast<int>(pFrameDesc->Roi.fWidth); // Bugzilla.2059
	const int width = DEC_SIZE(hdrWidth, decX);
	const int height = DEC_SIZE(static_cast<int>(pFrameDesc->Roi.fHeight), decY);
	const int numPixels = width * height;

	// If this isn't one of the formats we support, we're out of here. 
	switch(uDataFormat) {
		case PIXEL_FORMAT_RGB24:
		case PIXEL_FORMAT_BGR24:
		case PIXEL_FORMAT_MONO8:
        case PIXEL_FORMAT_YUV422: // Bugzilla.2196
			break;
		default:
			return ApiInvalidParameterError;
	}

	if (PIXEL_FORMAT_RGB24 == uDataFormat ||
        PIXEL_FORMAT_BGR24 == uDataFormat) {
		U8* pPixel = (U8*)pFrameData;
		U8* pLastPixel = &pPixel[(numPixels-1)*3];
		for(; pPixel <= pLastPixel; pPixel+=3) {
			pPixel[0] = 255 - pPixel[0];
			pPixel[1] = 255 - pPixel[1];
			pPixel[2] = 255 - pPixel[2];
		}
	} else if (PIXEL_FORMAT_MONO8 == uDataFormat) {
		U8* pPixel = (U8*)pFrameData;
		U8* pLastPixel = &pPixel[numPixels-1];
		for(; pPixel <= pLastPixel; pPixel++) {
			const U8 pixelVal = *pPixel;
			*pPixel = 255 - pixelVal;
		}
    } else if (PIXEL_FORMAT_YUV422 == uDataFormat) { // Bugzilla.2196
		U8* pPixelComponent = ((U8*)pFrameData) + 1; // The Y componet is in MSB 
		U8* pLastPixelComponent = &pPixelComponent[(numPixels-1)*2];
		for(; pPixelComponent <= pLastPixelComponent; pPixelComponent+=2) {
			const U8 pixelVal = *pPixelComponent;
			*pPixelComponent = 255 - pixelVal;
		}
    }

	return ApiSuccess;

}


//
// Displaying areas of saturated pixels and pure black pixels, thereby
// highlighting areas that have lost detail because it's too dark or too bright
//
PXLAPI_CALLBACK(Grayscale)
{
	const int decX = static_cast<int>(pFrameDesc->PixelAddressingValue.fHorizontal);
	const int decY = static_cast<int>(pFrameDesc->PixelAddressingValue.fVertical);
    const int hdrWidth = pFrameDesc->HDRInfo.uMode == FEATURE_GAIN_HDR_MODE_INTERLEAVED ? 
        static_cast<int>(pFrameDesc->Roi.fWidth*2.0) : static_cast<int>(pFrameDesc->Roi.fWidth); // Bugzilla.2059
	const int width = DEC_SIZE(hdrWidth, decX);
	const int height = DEC_SIZE(static_cast<int>(pFrameDesc->Roi.fHeight), decY);
	const int numPixels = width * height;

	// If this isn't one of the formats we support, we're out of here. 
	switch(uDataFormat) {
		case PIXEL_FORMAT_RGB24:
		case PIXEL_FORMAT_BGR24:
		case PIXEL_FORMAT_MONO8:
        case PIXEL_FORMAT_YUV422: // Bugzilla.2196
			break;
		default:
			return ApiInvalidParameterError;
	}

	if (PIXEL_FORMAT_RGB24 == uDataFormat ||
        PIXEL_FORMAT_BGR24 == uDataFormat) {
		U8* pPixel = (U8*)pFrameData;
		U8* pLastPixel = &pPixel[(numPixels-1)*3];
		for(; pPixel <= pLastPixel; pPixel+=3) {
            // Both RGB24_DIB and BGR24 have the colors sequenced as b-g-r
			const float b = static_cast<float>(pPixel[0]);
			const float g = static_cast<float>(pPixel[1]);
			const float r = static_cast<float>(pPixel[2]);
			const float Y =  (0.2989f * r) + (0.5870f * g) + (0.1140f * b);
			const U8 y = static_cast<U8>(Y);
			pPixel[0] = y;
			pPixel[1] = y;
			pPixel[2] = y;
		}
    } else if (PIXEL_FORMAT_YUV422 == uDataFormat) { // Bugzilla.2196
		U8* pPixelComponent = (U8*)pFrameData;
		U8* pLastPixelComponent = &pPixelComponent[(numPixels-1)*2];
		for(; pPixelComponent <= pLastPixelComponent; pPixelComponent+=2) {
            *pPixelComponent = 0x80;  // For mono images, both the U and the V components, are 0x80 
        }
    }

	return ApiSuccess;

}


//
// A very simple temporal filter that attempts to remove
// a bit of noise from images by comparing the current image to
// the previous image. 
//
static U32 
TemporalFilterImpl(void* pFrameData, const U32 pixelFormat, FRAME_DESC const * pFrameDesc, const U8 noiseThreshold)
{
	static std::vector<U8>  s_lastFrame;
	const int decX = static_cast<int>(pFrameDesc->PixelAddressingValue.fHorizontal);
	const int decY = static_cast<int>(pFrameDesc->PixelAddressingValue.fVertical);
    const int hdrWidth = pFrameDesc->HDRInfo.uMode == FEATURE_GAIN_HDR_MODE_INTERLEAVED ? 
        static_cast<int>(pFrameDesc->Roi.fWidth*2.0) : static_cast<int>(pFrameDesc->Roi.fWidth); // Bugzilla.2059
	const int width = DEC_SIZE(hdrWidth, decX);
	const int height = DEC_SIZE(static_cast<int>(pFrameDesc->Roi.fHeight), decY);
	const int numPixels = width * height;
	const float pixelSize = GetPixelFormatSize(pixelFormat);
	const int frameSize = static_cast<int>( static_cast<float>(numPixels) * pixelSize);

	// Is this the first frame?
	if (s_lastFrame.size() != frameSize) {
		s_lastFrame.clear();
		s_lastFrame.resize(frameSize);
		memcpy(&s_lastFrame[0], (U8*)pFrameData, frameSize);
		return ApiSuccess;
	}

	// If this isn't one of the formats we support, we're out of here. 
	switch(pixelFormat) {
		case PIXEL_FORMAT_RGB24:
		case PIXEL_FORMAT_BGR24:
		case PIXEL_FORMAT_MONO8:
        case PIXEL_FORMAT_YUV422: //Bugzilla.2196
			break;
		default:
			return ApiInvalidParameterError;
	}

	// Go through the pixel values 1 by 1 looking for differences.
	// If the difference is greater than our noise threshold, use the new value and store it in the last frame.
	// If the difference is less than our noise threshold, use the old value.
	U8* pNewPixel		= (U8*)pFrameData;
	U8* pOldPixel		= &s_lastFrame[0];
	U8* pLastOldPixel	= &s_lastFrame[s_lastFrame.size()-1];

	for( ; pOldPixel <= pLastOldPixel ; pNewPixel++, pOldPixel++) {
		U16 newPixel = (U16)*pNewPixel;
		U16 oldPixel = (U16)*pOldPixel;
		U16 delta = abs(newPixel - oldPixel);
		if (delta < noiseThreshold) {
			*pNewPixel = (U8)oldPixel;
		} else {
			*pOldPixel = (U8)newPixel;
		}
	}
	return ApiSuccess;
}

//
// A very simple temporal filter that shows areas that are changing rapidly.
//
PXLAPI_CALLBACK(TemporalFilterMotion)
{
	static std::vector<U8>  s_lastFrame;
	const int decX = static_cast<int>(pFrameDesc->PixelAddressingValue.fHorizontal);
	const int decY = static_cast<int>(pFrameDesc->PixelAddressingValue.fVertical);
    const int hdrWidth = pFrameDesc->HDRInfo.uMode == FEATURE_GAIN_HDR_MODE_INTERLEAVED ? 
        static_cast<int>(pFrameDesc->Roi.fWidth*2.0) : static_cast<int>(pFrameDesc->Roi.fWidth); // Bugzilla.2059
	const int width = DEC_SIZE(hdrWidth, decX);
	const int height = DEC_SIZE(static_cast<int>(pFrameDesc->Roi.fHeight), decY);
	const int numPixels = width * height;
	const float pixelSize = GetPixelFormatSize(uDataFormat);
	const int frameSize = static_cast<int>(static_cast<float>(numPixels) * pixelSize);

	// Is this the first frame?
	if (s_lastFrame.size() != frameSize) {
		s_lastFrame.clear();
		s_lastFrame.resize(frameSize);
		memcpy(&s_lastFrame[0], (U8*)pFrameData, frameSize);
		return ApiSuccess;
	}

	// If this isn't one of the formats we support, we're out of here. 
	switch(uDataFormat) {
		case PIXEL_FORMAT_RGB24:
		case PIXEL_FORMAT_BGR24:
		case PIXEL_FORMAT_MONO8:
        case PIXEL_FORMAT_YUV422: //Bugzilla.2196
			break;
		default:
			return ApiInvalidParameterError;
	}

	// Go through the pixel values 1 by 1 looking for differences.
	U8* pNewPixel		= (U8*)pFrameData;
	U8* pOldPixel		= &s_lastFrame[0];
	U8* pLastOldPixel	= &s_lastFrame[s_lastFrame.size()-1];

	U32 numPixelsMoving =0;

	for( ; pOldPixel <= pLastOldPixel ; pNewPixel++, pOldPixel++) {
		U16 newPixel = (U16)*pNewPixel;
		U16 oldPixel = (U16)*pOldPixel;
		U16 delta = abs(newPixel - oldPixel);
		if (delta > 64) {
			*pOldPixel = (U8)newPixel;
			*pNewPixel = (U8)0xFF;
			numPixelsMoving++;
		} else {
			*pNewPixel = 0x00;
		}
	}

	float percentMoving = (float)numPixelsMoving / (float)numPixels;
	if (percentMoving > 0.001f) {
		OutputDebugString("Motion detected\n");
	}
	return ApiSuccess;
}


PXLAPI_CALLBACK(TemporalFilter_Threshold_5)
{
	return TemporalFilterImpl(pFrameData, uDataFormat, pFrameDesc, 5);
}

PXLAPI_CALLBACK(SharpnessScoreRoi)
{
	const int decX = static_cast<int>(pFrameDesc->PixelAddressingValue.fHorizontal);
	const int decY = static_cast<int>(pFrameDesc->PixelAddressingValue.fVertical);
    const int hdrWidth = pFrameDesc->HDRInfo.uMode == FEATURE_GAIN_HDR_MODE_INTERLEAVED ? 
        static_cast<int>(pFrameDesc->Roi.fWidth*2.0) : static_cast<int>(pFrameDesc->Roi.fWidth); // Bugzilla.2059
	int roiWidth = DEC_SIZE(hdrWidth, decX);
	int roiHeight = DEC_SIZE(static_cast<int>(pFrameDesc->Roi.fHeight), decY);
	int offsetX = DEC_SIZE(static_cast<int>(pFrameDesc->SharpnessScoreParams.fLeft), decX);
	int offsetY = DEC_SIZE(static_cast<int>(pFrameDesc->SharpnessScoreParams.fTop), decY);
	int width = DEC_SIZE(static_cast<int>(pFrameDesc->SharpnessScoreParams.fWidth), decX);
	int height = DEC_SIZE(static_cast<int>(pFrameDesc->SharpnessScoreParams.fHeight), decY);

    // Adjust the SS_ROI to deal with rotation
    if (pFrameDesc->Rotate.fValue != 0.0)
    {
        // At this point, RoiWidth and RoiHeight are representative of the converted image, so the rotate has already
        // been applied.  However, the SS roi information still refers to the original (on converted) image, so update
        // the values to compensate for the rotate. 
        int temp;
        if (pFrameDesc->Rotate.fValue == 90.0f)
        {
            std::swap(width, height);
            temp = offsetX;
            offsetX = roiWidth - width - offsetY;
            offsetY = temp;
        } else if (pFrameDesc->Rotate.fValue == 180.0f) {
           offsetX = roiWidth - width - offsetX; 
           offsetY = roiHeight - height - offsetY; 
        } else {
            std::swap(width, height);
            temp = offsetY;
            offsetY = roiHeight - height - offsetX;
            offsetX = temp;
        }
    }
    
    // Bugzilla.561 - Adjust the SS ROI to deal with flipping
    if (pFrameDesc->Flip.fHorizontal != 0.0)
    {
        offsetX = roiWidth - width - offsetX;
    }
    if (pFrameDesc->Flip.fVertical != 0.0)
    {
        offsetY = roiHeight - height - offsetY;
    }
    
    #define LINE_WIDTH 5 // the width of our SharpnessScore ROI box.
    
	if (uDataFormat == PIXEL_FORMAT_MONO8) 
	{
		U8* pPixel = ((U8*)pFrameData) + offsetY*roiWidth + offsetX;
		for(int row = 0; row < height; row++) 
		{
		    int col;
			if (row < LINE_WIDTH || row > (height-LINE_WIDTH))
			{
			    // top or bottom of the SSRoi -- draw a white line
			    for (col = 0; col < width; col++) *pPixel++ = 255;
			} else {
			    // Middle of the SsRoi -- draw two short white lines on either side.
			    for (col = 0; col < LINE_WIDTH; col++) *pPixel++ = 255;
			    pPixel += (width - 2*LINE_WIDTH);
			    for (col = 0; col < LINE_WIDTH; col++) *pPixel++ = 255;
			}
		    pPixel += (roiWidth-width);
		}
	} else if (PIXEL_FORMAT_RGB24 == uDataFormat ||
               PIXEL_FORMAT_BGR24 == uDataFormat) {
	    RGBPixel* pPixel;
		if (PIXEL_FORMAT_RGB24 == uDataFormat)
        {
            // RGB images are orientated from bottom up, so adjust our start box accordingly.
            pPixel = ((RGBPixel*)pFrameData) + (roiHeight-offsetY-height)*roiWidth + offsetX;;
        } else {
            pPixel = ((RGBPixel*)pFrameData) + offsetY*roiWidth + offsetX;;
        }
		for(int row = 0; row < height; row++) 
		{
		    int col;
			if (row < LINE_WIDTH || row > (height-LINE_WIDTH))
			{
			    // top or bottom of the SSRoi -- draw a green line
			    for (col = 0; col < width; col++) 
			    {
			        pPixel->R = 0;
			        pPixel->G = 255;
			        pPixel->B = 0;
			        pPixel++;
			    }
			} else {
			    // Middle of the SsRoi -- draw two short green lines on either side.
			    for (col = 0; col < LINE_WIDTH; col++) 
			    {
			        pPixel->R = 0;
			        pPixel->G = 255;
			        pPixel->B = 0;
			        pPixel++;
			    }
			    pPixel += (width - 2*LINE_WIDTH);
			    for (col = 0; col < LINE_WIDTH; col++) 
			    {
			        pPixel->R = 0;
			        pPixel->G = 255;
			        pPixel->B = 0;
			        pPixel++;
			    }
			}
		    pPixel += (roiWidth-width);
		}
	}
	return 0;
}

PXLAPI_CALLBACK(AutoRoi)
{
   const int decX = static_cast<int>(pFrameDesc->PixelAddressingValue.fHorizontal);
   const int decY = static_cast<int>(pFrameDesc->PixelAddressingValue.fVertical);
   const int hdrWidth = pFrameDesc->HDRInfo.uMode == FEATURE_GAIN_HDR_MODE_INTERLEAVED ?
      static_cast<int>(pFrameDesc->Roi.fWidth*2.0) : static_cast<int>(pFrameDesc->Roi.fWidth); // Bugzilla.2059
   int roiWidth = DEC_SIZE(hdrWidth, decX);
   int roiHeight = DEC_SIZE(static_cast<int>(pFrameDesc->Roi.fHeight), decY);
   int offsetX = DEC_SIZE(static_cast<int>(pFrameDesc->AutoROI.fLeft), decX);
   int offsetY = DEC_SIZE(static_cast<int>(pFrameDesc->AutoROI.fTop), decY);
   int width = DEC_SIZE(static_cast<int>(pFrameDesc->AutoROI.fWidth), decX);
   int height = DEC_SIZE(static_cast<int>(pFrameDesc->AutoROI.fHeight), decY);

   // Adjust the Auto_ROI to deal with rotation
   if (pFrameDesc->Rotate.fValue != 0.0)
   {
      // At this point, RoiWidth and RoiHeight are representative of the converted image, so the rotate has already
      // been applied.  However, the SS roi information still refers to the original (on converted) image, so update
      // the values to compensate for the rotate. 
      int temp;
      if (pFrameDesc->Rotate.fValue == 90.0f)
      {
         std::swap(width, height);
         temp = offsetX;
         offsetX = roiWidth - width - offsetY;
         offsetY = temp;
      }
      else if (pFrameDesc->Rotate.fValue == 180.0f) {
         offsetX = roiWidth - width - offsetX;
         offsetY = roiHeight - height - offsetY;
      }
      else {
         std::swap(width, height);
         temp = offsetY;
         offsetY = roiHeight - height - offsetX;
         offsetX = temp;
      }
   }

   // Bugzilla.561 - Adjust the Auto ROI to deal with flipping
   if (pFrameDesc->Flip.fHorizontal != 0.0)
   {
      offsetX = roiWidth - width - offsetX;
   }
   if (pFrameDesc->Flip.fVertical != 0.0)
   {
      offsetY = roiHeight - height - offsetY;
   }

   if (uDataFormat == PIXEL_FORMAT_MONO8)
   {
      U8* pPixel = ((U8*)pFrameData) + offsetY*roiWidth + offsetX;
      for (int row = 0; row < height; row++)
      {
         int col;
         if (row < LINE_WIDTH || row >(height - LINE_WIDTH))
         {
            // top or bottom of the AutoRoi -- draw a white line
            for (col = 0; col < width; col++) *pPixel++ = 255;
         }
         else {
            // Middle of the AutoRoi -- draw two short white lines on either side.
            for (col = 0; col < LINE_WIDTH; col++) *pPixel++ = 255;
            pPixel += (width - 2 * LINE_WIDTH);
            for (col = 0; col < LINE_WIDTH; col++) *pPixel++ = 255;
         }
         pPixel += (roiWidth - width);
      }
   }
   else if (PIXEL_FORMAT_RGB24 == uDataFormat ||
      PIXEL_FORMAT_BGR24 == uDataFormat) {
      RGBPixel* pPixel;
      if (PIXEL_FORMAT_RGB24 == uDataFormat)
      {
         // RGB images are orientated from bottom up, so adjust our start box accordingly.
         pPixel = ((RGBPixel*)pFrameData) + (roiHeight - offsetY - height)*roiWidth + offsetX;;
      }
      else {
         pPixel = ((RGBPixel*)pFrameData) + offsetY*roiWidth + offsetX;;
      }
      for (int row = 0; row < height; row++)
      {
         int col;
         if (row < LINE_WIDTH || row >(height - LINE_WIDTH))
         {
            // top or bottom of the AutoRoi -- draw a green line
            for (col = 0; col < width; col++)
            {
               pPixel->R = 0;
               pPixel->G = 255;
               pPixel->B = 0;
               pPixel++;
            }
         }
         else {
            // Middle of the SsRoi -- draw two short green lines on either side.
            for (col = 0; col < LINE_WIDTH; col++)
            {
               pPixel->R = 0;
               pPixel->G = 255;
               pPixel->B = 0;
               pPixel++;
            }
            pPixel += (width - 2 * LINE_WIDTH);
            for (col = 0; col < LINE_WIDTH; col++)
            {
               pPixel->R = 0;
               pPixel->G = 255;
               pPixel->B = 0;
               pPixel++;
            }
         }
         pPixel += (roiWidth - width);
      }
   }
   return 0;
}


#define REQUIRES_FILE		true
#define NO_FILE_REQUIRED	false

static CallbackDesc 
s_callbacks[] = 
{
	{
		NULL,
		NULL,
		_T("None"),
		NO_FILE_REQUIRED
	},
	{
		Negative,
		NULL,
		_T("Negative"),
		NO_FILE_REQUIRED
	},
	{
		Grayscale,
		NULL,
		_T("Grayscale"),
		NO_FILE_REQUIRED
	},
	{
		HistogramEqualization,
		NULL,
		_T("Histogram Equalization"),
		NO_FILE_REQUIRED
	},
	{
		MaxAndMinPixels,
		NULL,
		_T("Saturated and Black"),
		NO_FILE_REQUIRED
	},
	{
		Threshold,
		NULL,
		_T("Threshold (50%)"),
		NO_FILE_REQUIRED
	},
	{
		LowPassFilter_3x3,
		NULL,
		_T("Low Pass Filter (3x3)"),
		NO_FILE_REQUIRED
	},
	{
		MedianFilter_3x3,
		NULL,
		_T("Median Filter (3x3)"),
		NO_FILE_REQUIRED
	},
	{
		HighPassFilter_3x3,
		NULL,
		_T("High Pass Filter (3x3)"),
		NO_FILE_REQUIRED
	},
	{
		SobelFilter_3x3,
		NULL,
		_T("Sobel Filter (3x3)"),
		NO_FILE_REQUIRED
	},
	{
		TemporalFilter_Threshold_5,
		NULL,
		_T("Temporal Noise Filter"),
		NO_FILE_REQUIRED
	},
	{
		TemporalFilterMotion,
		NULL,
		_T("Motion Detector"),
		NO_FILE_REQUIRED
	},
	{
		BitmapOverlay,
		NULL,
		_T("Bitmap Overlay"),
		REQUIRES_FILE
	},
	{
		Ascii,
		NULL,
		_T("ASCII"),
		NO_FILE_REQUIRED
	},
	{
		SharpnessScoreRoi,
		NULL,
		_T("Sharpness Score ROI"),
		NO_FILE_REQUIRED
	},
   {
      AutoRoi,
      NULL,
      _T("Auto ROI"),
      NO_FILE_REQUIRED
   },


};

void SetCallbackComboEntries(CComboBox& cb)
{
	ClearComboBox(cb);

	const int NUM_CALLBACKS = sizeof(s_callbacks)/sizeof(s_callbacks[0]);

	for (CallbackDesc* pcb = &s_callbacks[0]; pcb < &s_callbacks[NUM_CALLBACKS]; pcb++)
	{
		int idx = cb.AddString(pcb->callbackName);
		if (idx != CB_ERR) {
			cb.SetItemDataPtr(idx, pcb);
		}
	}
	cb.SetCurSel(0);
}


/******************************************************************************
* CCallbacksGui dialog
******************************************************************************/

IMPLEMENT_DYNAMIC(CCallbacksGui, CDevAppPage)


/**
* Function: CCallbacksGui
* Purpose:  Constructor
*/
CCallbacksGui::CCallbacksGui(CPxLDevAppView* pView)
	: CDevAppPage(CCallbacksGui::IDD, pView)
{
    s_CallbackGui = this;
}


/**
* Function: ~CCallbacksGui
* Purpose:  Destructor
*/
CCallbacksGui::~CCallbacksGui()
{
}


/**
* Function: DoDataExchange
* Purpose:  
*/
void CCallbacksGui::DoDataExchange(CDataExchange* pDX)
{
	CDevAppPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CB_CAMERASELECT_COMBO, m_CameraSelectCombo);
	DDX_Control(pDX, IDC_CB_PLAY_BUTTON, m_PlayButton);
	DDX_Control(pDX, IDC_CB_PAUSE_BUTTON, m_PauseButton);
	DDX_Control(pDX, IDC_CB_STOP_BUTTON, m_StopButton);

	DDX_Control(pDX, IDC_CB_PREVIEWFUNCTION_COMBO, m_PreviewFunctionCombo);
	DDX_Control(pDX, IDC_CB_PREVIEWFILE_EDIT, m_PreviewFileEdit);
	DDX_Control(pDX, IDC_CB_PREVIEWBROWSE_BUTTON, m_PreviewBrowseButton);

	DDX_Control(pDX, IDC_CB_IMAGEFUNCTION_COMBO, m_ImageFunctionCombo);
	DDX_Control(pDX, IDC_CB_IMAGEFILE_EDIT, m_ImageFileEdit);
	DDX_Control(pDX, IDC_CB_IMAGEBROWSE_BUTTON, m_ImageBrowseButton);

	DDX_Control(pDX, IDC_CB_CLIPFUNCTION_COMBO, m_ClipFunctionCombo);
	DDX_Control(pDX, IDC_CB_CLIPFILE_EDIT, m_ClipFileEdit);
	DDX_Control(pDX, IDC_CB_CLIPBROWSE_BUTTON, m_ClipBrowseButton);

	DDX_SLIDER_SUITE(CB_SHARPNESS,Sharpness)

	DDX_Control(pDX, IDC_CB_SHARPNESSONOFF_CHECK, m_SharpnessOnOffCheck);
}


/**
* Function: MESSAGE MAP
* Purpose:  
*/
BEGIN_MESSAGE_MAP(CCallbacksGui, CDevAppPage)

	ON_CBN_DROPDOWN(IDC_CB_CAMERASELECT_COMBO, OnDropdownCameraSelectCombo)
	ON_CBN_SELCHANGE(IDC_CB_CAMERASELECT_COMBO, OnSelchangeCameraSelectCombo)
	ON_BN_CLICKED(IDC_CB_PLAY_BUTTON, OnClickPlayButton)
	ON_BN_CLICKED(IDC_CB_PAUSE_BUTTON, OnClickPauseButton)
	ON_BN_CLICKED(IDC_CB_STOP_BUTTON, OnClickStopButton)

	ON_CBN_SELCHANGE(IDC_CB_PREVIEWFUNCTION_COMBO, OnSelchangePreviewFunctionCombo)
	ON_EN_KILLFOCUS(IDC_CB_PREVIEWFILE_EDIT, OnKillFocusPreviewFileEdit)
	ON_BN_CLICKED(IDC_CB_PREVIEWBROWSE_BUTTON, OnClickPreviewBrowseButton)

	ON_CBN_SELCHANGE(IDC_CB_IMAGEFUNCTION_COMBO, OnSelchangeImageFunctionCombo)
	ON_EN_KILLFOCUS(IDC_CB_IMAGEFILE_EDIT, OnKillFocusImageFileEdit)
	ON_BN_CLICKED(IDC_CB_IMAGEBROWSE_BUTTON, OnClickImageBrowseButton)

	ON_CBN_SELCHANGE(IDC_CB_CLIPFUNCTION_COMBO, OnSelchangeClipFunctionCombo)
	ON_EN_KILLFOCUS(IDC_CB_CLIPFILE_EDIT, OnKillFocusClipFileEdit)
	ON_BN_CLICKED(IDC_CB_CLIPBROWSE_BUTTON, OnClickClipBrowseButton)

	ON_EN_KILLFOCUS(IDC_CB_SHARPNESS_EDIT, OnKillFocusSharpnessEdit)

	ON_UPDATE_COMMAND_UI(IDC_CB_PREVIEWFILE_EDIT, OnUpdatePreviewFileEdit)
	ON_UPDATE_COMMAND_UI(IDC_CB_IMAGEFILE_EDIT, OnUpdateImageFileEdit)
	ON_UPDATE_COMMAND_UI(IDC_CB_CLIPFILE_EDIT, OnUpdateClipFileEdit)

	MM_SLIDER_SUITE(CB_SHARPNESS,Sharpness)

	ON_BN_CLICKED(IDC_CB_SHARPNESSONOFF_CHECK, OnClickSharpnessOnOffCheck)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CCallbacksGui, CDevAppPage)

	EVENTSINK_SLIDER_SUITE(CCallbacksGui,CB_SHARPNESS,Sharpness)

END_EVENTSINK_MAP()


/**
* Function: GetHelpContextId
* Purpose:  
*/
int CCallbacksGui::GetHelpContextId(void)
{
	return IDH_CALLBACKS_TAB;
}


/**
* Function: OnInitDialog
* Purpose:  
*/
BOOL CCallbacksGui::OnInitDialog()
{
	CDevAppPage::OnInitDialog();

	// Fill the combo boxes
	SetCallbackComboEntries(m_PreviewFunctionCombo);
	SetCallbackComboEntries(m_ImageFunctionCombo);
	SetCallbackComboEntries(m_ClipFunctionCombo);

	m_SharpnessSlider.SetLinkedWindows(&m_SharpnessEdit, &m_SharpnessMinLabel, &m_SharpnessMaxLabel, &m_SharpnessFrame);

	return TRUE;
}


/**
* Function: SetGUIState
* Purpose:  Enable / Disable controls, according to the state of the camera.
*           Most controls cannot be used in any meaningful way until a camera
*           is initialized.
*/
void CCallbacksGui::SetGUIState(eGUIState state)
{
	// The CameraSelect and Play/Pause/Stop button states are set in the base
	// class version of this function.
	CDevAppPage::SetGUIState(state);

	CPxLCamera* cam = GetActiveCamera();

	bool enable = (state != GS_NoCamera);

	m_PreviewFunctionCombo.EnableWindow(enable);
	m_PreviewFileEdit.EnableWindow(enable);
	m_PreviewBrowseButton.EnableWindow(enable);

	m_ImageFunctionCombo.EnableWindow(enable);
	m_ImageFileEdit.EnableWindow(enable);
	m_ImageBrowseButton.EnableWindow(enable);

	m_ClipFunctionCombo.EnableWindow(enable);
	m_ClipFileEdit.EnableWindow(enable);
	m_ClipBrowseButton.EnableWindow(enable);

	ScrollToEnd(m_PreviewFileEdit);
	ScrollToEnd(m_ImageFileEdit);
	ScrollToEnd(m_ClipFileEdit);
	
    // NOTE that the ENABLE_SLIDER_SUITE macro uses the varaible 'enable' -- be sure it is set up correctly
    //
	m_SharpnessOnOffCheck.EnableWindow(enable && cam->FeatureSupportsOnOff(FEATURE_SHARPNESS));

    // Only enable it if we have a camera, and the sharpness is ON
    enable = enable && !cam->IsFeatureFlagSet(FEATURE_SHARPNESS, FEATURE_FLAG_OFF);

	ENABLE_SLIDER_SUITE(Sharpness,SHARPNESS)
	
}


/**
* Function: ConfigureControls
* Purpose:  Set up the controls on the page to customize them according to the
*           currently active camera. This involves things like setting the max
*           and min values for sliders and setting the bounds of the subwindow
*           control.
*/
void CCallbacksGui::ConfigureControls(void)
{
	SetSliderRange(m_SharpnessSlider, FEATURE_SHARPNESS);

}


/**
* Function: PopulateDefaults
* Purpose:  Put some meaningles default values into the controls, for purely
*           aesthetic reasons.
*/
void CCallbacksGui::PopulateDefaults(void)
{
	m_PreviewFunctionCombo.SetCurSel(0);
	m_ImageFunctionCombo.SetCurSel(0);
	m_ClipFunctionCombo.SetCurSel(0);
	
	SetSliderDefault(m_SharpnessSlider);	

	m_SharpnessOnOffCheck.SetCheck(BST_UNCHECKED);

}


/**
* Function: ApplyUserSettings
* Purpose:  
*/
void CCallbacksGui::ApplyUserSettings(UserSettings& settings)
{
	// We store the three file paths as user settings.
	m_PreviewFileEdit.SetWindowText(settings.GetStringSetting(USS_CALLBACK_PREVIEW_FILE));
	m_ImageFileEdit.SetWindowText(settings.GetStringSetting(USS_CALLBACK_IMAGE_FILE));
	m_ClipFileEdit.SetWindowText(settings.GetStringSetting(USS_CALLBACK_CLIP_FILE));
}


/**
* Function: StoreUserSettings
* Purpose:  
*/
void CCallbacksGui::StoreUserSettings(UserSettings& settings)
{
	settings.SetStringSetting(USS_CALLBACK_PREVIEW_FILE, WindowTextAsString(m_PreviewFileEdit));
	settings.SetStringSetting(USS_CALLBACK_IMAGE_FILE, WindowTextAsString(m_ImageFileEdit));
	settings.SetStringSetting(USS_CALLBACK_CLIP_FILE, WindowTextAsString(m_ClipFileEdit));
}


/**
* Function: GetConfigFileData
* Purpose:  This method is called when a config file is saved. We need to 
*           append to the vector any data that we want restored when the
*           config file is loaded.
*/
void CCallbacksGui::GetConfigFileData(std::vector<byte>& cfgData)
{
	// We store the selected index of each of the combo boxes, and the
	// text of the File box if required.

	// Data format:
	// ComboBox selected index - int, for Preview, Image, and Clip.
	// Number of bytes (N) of extra data for Preview callback - int.
	// N bytes of data (string).
	// Number of bytes (N) of extra data for Image callback - int.
	// N bytes of data (string).
	// Number of bytes (N) of extra data for Clip callback - int.
	// N bytes of data (string).

	int myDataSize = 3 * sizeof(int); // combo-box selected-index (3 of them).
	myDataSize += 3 * sizeof(int); // Length of associated string data (filename).

	const size_t initialCfgDataSize = cfgData.size();
	cfgData.resize(cfgData.size() + myDataSize, 0);
	// Get pointer to just past the old end of the vector.
	int* ip = reinterpret_cast<int*>(&cfgData[initialCfgDataSize]);

	*ip++ = m_PreviewFunctionCombo.GetCurSel();
	*ip++ = m_ImageFunctionCombo.GetCurSel();
	*ip++ = m_ClipFunctionCombo.GetCurSel();

	CComboBox* cbs[3] = {&m_PreviewFunctionCombo, &m_ImageFunctionCombo, &m_ClipFunctionCombo};
	CEdit* eds[] = {&m_PreviewFileEdit, &m_ImageFileEdit, &m_ClipFileEdit};
	for (int i = 0; i < 3; i++)
	{
		CallbackDesc* pCBDesc = ItemDataPtr<CallbackDesc*>(*cbs[i]);
		if (pCBDesc->requiresFile)
		{
			CString str;
			eds[i]->GetWindowText(str);
			int dataLen = str.GetLength();
			*ip++ = dataLen;
			myDataSize += dataLen;
			cfgData.resize(cfgData.size() + dataLen);
			byte* pbyte = reinterpret_cast<byte*>(ip);
			for (int j = 0; j < dataLen; j++)
			{
				*pbyte++ = str[j];
			}
			ip = reinterpret_cast<int*>(pbyte);
		}
		else
		{
			*ip++ = 0;
		}
	}
}


/**
* Function: SetFromConfigFileData
* Purpose:  This method is called when a config file is read. We will be
*           passed a pointer to the data that we previously stored in
*           GetConfigFileData. We need to read and interpret this data,
*           take whatever action is required to configure this page.
*/
void CCallbacksGui::SetFromConfigFileData(byte const* pData, int nBytes)
{
	// We stored at least:
	// 3 integers holding the indexes of the three combo boxes (preview, image capture, and video capture)
	// 3 integers holding the length of the file name associated with each combo box
	// 
	// See comment in GetConfigFileData, above, for more information about the file data format.
	if (nBytes < (2 * 3 * sizeof(int))) {
		return;  // error
	}

	
	int const* ip = reinterpret_cast<int const*>(pData);

	// Set selected index in the three combo boxes.
	CComboBox* cbs[3] = {&m_PreviewFunctionCombo, &m_ImageFunctionCombo, &m_ClipFunctionCombo};
	for (int i = 0; i < 3; i++, ip++)
	{
		int idx = *ip;
		if (idx < cbs[i]->GetCount())
		{
			cbs[i]->SetCurSel(idx); // This does not trigger an OnSelchange event. Why?
		}
	}

	// If required, set the contents of the filename edit boxes.
	CEdit* eds[] = {&m_PreviewFileEdit, &m_ImageFileEdit, &m_ClipFileEdit};
	for (int i = 0; i < 3; i++)
	{
		int len = *ip++;
		if (len > 0)
		{
			byte const* pbyte = reinterpret_cast<byte const*>(ip);
			if (pbyte + len >= pData + nBytes) // error
			{
				TRACE2("CallbacksGui::SetFromConfigFileData - Trying to read %d bytes - only %d available.", len, (pData+nBytes)-pbyte);
				return;
			}
			CString str('\0', len+1);
			for (int j = 0; j < len; j++)
			{
				str.SetAt(j, pbyte[j]);
			}
			eds[i]->SetWindowText(str);
			pbyte += len;
			ip = reinterpret_cast<int const*>(pbyte);
		}
	}

	// Make sure all the callbacks get updated.
	U32 cbtypes[3] = {OVERLAY_PREVIEW, OVERLAY_FORMAT_IMAGE, OVERLAY_FORMAT_CLIP};
	for (int i = 0; i < 3; i++)
	{
		CallbackDesc* pcb = ItemDataPtr<CallbackDesc*>(*cbs[i]);
		UpdateCallback(pcb, cbtypes[i]);
	}
}


/**
* Function: PopulateControls
* Purpose:  Make the values displayed in the controls reflect the current 
*           settings of the active camera.  Called immediately after a 
*           camera is initialized.
*/
void CCallbacksGui::PopulateControls(void)
{
	CDevAppPage::PopulateControls();

	PopulateCallbackCombo(m_PreviewFunctionCombo, GetDocument()->GetActiveCamera()->GetCallback(OVERLAY_PREVIEW));
	PopulateCallbackCombo(m_ImageFunctionCombo, GetDocument()->GetActiveCamera()->GetCallback(OVERLAY_FORMAT_IMAGE));
	PopulateCallbackCombo(m_ClipFunctionCombo, GetDocument()->GetActiveCamera()->GetCallback(OVERLAY_FORMAT_CLIP));
	
	PopulateSharpness();
	PopulateSharpnessOnOff();	
}

/**
* Function: PopulateSharpnessOnOff
* Purpose:  
*/
void CCallbacksGui::PopulateSharpnessOnOff(void)
{
	if (GetActiveCamera()->FeatureSupported(FEATURE_SHARPNESS))
	{
		try
		{
			TempVal<bool> temp(m_bAdjusting[FEATURE_SHARPNESS], true);

			bool off = GetActiveCamera()->IsFeatureFlagSet(FEATURE_SHARPNESS, FEATURE_FLAG_OFF);
			m_SharpnessOnOffCheck.SetCheck(!off);
			m_SharpnessSlider.EnableWindow(!off);
			m_SharpnessMinLabel.EnableWindow(!off);
			m_SharpnessMaxLabel.EnableWindow(!off);
			m_SharpnessEdit.EnableWindow(!off);
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


/**
* Function: FeatureChanged
* Purpose:  This method is called by the base class (CDevAppPage) as a result
*           of a call to the View class's OnUpdate method (which is called by
*           the Document class as a result of a change to its data).
*/
void CCallbacksGui::FeatureChanged(U32 featureId)
{
	switch (featureId)
	{
	case FEATURE_SHARPNESS:
		PopulateSharpness();
		PopulateSharpnessOnOff();
		break;
	default:
		// A feature that is not relevant to this page. Ignore the change.
		break;
	}
}



/**
* Function: PopulateCallbackCombo
* Purpose:  
*/
void CCallbacksGui::PopulateCallbackCombo(CComboBox& cb, PxLApiCallback callback)
{
	for (int idx = 1; idx < cb.GetCount(); ++idx)
	{
		CallbackDesc* pcb = static_cast<CallbackDesc*>(cb.GetItemDataPtr(idx));
		if (pcb != NULL && callback == pcb->callback)
		{
			cb.SetCurSel(idx);
			return;
		}
	}
	cb.SetCurSel(0);
}


/**
* Function: UpdateCallback
* Purpose:  Make the neccesary calls to the API to set the callback.
*/
void CCallbacksGui::UpdateCallback(CallbackDesc* pcb, U32 overlayFlags)
{
	// map : <camera, overlay type> => Ref-Counted holder object for CBitmap
	typedef std::map< std::pair<CPxLCamera*,U32>, BmpOverlayObj* > BmpOverlayMap_t;
	static  BmpOverlayMap_t overlayMap;

	try
	{
		if (pcb->requiresFile)
		{
			// The callback being set requires a file. At the moment, the only
			// callback implemented that uses a file is the Bitmap Overlay.
			// We need to load the bitmap specified by the user, and allocate
			// a CBitmap object that the callback function can access through
			// its pContext parameter.

			// Get the file path from the edit box.
			CString fileName;
			if (overlayFlags == OVERLAY_PREVIEW)
				m_PreviewFileEdit.GetWindowText(fileName);
			else if (overlayFlags == OVERLAY_FORMAT_IMAGE)
				m_ImageFileEdit.GetWindowText(fileName);
			else if (overlayFlags == OVERLAY_FORMAT_CLIP)
				m_ClipFileEdit.GetWindowText(fileName);
			// else : bad parameter. LoadBitmap (below) will fail.

			// If a bitmap is already loaded for the specified callback for the
			// current camera, free it.
			std::pair<CPxLCamera*,U32> key = std::make_pair(GetDocument()->GetActiveCamera(), overlayFlags);
			BmpOverlayMap_t::iterator it = overlayMap.find(key);
			if (it != overlayMap.end())
			{
				// There is already an overlay in place. Release it. If it is
				// not currently in use by the callback, it will be deleted
				// now - otherwise it will be deleted when the callback exits.
				GetActiveCamera()->SetCallback(overlayFlags, NULL, NULL);
				it->second->Unlock();
				overlayMap.erase(it);
			}

			// Load the image from file:
			HANDLE hbmp = ::LoadImage(NULL, fileName, IMAGE_BITMAP, 0, 0, 
									  LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
			if (hbmp != NULL)
			{
				CBitmap* newBmp = new CBitmap;
				newBmp->Attach(hbmp);
				overlayMap[key] = new BmpOverlayObj(newBmp); // Created with refcount of 1 - don't call Lock().
				GetActiveCamera()->SetCallback(overlayFlags, overlayMap[key], pcb->callback);
			} else {
				// Could not load the bitmap.
				GetActiveCamera()->SetCallback(overlayFlags, NULL, pcb->callback);
			}
			
		}
		else
		{
			// No file required - just set the callback.
			GetActiveCamera()->SetCallback(overlayFlags, NULL, pcb->callback);
		}
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/******************************************************************************
* Message Handlers
******************************************************************************/

/**
* Function: OnSelchangePreviewFunctionCombo
* Purpose:  
*/
void CCallbacksGui::OnSelchangePreviewFunctionCombo(void)
{
	CallbackDesc* pcb = ItemDataPtr<CallbackDesc*>(m_PreviewFunctionCombo);
	if (((CallbackDesc*)-1) == pcb) return;
	UpdateCallback(pcb, OVERLAY_PREVIEW);
}


/**
* Function: OnKillFocusPreviewFileEdit
* Purpose:  
*/
void CCallbacksGui::OnKillFocusPreviewFileEdit(void)
{
	CallbackDesc* pcb = ItemDataPtr<CallbackDesc*>(m_PreviewFunctionCombo);
	if (((CallbackDesc*)-1) == pcb) return;
	UpdateCallback(pcb, OVERLAY_PREVIEW);
}


/**
* Function: OnClickPreviewBrowseButton
* Purpose:  
*/
void CCallbacksGui::OnClickPreviewBrowseButton(void)
{
	BrowseForFile(m_PreviewFileEdit,
				  bitmapFilterString,
				  _T("bmp"));

	ScrollToEnd(m_PreviewFileEdit);

	CallbackDesc* pcb = ItemDataPtr<CallbackDesc*>(m_PreviewFunctionCombo);
	UpdateCallback(pcb, OVERLAY_PREVIEW);
}


/**
* Function: OnSelchangeImageFunctionCombo
* Purpose:  
*/
void CCallbacksGui::OnSelchangeImageFunctionCombo(void)
{
	CallbackDesc* pcb = ItemDataPtr<CallbackDesc*>(m_ImageFunctionCombo);
	if (((CallbackDesc*)-1) == pcb) return;
	UpdateCallback(pcb, OVERLAY_FORMAT_IMAGE);
}


/**
* Function: OnKillFocusImageFileEdit
* Purpose:  
*/
void CCallbacksGui::OnKillFocusImageFileEdit(void)
{
	CallbackDesc* pcb = ItemDataPtr<CallbackDesc*>(m_ImageFunctionCombo);
	if (((CallbackDesc*)-1) == pcb) return;
	UpdateCallback(pcb, OVERLAY_FORMAT_IMAGE);
}


/**
* Function: OnClickImageBrowseButton
* Purpose:  
*/
void CCallbacksGui::OnClickImageBrowseButton(void)
{
	BrowseForFile(m_ImageFileEdit,
				  bitmapFilterString,
				  _T("bmp"));

	ScrollToEnd(m_ImageFileEdit);

	CallbackDesc* pcb = ItemDataPtr<CallbackDesc*>(m_ImageFunctionCombo);
	UpdateCallback(pcb, OVERLAY_FORMAT_IMAGE);
}


/**
* Function: OnSelchangeClipFunctionCombo
* Purpose:  
*/
void CCallbacksGui::OnSelchangeClipFunctionCombo(void)
{
	CallbackDesc* pcb = ItemDataPtr<CallbackDesc*>(m_ClipFunctionCombo);
	if (((CallbackDesc*)-1) == pcb) return;
	UpdateCallback(pcb, OVERLAY_FORMAT_CLIP);
}


/**
* Function: OnKillFocusClipFileEdit
* Purpose:  
*/
void CCallbacksGui::OnKillFocusClipFileEdit(void)
{
	CallbackDesc* pcb = ItemDataPtr<CallbackDesc*>(m_ClipFunctionCombo);
	if (((CallbackDesc*)-1) == pcb) return;
	UpdateCallback(pcb, OVERLAY_FORMAT_CLIP);
}


/**
* Function: OnClickClipBrowseButton
* Purpose:  
*/
void CCallbacksGui::OnClickClipBrowseButton(void)
{
	BrowseForFile(m_ClipFileEdit,
				  bitmapFilterString,
				  _T("bmp"));

	ScrollToEnd(m_ClipFileEdit);

	CallbackDesc* pcb = ItemDataPtr<CallbackDesc*>(m_ClipFunctionCombo);
	UpdateCallback(pcb, OVERLAY_FORMAT_CLIP);
}


/******************************************************************************
* UPDATE_CMD_UI handlers
******************************************************************************/

/**
* Function: OnUpdatePreviewFileEdit
* Purpose:  
*/

void CCallbacksGui::OnUpdatePreviewFileEdit(CCmdUI* pCmdUI)
{
	CallbackDesc* pcb = ItemDataPtr<CallbackDesc*>(m_PreviewFunctionCombo);
	if ((CallbackDesc*)-1 == pcb) return;	// handle case where nothing is selected;
	m_PreviewFileEdit.EnableWindow(pcb->requiresFile);
	m_PreviewBrowseButton.EnableWindow(pcb->requiresFile);
}


/**
* Function: OnUpdateImageFileEdit
* Purpose:  
*/
void CCallbacksGui::OnUpdateImageFileEdit(CCmdUI* pCmdUI)
{
	CallbackDesc* pcb = ItemDataPtr<CallbackDesc*>(m_ImageFunctionCombo);
	if ((CallbackDesc*)-1 == pcb) return;	// handle case where nothing is selected;
	m_ImageFileEdit.EnableWindow(pcb->requiresFile);
	m_ImageBrowseButton.EnableWindow(pcb->requiresFile);
}


/**
* Function: OnUpdateClipFileEdit
* Purpose:  
*/
void CCallbacksGui::OnUpdateClipFileEdit(CCmdUI* pCmdUI)
{
	CallbackDesc* pcb = ItemDataPtr<CallbackDesc*>(m_ClipFunctionCombo);
	if ((CallbackDesc*)-1 == pcb) return;	// handle case where nothing is selected;
	m_ClipFileEdit.EnableWindow(pcb->requiresFile);
	m_ClipBrowseButton.EnableWindow(pcb->requiresFile);
}

/**
* Function: OnClickSharpnessOnOffCheck
* Purpose:  
*/
void CCallbacksGui::OnClickSharpnessOnOffCheck(void)
{
	CWaitCursor wc;
	if (!m_bAdjusting[FEATURE_SHARPNESS])
	{
		try
		{
			GetActiveCamera()->SetOnOff(FEATURE_SHARPNESS, m_SharpnessOnOffCheck.GetCheck() == BST_CHECKED);
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}

SLIDER_CONTROL_HANDLER_IMPL(CCallbacksGui,Sharpness,SHARPNESS)

void CCallbacksGui::OnEnChangeCbSharpnessEdit()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDevAppPage::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
}

void CCallbacksGui::EnableSSRoiCallback (bool enable, CCallbacksGui* pCallbackGui)
{

    // Find the SharpnesScore ROI callback
	const int NUM_CALLBACKS = sizeof(s_callbacks)/sizeof(s_callbacks[0]);
	int idx;

	// Use the first callback (None) if the user is disabling SSRoi callback, or if can't find it
	for (idx = 0; enable && idx < NUM_CALLBACKS; idx++)
	{
		if (SharpnessScoreRoi == s_callbacks[idx].callback) break;
	}
	if (idx == NUM_CALLBACKS) idx = 0;

    pCallbackGui->m_PreviewFunctionCombo.SetCurSel(idx);
    pCallbackGui->UpdateCallback(&s_callbacks[idx], OVERLAY_PREVIEW);
}

bool CCallbacksGui::SSRoiCallbackEnabled (CCallbacksGui* pCallbackGui)
{
    
    // Find the SharpnesScore ROI callback
	const int NUM_CALLBACKS = sizeof(s_callbacks)/sizeof(s_callbacks[0]);
	int idx;

	// Use the first callback (None) if the user is disabling SSRoi callback, or if can't find it
	for (idx = 0; idx < NUM_CALLBACKS; idx++)
	{
		if (SharpnessScoreRoi == s_callbacks[idx].callback) break;
	}

    return (idx < NUM_CALLBACKS &&                                    // did we find the SS callback in the list
            pCallbackGui->m_PreviewFunctionCombo.GetCurSel() == idx); // if so, is it currently selected?
}

void CCallbacksGui::EnableAutoRoiCallback(bool enable, CCallbacksGui* pCallbackGui)
{

   // Find the Auto ROI callback
   const int NUM_CALLBACKS = sizeof(s_callbacks) / sizeof(s_callbacks[0]);
   int idx;

   // Use the first callback (None) if the user is disabling AutoRoi callback, or if can't find it
   for (idx = 0; enable && idx < NUM_CALLBACKS; idx++)
   {
      if (AutoRoi == s_callbacks[idx].callback) break;
   }
   if (idx == NUM_CALLBACKS) idx = 0;

   pCallbackGui->m_PreviewFunctionCombo.SetCurSel(idx);
   pCallbackGui->UpdateCallback(&s_callbacks[idx], OVERLAY_PREVIEW);
}

bool CCallbacksGui::AutoRoiCallbackEnabled(CCallbacksGui* pCallbackGui)
{

   // Find the SharpnesScore ROI callback
   const int NUM_CALLBACKS = sizeof(s_callbacks) / sizeof(s_callbacks[0]);
   int idx;

   // Use the first callback (None) if the user is disabling AutoRoi callback, or if can't find it
   for (idx = 0; idx < NUM_CALLBACKS; idx++)
   {
      if (AutoRoi == s_callbacks[idx].callback) break;
   }

   return (idx < NUM_CALLBACKS &&                                    // did we find the SS callback in the list
      pCallbackGui->m_PreviewFunctionCombo.GetCurSel() == idx); // if so, is it currently selected?
}


