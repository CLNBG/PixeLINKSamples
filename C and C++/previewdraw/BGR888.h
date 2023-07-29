#ifndef PIXELINK_COM_BGR888_H
#define PIXELINK_COM_BGR888_H

//
// Color cameras provide preview data to the preview callback in a format similar to
// that used by BMPs. i.e. the image data is an array of pixels, but: 
// 1) The pixels are 24 bits, 8 bits of blue, 8 bits of green, then 8 bits of red. (BGR888)
// 2) The rows of the image are inverted in memory. 
//    i.e. The first pixel in memory is the first pixel of the last row. 
//

#include <windows.h>
#include <PixeLINKTypes.h>

class BGR888
{
public:
	BGR888(U8 blue, U8 green, U8 red) :
		m_blue(blue), m_green(green), m_red(red)
	{
	}

	BGR888(U8 greyValue) :
		m_blue(greyValue), m_green(greyValue), m_red(greyValue)
	{
	}

	U8 m_blue;
	U8 m_green;
	U8 m_red;
};

#endif /* PIXELINK_COM_BGR888_H */