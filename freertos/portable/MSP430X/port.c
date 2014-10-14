#include "FreeRTOS.h"
#include "task.h"

#define portACLK_FREQUENCY_HZ			( ( TickType_t ) 32768 )
#define portINITIAL_CRITICAL_NESTING	( ( uint16_t ) 10 )
#define portFLAGS_INT_ENABLED			( ( StackType_t ) 0x08 )

typedef void TCB_t;
extern volatile TCB_t * volatile pxCurrentTCB;

volatile uint16_t usCriticalNesting = portINITIAL_CRITICAL_NESTING;

void vPortSetupTimerInterrupt( void );

StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
	uint16_t *pusTopOfStack;
	uint32_t *pulTopOfStack;
	if( sizeof( StackType_t ) == sizeof( uint16_t ) )
	{
		pusTopOfStack = ( uint16_t * ) pxTopOfStack;
		pusTopOfStack--;
		pulTopOfStack = ( uint32_t * ) pusTopOfStack;
	}
	else
	{
		pulTopOfStack = ( uint32_t * ) pxTopOfStack;
	}
	*pulTopOfStack = ( uint32_t ) pxCode;

	pusTopOfStack = ( uint16_t * ) pulTopOfStack;
	pusTopOfStack--;
	*pusTopOfStack = portFLAGS_INT_ENABLED;
	pusTopOfStack -= ( sizeof( StackType_t ) / 2 );

	pxTopOfStack = ( StackType_t * ) pusTopOfStack;

#ifdef PRELOAD_REGISTER_VALUES
	*pxTopOfStack = ( StackType_t ) 0xffff;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0xeeee;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0xdddd;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) pvParameters;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0xbbbb;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0xaaaa;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x9999;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x8888;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x5555;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x6666;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x5555;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x4444;
	pxTopOfStack--;
#else
	pxTopOfStack -= 3;
	*pxTopOfStack = ( StackType_t ) pvParameters;
	pxTopOfStack -= 9;
#endif

	*pxTopOfStack = ( StackType_t ) portNO_CRITICAL_SECTION_NESTING;

	return pxTopOfStack;
}

void vPortEndScheduler( void )
{
}

void vPortSetupTimerInterrupt( void )
{
	vApplicationSetupTimerInterrupt();
}


__attribute__ ( ( interrupt(configTICK_VECTOR) , naked) ) void vTickISREntry( void )
{
	extern void vPortTickISR( void );

    // Bug in __DATA_MODEL_SMALL_
	//__bic_SR_register_on_exit( SCG1 + SCG0 + OSCOFF + CPUOFF );

	#if configUSE_PREEMPTION == 1
		extern void vPortPreemptiveTickISR( void );
		vPortPreemptiveTickISR();
	#else
		extern void vPortCooperativeTickISR( void );
		vPortCooperativeTickISR();
	#endif

	asm volatile ("reti");
}
