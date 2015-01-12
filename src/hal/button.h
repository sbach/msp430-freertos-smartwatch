#ifndef HAL_BUTTON_H
#define HAL_BUTTON_H

#define BUTTON_NO_INTERRUPT   ( 0 )
#define BUTTON_P0_IFG         ( 2 )
#define BUTTON_P1_IFG         ( 4 )
#define BUTTON_P2_IFG         ( 6 )
#define BUTTON_P3_IFG         ( 8 )
#define BUTTON_P4_IFG         ( 10 )
#define BUTTON_P5_IFG         ( 12 )
#define BUTTON_P6_IFG         ( 14 )
#define BUTTON_P7_IFG         ( 16 )

void hal_button_init(uint8_t pin);
void __attribute__ ( ( interrupt(PORT1_VECTOR) ) ) hal_button_isr( void );

#endif
