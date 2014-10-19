#include "FreeRTOS.h"

#include "msp430.h"
#include "rtc.h"


void hal_rtc_a_init( void )
{
    // Stop the RTC_A
    // Set RTCHOLD = 1 before writing into any of the calendar or prescalar registers
    // @see slau208n.pdf, page 622
    RTCCTL01 = RTCHOLD;

    // 6 bit RTCCALx register's value
    // Each LSB represents approximately +4-ppm (RTCCALS = 1) or
    // a –2-ppm (RTCCALS = 0) adjustment in frequency.
    signed char calibration_value = 0x0;

    if (calibration_value < 0)
    {
        // We only store absolute values in the register
        calibration_value = -calibration_value;

        RTCCTL2 = calibration_value & RTCCAL_VALUE_MASK;
    }
    else
    {
        RTCCTL2 = calibration_value & RTCCAL_VALUE_MASK;

        // Frenquency adjusted up (+), default is down (-)
        // @see slau208n.pdf, page 603
        RTCCTL2 |= RTCCALS;
    }

    // Use the calendar mode
    // @see slau208n.pdf, page 566, 640
    RTCCTL01 |= RTCMODE;

    // Configure the prescale dividers, RT0PS and RT1PS, to provide a 1-second clock
    // RT0PS is sourced from the low-frequency oscillator XT1. The output of RT0PS divided
    // by 256 (Q7) sources RT1PS. RT1PS is further divided by 128 to source the real-time
    // clock counter registers that provide the required 1-second time interval.
    // @see slau208n.pdf, page 621, 622

    // Configure RT0PS
    // Exemple: XT1 = 32768Hz; RTI0P_7 (/256); we have a 128Hz rate
    // @see slau208n.pdf, page 653
    RTCPS0CTL |= RT0IP_7;

    // Configure RT1PS and enable interrupt
    // Exemple: RT0PS = 128Hz; RT1IP_6 (/128); we have a 1Hz rate
    // @see slau208n.pdf, page 654
    RTCPS1CTL |= RT1IP_6 | RT1PSIE;

    // Real-time clock calibration frequency for measurements
    // (Only available in calendar mode)
    // Exemple: RTCCALF_3 = 1Hz
    // @see slau208n.pdf, page 641
    RTCCTL23 |= RTCCALF_3;

    // Enable the RTC
    RTCCTL01 &= ~RTCHOLD;

    // Configure P2.4 to output RTCCLK
    // We can use this pin to calibrate the RTC clock
    // @see slau208n.pdf, page 626, (570, 596)
    P2SEL |= (BIT4);
    P2DIR |= (BIT4);
}

void hal_rtc_a_set_datetime( rtcdatetime_t* data )
{
    // Stop the RTC_A
    RTCCTL01 |= RTCHOLD;

    // Write the date/time in the registers
    RTCYEAR = data->year;
    RTCMON  = data->month;
    RTCDAY  = data->day;
    RTCDOW  = data->dayofweek;
    RTCHOUR = data->hour;
    RTCMIN  = data->minute;
    RTCSEC  = data->second;

    // Enable the RTC
    RTCCTL01 &= ~RTCHOLD;
}

void hal_rtc_a_get_datetime( rtcdatetime_t* data )
{
    // Read the date/time registers
    data->year      = RTCYEAR;
    data->month     = RTCMON;
    data->day       = RTCDAY;
    data->dayofweek = RTCDOW;
    data->hour      = RTCHOUR;
    data->minute    = RTCMIN;
    data->second    = RTCSEC;
}

void hal_rtc_a_set_ps0_interrupt( int enable )
{
    portENTER_CRITICAL();
    if (enable)
        RTCPS0CTL |= RT0PSIE;
    else
        RTCPS0CTL &= ~RT0PSIE;
    portEXIT_CRITICAL();
}

void __attribute__ ( ( interrupt(RTC_VECTOR) ) ) hal_rtc_a_isr( void )
{
    // Handle the different causes of the RTC interrupt
    // The source of the interrupt is described by the RTCIV register.
    // We check that it's value is even and in the 0-10 range.
    // @see slau208n.pdf, page 657
    switch(__even_in_range(RTCIV, 0xa))
    {
        case RTC_NO_INTERRUPT:
            break;
        case RTC_READY_IFG:
            break;
        case RTC_EVENT_IFG:
            break;
        case RTC_ALARM_IFG:
            break;
        case RTC_PRESCALE_ZERO_IFG:
            break;
        case RTC_PRESCALE_ONE_IFG:
            break;
        default:
            break;
    }
}
