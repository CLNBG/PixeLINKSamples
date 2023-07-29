#ifndef PIXELINK_COM_PIXELINKARRAYUTILS_H
#define PIXELINK_COM_PIXELINKARRAYUTILS_H

// 
// Some IPixelArray utilities.
//
//

#include "IPixelArray.h"
#include "BGR888.h"

template <typename PixelType>	PixelType GetMean(const IPixelArray<PixelType>& pixelArray, int x, int y, int width, int height);

template <typename PixelType>	PixelType GetMean(const IPixelArray<PixelType>& pixelArray)
{
	return GetMean<PixelType>(pixelArray, 0, 0, pixelArray.GetWidth(), pixelArray.GetHeight());
}

U8
GetMean(const IPixelArray<U8>& pixelArray, const int x0, const int y0, const int width, const int height)
{
	U32 total = 0;
	const int y1 = y0 + height - 1;
	const int x1 = x0 + width  - 1;

	for(int y = y0; y <= y1; y++) {
		for(int x = x0; x < x1; x++) {
			total += pixelArray.GetPixel(x,y);
		}
	}
	const int numPixels = width * height;
	const U32 mean = total / numPixels;
	return (U8) mean;
}

template<>
BGR888
GetMean(const IPixelArray<BGR888>& pixelArray, const int x0, const int y0, const int width, const int height)
{
	U32 totalBlu = 0;
	U32 totalGre = 0;
	U32 totalRed = 0;

	const int y1 = y0 + height - 1;
	const int x1 = x0 + width  - 1;

	for(int y = y0; y <= y1; y++) {
		for(int x = x0; x < x1; x++) {
			BGR888 pixel(pixelArray.GetPixel(x,y));
			totalBlu	+= pixel.m_blue;
			totalGre	+= pixel.m_green;
			totalRed	+= pixel.m_red;
		}
	}

	const int numPixels = width * height;
	BGR888 mean(
		(U8)((double)totalBlu / (double)numPixels), 
		(U8)((double)totalGre / (double)numPixels), 
		(U8)((double)totalRed / (double)numPixels));
	return mean;

}

#endif // PIXELINK_COM_PIXELINKARRAYUTILS_H