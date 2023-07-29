#ifndef PIXELINK_COM_PIXELARRAYADAPTER_H
#define PIXELINK_COM_PIXELARRAYADAPTER_H

//
// PixelArrayAdapter is a class which will provide a IPixelArray inteface
// for a simple data pointer. (e.g. U8*). 
// 
// The other service provided is that of optional bounds checking. 
// If bounds checking is enabled:
//		Setting a pixel out of bounds had no effect
//      Getting a pixel out of bounds throws a std::runtime_error exception.
//
// If bounds is disabled:
//		No guarantees are provided. It is up to the caller to ensure that
//		all accesses will access valid memory.
//		However, there is slightly less overhead when bounds checking is disabled.
//	
//
// See PixelArrayAdapterBGR888.h for a specialization for BGR888 pixels.
//

#include <windows.h>
#include <PixeLINKTypes.h>
#include "BGR888.h"
#include "IPixelArray.h"
#include <stdexcept>

#define GETPIXEL_OUT_OF_BOUNDS "GetPixel() accessing pixel out of bounds"


template <typename PixelType, bool checkBounds = true>
class PixelArrayAdapter : public IPixelArray<PixelType>
{
public:
	PixelArrayAdapter(PixelType* pData, const int width, const int height) :
		m_pData(pData), m_width(width), m_height(height), m_numPixels(width * height)
	{
	}

	virtual ~PixelArrayAdapter() { } 

	// IPixelArray interface
	virtual int			GetWidth()	const { return m_width;		}
	virtual int			GetHeight()	const { return m_height;	}
	virtual void		SetPixel(int x, int y,	const PixelType& value)	
	{
		if (checkBounds) {	if ((x<0) || (x>=m_width) || (y<0) || (y>=m_height)) {	return;	}	}
		m_pData[GetIndex(x,y)] = value;
	}
	virtual void		SetPixel(int index,		const PixelType& value)
	{
		if (checkBounds) {	if ((index<0) || (index >= m_numPixels)) {	return;	}	}
		m_pData[index] = value;
	}
	virtual PixelType	GetPixel(int x, int y)	const
	{
		return m_pData[GetIndex(x,y)];
	}
	virtual PixelType	GetPixel(int index)		const
	{
		if (checkBounds) {	if ((index<0) || (index >= m_numPixels)) { throw std::runtime_error(GETPIXEL_OUT_OF_BOUNDS); } }
		return m_pData[index];
	}


private:
	// Disable copy constructor and assignment
	PixelArrayAdapter(const PixelArrayAdapter& rhs);
	PixelArrayAdapter& operator=(const PixelArrayAdapter& rhs);

	inline int GetIndex(const int x, const int y) const { return x + y * m_width; }

	PixelType * const m_pData;	// const pointer to non-const data
	const int m_width;
	const int m_height;
	const int m_numPixels;

};


#endif // PIXELINK_COM_PIXELARRAYADAPTER_H