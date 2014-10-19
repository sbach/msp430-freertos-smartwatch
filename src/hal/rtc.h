#ifndef HAL_RTC_H
#define HAL_RTC_H


/*******************************************************************************
 * Macros
 ******************************************************************************/

#define RTC_NO_INTERRUPT        ( 0 )
#define RTC_READY_IFG           ( 2 )
#define RTC_EVENT_IFG           ( 4 )
#define RTC_ALARM_IFG           ( 6 )
#define RTC_PRESCALE_ZERO_IFG   ( 8 )
#define RTC_PRESCALE_ONE_IFG    ( 10 )

#define RTCCAL_VALUE_MASK       ( 0x3F )


/*******************************************************************************
 * Structures
 ******************************************************************************/

typedef struct
{
  unsigned int year;
  unsigned char month;
  unsigned char day;
  unsigned char dayofweek;
  unsigned char hour;
  unsigned char minute;
  unsigned char second;
} rtcdatetime_t;


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void hal_rtc_a_init( void );
void hal_rtc_a_set_datetime( rtcdatetime_t* data );
void hal_rtc_a_get_datetime( rtcdatetime_t* data );
void hal_rtc_a_set_ps0_interrupt(int enable);
void __attribute__ ( ( interrupt(RTC_VECTOR) ) ) hal_rtc_a_isr( void );

#endif /* HAL_RTC_H */
