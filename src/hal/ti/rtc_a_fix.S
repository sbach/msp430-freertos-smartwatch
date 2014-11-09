;******************************************************************************
;  MSP430F5438 - Real Time Clock Workaround
;
;  Description: This program demonstrates the RTC Workaround for the RTC
;  Errata deailed in the XMS '5438 Errata under RTC3
;  Assembly functions for the RTC workaround
;  Allows safe access to the RTC registers that potentially have a write issue
;  Code is aligned so that the lower 5 bits of the address of the instruction
;  fetch following the RTC register write are equal
;
;  Texas Instruments Inc.
;  October 2008
;
;*******************************************************************************
;  Do not change the code in the ASM file, all nops in there are needed.
;*******************************************************************************

    #include "msp430.h"
    #include "data_model.h"

    .global ti_hal_rtc_a_set_year_fix
    .global	ti_hal_rtc_a_set_month_fix
    .global	ti_hal_rtc_a_set_day_fix
    .global	ti_hal_rtc_a_set_dayofweek_fix

    .global	ti_hal_rtc_a_set_hour_fix
    .global	ti_hal_rtc_a_set_minute_fix
    .global	ti_hal_rtc_a_set_second_fix

    .text
    .align 2

;-------------------------------------------------------------------------------
; int ti_hal_rtc_a_set_year_fix(int year)
; year will be set in the RTCYEAR register
; return value is read back from the RTCYEAR register
; Works only in calendar mode (RTCMODE = 1)
; argument year is in R12
; return value is in R12
;-------------------------------------------------------------------------------
ti_hal_rtc_a_set_year_fix:

    nop
    nop
    nop
    nop
    dint
    nop
    nop
    nop
    mov.w   R12,&RTCYEAR            ; Set RTCYEAR to year
    nop
    nop
    mov.w   &RTCYEAR,R12            ; Return value
    eint
    ret_x

    .align 2

;-------------------------------------------------------------------------------
; int ti_hal_rtc_a_set_month_fix(int month)
; month will be set in the RTCMON register
; Workaround requires to write 16 bits into RTCDATE
; return value is read back from the RTCMON register
; argument month is in R12
; return value is in R12
;-------------------------------------------------------------------------------
ti_hal_rtc_a_set_month_fix:

    push.w  R5                      ; push R5
    mov.b   &RTCDAY,R5              ; read RTCDAY into R5
    swpb    R12                     ; R12 holds month - swap lower and upper byte
    bis.w   R12,R5                  ; combine read RTCDAY and new RTCMON
    dint
    nop
    mov.w   R5,&RTCDATE             ; write to RTCDATE
    nop
    nop
    nop
    nop
    eint
    mov.b   &RTCMON,R12             ; Read RTCMON register
    pop.w   R5                      ; pop R5
    ret_x

    .align 2

;-------------------------------------------------------------------------------
; int ti_hal_rtc_a_set_day_fix(int day)
; day will be set in the RTCDAY register
; Workaround requires to write 16 bits into RTCDATE
; return value is read back from the RTCDAY register
; argument day is in R12
; return value is in R12
;-------------------------------------------------------------------------------
ti_hal_rtc_a_set_day_fix:                          ; R12 holds day in lower byte

    push.w  R5                      ; push R5
    mov.b   &RTCMON,R5              ; read RTCMON into lower byte of R5
    swpb    R5                      ; month is now in higher byte of R5
    bic.w   #00FFh,R5               ; clear lower byte of R5
    bis.w   R12,R5                  ; combine read RTCDAY and new RTCMON
    dint
    nop
    mov.w   R5,&RTCDATE             ; write to RTCDATE
    nop
    eint
    mov.b   &RTCDAY,R12             ; Read RTCDAY register
    pop.w   R5                      ; pop R5
    ret_x

    .align 2

;-------------------------------------------------------------------------------
; int ti_hal_rtc_a_set_dayofweek_fix(int dow)
; dow will be set in the RTCDOW register
; Workaround requires to write 16 bits into RTCTIM1
; return value is read back from the RTCDOW register
; argument dow is in R12
; return value is in R12
;-------------------------------------------------------------------------------
ti_hal_rtc_a_set_dayofweek_fix:

    push.w  R5                      ; push R5
    mov.b   &RTCHOUR,R5             ; read RTCHOUR into R5
    swpb    R12                     ; R12 holds dow - swap lower and upper byte
    bis.w   R12, R5                 ; combine read RTCHOUR and new RTCDOW
    dint
    nop
    mov.w   R5,&RTCTIM1             ; write to RTCTIM1
    nop
    eint
    mov.b   &RTCDOW,R12             ; Read RTCDOW register
    pop.w   R5                      ; pop R5
    ret_x

    .align 2

;-------------------------------------------------------------------------------
; int ti_hal_rtc_a_set_hour_fix(int hour)
; hour will be set in the RTCHOUR register
; Workaround requires to write 16 bits into RTCTIM1
; return value is read back from the RTCHOUR register
; argument hour is in R12
; return value is in R12
;-------------------------------------------------------------------------------
ti_hal_rtc_a_set_hour_fix:                         ; R12 holds day in lower byte

    push.w  R5                      ; push R5
    mov.b   &RTCDOW,R5              ; read RTCDOW into lower byte of R5
    swpb    R5                      ; dow is now in higher byte of R5
    bis.w   R12,R5                  ; combine read RTCDOW and new RTCHOUR
    dint
    nop
    nop
    mov.w   R5,&RTCTIM1             ; write to RTCTIM1
    nop
    eint
    mov.b   &RTCHOUR,R12            ; Read RTCHOUR register
    pop.w   R5                      ; pop R5
    ret_x

    .align 2

;-------------------------------------------------------------------------------
; int ti_hal_rtc_a_set_minute_fix(int min)
; min will be set in the RTCMIN register
; Workaround requires to write 16 bits into RTCTIM0
; return value is read back from the RTCDOW register
; argument min is in R12
; return value is in R12
;-------------------------------------------------------------------------------
ti_hal_rtc_a_set_minute_fix:

    push.w  R5                      ; push R5
    mov.b   &RTCSEC, R5             ; read RTCSEC into R5
    swpb    R12                     ; R12 holds min - swap lower and upper byte
    bis.w   R12,R5                  ; combine read RTCMIN and new RTCSEC
    dint
    nop
    mov.w   R5,&RTCTIM0             ; write to RTCTIM0
    nop
    eint
    mov.b   &RTCMIN,R12             ; Read RTCDOW register
    pop.w   R5                      ; pop R5
    ret_x

    .align 2

;-------------------------------------------------------------------------------
; int ti_hal_rtc_a_set_second_fix(int sec)
; sec will be set in the RTCSEC register
; Workaround requires to write 16 bits into RTCTIM0
; return value is read back from the RTCSEC register
; argument sec is in R12
; return value is in R12
;-------------------------------------------------------------------------------
ti_hal_rtc_a_set_second_fix:                          ; R12 holds day in lower byte

    push.w  R5                      ; push R5
    mov.b   &RTCMIN, R5             ; read RTCMIN into lower byte of R5
    swpb    R5                      ; min is now in higher byte of R5
    bis.w   R12, R5                 ; combine read RTCMIN and new RTCSEC
    dint
    nop
    nop
    mov.w   R5,&RTCTIM0             ; write to RTCTIM0
    nop
    eint
    mov.b   &RTCSEC, R12            ; Read RTCSEC register
    pop.w   R5                      ; pop R5
    ret_x
