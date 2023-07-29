

#ifndef PIXELINK_COM_IPIXELARRAY_H
#define PIXELINK_COM_IPIXELARRAY_H

//
// Interface to a pixel array which is accessable as a 2D image.
//
//


template <typename PixelType>
class IPixelArray
{
public:
	IPixelArray() {}
	virtual ~IPixelArray() {} 

	virtual int GetWidth()	const = 0;
	virtual int GetHeight() const = 0;

	virtual void SetPixel(int x, int y, const PixelType& value)     = 0;
	virtual void SetPixel(int index,	const PixelType& value)		= 0;

	virtual PixelType	GetPixel(int x, int y)  const = 0;
	virtual PixelType   GetPixel(int index)     const = 0;

};

#endif // PIXELINK_COM_IPIXELARRAY_H
