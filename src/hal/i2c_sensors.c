// vim: ft=c sw=4 ts=4 noet tw=90 cc=+1

#include <msp430.h>
#include <stdint.h>
#include "i2c_sensors.h"

/*******************************************************************************
 * \brief  Init the i2c bus for communication with sensors
 *
 * \param void
 * \return void
 ******************************************************************************/
void i2c_sensors_init(void)
{	

    // I2C SDA PIN for I2C mode (P3.7)
	P3SEL |= BIT7;
	
	// I2C_SCL_PIN for I2C mode (P5.4)
	P5SEL |= BIT4;

	// set UCSWRST before init USCI register
	UCB1CTL1 |= UCSWRST;

    // config is on slau208n page 950
	// Configure USCB1 for I2C mode:
	// UCMST - I2C master,
	// UCMODE_3 - I2C mode,
	// 7-bits addresses
	// UCSYNC - synchronous mode enable - I2C is synchronous communication
	UCB0CTL0 |= UCMST + UCMODE_3 + UCSYNC;

	// Clock selection : SMCLK
	UCB0CTL1 |= UCSSEL_3;
	
	// baud rate prescaler 
	UCB0BR0 = 0x0C;
	UCB0BR1 = 0;	//fSCL = SMCLK/12

	// Initialize USCI state machine
	UCB1CTL1 &= ~UCSWRST;
}

/*******************************************************************************
 * \brief   write one or more words in the sensor registers
 *
 * \param uint8_t sensor's address 
 * \param uint8_t * pData is the pointer to the data to be written
 * \param uint8_t length of the data sent
 * \return void
 ******************************************************************************/
void i2c_sensors_write(uint8_t addr, uint8_t *pData, uint8_t length)
{

}

/*******************************************************************************
 * \brief   read the data in the i2c bus.
 *
 * \param uint8_t sensor's address 
 * \param uint8_t * pData is the pointer to the data to be written
 * \param uint8_t length of the data sent
 * \return void
 ******************************************************************************/
void i2c_sensors_read(uint8_t addr, uint8_t *pData, uint8_t length)
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
    switch (__even_in_range(UCB1IV,12)) {
        case I2C_SENSORS_NO_INTERRUPTS:
            break;
        case I2C_SENSORS_ALIFG:
            break;
        case I2C_SENSORS_NACKIFG:
            break;
        case I2C_SENSORS_STTIFG:
    		break;
		case I2C_SENSORS_STPIFG:
    		break;
    	case I2C_SENSORS_RXIFG:
    		break;
		case I2C_SENSORS_TXIFG:
			break;
        default:
            break;
    }
}
