// vim: ft=c sw=4 ts=4 noet tw=90 cc=+1

#include <msp430.h>
#include <stdint.h>
#include "lcd.h"




uint8_t VCOMbit= 0x40;

//use this flag to know if we have to Toggle the bit on VCOM to refresh the lcd
uint8_t flagSendToggleVCOMCommand = 0;



void lcd_init(void)
{	
	// Configure LCD_POWER_PIN as output to supply the LCD 
	// and LCD_DISP_PIN as output for DISP
	P3DIR |= BIT4 + BIT5;

	// Provide power to LCD and turn on the lcd (via DISP pin)
	P3OUT |= BIT4 + BIT5;
	
    // LCD_MOSI_PIN for SPI_MOSI mode
	P3SEL |= BIT1;
	
	// LCD_SCLK_PIN for SPI_CLK mode
	P3SEL |= BIT3;

	// Configure LCD_SCS_PIN as output and clear pin
	P3DIR |= BIT0;
	
	//Clear CS
	P3OUT &= ~BIT0;

    // config is on slau208n page 922
	// Configure USCIB0 for SPI mode:
	// SPI master,
	// 3-pin SPI,
	// 8-bit data,
	// Data is captured on the first UCLK edge and changed on the following edge
	// SMCLK sources USCI clock
	// UCCKPH => clock phase select - data capture on first UCLK
	// UCMSB => MSB first
	// UCMST => master mode select
	// UCSYNC => synchro mode enable (SPI is synchronous communication)
	// set UCSWRST before init USCI register and 

	UCB0CTL1 |= UCSWRST;

	UCB0CTL0 |= UCCKPH + UCMSB + UCMST + UCSYNC;
	UCB0CTL1 |= UCSSEL_3;
	UCB0BR0 |= 0x08;
	UCB0BR1 = 0;

	// Initialize USCI state machine
	UCB0CTL1 &= ~UCSWRST;
}

//*****************************************************************************
//
//! Power ON on OFF the screen.
//!
//! This function power off/on the screen.
//!
//! \return None.
//
//*****************************************************************************
void lcd_toggle_power( void )
{
	// Provide power to LCD and turn on the lcd (via DISP pin)
	P3OUT ^= BIT4;  // power
	P3OUT ^= BIT5;  // lcd enable
}

//*****************************************************************************
//
//! Flushes any cached drawing operations.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//!
//!
//! This functions flushes any cached drawing operations to the display.  This
//! is useful when a local frame buffer is used for drawing operations, and the
//! flush would copy the local frame buffer to the display.
//!
//! \return None.
//
//*****************************************************************************
void lcd_write(void *pvDisplayData)
{
	uint8_t *pucData = pvDisplayData;
	int32_t xi =0;
	int32_t xj = 0;

	//image update mode(1X000000b)
	uint8_t command = SHARP_LCD_CMD_WRITE_LINE;

	//add VCOM inversion bit
	command = command^SHARP_VCOM_TOGGLE_BIT;

	//enable the communication on the bus
	SetCS();

	//send the order (write)
	WriteCmdData(command);

	//disable the toggle for the other routine. We just do it now
	flagSendToggleVCOMCommand = SHARP_SKIP_TOGGLE_VCOM_COMMAND;

#ifdef LANDSCAPE
	for(xj=0; xj<LCD_VERTICAL_MAX; xj++)
	{
		WriteCmdData(reverse(xj + 1));

		for(xi=0; xi<(LCD_HORIZONTAL_MAX>>3); xi++)
		{
			WriteCmdData(*(pucData++));
		}
		WriteCmdData(SHARP_LCD_TRAILER_BYTE);
	}
#endif
#ifdef LANDSCAPE_FLIP
	pucData = &DisplayBuffer[LCD_VERTICAL_MAX-1][(LCD_HORIZONTAL_MAX>>3)-1];

	for(xj=1; xj<=LCD_VERTICAL_MAX; xj++)
	{
		WriteCmdData(reverse(xj));

		for(xi=0; xi < (LCD_HORIZONTAL_MAX>>3); xi++)
		{
			WriteCmdData(reverse(*pucData--));
		}
		WriteCmdData(SHARP_LCD_TRAILER_BYTE);
	}
#endif

	// send the trailer byte
	WriteCmdData(SHARP_LCD_TRAILER_BYTE);

	// Wait for last byte to be sent, then drop SCS
	WaitUntilLcdWriteFinished();

	// Ensure a 2us min delay to meet the LCD's thSCS
	__delay_cycles(SYSTEM_CLOCK_SPEED * 0.000002);

	//disable the communication bus
	ClearCS();
}


//*****************************************************************************
//
//! Send command to clear screen.
//!
//! This function sets every pixel to the background color.
//!
//! \return None.
//
//*****************************************************************************
void lcd_clearScreen(void)
{
	//clear screen mode(0X100000b)
	uint8_t command = SHARP_LCD_CMD_CLEAR_SCREEN;
	
	//add VCOM inversion bit
	command = command^SHARP_VCOM_TOGGLE_BIT;

	//enable communication via SPI
	SetCS();

	//send the order (clear)
	WriteCmdData(command);
	
	//disable the toggle VCOM operation for the other routine
	flagSendToggleVCOMCommand = SHARP_SKIP_TOGGLE_VCOM_COMMAND;

	WriteCmdData(SHARP_LCD_TRAILER_BYTE);

	// Wait for last byte to be sent, then drop SCS
	WaitUntilLcdWriteFinished();

	// Ensure a 2us min delay to meet the LCD's thSCS
	__delay_cycles(SYSTEM_CLOCK_SPEED * 0.000002);

	//disable communication bus
	ClearCS();
}


//*****************************************************************************
//
//! Send toggle VCOM command.
//!
//! This function toggles the state of VCOM which prevents a DC bias from being 
//! built up within the panel.
//!
//! \return None.
//
//*****************************************************************************
void lcd_sendToggleVCOMCommand(void)
{
	VCOMbit ^= SHARP_VCOM_TOGGLE_BIT;

	if(SHARP_SEND_TOGGLE_VCOM_COMMAND == flagSendToggleVCOMCommand)
	{
		//clear screen mode(0X100000b)
		uint8_t command = SHARP_LCD_CMD_CHANGE_VCOM;
		//COM inversion bit
		command = command^VCOMbit;

	    SetCS();

		WriteCmdData(command);
		WriteCmdData(SHARP_LCD_TRAILER_BYTE);

		// Wait for last byte to be sent, then drop SCS
		WaitUntilLcdWriteFinished();

		// Ensure a 2us min delay to meet the LCD's thSCS
		__delay_cycles(SYSTEM_CLOCK_SPEED * 0.000002);

		ClearCS();
	}

	flagSendToggleVCOMCommand = SHARP_SEND_TOGGLE_VCOM_COMMAND;
}



//*****************************************************************************
//! Reverses the bit order.- Since the bit reversal function is called
//! frequently by the several driver function this function is implemented
//! to maximize code execution
//*****************************************************************************
const uint8_t referse_data[] = {0x0, 0x8, 0x4, 0xC, 0x2, 0xA, 0x6, 0xE, 0x1, 0x9, 0x5, 0xD, 0x3, 0xB, 0x7, 0xF};
uint8_t reverse(uint8_t x)
{
  uint8_t b = 0;

  b  = referse_data[x & 0xF]<<4;
  b |= referse_data[(x & 0xF0)>>4];
  return b;
}
