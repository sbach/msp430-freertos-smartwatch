// Standard
#include <stdio.h>

// FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"

// Hardware includes
#include "msp430.h"

// HAL includes
#include "hal/misc.h"

#include "log.h"


// Prototypes
void vApplicationTickHook( void );
void vApplicationSetupTimerInterrupt( void );
void vApplicationIdleHook( void );
void vApplicationMallocFailedHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );


int main( void )
{
    //Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;

    // Setup the UCS and PMM
    hal_setup_clock_pmm();

#ifdef CONFIG_LOGGING
    enable_logging();
#endif

    // Start the scheduler
    vTaskStartScheduler();

    // If all is well then thes line will never be reached. If it is reached
    // then it is likely that there was insufficient (FreeRTOS) heap memory space
    // to create the idle task.  This may have been trapped by the malloc() failed
    //hook function, if one is configured.
    for( ;; );

    return 0;
}

void vApplicationTickHook( void )
{
}

// The MSP430X port uses this callback function to configure its tick interrupt.
// This allows the application to choose the tick interrupt source.
// configTICK_VECTOR must also be set in FreeRTOSConfig.h to the correct
// interrupt vector for the chosen tick interrupt source.  This implementation of
// vApplicationSetupTimerInterrupt() generates the tick from timer A0, so in this
// case configTICK_VECTOR is set to TIMER0_A0_VECTOR.
void vApplicationSetupTimerInterrupt( void )
{
    // Ensure the timer is stopped
    TA0CTL = 0;

    // Run the timer from the ACLK
    TA0CTL = TASSEL_1;

    // Clear everything to start with
    TA0CTL |= TACLR;

    // Set the compare match value according to the tick rate we want
    TA0CCR0 = CONFIG_XT1_CLOCK_HZ / CONFIG_FREERTOS_TICK_RATE_HZ;

    // Enable the interrupts
    TA0CCTL0 = CCIE;

    // Start up clean.
    TA0CTL |= TACLR;

    // Up mode
    TA0CTL |= MC_1;
}

void vApplicationIdleHook( void )
{
    // Called on each iteration of the idle task. In this case the idle task
    // just enters a low(ish) power mode. */
    __bis_SR_register( LPM1_bits + GIE );
}

void vApplicationMallocFailedHook( void )
{
    // Called if a call to pvPortMalloc() fails because there is insufficient
    // free memory available in the FreeRTOS heap.  pvPortMalloc() is called
    // internally by FreeRTOS API functions that create tasks, queues or
    // semaphores.
    taskDISABLE_INTERRUPTS();
    for( ;; );
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    ( void ) pxTask;
    ( void ) pcTaskName;

    // Run time stack overflow checking is performed if
    // configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook
    // function is called if a stack overflow is detected.
    taskDISABLE_INTERRUPTS();
    for( ;; );
}
