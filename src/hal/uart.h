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

void hal_init_debug_uart( void );
void hal_debug_uart_write(const char *buf);

void __attribute__ ( ( interrupt(USCI_A1_VECTOR) ) ) hal_debug_uart_isr( void );

#endif /* HAL_UART_H */

