#ifndef HAL_UART_H
#define HAL_UART_H

#include "config.h"


/*******************************************************************************
 * Macros
 ******************************************************************************/

#define UART_NO_INTERRUPT   ( 0 )
#define UART_RX_IFG         ( 2 )
#define UART_TX_IFG         ( 4 )

#define WRITE_DEBUG_UART(_x) {UCA3TXBUF = _x; while ((UCA3IFG & UCTXIFG) == 0);}

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void halDebugUARTInit( void );
void halDebugUARTWrite(const char *buf);

void __attribute__ ( ( interrupt(USCI_A3_VECTOR) ) ) halDebugUARTISR( void );

#endif /* HAL_UART_H */

