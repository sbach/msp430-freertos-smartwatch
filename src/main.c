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
#include "hal/timer.h"

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

    // Initialize the TimerA0
    hal_timer_a0_init();

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
    // Register the special slot for the FreeRTOS tick
    hal_timer_a0_register(TIMER_A0_SLOT0, CONFIG_FREERTOS_TICK_COUNT, NULL);
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
