#ifndef HAL_TIMER_H
#define HAL_TIMER_H

#include "config.h"


/*******************************************************************************
 * Macros
 ******************************************************************************/

#define TIMER_A0_SLOT0    ( 0 )
#define TIMER_A0_SLOT1    ( 1 )
#define TIMER_A0_SLOT2    ( 2 )
#define TIMER_A0_SLOT3    ( 3 )
#define TIMER_A0_SLOT4    ( 4 )

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void hal_timer_a0_init( void );
void hal_timer_a0_register( unsigned char id, unsigned int ticks, void (*callback) (void) );
void hal_timer_a0_unregister( unsigned char id );

void __attribute__ ( ( interrupt(TIMER0_A1_VECTOR) ) ) hal_timer_a0_isr( void );

#endif /* HAL_TIMER_H */

