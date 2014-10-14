#ifndef PORTMACRO_H
#define PORTMACRO_H

#include "msp430.h"

#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		int
#define portBASE_TYPE	portSHORT

#ifdef __DATA_MODEL_LARGE__
	#define portSTACK_TYPE uint32_t
#else
	#define portSTACK_TYPE uint16_t
#endif

typedef portSTACK_TYPE StackType_t;
typedef short BaseType_t;
typedef unsigned short UBaseType_t;

#if( configUSE_16_BIT_TICKS == 1 )
	typedef uint16_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffff
#else
	typedef uint32_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#endif

#define portDISABLE_INTERRUPTS()	__disable_interrupt(); __nop()
#define portENABLE_INTERRUPTS()		__enable_interrupt(); __nop()

#define portNO_CRITICAL_SECTION_NESTING		( ( uint16_t ) 0 )

#define portENTER_CRITICAL()													\
{																				\
extern volatile uint16_t usCriticalNesting;										\
	portDISABLE_INTERRUPTS();													\
	usCriticalNesting++;														\
}

#define portEXIT_CRITICAL()														\
{																				\
extern volatile uint16_t usCriticalNesting;										\
																				\
	if( usCriticalNesting > portNO_CRITICAL_SECTION_NESTING )					\
	{																			\
		usCriticalNesting--;													\
																				\
		if( usCriticalNesting == portNO_CRITICAL_SECTION_NESTING )				\
		{																		\
			portENABLE_INTERRUPTS();											\
		}																		\
	}																			\
}

extern void vPortYield( void );
#define portYIELD() vPortYield()

#define portBYTE_ALIGNMENT			2
#define portSTACK_GROWTH			( -1 )
#define portTICK_PERIOD_MS			( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portNOP()					__no_operation()

#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )

extern void vTaskSwitchContext( void );
#define portYIELD_FROM_ISR( x ) if( x ) vPortYield()

void vApplicationSetupTimerInterrupt( void );

#define portLU_PRINTF_SPECIFIER_REQUIRED

#endif /* PORTMACRO_H */

