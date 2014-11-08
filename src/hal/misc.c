#include "FreeRTOS.h"

#include "msp430.h"
#include "misc.h"
#include "ti/pmm.h"
#include "ti/ucs.h"


/*******************************************************************************
 * \brief   Setup system clocks and basic power managment
 *
 * \param void
 * \return void
 ******************************************************************************/
void hal_setup_clock_pmm(void) {

    // Set the only supported PMMCOREVx (for MSP430F5438)
    // The system frequency can operate in the 0-18MHz range
    // @see slau208n.pdf, page 135
    // @see slas612e.pdf, page 15 (device specific)
    ti_hal_set_vcore(PMMCOREV_2);

    // Setup XT1 pins, P7.0 (XIN) and P7.1 (XOUT)
    P7SEL |= BIT0 + BIT1;

    // Select the internal capacitor for XT1
    ti_hal_set_xcap(3);

    // Start XT1, 32kHz crystal, in LF mode
    while(!ti_hal_lfxt_start_timeout(XT1DRIVE_0, 50000));

    // Set the reference clocks for the auxiliray clock
    SELECT_ACLK(SELA__XT1CLK);

    // Set XT1 as the reference clock for FLL (enabled by default.)
    // @note: The Frequency Locked Loop is used to stabilize the
    //        Digitally-Controlled Oscillator.
    SELECT_FLLREF(SELREF__XT1CLK);

    unsigned long ratio = CONFIG_CPU_CLOCK_HZ / CONFIG_XT1_CLOCK_HZ;

    // Convert a Hz value to a KHz value, as required
    //  by the Init_FLL_Settle() function.
    unsigned long cpu_clock_khz = CONFIG_CPU_CLOCK_HZ / 1000UL;

    //Make sure we aren't overclocking
    if(cpu_clock_khz > CONFIG_CPU_CLOCK_LIMIT_KHZ) {
        cpu_clock_khz = CONFIG_CPU_CLOCK_LIMIT_KHZ;
    }

    // Init the FLL and DCO with the requested frequency (in KHz)
    // Also set the reference clocks for the master and sub-system master clock
    ti_hal_init_fll_settle((unsigned short) cpu_clock_khz, ratio);

    // Set SCG0 bits in the status register.
    // Disable the FLL control loop
    __bis_SR_register(SCG0);

    // Setup for quick wake up from interrupt and
    // minimal power consumption in sleep mode
    // @see page 107 of slau208n.pdf
    DISABLE_SVSL();                           // SVS Low side is turned off
    DISABLE_SVSL_RESET();
    DISABLE_SVML();                           // Monitor low side is turned off
    DISABLE_SVML_INTERRUPT();
    DISABLE_SVMH();                           // Monitor high side is turned off
    DISABLE_SVMH_INTERRUPT();
    ENABLE_SVSH();                            // SVS High side is turned on
    ENABLE_SVSH_RESET();                      // Enable POR on SVS Event
    SVSH_ENABLED_IN_LPM_FULL_PERF();          // SVS high side Full perf mode,
                                              // stays on in LPM3,enhanced protect

    // Wait for the interrupt flags to be set
    // If SVS_L and SVM_L power modes are modified, or if a voltage level is modified,
    // a delay element masks the interrupts and POR sources until the SVS L and SVM L circuits
    // have settled. When SVSMLDLYST (delay status) reads zero, the delay has expired.
    // In addition, the SVSMLDLYIFG (SVS L /SVM L delay expired) interrupt flag is set.
    // @see page 104 of slau208n.pdf
    while ((PMMIFG & SVSMLDLYIFG) == 0 && (PMMIFG & SVSMHDLYIFG) == 0);

    // Clear those flags
    CLEAR_PMM_IFGS();
}
