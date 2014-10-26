#include "FreeRTOS.h"

#include "timer.h"


static void (*timer_a0_callback_1)(void);
static void (*timer_a0_callback_2)(void);
static void (*timer_a0_callback_3)(void);
static void (*timer_a0_callback_4)(void);

static unsigned char timer_a0_users;

unsigned int FreeRTOSTickCount = CONFIG_FREERTOS_TICK_COUNT;


/*******************************************************************************
 * \brief   Setup TimerA0
 *
 * \param void
 * \return void
 ******************************************************************************/
void hal_timer_a0_init( void )
{
    // Ensure the timer is stopped
    TA0CTL = 0;

    // Clear everything to start with
    TA0CTL |= TACLR;

    // Configure the second input clock divider
    TA0EX0 = 0x7;

    timer_a0_users = 0;
}

/*******************************************************************************
 * \brief   Get the tick count from the TA0R register
 *
 * \param void
 * \return unsigned int     Value of the TA0R register
 ******************************************************************************/
static inline unsigned int GetTickCount(void)
{
    unsigned int value1;
    unsigned int value2 = TA0R;

    // When the timer clock is asynchronous to the CPU clock,
    // any read from TAxR should occur while the timer is not
    // operating or the results may be unpredictable.
    // Alternatively, the timer may be read multiple times while
    // operating, and a majority vote taken in software to
    // determine the correct reading.
    // @see page 461 of slau208n.pdf

    do
    {
        value1 = value2;
        value2 = TA0R;
    } while (value1 != value2);

    return value1;
}

/*******************************************************************************
 * \brief   Register a sub-timer to TimerA0
 *
 * \param unsigned char     Compare register to use
 * \param unsigned int      Number of ticks to countdown
 * \param void(void)        Callback when the timer is over
 * \return void
 ******************************************************************************/
void hal_timer_a0_register( unsigned char id, unsigned int ticks, void (*callback) (void) )
{
    if ( ticks < 1 ) ticks = 1;
    if ( id != TIMER_A0_SLOT0 && !callback ) return;

    portENTER_CRITICAL();

    unsigned int time = GetTickCount() + ticks;

    switch(id) {
        case TIMER_A0_SLOT0:
            TA0CCTL0 = 0; TA0CCR0 = time; TA0CCTL0 = CCIE;
            break;
        case TIMER_A0_SLOT1:
            TA0CCTL1 = 0; TA0CCR1 = time; TA0CCTL1 = CCIE;
            timer_a0_callback_1 = callback;
            break;
        case TIMER_A0_SLOT2:
            TA0CCTL2 = 0; TA0CCR2 = time; TA0CCTL2 = CCIE;
            timer_a0_callback_2 = callback;
            break;
        case TIMER_A0_SLOT3:
            TA0CCTL3 = 0; TA0CCR3 = time; TA0CCTL3 = CCIE;
            timer_a0_callback_3 = callback;
            break;
        case TIMER_A0_SLOT4:
            TA0CCTL4 = 0; TA0CCR4 = time; TA0CCTL4 = CCIE;
            timer_a0_callback_4 = callback;
            break;
  default:
            break;
    }

    if(timer_a0_users == 0)
        TA0CTL |= TASSEL_1 |    // Timer source clock
                  MC_2 |        // Continuous mode
                  ID_2;         // Input clock first divider

    timer_a0_users |= (1 << id);

    portEXIT_CRITICAL();
}

/*******************************************************************************
 * \brief   Remove a sub-timer of TimerA0
 *
 * \param unsigned char     Compare register of the sub-timer to reset
 * \return void
 ******************************************************************************/
void hal_timer_a0_unregister( unsigned char id )
{
    portENTER_CRITICAL();

    switch (id) {
        case TIMER_A0_SLOT0: TA0CCTL0 = 0; break;
        case TIMER_A0_SLOT1: TA0CCTL1 = 0; break;
        case TIMER_A0_SLOT2: TA0CCTL2 = 0; break;
        case TIMER_A0_SLOT3: TA0CCTL3 = 0; break;
        case TIMER_A0_SLOT4: TA0CCTL4 = 0; break;
        default:
            break;
    }

    timer_a0_users &= ~(1 << id);

    if (timer_a0_users == 0)
        TA0CTL = 0;

    portEXIT_CRITICAL();
}

/*******************************************************************************
 * \brief   ISR to handle TA0CCR1-4 CCIFG events and a timer overflow
 *
 *          The first instantiation of TimerA for the MSP430F5438 has 5
 *          capture compare registers. The first one, TA0CCR0 has a dedicated
 *          interrupt vector and the highest priority. This one is used for the
 *          RTOS tick.
 *
 *          @see page 6 of msp430f5438.pdf
 *          @see page 471 of slau208n.pdf
 *
 * \param void
 * \return void
 ******************************************************************************/
void __attribute__ ( ( interrupt(TIMER0_A1_VECTOR) ) ) hal_timer_a0_isr( void )
{
    switch (__even_in_range(TA0IV,8)) {
        case 2:
            hal_timer_a0_unregister(TIMER_A0_SLOT1);
            timer_a0_callback_1();
            break;
        case 4:
            hal_timer_a0_unregister(TIMER_A0_SLOT2);
            timer_a0_callback_2();
            break;
        case 6:
            hal_timer_a0_unregister(TIMER_A0_SLOT3);
            timer_a0_callback_3();
            break;
        case 8:
            hal_timer_a0_unregister(TIMER_A0_SLOT4);
            timer_a0_callback_4();
            break;
        default:
            break;
    }
}
