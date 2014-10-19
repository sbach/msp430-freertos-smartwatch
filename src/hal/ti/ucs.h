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

#ifndef HAL_UCS_H
#define HAL_UCS_H


/*******************************************************************************
 * Macros
 ******************************************************************************/

// Select source for FLLREF
#define SELECT_FLLREF(source) UCSCTL3 = (UCSCTL3 & ~(SELREF_7)) | (source)
// Select source for ACLK
#define SELECT_ACLK(source)   UCSCTL4 = (UCSCTL4 & ~(SELA_7)) | (source)
// Select source for MCLK
#define SELECT_MCLK(source)   UCSCTL4 = (UCSCTL4 & ~(SELM_7)) | (source)
// Select source for SMCLK
#define SELECT_SMCLK(source)  UCSCTL4 = (UCSCTL4 & ~(SELS_7)) | (source)
// Select source for MCLK and SMCLK
#define SELECT_MCLK_SMCLK(sources) UCSCTL4 = (UCSCTL4 & ~(SELM_7 + SELS_7)) | (sources)

// Select ACLK divider
#define ACLK_DIV(x)         UCSCTL5 = (UCSCTL5 & ~(DIVA_7)) | (DIVA__##x)
// Select MCLK divider
#define MCLK_DIV(x)         UCSCTL5 = (UCSCTL5 & ~(DIVM_7)) | (DIVM__##x)
// Select SMCLK divider
#define SMCLK_DIV(x)        UCSCTL5 = (UCSCTL5 & ~(DIVS_7)) | (DIVS__##x)
// Select FLLREF divider
#define SELECT_FLLREFDIV(x) UCSCTL3 = (UCSCTL3 & ~(FLLREFDIV_7))|(FLLREFDIV__##x)

// Output the clocks on P11.0, P11.1, and P11.2 pins
#define OUTPUT_ACLK_MCLK_SMCLK() P11DIR |= 0x07; P11SEL |= 0x07


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

unsigned int LFXT_Start_Timeout(unsigned int xtdrive, unsigned int timeout);
unsigned int SetXCAP(unsigned int xcap);
void Init_FLL_Settle(unsigned int fsystem, unsigned int ratio);
void Init_FLL(unsigned int fsystem, unsigned int ratio);

#endif /* HAL_UCS_H */
