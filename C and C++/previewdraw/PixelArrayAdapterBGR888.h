#ifndef PIXELINK_COM_PIXELARRAYADAPTERBGR888_H
#define PIXELINK_COM_PIXELARRAYADAPTERBGR888_H


//////////////////////////////////////////////////////////////////////////////
// BGR888 specialization 
//
// The preview image for a color camera has the same pixel layout as a BMP.
// i.e. the pixels are BGR888, and the rows are inverted.
//
//


#include "PixelArrayAdapter.h"
#include "BGR888.h"

//
#define BGR888_PIXEL_INDEX(x,y)        (((m_height- (y) - 1) * m_width) + (x))

template<>
void
PixelArrayAdapter<BGR888,false>::SetPixel(const int x, const int y,
const BGR888& value)
{
    m_pData[BGR888_PIXEL_INDEX(x,y)] = value;
}

template<>
void
PixelArrayAdapter<BGR888,false>::SetPixel(const int index, const BGR888& value)
{
	// Reverse-engineer the original x and y
	const int x = index % m_width;
	const int y = index / m_width;
	m_pData[BGR888_PIXEL_INDEX(x,y)] = value;
}


template<>
BGR888
PixelArrayAdapter<BGR888,false>::GetPixel(const int x, const int y) const
{
	return m_pData[BGR888_PIXEL_INDEX(x,y)];
}

template<>
BGR888
PixelArrayAdapter<BGR888,false>::GetPixel(const int index) const
{
	return m_pData[index];
}


//////////////////////////////////////////////////////////////////////////////
// BGR888 specialization - With bounds checking
//

template<>
void
PixelArrayAdapter<BGR888,true>::SetPixel(const int x, const int y, const
BGR888& value)
{
	if ((x >= 0) && (x < m_width) && (y >= 0) && (y < m_height)) {
		m_pData[BGR888_PIXEL_INDEX(x,y)] = value;
	} else {
		// We're trying to draw out of bounds, so do nothing
	}
}

template<>
void
PixelArrayAdapter<BGR888,true>::SetPixel(const int index, const BGR888& value)
{
	// Reverse-engineer the original x and y
	const int x = index % m_width;
	const int y = index / m_width;
	if ((x >= 0) && (x < m_width) && (y >= 0) && (y < m_height)) {
		m_pData[BGR888_PIXEL_INDEX(x,y)] = value;
	} else {
		// We're trying to draw out of bounds, so do nothing
	}
}

template<>
BGR888
PixelArrayAdapter<BGR888,true>::GetPixel(const int x, const int y) const
{
	if ((x >= 0) && (x < m_width) && (y >= 0) && (y < m_height)) {
		return m_pData[BGR888_PIXEL_INDEX(x,y)];
	} else {
		throw std::runtime_error(GETPIXEL_OUT_OF_BOUNDS);
	}
}

template<>
BGR888
PixelArrayAdapter<BGR888,true>::GetPixel(const int index) const
{
	// Reverse-engineer the original x and y
	const int x = index % m_width;
	const int y = index / m_width;
	if ((x >= 0) && (x < m_width) && (y >= 0) && (y < m_height)) {
		return m_pData[BGR888_PIXEL_INDEX(x,y)];
	} else {
		throw std::runtime_error(GETPIXEL_OUT_OF_BOUNDS);
	}
}



#endif // PIXELINK_COM_PIXELARRAYADAPTERBGR888_H