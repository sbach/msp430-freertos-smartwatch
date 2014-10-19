#ifndef HAL_UART_H
#define HAL_UART_H

#include "config.h"


/*******************************************************************************
 * Macros
 ******************************************************************************/

#define UART_NO_INTERRUPT   ( 0 )
#define UART_RX_IFG         ( 2 )
#define UART_TX_IFG         ( 4 )

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void halDebugUARTInit( void );
void __attribute__ ( ( interrupt(USCI_A3_VECTOR) ) ) halDebugUARTISR( void );

#endif /* HAL_UART_H */

