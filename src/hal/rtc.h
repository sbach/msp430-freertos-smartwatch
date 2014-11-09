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

#ifdef __MSP430F5438__
extern int ti_hal_rtc_a_set_year_fix(int year);
extern int ti_hal_rtc_a_set_month_fix(int month);
extern int ti_hal_rtc_a_set_day_fix(int day);
extern int ti_hal_rtc_a_set_dayofweek_fix(int dayofweek);
extern int ti_hal_rtc_a_set_hour_fix(int hour);
extern int ti_hal_rtc_a_set_minute_fix(int minute);
extern int ti_hal_rtc_a_set_second_fix(int second);
#endif

void hal_rtc_a_init( void );
void hal_rtc_a_set_datetime( rtcdatetime_t* data );
void hal_rtc_a_get_datetime( rtcdatetime_t* data );
void hal_rtc_a_set_ps0_interrupt(int enable);
void __attribute__ ( ( interrupt(RTC_VECTOR) ) ) hal_rtc_a_isr( void );

#endif /* HAL_RTC_H */
