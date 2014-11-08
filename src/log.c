#include <stdarg.h>

#include "FreeRTOS.h"
#include "semphr.h"

#include "log.h"


#ifdef CONFIG_LOGGING

// Mutex for the debug UART interface access
static xSemaphoreHandle uart_logging_mutex;

// Text buffer used to print on the UART interface
static char logging_buffer[50];


/*******************************************************************************
 * \brief   Initialize the hardware and variables to handle logging on
 *          various interfaces.
 *
 * \param void
 * \return void
 ******************************************************************************/
void enable_logging( void )
{
    if(!uart_logging_mutex) {
        // Enable the debug UART interface on P10.4 and P10.5
        hal_init_debug_uart();

        // Create a mutex to synchronize access to the buffer
        uart_logging_mutex = xSemaphoreCreateMutex();
        xSemaphoreGive(uart_logging_mutex);
    }
}

/*******************************************************************************
 * \brief   Send the result of a formatted string to the logging interfaces.
 *
 * \param char *    Format string
 * \param ...       Argument list
 * \return void
 ******************************************************************************/
void slog( const char *fmt, ... )
{
    // Take the mutex
    xSemaphoreTake(uart_logging_mutex, portMAX_DELAY);

    va_list va;
    va_start(va,fmt);
    vuprintf(logging_buffer, fmt, va);
    va_end(va);

    // Write the string to the UART debug interface
    hal_debug_uart_write(logging_buffer);

    // Add EOL characters
    WRITE_DEBUG_UART('\r');
    WRITE_DEBUG_UART('\n');

    // Give the mutex
    xSemaphoreGive(uart_logging_mutex);
}

#endif /* CONFIG_LOGGING */
