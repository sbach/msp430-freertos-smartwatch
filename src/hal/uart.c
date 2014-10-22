#include <msp430.h>
#include "uart.h"


/*******************************************************************************
 * \brief   Setup UCA3 UART interface
 *
 * \param void
 * \return void
 ******************************************************************************/
void halDebugUARTInit( void )
{
    // Set P10.4 and P10.5 to UCA function
    P10SEL |= BIT4;
    P10SEL |= BIT5;

    // Put the USCI state machine in reset
    UCA3CTL1 = UCSWRST;

    // Select the clock source
    UCA3CTL1 |= UCSSEL__ACLK;


    // Configure the baud rate
    // @see page 905, 906 of slau208n.pdf

    // CLK          baud    BRx     BRSx    BRFx
    // -----------------------------------------
    // 32768 Hz     9600    3       0       3

    // Prescaler value UCBRx (16-bit)
    // @see page 913 of slau208n.pdf
    // BRx = (BR0 + BR1 × 256)
    UCA3BR0 = 0x03;
    UCA3BR1 = 0x00;

    // First and second modulation stage selection
    // These bits determine the modulation pattern.
    // @see page 913 of slau208n.pdf
    UCA3MCTL = UCBRS_3 + UCBRF_0;


    // Reset UCAxSTAT register to clear error flags
    UCA3STAT = 0;

    // Initialize USCI state machine
    UCA3CTL1 &= ~UCSWRST;

    // Enable USCI_A0 RX interrupt
    UCA3IE = UCRXIE;

    // Clear interrupt flags
    UCA3IFG = 0;
}

/*******************************************************************************
 * \brief   Write a string to the UART TX buffer
 *
 * \param const char *  Text to write
 * \return void
 ******************************************************************************/
void halDebugUARTWrite(const char *buf)
{
    // Send each character to the TX buffer
    while (*buf) WRITE_DEBUG_UART(*buf++);

    // Wait until transmit is done
    while (UCA3STAT & UCBUSY);
}

/*******************************************************************************
 * \brief   ISR to handle events on the USCI_A3 pins.
 *
 * \param void
 * \return void
 ******************************************************************************/
void __attribute__ ( ( interrupt(USCI_A3_VECTOR) ) ) halDebugUARTISR( void )
{
    switch (__even_in_range(UCA3IV,4)) {
        case UART_NO_INTERRUPT:
            break;
        case UART_RX_IFG:
            UCA3IFG  &= ~UCRXIFG;
            UCA3TXBUF = UCA3RXBUF;
            break;
        case UART_TX_IFG:
            break;
        default:
            break;
    }
}
