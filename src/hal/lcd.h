// vim: ft=c sw=4 ts=4 noet tw=90 cc=+1

 //*****************************************************************************
//
// HAL_MSP-EXP430_Sharp96x96 - Prototypes for the Sharp96x96 LCD display driver.
//                                 There is no output from Sharp96x96 LCD.
//
//                 MSP430G2XX                    LCD Sharp96x96
//                -----------------             -----------------
//               |     UCB0CLK/P3.3|---------> |SCLK     EXTCOMIN|-- 0
//            /|\|                 |           |            	 |
//             | |    UCB0SIMO/P3.1|---------> |SI(MOSI)  EXTMODE|-- 0
//             --|RST              |           |              	 |
//               |     UCB0STE/P3.0|---------> |SCS              |
//               |             	   |		   |         	     |
//               |             P3.4|---------> |DISP             |
//               |                 |		   |                 |
//               |                 | 	       |                 |
//               |                 |            -----------------
//
//*****************************************************************************

#ifndef __HAL_LCD_H__
#define __HAL_LCD_H__

#include <stdint.h>

//*****************************************************************************
//
// User Configuration for the LCD Driver
//
//*****************************************************************************

// SYSTEM_CLOCK_SPEED (in Hz) allows to properly closeout SPI communication
#define SYSTEM_CLOCK_SPEED      8000000

// Define LCD Screen Orientation Here
#define LANDSCAPE

//Maximum Colors in an image color palette
#define MAX_PALETTE_COLORS  2

#define SHARP_BLACK				0x00
#define SHARP_WHITE				0xFF

#define SHARP_SEND_TOGGLE_VCOM_COMMAND		0x01
#define SHARP_SKIP_TOGGLE_VCOM_COMMAND 		0x00

#define SHARP_LCD_TRAILER_BYTE			0x00

#define SHARP_VCOM_TOGGLE_BIT 		   	0x40

#define SHARP_LCD_CMD_CHANGE_VCOM		0x00
#define SHARP_LCD_CMD_CLEAR_SCREEN		0x20
#define SHARP_LCD_CMD_WRITE_LINE		0x80

// LCD Screen Dimensions
#define LCD_VERTICAL_MAX                   96
#define LCD_HORIZONTAL_MAX                 96

//*****************************************************************************
// Clears CS line
// This macro allows to clear the Chip Select (CS) line
// \return None
//*****************************************************************************
#define ClearCS()                    	\
       P3OUT &= ~BIT0;


//*****************************************************************************
// Set CS line
// This macro allows to set the Chip Select (CS) line
// \return None
//*****************************************************************************
#define SetCS()                    	\
       P3OUT |= BIT0;


//*****************************************************************************
// Waits until the SPI communication with the LCD is finished a command to
// the LCD Driver
// \param None
// \return None
//*****************************************************************************
#define WaitUntilLcdWriteFinished()                \
        while  (UCB0STAT & UCBUSY)


//*****************************************************************************
// Writes command or data to the LCD Driver
// \param ucCmdData is the 8 or 16 bit command to send to the LCD driver
// Uses the SET_LCD_DATA macro
// \return None
//*****************************************************************************
#define WriteCmdData(ucCmdData)                    	\
        do                                         	\
        {                                           \
            while (!(UCTXIFG & UCB0IFG));      		\
            UCB0TXBUF = ucCmdData;                  \
        }                                           \
        while(0)


//*****************************************************************************
// Prototypes for the globals exported by this driver.
//*****************************************************************************
void lcd_init(void);

void lcd_write(void *pvDisplayData);
void lcd_clearScreen(void);
void lcd_sendToggleVCOMCommand(void);

uint8_t reverse(uint8_t x);

#endif // __HAL_LCD_H__
