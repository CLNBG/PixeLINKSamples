#ifndef PIXELINK_COM_GRAPHICSENGINE_H
#define PIXELINK_COM_GRAPHICSENGINE_H

//
// An extremely simple graphics engine for drawing into any object supporting IPixelArray.
// All the algorithms here use simple integer arithmetic.
//
// NOTE: This class does not do bounds checking. 
// If you provide an IPixelArray with a width and height of 10, but then
// ask for a line from (0,0) to (100,100), you're in trouble, the engine will
// attempt to draw the entire line. 
//
// However, if you use a PixelArrayAdapter with bounds checking, you'll be ok. 
//
//

#include "IPixelArray.h"

template <typename PixelType>
class GraphicsEngine
{
public:
	GraphicsEngine(IPixelArray<PixelType>& pixelArray) :
		m_pixelArray(pixelArray)
	{
	}

	IPixelArray<PixelType>& GetPixelArray() const { return m_pixelArray; }


	//
	//  Draw a line using the Bresenham algorithm.
	//
	void	
	DrawLine(int x0, int y0, int x1, int y1, const PixelType& value)
	{
		int dy = y1 - y0;
		int dx = x1 - x0;
		int stepx, stepy;
		const int width = m_pixelArray.GetWidth();

		if (dy < 0) { dy = -dy;  stepy = -width; }	else { stepy = width; }
		if (dx < 0) { dx = -dx;  stepx = -1; }			else { stepx = 1; }
		dy <<= 1;
		dx <<= 1;

		y0 *= width;
		y1 *= width;
		m_pixelArray.SetPixel(x0+y0, value);
		if (dx > dy) {
			int fraction = dy - (dx >> 1);
			while (x0 != x1) {
				if (fraction >= 0) {
					y0 += stepy;
					fraction -= dx;
				}
				x0 += stepx;
				fraction += dy;
				m_pixelArray.SetPixel(x0+y0, value);
			}
		} else {
			int fraction = dx - (dy >> 1);
			while (y0 != y1) {
				if (fraction >= 0) {
					x0 += stepx;
					fraction -= dy;
				}
				y0 += stepy;
				fraction += dx;
				m_pixelArray.SetPixel(x0+y0, value);
			}
		}
	}

	//
	// Draw a circle using the midpoint circle algorithm.
	//
	void	
	DrawCircle(const int x0, const int y0, const int radius, const PixelType& value)
	{
		int f = 1 - radius;
		int ddF_x = 1;
		int ddF_y = -2 * radius;
		int x = 0;
		int y = radius;
	 
		// Draw the simplest points first
		m_pixelArray.SetPixel(x0, y0 + radius, value);
		m_pixelArray.SetPixel(x0, y0 - radius, value);
		m_pixelArray.SetPixel(x0 + radius, y0, value);
		m_pixelArray.SetPixel(x0 - radius, y0, value);
	 
		while(x < y) {
			if(f >= 0) {
				y--;
				ddF_y += 2;
				f += ddF_y;
			}
			x++;
			ddF_x += 2;
			f += ddF_x;    

			m_pixelArray.SetPixel(x0 + x, y0 + y, value);
			m_pixelArray.SetPixel(x0 - x, y0 + y, value);
			m_pixelArray.SetPixel(x0 + x, y0 - y, value);
			m_pixelArray.SetPixel(x0 - x, y0 - y, value);
			m_pixelArray.SetPixel(x0 + y, y0 + x, value);
			m_pixelArray.SetPixel(x0 - y, y0 + x, value);
			m_pixelArray.SetPixel(x0 + y, y0 - x, value);
			m_pixelArray.SetPixel(x0 - y, y0 - x, value);
		}
	}

	//
	// Draw an ellipse using a fast Bresenham type algorithm.
	// http://homepage.smc.edu/kennedy_john/BELIPSE.PDF
	//
	void	
	DrawEllipse(const int x0, const int y0, const int xRadius, const int yRadius, const PixelType& value)
	{
		const int twoASquared = 2 * xRadius * xRadius;
		const int twoBSquared = 2 * yRadius * yRadius;
		int x = xRadius;
		int y = 0;
		int xChange = yRadius * yRadius * (1 - 2 * xRadius);
		int yChange = xRadius * xRadius;
		int ellipseError = 0;
		int stoppingX = twoBSquared * xRadius;
		int stoppingY = 0;

		while(stoppingX >= stoppingY) {
			Plot4EllipsePoints(x0, y0, x,y, value);
			y++;
			stoppingY += twoASquared;
			ellipseError += yChange;
			yChange += twoASquared;
			if ((2 * ellipseError + xChange) > 0) {
				x--;
				stoppingX -= twoBSquared;
				ellipseError += xChange;
				xChange += twoBSquared;
			}
		}

		// First point set is done;
		// Start the second set of points
		x = 0;
		y = yRadius;
		xChange = yRadius * yRadius;
		yChange = xRadius * xRadius * (1 - 2 * yRadius);
		ellipseError = 0;
		stoppingX = 0;
		stoppingY = twoASquared * yRadius;
		while( stoppingX <= stoppingY) {
			Plot4EllipsePoints(x0, y0, x,y, value);
			x++;
			stoppingX += twoBSquared;
			ellipseError += xChange;
			xChange += twoBSquared;
			if ((2 * ellipseError + yChange) > 0) {
				y--;
				stoppingY -= twoASquared;
				ellipseError += yChange;
				yChange += twoASquared;
			}
		}
	}


	//
	// Draw a rectangle with the upper left corner at (x0,y0)
	//
	//
	void	
	DrawRect(const int x0, const int y0, const int width, const int height, const PixelType& value)
	{
		const int x1 = x0 + width - 1;
		const int y1 = y0 + height - 1;

		DrawLine(x0,  y0,    x1, y0,	value); // top line
		DrawLine(x1,  y0+1,  x1, y1,	value); // right side
		DrawLine(x1-1,y1,    x0, y1,	value); // bottom;
		DrawLine(x0  ,y1-1,  x0, y0+1,	value); // left side
	}

	//
	// Draw a solid rectangle with the upper left corner at (x0,y0)
	//
	void	
	DrawSolidRect(const int x0, const int y0, const int width, const int height, const PixelType& value)
	{
		const int y1 = y0 + height - 1;
		const int x1 = x0 + width - 1;

		for(int y = y0; y <= y1; y++) {
			DrawLine(x0,y, x1, y, value);
		}
	}

private:

	//
	// Helper routine for DrawEllipse
	//
	void	
	Plot4EllipsePoints(const int x0, const int y0, const int x, const int y, const PixelType& value)
	{
		m_pixelArray.SetPixel(x0 + x, y0 + y, value);
		m_pixelArray.SetPixel(x0 - x, y0 + y, value);
		m_pixelArray.SetPixel(x0 - x, y0 - y, value);
		m_pixelArray.SetPixel(x0 + x, y0 - y, value);
	}

private:
	// Disable copy constructor and assignment
	GraphicsEngine(const GraphicsEngine&);
	GraphicsEngine& operator=(const GraphicsEngine& rhs);

	// Member data
	IPixelArray<PixelType>& m_pixelArray;

};

#endif // PIXELINK_COM_GRAPHICSENGINE_H

