// vim: ft=c sw=4 ts=4 noet tw=90 cc=+1

#include <msp430.h>
#include <stdint.h>
#include "i2c_sensors.h"


void i2c_sensors_init(void)
{	

    // I2C SDA PIN for I2C mode
	P3SEL |= BIT7;
	
	// I2C_SCL_PIN for I2C mode
	P5SEL |= BIT4;

    // config is on slau208n page 950
	// Configure USCB1 for I2C mode:
	// UCMST - I2C master,
	// UCMODE_3 - I2C mode,
	// 7-bits addresses
	// UCSYNC - synchronous mode enable - I2C is synchronous communication
	// set UCSWRST before init USCI register and 

	UCB1CTL1 |= UCSWRST;

	UCB0CTL0 |= UCMST + UCMODE_3 + UCSYNC;
	UCB0CTL1 |= UCSSEL_3;
	UCB0BR0 = 0x0C;
	UCB0BR1 = 0;	//fSCL = SMCLK/12

	// Initialize USCI state machine
	UCB1CTL1 &= ~UCSWRST;
}

void i2c_sensors_write(uint8_t addr, uint8_t *pData, uint8_t length)
{

}


/*******************************************************************************
 * \brief   ISR to handle events on the USCI_B1 pins.
 *
 * \param void
 * \return void
 ******************************************************************************/
void __attribute__ ( ( interrupt(USCI_B1_VECTOR) ) ) i2c_sensors_ISR( void )
{
    switch (__even_in_range(UCA1IV,4)) {
        case UART_NO_INTERRUPT:
            break;
        case UART_RX_IFG:
            UCA1IFG  &= ~UCRXIFG;
            UCA1TXBUF = UCA1RXBUF;
            break;
        case UART_TX_IFG:
            break;
        default:
            break;
    }
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
