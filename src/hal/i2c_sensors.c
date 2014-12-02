// vim: ft=c sw=4 ts=4 noet tw=90 cc=+1

#include <msp430.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "semphr.h"

#include "i2c_sensors.h"

// Mutex for synchronization between methods and interruptions
static xSemaphoreHandle i2c_sensors_mutex;

static unsigned char *pXData;
static unsigned char byteCtr;
/*******************************************************************************
 * \brief  Init the i2c bus for communication with sensors
 *
 * \param void
 * \return void
 ******************************************************************************/
void i2c_sensors_init(void)
{
	// create mutex to synchronize write/read method with interrupts	
	if(!i2c_sensors_mutex)
	{
	    i2c_sensors_mutex = xSemaphoreCreateMutex();
	}

        // use I2C SDA PIN for I2C mode (P3.7)
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
void i2c_sensors_write(uint8_t addr, uint8_t reg_addr, uint8_t *pData, uint8_t length)
{
/*********COMMON PART WITH READ *****************************/
	// copy counter and buffer address
	*pXData = pData;
	byteCtr = length;
	
	// clear TX flag
	UCB1IFG = 0;
	
	// write sensor address
	ICB1I2CSA = addr; 
	
	// wait for bus to be free
	while (UCB1STAT & UCBBUSY);
	
	//master as transmitter plus START condition
	UCB1CTL1 |= UCTR + UCTXSTT;
	
	// wait address acknowledge
	while(!(UCB1IFG &UCTXIFG));
	
	// write register address 
	UCB1IFG = 0;
	UCB1TXBUF = reg_addr;
	
	// wait for an ack
	while(!(UCB1IFG &UCTXIFG));

	UCB1IFG = 0;
	
/************************************************************/

	// enable TX interrupt
	UCB1IE |= UCTXIE;
	
	// wait for interrupt ends
	xSemaphoreTake(i2c_sensors_mutex, portMAX_DELAY);
}


/*******************************************************************************
 * \brief   read the data in the i2c bus.
 *
 * \param uint8_t sensor's address 
  * \param uint8_t sensor register address 
 * \param uint8_t * pData is the pointer to the data to be written
 * \param uint8_t length of the data sent
 * \return void
 ******************************************************************************/
void i2c_sensors_read(uint8_t addr, uint8_t reg_addr, uint8_t *pData, uint8_t length)
{
//*************************************************************
	// copy counter and buffer address
	*pXData = pData;
	byteCtr = length;
	
	// clear TX flag
	UCB1IFG = 0;
	
	// write sensor address
	ICB1I2CSA = addr; 
	
	// wait for bus to be free
	while (UCB1STAT & UCBBUSY);
	
	//master as transmitter plus START condition
	UCB1CTL1 |= UCTR + UCTXSTT;
	
	// wait address acknowledge
	while(!(UCB1IFG &UCTXIFG));
	
	// write register address 
	UCB1IFG = 0;
	UCB1TXBUF = reg_addr;
	
	// wait for an ack
	while(!(UCB1IFG &UCTXIFG));
//*************************************************************

	// master as receiver
	UCB1CTL1 &= ~UCTR;
	UCB1IFG = 0;

	// enable RX interrupt
	UCB1IE |= UCRXIE;

	// if we read only one byte, stop condition must already be sent
	if(length == 1)
	{
		/* errata usci30: prevent interruption of sending stop 
		 * so that only one byte is read
		 * this requires 62 us @ 320 kHz, 51 @ 400 kHz
		 */
		portENTER_CRITICAL();
	  
		UCB1CTL1 |= UCTXSTT;
	  
		while (UCB1CTL1 & UCTXSTT);

		UCB1CTL1 |= UCTXSTP;
	  
		portEXIT_CRITICAL();
	}
	else
	{
		// send the RESTART condition 
		UCB1CTL1 |= UCTXSTT;
	}
	
	// wait until all data has been received and stop bit has been sent
	if(xSemaphoreTake(i2c_sensors_mutex, portMAX_DELAY) == pdTRUE){}
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
    		
    	case I2C_SENSORS_RXIFG:	//read operation
    		byteCtr--;

		// is there any byte to transmit ?
    		if(byteCtr)
    		{
    			*pXData++ = UCB1RXBUF;

    			if(byteCtr == 1)
    			{
    				// only one byte left ? Generate STOP condition
    				UCB1CTL1 |= UCTXSTP;
    			}
    		}
    		else
    		{
    			// copy the last byte
    			*pXData = UCB1RXBUF;
    			// disable RX interrupt;
    			UCB1IE &= ~UCRXIE;
    			// clear TX flag
    			UCB0IFG &= ~UCTXIFG; 
    		}
    		break;
    		
		case I2C_SENSORS_TXIFG:	//write operation
			if(byteCtr)
			{
				// copy data into TX register
				UCB1TXBUF = *pXData++;
				byteCtr--;
			}
			else
			{
				// send STOP condition
				UCB1CTL1 |= UCTXSTP;
				// disable RX interrupt;
        			UCB1IE &= ~UCTXIE;
				// clear TX flag
				UCB0IFG &= ~UCTXIFG; 
			}
			break;
			
        default:
            break;
    }
}
