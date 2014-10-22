#include <stdarg.h>

#include "FreeRTOS.h"
#include "semphr.h"

#include "log.h"


#ifdef CONFIG_LOGGING

// Mutex for the debug UART interface access
static xSemaphoreHandle UARTLoggingMutex;

// Text buffer used to print on the UART interface
char LoggingBuffer[50];


/*******************************************************************************
 * \brief   Initialize the hardware and variables to handle logging on
 *          various interfaces.
 *
 * \param void
 * \return void
 ******************************************************************************/
void EnableLogging( void )
{
    if(!UARTLoggingMutex) {
        // Enable the debug UART interface on P10.4 and P10.5
        halDebugUARTInit();

        // Create a mutex to synchronize access to the buffer
        UARTLoggingMutex = xSemaphoreCreateMutex();
        xSemaphoreGive(UARTLoggingMutex);
    }
}

/*******************************************************************************
 * \brief   Send the result of a formatted string to the logging interfaces.
 *
 * \param char *    Format string
 * \param ...       Argument list
 * \return void
 ******************************************************************************/
void Log( const char *fmt, ... )
{
    // Take the mutex
    xSemaphoreTake(UARTLoggingMutex, portMAX_DELAY);

    va_list va;
    va_start(va,fmt);
    vuprintf(LoggingBuffer, fmt, va);
    va_end(va);

    // Write the string to the UART debug interface
    halDebugUARTWrite(LoggingBuffer);

    // Add EOL characters
    WRITE_DEBUG_UART('\r');
    WRITE_DEBUG_UART('\n');

    // Give the mutex
    xSemaphoreGive(UARTLoggingMutex);
}

#endif /* CONFIG_LOGGING */
