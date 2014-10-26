	#include "msp430.h"
	#include "data_model.h"

	.global xTaskIncrementTick
	.global vTaskSwitchContext
	.global vPortSetupTimerInterrupt
	.global pxCurrentTCB
	.global usCriticalNesting

	.global vPortPreemptiveTickISR
	.global vPortCooperativeTickISR
	.global vPortYield
	.global xPortStartScheduler

	.global FreeRTOSTickCount


.macro portSAVE_CONTEXT

	;Save the remaining registers.
	pushm_x	#12, r15
	mov.w	&usCriticalNesting, r14
	push_x r14
	mov_x	&pxCurrentTCB, r12
	mov_x	sp, 0( r12 )
	.endm

.macro portRESTORE_CONTEXT

	mov_x	&pxCurrentTCB, r12
	mov_x	@r12, sp
	pop_x	r15
	mov.w	r15, &usCriticalNesting
	popm_x	#12, r15
	pop.w	sr
	ret_x
	.endm


	.text
	.align 2

vPortPreemptiveTickISR:

	; The sr is not saved in portSAVE_CONTEXT() because vPortYield() needs
	;to save it manually before it gets modified (interrupts get disabled).
	push.w sr
	portSAVE_CONTEXT

	movx.w &TA0R, &TA0CCR0
	add.w &FreeRTOSTickCount, &TA0CCR0

	call_x	#xTaskIncrementTick
	call_x	#vTaskSwitchContext

	portRESTORE_CONTEXT


	.align 2

vPortCooperativeTickISR:

	; The sr is not saved in portSAVE_CONTEXT() because vPortYield() needs
	;to save it manually before it gets modified (interrupts get disabled).
	push.w sr
	portSAVE_CONTEXT

	movx.w &TA0R, &TA0CCR0
	add.w &FreeRTOSTickCount, &TA0CCR0

	call_x	#xTaskIncrementTick

	portRESTORE_CONTEXT


	.align 2

vPortYield:

	; The sr needs saving before it is modified.
	push.w	sr

	; Now the SR is stacked we can disable interrupts.
	dint
	nop

	; Save the context of the current task.
	portSAVE_CONTEXT

	; Select the next task to run.
	call_x	#vTaskSwitchContext

	; Restore the context of the new task.
	portRESTORE_CONTEXT


    .align 2

xPortStartScheduler:

	; Setup the hardware to generate the tick. Interrupts are disabled
	; when this function is called.
	call_x	#vPortSetupTimerInterrupt

	; Restore the context of the first task that is going to run.
	portRESTORE_CONTEXT
