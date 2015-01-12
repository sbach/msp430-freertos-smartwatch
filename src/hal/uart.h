#ifndef HAL_UART_H
#define HAL_UART_H

#include "config.h"


/*******************************************************************************
 * Macros
 ******************************************************************************/

#define UART_NO_INTERRUPT   ( 0 )
#define UART_RX_IFG         ( 2 )
#define UART_TX_IFG         ( 4 )

#define WRITE_DEBUG_UART(_x) {UCA1TXBUF = _x; while ((UCA1IFG & UCTXIFG) == 0);}

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void halDebugUARTInit( void );
void halDebugUARTWrite(const char *buf);
short halDebugUARTRead(char *buf);

void __attribute__ ( ( interrupt(USCI_A1_VECTOR) ) ) halDebugUARTISR( void );

#endif /* HAL_UART_H */

