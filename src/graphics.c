#include "graphics.h"
#include "hal/lcd.h"

// Global variable to handle the text cursor
static unsigned char gColumn = 2;
static unsigned char gRow = 5 ;

//*****************************************************************************
//
//! Initialize DisplayBuffer.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//!
//!	\param ucValue is the foreground color of the buffered data.
//!
//! This function initializes the display buffer and discards any cached data.
//!
//! \return None.
//
//*****************************************************************************
void initializeDisplayBuffer(void *pvDisplayData, uint8_t ucValue)
{
	uint8_t i=0,j=0;
	uint8_t *pucData = pvDisplayData;

	for(i =0; i< LCD_VERTICAL_MAX; i++)
	for(j =0; j< (LCD_HORIZONTAL_MAX>>3); j++)
		*pucData++ = ucValue;
}


//*****************************************************************************
//
//! Draws a pixel on the screen.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX is the X coordinate of the pixel.
//! \param lY is the Y coordinate of the pixel.
//! \param ulValue is the color(white or black) of the pixel.
//!
//! This function sets the given pixel to a particular color.  The coordinates
//! of the pixel are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
void pixelDraw( Buffer pvDisplayData, int8_t lX, int8_t lY,
                                   uint8_t ulValue)
{
    
    // we keep the modulo of lX by 8 and clear or set the corresponding bit in 
    // the corresponding byte
    if( COLOR_Black == ulValue){
	pvDisplayData[lY][lX>>3] &= ~(0x80 >> (lX & 0x7));
    }else{
	pvDisplayData[lY][lX>>3] |= (0x80 >> (lX & 0x7));
    }
}


//*****************************************************************************#include "graphics.h"
//
//! Draws a horizontal line. (Fast method)
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX1 is the X coordinate of the start of the line.
//! \param lX2 is the X coordinate of the end of the line.
//! \param lY is the Y coordinate of the line.
//! \param thick is the thickness of the line.
//! \param ulValue is the color of the line.
//!
//! This function draws a horizontal line on the display.  The coordinates of
//! the line are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
void lineHDraw(Buffer pvDisplayData, int8_t lX1, int8_t lX2,
                                   uint8_t thick, int8_t lY, uint8_t ulValue)
{
	volatile uint16_t xi, xj = 0;
	volatile uint8_t x_index_min = lX1>>3;
	volatile uint8_t x_index_max = lX2>>3;
	volatile uint8_t ucfirst_x_byte, uclast_x_byte;

    //check if the first X coordinate is lower than the second
    
	//calculate first byte
	//mod by 8 and shift this # bits
	ucfirst_x_byte = (0xFF >> (lX1 & 0x7));    
	//calculate last byte
	//mod by 8 and shift this # bits
	uclast_x_byte = (0xFF << (7-(lX2 & 0x7))); 

    for(xj=0;xj<thick; xj++)
    {
	    //check if more than one data byte
	    if(x_index_min != x_index_max)
	    {
	        //black pixels (clear bits)
	        if(COLOR_Black == ulValue)
	        {
		        //write first byte
		        pvDisplayData[lY+xj][x_index_min] &= ~ucfirst_x_byte;

		        //write middle bytes
		        for(xi = x_index_min+1; xi < x_index_max; xi++)
		        {
			    pvDisplayData[lY+xj][xi] = 0x00;
		        }

		        //write last byte
		        pvDisplayData[lY+xj][x_index_max] &= ~uclast_x_byte;
	        }
	        //white pixels (set bits)
	        else
	        {
		    //write first byte
		    pvDisplayData[lY+xj][x_index_min] |= ~ucfirst_x_byte;

		    //write middle bytes
		    for(xi = x_index_min+1; xi < x_index_max; xi++)
		    {
			pvDisplayData[lY+xj][xi] = 0xFF;
		    }

		    //write last byte
		    pvDisplayData[lY+xj][x_index_max] |= ~uclast_x_byte;
	        }
	    }
	    //only one data byte
	    else
	    {
		    //calculate value of single byte
		    ucfirst_x_byte &= uclast_x_byte;

		    //draw black pixels (clear bits)
		    if(COLOR_Black == ulValue)
		    {
			pvDisplayData[lY+xj][x_index_min] &= ~ucfirst_x_byte;
		    }
		    //white pixels (set bits)
		    else
		    {
			pvDisplayData[lY+xj][x_index_min] |= ucfirst_x_byte;
		    }
	    }
    }
}


//*****************************************************************************
//
//! Draws a vertical line. (Fast method)
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX is the X coordinate of the line.
//! \param lY1 is the Y coordinate of the start of the line.
//! \param lY2 is the Y coordinate of the enGrClearDisplayd of the line.
//! \param ulValue is the color of the line.
//!
//! This function draws a vertical line on the display.  The coordinates of the
//! line are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
void lineVDraw(Buffer pvDisplayData, int8_t lX, int8_t lY1,
                                   int8_t lY2, uint8_t ulValue)
{

	volatile uint8_t yi = 0;
	volatile uint8_t x_index = lX>>3;
	volatile uint8_t data_byte;

	//calculate data byte
	//mod by 8 and shift this # bits
	data_byte = (0x80 >> (lX & 0x7));     

	//write data to the display buffer
	for(yi = lY1; yi <= lY2; yi++){

		//black pixels (clear bits)
		if(COLOR_Black == ulValue)
		{
			pvDisplayData[yi][x_index] &= ~data_byte;
		}
		//white pixels (set bits)
		else
		{
			pvDisplayData[yi][x_index] |= data_byte;
		}
	}
}


//*****************************************************************************
//
//! Draw a line.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX1 is the x coordinate for the first point of the line
//! \param lY1 is the y coordinate for the first point of the line.
//! \param lX2 is the x coordinate for the last point of the line.
//! \param lY2 is the y coordinate for the last point of the line.
//! \param ulValue is the color of the rectangle.
//!
//! This function draws a line on the display.  The coordinates of the
//! line are assumed to be within the extents of the display.
//! Bresenham algorithm is used to draw the line in all quadran.
//!
//! \return None.
//
//*****************************************************************************

void lineDraw(Buffer pvDisplayData, int8_t lX1, int8_t lY1, int8_t lX2, int8_t lY2, uint8_t ulValue)
{
    int8_t dx, dy;
     
    dx = lX2-lX1;
    dy = lY2-lY1;
    if(dx!=0){
        if(dx>0){
            if(dy!=0){
                if(dy>0){       // oblique vector in the 1st quadran
                    
                    if(dx>=dy){     //diagonal vector near horizontal in 1st octant
                        int8_t e=dx;
                        dx=e*2;
                        dy=dy*2;
                        do{	// horizontal movement
                            pixelDraw(pvDisplayData, lX1, lY1, COLOR_Black);
                            e = e-dy;
			    if(e<0){
                                e = e + dx;
				lY1 = lY1 + 1;	//diagonal movement
                            }
			    lX1 = lX1+1;
                        }while(lX1!=lX2);

                    }else{  //oblique vector near vertical in 2nd octant
                        int8_t e = dy;
                        dy = e*2;
                        dx = dx * 2;
                        do{	//vertical movement
                            pixelDraw(pvDisplayData, lX1, lY1, COLOR_Black);
                            e=e-dx;
			    if(e<=0){
                                lX1 = lX1 + 1;	//diagonal movement
                                e = e + dy;
                            }
			    lY1=lY1+1;
                        }while(lY1!=lY2);
                    }
                }else{      		// dy<0 et dx>0
		    // oblique vector in 4th quadran 
                    if(dx>= -dy){	//diagonal vector near horizontal in 8th octant
                        int8_t e=dx;
                        dx = e*2;
                        dy = dy *2;
                        do{	//horizontal movement
                            pixelDraw(pvDisplayData, lX1, lY1, COLOR_Black);
                            e=e-dy;
			    if(e<0){
                                lY1 = lY1 - 1;	//diagonal movement
                                e = e + dx;
                            }
			    lX1=lX1+1;
                        }while(lX1!=lX2);
                    
		    }else{	//oblique vector near vertical in 7th octant
                        int8_t e=dy;
                        dy = e*2;
                        dx = dx*2;
                        do{	// vertical movement
                            pixelDraw(pvDisplayData, lX1, lY1, COLOR_Black);
                            e=e-dx;
			    if(e>0){
                                lX1 = lX1 + 1;	//diagonal movement
                                e = e + dy;
                            }
			    lY1=lY1-1;
                        }while(lY1!=lY2);
                    }
                }
            }else{      //dy=0 et dx>0
			//horizontal vector to the right
                do{
                    pixelDraw(pvDisplayData, lX1, lY1, COLOR_Black);
		    lX1=lX1+1;
                }while(lX1 == lX2);
            }
        }else{ //dx<0
	    dy=lY2-lY1;
            if(dy!=0){
                
		if(dy>0){   // oblique vector in 2nd quadran
                    if(-dx>=dy){	//oblique vector near horizontal in the 4th octant
                        int8_t e=dx;  
                        dx = e*2;
                        dy = dy *2;
                        do{	//horizontal movement
                            pixelDraw(pvDisplayData, lX1, lY1, COLOR_Black);
                            e=e+dy;
			    if(e>=0){
                                lY1 = lY1 + 1;	//diagonal movement
                                e = e + dy;
                            }
			    lX1=lX1-1;
                        }while(lX1!=lX2);
                    }else{	//oblique vector near vertical in 3rd octant
                        int8_t e=dy;
                        dy = e*2;
                        dx = dx *2;
                        do{	//vertical movement
                            pixelDraw(pvDisplayData, lX1, lY1, COLOR_Black);
                            e=e+dx;
			    if(e<=0){
                                lX1 = lX1 - 1;	//diagonal movement
                                e = e + dy;
                            }
			    lY1=lY1+1;
                        }while(lY1!=lY2);
                    }
                }else{      //dy < 0 and dx <0
			    //oblique vector in the 3rd quadran
                    if(dx<=dy){		//oblique vector near horizontal in 5th octant
                        int8_t e=dx;
                        dx = e*2;
                        dy = dy *2;
                        do{	//horizontal movement
                            pixelDraw(pvDisplayData, lX1, lY1, COLOR_Black);
                            e=e-dy;
			    if(e>=0){
                                lY1 = lY1 - 1;	//diagonal movement
                                e = e + dx;
                            }
			    lX1=lX1-1;
                        }while(lX1!=lX2);
                    }else{	//oblique vector near vertical in 6th octant
                        int8_t e=dy;
                        dy = e*2;
                        dx = dx *2;
                        do{	//vertical movement
                            pixelDraw(pvDisplayData, lX1, lY1, COLOR_Black);
                            e=e-dx;
			    if(e>=0){
                                lX1 = lX1 - 1;	//diagonal movement
                                e = e + dy;
                            }
			    lY1=lY1-1;
                        }while(lY1!=lY2);
                    }
                }
            }else{      //dy = 0 et dx <0
			//horizontal vector to the left
                do{
                    pixelDraw(pvDisplayData, lX1, lY1, COLOR_Black);
		    lX1=lX1-1;
                }while(lX1!=lX2);
            }
        }
    }else{      //dx = 0
	dy=lY2-lY1;
        if(dy!=0){
            if(dy > 0){
                do{	// vertical ascending vector 
                    pixelDraw(pvDisplayData, lX1, lY1, COLOR_Black);
		    lY1=lY1+1;
                }while(lY1!=lY2);
            }else{
                do{	// vertical descending vector
                    pixelDraw(pvDisplayData, lX1, lY1, COLOR_Black);
		    lY1=lY1-1;
                }while(lY1!=lY2);
            }
        }
    }
}


//*****************************************************************************
//
//! Fills a rectangle.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param pRect is a pointer to the structure describing the rectangle.
//! \param ulValue is the color of the rectangle.
//!
//! This function fills a rectangle on the display.  The coordinates of the
//! rectangle are assumed to be within the extents of the display, and the
//! rectangle specification is fully inclusive (in other words, both sXMin and
//! sXMax are drawn, along with sYMin and sYMax).
//!
//! \return None.
//
//*****************************************************************************
void rectFill(Buffer pvDisplayData, const tRectangle *pRect,
                                  uint8_t ulValue)
{
	volatile uint8_t xi = 0;
	volatile uint8_t yi = 0;
	volatile uint8_t x_index_min = pRect->sXMin>>3;
	volatile uint8_t x_index_max = pRect->sXMax>>3;
	volatile uint8_t ucfirst_x_byte, uclast_x_byte;

	//calculate first byte
	//mod by 8 and shift this # bits
	ucfirst_x_byte = (0xFF >> (pRect->sXMin & 0x7));  

	//calculate last byte
	//mod by 8 and shift this # bits
	uclast_x_byte = (0xFF << (7-(pRect->sXMax & 0x7)));   

	//check if more than one data byte
	if(x_index_min != x_index_max){

		//write bytes
		for (yi = pRect->sYMin; yi<= pRect->sYMax; yi++)
		{
			//black pixels (clear bits)
			if(COLOR_Black == ulValue)
			{
				//write first byte
				pvDisplayData[yi][x_index_min] &= ~ucfirst_x_byte;

				//write middle bytes
				for(xi = x_index_min+1; xi < x_index_max; xi++)
				{
				    pvDisplayData[yi][xi] = 0x00;
				}

				//write last byte
				pvDisplayData[yi][x_index_max]&= ~uclast_x_byte;
			}
			//white pixels (set bits)
			else
			{
				//write first byte
				pvDisplayData[yi][x_index_min] |= ucfirst_x_byte;

				//write middle bytes
				for(xi = x_index_min+1; xi < x_index_max; xi++)
				{
					pvDisplayData[yi][xi] = 0xFF;
				}

				//write last byte
				pvDisplayData[yi][x_index_max] |= uclast_x_byte;
			}
		}
	}
	//only one data byte
	else
	{
		//calculate value of single byte
		ucfirst_x_byte &= uclast_x_byte;

		//black pixels (clear bits)
		if(COLOR_Black == ulValue)
		{
		    pvDisplayData[pRect->sYMin][x_index_min] &= ~ucfirst_x_byte;
		}
		//white pixels (set bits)
		else
		{
		    pvDisplayData[pRect->sYMin][x_index_min] |= ucfirst_x_byte;
		}
	}
}


//*****************************************************************************
//
//! Draw a rectangle.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param pRect is a pointer to the structure describing the rectangle.
//! \param ulValue is the color of the rectangle.
//!
//! This function draws a rectangle on the display.  The coordinates of the
//! rectangle are assumed to be within the extents of the display, and the
//! rectangle specification is fully inclusive (in other words, both sXMin and
//! sXMax are drawn, along with sYMin and sYMax).
//!
//! \return None.
//
//*****************************************************************************

void rectDraw(Buffer pvDisplayData, const tRectangle *pRect,
                        uint8_t ulValue)
{
	volatile uint8_t x_min = pRect->sXMin;
	volatile uint8_t x_max = pRect->sXMax;
	volatile uint8_t y_min = pRect->sYMin;
	volatile uint8_t y_max = pRect->sYMax;
	
	lineHDraw(pvDisplayData, x_min, x_max, 1, y_min, ulValue);
	lineHDraw(pvDisplayData, x_min, x_max, 1, y_max, ulValue);
	lineVDraw(pvDisplayData, x_min, y_min, y_max, ulValue);
	lineVDraw(pvDisplayData, x_max, y_min, y_max, ulValue);
}


//*****************************************************************************
//
//! Draw a circle.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param centerX is the X coordinate of the circle.
//! \param centerY is the Y coordinate of the circle.
//! \param radius is the radius of this circle.
//!
//! This function draws a circle on the display.  The coordinates of the
//! circle are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************

void circleDraw(Buffer pvDisplayData, uint8_t centerX, uint8_t centerY, uint8_t radius)
{
    int x = radius;
    int y = 0;
    int radError = 1-x;
    
    while(x>=y){
        pixelDraw(pvDisplayData, x + centerX, y + centerY, COLOR_Black);
        pixelDraw(pvDisplayData, y + centerX, x + centerY, COLOR_Black);
        pixelDraw(pvDisplayData, -x + centerX, y + centerY, COLOR_Black);
        pixelDraw(pvDisplayData, -y + centerX, x + centerY, COLOR_Black);
        pixelDraw(pvDisplayData, -x + centerX, -y + centerY, COLOR_Black);
        pixelDraw(pvDisplayData, -y + centerX, -x + centerY, COLOR_Black);
        pixelDraw(pvDisplayData, x + centerX, -y + centerY, COLOR_Black);
        pixelDraw(pvDisplayData, y + centerX, -x + centerY, COLOR_Black);
        y = y+1;
        
        if(radError<0){
            radError = radError + 2*y +1;
        }else{
            x=x-1;
            radError = radError + 2*(y-x+1);
        }
    }
}


void DrawString(Buffer pvDisplayData, char const *pString, etFontType Font)
{
    while (*pString)
    {
        DrawChar(pvDisplayData, *pString++, Font);
    }
}

void DrawChar(Buffer pvDisplayData, char const Char, etFontType Font)
{
    tFont const *pFont = GetFont(Font);
    unsigned char const *pBitmap = GetFontBitmap(Char, Font);
    unsigned char CharWidth = GetCharWidth(Char, Font);

    unsigned char MaskBit = 1; 
    unsigned char x, y;
  
    for (y = 0; y < pFont->Height; y++)
    {
        for (x = 0; x < (pFont->WidthInBytes*8); x++)
        {
            if(*(pBitmap) & MaskBit){
                pixelDraw(pvDisplayData, gColumn+x, gRow+y, COLOR_Black);
            }
            MaskBit = MaskBit<<1;
            if(MaskBit==0){
                MaskBit = 1;
                pBitmap++;
            }
        }
        if(MaskBit!=1){
            MaskBit=1;
            pBitmap++;
        }
    }
    if(gColumn + CharWidth+3>LCD_HORIZONTAL_MAX){
        gColumn = 5;
        gRow = gRow + pFont->Height +2;
    }else{
        gColumn = gColumn + CharWidth;
    }
}
