#ifndef LOG_H
#define LOG_H

#include "config.h"

#include "hal/uart.h"


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

#ifdef CONFIG_LOGGING

void EnableLogging( void );
void Log( const char *fmt, ... );

#else

#define Log(_x, ...) {}

#endif /* CONFIG_LOGGING */

#endif /* LOG_H */
