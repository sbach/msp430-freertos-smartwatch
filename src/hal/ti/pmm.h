/*******************************************************************************
 *
 * Power Management Module Library for MSP430F5xx/6xx family
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

#ifndef HAL_PMM_H
#define HAL_PMM_H


/*******************************************************************************
 * Macros
 ******************************************************************************/

// Supply Voltage Supervisor and Monitor
// @see page of 98 slau208n.pdf

// Clear the interrupt flags of the PMM
#define CLEAR_PMM_IFGS()    PMMCTL0_H = 0xA5; PMMIFG = 0; PMMCTL0_H = 0x00

/*
 * Enable / Disable
 */

// Low-side supervisor
#define ENABLE_SVSL()       PMMCTL0_H = 0xA5; SVSMLCTL |= SVSLE;  \
                                PMMCTL0_H = 0x00
#define DISABLE_SVSL()      PMMCTL0_H = 0xA5; SVSMLCTL &= ~SVSLE; \
                                PMMCTL0_H = 0x00

// Low-side monitor
#define ENABLE_SVML()       PMMCTL0_H = 0xA5; SVSMLCTL |= SVMLE;  \
                                PMMCTL0_H = 0x00
#define DISABLE_SVML()      PMMCTL0_H = 0xA5; SVSMLCTL &= ~SVMLE; \
                                PMMCTL0_H = 0x00

// High-side supervisor
#define ENABLE_SVSH()       PMMCTL0_H = 0xA5; SVSMHCTL |= SVSHE;  \
                                PMMCTL0_H = 0x00
#define DISABLE_SVSH()      PMMCTL0_H = 0xA5; SVSMHCTL &= ~SVSHE; \
                                PMMCTL0_H = 0x00

// High-side monitor
#define ENABLE_SVMH()       PMMCTL0_H = 0xA5; SVSMHCTL |= SVMHE;  PMMCTL0_H = 0x00
#define DISABLE_SVMH()      PMMCTL0_H = 0xA5; SVSMHCTL &= ~SVMHE; PMMCTL0_H = 0x00


// Low-side supervisor and monitor
#define ENABLE_SVSL_SVML()  PMMCTL0_H = 0xA5; SVSMLCTL |= (SVSLE + SVMLE);  \
                                PMMCTL0_H = 0x00
#define DISABLE_SVSL_SVML() PMMCTL0_H = 0xA5; SVSMLCTL &= ~(SVSLE + SVMLE); \
                                PMMCTL0_H = 0x00

// High-side supervisor and monitor
#define ENABLE_SVSH_SVMH()  PMMCTL0_H = 0xA5; SVSMHCTL |= (SVSHE + SVMHE);  \
                                PMMCTL0_H = 0x00
#define DISABLE_SVSH_SVMH() PMMCTL0_H = 0xA5; SVSMHCTL &= ~(SVSHE + SVMHE); \
                                PMMCTL0_H = 0x00

/*
 * Enable / Disable, Reset & Interrupt
 */

#define ENABLE_SVSL_RESET()         PMMCTL0_H = 0xA5; PMMRIE |= SVSLPE;  \
                                        PMMCTL0_H = 0x00
#define DISABLE_SVSL_RESET()        PMMCTL0_H = 0xA5; PMMRIE &= ~SVSLPE; \
                                        PMMCTL0_H = 0x00
#define ENABLE_SVML_INTERRUPT()     PMMCTL0_H = 0xA5; PMMRIE |= SVMLIE;  \
                                        PMMCTL0_H = 0x00
#define DISABLE_SVML_INTERRUPT()    PMMCTL0_H = 0xA5; PMMRIE &= ~SVMLIE; \
                                        PMMCTL0_H = 0x00
#define ENABLE_SVSH_RESET()         PMMCTL0_H = 0xA5; PMMRIE |= SVSHPE;  \
                                        PMMCTL0_H = 0x00
#define DISABLE_SVSH_RESET()        PMMCTL0_H = 0xA5; PMMRIE &= ~SVSHPE; \
                                        PMMCTL0_H = 0x00
#define ENABLE_SVMH_INTERRUPT()     PMMCTL0_H = 0xA5; PMMRIE |= SVMHIE;  \
                                        PMMCTL0_H = 0x00
#define DISABLE_SVMH_INTERRUPT()    PMMCTL0_H = 0xA5; PMMRIE &= ~SVMHIE; \
                                        PMMCTL0_H = 0x00

/*
 * Enable / Disable in Low Power Mode
 */

// These settings use SVSH/LACE = 0
#define SVSL_ENABLED_IN_LPM_FAST_WAKE()     PMMCTL0_H = 0xA5; \
                                                SVSMLCTL |= (SVSLFP+SVSLMD); \
                                                SVSMLCTL &= ~SVSMLACE; PMMCTL0_H = 0x00
#define SVSL_ENABLED_IN_LPM_SLOW_WAKE()     PMMCTL0_H = 0xA5; SVSMLCTL |= SVSLMD; \
                                                SVSMLCTL &= ~(SVSLFP+SVSMLACE); \
                                                PMMCTL0_H = 0x00

#define SVSL_DISABLED_IN_LPM_FAST_WAKE()    PMMCTL0_H = 0xA5; SVSMLCTL |= SVSLFP; \
                                                SVSMLCTL &= ~(SVSLMD+SVSMLACE); \
                                                PMMCTL0_H = 0x00
#define SVSL_DISABLED_IN_LPM_SLOW_WAKE()    PMMCTL0_H = 0xA5; \
                                                SVSMLCTL &= ~(SVSLFP+SVSMLACE+SVSLMD); \
                                                PMMCTL0_H = 0x00

#define SVSH_ENABLED_IN_LPM_NORM_PERF()     PMMCTL0_H = 0xA5; SVSMHCTL |= SVSHMD; \
                                                SVSMHCTL &= ~(SVSMHACE+SVSHFP); \
                                                PMMCTL0_H = 0x00
#define SVSH_ENABLED_IN_LPM_FULL_PERF()     PMMCTL0_H = 0xA5; \
                                                SVSMHCTL |= (SVSHMD+SVSHFP); \
                                                SVSMHCTL &= ~SVSMHACE; PMMCTL0_H = 0x00

#define SVSH_DISABLED_IN_LPM_NORM_PERF()    PMMCTL0_H = 0xA5; \
                                                SVSMHCTL &= ~(SVSMHACE+SVSHFP+SVSHMD); \
                                                PMMCTL0_H = 0x00
#define SVSH_DISABLED_IN_LPM_FULL_PERF()    PMMCTL0_H = 0xA5; SVSMHCTL |= SVSHFP; \
                                                SVSMHCTL &= ~(SVSMHACE+SVSHMD); \
                                                PMMCTL0_H = 0x00

// These setting use SVSH/LACE = 1
#define SVSL_OPTIMIZED_IN_LPM_FAST_WAKE()   PMMCTL0_H = 0xA5; \
                                                SVSMLCTL |= (SVSLFP+SVSLMD+SVSMLACE); \
                                                PMMCTL0_H = 0x00
#define SVSH_OPTIMIZED_IN_LPM_FULL_PERF()   PMMCTL0_H = 0xA5; \
                                                SVSMHCTL |= (SVSHMD+SVSHFP+SVSMHACE); \
                                                PMMCTL0_H = 0x00


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

unsigned int SetVCore(unsigned char level);

#endif /* HAL_PMM_H */
