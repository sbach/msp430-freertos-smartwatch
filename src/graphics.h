
#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <stdint.h>
#include "hal/lcd.h"
#include "Fonts.h"

#define COLOR_Black     0x00
#define COLOR_White     0xff

#define DRAW_OPT_BITWISE_OR           (0)
#define DRAW_OPT_BITWISE_NOT          (1)
#define DRAW_OPT_BITWISE_SET          (2)
#define DRAW_OPT_BITWISE_DST_NOT      (3)
#define DRAW_OPT_BITWISE_MASK         (0x03)

// Define a type Buffer to represent the LCD memory
// 96 pixels by 96 pixels (12 8-bits words) 
typedef uint8_t Buffer[LCD_VERTICAL_MAX][LCD_HORIZONTAL_MAX/8];


//*****************************************************************************
//
//! This structure defines the extents of a rectangle.  All points greater than
//! or equal to the minimum and less than or equal to the maximum are part of
//! the rectangle.
//
//*****************************************************************************
typedef struct
{
    //! The minimum X coordinate of the rectangle.
    short sXMin;

    //! The minimum Y coordinate of the rectangle.
    short sYMin;

    //! The maximum X coordinate of the rectangle.
    short sXMax;

    //! The maximum Y coordinate of the rectangle.
    short sYMax;
}
tRectangle;


// initialize le local screen buffer with the given color
void initializeDisplayBuffer(void *pvDisplayData, uint8_t ucValue);

// draw a pixel on the point defined by X, Y, the color is given in parameter
void pixelDraw(Buffer pvDisplayData, int8_t lX, int8_t lY,
			    uint8_t ulValue);


// fast method to draw a line horizontally. It needs the Y lines, the thickness
// of the line. X1 and X2 are the start and the end points of this line.
void lineHDraw(Buffer pvDisplayData, int8_t lX1, int8_t lX2,
			    uint8_t thick, int8_t lY, uint8_t ulValue);


// fast method to draw a line vertically. It draw the line in X, Y1 and Y2 are
// the start and the end of this line.
void lineVDraw(Buffer pvDisplayData, int8_t lX, int8_t lY1,
			    int8_t lY2, uint8_t ulValue);


// method to draw a line between two points. Bresenham algorithm used to draw in
// in all possible orientations.
void lineDraw(Buffer pvDisplayData, int8_t lX1, int8_t lY1, int8_t lx1, int8_t lY2, uint8_t ulValue);


// draw a rectangle completelly filled
void rectFill(Buffer pvDisplayData, const tRectangle *pRect,
			    uint8_t ulValue);


// draw a simple rectangle
void rectDraw(Buffer pvDisplayData, const tRectangle *pRect,
			    uint8_t ulValue);


// draw a circle. Bresenham algorithm used for this method. 
void circleDraw(Buffer pvDisplayData, uint8_t centerX,
			    uint8_t centerY, uint8_t radius);


void DrawString(Buffer pvDisplayData, char const *pString, etFontType Font, unsigned char Op);

void DrawChar(Buffer pvDisplayData, char const Char, etFontType Font, unsigned char Op);

#endif
