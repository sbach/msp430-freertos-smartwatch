/*******************************************************************************
 *
 * Provides Functions to Initialize the UCS/FLL and clock sources
 *
 *
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

#include "msp430.h"
#include "ucs.h"


/*******************************************************************************
 * \brief   Initializes the XT1 Oscillator of the UCS in LF mode (default)
 *
 * \param xtdrive  drive setting (XT1DRIVE_0 = lowest current consumption)
 * \param timeout  how long to check for the oscillator to stabilize
 ******************************************************************************/
unsigned int LFXT_Start_Timeout(unsigned int xtdrive, unsigned int timeout) {

    // Maximum drive capability and maximum current consumption for XT1 LF mode
    // Start with the highest supply current for reliable and quick startup.
    // @see slau208n.pdf, page 178
    UCSCTL6_L |= XT1DRIVE1_L + XT1DRIVE0_L;

    // Loop until the XT1 oscillator stabilizes.
    // Check, at least once, for an oscillator fault.
    do {
        // Clear oscillator fault flags
        // @see slau208n.pdf, page 180
        UCSCTL7 &= ~(XT2OFFG +      // XT2 flag
                     XT1LFOFFG +    // XT1 flag in LF mode
                     XT1HFOFFG +    // XT1 flag in HF mode
                     DCOFFG);       // DCO flag

        // Clear the oscillator-fault interrupt flag
        // @see slay208n.pdf, page 82
        SFRIFG1 &= ~OFIFG;

    } while (SFRIFG1 & OFIFG && timeout--);

    // Set the requested DRIVE_x mode
    UCSCTL6 = (UCSCTL6 & ~(XT1DRIVE_3)) | (xtdrive);

    return timeout;
}

/*******************************************************************************
 * \brief   Select the internal capacitor of the XIN/XOUT pins
 *
 * \param xcap  requested capacitor XCAPx, 2 pF, 6 pF, 9 pF, or 12 pF (default)
 ******************************************************************************/
unsigned int SetXCAP(unsigned int xcap) {

    // Convert the number to the correct XCAPx value,
    // and prevent value overflow with a mask.
    // We can only choose from XCAPx=0 to XCAPx=3 (default)
    xcap = (xcap << 2) & (XCAP0 + XCAP1);

    // Select the new capacitor XCAPx
    UCSCTL6 = (UCSCTL6 & ~(XCAP0 + XCAP1)) | (xcap);

    return 1;
}

/*******************************************************************************
 * \brief   Configure the Frequency Locked Loop and the Digitally-Controlled
 *          Oscillator to set the MCLK and SMCLK frequencies source and wait
 *          for the DCO to settle.
 *
 * \param fsystem   the requested system frequency (for MCLK)
 * \param ratio     the multiplier to get the requested frequency from the
 *                  reference clock frequency (XT1 by default)
 ******************************************************************************/
void Init_FLL_Settle(unsigned int fsystem, unsigned int ratio)
{
    volatile unsigned int x = ratio * 32;

    // Save actual state of FLL loop control
    unsigned int globalInterruptState = __get_SR_register() & SCG0;

    __bic_SR_register(SCG0);        // Enable FLL loop control

    Init_FLL(fsystem, ratio);       // Configure the FLL

    // Wait for a few cycles for the DCO to settle on the proper tap for
    // normal operation. Worst case is (n * 32 * 32) f_FLLREFCLK cycles.
    while(x--) {
        __delay_cycles(32);
    }

    // Restore previous state
    __bis_SR_register(globalInterruptState);

}

/*******************************************************************************
 * \brief   Configure the Frequency Locked Loop and the Digitally-Controlled
 *          Oscillator to set the MCLK and SMCLK frequencies source
 *
 * \param fsystem   the requested system frequency (for MCLK)
 * \param ratio     the multiplier to get the requested frequency from the
 *                  reference clock frequency (XT1 by default)
 ******************************************************************************/
void Init_FLL(unsigned int fsystem, unsigned int ratio)
{
    unsigned int mul_div, dco_div;
    unsigned int mode = 0;      // Will use DCOCLKDIV (default)

    // About the following operations on mul_div, dco_div, and fsystem:
    // @see http://e2e.ti.com/support/microcontrollers/msp430/f/166/t/123555.aspx

    mul_div = ratio;
    dco_div = FLLD__2;              // Have at least a divider of 2

    if (fsystem > 16000){
        mul_div >>= 1;
        mode = 1;                   // Will use DCOCLK as reference
    } else {
        fsystem <<= 1;
    }

    // Modify FLLN and FLLD to cap FLLN at a value of 511
    while (mul_div > 512) {
        dco_div = dco_div + FLLD0;  // set next higher div level
        mul_div >>= 1;
    }

    // Set the DCO tap and the MOD bit counter to the lowest possible value.
    // They will be automatically modified during FLL operation.
    UCSCTL0 = 0x0;

    // Reset the FLLN field, aka. Multiplier bits.
    // These bits set the multiplier value N of the DCO. N must be
    // greater than 0. Here, writing zero to FLLN causes N to be set to 1
    UCSCTL2 &= ~(0x03FF);

    // Set FLLN and FLLD, the multiplier and divider bits
    // We can compute the output frequencies with:
    //   f_DCOCLK = D × (N + 1) × (f_FLLREFCLK / n)
    //   f_DCOCLKDIV = (N + 1) × (f_FLLREFCLK / n)
    UCSCTL2 = dco_div | (mul_div - 1);

    // Select a suitable range for the FLL to adjust the
    // DCO output frequeny.
    if (fsystem <= 700)             // 0.20MHz          0.70MHz
        UCSCTL1 = DCORSEL_0;
    else if (fsystem <  1470)       // 0.36MHz          1.47MHz
        UCSCTL1 = DCORSEL_1;
    else if (fsystem <  3170)       // 0.75MHz          3.17MHz
        UCSCTL1 = DCORSEL_2;
    else if (fsystem <  6070)       // 1.51MHz          6.07MHz
        UCSCTL1 = DCORSEL_3;
    else if (fsystem <  12300)      // 3.20MHz          12.3MHz
        UCSCTL1 = DCORSEL_4;
    else if (fsystem <  23700)      // 6.00MHz          23.7MHz
        UCSCTL1 = DCORSEL_5;
    else if (fsystem <  39000)      // 10.7MHz          39.0MHz
        UCSCTL1 = DCORSEL_6;
    else                            // 19.6MHz          60MHz
        UCSCTL1 = DCORSEL_7;
                                    // f_DCO(n,0)_MAX   f_DCO(n, 31)_MIN

    // Loop until the XT1 oscillator & DCO stabilizes.
    // Check, at least once, for an oscillator fault.
    do
    {
        // Clear oscillator fault flags
        // @see page 115, 129 of slau259e.pdf
        UCSCTL7 &= ~(XT2OFFG +      // XT2 flag
                     XT1LFOFFG +    // XT1 flag in LF mode
                     XT1HFOFFG +    // XT1 flag in HF mode
                     DCOFFG);       // DCO flag

        // Clear the oscillator-fault interrupt flag
        // @see page 67, 115 of slau259e.pdf
        SFRIFG1 &= ~OFIFG;
    }
    while ((SFRIFG1 & OFIFG));  // Check OFIFG bit

    // Select the reference clock for MCLK and SMCLK
    if (mode == 1)
        SELECT_MCLK_SMCLK(SELM__DCOCLK + SELS__DCOCLK);       // Select DCOCLK
    else
        SELECT_MCLK_SMCLK(SELM__DCOCLKDIV + SELS__DCOCLKDIV); // Select DCODIVCLK

}
