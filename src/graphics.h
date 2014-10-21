
#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <stdint.h>
#include "hal/lcd.h"

#define COLOR_Black     0x00
#define COLOR_White     0xff

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




void initializeDisplayBuffer(void *pvDisplayData, uint8_t ucValue);

void pixelDraw(Buffer pvDisplayData, int8_t lX, int8_t lY,
			    uint8_t ulValue);

void lineHDraw(Buffer pvDisplayData, int8_t lX1, int8_t lX2,
			    uint8_t thick, int8_t lY, uint8_t ulValue);

void lineVDraw(Buffer pvDisplayData, int8_t lX, int8_t lY1,
			    int8_t lY2, uint8_t ulValue);

void rectFill(Buffer pvDisplayData, const tRectangle *pRect,
			    uint8_t ulValue);

void rectDraw(Buffer pvDisplayData, const tRectangle *pRect,
			    uint8_t ulValue);

void circleDraw(Buffer pvDisplayData, uint8_t centerX,
			    uint8_t centerY, uint8_t radius);
//TODO CLEAR SCREEN  call the clear screen and clear the buffer with init buffer and a color


#endif
