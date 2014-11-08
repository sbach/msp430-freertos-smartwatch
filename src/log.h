#ifndef LOG_H
#define LOG_H

#include "config.h"

#include "hal/uart.h"


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

#ifdef CONFIG_LOGGING

void enable_logging( void );
void slog( const char *fmt, ... );

#else

#define slog(_x, ...) {}

#endif /* CONFIG_LOGGING */

#endif /* LOG_H */
