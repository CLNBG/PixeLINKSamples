
#include <PixeLINKApi.h>
#include "PixelArrayAdapter.h"
#include "PixelArrayAdapterBGR888.h"
#include "PixelArrayUtils.h"
#include "GraphicsEngine.h"

#include "Preview.h"


///////////////////////////////////////////////////////////////////////////////////////
//
//  All the camera code
//

static U32 __stdcall	OverlayPreviewCallback(HANDLE hCamera, void*, U32, FRAME_DESC const *, void*);
static BOOL		InitializeCameraImpl(HANDLE hCamera);

template <typename T> static void	DrawOverlay		(GraphicsEngine<T>& engine, const T& color);
template <typename T> static void	DrawImageMean	(GraphicsEngine<T>& engine);
template <typename T> static void	DrawValueBar	(GraphicsEngine<T>& engine, int x0, int y0, const T& color, U8 value);
template <typename T> static void	DrawMovingShip	(GraphicsEngine<T>& engine, int y0);
template <typename T> static void	DrawShip		(GraphicsEngine<T>& engine, const int x0, const int y0, const int shipWidth, const int shipHeight);

// Handle to the camera we're dealing with
static HANDLE s_hCamera;

BOOL
InitializeCamera()
{
	PXL_RETURN_CODE rc = PxLInitialize(0, &s_hCamera);
	if (!API_SUCCESS(rc)) { return FALSE; }

	rc = InitializeCameraImpl(s_hCamera);
	if (API_SUCCESS(rc)) {
		return TRUE;
	}

	UninitializeCamera();
	return FALSE;

}

static BOOL
InitializeCameraImpl(HANDLE hCamera)
{
	PXL_RETURN_CODE rc;

	rc = PxLSetStreamState(s_hCamera, START_STREAM);
	if (!API_SUCCESS(rc)) { return FALSE; }

	rc = PxLSetCallback(s_hCamera, CALLBACK_PREVIEW, (void*)0xAB1DE, OverlayPreviewCallback);
	if (!API_SUCCESS(rc)) { return FALSE; }

	HWND hWnd;
	rc = PxLSetPreviewState(hCamera, START_PREVIEW, &hWnd);
	if (!API_SUCCESS(rc)) { return FALSE; }

	return TRUE;
}

void
UninitializeCamera()
{
	HWND hWnd = 0;
	PxLSetPreviewState(s_hCamera, STOP_PREVIEW, &hWnd);
	PxLSetStreamState(s_hCamera, STOP_STREAM);
	PxLUninitialize(s_hCamera);
	s_hCamera = 0;
}

//
// Note that we're creating PixelArrayAdapters with bounds checking enabled.
// This is because we're drawing the ship, at times, out of bounds.
// If you can guarantee that you'll always draw within the bounds of the preview image,
// you can use a PixelArrayAdapter with no bounds checking
// e.g. PixelArrayAdapter<U8,false> pixelArray(reinterpret_cast<U8*>(pFrameData), width, height);
// 
//
static U32 __stdcall
OverlayPreviewCallback(const HANDLE hCamera, void* const pFrameData, const U32 pixelFormat, FRAME_DESC const * const pFrameDesc, void* const context)
{
	const U32 width		= (U32)(pFrameDesc->Roi.fWidth / pFrameDesc->Decimation.fValue);
	const U32 height	= (U32)(pFrameDesc->Roi.fHeight / pFrameDesc->Decimation.fValue);

	
	if (PIXEL_FORMAT_MONO8 == pixelFormat) {
		PixelArrayAdapter<U8> pixelArray(reinterpret_cast<U8*>(pFrameData), width, height);
		const U8 color = 0xFF;
		GraphicsEngine<U8> engine(pixelArray);
		DrawOverlay(engine, color);
	} else {
		PixelArrayAdapter<BGR888> pixelArray(reinterpret_cast<BGR888*>(pFrameData), width, height);
		const BGR888 color(0x00, 0x00, 0xFF);
		GraphicsEngine<BGR888> engine(pixelArray);
		DrawOverlay(engine, color);
	}

	return 0;
}

//
// Draw a crosshair.
// Draw a bar in the upper left that shows the average pixel value
// Draw a bar in the upper right that shows the centre pixel value
//

template<typename T>
static void 
DrawOverlay(GraphicsEngine<T>& engine, const T& color)
{
	const int height = engine.GetPixelArray().GetHeight();
	const int width  = engine.GetPixelArray().GetWidth();
	const int midY = height / 2;
	const int midX = width / 2;

	// Draw the ship first so that it'll be overdrawn by the scope.
	DrawMovingShip(engine, midY);

	const int scopeRadius = min (height / 4, width / 4);
	engine.DrawCircle(midX, midY, scopeRadius + 0, color);
	engine.DrawCircle(midX, midY, scopeRadius + 1, color);
	engine.DrawCircle(midX, midY, scopeRadius + 2, color);
	engine.DrawCircle(midX, midY, scopeRadius + 3, color);

	// Horizontal line with break in the middle
	const int gap = scopeRadius / 8;
	const int bar = gap / 2;
	engine.DrawLine(0, midY,			midX - gap, midY, color);
	engine.DrawLine(midX + gap, midY,	width-1, midY, color);
	engine.DrawSolidRect(0, midY-bar, bar*4, bar*2, color);
	engine.DrawSolidRect(width-bar*4, midY-bar, bar*4, bar*2, color);

	// Vertical line with break in the middle
	engine.DrawLine(midX, 0,			midX, midY - gap, color);
	engine.DrawLine(midX, midY+gap,		midX, height-1, color);
	engine.DrawSolidRect(midX-bar, 0, bar*2, bar*4, color);
	engine.DrawSolidRect(midX-bar, height-bar*4, bar*2, bar*4, color);

	const int gratSpacing = scopeRadius / 8;
	const int gratWidth = gratSpacing / 2; // distance above AND below the line

	// Horizontal graticules (left side, then right side)
	// Only draw graticules if there's enough room for them
	if (gratSpacing > 0) {
		for(int x = (midX - scopeRadius); x < midX; x += gratSpacing ) {
			engine.DrawLine(x, midY-gratWidth, x, midY+gratWidth, color);
		}
		for(int x = (midX + gratSpacing); x <= (midX + scopeRadius); x += gratSpacing ) {
			engine.DrawLine(x, midY-gratWidth, x, midY+gratWidth, color);
		}

		// Vertical graticules (top, then bottom)
		for(int y = (midY - scopeRadius); y < midY ; y += gratSpacing) {
			engine.DrawLine(midX-gratWidth, y, midX+gratWidth, y, color);
		}
		for(int y = (midY + gratSpacing); y < (midY + scopeRadius) ; y += gratSpacing) {
			engine.DrawLine(midX-gratWidth, y, midX+gratWidth, y, color);
		}
	}

	// Diagonal lines
	const int diagGap = 5;
	engine.DrawLine(midX - scopeRadius, midY - scopeRadius,	midX - diagGap, midY - diagGap, color);
	engine.DrawLine(midX + scopeRadius, midY - scopeRadius,	midX + diagGap, midY - diagGap, color);
	engine.DrawLine(midX - scopeRadius, midY + scopeRadius,	midX - diagGap, midY + diagGap, color);
	engine.DrawLine(midX + scopeRadius, midY + scopeRadius,	midX + diagGap, midY + diagGap, color);

	// Calculating the mean can be a little slow.
	// (but if you really want it, it's faster to do it using pure pointers)
	// DrawImageMean(engine);
}

template <>
static void 
DrawImageMean(GraphicsEngine<U8>& engine)
{
	// Draw the engine mean as a bar on the left
	IPixelArray<U8>& pixelArray = engine.GetPixelArray();
	const U8 imageMean = GetMean(pixelArray);
	const U8 imageMeanColor(255 - imageMean);
	DrawValueBar(engine, 50, 50, (U8)imageMeanColor, imageMean);
}

template <>
static void 
DrawImageMean(GraphicsEngine<BGR888>& engine)
{
	// Draw the engine mean as a bar on the left
	const BGR888 imageMean = GetMean(engine.GetPixelArray());
	const BGR888 red  (0x00, 0x00, 0xFF);
	BGR888 green(0x00, 0xFF, 0x00);
	BGR888 blue (0xFF, 0x00, 0x00);
	DrawValueBar(engine, 50, 50, red,	imageMean.m_red);
	DrawValueBar(engine, 70, 50, green,	imageMean.m_green);
	DrawValueBar(engine, 90, 50, blue,	imageMean.m_blue);
}

template<typename T>
static void
DrawValueBar(GraphicsEngine<T>& engine, int x0, int y0, const T& color, U8 value)
{
	// Draw the engine mean as a bar
	const int meanXOffset = x0;
	const int meanYOffset = y0;
	const int meanHeight = 255;
	const int barWidth = 10;	// actually 1/2 the width
	engine.DrawLine(meanXOffset-barWidth, meanYOffset,				meanXOffset+barWidth, meanYOffset,				color);
	engine.DrawLine(meanXOffset-barWidth, meanYOffset+meanHeight,	meanXOffset+barWidth, meanYOffset+meanHeight,	color);
	engine.DrawLine(meanXOffset, meanYOffset,	meanXOffset, meanYOffset+meanHeight, color);

	// Now draw the mean
	engine.DrawSolidRect(meanXOffset-(barWidth/2), meanYOffset + (255-value), barWidth, value, color);
}

//
// Draw a ship moving along with the waves.
// 
//
template<typename T>
static void
DrawMovingShip(GraphicsEngine<T>& engine, int y0)
{
	const int imageHeight = engine.GetPixelArray().GetHeight();
	const int imageWidth = engine.GetPixelArray().GetWidth();
	const int maxX = imageWidth - 1;
	const int shipWidth  = imageHeight/20;
	const int shipHeight = shipWidth/6;
	static int s_shipXOffset = shipWidth;
	static int s_shipYOffset = 0;
	static int s_shipYOffsetDelta = 1;
	static bool s_applyDelta = false; // apply delta change every 2nd preview so that it slows the undulation down

	// Move to the right
	s_shipXOffset++;
	if (s_shipXOffset >= (maxX)) {
		s_shipXOffset = 0;
	}

	// Move up and down every 2nd preview
	s_applyDelta = !s_applyDelta; 
	if (s_applyDelta) {
		s_shipYOffset += s_shipYOffsetDelta;
		if (s_shipYOffset == 3) {
			s_shipYOffsetDelta = -1;
		}
		if (s_shipYOffset == -3) {
			s_shipYOffsetDelta = 1;
		}
	}

	DrawShip(engine, s_shipXOffset, y0 + s_shipYOffset, shipWidth, shipHeight);



}

//
template <typename T>
static void
DrawShip(GraphicsEngine<T>& engine, const int x0, const int y0, const int shipWidth, const int shipHeight)
{
	const T grey(0x80);

	// Draw the main body
	engine.DrawSolidRect(x0-shipWidth/2, y0-shipHeight, shipWidth, shipHeight, grey);

	// Draw the bow
	for(int i=0; i < shipHeight; i++) {
		const int len = shipHeight - i;
		engine.DrawLine(x0+shipWidth/2, y0-shipHeight+i,    x0+shipWidth/2+len, y0-shipHeight+i, grey);
	}

	// Draw the superstructure
	const int ssHeight	 = shipHeight / 2; // ss = superstructure
	const int ssWidth	 = shipWidth  / 4;
	engine.DrawSolidRect(x0-ssWidth/2, y0-shipHeight-ssHeight, ssWidth, ssHeight, grey);

	// Draw a tower
	const int towerHeight = ssHeight;
	const int towerWidth  = ssWidth / 2;
	engine.DrawSolidRect(x0-towerWidth/2, y0-shipHeight-ssHeight-towerHeight, towerWidth, towerHeight, grey);

	// And a turret coming out of the tower
	const int gunLength = shipWidth/5;
	engine.DrawLine(x0, y0-shipHeight-ssHeight, x0+gunLength, y0-shipHeight-ssHeight-gunLength/3, grey);

	const int turretWidth = shipWidth / 10;
	const int turretHeight = shipHeight / 2;
	const int turretOffset = shipWidth / 3;
	engine.DrawSolidRect(x0+shipWidth/2-turretOffset, y0-shipHeight-turretHeight, turretWidth, turretHeight, grey);
	engine.DrawLine(
		x0+shipWidth/2-turretOffset+turretWidth,			y0-shipHeight-turretHeight, 
		x0+shipWidth/2-turretOffset+turretWidth+gunLength,	y0-shipHeight-turretHeight-gunLength/3, grey);
							


}
