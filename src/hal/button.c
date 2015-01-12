#include "FreeRTOS.h"
#include "semphr.h"

#include <msp430.h>
#include "button.h"

SemaphoreHandle_t hal_button_mutex = NULL;

void hal_button_init(uint8_t pin)
{
    // initialize the mutex for rtc Interrupts
    hal_button_mutex = xSemaphoreCreateBinary();

    P1OUT |= (1 << pin);        // The outputs are low
    P1SEL = 0x00;               // Peripheral module function: I/O function is selected.
    P1DIR = 0x01;               // 1 -> Output; 0 -> Input; All inputs except LED 
    P1REN |= (1 << pin);        // Pullup/pulldown resistor disabled
    P1DS =  0x00;               // 0 -> Reduced output drive strength;
                                // 1 -> Full output drive strength
    P1IES |= (1 << pin);        // Interrupt Edge Select Registers
                                // 0 -> low-to high transition; 1-> high-to-low transition
    P1IE |= (1 << pin);         // Disable all PORTx interrupts
    P1IFG = 0x00;               // Clear all pending interrupt
}

void __attribute__ ( ( interrupt(PORT1_VECTOR) ) ) hal_button_isr( void )
{
    static BaseType_t xHigherPriorityTaskWoken;

    switch (__even_in_range(P1IV,16)) {
        case BUTTON_NO_INTERRUPT:
            break;
        case BUTTON_P0_IFG:
            break;
        case BUTTON_P1_IFG:
            break;
        case BUTTON_P2_IFG:
            break;
        case BUTTON_P3_IFG:
            break;
        case BUTTON_P4_IFG:

            // P1.4 IFG cleared
            P1IFG &= ~0x010;
            
            // Give mutex
            xHigherPriorityTaskWoken = pdFALSE;
            xSemaphoreGiveFromISR(hal_button_mutex, &xHigherPriorityTaskWoken);
            break;
        case BUTTON_P5_IFG:
            break;
        case BUTTON_P6_IFG:
            break;
        case BUTTON_P7_IFG:
            break;
        default:
            break;
    }
}
