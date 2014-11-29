// vim: ft=c sw=4 ts=4 noet tw=90 cc=+1

 //*****************************************************************************
//
// HAL_MSP-EXP430_I2C - Prototypes for the I2C driverr.
//
//                 MSP430X - Master    10k 10k     I2C peripheral
//                -----------------     |   |   -----------------
//               |     UCB1SDA/P3.7|---------> |SDA              |
//            /|\|                 |        |  |            	 |
//             | |     UCB1SCL/P5.4|---------> |SCL              |
//             --|RST              |           |              	 |
//               | 	               |           |                 |
//               |             	   |		   |          	     |
//               |                 |           |                 |
//               |                 |		   |                 |
//               |                 | 	       |                 |
//               |                 |            -----------------
//
//*****************************************************************************

#ifndef __HAL_I2C_SENSORS_H__
#define __HAL_I2C_SENSORS_H__

#include <stdint.h>

//*****************************************************************************
// Macros.
//*****************************************************************************

#define I2C_SENSORS_NO_INTERRUPTS ( 0 )
#define I2C_SENSORS_ALIFG         ( 2 )
#define I2C_SENSORS_NACKIFG       ( 4 )
#define I2C_SENSORS_STTIFG        ( 6 )
#define I2C_SENSORS_STPIFG        ( 8 )
#define I2C_SENSORS_RXIFG         ( 10 )
#define I2C_SENSORS_TXIFG         ( 12 )

//*****************************************************************************
// Prototypes for the globals exported by this driver.
//*****************************************************************************
void i2c_sensors_init(void);

void i2c_sensors_write(uint8_t addr, uint8_t reg_addr, uint8_t *pData, uint8_t length);

void i2c_sensors_read(uint8_t addr, uint8_t reg_addr, uint8_t *pData, uint8_t length);

void __attribute__ ( ( interrupt(USCI_B1_VECTOR) ) ) i2c_sensors_ISR( void );

#endif // __HAL_LCD_H__
